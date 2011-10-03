/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include "HourMinControl.h"

// System includes
#include	<GroupLayout.h>
#include <Font.h>
#include <Layout.h>
#include <Menu.h>
#include <MenuItem.h>
#include <String.h>
#include <View.h>

/*==========================================================================
**			IMPLEMENTATION OF CLASS HourMinControl
**========================================================================*/


/*!	\brief		Constructor of class HourMinControl.
 *		\param[in]	bounds		Frame of the control.
 *		\param[in]	name			Name of this object instance.
 *		\param[in]	label			What is displayed to the left of the menubar.
 *		\param[in]	trIn			Initial time (what is set at the time chooser)
 *										Only hours and minutes matter!
 *		\param[in]	representingTime		If "true", the menus always cover 24 hours
 *										(according to Time preferences), and the checkbox is
 *										always "AM / PM" or disabled. If "false", the caller
 *										may specify its own checkbox label and time limit.
 * 	\param[in]	checkBoxLabel			If the time represented is not "real",
 *										user may add a checkbox by providing this label. If
 *										no label is provided, no checkbox is created. If the
 *										represented time IS real, the checkbox is always 
 *										"AM/PM".
 *		\param[in]	initialCheckBoxValue		Checkbox will be checked if "true", unchecked
 *										if "false". If there's no checkbox, or if representing real
 *										time, this parameter is meaningless.
 *		\param[in]	hoursLimit	What is the limit of hours supported by this control?
 *		\param[in]	minutesLimit	What is the limit of minutes supported by this control?
 *		\param[in]  invocationMessage		If specified, it is sent every time the control's
 *										represented time is updated.
 */
HourMinControl::HourMinControl( BRect bounds,
											const char* name,
											const BString& label,
											const TimeRepresentation *trIn,
											bool representingTime,
											const char* checkBoxLabel,					
											unsigned int hoursLimit,
											unsigned int minutesLimit,
											BMessage* invocationMessage )
	:
	BView( bounds, name, 
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
		B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS ),
	labelView(NULL),
	chooserMenuBar(NULL),
	hoursMenu(NULL),
	minutesMenu(NULL),
	hasCheckBox( false )
{
	status_t retVal = B_OK;
	
	
	// Initialize the represented time structure.
	if ( trIn ) {
		this->representedTime = *trIn;
	} else {
		// If no time is given, and we're representing real time,
		//		represent current time.
		if ( representingTime ) {
			struct tm * curTime = NULL;
			time_t tempTime = 0;
			time( &tempTime );
			curTime = localtime( &tempTime );
			if ( curTime ) {
				this->representedTime = TimeRepresentation( *curTime );
			} else {	// Returned struct is NULL
				this->representedTime.tm_hour = 0;
				this->representedTime.tm_min = 0;
			}
		} else {
			// If no time is given, and we're representing something which
			// is NOT a real time, then initialize the clock to 00:05.
			this->representedTime.tm_hour = 0;
			this->representedTime.tm_min = 5;
		}
	}
	
	// Set internal variables
	this->hoursLimit = hoursLimit;
	this->minutesLimit = minutesLimit;
	this->representingTime = representingTime;
	this->fLabel = label;
	
	/* Deal with invocation message */
	if ( invocationMessage ) {
		this->invocationMessage = invocationMessage;
	} else {
		this->invocationMessage = new BMessage( kTimeControlUpdated );
		// Here the message actually may be NULL, but it does not matter.
		// Maximum - nothing will be sent to the parent of the view.
	}
	
	/* Deal with the checkbox */
	if ( representingTime ) {
		// If representing real time, then the checkbox always exists, and its
		//	label is "PM".
		this->fCheckBoxLabel.SetTo( "PM" );
		this->hasCheckBox = true;
	}
	else
	{
		// If representing something else, (say, duration of something),
		// then checkbox will exist only if its label is provided.
		if ( checkBoxLabel ) {
			this->hasCheckBox = true;
			this->fCheckBoxLabel.SetTo( checkBoxLabel );
		}
		else
		{
			this->hasCheckBox = false;
			this->fCheckBoxLabel.Truncate( 0 );
		}
	}
	
	InitUI ( bounds, label );
}	/* End of constructor */

/*!	
 *	\brief			Main initialization and configuration function for HMControl.
 */
