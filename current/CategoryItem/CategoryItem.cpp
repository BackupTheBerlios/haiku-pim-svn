/*
 * Copyright 2010 Alexey Burshtein	<aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include <math.h>		/* For the "ceilf" function */
#include <stdio.h>
#include <stdlib.h>

#include <Application.h>
#include <Font.h>
#include <Message.h>
#include <GroupLayout.h>
#include <GridLayout.h>
#include <InterfaceKit.h>
#include <LayoutItem.h>
#include <ListItem.h>
#include <ListView.h>
#include <Size.h>


#include "CategoryItem.h"
#include "Utilities.h"

#ifndef SPACING
	#define SPACING 2
#endif

class clsMainWindow;

/*======================================================================
 * 		Implementation of class ColorUpdateWindow
 *=====================================================================*/
 
/*!	\function	ColorUpdateWindow::ColorUpdateWindow
 *	\brief		Constructor.
 *	\param[in]	corner	One of the corners of the window IN SCREEN COORDINATES!
 *	\param[in]	label	Label of the color. (Usually name of the category).
 *	\param[in]	enableEditingLabel	If "true", label can be edited. If "false", it's constant.
 *	\param[in]	title			Title of the window
 *	\param[in]	defaultColor	Original color. Defaults to black.
 *	\param[in]	targetLooper	The target which receives message with the results.
 *	\param[in]	currentScreen	Defines the screen the program runs in. Usually it's
 *								B_MAIN_SCREEN_ID.
 *	\param[in]	message			The template message to be used. If this parameter is NULL,
 *								a new message is constructed.
 */
ColorUpdateWindow::ColorUpdateWindow( BPoint corner,
								  	  BString& label,
								  	  rgb_color &defaultColor,
								  	  bool enableEditingLabel,
								  	  BString title,
								      BHandler *targetHandler,
								      screen_id currentScreen,
								   	  BMessage* message )
	:
	BWindow( BRect( 100, 100, 300, 500 ),
			 title.String(),
			 B_MODAL_WINDOW,
			 B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS | B_WILL_ACCEPT_FIRST_CLICK ),
	originalString( label ),
	originalColor( defaultColor),
	labelView( NULL ),
	okButton( NULL ),
	revertButton( NULL ),
	messageToSend( NULL ),
	target( targetHandler ),
	dirty( false )
{
	BSize layoutSize;
	BLayoutItem* item = NULL;
	float width, height, dontCare;
	BView* background = new BView( this->Bounds(),
								   "Background",
								   B_FOLLOW_LEFT | B_FOLLOW_TOP,
								   B_FRAME_EVENTS | B_WILL_DRAW );
	if ( !background )
	{
		/* Panic! */
		exit(1);
	}
	background->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );	
	
	this->enableEditingLabel = enableEditingLabel;
	
	/* There are three parts in the interface of the control.
	 * Upmost is the label, which, according to the user's settings,
	 * can be editable or not.
	 * Next is the color control.
	 * Last is the row of two buttons, Revert and Ok.
	 */

	// Debugging
	printf( "Color Selected = %u, Color Reverted = %u.\n",
			kColorSelected,
			kColorReverted );

	// Construct background view and layout
	BGridLayout* layout = new BGridLayout( B_VERTICAL );
	if ( !layout ) { /* Panic! */ exit(1); }
	layout->SetInsets( 5, 5, 5, 5 );
	
	background->SetLayout( layout );
	this->AddChild( background );

	// Constructing the name label, editable or not.
	if ( enableEditingLabel )
	{
		labelView = new BTextControl( BRect(0, 0, 1, 1),
									  "Label",
								   	  NULL,
								   	  label.String(),
								   	  NULL );
	} else {
		labelView = new BStringView( BRect (0, 0, 1, 1),
									 "Label",
									 label.String() );
	}
	if ( !labelView )
	{
		/* Panic! */
		exit(1);
	}
	labelView->ResizeToPreferred();
	item = layout->AddView( labelView, 0, 0, 3, 1 );
	if ( !item ) { /* Panic! */ exit(1); }
	item->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH, B_ALIGN_VERTICAL_CENTER ) );
