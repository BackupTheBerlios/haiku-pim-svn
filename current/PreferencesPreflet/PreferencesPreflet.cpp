/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include "PreferencesPreflet.h"

#include <Box.h>
#include <CheckBox.h>
#include <GridLayout.h>
#include <Layout.h>
#include <LayoutItem.h>
#include <ListView.h>
#include <ListViewItem.h>
#include <Message.h>
#include <MenuItem.h>
#include <String.h>



/****************************************************************************
 * 		Implementation of class CalendarModulePreferencesView
 ***************************************************************************/

/*!	\function 		CalendarModulePreferencesView::CalendarModulePreferencesView
 *	\brief			Default constructor
 *	\param[in]	frame	The rectangle enclosing the view
 *	\param[in]	name	Name of the view. Will be passed to BView's constructor.
 *
 */
CalendarModulePreferencesView::CalendarModulePreferencesView( BRect frame,
															  const char* name,
															  BString  )
	:
	BTab(frame, 
		 name,
		 B_FOLLOW_LEFT | B_FOLLOW_TOP,
		 B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS )
{
	BMenuItem* toAdd = NULL;
	BMessage*  toSend = NULL;
	CalendarModule* currentCalModule = NULL;
	BRect tempFrame( frame );	// Got the boundaries
	
	
	
	
}

/*!	\function		CalendarModulePreferencesView::PopulateModulesMenu
 *	\brief			Create a menu to select the module user is setting up.
 *	\note			This is an internal function.
 */
BPopUpMenu*		CalendarModulePreferencesView::PopulateModulesMenu( void )
{
	BPopUpMenu* toReturn = NULL;
	BMenuItem* 	toAdd = NULL;
	BMessage*  	toSend = NULL;
	BString		moduleId;	// Note: this is not a pointer!
	CalendarModule* currentCalModule = NULL;
	
	toReturn = new BPopUpMenu("Calendar Modules");
	if ( ! toReturn )
	{ 
		/* Panic! */
		exit (1);
	}
	
	// Looping on all calendar modules available in the system.
	for ( uint i=0; i < NUMBER_OF_CALENDAR_MODULES; ++i )
	{
		// Obtain the next calendar module
		currentCalModule = (CalendarModule *)listOfCalendarModules->ItemAt( i );
		if ( !currentCalendarModule )
		{
			continue;	// Skipping unavailable ones
		}
		
		// Create the message
		toSend = new BMessage( kCalendarModuleChosen );
		if ( !toSend )
		{
			/* Panic! */
			exit(1);
		}
		
		// Add the ID of the module to the message
		moduleId = currentCalModule->Identify();
		toSend->AddString( "Module ID", moduleId );
		
		// Create the menu item
		toAdd = new BMenuItem( moduleId.String(), toSend );
		if ( ! toAdd )
		{
			/* Panic! */
			exit(1);
		}
		
		// Add the newly created item to the menu
		toReturn->AddItem( toAdd );
		
		// Gregorian calendar module is the default one
		if ( moduleId == "Gregorian" )
		{
			toAdd->SetMarked( true );
		}
		
	}	/* <-- end of "for ( every calendar module )" */
	
	return toReturn;
}	// <-- end of function "CalendarModulePreferencesView::PopulateModulesMenu"

/*!	\function		CalendarModulePreferences::CreateWeekendSelectionBox
 *	\brief			Create box for selection of the weekend days
 *	\note			Additionally, select the color for weekends and weekdays
 *	\param[in]	frame	Enclosing rectangle.
 *	\param[in]	label	Reference to label of the enclosing BBox.
 *	\param[in]	calModule	The calendar module for which the preferences are set.
 */
void	CalendarModulePreferences::CreateWeekendSelectionBox( BRect frame,
															  BString &label,
															  CalendarModule* calModule )
{
	/*!	\par	Notes on implementation:
	 *			It's not all that straightforward - to create this selection box.
	 *			The problem is that number of days in week is dependent on the
	 *			Calendar Module, therefore the frame rectangle must be divided
	 *			properly. We should take into account the possibility that there's
	 *			not enough place for all days in the submitted frame.
	 *
	 *	\par	
	 *			The solution will be as follows:
	 *			Let number of days in week be N. I create two columns and 
	 *			several rows (the number depends on N). Days in week will be
	 *			proceeded in the order <em>as Calendar Module supplies them</em>.
	 *			The days occupy both columns, and are located in rows
	 *			[0, (ceiling of (N/2)) ). Days returned from CalendarModule are
	 *			placed as follows: days from 0 to (ceiling of (N/2)-1) in the left
	 *			column, days from (ceiling of (N/2)-1) to (N-1) in right column.
	 *
	 *	\par	
	 *			Two controls for setting the colors of weekdays and weekends
	 *			are located at the bottom row of the grid; weekdays on right,
	 *			weekends on left.
	 *
	 *	\par	
	 *			There will be an empty cell in the right column, if number
	 *			of days in week is odd.
	 */
	BMessage* 	toSend = NULL;
	BCheckBox* 	dayCheckBox = NULL;
	BString		tempString;
	
	/* indexX is 0 for left column or 1 for right column.
	 * indexY is 0 for topmost row, 1 for second from top row, etc.
	 * Max value for indexY = (ceiling of (N/2)).
	 */
	int indexX, indexY;
	 
	// Prepare the item to be returned
	BBox*	enclosingBox = new BBox( frame, "Weekend selector" );
	if ( !enclosingBox )
	{
		/* Panic! */
		exit(1);
	}

	// Prepare the layout to be used
	BGridLayout* layout = new BGridLayout();
	if ( !layout)
	{
		/* Panic! */
		exit(1);
	}	
	enclosingBox->SetLayout( layout );
	
	// Get the data on days of week
	unsigned int daysInWeek = (unsigned int)( calModule->GetDaysInWeek() );
	map<uint32, DoubleNames> weekdayNames = calModule->GetWeekdayNames();
	
	indexX = indexY = 0;
	for (unsigned char day = 0; day < daysInWeek; ++day )
	{
		/* Creating the message to be sent */
		toSend = new BMessage( kCalendarModuleWeekendDaySelected );
		if ( !toSend )
		{
			/* Panic! */
			exit(1);
		}
		toSend->AddInt32( "Weekday no", day );

		/* Obtain the name of the day. I use the long name. */
		tempString = (DoubleNames)weekdayNames[ day ].longName;
		
		/* Creating the checkbox */
		dayCheckBox = new BCheckBox( BRect(0, 0, 1, 1),
									 "Weekday",
									 tempString.String(),
									 toSend );
		if (!dayCheckBox)
		{
			// Panic!
			exit(1);
		}
		
		dayCheckBox->ResizeToPreferred();
		
		/* Adding the item to the BBox */
		layout->AddView( dayCheckBox, indexX, indexY );
		
		/* Advancing to the next cell in grid */
		// If arrived to the last item in the first column, advancing to second
		if ( day == ( int )( ( daysInWeek + 1 ) / 2 ) )
		{
			indexX = 1;	
			indexY = 0;
		}
		else 	// Staying in the same column, but advancing down
		{
			++indexY;	
		}
		
	}	// <-- end of "for (all days in week)"

	BListView* weekdayColorSelector = NULL;	
	
	
	
	
}


	calendarModuleSelector = new BMenuField( BRect(0, 0, 1, 1),
											 "CalModule Prefs",
											 "Calendar module:",
											 
											 ;
	
	
}