void HourMinControl::InitUI( BRect bounds, BString label )
{
	/* Creating view for the label */
	labelView = new BStringView( BRect(0, 0, 1, 1),
								 "label",
								 label.String() );
	if ( !labelView ) {
		/* Panic! */
		exit(1);
	}
	labelView->ResizeToPreferred();
	
	/* Initializing the menubar */
	if ( !this->CreateMenuBar() )
	{
		/* Panic! */
		exit(1);
	}
	
	/* Initialized the checkbox */
	this->CreateCheckBox();
	
	/* Laying all views out */
	BGroupLayout* lay = new BGroupLayout(B_HORIZONTAL);
	
	if (!lay) { 
		// Panic! 
		exit(1); 
	}
	lay->SetInsets(0, 0, 0, 0);
	lay->SetSpacing(10);
	BView::SetLayout(lay);
	
	BLayoutItem *layoutItem;
	BSize size;
	
	layoutItem = lay->AddView(labelView);
	size.SetWidth( layoutItem->Bounds().Width() );
	size.SetHeight( layoutItem->Bounds().Height() );
	layoutItem->SetExplicitPreferredSize(size);
	layoutItem->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	
	layoutItem = lay->AddView( chooserMenuBar );
	layoutItem->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	
	if ( this->fCheckBox )
	{
		layoutItem = lay->AddView( fCheckBox );
		size.SetWidth( layoutItem->Frame().Width() );
		size.SetHeight( layoutItem->Frame().Height() );
		layoutItem->SetExplicitPreferredSize(size);
		layoutItem->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	}
	
//	size.SetHeight(size.Height());
//	layoutItem->SetExplicitMaxSize(size);	
}

HourMinControl::~HourMinControl()
{
	if ( chooserMenuBar ) {
		this->RemoveChild( chooserMenuBar );
		delete chooserMenuBar;
	}
	
	if ( this->fCheckBox ) {
		this->RemoveChild( fCheckBox );
		delete fCheckBox;
	}
	
	if ( this->labelView ) {
		this->RemoveChild( labelView );
		delete labelView;
	}
}	/* <-- end of destructor for hours and minutes control */

/*!	
 *	\brief		This function initializes the hours menu.
 *	\details	If user's preference is to use 24-hours clock, the items
 *				will be laid in matrix (two columns of 12 items each).
 *				Else, the items will be laid in column.
 */
BMenu* HourMinControl::CreateHoursMenu()
{
	BMessage* toSend = NULL;
	BMenuItem*	toAdd = NULL;
	BString sb;
	
	if ( representingTime )
	{
		// We should build real 24-h menu.
		return CreateAMHoursMenu( NULL );		
	}
	else
	{
		// The menu to be created is a static and limited by "hoursLimit".
		if ( hoursLimit < 12 ) {
			BMenu* toReturn = new BMenu( "HoursMenu", B_ITEMS_IN_COLUMN );
		}
		else
		{
			BMenu* toReturn = new BMenu( "HoursMenu", B_ITEMS_IN_MATRIX );
		}
		if ( ! toReturn ) {
			/* Panic! */
			exit( 1 );
		}
		toReturn->SetLabelFromMarked( true );
		toReturn->SetRadioMode( true );
		
		if ( hoursLimit < 12 ) {
			for ( uint32 i = 0; i <= hoursLimit; ++i )	
			{
				toSend = new BMessage( kHoursUpdated );
				if ( !toSend ) {
					/* Panic! */
					exit( 1 );
				}
				toSend->AddInt32( "SelectedHour", ( int32 )i );
				
				sb.Truncate( 0 );
				sb << i;
				toAdd = new BMenuItem( sb.String(),
											  toSend );
				if ( ! toAdd )	{
					/* Panic! */
					exit( 1 );						
				}
				if ( i == representedTime.tm_hour ) {
					toAdd->SetMarked( true );
				}
				toReturn->AddItem( toAdd );
			}
		}
		else
		{
			/* Build a matrix menu */
			BuildMatrixMenu( toReturn );
		}

		return toReturn;	
	}
}	/* <-- end of function HourMinControl::CreateHoursMenu */



/*!	\brief					Builds a matrix menu according to time limit selected.
 *		\param[out]	toEdit	The menu to be filled.
 *		\note						Assumptions:
 *									It is assumed the menu is already allocated and empty.
 *									If the menu is not in matrix form, the function will fail.
 */