//	item->SetExplicitMinSize( BSize( width, height ) );
	
	// Construct color control
	BMessage* toSend = new BMessage( kColorChanged );
	if ( !toSend ) { /* Panic! */ exit(1); }
	colorControl = new BColorControl( BPoint( 0, 0 ),
								      B_CELLS_32x8,
								      4.0,
								      "Color Control",
								      toSend );
	if ( !colorControl )
	{
		/* Panic! */
		exit(1);
	}
	colorControl->GetPreferredSize( &width, &height );
	colorControl->ResizeTo( width, height );
	colorControl->SetTarget( this );
	item = layout->AddView( colorControl, 0, 1, 3, 1 );
	if ( !item ) { /* Panic! */ exit(1); }
	item->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH, B_ALIGN_TOP ) );

	// Construct last two buttons
	// Revert button
	toSend = new BMessage( kColorReverted );
	if ( !toSend ) { /* Panic! */ exit(1); }
	revertButton = new BButton( BRect( 0, 0, 1, 1),
								"Revert button",
								"Revert",
								toSend );
	if ( !revertButton ) { /* Panic! */ exit(1); }
	revertButton->ResizeToPreferred();
	
	// Ok button
	toSend = new BMessage( kColorSelected );
	if ( !toSend ) { /* Panic! */ exit(1); }
	okButton = new BButton( BRect( 0, 0, 1, 1),
						    "Ok button",
						    "Ok",
						    toSend,
						    B_FOLLOW_RIGHT | B_FOLLOW_TOP );
	if ( !okButton ) { /* Panic! */ exit(1); }
	okButton->ResizeToPreferred();
	
	// Attach the buttons to current layout
	item = layout->AddView( revertButton, 0, 2 );
	if ( ! item ) { /* Panic! */ exit(1); }
	item->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT, B_ALIGN_MIDDLE ) );
		// Note I'm skipping one cell - this is for showing current color!
	item = layout->AddView( okButton, 2, 2 );
	if ( ! item ) { /* Panic! */ exit(1); }
	item->SetExplicitAlignment( BAlignment( B_ALIGN_RIGHT, B_ALIGN_MIDDLE ) );
	
	// Make "Ok" button the default
	okButton->MakeDefault( true );
	
	// Now, find the correct place for this window. 
	// We have one of the corners from constructor, we need to position the window
	// in such manner that it will be fully visible and keep one of the corners in
	// the specified point.
	layout->Relayout( true );
	layoutSize = layout->PreferredSize();
	this->ResizeTo( layoutSize.width, layoutSize.height );
	background->ResizeTo( layoutSize.width, layoutSize.height );
	
	float windowWidth = layoutSize.width, windowHeight = layoutSize.height;
	BScreen* mainScreen = new BScreen( currentScreen ); // Get access to current screen
	display_mode currentDisplayMode;
	mainScreen->GetMode( &currentDisplayMode );		// Obtain the width and height of screen
	
	// The following booleans uniquely define where the window be located regarding
	// given corner.
	bool leftFromCorner = false, upFromCorner = false;
	
	// Check where the window should span regarding to the corner
	if ( corner.x + windowWidth >= currentDisplayMode.virtual_width )
	{
		if ( corner.x - windowWidth < 0 )
		{
			corner.x = 0;
			leftFromCorner = false;
		}
		else
		{
			leftFromCorner = true;
		}
	}
	
	if ( corner.y + windowHeight >= currentDisplayMode.virtual_height )
	{
		if ( corner.y - windowHeight < 0 )
		{
			corner.y = 0;
			upFromCorner = false;
		}
		else
		{
			upFromCorner = true;
		}	
	}
	
	// Calculate new top-left corner of the window
	if ( leftFromCorner ) 	{ corner.x -= windowWidth; }
	if ( upFromCorner )		{ corner.y -= windowHeight; }
	
	// Move the window to calculated position
	this->MoveTo( corner );
	
	// Show the window
	this->Show();
	colorControl->Invoke();
}	// <-- end of constructor for ColorUpdateWindow



/*!	\function		ColorUpdateWindow::~ColorUpdateWindow
 *	\brief			Default destructor
 */
ColorUpdateWindow::~ColorUpdateWindow()
{
	this->Hide();
	
	int children = this->CountChildren();
	BView* tempView = NULL;
	
	for ( int i = 0; i < children; ++i )
	{
		tempView = this->ChildAt( i );
		this->RemoveChild( tempView );
		delete tempView;
	}
	
	colorControl = NULL;
	labelView = okButton = revertButton = NULL;
	originalString.Truncate( 0 );	
}	// <-- end of default destructor



/*!	\function		ColorUpdateWindow::MessageReceived
 *	\brief			Main function
 */
