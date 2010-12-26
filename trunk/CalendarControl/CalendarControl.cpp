#include "CalendarControl.h"
//#include "TimeRepresentation.h"

#include <InterfaceKit.h>
#include <OS.h>
#include <String.h>
#include <Size.h>
#include <Application.h>
#include <Invoker.h>
#include <Layout.h>
#include <LayoutUtils.h>
#include <SpaceLayoutItem.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <stdio.h>
//#include "CalendarModule.h"
//#include "GregorianCalendarModule.h"

#define 	BUTTON_WIDTH	30
#define		SPACING			2
#define		YEARS_UP_AND_DOWN	5

rgb_color weekdayNameColor = {0x80, 0x80, 0x80, 255};
rgb_color weekendNameColor = {255, 0x80, 0x80, 255};
rgb_color weekendDateColor = {255, 0x0, 0x0, 255};
rgb_color todayBackColor = {0, 0, 0x80, 255};


/*==========================================================================
**			IMPLEMENTATION OF CLASS DayItem
**========================================================================*/
DayItem::DayItem(BString date, BMessage* message)
	:
	BMenuItem(date.String(), message),
	isToday(false),
	isServiceItem(false)
{ 
	fFrontColor = ui_color(B_MENU_ITEM_TEXT_COLOR);
	fBackColor = ui_color(B_MENU_BACKGROUND_COLOR);
	invocationMessage = message;
	targetHandler = be_app;
}

DayItem::DayItem(const char* date, BMessage* message) 
	:
	BMenuItem(date, message),
	isToday(false),
	isServiceItem(false)
{
	fFrontColor = ui_color(B_MENU_ITEM_TEXT_COLOR);
	fBackColor = ui_color(B_MENU_BACKGROUND_COLOR);
	invocationMessage = message;
	targetHandler = be_app;
}

void DayItem::SetFrontColor(rgb_color in) 
{
	fFrontColor = in; 
}
/*
void DayItem::GetContentSize(float *width, float* height) {
	if (!width || !height || !Menu()) { return; }	
	BFont font;
	Menu()->GetFont(&font);
	font.GetHeight((font_height*)height);
	*width = font.StringWidth(this->Label());
}
// <-- end of function DayItem::GetContentSize.
*/
void DayItem::DrawContent() {
	BMenu* menu = this->Menu();
	if (! menu) { return; }	// Nothing to do if not attached to menu
	rgb_color backColor = menu->LowColor(),
				frontColor = menu->HighColor();				
	
	menu->SetLowColor(fBackColor);
	menu->SetHighColor(fFrontColor);
	BRect frame = this->Frame(); 
	frame.InsetBySelf(-1, -1);
	frame.OffsetBySelf(-1, -1);
	menu->StrokeRect(frame, B_SOLID_LOW);
	BMenuItem::DrawContent();
	menu->SetLowColor(backColor);
	menu->SetHighColor(frontColor);	
}

void DayItem::SetTarget(BHandler* target) {
	if (target) {
		this->targetHandler = target;
	}
	BMenuItem::SetTarget(target);	
}

void DayItem::Fire(void) {
	if (invocationMessage && targetHandler) {
//		BMessenger mess(targetHandler);
		if (be_app->Looper()->Lock()) {
//			be_app->SendMessage(invocationMessage);
			be_app->Looper()->Unlock();
		}
	}
}

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
			BAlert* al = new BAlert("A", "Inside!", "Ok");
//			if (al) al->Go();
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

	return;
}


/*==========================================================================
**			IMPLEMENTATION OF CLASS CalendarControl
**========================================================================*/

/*!	\function 		CalendarControl::CalendarControl
 *	\brief			The constructor of CalendarControl.
 *	\param[in]	labelInForCalendar		The label of the calendar part of control.
 *	\param[in]	labelInForTime			The label of the time setting part.
 */
