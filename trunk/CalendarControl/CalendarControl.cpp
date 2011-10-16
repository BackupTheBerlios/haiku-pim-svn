
// OS includes
#include <InterfaceKit.h>
#include <OS.h>
#include <String.h>
#include <Size.h>
#include <Application.h>
#include <Invoker.h>
#include <Layout.h>
#include <LayoutUtils.h>
#include <SpaceLayoutItem.h>
#include <GridLayout.h>

// POSIX includes
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Project includes
#include "CalendarControl.h"
#include "DayItem.h"
#include "Preferences.h"
#include "Utilities.h"

#define 	BUTTON_WIDTH	30
#define		SPACING			4

	/*!	\details	Defines number of years before and after selected year in the 
	 *					years menu */
#define		YEARS_UP_AND_DOWN	5

rgb_color weekdayNameColor = {0x80, 0x80, 0x80, 255};
rgb_color weekendNameColor = {255, 0x80, 0x80, 255};
rgb_color weekendDateColor = {255, 0x0, 0x0, 255};
rgb_color todayBackColor = {0, 0, 0x80, 255};



/*==========================================================================
**			IMPLEMENTATION OF CLASS MonthMenu
**========================================================================*/
MonthMenu::MonthMenu(const char *name, float width, float height)
	:
	BMenu(name, width, height)
{
	
}

MonthMenu::~MonthMenu() {}


void MonthMenu::MouseDown(BPoint where) {
	BMenu::MouseDown(where);
/*
	int i=0, limit=this->CountItems();
	BMenuItem* item = NULL;
	DayItem* dayItem = NULL;
	for (; i<limit; ++i) {
		item = this->ItemAt(i);
		if (item->Frame().Contains(where)) {
			break;
		}		
	}
	
	if ( (item) && (dayItem = dynamic_cast<DayItem*>(item))) {
		if (dayItem->IsServiceItem()) {
			bool en = dayItem->IsEnabled();
			dayItem->SetEnabled(false);
//			BMenu::MouseDown(where);			
			dayItem->SetEnabled(en);
			dayItem->Fire();
			Track(true);
			return;
		}
	} else {
		BMenu::MouseDown(where);
	}
*/
	return;
}



/*==========================================================================
**			IMPLEMENTATION OF CLASS CalendarControl
**========================================================================*/

/*!	
 *	\brief			The constructor of CalendarControl.
 *	\param[in]	frame			The outer bounds of the control.
 *	\param[in]	name			Name of the control.
 *	\param[in]	labelInForCalendar		The label of the calendar part of control.
 *	\param[in]	calModuleName				ID of the calendar module used.
 *	\param[in]	initialTime					The initial time to be set, passed in seconds.
 *													If 0, current time is set.
 *	\attention		It's assumed that calendar modules were already initialized prior
 *						to calling this function.
 */
CalendarControl::CalendarControl(BRect frame,
								 const char* name,
								 const BString& labelInForCalendar,
								 const BString& calModuleName,
								 time_t	initialTime )
	:
	BView( frame, name, 
			 B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
			 B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS ),
	fLabel(NULL),
	fDateLabel(NULL),
	bIsControlEnabled(true),
	fLastError( B_OK )
{
	// Firstly, set the calendar module
	this->fCalModule = utl_FindCalendarModule( calModuleName );
	if ( this->fCalModule == NULL ) {
		fLastError = B_BAD_VALUE;
		return;
	}
	
	// Load the preferences of this calendar module
	ParsePreferences();
	
	// Init the internal data to submitted time representation
	InitTimeRepresentation( initialTime );
	
	// Create the UI elements
	fLabel = new BStringView( BRect( 0, 0, 1, 1 ), 
									  "label", 
									  labelInForCalendar.String() );
	if ( !fLabel) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fLabel->ResizeToPreferred();
	
	fDateLabel = new BStringView( BRect( 0, 0, 1, 1 ), 
											"dateLabel", 
											NULL );
	if ( !fDateLabel ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	
	// Create the menu
	CreateMenu();
	
	BRect stringViewFrame = fLabel->Frame();
	
	BPoint topLeftCorner = stringViewFrame.RightTop();
	BSize size( BUTTON_WIDTH, stringViewFrame.Height() + SPACING );
	
	
	fMenuBar = new BMenuBar( BRect(topLeftCorner, size),
								"menuBar",
								B_FOLLOW_RIGHT | B_FOLLOW_TOP,
								B_ITEMS_IN_ROW,
								false);
	if (! fMenuBar) {
		// Panic!
		fLastError = B_NO_MEMORY;
		return;
	}
	fMenuBar->SetBorder( B_BORDER_EACH_ITEM );
	fMenuBar->AddItem( fDateSelector );

	// Update the selected date label to currently selected moment of time
	UpdateText();
	
	// Initializing the layout
	BGridLayout* lay = new BGridLayout( );
	
	if (!lay) { 
		// Panic! 
		fLastError = B_NO_MEMORY;
		return; 
	}
	lay->SetInsets(0, 0, 0, 0);
	lay->SetSpacing( 10, 5 );
	BView::SetLayout(lay);
	
	BLayoutItem* layoutItem;
	BSize size1;
	
	layoutItem = lay->AddView( fLabel, 0, 0 );
	size1.SetWidth( layoutItem->Frame().Width() );
	size1.SetHeight( layoutItem->Frame().Height() );
	layoutItem->SetExplicitPreferredSize( size1 );
	layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT, B_ALIGN_TOP ) );
	layoutItem = lay->AddView( fDateLabel, 1, 0 );
	layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT, B_ALIGN_TOP ) );
	layoutItem = lay->AddView( fMenuBar, 2, 0 );	
	layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_RIGHT, B_ALIGN_TOP ) );
	size.SetHeight( size.Height() );
	layoutItem->SetExplicitMaxSize( size );
	
	fLastError = B_OK;
}

