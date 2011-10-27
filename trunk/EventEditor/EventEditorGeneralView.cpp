/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
 
// Project includes
#include "EventEditorGeneralView.h"

// OS includes
#include <Alignment.h>
#include <GridLayout.h>
#include <GroupLayout.h>
#include <LayoutItem.h>
#include <Size.h>
#include <StringView.h>

// POSIX includes
#include <stdio.h>

/*----------------------------------------------------------------------------
 *								Messages constants
 *---------------------------------------------------------------------------*/
 
const uint32		kEventNameUpdated				= 'ENup';
const uint32		kEventLocationUpdated		= 'ELup';
const uint32		kEventCategoryUpdated		= 'ECup';
const uint32		kStartTimeUpdated				= 'STup';
const uint32		kStartDateUpdated				= 'SDup';
const uint32		kEndTimeUpdated				= 'ETup';
const uint32		kEndDateUpdated				= 'EDup';
const	uint32		kEventLastsWholeDays			= 'ELWD';
const uint32		kEventNoDurationToggled		= 'ENoD';
const uint32		kNewCalendarModuleSelected = 'ECMS';
const uint32		kEventTypeSet					= 'ETyS';
const uint32		kPrivateToggled				= 'EPrv';

/*----------------------------------------------------------------------------
 *						Implementation of class EventEditor_GeneralView
 *---------------------------------------------------------------------------*/

/*!	\brief		Default constructor
 */
