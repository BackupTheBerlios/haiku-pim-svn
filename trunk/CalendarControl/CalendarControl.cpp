#include "CalendarControl.h"
//#include "TimeRepresentation.h"

#include <InterfaceKit.h>
#include <String.h>
#include <Size.h>
#include <Application.h>
#include <Invoker.h>
#include <Layout.h>
#include <LayoutUtils.h>
#include <SpaceLayoutItem.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
//#include "CalendarModule.h"
//#include "GregorianCalendarModule.h"

#define 	BUTTON_WIDTH	30
#define		SPACING			2
#define		YEARS_UP_AND_DOWN	5

const uint32 firstDayOfEveryWeek = kSunday;


/*==========================================================================
**			IMPLEMENTATION OF CLASS DayItem
**========================================================================*/



/*==========================================================================
**			IMPLEMENTATION OF CLASS MonthMenu
**========================================================================*/
MonthMenu::MonthMenu(BRect frame)
	:
	BMenu("datesMenu", frame.Width(), frame.Height())
{
	
}

/*==========================================================================
**			IMPLEMENTATION OF CLASS CalendarControl
**========================================================================*/

/*!	\function 		CalendarControl::CalendarControl
 *	\brief			The constructor of CalendarControl.
 *	\param[in]	labelIn		The label of the control.
 */
CalendarControl::CalendarControl(BRect frame,
								 const char* name,
								 const char* labelIn)
	:
	BView(frame, name, 
				B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
				B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS ),	
	label(NULL),
	menuField(NULL)
{
	// First, create the TextControl that is the main part of the UI.
	frame = this->Bounds();
	label = new BStringView(BRect(0,0,1,1), 
							"label", 
							labelIn);
	if (!label) { /* Panic! */ exit(1); }
	
	BRect stringViewFrame = label->Frame();
	
	BPoint topLeftCorner = stringViewFrame.RightTop();
	BSize size(frame.Width()-stringViewFrame.Width(), stringViewFrame.Height());
	
	Init();	// Initialize the day representation and define the menus
	
	menuField = new BMenuField(BRect(topLeftCorner, size),
								"menu",
								"",
								dateSelector,
								true,
								B_FOLLOW_LEFT | B_FOLLOW_TOP);

	UpdateText();
	
	BGroupLayout* lay = new BGroupLayout(B_HORIZONTAL);
	
	if (!lay) { 
		// Panic! 
		exit(1); 
	}
	lay->SetInsets(5, 0, 5, 0);
	lay->SetSpacing(10);
	BView::SetLayout(lay);
	
	
	lay->AddView(label);
	lay->AddView(menuField);
	(menuField->MenuBar())->SetBorder(B_BORDER_EACH_ITEM);
//	(menuField->MenuBar())->SetAlignment(B_FOLLOW_LEFT);
	(menuField->MenuBar())->ResizeTo(30, menuField->MenuBar()->Bounds().Height());
	menuField->SetDivider(be_plain_font->StringWidth(menuField->Label())+SPACING);


/*	AddChild(BGroupLayoutBuilder(B_HORIZONTAL)
		.Add(label)
		.AddStrut(10)
		.Add(menuField)
	);
*/
}

/*!	\function		CalendarControl::AttachedToWindow
	\brief			Sets up the view color and calls this view's children.
*/
void CalendarControl::AttachedToWindow() {
	if (Parent()) {
		SetViewColor(Parent()->ViewColor());
	}
	BView::AttachedToWindow();
/*	if (textControl) {
		textControl->AttachedToWindow();
	}
	if (openMenuButton) {
//		openMenuButton->AttachedToWindow();
	}
*/	
	BLooper* looper = (BLooper*)Looper();
	if (looper && looper->LockLooper()) {
		looper->AddHandler((BHandler*) this);
		BAlert *al = new BAlert("Aaa", "Added successfully", "Ok");
		if (al) {
			al->Go();
		}
		looper->UnlockLooper();
	}
	BMenu* men;
	if (dateSelector) {
		for (int i=0; i<dateSelector->CountItems(); i++) {
			if (men = dateSelector->SubmenuAt(i)) {
				men->SetTargetForItems(this);	
			}
		}	
	}
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
	if (!menuField) { /* Nothing to update! */ return; }
		
	// We need to calculate the place required for the date representation.
	BFont textViewFont(be_plain_font);
//	textView->GetFontAndColor(0, &textViewFont, NULL);
	builderLong = BuildDateRepresentationString(true);
	builderShort = BuildDateRepresentationString(false);
	longStringLength = textViewFont.StringWidth(builderLong.String());
	shortStringLength = textViewFont.StringWidth(builderShort.String());
	
	BString sb;
	sb << "Long string: \"" << builderLong;
	sb << "\" and short string is " << builderShort;
	BAlert *al = new BAlert("Aaa", sb.String(), "Ok");
	if (al) {
//		al->Go();
	}
/*	
	frame = textView->Frame();
	if ((frame.right - frame.left) > longStringLength) {
		textView->SetText(builderLong.String());
	} else {
		textView->SetText(builderShort.String());
	}
*/
	menuField->SetLabel(builderLong.String());
	menuField->SetDivider(longStringLength);
//	menuField->ResizeToPreferred();
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
	if (!menuField) {
		RemoveChild(menuField);
		delete menuField;
		menuField = NULL;
	}
	if (!label) {
		RemoveChild(label);
		delete label;
		label = NULL;
	}
}
// <-- end of destructor fot the CalendarControl