void ColorUpdateWindow::MessageReceived( BMessage* in )
{
	BAlert* al = NULL;
	DebuggerPrintout* deb = NULL;
	BTextView* textView = NULL;
	BView* background = NULL;
	BMessenger* mesg = NULL;
	status_t errorCode = B_OK;
	BString currentString( this->originalString );
	rgb_color previousHighColor;
	BRect tempRect;
	switch ( in->what )
	{
		case kColorSelected:
			/* The user had make his choise. */
			/* Checking if the color has changed... */
			if ( colorControl )
			{
				currentColor = colorControl->ValueAsColor();
				if ( currentColor != originalColor )
				{
					this->dirty = true;
				}
			}
			
			/* Checking if the string has changed... */
			if ( enableEditingLabel )
			{
				if ( labelView )
				{
					// textView = ( BTextView* )labelView;	// For matter of simplicity only
					currentString.SetTo( ( ( BTextControl* )labelView )->Text() );
					
					if ( currentString != originalString )	// If there were changes
					{
						if ( ! utl_CheckStringValidity( currentString ) )
						{
							// User has changed the string to an invalid one -
							// he must correct it before proceeding
							al = new BAlert("Error detected!",
											"The string you've entered is invalid. Please correct.",
											"Ok",
											NULL,
											NULL,
											B_WIDTH_AS_USUAL,
											B_STOP_ALERT );
							if ( al ) { 
								al->Go();
								break; 		// Returning to main window
							} else {
								/* Panic! */
								exit(1);
							}						
						}	// <-- end of "if (string is not valid) "
						else
						{
							// String is valid - verify it's different
							if ( currentString != originalString )
							{
								this->dirty = true;
							}							
						}						
					}	// <-- end of "if (user changed the string)					
				}	// <-- end of "if ( BTextView exists )"
			}	// <-- end of "if (user has possibility to change the string)"
		
			// If anything has changed, send the update message. Else, send revert message.
			if ( ! messageToSend )
			{
				if ( this->dirty )
				{
					this->messageToSend = new BMessage( kColorSelected );
				} else {
					this->messageToSend = new BMessage( kColorReverted );
				}
				if ( ! this->messageToSend ) {
					/* Panic! */
					exit(1);
				}
			}
			
			// Stuff the message with needed data
			messageToSend->AddBool( "Dirty", this->dirty );
			messageToSend->AddString ( "Original string", this->originalString );
			if ( this->dirty ) {
				messageToSend->AddString("New string", currentString );
			}

			messageToSend->AddInt32( "Original color", RepresentColorAsUint32( this->originalColor ) );
			messageToSend->AddInt32( "New color", RepresentColorAsUint32( this->currentColor ) );
			
			// Send the message and close current window
			// mesg = new BMessenger( (BHandler* )target, NULL,  &errorCode );
			mesg = new BMessenger( "application/x-vnd.Hitech.Skeleton", -1, &errorCode );
			
			if ( errorCode == B_OK ) {
				
				mesg->SendMessage( messageToSend, ( BHandler* )NULL );
				
				deb = new DebuggerPrintout( "Message was sent" );
			} else {
				deb = new DebuggerPrintout( "Message wasn't sent" );
			}
			
			this->Quit();
			
			break;

		case kColorReverted:
			/* Returning to original settings */
			if ( colorControl )
			{
				colorControl->SetValue( originalColor );
				colorControl->Invoke();
			}

			if ( enableEditingLabel && this->labelView )
			{
				( (BTextControl*)this->labelView )->SetText( this->originalString.String() );
				( (BTextControl*)this->labelView )->MakeFocus( false );	// Prevent accidental change of text
			}
			
			break;
			
		case kColorChanged:
		
			// We need to reflect the change in color.
			// We'll do it using the current view's high color. For this, we need to
			// back up current high color in order to restore it later.
			background = this->FindView( "Background" );
			if ( ! background )
			{
				deb = new DebuggerPrintout( "Didn't find background!" );
				return;
			}
			previousHighColor = background->HighColor();

			background->SetHighColor( 0, 0, 1 );	// almost black
			
			tempRect = BRect( ( revertButton->Frame() ).right + 10,
							  ( revertButton->Frame() ).top,
							  ( okButton->Frame() ).left - 10,
							  ( revertButton->Frame() ).bottom );

			// Actual drawing
			if ( this->Lock() ) {
				background->SetPenSize( 1 );
				
				// Create border
				background->StrokeRoundRect( tempRect.InsetBySelf( 1, 1 ), 4, 4 );
				
				// Fill the rectangle
				background->SetHighColor( colorControl->ValueAsColor() );
				background->FillRoundRect( tempRect.InsetBySelf( 1, 1 ), 4, 4 );
				background->Flush();
				this->Flush();
				this->Unlock();	
			}		

			background->SetHighColor( previousHighColor );
			break;

		default:
			BWindow::MessageReceived( in );
	};
	
}	// <-- end of function "ColorUpdateWindow::MessageReceived"
 

/*****************************************************************************************
 * 		Implementation of class CategoryListItem
 *---------------------------------------------------------------------------------------
 * Inspired by IconListItem by Fabien Fulhaber
 ****************************************************************************************/