CalendarControl::CalendarControl(BRect frame,
								 const char* name,
								 const char* labelInForCalendar)
	:
	BView(frame, name, 
				B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
				B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS ),	
	label(NULL),
	dateLabel(NULL),
	isControlEnabled(true)
{
	// Preferences to be set.
	this->weekends.AddItem((void*)kFriday);
	this->weekends.AddItem((void*)kSaturday);
	this->weekends.AddItem((void*)kMonday);
	this->firstDayOfEveryWeek = kTuesday;
	
	
	frame = this->Bounds();
	label = new BStringView(BRect(0,0,1,1), 
							"label", 
							labelInForCalendar);
	if (!label) { /* Panic! */ exit(1); }
	label->ResizeToPreferred();
	
	dateLabel = new BStringView(BRect(0,0,1,1), 
							"dateLabel", 
							NULL);
	if (!dateLabel) { /* Panic! */ exit(1); }
	
	BRect stringViewFrame = label->Frame();
	
	BPoint topLeftCorner = stringViewFrame.RightTop();
	BSize size(30, stringViewFrame.Height()+SPACING);
	
	Init();	// Initialize the day representation and define the menus
	
	menuBar = new BMenuBar(BRect(topLeftCorner, size),
								"menuBar",
								B_FOLLOW_RIGHT | B_FOLLOW_TOP,
								B_ITEMS_IN_ROW,
								false);
	if (! menuBar) {
		// Panic!
		exit(1);
	}
	menuBar->SetBorder(B_BORDER_EACH_ITEM);
	menuBar->AddItem(dateSelector);

	UpdateText();
	
	BGroupLayout* lay = new BGroupLayout(B_HORIZONTAL);
	
	if (!lay) { 
		// Panic! 
		exit(1); 
	}
	lay->SetInsets(0, 0, 0, 0);
	lay->SetSpacing(10);
	BView::SetLayout(lay);
	
	BLayoutItem* layoutItem;
	BSize size1;
	
	layoutItem = lay->AddView(label);
	size1.SetWidth(layoutItem->Frame().Width());
	size1.SetHeight(layoutItem->Frame().Height());
	layoutItem->SetExplicitPreferredSize(size1);
	layoutItem->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	layoutItem = lay->AddView(dateLabel);
	layoutItem->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	layoutItem = lay->AddView(menuBar);	
	layoutItem->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	size.SetHeight(size.Height());
	layoutItem->SetExplicitMaxSize(size);
	

}

/*!	\function		CalendarControl::AttachedToWindow
	\brief			Sets up the view color and calls this view's children.
*/
void CalendarControl::AttachedToWindow() {
	
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
	if (dateSelector) {
		for (int i=0; i<dateSelector->CountItems(); i++) {
			if (men = dateSelector->SubmenuAt(i)) {
				men->SetTargetForItems(this);	
			} else {
				if (item = dateSelector->ItemAt(i)) {
					item->SetTarget(this);	
				}	
			}
		}	
	}

//	UpdateTargets(this);
}
// <-- end of function CalendarControl::AttachedToWindow

 
/*!	\function		CalendarControl::Init
 *	\brief			Initializes the CalendarControl to default date, separator and order.
 */
void CalendarControl::Init() {
	time_t currentTime = time(NULL);
	tm* today = localtime(&currentTime);
	today->tm_mon++;
	today->tm_year += 1900;
	this->representedTime = TimeRepresentation(*today);
	this->representedTime.SetIsRepresentingRealDate(true);
	this->representedTime.SetCalendarModule(BString("Gregorian"));
	
	int limit = (int)listOfCalendarModules.CountItems();
	while (limit > 0) {
		this->calModule = (CalendarModule*)listOfCalendarModules.ItemAt(--limit);
		if (calModule->Identify() == BString("Gregorian")) {
			break;
		}
	} 

	this->separator = ' ';
	this->orderOfElements = kDayMonthYear;
	
	CreateMenu();
}
// <-- end of function CalendarControl::Init

/*!	\function		CalendarControl::UpdateText
 *	\brief			Redraws the text inside of the text control according to current settings.
 */
void CalendarControl::UpdateText() {
	BString builderLong, builderShort;
	float longStringLength, shortStringLength;
	BRect frame;

	if (!calModule) { return; }
	if (!dateLabel) { /* Nothing to update! */ return; }
		
	// We need to calculate the place required for the date representation.
	BFont textViewFont(be_plain_font);
//	textView->GetFontAndColor(0, &textViewFont, NULL);
	builderLong = BuildDateRepresentationString(true);
	builderShort = BuildDateRepresentationString(false);
	longStringLength = textViewFont.StringWidth(builderLong.String());
	shortStringLength = textViewFont.StringWidth(builderShort.String());
	
	dateLabel->SetText(builderLong.String());
}
// <-- end of function CalendarControl::UpdateText

/*!	\function 		CalendarControl::BuildDateRepresentationString
 *	\brief			Builds the string which represents the date.
 *	\details		The string is built according to order of representation
 *					stated in the object.
 *	\param[in]	useLongMonthNames	If "true", the longer version of the
 *									month name is used.
 *	\returns		The created BString object.
 */