/*!	\function		CalendarControl::FrameResized
 *	\brief			This function resizes the control to the new size.
 *	\param[in]	width	New width of the frame.
 *	\param[in]	height	New height of the frame.
 */
void CalendarControl::FrameResized(float width, float height) {
	// Updating the BView.
	BView::FrameResized(width, height);
/*	
	// First, resizing the BTextControl object.
	BRect oldControlFrame = textControl->Bounds(),
		oldTextViewFrame = textView->Bounds();
	// 6*SPACING is a result of trial and error.
	float newDelta = width - oldControlFrame.Width() - BUTTON_WIDTH - 6*SPACING;
	textControl->ResizeTo(oldControlFrame.Width() + newDelta, 
						  oldControlFrame.Height());
						  
	// Now we need to resize the BTextView inside of BTextControl	
	textView->ResizeTo(oldTextViewFrame.Width() + newDelta, 
					oldTextViewFrame.Height());
					
	// The last thing is to update the position of the BButton
	BPoint textViewRightTopCorner = (textView->Frame()).RightTop();
	textViewRightTopCorner.x += SPACING*2;
	textViewRightTopCorner.y = SPACING ;
	openMenuButton->MoveTo(textViewRightTopCorner);
	
	// Redrawing the whole content of the BView
	UpdateText();
	textControl->Invalidate();
	textView->Draw(textView->Bounds());
	openMenuButton->Draw(openMenuButton->Bounds());
	textView->Invalidate();
	openMenuButton->Invalidate();
*/
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
	int firstDayOfWeek = calModule->GetWeekDayForLocalDateAsInt(firstDayOfEveryWeek);
	
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
	BString day; 
	day << (int )daysInMonth;	// Maximum length of single day can't be more
								// then there are days in the month
	int width1 = (int )(daysInWeek*fixedFont.StringWidth(day.String()));
	int width2 = (int )plainFont.StringWidth(longestMonth.String()) +
		(int )plainFont.StringWidth("2010");
		
	rectangle.left = 0;
	width1 > width2 ? rectangle.right = width1 : rectangle.right = width2;
	
	
	BString sb;
	
	sb.Truncate(0);
	
	// Now "rectangle" has enough space to accomodate the whole set of items.
	// Let's build the menu!
	
	// This is the items we're going to use.
	BMessage* messageOfItem = NULL;
	BMenuItem* itemToAdd = NULL;
	BPoint topLeftCorner;
	BSize rectSize(0, 0);
	topLeftCorner.x = topLeftCorner.y = 0;
	
	// This is the menu we're adding items to.
	dateSelector = new BMenu("⇩",
							 rectangle.Width(),
							 rectangle.Height());

/*	dateSelector = new BMenu("⇩",
							 400,
							 400);
*/							 
	// Sanity check
	if (!dateSelector) {
		// Panic!
		exit(1);
	}
	
	
	
	// Build the list of months and year.
	BMenu* listOfMonths = CreateMonthsMenu(monthNames);

	// Add the list of months
	topLeftCorner.x = SPACING+5;
	topLeftCorner.y = SPACING;
	rectSize.SetHeight((float)plainFont.Size());
	rectSize.SetWidth((float)plainFont.StringWidth(longestMonth.String()));
	dateSelector->AddItem(listOfMonths, 
					BRect(topLeftCorner, rectSize)
				  );
	// Add year
	sb.Truncate(0);
	sb << representedTime.tm_year;
	topLeftCorner.x += SPACING + rectSize.Width();	
	rectSize.SetWidth((float )plainFont.StringWidth(sb.String()));
	BMenu* listOfYears = CreateYearsMenu(this->representedTime.tm_year);	
	dateSelector->AddItem(listOfYears,
					BRect(topLeftCorner, rectSize));
	
	
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
//	toReturn->SetTargetForItems(item);
	return toReturn;
}
// <-- end of function CalendarControl::CreateYearsMenu

void CalendarControl::UpdateTargets(BView* in) {
	int i = 0;
	for (; i<in->CountChildren(); ++i) {
		BView* child = in->ChildAt(i);
		BInvoker* inv = dynamic_cast<BInvoker*>	(child);
		if (inv) {
			inv->SetTarget(this);
		}
		UpdateTargets(child);
	}
}

struct cha {
	char a;
	char b;
	char c;
	char d;	
};

union u {
	uint32 i;
	cha ch;
};

/*!	\function		CalendarControl::MessageReceived
 *	\brief			Main function in this control.
 *	\param[in]	in	The BMessage which was sent to this control.
 */
void CalendarControl::MessageReceived(BMessage* in) {
	int month, year;
	if (!in) { return; }	// Sanity check
	
	u a;
	a.i = (uint32)in->what;

	BString sb;
	sb << "Got message inside CalControl! " << a.ch.a << a.ch.b << a.ch.c << a.ch.d;
	BAlert *al = new BAlert("Aaa", sb.String(), "Ok");
	if (al) {
		al->Go();
	}
	
	switch (in->what) {
		case (kMonthChanged):
			month = in->FindInt8("Month");
			this->representedTime.tm_mon = month;
			UpdateText();
			break;
		case (kYearChanged):
			year = in->FindInt32("Year");
			this->representedTime.tm_year = year;
			UpdateText();
			break;
		case (kOpenDateSelector):	
		default:
			BView::MessageReceived(in);
	}
}
// <-- end of function CalendarControl::MessageReceived
