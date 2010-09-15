#include "GregorianCalendarModule.h"
#include "TimeRepresentation.h"

const BString GregorianCalendar::Identify(void) {
	BString toReturn("Gregorian");
	return toReturn;
}

GregorianCalendar::GregorianCalendar()
	:
	CalendarModule("Gregorian")
{
	int i=1;
	BString builder;

	

	// Fill in the dates
	for (; i<32; ++i) {
		builder << (uint32)i << std::endl();
		this->fDaysNames[i] = builder;
		builder.Truncate(0);		// Remove the contents of the string
	}
	
	struct DoubleNames names;

	names.longName = BString("January");
	names.shortName = BString("Jan");
	this->fMonthsNames[1] = names;

	names.longName = BString("February");
	names.shortName = BString("Feb");
	this->fMonthsNames[2] = names;

	names.longName = BString("March");
	names.shortName = BString("Mar");
	this->fMonthsNames[3] = names;

	names.longName = BString("April");
	names.shortName = BString("Apr");
	this->fMonthsNames[4] = names;

	names.longName = BString("May");
	names.shortName = BString("May");
	this->fMonthsNames[5] = names;

	names.longName = BString("June");
	names.shortName = BString("Jun");
	this->fMonthsNames[6] = names;

	names.longName = BString("July");
	names.shortName = BString("Jul");
	this->fMonthsNames[7] = names;

	names.longName = BString("August");
	names.shortName = BString("Aug");
	this->fMonthsNames[8] = names;

	names.longName = BString("September");
	names.shortName = BString("Sep");
	this->fMonthsNames[9] = names;

	names.longName = BString("October");
	names.shortName = BString("Oct");
	this->fMonthsNames[10] = names;

	names.longName = BString("November");
	names.shortName = BString("Nov");
	this->fMonthsNames[11] = names;

	names.longName = BString("December");
	names.shortName = BString("Dec");
	this->fMonthsNames[12] = names;

	this->fDaysInWeek = 7;

	WeekDays counter = kSunday;
	names.longName = "Sunday";
	names.shortName = "Su";
	this->fWeekdaysNames[(int )counter] = names;

	counter = kMonday;
	names.longName = "Monday";
	names.shortName = "Mo";
	this->fWeekdaysNames[(int )counter] = names;

	counter = kTuesday;
	names.longName = "Tuesday";
	names.shortName = "Tu";
	this->fWeekdaysNames[(int )counter] = names;

	counter = kWednesday;
	names.longName = "Wednesday";
	names.shortName = "We";
	this->fWeekdaysNames[(int )counter] = names;

	counter = kThursday;
	names.longName = "Thursday";
	names.shortName = "Th";
	this->fWeekdaysNames[(int )counter] = names;

	counter = kFriday;
	names.longName = "Friday";
	names.shortName = "Fr";
	this->fWeekdaysNames[(int )counter] = names;

	counter = kMonday;
	names.longName = "Saturday";
	names.shortName = "Sa";
	this->fWeekdaysNames[(int )counter] = names;
}

GregorianCalendar* GregorianCalendar::Init() {
	if (!this->fCurrentObject) {
		this->fCurrentObject = new GregorianCalendar();
		if (!this->fCurrentObject) {
			// Panic
		}
	}
	return this->fCurrentObject;
}

int GregorianCalendar::fromGregorianToLocalYear(int year) { return year; }

int GregorianCalendar::fromLocalToGregorianYear(int year) { return year; }

map<int, BString> GregorianCalendar::getDayNamesForYearMonth(int gregorianYear, int month)
{
	map<int, BString> toReturn;
	int i = 0, limit;
	switch (month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			// 31 days in month
			limit = 32;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			// 30 days in month
			limit = 31;
			break;
		case 2:
			// 28 or 29 days in month
			if (GregorianCalendar::IsYearLeap(gregorianYear)) {
				limit = 30;
			} else {
				limit = 29;
			}
			break;
		default:
			// Panic!

	};
	for (i=1; i<limit; ++i) {
		toReturn[i] = this->fDaysNames[i];
	}
	return toReturn;
}