EventEditor_GeneralView::EventEditor_GeneralView( BRect frame, EventData* data )
	: 
	BView( frame,
			"General Event Editor tab",
			B_FOLLOW_LEFT | B_FOLLOW_TOP,
			B_WILL_DRAW | B_FRAME_EVENTS ),
	fData( data ),
	_EventName( NULL ),
	_Location( NULL ),
	_CategoryMenu( NULL ),
	_CategoryMenuField( NULL ),
	_StartMomentSelector( NULL )
{
	BMessage* toSend = NULL;
	BLayoutItem* layoutItem = NULL;
	
	
	// Sanity check
	if ( !data )
	{
		_LastError = B_BAD_VALUE;
		return;
	}

	
	fStartTime = fData->GetStartTime();
	fCalModule = utl_FindCalendarModule( fStartTime.GetCalendarModule() );
	
	if ( !fCalModule ) {
		/* Panic! */
		_LastError = B_BAD_VALUE;
		return;
	}
	fDuration = fData->GetDuration();
	fEndTime = fCalModule->FromTimeTToLocalCalendar( ( uint32 )fDuration +
					+ fCalModule->FromLocalCalendarToTimeT( fStartTime ) );
	
	// Create layout
	BGridLayout* gridLayout = new BGridLayout();
	if ( ! gridLayout ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	this->SetLayout( gridLayout );
	gridLayout->SetInsets( 5, 5, 10, 5 );
	
	/*----------------------------
	 *   Event Name text field
	 *---------------------------*/
	
	// Message
	toSend = new BMessage( kEventNameUpdated );
	if ( !toSend ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	
	// Create field for Event name label
	BStringView* eventNameLabel = new BStringView( BRect( 0, 0, 1, 1 ),
																  "Event name label",
																  "Event name:" );	// Label
	if ( !eventNameLabel ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	eventNameLabel->ResizeToPreferred();
	layoutItem = gridLayout->AddView( eventNameLabel, 0, 0 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_RIGHT, B_ALIGN_MIDDLE ));
	}
	
	// Create the text control
	_EventName = new BTextControl( BRect( 0, 0, 1, 1 ),
											 "Event name text control",
											 NULL,		// Label
											 fData->GetEventName().String(),
											 toSend );
	if ( !_EventName ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	_EventName->ResizeToPreferred();
	_EventName->SetToolTip( "Identifier of the Event.\n"
								  "May be not unique, or even empty.\n"
								  "Limited to 255 symbols." );
	_EventName->SetDivider( 0 );
	_EventName->TextView()->SetMaxBytes( 255 );
	layoutItem = gridLayout->AddView( _EventName, 1, 0 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH, B_ALIGN_MIDDLE ) );
	}
	
	/*----------------------------
	 *   Location text field
	 *---------------------------*/
	
	// Message
	toSend = new BMessage( kEventLocationUpdated );
	if ( !toSend ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	
	// Create label for the Event location
	BStringView* locationLabel = new BStringView( BRect( 0, 0, 1, 1 ),
																 "Event location label",
																 "Event location:" );		// Label
	if ( ! locationLabel ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	locationLabel->ResizeToPreferred();
	layoutItem = gridLayout->AddView( locationLabel, 0, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_RIGHT, B_ALIGN_MIDDLE ) );
	}
	
	// Create field for Event location
	_Location = new BTextControl( BRect( 0, 0, 1, 1 ),
											 "Event location text control",
											 NULL,
											 fData->GetEventLocation().String(),
											 toSend );
	if ( !_Location ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	_Location->ResizeToPreferred();
	_Location->TextView()->SetMaxBytes( 255 );
	_Location->SetToolTip( "Where will the Event occur?\n"
								  "Limited to 255 symbols." );
	layoutItem = gridLayout->AddView( _Location, 1, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH, B_ALIGN_MIDDLE ) );
	}
	
	/*------------------------------
	 * 		Categories menu
	 *-----------------------------*/
	toSend = new BMessage( kEventCategoryUpdated );
	if ( !toSend ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}

	_CategoryMenu = new CategoryMenu( "Catrgories menu",
												 false,		// Separator is not needed
												 toSend,		// Template message
												 NULL );		// Categories from the global list
	if ( !_CategoryMenu ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	
	// Mark the category representing current item
	CategoryMenuItem* item = ( CategoryMenuItem* )_CategoryMenu->FindItem( fData->GetCategory().String() );
	if ( !item ) {
		item = ( CategoryMenuItem* )_CategoryMenu->FindItem( "Default" );
		fData->SetCategory( "Default" );
	}
	if ( !item ) {
		_LastError = B_BAD_VALUE;
		return;
	}
	item->SetMarked( true );
	
	/*------------------------------
	 *  	Separator field
	 *-----------------------------*/
	BBox* separatorBox = new BBox( BRect( 0, 0, Bounds().Width() - 15, 1 ), "Separator Box" );
	if ( separatorBox ) {
		layoutItem = gridLayout->AddView( separatorBox, 0, 2, 2, 1 );
		BSize size( Bounds().Width() - 15, 1 );
		layoutItem->SetExplicitMaxSize( size );
		layoutItem->SetExplicitMinSize( size );
	}
	
	/*------------------------------
	 *  	Categories menu field
	 *-----------------------------*/
	BStringView* menuStringView = new BStringView( BRect( 0, 0, 1, 1 ),
																  "Category menu explanation",
																  "Category:" );
	if ( !menuStringView ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	menuStringView->ResizeToPreferred();
	layoutItem = gridLayout->AddView( menuStringView, 0, 3 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_RIGHT, B_ALIGN_MIDDLE ) );
	}
	
	BMenuBar* categoryMenuBar = new BMenuBar( BRect( 0, 0, 1, 1 ),
															"Category selector",
															B_FOLLOW_LEFT | B_FOLLOW_TOP,
															B_ITEMS_IN_ROW,
															true );
	if ( !categoryMenuBar ) {
		/* Panic! */
		_LastError = B_NO_MEMORY;
		return;
	}
	categoryMenuBar->AddItem( _CategoryMenu );
	
	BMenuItem* tempMenuItem = new BMenuItem( "|", NULL );
	tempMenuItem->SetEnabled( false );
	categoryMenuBar->AddItem( tempMenuItem );
	categoryMenuBar->SetBorder( B_BORDER_EACH_ITEM );
	layoutItem = gridLayout->AddView( categoryMenuBar, 1, 3 );
	if ( layoutItem ) {
		float maxLength = 0, tempLength;
		int limit = global_ListOfCategories.CountItems();
		Category* cat;
		for ( int i = 0; i < limit; ++i ) {
			cat = ( Category* )global_ListOfCategories.ItemAt( i );
			if ( ( tempLength = be_plain_font->StringWidth( cat->categoryName.String() ) ) > maxLength ) {
				maxLength = tempLength;
			}
		}
		BSize tempSize( maxLength + 20, menuStringView->Bounds().Height() + 3 );
		layoutItem->SetExplicitMaxSize( tempSize );
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT, B_ALIGN_MIDDLE ) );
	}
	
	/*------------------------------
	 *  	Start date part
	 *-----------------------------*/
	BBox* startMomentSelector = CreateStartMomentSelector();
	if ( !startMomentSelector ) {
		_LastError = B_NO_MEMORY;
		return;
	}
	startMomentSelector->ResizeToPreferred();
	layoutItem = gridLayout->AddView( startMomentSelector, 0, 4, 2, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT, B_ALIGN_TOP ) );
		layoutItem->SetExplicitPreferredSize( BSize( Bounds().Width()-15, startMomentSelector->Bounds().Height() ) );