BString CalendarControl::BuildDateRepresentationString(bool useLongMonthNames) 
{
	map<int, BString> dayNames = calModule->GetDayNamesForLocalYearMonth(
			this->representedTime.tm_year,
			this->representedTime.tm_mon);
	map<int, DoubleNames> monthNames = calModule->GetMonthNamesForLocalYear(
			this->representedTime.tm_year);
			
	BString builder, day, month, year;
	
	day = dayNames[this->representedTime.tm_mday];
	year << this->representedTime.tm_year;
	if (useLongMonthNames) {
		month = ((DoubleNames)(monthNames[this->representedTime.tm_mon])) .longName;
	} else {
		month = ((DoubleNames)(monthNames[this->representedTime.tm_mon])).shortName; 
	}
	
	switch ((int)this->orderOfElements) {
		case ((int)kMonthDayYear):
			// Month
			builder << month;
			builder.Append(this->separator, 1);
			// Day
			builder << day;
			builder.Append(this->separator, 1);
			// Year
			builder << year;
			break;
		case ((int)kYearMonthDay):
			// Year
			builder << year;
			builder.Append(this->separator, 1);
			// Month
			builder << month;
			builder.Append(this->separator, 1);
			// Day
			builder << day;
			break;
		default:		// Intentional fall-through
		case ((int)kDayMonthYear):
			// Day
			builder << day;
			builder.Append(this->separator, 1);
			// Month
			builder << month;
			builder.Append(this->separator, 1);
			// Year
			builder << year;
			break;
	};
	return builder;
}
// <-- end of function CalendarControl::BuildDateRepresentationString

/*!	\function	CalendarControl::~CalendarControl
 *	\brief		Destructor of the CalendarControl.
 */
CalendarControl::~CalendarControl(void)
{
	if (!menuBar) {
		RemoveChild(menuBar);
		delete menuBar;
		menuBar = NULL;
	}
	if (!label) {
		RemoveChild(label);
		delete label;
		label = NULL;
	}
	if (!dateLabel) {
		RemoveChild(dateLabel);
		delete dateLabel;
		dateLabel = NULL;
	}
//	this->weekends.MakeEmpty();
}
// <-- end of destructor for the CalendarControl

/*!	\function		CalendarControl::FrameResized
 *	\brief			This function resizes the control to the new size.
 *	\param[in]	width	New width of the frame.
 *	\param[in]	height	New height of the frame.
 */
void CalendarControl::FrameResized(float width, float height) {
	// Updating the BView.
	BView::FrameResized(width, height);
//	menuBar->ResizeTo(30, menuBar->Bounds().Height());
}
// <-- end of function CalendarControl::FrameResized

/*!	\function		CalendarControl::MakeFocus
 *	\brief			This function overrides BTextControl's version of MakeFocus.
 *	\details		When invoked, this function opens the date selection menu.
 *	\param[in]	focused   If the control is in focus, this parameter is "true".
 */
void CalendarControl::MakeFocus(bool focused) {
	BView::MakeFocus(focused);
}
// <-- end of function CalendarControl::MakeFocus

/*!	\function		CalendarControl::CreateMenu
 *	\brief			This function creates and updates the BPopUpMenu.
 *	\details		The created menu is updated every time the TimeRepresentation
 *					changes.
 */