void		HourMinControl::BuildMatrixMenu( BMenu* toEdit )
{
	if ( !toEdit ) { return; }
	
	BMessage* toSend;
	BMenuItem* toAdd;
	int horizontalSpacing = 5;
	int verticalSpacing = 2;
	int tempVar = 0;
	BPoint	topLeftCorner( horizontalSpacing, verticalSpacing );
	BString 	sb;
	BRect		itemFrame;
	
	// What is the maximal length of the item?
	sb << hoursLimit;
	tempVar = sb.CountChars();
	if ( tempVar == 0 ) { tempVar = 1; }
	sb.Truncate( 0 );
	for ( int i = 0; i < tempVar; ++i ) {
		sb << 'W';		// "W" is the widest char in latin alphabet
	}
	int itemWidth = ( int )font.StringWidth( sb.String() );
	
	// What is height of every single item?
	BFont font( be_plain_font );
	font_height fh;
	font.GetHeight( &fh );
	int itemHeight = ( int )( fh.ascent + fh.descent + fh.leading ) + 2;
	
	/* Preparations before the main loop */	
	itemFrame.top = 0;
	itemFrame.bottom = itemHeight;
	itemFrame.left = 0;
	itemFrame.right = itemWidth;
	
	itemFrame.SetLeftTop( topLeftCorner );
	
	for ( uint32 i = 0; i <= hoursLimit; ++i ) {
		
		// Message to be sent
		toSend = new BMessage( kHoursUpdated );
		if ( !toSend ) {
			/* Panic! */
			exit( 1 );
		}
		toSend->AddInt32( "SelectedHour", ( int32 )i );
		
		// Label of the item
		sb.Truncate( 0 );
		sb << i;
		
		// Item to be added		
		toAdd = new BMenuItem( sb.String(), toSend );
		if ( !toAdd ) {
			/* Panic! */
			exit( 1 );
		}
		if ( i == representedTime.tm_hour ) {
			toAdd->SetMarked( true );
		}		
		
		// Add the item to the menu
		toEdit->AddItem( toAdd, itemFrame );
		
		// Moving to the next frame
		if ( ( i+1 ) % 12 == 0 ) {
			// Advancing one column right
			topLeftCorner.x += itemWidth + horizontalSpacing;
			topLeftCorner.y = verticalSpacing;
		}
		else
		{
			// Descending one item down
			topLeftCorner.y += itemHeight + verticalSpacing;
		}
		itemFrame.SetLeftTop( topLeftCorner );
	}
	
}	// <-- end of function HourMinControl::BuildMatrixMenu



/*!	
 *	\brief		Receive the message with preferences and parse it.
 *	\details	The class HourMinControl uses the following preferences:
 *				a) is the clock 24-hours or 12-hours
 *				b) and c) what are the hours and minutes limits
 *				d) if the represented time is negative.
 *	\param[in] preferences	The message with preferences.
 */
void HourMinControl::ParsePreferences(const BMessage* preferences )
{
	if ( !preferences ||
		  preferences->FindBool("TwentyFourHoursClock", &fTwentyFourHoursClock) != B_OK )
	{
		this->fTwentyFourHoursClock = false;
	}
	if ( !preferences ||
		  preferences->FindInt8("HoursLimit", (int8*)&hoursLimit) 	  != B_OK )
	{
		if ( fTwentyFourHoursClock ) {
			this->hoursLimit = 23;
		} else {
			this->hoursLimit = 11;
		}
	}
	if ( !preferences ||
		  preferences->FindInt8("MinutesLimit", (int8*)&minutesLimit) != B_OK )
	{
		this->minutesLimit = 55;
	}
	
	if ( !preferences ||
		  preferences->FindBool("NegativeTime", &negativeTime) != B_OK )
	{
		this->negativeTime = false;
	}
	
}	// <-- end of function HourMinControl::ParsePreferences



/*!	
 *	\brief		Create the hours menu corresponding to AM / PM differentiation
 *	\param[in]	pbPM	Pointer to boolean. It allows to pass three values:
 *						"true", "false" and NULL. If NULL (default), the menu
 *						to be built is 24-hours. If "false", the menu represents
 *						AM. If "true", the menu represents PM.
 */