//		layoutItem->SetExplicitMinSize( BSize( Bounds().Width()-15, startMomentSelector->Bounds().Height() ) );
	}

	/*------------------------------
	 *  	Event takes whole day
	 *-----------------------------*/
	toSend = new BMessage( kEventLastsWholeDays );
	if ( !toSend ) {
		_LastError = B_NO_MEMORY;
		return;
	}
	
	_EventLastsWholeDays = new BCheckBox( BRect( 0, 0, 1, 1 ),
													  "Event lasts whole days",
													  "All day",
													  toSend );
	if ( !_EventLastsWholeDays ) {
		_LastError = B_NO_MEMORY;
		return;
	}
	_EventLastsWholeDays->ResizeToPreferred();
	_EventLastsWholeDays->SetToolTip( "If this checkbox is selected, Event\n"
												 "will start at midnight and end on\n"
												 "midnight. It will occupy enough days\n"
												 "to include all previously set Duration." );
	_EventLastsWholeDays->SetValue( fData->GetLastsWholeDays() == true );
	
	layoutItem = gridLayout->AddView( _EventLastsWholeDays, 0, 5 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT, B_ALIGN_MIDDLE ) );
	}
	
	
	/*------------------------------
	 *  		Event is private
	 *-----------------------------*/
	toSend = new BMessage( kPrivateToggled );
	if ( !toSend ) {
		_LastError = B_NO_MEMORY;
		return;
	}
	
	_EventIsPrivate = new BCheckBox( BRect( 0, 0, 1, 1 ),
													  "Event is private",
													  "Private",
													  toSend );
	if ( !_EventIsPrivate ) {
		_LastError = B_NO_MEMORY;
		return;
	}
	_EventIsPrivate->ResizeToPreferred();
	_EventIsPrivate->SetValue( fData->GetPrivate() == true );
	_EventIsPrivate->SetToolTip( "This option does nothing. It's intented\n"
										  "for the glorious times when Haiku is real\n"
										  "multiuser. Then it will hid Events of one\n"
										  "user from the suspecting eyes of another.\n" );
	
	layoutItem = gridLayout->AddView( _EventIsPrivate, 1, 5 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_RIGHT, B_ALIGN_MIDDLE ) );
	}


	_LastError = B_OK;
	
}	// <-- end of EventEditor_GeneralView::EventEditor_GeneralView



/*!	\brief 		Destructor.
 */
EventEditor_GeneralView::~EventEditor_GeneralView()
{
	if ( _EventName ) {
		_EventName->RemoveSelf();
		delete _EventName;
		_EventName = NULL;
	}
	
	if ( _Location ) {
		_Location->RemoveSelf();
		delete _Location;
		_Location = NULL;
	}
	
	if ( _CategoryMenuField ) {
		_CategoryMenuField->RemoveSelf();
		delete _CategoryMenuField;
		_CategoryMenuField = NULL;
	}
	
	
	
	
}	// <-- end of destructor for class EventEditor_GeneralView



/*!	\brief		Creates the box that manages start time.
 */
