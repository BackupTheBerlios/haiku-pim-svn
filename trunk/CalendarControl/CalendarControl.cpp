#include "CalendarControl.h"

#include <TextView.h>
#include <String.h>
//#include "CalendarModule.h"
//#include "GregorianCalendarModule.h"

#define 	BUTTON_WIDTH	30
#define		SPACING			2

const WeekDays firstDayOfEveryWeek = kSunday;


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
 */
CalendarControl::CalendarControl(BRect frame,
								 const char* name,
								 const char* label)
	:
	BView(frame, name, 
				B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
				B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS ),	
	textControl(NULL),
	openMenuButton(NULL)
{
	// First, create the TextControl that is the main part of the UI.
	frame = this->Bounds();
	textControl = new BTextControl(BRect(frame.left, 
										frame.top,
										frame.right-BUTTON_WIDTH-2*SPACING,
										frame.bottom),
				 name, label, NULL, NULL,
				 B_FOLLOW_LEFT | B_FOLLOW_TOP,
				 B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS );
	if (! textControl) {
		// Panic!
		exit(1);
	}
	
	// textView is used for convenience.
	textView = textControl->TextView();
	if (!textView) {
		// Panic!
		exit(1);
	}
	
	Init();
	
	BRect button = textView->Bounds();
	button.right = frame.right-SPACING;
	button.left = frame.right-BUTTON_WIDTH-SPACING;
	button.top = 0;
//	button.right = frame.right-3*SPACING;
//	button.bottom -= 2*SPACING;
	
	openMenuButton = new BButton(button, "openMenu", "⇩", NULL);
	if (!openMenuButton) {
		// Panic!
		exit(1);
	}
	
	textView->MakeEditable(false);
	textView->MakeSelectable(false);
	
	AddChild(textControl);
	AddChild(openMenuButton);		
}

/*!	\function		CalendarControl::AttachedToWindow
	\brief			Sets up the view color and calls this view's children.
*/
void CalendarControl::AttachedToWindow() {
	if (Parent()) {
		SetViewColor(Parent()->ViewColor());
	}
	BView::AttachedToWindow();
	if (textControl) {
		textControl->AttachedToWindow();
	}
	if (openMenuButton) {
		openMenuButton->AttachedToWindow();	
		BRect button = textView->Bounds();
		
		openMenuButton->ResizeTo(BUTTON_WIDTH, button.Height()+2*SPACING);		
	}
	
/*	BAlert* al = new BAlert("Aaa", "Aaaa", "Ok");
	if (al) {
		al->Go();
	}
*/
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
	textView->MakeEditable(false);
	textView->MakeSelectable(false);
//	textView->AcceptsDrop(NULL);
//	textView->AcceptsPaste(NULL);
	for (uint32 a = 1; a < 16056; a++) {
		textView->DisallowChar(a);
	}
	
	textControl->SetDivider(50);
	UpdateText();
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
		
	if (!textView) { return; }	// Nothing to update
	
	// We need to calculate the place required for the date representation.
	BFont textViewFont;
	textView->GetFontAndColor(0, &textViewFont, NULL);
	builderLong = BuildDateRepresentationString(true);
	builderShort = BuildDateRepresentationString(false);
	longStringLength = textViewFont.StringWidth(builderLong.String());
	shortStringLength = textViewFont.StringWidth(builderShort.String());
	frame = textView->Frame();
	if ((frame.right - frame.left) > longStringLength) {
		textView->SetText(builderLong.String());
	} else {
		textView->SetText(builderShort.String());
	}
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
	if (!textControl) {
		RemoveChild(textControl);
		delete textControl;
		textControl = NULL;
		textView = NULL;
	}
	if (!openMenuButton) {
		RemoveChild(openMenuButton);
		delete openMenuButton;
		openMenuButton = NULL;
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
	textViewRightTopCorner.y = 0 ;
	openMenuButton->MoveTo(textViewRightTopCorner);
	
	// Redrawing the whole content of the BView
	UpdateText();
	textControl->Invalidate();
	textView->Draw(textView->Bounds());
	openMenuButton->Draw(openMenuButton->Bounds());
	textView->Invalidate();
	openMenuButton->Invalidate();
}
// <-- end of function CalendarControl::FrameResized

/*!	\function		CalendarControl::MakeFocus
 *	\brief			This function overrides BTextControl's version of MakeFocus.
 *	\details		When invoked, this function opens the date selection menu.
 *	\param[in]	focused   If the control is in focus, this parameter is "true".
 */
void CalendarControl::MakeFocus(bool focused = true) {
	if (focused) {
		BAlert* al = new BAlert("Aaa", "Aaaa", "Ok");
		al->Go();
	}
	textControl->MakeFocus(false);
}
// <-- end of function CalendarControl::MakeFocus

/*!	\function		CalendarControl::CreateMenu
 *	\brief			This function creates and updates the BPopUpMenu.
 *	\details		The created menu is updated every time the TimeRepresentation
 *					changes.
 */
void CalendarControl::CreateMenu(void) {
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
	for (int i=2; i < monthNames.size(); i++) {
		if (((BString)(monthNames[i].longName)).Length() > longestMonth.Length()) {
			longestMonth = monthNames[i].longName;
		}	
	}
	BString day; 
	day << (int )daysInMonth;
	int width1 = (int )daysInWeek*fixedFont.StringWidth(day.String());
	int width2 = (int )plainFont.StringWidth(longestMonth.String()) +
		(int )plainFont.StringWidth("2010");
		
	rectangle.left = 0;
	width1 > width2 ? rectangle.right = width1 : rectangle.right = width2;
	
}
// <-- end of function CalendarControl::CreateMenu