BMenu* HourMinControl::CreateAMHoursMenu( bool *pbPM )
{
	BMenu*	toReturn;
	BString	hourLabel;	
	BMenuItem* toAdd;
	BMessage* toSend;
	
	
	if ( !pbPM )	// 24-hours menu
	{
		BRect tempRect;
		BSize	size;		// Size of any individual item
		BPoint	topLeftCorner( SPACING, SPACING );

		hourLabel.Truncate(0);
//		hourLabel << "00";	// Take the widest character to calculate needed size
		
		// Prepare the items required to calculate the menu's size
		BFont  	plainFont(be_plain_font);
		font_height	fontHeightStruct;
		
		hourLabel = "00";	// Take the widest character to calculate needed size
		
			// Getting the required height of the single item.
		plainFont.GetHeight( &fontHeightStruct );		
		size.SetHeight( plainFont.Size() + 
//						fontHeightStruct.ascent + 
//						fontHeightStruct.descent +
						fontHeightStruct.leading + SPACING );
			
		size.SetWidth( plainFont.StringWidth( hourLabel.String() ) + SPACING );
		
		toReturn = new BMenu("HoursMenu", 
							 size.Width()*2 + SPACING + 20,
							 size.Height()*12 + SPACING*13 );
		if (!toReturn ) { /* Panic! */ exit(1); }
		toReturn->SetRadioMode( true );
		toReturn->SetLabelFromMarked( true );
		
		/*================================================
		 * In this loop I'm adding the hours to the menu.
		 *===============================================*/
		
		// There are 12 rows, starting at 0 and finishing at 11
		for ( int index = 0; index < 12; index++ )
		{
			topLeftCorner.x = 10;	// From left corner to leftmost item
			
			// Left and right halves of the menu, "right" is "left + 12".
			for ( int parity = 0; parity < 2; parity++ )
			{
				//! This variable is the actual hour. It's 0 to 11 if parity is 0
				//	and 12 to 23 if parity is 1.
				int hourVariable = index + ( parity * 12);
				
				// Create the label
				hourLabel.Truncate( 0 );
				if ( hourVariable < 10 ) {
					hourLabel << ' ' << hourVariable;
				} else {
					hourLabel << hourVariable;
				}
				
				// Create the message to be sent
				toSend = new BMessage( kHourUpdated );
				if ( !toSend ) { /* Panic! */ exit(1); }
				toSend->AddInt8( "Hour", hourVariable );				
				
				// Create the menu item
				toAdd = new BMenuItem( hourLabel.String(), toSend );
				if ( !toAdd ) { /* Panic! */ exit(1); }
				
				if ( hourVariable == this->representedTime.tm_hour ) {
					toAdd->SetMarked( true );
				}
								
				// Add the menu item to the menu
				toReturn->AddItem( toAdd,
								   BRect( topLeftCorner, size ) );
				
				topLeftCorner.x += size.Width() + 10;	// SPACING
			}	// <-- end of "left and right halves"
			topLeftCorner.y += size.Height() + SPACING;
		}	// <-- end of "0 to 11 rows"
	}	// <-- end of "if (the required menu is for 24H clock)"
	
	else	// 12-hours clock
	{
		/* The solution is building 12-rows menu from 0 or 12 through 1 to 11. */
		
		// Prepare the menu
		toReturn = new BMenu( "HoursMenu", B_ITEMS_IN_COLUMN );
		if ( !toReturn ) { /* Panic! */ exit(1); }
		toReturn->SetRadioMode( true );
		toReturn->SetLabelFromMarked( true );
		
		for ( int hourVariable = 0; hourVariable <= 11; hourVariable++ )
		{
			// Prepare the label
			hourLabel.Truncate( 0 );	// Clear the label
			if ( *pbPM && hourVariable == 0 )
			{
				hourLabel << "12";
			} else if ( hourVariable < 10 ) {
				hourLabel << ' ' << hourVariable;
			} else {
				hourLabel << hourVariable;
			}				
			
			// Prepare the message
			toSend = new BMessage( kHourUpdated );
			if ( ! toSend ) { 	/* Panic! */ exit (1); }
			toSend->AddInt8( "Hour", *pbPM ? hourVariable + 12 : hourVariable );			
			
			// Prepare the menu item
			toAdd = new BMenuItem( hourLabel.String(), toSend );
			if ( ! toAdd ) { /* Panic! */ exit(1); }
			if ( ( *pbPM &&
				   ( representedTime.tm_hour == hourVariable + 12 ) ) ||
				 ( ! *pbPM &&
				   ( representedTime.tm_hour == hourVariable ) ) )
			{
				toAdd->SetMarked( true );
			}
			
			toReturn->AddItem( toAdd );
			
		}	// <-- end of "for (hours from 0 to 11)"
		
	}	// <-- end of 12-hours clock
	
	return toReturn;
	
}	// <-- end of function "HourMinControl::CreateAMHoursMenu"