BBox*		EventEditor_GeneralView::CreateStartMomentSelector()
{
	BBox* toReturn;
	BGroupLayout* layout;
	BLayoutItem* layoutItem;
	BMessage* toSend;
	
	toReturn = new BBox( BRect( 5, 5, this->Bounds().right - 5, 30 ),
								"Start moment selector" );
	if ( !toReturn ) {
		return NULL;
	}
	toReturn->SetLabel( "Start day and time" );
	
	layout = new BGroupLayout( B_VERTICAL );
	if ( !layout ) {
		delete toReturn;
		return NULL;
	}
	toReturn->SetLayout( layout );
	layout->SetInsets( 10, 15, 10, 10 );
	
	// Start date selector
	toSend = new BMessage( kStartDateUpdated );
	if ( !toSend ) {
		/* Panic! */
		return NULL;
	}
	toSend = new BMessage( kStartDateUpdated );
	_StartDateControl = new CalendarControl( BRect( 0, 0, 1, 1 ),
														  "Start date selector",
														  "Start date:",
														  fCalModule->Identify(),
														  fCalModule->FromLocalCalendarToTimeT( fStartTime ),
														  toSend );
	if ( !_StartDateControl ) {
		/* Panic! */
		delete toReturn;
		return NULL;
	}
	
	_StartDateControl->ResizeToPreferred();
	layout->AddView( _StartDateControl );
	
	
	// Start time selector													  
	toSend = new BMessage( kStartTimeUpdated );
	if ( !toSend ) {
		/* Panic! */
		delete toReturn;
		return NULL;
	}
	
	_StartTimeHourMinControl = new TimeHourMinControl( BRect( 0, 0, 1, 1 ),
																		"Start time selector",
																		"Start time:",
																		toSend );
	if ( !_StartTimeHourMinControl ) {
		/* Panic! */
		delete toReturn;
		return NULL;
	}	
	layout->AddView( _StartTimeHourMinControl );
	_StartTimeHourMinControl->SetCurrentTime( fStartTime );
	
	return toReturn;
}	// <-- end of function EventEditor_GeneralView::CreateStartMomentSelector()



/*!	\brief		Main function of the class.
 */
void		EventEditor_GeneralView::MessageReceived( BMessage* in )
{
	BString tempString;
	uint32	tempUint32_1, tempUint32_2;
	
	switch( in->what )
	{
		case kEventNameUpdated:
			fData->SetEventName( _EventName->Text() );
			break;
		
		case kEventLocationUpdated:
			fData->SetEventLocation( _Location->Text() );
			break;
		
		case kEventCategoryUpdated:
			if ( B_OK != in->FindString( "Category", &tempString ) ) {
				tempString.SetTo( "Default" );
			}
			fData->SetCategory( tempString );
			break;
		
		case kStartTimeUpdated:
			fData->GetStartTime( ( int* )&tempUint32_1, ( int* )&tempUint32_2 );
			in->FindInt32( kHoursValueKey.String(), ( int32* )&tempUint32_1 );
			in->FindInt32( kMinutesValueKey.String(), ( int32* )&tempUint32_2 );
			fData->SetStartTime( tempUint32_1, tempUint32_2 );
			break;
		
		default:
			BView::MessageReceived( in );	
	}
	
}	// <-- end of function EventEditor_GeneralView::MessageReceived



/*!	\brief		Adding this view to the messaging loop
 */
void		EventEditor_GeneralView::AttachedToWindow() {
	BView::AttachedToWindow();
	
	if ( Parent() )
		SetViewColor( Parent()->ViewColor() );
	
	if ( this->Looper() && this->Looper()->Lock() ) {
		Looper()->AddHandler( this );
		Looper()->Unlock();	
	}
	
	// Update targets
	_EventName->SetTarget( this );
	_Location->SetTarget( this );
	_EventLastsWholeDays->SetTarget( this );
	_EventIsPrivate->SetTarget( this );
	_StartTimeHourMinControl->SetTarget( this );
	_StartDateControl->SetTarget( this );
//	_EndDateControl->SetTarget( this );
//	_EndTimeHourMinControl->SetTarget( this );
}	// <-- end of function EventEditor_GeneralView::AttachedToWindow


/*!	\brief		Create the box for the end time.
 */
// BBox*			EventEditor_GeneralView::