/*!	\function 		CategoryListItem::CategoryListItem
 *	\brief			Constructor
 *	\details		Includes preparations for multilevel categories
 *	\param[in]	color	The color associated with a category
 *	\param[in]	label	Reference to a BString that contains name of the category
 *	\param[in]	level	Currently unused
 *	\param[in]	expanded	Currently unused
 */
CategoryListItem::CategoryListItem( rgb_color color, BString& label, int level, bool expanded )
	:
	BListItem( level, expanded ),
	currentColor( color ),
	currentLabel( label ),
	icon( NULL )
{
	// Creating of the icon depends on height of the item, thus calculate the height first.
	font_height	fh;
	BFont font(be_plain_font);
	font.GetHeight( &fh );
	SetHeight( ceilf( fh.ascent + fh.descent + fh.leading ) );
	
	// Creating the icon. It's a constant, therefore may be created 
	//	immediately after creation of the item. On color update it will be updated too.
	this->icon = CreateIcon( color, NULL );

	float width_of_item=font.StringWidth( label.String() );
	SetWidth(width_of_item);
	
}	// <-- end of default constructor for CategoryListItem


/*!	\function 	CategoryListItem::CreateIcon
 *	\brief		This function creates a black square filled with submitted color.
 *	\param[in]	colorIn		The color to be used inside the square (black is Ok).
 *	\param[in]	toChange	In case the BBitmap was already allocated, it's more efficient
 *							to draw on it then to deallocate it an reallocate it anew.
 *							A new BBitmap is allocated if "toChange" is NULL (default).
 *							If dimensions of the icon don't suit, it's reallocated anyway.
 *							The new pointer is written into "toChange" and also returned.
 *	\note		
 *				Validity of parameter "toChange" is NOT checked!
 *	\note	Colors usage:
 *				This function draws the background with B_DOCUMENT_BACKGROUND_COLOR
 *				(which is by default white), and the square frame around the requested
 *				color with B_DOCUMENT_TEXT_COLOR (which is by default black).
 *
 *	\note	Icon size:
 *				The size of the icon is derived from the height of the CategoryListItem
 *				object, which depends on selected system font. Whatever the height is,
 *				icon will be square, leaving one pixel of free space above it and one
 *				pixel below. Hence, the dimensions of the square with side "a" are:
 *					a = this->Height() - 2.
 *		
 *	\returns	Created BBitmap or NULL in case of error.
 */
BBitmap* CategoryListItem::CreateIcon(const rgb_color colorIn, BBitmap* toChange )
{
	int squareSize = ceilf( this->Height() - 2 );	//!< Side of the square.
	BRect rect(0, 0, squareSize, squareSize );
	BBitmap* toReturn = NULL;
	if ( !toChange )	// Checking availability of the input
	{
		toReturn = new BBitmap(rect, B_RGB32, true);
	} else {
		// It would be a good idea to check also the color space,
		// but it may be changed by the BBitmap itself, so...
		if ( ceilf ( ( toChange->Bounds() ).Width() ) != squareSize )
		{
			delete toChange;
			toChange = new BBitmap(rect, B_RGB32, true);
			if ( !toChange )
			{
				/* Panic! */
				exit(1);
			}			
		}
		toReturn = toChange;
	}
	
	BView* drawing = new BView( rect, 
								"Drawer", 
								B_FOLLOW_LEFT | B_FOLLOW_TOP,
								B_WILL_DRAW);
	if (!drawing || !toReturn) { return NULL; }	
	toReturn->AddChild(drawing);
	if (toReturn->Lock()) {
		
		// Clean the area
		drawing->SetHighColor( ui_color( B_DOCUMENT_BACKGROUND_COLOR ) );
		drawing->FillRect(rect);
		
		// Draw the black square
		drawing->SetHighColor( ui_color( B_DOCUMENT_TEXT_COLOR ) );
		drawing->SetPenSize(1);
		drawing->StrokeRect(rect);
		
		// Fill the inside of the square
		drawing->SetHighColor( colorIn );
		drawing->FillRect(rect.InsetBySelf(1, 1));
		
		// Flush the actions to BBitmap
		drawing->Sync();
		toReturn->Unlock();
	}
	toReturn->RemoveChild(drawing);			// Cleanup
	delete drawing;
	return toReturn;
}	// <-- end of function CategoryListItem::CreateIcon


/*!	\function		CategoryListItem::DrawItem
 *	\brief			Performs actual drawing into the ListView.
 *	\param[in]	owner	Pointer to the view that owns this item.
 *	\param[in]	frame	Frame of the item
 *	\param[in]	touchEverything		Every pixel in the "frame" should be touched.	
 */