/*!	
 *	\brief		Create the menu with minutes.
 */
BMenu* HourMinControl::CreateMinutesMenu()
{
	BMenu* toReturn = new BMenu("MinutesMenu", B_ITEMS_IN_COLUMN);
	if (!toReturn) {
		/* Panic! */
		exit(1);
	}
	BString itemLabel;
	BMenuItem *toAdd = NULL;
	BMessage  *toSend = NULL;
	int minLimit = ( minutesLimit < 55 ) ? minutesLimit : 55;
	toReturn->SetRadioMode( true );
	toReturn->SetLabelFromMarked( true );
	for ( int mins = 0; mins <= minLimit; mins += 5 )
	{
		itemLabel.Truncate(0);
		if ( mins < 10 )
		{
			itemLabel << " " << mins;
		} else {
			itemLabel << mins;
		}
		
		toSend = new BMessage( kMinuteUpdated );
		if ( !toSend )
		{
			/* Panic! */
			exit(1);
		} 
		
		toSend->AddInt8("Minutes", mins);
		
		toAdd = new BMenuItem( itemLabel.String(), toSend );
		if ( ! toAdd ) {
			/* Panic! */
			exit(1);
		}		
		
		toReturn->AddItem( toAdd );
		if ( mins == ( (int)(this->representedTime.tm_min / 5 )*5 ) )
		{
			toAdd->SetMarked( true );
		}
		
	}	/* <-- end of "for (minutes from 0 to the limit)" */
	
	return toReturn;	
}	/* <-- end of function HourMinControl::CreateMinutesMenu */

/*!	
 *	\brief		Creates and initializes the menus that manage hours and minutes.
 *	\attention	This function assumes that Control's label is already set.
 */
BMenuBar*	HourMinControl::CreateMenuBar( void )
{
	BRect frame = this->Bounds();
	BMenuItem* menuItem = NULL;
	BFont plainFont(be_plain_font);
	/* It's assumed the "label" variable is already set */
	frame.left += plainFont.StringWidth( label.String() ) + SPACING;
	frame.top += SPACING;
	frame.right -= SPACING;
	frame.bottom -= SPACING;
	chooserMenuBar = new BMenuBar(frame, 
								  "TimeControl menubar",
								  B_FOLLOW_LEFT | B_FOLLOW_TOP,
								  B_ITEMS_IN_ROW,
								  false );
	if (!chooserMenuBar) {
		/* Panic! */
		exit(1);
	}
	
	/* Hours menu */
	bool  bPM, *pbPM = &bPM;
	
	if ( fTwentyFourHoursClock ) {
		pbPM = NULL;
	} else if ( representedTime.tm_hour >= 12 ) {
		bPM = true;
	} else {
		bPM = false;
	}
	this->hoursMenu = CreateAMHoursMenu( pbPM );
	if ( !hoursMenu ) {
		/* Panic! */
		exit(1);
	}
	hoursMenu->SetRadioMode( true );
	chooserMenuBar->AddItem( hoursMenu );
	
	/* Separator - it's disabled symbol ":" */
	menuItem = new BMenuItem( ":", NULL );
	if ( !menuItem ) {
		/* Panic! */
		exit(1);
	}
	menuItem->SetEnabled( false );
	chooserMenuBar->AddItem( menuItem );
	
	/* Minutes menu */
	this->minutesMenu = CreateMinutesMenu();
	if ( !minutesMenu ) {
		/* Panic! */
		exit(1);
	}
	minutesMenu->SetRadioMode( true );
	chooserMenuBar->AddItem( minutesMenu );
	
	return( chooserMenuBar );
	
}	/* <-- end of function HourMinControl::CreateMenuBar */


/*!	\brief		Main function of the control.
 */