void CalendarControl::CreateMenu(void) {
	BAlert* al;
	// The whole menu will be created in fixed font.
	BFont fixedFont(be_fixed_font);
	BFont plainFont(be_plain_font);
	BRect rectangle;
	
	float widthOfTheWeekRows = 0;
	float widthOfFirstRow = 0;
	
	// Which month shall we represent?
	map<int, BString> dayNames = calModule->GetDayNamesForLocalYearMonth(
			this->representedTime.tm_year,
			this->representedTime.tm_mon);
	map<int, DoubleNames> monthNames = calModule->GetMonthNamesForLocalYear(
			this->representedTime.tm_year);			
	
	int daysInMonth = dayNames.size();
	int daysInWeek = (int )calModule->GetDaysInWeek();
	
	// We need to determine the bounding rectangle for the menu.
	// For this, we need to obtain the maximum bounding rectangle for a string.
	font_height fontHeightStruct;
	fixedFont.GetHeight(&fontHeightStruct);
	float fontHeightString = fixedFont.Size();
	fontHeightString += fontHeightStruct.ascent + 
						fontHeightStruct.descent +
						fontHeightStruct.leading;
	// Now fontHeightString is surely big enough to enclose every string in 
	// height. How many lines will we need? One for name of month and year,
	// one for weekday names, and several more for the dates themselves.
	TimeRepresentation tempDay(this->representedTime);
	tempDay.tm_mday = 1;
	
	int firstDayOfMonthWD = calModule->GetWeekDayForLocalDateAsInt(tempDay);
	int firstDayOfWeek = GetFirstDayOfWeek();
	
	int firstDayOfMonthInFirstWeek =
		(firstDayOfMonthWD + daysInWeek - firstDayOfWeek) % daysInWeek;
		
	float numberOfWeeksRequiredFL = 1 + 
		((float)(daysInMonth - (daysInWeek - firstDayOfMonthInFirstWeek))
			/ daysInWeek);
	  
	int numberOfWeeksRequired = (int )numberOfWeeksRequiredFL;
	if (numberOfWeeksRequiredFL > (int )numberOfWeeksRequiredFL) {
		++numberOfWeeksRequired;
	}
	
	// Now we have the correct amount of weeks in numberOfWeeksRequired.
	// We can calculate the height of required rectangle.
	rectangle.top = 0;
	rectangle.bottom = (numberOfWeeksRequired + 1 )*fontHeightString;
		
	BString longestMonth = monthNames[1].longName;
	for (int i=2; i < (int)monthNames.size(); i++) {
		if (((BString)(monthNames[i].longName)).Length() > longestMonth.Length()) {
			longestMonth = monthNames[i].longName;
		}	
	}
	BString day, sb; 
	day << (int )daysInMonth;	// Maximum length of single day can't be more
								// then there are days in the month
	sb << (int )representedTime.tm_year;
	int width1 = (int )(daysInWeek*fixedFont.StringWidth(day.String()));
	int width2 = (int )plainFont.StringWidth(longestMonth.String()) +
		(int )plainFont.StringWidth(sb.String());
		
	rectangle.left = 0;
	width1 > width2 ? rectangle.right = width1 : rectangle.right = width2;
		
	sb.Truncate(0);
	
	// Now "rectangle" has enough space to accomodate the whole set of items.
	// Let's build the menu!
	
	// This is the items we're going to use.
	BMessage* messageOfItem = NULL;
	DayItem* itemToAdd = NULL;
	BPoint topLeftCorner;
	BSize rectSize(0, 0);
	
	// This is the menu we're adding items to.
/*	dateSelector = new MonthMenu("⇩",
							 rectangle.Width(),
							 rectangle.Height());
*/

	dateSelector = new MonthMenu("⇩", 800, 800);
	
	// Sanity check
	if (!dateSelector) {
		// Panic!
		exit(1);
	}
	
	dateSelector->SetViewColor(ui_color(B_MENU_BACKGROUND_COLOR));
	
	dateSelector->SetFont(&fixedFont);
	
	topLeftCorner.x = SPACING+5;
	topLeftCorner.y = SPACING;	
	
	// Build the list of months.
	BMenu* listOfMonths = CreateMonthsMenu(monthNames);
	
	//-----------------------------------------------------
	// FIRST ROW.
	//-----------------------------------------------------
	
	// Add the item to scroll list of months back
	messageOfItem = new BMessage(kMonthDecreased);
	itemToAdd = new DayItem("‹", messageOfItem);
	if (!itemToAdd) { /* Panic! */ exit(1); }
	rgb_color color;
	color.red = 0; color.green = 0; color.blue = 255; color.alpha = 255;
	itemToAdd->SetFrontColor(color);
	itemToAdd->SetBackColor(ui_color(B_MENU_BACKGROUND_COLOR));
	itemToAdd->SetServiceItem(true);
	itemToAdd->SetEnabled(true);
	rectSize.SetHeight((float)fixedFont.Size());
	rectSize.SetWidth((float)fixedFont.StringWidth("‹"));
	dateSelector->AddItem(itemToAdd, 
				BRect(topLeftCorner, rectSize));

	topLeftCorner.x += (float)fixedFont.StringWidth("‹") + SPACING;

	// Add the list of months	
	rectSize.SetHeight((float)plainFont.Size());
	rectSize.SetWidth((float)plainFont.StringWidth(longestMonth.String()));
	dateSelector->AddItem(listOfMonths, 
					BRect(topLeftCorner, rectSize) );
	topLeftCorner.x += SPACING + rectSize.Width();	

	// Add the item to scroll list of months forward.
	messageOfItem = new BMessage(kMonthIncreased);
	itemToAdd = new DayItem("›", messageOfItem);
	if (!itemToAdd) { /* Panic! */ exit(1); }
	color.red = 0; color.green = 0; color.blue = 255; color.alpha = 255;
	itemToAdd->SetFrontColor(color);
	itemToAdd->SetBackColor(ui_color(B_MENU_BACKGROUND_COLOR));
	itemToAdd->SetServiceItem(true);
	itemToAdd->SetEnabled(true);
	rectSize.SetHeight((float)fixedFont.Size());
	rectSize.SetWidth((float)fixedFont.StringWidth("›"));
	dateSelector->AddItem(itemToAdd, 
				BRect(topLeftCorner, rectSize));

	topLeftCorner.x += (float)fixedFont.StringWidth("›") + 10 + SPACING;

	// Add the item to scroll list of years down.
	messageOfItem = new BMessage(kYearDecreased);
	itemToAdd = new DayItem("‒", messageOfItem);
	if (!itemToAdd) { /* Panic! */ exit(1); }
	color.red = 0; color.green = 0; color.blue = 255; color.alpha = 255;
	itemToAdd->SetFrontColor(color);
	itemToAdd->SetBackColor(ui_color(B_MENU_BACKGROUND_COLOR));
	itemToAdd->SetServiceItem(true);
	itemToAdd->SetEnabled(true);
	rectSize.SetHeight((float)fixedFont.Size());
	rectSize.SetWidth((float)fixedFont.StringWidth("‒"));
	dateSelector->AddItem(itemToAdd, 
				BRect(topLeftCorner, rectSize));

	topLeftCorner.x += (float)fixedFont.StringWidth("‒") + SPACING;
	
	// Add year
	sb.Truncate(0);
	sb << representedTime.tm_year;
	
	rectSize.SetWidth((float )plainFont.StringWidth(sb.String()));
	BMenu* listOfYears = CreateYearsMenu(this->representedTime.tm_year);	
	dateSelector->AddItem(listOfYears,
					BRect(topLeftCorner, rectSize));
	topLeftCorner.x += (float)plainFont.StringWidth("0000") + SPACING;

	// Add item to scroll list of years up.
		// Add the item to scroll list of years down.
	messageOfItem = new BMessage(kYearIncreased);
	itemToAdd = new DayItem("+", messageOfItem);
	if (!itemToAdd) { /* Panic! */ exit(1); }
	color.red = 0; color.green = 0; color.blue = 255; color.alpha = 255;
	itemToAdd->SetFrontColor(color);
	itemToAdd->SetBackColor(ui_color(B_MENU_BACKGROUND_COLOR));
	itemToAdd->SetServiceItem(true);
	itemToAdd->SetEnabled(true);
	rectSize.SetHeight((float)fixedFont.Size());
	rectSize.SetWidth((float)fixedFont.StringWidth("+"));
	dateSelector->AddItem(itemToAdd, 
				BRect(topLeftCorner, rectSize));

	//-----------------------------------------------------
	// SECOND ROW.	WEEKDAY NAMES
	//-----------------------------------------------------
	rectSize.SetHeight(fixedFont.Size()+SPACING);
	rectSize.SetWidth(fixedFont.StringWidth("WW")+SPACING);
	float rowHeight = rectSize.Height()+SPACING;
	float itemWidth = rectSize.Width()+ 5 +SPACING;
	
	printf("Item width is %d.\n", (int )itemWidth);
	
	topLeftCorner.x = SPACING; 
	topLeftCorner.y = rowHeight+SPACING;
	
	map<uint32, DoubleNames> weekdayNames = calModule->GetWeekdayNames();
	uint32 limit = (uint32)calModule->GetDaysInWeek();
	uint32 curDay;
	
	for (uint32 i = firstDayOfWeek; i < limit+firstDayOfWeek; ++i) {	
		curDay = ((i-1)%limit) + 1;
		
		itemToAdd = new DayItem(weekdayNames[curDay].shortName.String(), NULL);
		printf("Printing string %s at offset %d.\n", 
			weekdayNames[curDay].shortName.String(), 
			(int )topLeftCorner.x);
		if (!itemToAdd) { /* Panic! */ exit(1); }
		itemToAdd->SetServiceItem(true);
		if (weekends.HasItem((void*)i) ||
			weekends.HasItem((void*)(i%daysInWeek))) 
		{
			itemToAdd->SetFrontColor(weekendNameColor);
		} else {
			itemToAdd->SetFrontColor(weekdayNameColor);
		}
		itemToAdd->SetEnabled(false);
		itemToAdd->SetBackColor(ui_color(B_MENU_BACKGROUND_COLOR));
		dateSelector->AddItem(itemToAdd, 
			BRect(topLeftCorner, rectSize));
		topLeftCorner.x += itemWidth + SPACING;		
	}
	topLeftCorner.x = SPACING;	
	
	//-----------------------------------------------------------------------
	// THIRD ROW AND DOWN - THE WEEK INDIVIDUAL DAYS.
	//----------------------------------------------------------------------
	uint32 currentWeekday = (uint32)firstDayOfMonthInFirstWeek;
	topLeftCorner.x += (itemWidth+SPACING) * firstDayOfMonthInFirstWeek;
	topLeftCorner.y += rowHeight + SPACING;
	for (int day=1; day <= daysInMonth; ++day) {
		messageOfItem = new BMessage(kTodayModified);
		if (!messageOfItem) { /* Panic! */ exit(1); }
		messageOfItem->AddInt32("Date", day);

		sb.Truncate(0);
		char padding = ' ';	// <-- For proper aligning of the items
		day < 10 ? sb << padding << day : sb << day;
		itemToAdd = new DayItem(sb.String(), messageOfItem);
		if (!itemToAdd) { /* Panic! */ exit(1); }
		itemToAdd->SetEnabled(true);
		itemToAdd->SetServiceItem(false);
		messageOfItem->AddPointer("Item", &itemToAdd);
		if (weekends.HasItem((void*)((firstDayOfEveryWeek+currentWeekday)%daysInWeek)) ||
		    weekends.HasItem((void*)((firstDayOfEveryWeek+currentWeekday))))
		{
			itemToAdd->SetFrontColor(weekendDateColor);
		} else {
			itemToAdd->SetFrontColor(ui_color(B_MENU_ITEM_TEXT_COLOR));
		}
		if (representedTime.tm_mday == day) {
			itemToAdd->SetBackColor(todayBackColor);
			itemToAdd->SetMarked(true);
		}
		dateSelector->AddItem(itemToAdd,
			BRect(topLeftCorner, rectSize));
		
		topLeftCorner.x += itemWidth + SPACING;
			
		++currentWeekday;
		if (currentWeekday%daysInWeek == 0) {
			topLeftCorner.x = SPACING;
			topLeftCorner.y += rowHeight+SPACING;
			currentWeekday = 0;
		}	
	}
	
	//-----------------------------------------------------------------------
	// LAST ROW - The option to return to current date.
	//----------------------------------------------------------------------
	topLeftCorner.y += rowHeight + SPACING;
	messageOfItem = new BMessage(kReturnToToday);
	if (!messageOfItem) { /* Panic! */ exit(1); }
	sb.Truncate(0);
	sb << "Go to today";		// Label
	itemToAdd = new DayItem(sb.String(), messageOfItem);
	if (!itemToAdd) { /* Panic! */ exit(1); }
	// Setting the color to blue
	color.red = 0; color.green = 0; color.blue = 255; color.alpha = 255;
	itemToAdd->SetFrontColor(color);
	itemToAdd->SetBackColor(ui_color(B_MENU_BACKGROUND_COLOR));
	itemToAdd->SetServiceItem(true);
	itemToAdd->SetEnabled(true);
	// The new v-alignment was already set above. Now it's time to set the
	// x-alignment. I'd like to align this item to the center of Menu's rec-
	// tangle, which require some additional calculations.
	BRect menuRect = dateSelector->Bounds();
	float desiredWidth = plainFont.StringWidth(sb.String());
	float currentWidth = 0;
	width1 + (daysInWeek+1)*2*SPACING > width2 ?	\
		currentWidth = width1  + (daysInWeek+1)*2*SPACING :	\
		currentWidth = width2;
	topLeftCorner.x = 0.5*(currentWidth - desiredWidth);
	rectSize.SetHeight( plainFont.Size() + SPACING );
	rectSize.SetWidth(desiredWidth); 
	dateSelector->AddItem(itemToAdd, BRect(topLeftCorner, rectSize));
}
// <-- end of function CalendarControl::CreateMenu