/*!	\brief			Parses BMessage with preferences.
 *		\attention		It is assumed that preferences were already read.
 */
void CalendarControl::ParsePreferences( )
{
	CalendarModulePreferences* prefs = pref_GetPreferencesForCalendarModule( fCalModule->Identify() );
	
	if ( !prefs )
	{
		utl_Deb = new DebuggerPrintout( "Didn't succeed to read the preferences, using defaults." );
		this->fWeekends = *( fCalModule->GetDefaultWeekend() );
		this->fFirstDayOfEveryWeek = fCalModule->GetDefaultStartingDayOfWeek();
		this->fColorForWeekends.set_to( 255, 0, 0, 255 );	// Solid red
		this->fColorForWeekdays.set_to( 0, 0, 0, 255 );		// Solid black
		this->fColorForServiceItems.set_to( 0, 0, 128, 255 );	// Dark blue
		this->fDateOrder = kDayMonthYear;
	}
	else
	{
		this->fWeekends = *prefs->GetWeekends();
		this->fFirstDayOfEveryWeek = prefs->GetFirstDayOfWeek();
		this->fColorForWeekends = prefs->GetWeekendsColor( false );
		this->fColorForWeekdays = prefs->GetWeekdaysColor( false );
		this->fColorForServiceItems = prefs->GetServiceItemsColor( false );
		this->fDateOrder = prefs->GetDayMonthYearOrder();
	}
}	// <-- end of function CalendarControl::ParsePreferences



/*!	
	\brief			Sets up the view color and calls this view's children.
*/
void CalendarControl::AttachedToWindow() {
	
	// Get the view color of the father
	if ( Parent() ) {
		SetViewColor( Parent()->ViewColor() );
	}
	// Attach to window both current view and all of its children
	BView::AttachedToWindow();
	
	// This view should respond to the messages - thus the Looper must know it
	BLooper* looper = ( BLooper* )Looper();
	if ( looper && looper->LockLooper() ) {
		looper->AddHandler( ( BHandler* )this );
		looper->UnlockLooper();
	}
	
	/* Update targets of all children
	BMenu* men; BMenuItem* item;
	if (fDateSelector) {
		for (int i=0; i<fDateSelector->CountItems(); i++) {
			if ( (men = fDateSelector->SubmenuAt(i) ) != NULL ) {
				men->SetTargetForItems(this);	
			} else {
				if ( ( item = fDateSelector->ItemAt(i) ) != NULL ) {
					item->SetTarget(this);	
				}	
			}
		}	
	}
*/
	UpdateTargets( fDateSelector );
}
// <-- end of function CalendarControl::AttachedToWindow

 
/*!	\brief			Initializes the CalendarControl to default date, separator and order.
 *		\param[in]	initialSeconds		The moment of time this control initially represents
 *		\attention		It's assumed that calendar module is already set
 */
void CalendarControl::InitTimeRepresentation( time_t initialSeconds ) {
	if ( initialSeconds == 0 ) {
		initialSeconds = time( NULL );	// Obtain current tine
	}
	
	this->fRepresentedTime = fCalModule->FromTimeTToLocalCalendar( initialSeconds );
	this->fRepresentedTime.SetIsRepresentingRealDate( true );
	this->fRepresentedTime.SetCalendarModule( fCalModule->Identify() );

	CreateMenu();
}
// <-- end of function CalendarControl::Init

/*!	
 *	\brief			Redraws the text inside of the text control according to current settings.
 */
void CalendarControl::UpdateText() {
	BString builderLong, builderShort;
	float longStringLength, shortStringLength;
	BRect frame;

	if (!fCalModule) { return; }
	if (!fDateLabel) { /* Nothing to update! */ return; }
		
	// We need to calculate the place required for the date representation.
	BFont textViewFont(be_plain_font);
//	textView->GetFontAndColor(0, &textViewFont, NULL);
	builderLong = BuildDateRepresentationString( true );
	builderShort = BuildDateRepresentationString( false );
	longStringLength = textViewFont.StringWidth(builderLong.String());
	shortStringLength = textViewFont.StringWidth(builderShort.String());
	
	fDateLabel->SetText( builderLong.String() );
	this->InvalidateLayout();
	this->Invalidate();
}
// <-- end of function CalendarControl::UpdateText