void HourMinControl::MessageReceived( BMessage* in ) {
	int temp = 0;
	int8 cTempVar = 0;	/* "c" is for "char" */
	uint32 command = in->what;
	
	switch( command )
	{
		case kHourUpdated:
			if ( B_OK != in->FindInt8("Hour", &cTempVar ) ) {
				/* error */
				return;	
			}
			this->representedTime.tm_hour = cTempVar;
			TogglePM( NULL );	// Update the PM checkbox
			break;
		
		case kMinuteUpdated:
			if ( B_OK != in->FindInt8("Minutes", &cTempVar ) ) {
				/* error */
				return;
			}
			this->representedTime.tm_min = cTempVar;
			break;
			
		case kPMToggled:
			
			if ( representedTime.tm_hour >= 12 ) {
				representedTime.tm_hour -= 12;
			} else {
				representedTime.tm_hour += 12;
			}
			if ( chooserMenuBar )
			{
				chooserMenuBar->RemoveItem( hoursMenu );
				delete hoursMenu;
				hoursMenu = NULL;
					/* Hours menu */
				bool  bPM, *pbPM = &bPM;
				
				if ( fTwentyFourHoursClock ) {
					pbPM = NULL;
				} else if ( representedTime.tm_hour >= 12 ) {
					bPM = true;
				} else {
					bPM = false;
				}
				
				this->hoursMenu = CreateAMHoursMenu( pbPM );
				if ( !hoursMenu ) {
					/* Panic! */
					exit(1);
				}
				hoursMenu->SetRadioMode( true );
				hoursMenu->SetLabelFromMarked( true );
				chooserMenuBar->AddItem( hoursMenu, 0 );
			}
			break;
		
		case kPreferences:
			this->ParsePreferences( in );
			break;
		
		default:
			BView::MessageReceived( in );
			break;
	};
	
	return;
}	/* <-- end of function HourMinControl::MessageReceived */


/*!	\brief			Executed when the control is attached to the window.
 */
void HourMinControl::AttachedToWindow() {
	// Get the view color of the father
	if (Parent()) {
		SetViewColor(Parent()->ViewColor());
	}
	// Attach to window both current view and all of its children
	BView::AttachedToWindow();
	
	// This view should respond to the messages - thus the Looper must know it
	BLooper* looper = (BLooper*)Looper();
	if (looper && looper->LockLooper()) {
		looper->AddHandler((BHandler*) this);
		looper->UnlockLooper();
	}
	
	// Update targets of all children
	BMenu* men; BMenuItem* item;
	if ( chooserMenuBar ) {
		for (int i=0; i<chooserMenuBar->CountItems(); i++) {
			if ( (men = chooserMenuBar->SubmenuAt(i) ) != NULL ) {
				men->SetTargetForItems(this);	
			} else {
				if ( (item = chooserMenuBar->ItemAt(i)) != NULL ) {
					item->SetTarget(this);	
				}	
			}
		}
	}
	
	if ( PMCheckBox ) {
		PMCheckBox->SetTarget( this );
	}
} // <-- end of function "HourMinControl::AttachedToWindow"



/*!	\brief		Creates and initializes the checkbox.
 */
BCheckBox* HourMinControl::CreateCheckBox( void )
{
	if ( !this->hasCheckBox ) {
		return NULL;
	}
	
	BRect frame(0, 0, 1, 1);
	BMessage* message = new BMessage( kCheckBoxToggled );
	if ( ! message )
	{
		exit(1);	/* Panic! */
	}
	fCheckBox = new BCheckBox( frame,
										"Checkbox Control",
										fCheckBoxLabel.String(),
										message );
	if ( !fCheckBox )
	{
		/* Panic! */
		exit(1);
	}
	
	fCheckBox->ResizeToPreferred();
	
	if (  representingTime && fTwentyFourHoursClock )
	{
		PMCheckBox->SetEnabled( false );
	} else {
		PMCheckBox->SetEnabled( true );
	}
	
	fCheckBox->SetTarget( this );
	
	return ( fCheckBox );	
}

/*!	
 *	\brief		Toggle the boolean "PM" and the PM checkbox correspondingly.
 *	\param[in]	toSet	Pointer to boolean value. Since it's a pointer,
 *					   	three values can be passed: "true", "false" or "NULL".
 *					   	in case of "NULL", correct value of the boolean and
 *						the need to check the checkbox is defined automatically.
 *						If "true", both are set; if "false", both are reset.
 */
void HourMinControl::TogglePM( bool *toSet )
{
	if ( ! toSet )		// NULL
	{
		if ( representedTime.tm_hour > 12 )
		{
			PMCheckBox->SetValue( 1 );
		} else {
			PMCheckBox->SetValue( 0 );
		}
	}
	else if ( *toSet )	// TRUE
	{
		if ( representedTime.tm_hour < 12 )
		{
			representedTime.tm_hour += 12;
		}
		PMCheckBox->SetValue( 1 );
	} else {			// FALSE
		if ( representedTime.tm_hour >= 12 )
		{
			representedTime.tm_hour -= 12;
		}
		PMCheckBox->SetValue( 0 );
	}
}	// <-- end of function HourMinControl::TogglePM