/*!	\function		CalendarControl::CreateMonthsMenu
 *	\brief			Internal function that creates a menu with month names.
 *	\param[in]	listOfMonths	List of months for a given year.
 *	\returns		The created BMenu.
 *	\remarks		Deletion and deallocation of the created menu is in
 *					responcibility of the caller.
 */
BMenu* CalendarControl::CreateMonthsMenu(map<int, DoubleNames> &listOfMonths) {
	BMessage* message = NULL;
	BMenuItem* item = NULL;
	BString monthName;
	BMenu* toReturn = new BMenu("Months list");
	
	if (!toReturn) { /* Panic! */ exit(1); }
	toReturn->SetLabelFromMarked(true);
	toReturn->SetRadioMode(true);
	BFont font(be_plain_font);
	toReturn->SetFont(&font, B_FONT_FAMILY_AND_STYLE);
		
	int limit = listOfMonths.size();
	
	for (int i=1; i<=limit; ++i) {
		message = new BMessage(kMonthChanged);
		if (!message) { /* Panic! */ exit(1); }
		if (B_OK != message->AddInt8("Month", (int8)i)) { 	//< Number of selected month in the year
			// Panic!
			exit(5);
		}
		monthName = listOfMonths[i].longName;
		item = new BMenuItem(monthName.String(), message);
		if (!item) { /* Panic! */ exit(1); }
		if (i == this->representedTime.tm_mon) {
			item->SetMarked(true);
		}
		toReturn->AddItem(item);
//		delete item;
//		delete message;
	}
	toReturn->SetTargetForItems((BHandler*)this);
	return (toReturn);
}
// <-- end of function CalendarControl::CreateMonthsMenu