map<int, BString> GregorianCalendar::getMonthNamesForLocalYear(int localYear) {
	return map<int, BString>(this->fMonthsNames);
}

map<int, BString> GregorianCalendar::getMonthNamesForGregorianYear(int localYear) {
	return map<int, BString>(this->fMonthsNames);
}

bool GregorianCalendar::IsYearLeap(TimeRepresentation date) {
	return IsYearLeap(date.tm_year);
}

bool GregorianCalendar::IsYearLeap(int year) {
	if (year % 400 == 0) { return true; }
	if (year % 100 == 0) { return false; }
	if (year % 4 == 0) { return true; }
	return false;
}

/*! \function 		GregorianCalendar::IsDateValid
 *	\brief			Checks if the date represents a valid date in Gregorian calendar.
 *	\details		Gregorian calendar was proposed at 1582. It was adopted by 
 *					different countries at different years; there is no way to determine 
 *					the correct date of adoption on-the fly for every current user. 
 *					However, it may be generally assumed that the adoption did not 
 *					happen before 1600.
 *					Gregorian calendar is defined as 12 months, from 1 (January) to 12 
 *					(December). Any other month is illegal.
 *					Every month is from 1 to 28, 29, 30 or 31 days long.
 *					Hours and minutes are between -24:-59 and 24:59.
 */
bool GregorianCalendar::IsDateValid(TimeRepresentation date) {	
	int limit;
	if (date.tm_year < 1600) { return false; }		// 

	if (date.tm_month <= 0 || date.tm_month > 12) { return false; }		// 

	if (date.tm_day <= 0) { return false; }
	switch (date.tm_month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			// 31 days in month
			limit = 32;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			// 30 days in month
			limit = 31;
			break;
		case 2:
			// 28 or 29 days in month
			if (GregorianCalendar::IsYearLeap(date.tm_year)) {
				limit = 30;
			} else {
				limit = 29;
			}
			break;
		default:
			// Panic!
	};
	if (date.tm_day >= limit) { return false; }
	if (date.tm_hour > 24 || date.tm_hour < -24) { return false; }
	if (date.tm_min > 59 || date.tm_min < -59) { return false; }
	return true;
}

/*! \function		GregorianCalendar::GetWeekDayForLocalDate
 *	\brief			Calculate the day of week for a given date
 *	\details		The only useful fields are "year", "month" and "day",
 *					since it's assumed that the week day is unknown.
 *					Working according to the Zeller's congruence
 *					(Wikipedia: http://en.wikipedia.org/wiki/Zeller's_congruence)
 *	\return			The corresponding day of week - as defined in the WeekDays enum.
 *	\param	date	Struct tm describing the date for which the day of week is needed.`
 *	\sa		struct tm, enum WEEKDAYS.
 */
WeekDays GregorianCalendar::GetWeekDayForLocalDate(TimeRepresentation date) {
	enum WEEKDAYS toReturn = kInvalid;
	 
	int dayOfWeek, year = date.tm_year;	
	int q = date.tm_year, k = q % 100, j = (int)q/100, h;
	int m = date.tm_month; if (m < 3) { m += 12; }	// Month is from 3 (March) to 14 (February)

	if (! IsDateValid(date)) { return kInvalid; }

	h = ((int)((int )((q + (int)(((m+1)*26)/10) + k + (int)(k/4) + (int)(j/4) + 5*j)%7) + 5) % 7) + 1;

	// Now h is from 1 - which is Monday - to 7 - which is Sunday.
	if (h < 1 || h > 7) {
		toReturn = kInvalid;
	} else {
		toReturn = kMonday; --h;	// h is between 0 and 6 including
		toReturn <<= h;				// shifting from 0 to 6 bytes left.
	}
	return toReturn;
}
// <-- end of function GregorianCalendar::GetWeekDayForLocalDate

TimeRepresentation& 