void	CategoryListItem::DrawItem( BView* owner, BRect frame, bool touchEverything )
{
	BRect iconRect = frame, labelRect = frame;
	int squareSize = ceilf( this->Height() - 2 );	//!< Side of the icon's square.
	
	if ( !owner ) { return; }	// Nothing to do
	
	/* Filling with background color */
	if ( touchEverything ) {
		owner->SetLowColor( ui_color( B_DOCUMENT_BACKGROUND_COLOR ) );
		owner->FillRect( frame, B_SOLID_LOW );
	}
	
	/* If the item is selected, its background must be drawn in another color */
	if ( this->IsSelected() )
	{
		owner->SetLowColor( ui_color( B_MENU_SELECTED_BACKGROUND_COLOR ) );
		owner->FillRect( frame, B_SOLID_LOW );
	}	// <-- end of "if ( this item is selected )"

	/*!	\note	What happens if there's no icon?
	 *			First of all, it's extremely unlikely. Default category has an icon,
	 *			and there's no way to add a category with no color. If a new category
	 *			is detected, it is assigned a random color.
	 *			The icon, of course, will not be drawn. However, the label will still
	 *			be written at some offset, like if the icon were there.
	 *			In short, all labels are left-aligned and start vertically at the same
	 *			offset, regardless if they have icons or not.
	 */
	owner->MovePenTo( 0, frame.top );
	iconRect.left = frame.left + SPACING;
	iconRect.top = frame.top + 1;
	iconRect.bottom = frame.bottom - 1;
	iconRect.right = iconRect.left + squareSize;

	/* Drawing the icon */
	if ( this->icon )
	{
		owner->SetDrawingMode( B_OP_OVER );
		owner->DrawBitmap( this->icon, iconRect );
	}	// <-- end of "if ( icon exists )"
	
	/* Making space for the label */
	labelRect.left = iconRect.right + 2 * SPACING;
	labelRect.top = frame.top;
	labelRect.bottom = frame.bottom;
	labelRect.right = frame.right;
	
	/* Drawing the label */
	font_height fh;
	owner->GetFontHeight( &fh );
	owner->MovePenTo( labelRect.left, labelRect.bottom - fh.descent );
	
	if ( this->IsSelected() )
	{
		owner->SetHighColor( ui_color( B_DOCUMENT_TEXT_COLOR ) );
	} else {
		owner->SetHighColor( ui_color( B_MENU_SELECTED_ITEM_TEXT_COLOR ) );
	}
	owner->DrawString( this->currentLabel.String() );
	
}	// <-- end of function CategoryListItem::DrawItem



/*!	\function		CategoryListView::Update
 *	\brief			Calculates the new height of the item upon font update.
 *	\details		Also recreates the icon.
 */
void 	CategoryListItem::Update( BView* owner, const BFont* font )
{
	// Update the height and width of the item.
	BListItem::Update( owner, font );
	
	// The width of the item may require update.
	int squareSize = ceilf( this->Height() - 2 );	//!< Side of the icon's square.
	int labelSize = font->StringWidth( this->currentLabel.String() );
	if ( this->Width() < ( labelSize + squareSize + SPACING * 4 ) )
	{
		this->SetWidth( labelSize + squareSize + SPACING * 4 );
	}

	// The icon may require an update too.
	this->icon = CreateIcon( currentColor, this->icon );
	
}	// <-- end of function CategoryListItem::Update


/*!	\function		CategoryListItem::~CategoryListItem
 *	\brief			Default destructor
 */
CategoryListItem::~CategoryListItem( )
{
	if ( icon )
	{
		delete icon;
	}
	icon = NULL;
}


/***************************************************************************************
 *		Implementation of class CategoryListView
 **************************************************************************************/

/*!	\function	CategoryListView::CategoryListView
 *	\brief		Constructor for the category list
 *	\details	The constructor sets the selection message and invocation message
 *				templates. It also defines the scrollbars.
 *	\note		Scroller support
 *				It is assumed that this class is scrolled. It even resizes itself
 *				to make place for the scrollbars in the same frame.
 */
CategoryListView::CategoryListView( BRect frame, const char *name )
	:
	BListView( BRect( frame.left,
					  frame.top,
					  frame.right - B_V_SCROLL_BAR_WIDTH,
					  frame.bottom - B_H_SCROLL_BAR_HEIGHT ),
			   name ),
	scrollView( NULL )
{
	BMessage* templateMessage = NULL;
	
	templateMessage = new BMessage( kCategorySelected );
	if ( !templateMessage ) {
		/* Panic! */
		exit(1);
	}
	this->SetSelectionMessage( templateMessage );
	
	templateMessage = new BMessage( kCategoryInvoked );
	if ( !templateMessage ) {
		/* Panic! */
		exit(1);
	}
	this->SetInvocationMessage( templateMessage );
	
}	// <-- end of constructor for CategoryListView