/*!	\function		CalendarControl::CreateYearsMenu
 *	\brief
 *	\param[in]	year	The current year
 *	\returns		The created BMenu.
 *	\remarks		It's up to the caller to delete this menu!
 */
BMenu* CalendarControl::CreateYearsMenu(int yearIn) {
	BMenu* toReturn = new BMenu("Years list");
	BMessage* message = NULL;
	BMenuItem* item = NULL;
	BString yearName;
	if (!toReturn) { /* Panic! */ exit(1); }
	toReturn->SetLabelFromMarked(true);
	toReturn->SetRadioMode(true);
//	BFont font(be_plain_font);
//	toReturn->SetFont(&font, B_FONT_FAMILY_AND_STYLE);
	for (int i = yearIn-YEARS_UP_AND_DOWN; 
			i <= yearIn+YEARS_UP_AND_DOWN; 
			++i) 
	{
		message = new BMessage(kYearChanged);
		if (!message) { /* Panic! */ exit(1); }
		if (B_OK != message->AddInt32("Year", i)) {
			exit(5);	
		}
		yearName << i;
		item = new BMenuItem(yearName.String(), message);
		item->SetTarget(this);
		if (!item) { /* Panic! */ exit(1); }
		if (i == yearIn) {
			item->SetMarked(true);
		}
		toReturn->AddItem(item);
		yearName.Truncate(0);
	}
	toReturn->SetTargetForItems(this);
	return toReturn;
}
// <-- end of function CalendarControl::CreateYearsMenu