/*!	
 *	\brief		Builds the string which represents the date.
 *	\details		The string is built according to order of representation
 *					stated in the object.
 *	\param[in]	useLongMonthNames	If "true", the longer version of the
 *									month name is used.
 *	\returns		The created BString object.
 */
BString CalendarControl::BuildDateRepresentationString( bool useLongMonthNames )
{
	map<int, BString> dayNames = fCalModule->GetDayNamesForLocalYearMonth(
			this->fRepresentedTime.tm_year,
			this->fRepresentedTime.tm_mon );
	map<int, DoubleNames> monthNames = fCalModule->GetMonthNamesForLocalYear(
			this->fRepresentedTime.tm_year );
			
	BString builder, day, month, year;
	
	day = dayNames[ this->fRepresentedTime.tm_mday ];
	
	year << this->fRepresentedTime.tm_year;
	
	if (useLongMonthNames) {
		month = ( ( DoubleNames )( monthNames[ this->fRepresentedTime.tm_mon ] ) ).longName;
	} else {
		month = ( ( DoubleNames )( monthNames[ this->fRepresentedTime.tm_mon ] ) ).shortName; 
	}
	
	switch ( this->fDateOrder ) {
		case ( kMonthDayYear ):
			// Month
			builder << month;
			builder << ' ';
			// Day
			builder << day;
			builder << ", ";
			// Year
			builder << year;
			break;
		case ( kYearMonthDay ):
			// Year
			builder << year;
			builder << ", ";
			// Month
			builder << month;
			builder << ' ';
			// Day
			builder << day;
			break;
		default:		// Intentional fall-through
		case ( kDayMonthYear ):
			// Day
			builder << day;
			builder << " of ";
			// Month
			builder << month;
			builder << ", ";
			// Year
			builder << year;
			break;
	};
	return builder;
}
// <-- end of function CalendarControl::BuildDateRepresentationString



/*!	
 *	\brief		Destructor of the CalendarControl.
 */
CalendarControl::~CalendarControl(void)
{
	if (!fMenuBar) {
		RemoveChild(fMenuBar);
		delete fMenuBar;
		fMenuBar = NULL;
	}
	if (!fLabel) {
		RemoveChild( fLabel );
		delete fLabel;
		fLabel = NULL;
	}
	if (!fDateLabel) {
		RemoveChild(fDateLabel);
		delete fDateLabel;
		fDateLabel = NULL;
	}
//	this->weekends.MakeEmpty();
}
// <-- end of destructor for the CalendarControl



/*!	\brief		This function creates and updates the BPopUpMenu.
 *		\details		The created menu is updated every time the TimeRepresentation
 *						changes.
 */