/*!	\function		CategoryListView::GetPreferredSize
 *	\brief			Returns preferred width and height for this CategoryListView
 *	\param[out]		width	Preferred width
 *	\param[out]		height	Preferred height
 */
void		CategoryListView::GetPreferredSize( float* width, float* height )
{
	int numOfItems = this->CountItems();
	
	font_height fh;
	BFont font;
	this->GetFont( &font );
	font.GetHeight( &fh );
	
	int heightOfSingleItem = fh.leading + fh.ascent + fh.descent;
	
	*height = ( numOfItems == 0 ? heightOfSingleItem : heightOfSingleItem * numOfItems );
	
	float currentWidth = 0;
	*width = 0;
	
	for ( int i = 0; i < numOfItems; ++i )
	{
		currentWidth = ( ( this->ItemAt( i ) )->Width() );
		if ( currentWidth > *width )
		{
			*width = currentWidth;
		}
	}
	
	width += ( heightOfSingleItem - 2 ) + 2 * SPACING;
	
}	// <-- end of function "CategoryListView::GetPreferredSize"



/*!	\function		CategoryListView::SetScroller
 *	\brief			Obtain access to scroll view that is used to scroll *this.
 *	\details		Unsetting the scroll view is performed by submitting NULL.
 *	\param[in]	scrollViewIn	The BScrollView that is used to scroll current view.
 *	\note			Note on ownership of the BScrollView:
 *					This class takes care of updating the scrollbars etc., but
 *					it doesn't own the BScrollView. The creator of CategoryListView
 *					is responsible for deleting the BScrollView when it's not needed.
 */
void	CategoryListView::SetScroller( BScrollView* scrollViewIn )
{
	this->scrollView = scrollViewIn;
	if ( !this->scrollView ) {
		((BScrollBar*)(scrollView->ScrollBar(B_VERTICAL)))->SetSteps(5, 20);
		((BScrollBar*)(scrollView->ScrollBar(B_HORIZONTAL)))->SetSteps(5, 20);
		
		BListView::TargetedByScrollView( scrollViewIn );
		
		FixupScrollbars();
	}
}	// <-- end of function CategoryListView::SetScroller



/*!	\function		CategoryListView::GetScroller
 *	\brief			Obtain the currently defined scroller for this CategoryListView.
 *	\returns		The BScrollView previously set as the scroller for this view
 *					using function CategoryListView::SetScroller. May be NULL!
 */
BScrollView*	CategoryListView::GetScroller( void ) const
{
	return this->scrollView;	
}	// <-- end of function CategoryListView::GetScroller



/*!	\function		CategoryListView::FrameResized
 *	\brief			Called when the parent view is resized.
 *	\details		Main usage of this function is to fix the scrollbars.
 *	\param[in]	width	New width of the view.
 *	\param[in]	height	New height of the view.
 *	\note			Note on the scrollbars:
 *					It's assumed that this view is a target of a BScrollView. It's
 *					resized in the way which makes space for the scrollbars. If it
 *					knows about a BScrollView that targets this view, it resizes that
 *					BScrollView accordingly (thus the owner shouldn't do it).
 */
void	CategoryListView::FrameResized( float width, float height )
{
	// Firstly, resize the base class.
	BView::FrameResized( width,
						 height );

	// Now resize the BScrollView
	if ( this->scrollView )
	{
		(this->scrollView)->ResizeTo( width, height );
		
		FixupScrollbars();
	}
	
}	// <-- end of function "CategoryListView::FrameResized"



/*!	\function		CategoryListView::FixupScrollbars
 *	\brief			Resizes the scrollbars for correct size reflection
 */
void CategoryListView::FixupScrollbars( void )
{
	BRect bounds = this->Bounds();
	BScrollBar *sb;

	float ratio=1, realRectWidth=1, realRectHeight=1;
	if ( !scrollView ) {
		/* Nothing to fix */
		return;
	}

	/* Obtain info on current font */
	this->GetPreferredSize( &realRectWidth, &realRectHeight );
	font_height fh;
	BFont font( be_plain_font );
	font.GetHeight( &fh );
	
	/* Calculate the height occupied by all items *
	realRectHeight = this->CountItems() * ( ceilf( fh.ascent + fh.descent + fh.leading ) );

	* For calculating the width, we must find the item with longest label *
	realRectHeight = listView->CountItems() * ;
	realRectWidth += 15 + B_V_SCROLL_BAR_WIDTH;	
	*/
	
	// Fixing up horizontal scrollbar.
	sb = scrollView->ScrollBar( B_HORIZONTAL );
	if (sb) {
		ratio = bounds.Width() / ( (float)realRectWidth + 1 );

		sb->SetRange(0, realRectWidth-bounds.Width());
		if (ratio >= 1) {
			sb->SetProportion(1);
		} else {
			sb->SetProportion(ratio);
		}
	}

	// Fixing up vertical scrollbar.
	sb = scrollView->ScrollBar(B_VERTICAL);
	if (sb) {
		ratio = bounds.Height() / ( (float)realRectHeight + 1 );
		
		sb->SetRange(0, realRectHeight+2);
		if (ratio >= 1) {
			sb->SetProportion(1);
		} else {
			sb->SetProportion(ratio);
		}
	}
}	// <-- end of function CategoryListView::FixupScrollbars