void CalendarControl::UpdateTargets(BMenu* in) {	
	dateSelector->SetTargetForItems((BHandler*)this);	
}

/*!	\function		CalendarControl::MessageReceived
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
	
	BAlert *al;
	map<int, DoubleNames> monthNames = calModule->GetMonthNamesForLocalYear(
			this->representedTime.tm_year);
	map<int, BString> dayNames;
	BMenuItem* item = NULL;
	DayItem* dayItem1 = NULL;
	BMenu* menu = NULL;
	BMessage* mes = NULL;
	BPoint point;
	uint32 command = in->what;
	bool changePerformed = false;
	int prevYear = 0;
	switch (command) {
		case (kMonthChanged):
			in->FindInt8("Month", &month);
			this->representedTime.tm_mon = month;
			UpdateText();
			menuBar->RemoveItem(dateSelector);
			delete dateSelector;
			CreateMenu();
			menuBar->AddItem(dateSelector);
			UpdateTargets(dateSelector);
			return;
			break;			
		case (kTodayModified):
			sb.Truncate(0); 
			representedTime.tm_mday < 10 ? 
				sb << ' ' << representedTime.tm_mday :
				sb << representedTime.tm_mday;
			
			dayItem1 = dynamic_cast<DayItem*>(dateSelector->FindItem(sb.String()));
			if (dayItem1) {				
				dayItem1->SetBackColor(ui_color(B_MENU_BACKGROUND_COLOR));
				dayItem1->SetMarked(false);
			}
			sb.Truncate(0);
			if (B_OK != in->FindInt32("Date", (int32*)&prevYear) ||
				B_OK != in->FindPointer("Item", (void**)&dayItem1)) {
				// Panic!
				exit(6);
			}
			prevYear < 10 ? 
				sb << ' ' << prevYear :
				sb << prevYear;
			dayItem1 = dynamic_cast<DayItem*>(dateSelector->FindItem(sb.String()));
			if (dayItem1) {
				((DayItem*)dayItem1)-> SetBackColor(todayBackColor);
				printf("Set color to item %s.\n", dayItem1->Label());
				((BMenuItem*)dayItem1)->SetMarked(true);
			}
			this->representedTime.tm_mday = prevYear;
			// Get list of dates after update of the month and year
			dayNames = calModule->GetDayNamesForLocalYearMonth(
				this->representedTime.tm_year,
				this->representedTime.tm_mon);
			if (representedTime.tm_mday > dayNames.size()) {
				representedTime.tm_mday = dayNames.size();	
			}
			dateSelector->Invalidate();			
			UpdateText();
			UpdateTargets(dateSelector);
			return;
			break;
		case (kReturnToToday):
			this->representedTime = calModule->FromTimeTToLocalCalendar( time(NULL) );			
			UpdateText();
			menuBar->RemoveItem(dateSelector);
			delete dateSelector;
			CreateMenu();
			menuBar->AddItem(dateSelector);
			UpdateTargets(dateSelector);
			return;
			break;
		case (kYearIncreased):
		case (kYearDecreased):
		case (kYearChanged):
			prevYear = this->representedTime.tm_year;
			if (command == kYearIncreased) {				
				++(this->representedTime).tm_year;
			} else if (command == kYearDecreased) {
				--(this->representedTime).tm_year;
			} else {
				year = in->FindInt32("Year");
				prevYear = year;
				this->representedTime.tm_year = year;
				in->RemoveName("Year");
			};
			// Get list of dates after update of the month and year
			dayNames = calModule->GetDayNamesForLocalYearMonth(
				this->representedTime.tm_year,
				this->representedTime.tm_mon);
			if (representedTime.tm_mday > dayNames.size()) {
				representedTime.tm_mday = dayNames.size();	
			}
			UpdateText();
//			UpdateYearsMenu(prevYear, representedTime.tm_year);
			menuBar->RemoveItem(dateSelector);
			delete dateSelector;
			CreateMenu();
			menuBar->AddItem(dateSelector);
			UpdateTargets(dateSelector);
			return;
			break;
		case (kMonthDecreased):
		case (kMonthIncreased):
			sb.Truncate(0);
			sb << monthNames[representedTime.tm_mon].longName;
			if (command == kMonthDecreased) {
				--representedTime.tm_mon;				
				if (representedTime.tm_mon == 0) {
					changePerformed = true;
					prevYear = representedTime.tm_year;
					--representedTime.tm_year;
					monthNames = calModule->GetMonthNamesForLocalYear(
						this->representedTime.tm_year);
					representedTime.tm_mon = monthNames.size();
				}
			} else {
				++representedTime.tm_mon;
				if (representedTime.tm_mon > monthNames.size() ) {
					changePerformed = true;
					representedTime.tm_mon = 1;
					prevYear = representedTime.tm_year;
					++representedTime.tm_year;
					monthNames = calModule->GetMonthNamesForLocalYear(
						this->representedTime.tm_year);
				}
			}
			// Get list of dates after update of the month and year
			dayNames = calModule->GetDayNamesForLocalYearMonth(
				this->representedTime.tm_year,
				this->representedTime.tm_mon);
			if (representedTime.tm_mday > dayNames.size()) {
				representedTime.tm_mday = dayNames.size();	
			}
			UpdateText();
			menuBar->RemoveItem(dateSelector);
			delete dateSelector;
			CreateMenu();
			menuBar->AddItem(dateSelector);
			UpdateTargets(dateSelector);
			return;
			break;
		case (kOpenDateSelector):	
			if (al) al->Go();
		default:
			BView::MessageReceived(in);
	}
	in->SendReply(&reply);
//	BView::MessageReceived(in);
}
// <-- end of function CalendarControl::MessageReceived

void CalendarControl::UpdateYearsMenu(int prevYear, int curYear) {
	BMenuItem* item; BMenu* menu;
	BString sb;
	BRect fr;
	sb << prevYear;
	if (!dateSelector) return;
	if ( (item = dateSelector->FindItem(sb.String())) == NULL) {
		return;
	}
	fr = item->Frame();
	dateSelector->RemoveItem(item);
	delete item;
	menu = CreateYearsMenu(curYear);
	dateSelector->AddItem(menu, fr);
}

void CalendarControl::AddDayToWeekends(uint32 day) {
	if (weekends.HasItem((void*)day)) { return; }
	weekends.AddItem((void*)day);
}

void CalendarControl::RemoveDayFromWeekends(uint32 day) {
	weekends.RemoveItem((void*)day);
}

void CalendarControl::SetEnabled(bool toSet) {
	if (toSet == isControlEnabled) { return; }	
	
	rgb_color col;
	if (toSet) {
		col = ui_color(B_MENU_ITEM_TEXT_COLOR);	
	} else {
		col = ui_color(B_MENU_SELECTION_BACKGROUND_COLOR);
	}
	
	this->dateLabel->SetHighColor(col); 
	this->dateLabel->Draw(dateLabel->Bounds());
	this->menuBar->SetEnabled(toSet);
}
