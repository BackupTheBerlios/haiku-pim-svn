#pragma once
#ifndef	__TIME_REPRESENTATION_H__
#define __TIME_REPRESENTATION_H__

#include <support/String.h>
#include <support/List.h>
#include <posix/time.h>
#include <posix/string.h>
// #include <cpp/list.h>
#include "CalendarModule.h"

using namespace std;

extern BList listOfCalendarModules;

/*! \enum	WEEKDAYS
	\brief	The Calendar Module allows to check what day of week is a local date.
	\details	Used also in the Rule for defining a weekly repeating pattern.
				This Enum is build according to ISO Week Date (part of ISO 8601 standard).
				See Wikipedia http://en.wikipedia.org/wiki/ISO_week_date
*/
typedef enum WEEKDAYS {	
	kMonday		= 0x01,
	kTuesday	= 0x02,
	kWednesday	= 0x03,
	kThursday	= 0x08,
	kFriday		= 0x10,
	kSaturday	= 0x20,
	kSunday		= 0x40,
	k7DaysWeek	= 0x7F,
	kInvalid	= 0x8000
} WeekDays;

/*! \class	TimeRepresentation
	\brief	This class is an expansion of the struct tm.

	\details	TimeRepresentation is an extention of struct tm. However, since the TimeRepresentation
				is calendar-independent, all fields in its struct are disjointed from Gregorian calendar.
				As such, tm_year is NOT years since 1900, but the year itself. The tm_mon is not the
				months since January, but number of the month in current year (for Gregorian calendar,
				January is 1st month, and December is 12th, but Gregorian calendar is not the only option).
	\sa			struct tm
*/
class TimeRepresentation : public tm {
private:
	BString fCalendarModule;	//!< String that defines the Calendar module used for the representation
	bool fIsRepresentingRealDate;	//!< This variable is "true", if current object represents an actual date.	

public:
	TimeRepresentation();
	TimeRepresentation(struct tm &in, BString calModule = BString("Gregorian"));
	TimeRepresentation(TimeRepresentation &in);
	virtual ~TimeRepresentation();

	inline virtual void SetIsRepresentingRealDate(bool in) { this->fIsRepresentingRealDate = in; }
	inline virtual const bool GetIsRepresentingRealDate(void) { return this->fIsRepresentingRealDate; }
	inline virtual const BString GetCalendarModule(void) { BString a(this->fCalendarModule); return a; }
	inline virtual void SetCalendarModule(const BString &module) { this->fCalendarModule.SetTo(module); }

	virtual const tm GetRepresentedTime(void);
	

	// Operators
	virtual TimeRepresentation& operator= (const TimeRepresentation& in);
	virtual bool operator== (const TimeRepresentation &in);
//	virtual TimeRepresentation operator+ (const TimeRepresentation& op1, const TimeRepresentation &op2);
//	virtual TimeRepresentation& operator+= (const TimeRepresentation& op1);
//	virtual TimeRepresentation operator- (const TimeRepresentation& op1, const TimeRepresentation &op2);
//	virtual TimeRepresentation& operator-= (const TimeRepresentation& op1);
	virtual bool operator<(const TimeRepresentation& in);
	virtual inline bool operator<= (const TimeRepresentation &in) { return (this->operator<(in) || this->operator==(in)); }
	virtual inline bool operator> (const TimeRepresentation &in) { return (!this->operator<=(in)); }
	virtual inline bool operator>= (const TimeRepresentation &in) { return (!this->operator<(in)); }
};

#endif	// __TIME_REPRESENTATION_H__
