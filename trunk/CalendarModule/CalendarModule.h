#pragma once

#include <posix/time.h>
#include <ctime>
#include <support/SupportDefs.h>
#include <locale/TimeZone.h>
#include <support/String.h>

#include "TimeRepresentation.h"

class TimeRepresentation;

struct DoubleNames {
	BString shortName;
	BString longName;
};

/*! \class	CalendarModule
	\brief	An abstract class that represents a calendar.
*/
class CalendarModule
{
protected:		// Constants for the calendar calculation
	unsigned char	fDaysInWeek;		//!< Usually it's 7. The range is from 0 to 255.
	map<int, DoubleNames> fMonthsNames;		//!< Names of the months, localized, in short and long form.
	map<int, BString> fDaysNames;			//!< Names of the days, localized.
	map<int, DoubleNames> fWeekdaysNames;	//!< Names of the weekdays, localized, in short and long form.
	BString id;							//!< Identifier of the module.

public:
	//! These functions translate the times from one format to another.
	virtual TimeRepresentation fromLocalCalendarToGregorian(const struct tm timeIn);
	virtual TimeRepresentation fromGregorianCalendarToLocal(TimeRepresentation timeIn);

	//! These functions translate the years to and from the local calendar.
	int fromLocalToGregorianYear(int year) = 0;
	virtual virtual int fromGregorianToLocalYear(int year) = 0;
	
	//! These functions return the names of the months in given year.
	virtual map<int, DoubleNames> getMonthNamesForGregorianYear(int gregorianYear) = 0;
	virtual map<int, DoubleNames> getMonthNamesForLocalYear(int localYear) = 0;

	//! These functions return the localized names of the days in given year and month.
	virtual map<int, BString> getDayNamesForGregorianYearMonth(int gregoryanYear, int month) = 0;
	virtual map<int, BString> getDayNamesForLocalYearMonth(int localYear, int month) = 0;

	/*! The following function returns map where each weekday's name is mapped to corresponding
	 *	int from the enum WEEKDAYS.
	 */
	virtual map<int, DoubleNames> getWeekdayNames(void) = 0;

	/*! \brief	This function accepts a date and answers the question if it's valid or not.
	 *			It's the caller's responcibility to call this function only when 
	 *			the constructed struct tm represents really a date.
	 */
	virtual static bool IsDateValid(TimeRepresentation& in) = 0;

	/*!	\brief	This way the caller can place a given date at a specific place in the grid.
	 */
	virtual WeekDays GetWeekDayForLocalDate(TimeRepresentation& date) = 0;

	//! Identification
	virtual const BString Identify(void);	
	
	//! Construction and destruction
	CalendarModule();
	virtual ~CalendarModule(void);

	//! Date legality verification.
	virtual TimeRepresentation& NormalizeDate(TimeRepresentation &in) = 0;

	//! Date manipulation routines
	virtual TimeRepresentation& AddTime(const TimeRepresentation &op1, const TimeRepresentation &op2) = 0;
	virtual TimeRepresentation& AddTimeTo1stOperand(TimeRepresentation &op1, const TimeRepresentation &op2) = 0;
	virtual TimeRepresentation& SubTime(const TimeRepresentation &op1, const TimeRepresentation &op2) = 0;
	virtual TimeRepresentation& SubTimeFrom1stOperand(TimeRepresentation &op1, const TimeRepresentation &op2) = 0;
};
