/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

// Project includes
#include "NotificationView.h"
#include "Utilities.h"

// OS includes
#include <GridLayout.h>
#include <GroupLayout.h>
#include <InterfaceDefs.h>
#include <LayoutItem.h>
#include <Looper.h>

/*---------------------------------------------------------------------------
 *						Implementation of class NotificationView
 *--------------------------------------------------------------------------*/




/*!	\brief		Constructor.
 */
NotificationView::NotificationView( BRect frame,
												const char* name,
												const char* label,
												ActivityData* toEdit,
												BMessage* message )
	:
	BControl( frame, 
				 name, 
				 label, 
				 message, 
				 B_FOLLOW_LEFT | B_FOLLOW_TOP, 
				 B_WILL_DRAW | B_FRAME_EVENTS | B_PULSE_NEEDED ),
	fData( toEdit ),
	fLastError( B_OK ),
	fCheckBox( NULL ),
	fOutline( NULL ),
	fLabel( NULL ),
	fTextView( NULL ),
	fScroller( NULL )
{
	BRect textViewRect;
	BGroupLayout* groupLayout = new BGroupLayout( B_VERTICAL );
	if ( !groupLayout ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	groupLayout->SetInsets( 5, 5, 5, 5 );
	this->SetLayout( groupLayout );
	
	// Creating the checkbox
	BMessage* toSend = new BMessage( kNotificationActivityCheckBoxToggled );
	if ( !toSend ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fCheckBox = new BCheckBox( BRect( 0, 0, 1, 1 ),
										"Notification Enabler",
										"Display notification",
										toSend );
	if ( !fCheckBox ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fCheckBox->ResizeToPreferred();
	
	// Check / uncheck the checkbox and load current text
	if ( fData ) {
		fCheckBox->SetValue( fData->GetNotification( &fNotificationText ) );
	}
	
	// Create the outline
	fOutline = new BBox( ( textViewRect = ( ( this->Bounds() ).InsetBySelf( 5, 5 ) ) ),
								"Notification outline" );
	if ( !fOutline ) {
		 /* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fOutline->SetLabel( fCheckBox );
	BLayoutItem* layoutItem = groupLayout->AddView( fOutline );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH,
																	 B_ALIGN_USE_FULL_HEIGHT ) );
	}
	
	// Build internal layout
	BGridLayout* gridLayout = new BGridLayout();
	if ( ! gridLayout ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	gridLayout->SetInsets( 10, ( fCheckBox->Bounds() ).Height(), 10, 10 );
	gridLayout->SetSpacing( 5, 2 );
	fOutline->SetLayout( gridLayout );
	
	// Create label
	fLabel = new BStringView( BRect( 0, 0, 1, 1 ),
									  "Notification Label",	
									  label );
	if ( !fLabel ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fLabel->ResizeToPreferred();

	// Place the label
	layoutItem = gridLayout->AddView( fLabel, 0, 1, 1, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT, B_ALIGN_TOP ) );
	}
	
	// Create the text view and the scroller
	textViewRect.right -= ( 10 + B_V_SCROLL_BAR_WIDTH );
	textViewRect.bottom -= ( 10 + B_H_SCROLL_BAR_HEIGHT );
	fTextView = new BTextView( textViewRect,
										"Notification Text",
										BRect( 0, 0, textViewRect.right - textViewRect.left, textViewRect.bottom - textViewRect.top ),
										B_FOLLOW_ALL_SIDES,
										B_NAVIGABLE | B_WILL_DRAW );
	if ( !fTextView ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fTextView->SetStylable( false );
	fTextView->ResizeToPreferred();
	
	fScroller = new BScrollView( "Notification Text Scroller",
										  fTextView,
										  B_FOLLOW_ALL_SIDES,
										  0,
										  false,
										  true );
	if ( !fScroller ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fTextView->SetText( fNotificationText.String() );
	
	layoutItem = gridLayout->AddView( fScroller, 1, 1, 1, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT,
																	 B_ALIGN_USE_FULL_HEIGHT ) );
	}
	if ( fCheckBox->Value() == 0 ) {
		fTextView->MakeEditable( false );
	}
	
}	// <-- end of constructor



/*!	\brief		Destructor
 */
NotificationView::~NotificationView()
{
	if ( fScroller ) {
		fScroller->RemoveSelf();
		delete fScroller;
	}
	if ( fOutline ) {
		fOutline->RemoveSelf();
		delete fOutline;
	}	
}	// <-- end of destructor



/*!	\brief		Update the label of the text.
 */
void		NotificationView::SetLabel( const char* toSet )
{
	BControl::SetLabel( toSet );
	if ( fLabel ) {
		fLabel->SetText( toSet );
	}
	InvalidateLayout();
	Invalidate();
}	// <-- end of function NotificationView::SetLabel



/*!	\brief		This function gets called when the view becomes a window's child.
 */
void		NotificationView::AttachedToWindow()
{
	if ( this->Looper() && this->Looper()->Lock() ) {
		this->Looper()->AddHandler( this );
		this->Looper()->Unlock();
	}
	BControl::AttachedToWindow();
	fCheckBox->SetTarget( this );
}	// <-- end of function NotificationView::AttachedToWindow


/*!	\brief		Update the ActionData upon every keypress
 */
void		NotificationView::Pulse( void )
{
	if ( fData ) {
		fData->SetNotification( ( fCheckBox->Value() != 0 ),
										( fNotificationText.SetTo( fTextView->Text() ) ) );
	}
	BControl::Pulse();
}	// <-- end of function NotificationView::Draw



/*!	\brief		Respond to the notification messages.
 */
void		NotificationView::MessageReceived( BMessage* in )
{
	BCheckBox* toCheck = NULL;
	
	switch( in->what )
	{
		case kNotificationActivityCheckBoxToggled:
			if ( in->FindPointer( "source", ( void** )&toCheck ) != B_OK )
			{
				toCheck = fCheckBox;
			}
		
			if ( toCheck != NULL ) {
				
				if ( this->fCheckBox->Value() == 0 ) {
					if ( this->fTextView ) {
						this->fTextView->MakeEditable( false );
					}
					if ( this->fData ) {
						this->fData->SetNotification( false,
																this->fTextView->Text() );
					}
				}
				else
				{
					if ( this->fTextView ) {
						this->fTextView->MakeEditable( true );
					}
					if ( this->fData ) {
						this->fData->SetNotification( true,
																this->fTextView->Text() );
					}
				}
			}

			break;
		default:
			BControl::MessageReceived( in );
	};
	
}	// <-- end of function NotificationView::MessageReceived

