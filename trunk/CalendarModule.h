#pragma once

#include <time.h>
#include <ctime>
#include <support/SupportDefs.h>
#include <locale/TimeZone.h>
#include <support/String.h>
#include "TimeRepresentation.h"

class TimeRepresentation;

/*! \class	CalendarModule
	\brief	An abstract class that represents a calendar.
*/
class CalendarModule
{
protected:		// Constants for the calendar calculation
	unsigned char	fDaysInWeek;		//!< Usually it's 7. The range is from 0 to 255.
	map<int, BString> fMonthsNames;		//!< Names of the months, localized.
	map<int, BString> fDaysNames;		//!< Names of the days, localized.
	BString id;
	CalendarModule* fCurrentObject;		//!< Pointer to the only object of the class

	//! Construction and destruction
	CalendarModule();

public:
	//! These functions translate the times from one format to another.
	struct tm fromLocalCalendarToGregorian(const struct tm timeIn);
	struct tm fromGregorianCalendarToLocal(TimeRepresentation timeIn);

	//! These functions translate the years to and from the local calendar.
	int fromLocalToGregorianYear(int year) = 0;
	int fromGregorianToLocalYear(int year) = 0;
	
	//! These functions return the names of the months in given year.
	map<int, BString> getMonthNamesForGregorianYear(int gregorianYear) = 0;
	map<int, BString> getMonthNamesForLocalYear(int localYear) = 0;

	//! These functions return the localized names of the days in given year and month.
	map<int, BString> getDayNamesForGregorianYearMonth(int gregoryanYear, int month) = 0;
	map<int, BString> getDayNamesForLocalYearMonth(int localYear, int month) = 0;

	/*! \brief	This function accepts a date and answers the question if it's valid or not.
	 *			It's the caller's responcibility to call this function only when 
	 *			the constructed struct tm represents really a date.
	 */
	static bool IsDateValid(struct tm) = 0;

	WeekDays GetWeekDayForLocalDate(struct tm date) = 0;

	//! Identification
	const BString Identify(void);	
	
	virtual CalendarModule* Init();
	virtual ~CalendarModule(void);
};
