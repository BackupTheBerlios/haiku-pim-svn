#include "CalendarControl.h"

CalendarControl::CalendarControl(BRect frame,
								 const char* name,
								 const char* label)
	:
	BTextControl(BRect(frame.left, frame.top, frame.right-15, frame.bottom),
				 name, label, NULL, NULL)
{
	Init();
	((BTextView*)(this->TextView()))->MakeEditable(false);
}

/*!	\function		CalendarControl::Init
 *	\brief			Initializes the CalendarControl to default date, separator and order.
 */
void CalendarControl::Init() {
	time_t currentTime = time(NULL);
	tm today = localtime(currentTime);
	++today.tm_mon;
	today.tm_year += 1900;
	this->representedTime = TimeRepresentation(today);
	this->representedTime.SetIsRepresentingRealDate(true);
	this->representedTime.SetCalendarModule(BString("Gregorian"));
	
	int limit = (int)listOfCalendarModules.CountItems();
	while (limit > 0) {
		this->calModule = (CalendarModule*)listOfCalendarModules.ItemAt(--limit);
		if (calModule->Identify() == BString("Gregorian") {
			break;
		}
	}

	this->separator = '/';
	this->orderOfElements = kDayMonthYear;
}
// <-- end of function CalendarControl::Init

/*!	\function		CalendarControl::UpdateText
 *	\brief			Redraws the text inside of the text control according to current settings.
 */
void CalendarControl::UpdateText() {
	BString builder;

	if (!calModule) { return; }
	
	map<int, BString> dayNames = calModule->GetDayNamesForLocalYearMonth(this->representedTime.tm_year,
																	this->representedTime.tm_mon);

	switch (this->orderOfElements) {
		case (kMonthDayYear):
			// Month
			builder << this->representedTime.tm_mon;
			builder.Append(this->separator, 1);
			// Day
			builder.Append(dayNames[this->representedTime.tm_mday]);
			builder.Append(this->separator, 1);
			// Year
			builder << this->representedTime.tm_year;
			break;
		case (kYearMonthDay):
			// Year
			builder << this->representedTime.tm_year;
			builder.Append(this->separator, 1);
			// Month
			builder << this->representedTime.tm_mon;
			builder.Append(this->separator, 1);
			// Day
			builder.Append(dayNames[this->representedTime.tm_mday]);
			break;
		default:		// Intentional fall-through
		case (kDayMonthYear):
			// Day
			builder.Append(dayNames[this->representedTime.tm_mday]);
			builder.Append(this->separator, 1);
			// Month
			builder << this->representedTime.tm_mon;
			builder.Append(this->separator, 1);
			// Year
			builder << this->representedTime.tm_year;
			break;
	};
	((BTextView*)(this->TextView()))->SetText(builder.String());
}
// <-- end of function CalendarControl::UpdateText

CalendarControl::~CalendarControl(void)
{
}