void CalendarControl::CreateMenu( void ) {

	// The whole menu will be created in fixed font.
	BFont fixedFont(be_fixed_font);
	BFont plainFont(be_plain_font);
	BRect rectangle;
	BPoint	topLeftCorner( 0, 0 );
	BSize	rectSize;
	BString sb;
	float widthOfTheWeekRows = 0;
	float widthOfFirstRow = 0;
	
	// Which month shall we represent?
	map<int, BString> dayNames = fCalModule->GetDayNamesForLocalYearMonth(
			this->fRepresentedTime.tm_year,
			this->fRepresentedTime.tm_mon);
	map<int, DoubleNames> monthNames = fCalModule->GetMonthNamesForLocalYear(
			this->fRepresentedTime.tm_year);			
	
	int daysInMonth = dayNames.size();
	int daysInWeek = ( int )fCalModule->GetDaysInWeek();
	
	// We need to determine the bounding rectangle for the menu.
	// For this, we need to obtain the maximum bounding rectangle for a string.
	font_height fontHeightStruct;
	fixedFont.GetHeight( &fontHeightStruct );
	float fixedFontHeightString = fontHeightStruct.ascent + 
											fontHeightStruct.descent +
											fontHeightStruct.leading + SPACING;
	plainFont.GetHeight( &fontHeightStruct );
	float plainFontHeightString = fontHeightStruct.ascent + 
											fontHeightStruct.descent +
											fontHeightStruct.leading + SPACING;
	// Now fixedFontHeightString is surely big enough to enclose every string in 
	// height. How many lines will we need? One for name of month and year,
	// one for weekday names, and several more for the dates themselves. At the
	// bottom, there is an additional line for "Return to today" option.
	
	
	// tempDay is a running date in current month. Each day item will be initialized
	// from the tempDay.
	TimeRepresentation tempDay( this->fRepresentedTime );
	tempDay.tm_mday = 1;
	
	int firstDayOfMonthWD = fCalModule->GetWeekDayForLocalDateAsInt( tempDay );
	int firstDayOfWeek = ( int )fFirstDayOfEveryWeek;
	
	int firstDayOfMonthInFirstWeek =
		(firstDayOfMonthWD + daysInWeek - firstDayOfWeek) % daysInWeek;
		
	float numberOfWeeksRequiredFL = 1 + 
		( ( float )( daysInMonth - ( daysInWeek - firstDayOfMonthInFirstWeek ) )
			/ daysInWeek );
	  
	int numberOfWeeksRequired = floorf( numberOfWeeksRequiredFL + 0.5 );
	
	// This is the menu we're adding items to.
	fDateSelector = new BMenu("⇩", B_ITEMS_IN_MATRIX );	
	// Sanity check
	if ( !fDateSelector )
	{
		// Panic!
		fLastError = B_NO_MEMORY;
		return;
	}
	
	fDateSelector->SetViewColor( ui_color( B_MENU_BACKGROUND_COLOR ) );
	fDateSelector->SetFont( &fixedFont );
	
	topLeftCorner.x = SPACING + 5;
	topLeftCorner.y = SPACING;	
	
	// Build the list of months.
	BPopUpMenu* listOfMonths = CreateMonthsMenu(monthNames);
	
	//-----------------------------------------------------
	// FIRST ROW.
	//-----------------------------------------------------
	
	/*----------------------------------------------------------------------------
	 *			Adding months menu with option to scroll forward and backward
	 *----------------------------------------------------------------------------*/
	
	// Add the item to scroll list of months back
	BMessage* messageOfItem = new BMessage( kMonthDecreased );
	DayItem* itemToAdd = new DayItem("‹", messageOfItem);
	if ( !itemToAdd ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	itemToAdd->SetServiceItem( true );
	itemToAdd->SetFrontColor( fColorForServiceItems );
	itemToAdd->SetBackColor( ui_color( B_MENU_BACKGROUND_COLOR ) );
	itemToAdd->SetEnabled( true );
	rectSize.SetHeight( fixedFontHeightString );
	rectSize.SetWidth( 25 );
//	rectSize.SetWidth( ( float )fixedFont.StringWidth("‹") + SPACING );
	fDateSelector->AddItem(	itemToAdd, 
									BRect( topLeftCorner, rectSize ) );
	itemToAdd->SetTarget( this );

	topLeftCorner.x += rectSize.Width() + SPACING;

	// Add the list of months
	BString longestMonth = monthNames[ 1 ].longName;
	for ( int i = 2; i < ( int )monthNames.size(); i++ )
	{
		if ( ( ( BString )( monthNames[ i ].longName ) ).Length() > longestMonth.Length() )
		{
			longestMonth = monthNames[i].longName;
		}	
	}
	rectSize.SetHeight( plainFontHeightString );
	rectSize.SetWidth( (float)plainFont.StringWidth( longestMonth.String() ) + 10 + SPACING );
	fDateSelector->AddItem( listOfMonths, 
									BRect(topLeftCorner, rectSize) );
	topLeftCorner.x += rectSize.Width() + SPACING;

	// Add the item to scroll list of months forward.
	messageOfItem = new BMessage( kMonthIncreased );
	if ( !messageOfItem ) {
		// Panic! 
		fLastError = B_NO_MEMORY;
		return;
	}		
	itemToAdd = new DayItem( "›", messageOfItem );
	if ( !itemToAdd ) {
		/* Panic! */ 
		fLastError = B_NO_MEMORY; 
		return;
	}
	itemToAdd->SetServiceItem(true);
	itemToAdd->SetFrontColor( fColorForServiceItems );
	itemToAdd->SetBackColor( ui_color( B_MENU_BACKGROUND_COLOR ) );
	itemToAdd->SetEnabled( true );
	rectSize.SetHeight( fixedFontHeightString );
	rectSize.SetWidth( 25 );
//	rectSize.SetWidth( ( float )fixedFont.StringWidth("›") + SPACING );
	fDateSelector->AddItem( itemToAdd, 
									BRect( topLeftCorner, rectSize ) );
	itemToAdd->SetTarget( this );

	topLeftCorner.x += rectSize.Width() + 10 + SPACING;


	/*----------------------------------------------------------------------------
	 *			Adding years menu with option to scroll forward and backward
	 *----------------------------------------------------------------------------*/

	// Add the item to scroll list of years down.
	messageOfItem = new BMessage( kYearDecreased );
	if ( !messageOfItem ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	itemToAdd = new DayItem( "‒", messageOfItem );
	if ( !itemToAdd ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	itemToAdd->SetServiceItem(true);
	itemToAdd->SetFrontColor( fColorForServiceItems );
	itemToAdd->SetBackColor( ui_color( B_MENU_BACKGROUND_COLOR ) );
	itemToAdd->SetEnabled( true );
	rectSize.SetHeight( fixedFontHeightString );
	rectSize.SetWidth( 25 );
//	rectSize.SetWidth( ( float )fixedFont.StringWidth("‒") + SPACING );
	fDateSelector->AddItem( itemToAdd, 
									BRect( topLeftCorner, rectSize ) );
	itemToAdd->SetTarget( this );
	
	topLeftCorner.x += rectSize.Width() + SPACING;
	
	// Add year
	sb.Truncate( 0 );
	sb << fRepresentedTime.tm_year;
	rectSize.SetHeight( plainFontHeightString );
	rectSize.SetWidth( ( float )plainFont.StringWidth( sb.String() ) + 10 + SPACING );
	BPopUpMenu* listOfYears = CreateYearsMenu(this->fRepresentedTime.tm_year);
	if ( !listOfYears ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fDateSelector->AddItem( listOfYears,
									BRect( topLeftCorner, rectSize ) );
	topLeftCorner.x += rectSize.Width() + SPACING;

	// Add item to scroll list of years up.
	messageOfItem = new BMessage( kYearIncreased );
	if ( !messageOfItem ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	itemToAdd = new DayItem( "+", messageOfItem );
	if ( !itemToAdd ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	itemToAdd->SetServiceItem(true);
	itemToAdd->SetFrontColor( fColorForServiceItems );
	itemToAdd->SetBackColor( ui_color( B_MENU_BACKGROUND_COLOR ) );
	itemToAdd->SetEnabled(true);
	rectSize.SetHeight( fixedFontHeightString );
	rectSize.SetWidth( 25 );
//	rectSize.SetWidth( ( float )fixedFont.StringWidth( "+" ) + SPACING );
	fDateSelector->AddItem( itemToAdd, 
									BRect( topLeftCorner, rectSize ) );

	//-----------------------------------------------------
	// SECOND ROW.	WEEKDAY NAMES
	//-----------------------------------------------------
	sb.Truncate( 0 );
	rectSize.SetHeight( fixedFontHeightString );
	sb << ( int )fCalModule->GetLongestMonthLength();
	rectSize.SetWidth( fixedFont.StringWidth( sb.String() ) + SPACING );
	float rowHeight = rectSize.Height() + SPACING;
	float itemWidth = rectSize.Width() + 15 + SPACING;
	rectSize.SetWidth( itemWidth  );
	rectSize.SetHeight( rowHeight );
	
	topLeftCorner.x = SPACING; 
	topLeftCorner.y += rowHeight + ( SPACING * 2 );
	
	map<uint32, DoubleNames> weekdayNames = fCalModule->GetWeekdayNames();
	uint32 limit = ( uint32 )fCalModule->GetDaysInWeek();
	uint32 curDay;
	
	for (uint32 i = firstDayOfWeek; i < limit+firstDayOfWeek; ++i) {	
		curDay = ( (i - 1) % limit ) + 1;
		
		itemToAdd = new DayItem( weekdayNames[ curDay ].shortName.String(), NULL );
		if (!itemToAdd) {
			/* Panic! */
			fLastError = B_NO_MEMORY;
			return;
		}
		itemToAdd->SetServiceItem( true );
			// If this is a weekend, we shold display it in another color
		if ( fWeekends.HasItem( (void*)i ) ||
			  fWeekends.HasItem( (void*)( i % daysInWeek ) ) )
		{
			itemToAdd->SetFrontColor( fColorForWeekends );
		} else {
			itemToAdd->SetFrontColor( fColorForServiceItems );;
		}
		itemToAdd->SetEnabled(false);
		itemToAdd->SetBackColor( ui_color( B_MENU_BACKGROUND_COLOR ) );
		fDateSelector->AddItem( itemToAdd, 
										BRect( topLeftCorner, rectSize ) );
		topLeftCorner.x += itemWidth + SPACING;
	}
	
	topLeftCorner.x = SPACING;
	topLeftCorner.y += rowHeight + SPACING;
	
	//-----------------------------------------------------------------------
	// THIRD ROW AND DOWN - THE WEEK INDIVIDUAL DAYS.
	//----------------------------------------------------------------------
	uint32 currentWeekday = ( uint32 )firstDayOfMonthInFirstWeek;
	topLeftCorner.x += ( itemWidth + SPACING ) * firstDayOfMonthInFirstWeek;
	
	for (int day = 1; day <= daysInMonth; ++day )
	{
		messageOfItem = new BMessage(kTodayModified);
		if ( !messageOfItem ) {
			/* Panic! */
			fLastError = B_NO_MEMORY;
			return;
		}
		messageOfItem->AddInt32( "Date", day );

		sb.Truncate( 0 );
		char padding = ' ';	// <-- For proper aligning of the items
		( day < 10 ) ? sb << padding << day : sb << day;
		itemToAdd = new DayItem( sb.String(), messageOfItem );
		if ( !itemToAdd ) {
			/* Panic! */
			fLastError = B_NO_MEMORY;
			return;
		}
		itemToAdd->SetEnabled( true );
		itemToAdd->SetServiceItem( false );
		messageOfItem->AddPointer( "Item", &itemToAdd );
		if ( fWeekends.HasItem( ( void* )( ( fFirstDayOfEveryWeek + currentWeekday ) % daysInWeek ) ) ||
		     fWeekends.HasItem( ( void* )( ( fFirstDayOfEveryWeek + currentWeekday ) ) ) )
		{
			itemToAdd->SetFrontColor( weekendDateColor );
		} else {
			itemToAdd->SetFrontColor( ui_color( B_MENU_ITEM_TEXT_COLOR ) );
		}
		// Does this item represent today?
		if ( fRepresentedTime.tm_mday == day ) {
			itemToAdd->SetToday( true );
			itemToAdd->SetBackColor( fColorForServiceItems );
			itemToAdd->SetMarked( true );
		}
		fDateSelector->AddItem( itemToAdd,
										BRect( topLeftCorner, rectSize ) );
		itemToAdd->SetTarget( this );
		
		topLeftCorner.x += itemWidth + SPACING;
	
		++currentWeekday;
		if ( ( currentWeekday % daysInWeek == 0 ) &&
		     ( day < daysInMonth ) )
		{
			topLeftCorner.x = SPACING;
			topLeftCorner.y += rowHeight+SPACING;
			currentWeekday = 0;
		}	
	}
	
	//-----------------------------------------------------------------------
	// LAST ROW - The option to return to current date.
	//----------------------------------------------------------------------
	topLeftCorner.y += rowHeight + SPACING;
	messageOfItem = new BMessage( kReturnToToday );
	if ( !messageOfItem ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	sb.Truncate( 0 );
	sb << "Go to today";		// Label
	itemToAdd = new DayItem( sb.String(), messageOfItem );
	if ( !itemToAdd ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	// Setting the color to blue
	itemToAdd->SetServiceItem(true);
	itemToAdd->SetFrontColor( fColorForServiceItems );
	itemToAdd->SetBackColor( ui_color( B_MENU_BACKGROUND_COLOR ) );
	itemToAdd->SetEnabled( true );
	itemToAdd->SetTarget( this );
	// The new v-alignment was already set above. Now it's time to set the
	// x-alignment. I'd like to align this item to the center of Menu's rec-
	// tangle, which require some additional calculations.
	float currentWidth = itemWidth * daysInWeek + ( SPACING * 2 );
	float desiredWidth = plainFont.StringWidth( sb.String()) ;
	topLeftCorner.x = SPACING + 0.5 * ( currentWidth - desiredWidth );
	rectSize.SetHeight( plainFontHeightString );
	rectSize.SetWidth( desiredWidth + 30 ); 
	fDateSelector->AddItem( itemToAdd,
									BRect( topLeftCorner, rectSize ) );
	
	fDateSelector->SetTargetForItems( this );
	
	UpdateTargets( fDateSelector );
}
// <-- end of function CalendarControl::CreateMenu



/*!	
 *	\brief			Internal function that creates a menu with month names.
 *	\param[in]	listOfMonths	List of months for a given year.
 *	\returns		The created BMenu.
 *	\remarks		Deletion and deallocation of the created menu is in
 *					responcibility of the caller.
 */
BPopUpMenu* CalendarControl::CreateMonthsMenu( map<int, DoubleNames> &listOfMonths )
{
	BMessage* message = NULL;
	BMenuItem* item = NULL;
	BString monthName;
	BPopUpMenu* toReturn = new BPopUpMenu("Months list");
	
	if (!toReturn) {
		/* Panic! */
		fLastError = B_NO_MEMORY; 
		return NULL;
	}
	toReturn->SetLabelFromMarked(true);
	toReturn->SetRadioMode(true);
	BFont font(be_plain_font);
	toReturn->SetFont(&font, B_FONT_FAMILY_AND_STYLE);
		
	int limit = listOfMonths.size();
	
	for (int i = 1; i <= limit; ++i ) {
		message = new BMessage( kMonthChanged );
		if ( !message ) {
			/* Panic! */
			fLastError = B_NO_MEMORY;
			return NULL;
		}
		if ( B_OK != message->AddInt8( "Month", ( int8 )i ) ) { 	//< Number of selected month in the year
			// Panic!
			exit(5);
		}
		monthName = listOfMonths[ i ].longName;
		item = new BMenuItem( monthName.String(), message );
		if (!item) { 
			/* Panic! */ 
			fLastError = B_NO_MEMORY; 
			return NULL;
		}
		if ( i == this->fRepresentedTime.tm_mon )
		{
			item->SetMarked(true);
		}
		toReturn->AddItem(item);
	}
	UpdateTargets( toReturn );
	return toReturn;
}
// <-- end of function CalendarControl::CreateMonthsMenu

/*!	
 *	\brief
 *	\param[in]	year	The current year
 *	\returns		The created BMenu.
 *	\remarks		It's up to the caller to delete this menu!
 */
BPopUpMenu* CalendarControl::CreateYearsMenu( int yearIn )
{
	BPopUpMenu* toReturn = new BPopUpMenu("Years list");
	BMessage* message = NULL;
	BMenuItem* item = NULL;
	BString yearName;
	if (!toReturn) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return NULL;
	}
	toReturn->SetLabelFromMarked(true);
	toReturn->SetRadioMode(true);
	for ( int i = yearIn - YEARS_UP_AND_DOWN; 
			i <= yearIn + YEARS_UP_AND_DOWN; 
			++i )
	{
		message = new BMessage( kYearChanged );
		if ( !message )
		{
			/* Panic! */
			fLastError = B_NO_MEMORY;
			return NULL;
		}
		if ( B_OK != message->AddInt32( "Year", i ) )
		{
			exit(5);	
		}
		yearName.Truncate( 0 );
		yearName << i;
		item = new BMenuItem( yearName.String(), message );
		if ( !item ) {
			/* Panic! */
			fLastError = B_NO_MEMORY;
			return NULL;
		}
		item->SetTarget( this );
		if ( i == yearIn ) {
			item->SetMarked( true );
		}
		toReturn->AddItem( item );
	}
	UpdateTargets( toReturn );
	return toReturn;
}	// <-- end of function CalendarControl::CreateYearsMenu



/*!	\brief		Sets all items of all menus in this view to send messages to "this".
 *		\details		This is a recursive function; it receives the menu to work on. It
 *						starts with NULL, in which case it calls itself recursively on
 *						fDateSelector menu.
 */
void CalendarControl::UpdateTargets( BMenu* menuIn )
{
	BMenu* menu = NULL;
	BMenuItem* item = NULL;
	int i, limit;
	
	if ( !menuIn )
	{
		// First run. Search the children and run recursively on each.
		UpdateTargets( fDateSelector );
	}
	else
	{
		// Some of the internal runs. Updating subsequent targets.
		limit = menuIn->CountItems();
		for ( i = 0; i < limit; ++i )
		{
			if ( ( item = menuIn->ItemAt( i ) ) != NULL )
			{
				if ( item->Submenu() ) {
					// If the item controls a submenu, descending there
					UpdateTargets( item->Submenu() );
				} else {
					item->SetTarget( this );
				}
			}
		}	// <-- end of "for (all items in the submenu)"
	}
}	// <-- end of function CalendarControl::UpdateTargets



/*!	
 *	\brief			Main function in this control.
 *	\param[in]	in	The BMessage which was sent to this control.
 */
void CalendarControl::MessageReceived(BMessage* in) {
	int8 month;
	int year;
	BMessage reply(B_REPLY);
	time_t currentTime = 0;

	if (!in) { return; }	// Sanity check

	BString sb;
	
	if ( !fCalModule ) {
		return BView::MessageReceived( in );
	}
	map<int, DoubleNames> monthNames = fCalModule->GetMonthNamesForLocalYear(
			this->fRepresentedTime.tm_year);
	map<int, BString> dayNames;
	BMenuItem* item = NULL;
	DayItem* dayItem1 = NULL;
	BMenu* menu = NULL;
	BMessage* mes = NULL;
	BPoint point;
	uint32 command = in->what;
	bool changePerformed = false;
	int prevYear = 0;
	
	switch ( command )
	{
		case ( kMonthChanged ):
			in->FindInt8( "Month", &month );
			this->fRepresentedTime.tm_mon = month;
			UpdateText();
			fMenuBar->RemoveItem( fDateSelector );
			delete fDateSelector;
			CreateMenu();
			fMenuBar->AddItem( fDateSelector );
			UpdateTargets( fDateSelector );
			return;
			break;

		case ( kTodayModified ):
			sb.Truncate( 0 );
			fRepresentedTime.tm_mday < 10 ? 
				sb << ' ' << fRepresentedTime.tm_mday :
				sb << fRepresentedTime.tm_mday;
			
			dayItem1 = dynamic_cast< DayItem* >(fDateSelector->FindItem(sb.String()));
			if (dayItem1) {
				dayItem1->SetToday( false );
				dayItem1->SetBackColor( ui_color( B_MENU_BACKGROUND_COLOR ) );
				dayItem1->SetMarked(false);
			}
			sb.Truncate( 0 );
			if ( B_OK != in->FindInt32( "Date", ( int32* )&prevYear ) ||
				  B_OK != in->FindPointer( "Item", ( void** )&dayItem1 ) ) {
				// Panic!
				exit(6);
			}
			prevYear < 10 ?
				sb << ' ' << prevYear :
				sb << prevYear;
			dayItem1 = dynamic_cast< DayItem* >( fDateSelector->FindItem( sb.String() ) );
			if ( dayItem1 ) 
			{
				dayItem1->SetBackColor( fColorForServiceItems );				
				dayItem1->SetMarked( true );
			}
			this->fRepresentedTime.tm_mday = prevYear;

			// Get list of dates after update of the month and year
			dayNames = fCalModule->GetDayNamesForLocalYearMonth(
				this->fRepresentedTime.tm_year,
				this->fRepresentedTime.tm_mon);
			if ( fRepresentedTime.tm_mday > dayNames.size() )
			{
				fRepresentedTime.tm_mday = dayNames.size();	
			}
			fDateSelector->Invalidate();			
			UpdateText();
			UpdateTargets(fDateSelector);
			return;
			break;
		case ( kReturnToToday ):
			this->fRepresentedTime = fCalModule->FromTimeTToLocalCalendar( time( NULL ) );			
			UpdateText();
			fMenuBar->RemoveItem(fDateSelector);
			delete fDateSelector;
			CreateMenu();
			fMenuBar->AddItem( fDateSelector );
			UpdateTargets( fDateSelector );
			return;
			break;
		case (kYearIncreased):
		case (kYearDecreased):
		case (kYearChanged):
			prevYear = this->fRepresentedTime.tm_year;
			if (command == kYearIncreased) {				
				++(this->fRepresentedTime).tm_year;
			} else if (command == kYearDecreased) {
				--(this->fRepresentedTime).tm_year;
			} else {
				year = in->FindInt32("Year");
				prevYear = year;
				this->fRepresentedTime.tm_year = year;
				in->RemoveName("Year");
			};
			// Get list of dates after update of the month and year
			dayNames = fCalModule->GetDayNamesForLocalYearMonth(
				this->fRepresentedTime.tm_year,
				this->fRepresentedTime.tm_mon);
			if (fRepresentedTime.tm_mday > dayNames.size()) {
				fRepresentedTime.tm_mday = dayNames.size();	
			}
			UpdateText();
//			UpdateYearsMenu(prevYear, fRepresentedTime.tm_year);
			fMenuBar->RemoveItem(fDateSelector);
			delete fDateSelector;
			CreateMenu();
			fMenuBar->AddItem(fDateSelector);
			UpdateTargets(fDateSelector);
			return;
			break;
		case (kMonthDecreased):
		case (kMonthIncreased):
		
			sb.Truncate(0);
			sb << monthNames[fRepresentedTime.tm_mon].longName;
			if (command == kMonthDecreased) {
				--fRepresentedTime.tm_mon;				
				if (fRepresentedTime.tm_mon == 0) {
					changePerformed = true;
					prevYear = fRepresentedTime.tm_year;
					--fRepresentedTime.tm_year;
					monthNames = fCalModule->GetMonthNamesForLocalYear(
						this->fRepresentedTime.tm_year);
					fRepresentedTime.tm_mon = monthNames.size();
				}
			} else {
				++fRepresentedTime.tm_mon;
				if (fRepresentedTime.tm_mon > monthNames.size() ) {
					changePerformed = true;
					fRepresentedTime.tm_mon = 1;
					prevYear = fRepresentedTime.tm_year;
					++fRepresentedTime.tm_year;
					monthNames = fCalModule->GetMonthNamesForLocalYear(
						this->fRepresentedTime.tm_year);
				}
			}
			// Get list of dates after update of the month and year
			dayNames = fCalModule->GetDayNamesForLocalYearMonth(
				this->fRepresentedTime.tm_year,
				this->fRepresentedTime.tm_mon);
			if (fRepresentedTime.tm_mday > dayNames.size()) {
				fRepresentedTime.tm_mday = dayNames.size();	
			}
			UpdateText();
			fMenuBar->RemoveItem(fDateSelector);
			delete fDateSelector;
			CreateMenu();
			fMenuBar->AddItem(fDateSelector);
			UpdateTargets(fDateSelector);
			return;
			break;
		case (kOpenDateSelector):	
			
		default:
			BView::MessageReceived(in);
	}
//	in->SendReply(&reply);

	BView::MessageReceived(in);
}
// <-- end of function CalendarControl::MessageReceived

/*!	
 *	\details	After the user selects another year, the whole 
 *				years' menu should be changed. The selected year
 *				becomes the middle of the menu, with years before
 *				and after it surrounding it from top and bottom
 *				(respectively).
 *	\param[in]	prevYear		The year that was previously selected.
 *	\param[in]	curYear		The year that is selected now.
 */
void CalendarControl::UpdateYearsMenu(int prevYear, int curYear) {
	BMenuItem* item; BMenu* menu;
	BString sb;
	BRect fr;
	sb << prevYear;
	if (!fDateSelector) return;
	if ( (item = fDateSelector->FindItem(sb.String())) == NULL) {
		return;
	}
	fr = item->Frame();
	fDateSelector->RemoveItem( item );
	delete item;
	menu = CreateYearsMenu( curYear );
	if ( menu ) {
		fDateSelector->AddItem( menu, fr );
	}
}	// <-- end of function "CalendarControl::UpdateYearsMenu"



/*!	\brief		Enables or disables this control.
 *		\param[in]	toSet 	Enable if \em true, disable if \em false.
 */
void CalendarControl::SetEnabled( bool toSet ) {
	if ( toSet == bIsControlEnabled ) { return; }	
	
	rgb_color col;
	if (toSet) {
		col = ui_color( B_MENU_ITEM_TEXT_COLOR );
	} else {
		col = ui_color( B_MENU_SELECTION_BACKGROUND_COLOR );
	}
	
	this->fDateLabel->SetHighColor(col); 
	this->fDateLabel->Draw(fDateLabel->Bounds());
	this->fMenuBar->SetEnabled(toSet);
	bIsControlEnabled = toSet;
}	// <-- end of function CalendarControl::SetEnabled