/*!	\function		CategoryListView::~CategoryListView
 *	\brief			Destructor
 *	\details		Does nothing, since the class has no internal items.
 */
CategoryListView::~CategoryListView( )
{

}	// <-- end of "destructor for class CategoryListView"



/***************************************************************************************
 *		Implementation of class CategoryMenuItem
 **************************************************************************************/

/*!	\function	CategoryMenuItem::CategoryMenuItem
 *	\brief		Constructor.
 *	\param[in]	labelIn 		Reference to BString that defines the label of the Category.
 *	\param[in]	color		Color associated with the current Category.
 *	\param[in]	message		Message sent on invocation of this item.
 *	\note		Differences from standard BMenuItem constructor
 *				CategoryMenuItem actively rejects the shortcuts and modifiers, therefore
 *				they're not accepted even in standard constructor. Additionally,
 *				function "SetShortcut" is overloaded with empty body.
 */
CategoryMenuItem::CategoryMenuItem( BString& labelIn, const rgb_color color, BMessage* message )
	:
	BMenuItem( labelIn.String(), message ),
	icon( NULL )
{
	this->icon = CreateIcon( color, NULL );
	if ( !this->icon )
	{
		/* Panic! */
		exit(1);
	}

}	// <-- end of constructor of CategoryMenuItem



/*!	\function	CategoryMenuItem::CreateIcon
 *	\brief		Create the square icon filled with submitted color.
 *	\param[in]	color		The color of the requested icon.
 *	\param[in]	toChange	If there is an allocated item, it may be changed.
 *							If the submitted pointer is not NULL (which is default),
 *							this BBitmap is tested for dimensions match, and if dimensions
 *							allow, its contents are replaced with new icon. Else, old icon
 *							is deleted, and a new is created. In this case, both the
 *							"toChange" pointer and returned pointer point to the same
 *							BBitmap.
 */
BBitmap*	CategoryMenuItem::CreateIcon( const rgb_color color,
										  BBitmap* toChange )
{
	BBitmap* toReturn = NULL;	//!< This is the value to be returned.
	BRect tempRect;
	float width, height, squareSide;
	
	// Get size of the square
	this->GetContentSize( &width, &height );
	squareSide = ceilf( height ) - 2;
	
	// Compare submitted bitmap to calculated size
	if ( toChange )
	{
		tempRect = toChange->Bounds();
		if ( ( tempRect.Width() != squareSide ) ||
			 ( tempRect.Height() != squareSide ) )
		{
			// Dimensions don't match - need to delete the bitmap and reallocate it
			delete toChange;
			tempRect.Set( 0, 0, squareSide, squareSide );
			toChange = new BBitmap( tempRect, B_RGB32, true );
			if ( !toChange )
			{
				/* Panic! */
				exit(1);
			}
			
			toReturn = toChange;
		}
		else
		{
			/*!	\note	Note about color spaces
			 *			Actually, even if the dimensions are correct, the existing
			 *			BBitmap may be not suitable due to incorrect color space.
			 *			However, BBitmap may change the color space on its own, (and
			 *			probably will, since there's no much sense in having 32 bits
			 *			per pixel for bitmap with only 2 colors - black for the frame
			 *			and Category's color for the inside). Therefore, color space is
			 *			not checked. It's assumed that existing color space is good enough.
			 */
			// Dimensions match, color space is not checked - continuing
			toReturn = toChange;
		}
	}
	else	// No bitmap is submitted
	{
		toReturn = new BBitmap( tempRect, B_RGB32, true );
		if ( !toReturn )
		{
			/* Panic! */
			exit(1);
		}
	}
	
	/* Here toReturn is already set. */
	
	// Add the drawing view to the bitmap
	tempRect.Set( 0, 0, squareSide, squareSide );
	BView* drawing = new BView (tempRect,
								"Drawer", 
								B_FOLLOW_LEFT | B_FOLLOW_TOP,
								B_WILL_DRAW);
	if (!drawing || !toReturn) {
		/* Panic! */
		return NULL;
	}	
	toReturn->AddChild(drawing);
	if (toReturn->Lock()) {

		// Clean the area
		drawing->SetHighColor( ui_color( B_MENU_BACKGROUND_COLOR ) );
		drawing->FillRect( tempRect );
		
		// Draw the black square
		drawing->SetHighColor( ui_color( B_MENU_ITEM_TEXT_COLOR ) );
		drawing->SetPenSize( 1 );
		drawing->StrokeRect( tempRect );
		
		// Fill the inside of the square
		drawing->SetHighColor( color );
		drawing->FillRect( tempRect.InsetBySelf( 1, 1 ) );
		
		// Flush the actions to BBitmap
		drawing->Sync();
		toReturn->Unlock();	
	}

	toReturn->RemoveChild( drawing );
	delete drawing;

	return toReturn;
}	// <-- end of function "CategoryMenuItem::CreateIcon"


/*!	\function		CategoryMenuItem::GetContentSize
 *	\brief			Returns the size for this item, including possible icon and label.
 *	\param[out]		width	Width of rectangle which encloses the item.
 *	\param[out]		height	Height of rectangle which encloses the item.
 */
void 	CategoryMenuItem::GetContentSize( float* width, float* height )
{
	BFont plainFont(be_plain_font);
	font_height fh;
	plainFont.GetHeight( &fh );
	int fontHeight = ceilf( fh.leading + fh.ascent + fh.descent );
	
	// Get width and height for label only
//	if ( Menu() ) {
//		BMenuItem::GetContentSize( width, height );
//	} else {
	if ( height ) {
		*height = fontHeight;
	}
	if ( width )
	{
		// 2*SPACING between icon and label, size of icon is "height-2"
		*width = ceilf( plainFont.StringWidth( this->Label() ) ) +
			SPACING * 2 + ( fontHeight - 2 );;
	}

	
}	// <-- end of function CategoryMenuItem::GetContentSize



/*!	\function		CategoryMenuItem::DrawContent
 *	\brief			Performs actual drawing of the item.
 */
void	CategoryMenuItem::DrawContent( void )
{
	BPoint drawPoint( this->ContentLocation() );
	float height;
	this->GetContentSize( NULL, &height );
	
	if ( this->icon != NULL )
	{	// Drawing icon
		Menu()->SetDrawingMode( B_OP_OVER );
		Menu()->SetLowColor( B_TRANSPARENT_32_BIT );
		
		// If the item is enabled, draw its icon semi-transparent
		if ( this->IsEnabled() == false )
		{
			Menu()->SetDrawingMode( B_OP_BLEND );
			Menu()->DrawBitmap( this->icon, drawPoint );
			Menu()->SetDrawingMode( B_OP_OVER );
		}
		else	// else, use original color
		{
			Menu()->DrawBitmap( this->icon, drawPoint );
		}	
	}
	
	// Move the pen to starting position of the label
	drawPoint.x += 2 * SPACING + ceilf( height - 2 );
	Menu()->MovePenTo( drawPoint );
	
	// Draw the label using BMenuItem's function
	BMenuItem::DrawContent();
	
}	// <-- end of function "CategoryMenuItem::DrawContent"


/*!	\function		CategoryMenuItem::~CategoryMenuItem
 *	\brief			Destructor.
 *	\details		Deallocates the icon.
 */
CategoryMenuItem::~CategoryMenuItem()
{
	if ( this->icon )
	{
		delete this->icon;
		this->icon = NULL;
	}
}	// <-- end of function "CategoryMenuItem::~CategoryMenuItem"


/***************************************************************************************
 *		Implementation of class CategoryMenu
 **************************************************************************************/

/*!	\function		CategoryMenu::CategoryMenu
 *	\brief			Constructor for CategoryMenu
 *	\details		This menu populates itself with Categories found in message with
 *					preferences. If this parameter is NULL, empty menu is created.
 *	\param[in]	name		Name of the menu
 *	\param[in]	preferences	Message with all preferences. It is parsed, and categories'
 *							info is used to populate the menu with categories.
 */
CategoryMenu::CategoryMenu( const char *name, BMessage* preferences )
	:
	BMenu( name, B_ITEMS_IN_COLUMN )
{
	
	
	
}	// <-- end of constructor for CategoryMenu


/*!	\function		CategoryMenu::~CategoryMenu
 *	\brief			Destructor for CategoryMenu
 *	\details		The menu doesn't have internal data, therefore the destructor does
 *					nothing.
 */
CategoryMenu::~CategoryMenu()
{
	
}	// <-- end of destructor for CategoryMenu


/*!	\function		CategoryMenu::RefreshMenu
 *	\details		This function removes all items from the menu and populates it
 *					again according to the data found in the preferences message.
 *	\param[in]	preferences		The message with categories used to populate the menu.
 */
void		CategoryMenu::RefreshMenu( BMessage* preferences )
{
	if ( !preferences ) { return; }
	
	
}	// <-- end of function CategoryMenu::RefreshMenu
