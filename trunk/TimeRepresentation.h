#pragma once

#include <time.h>
#include <string>
#include "CalendarModule.h"

class CalendarModule;

/*! \enum	WEEKDAYS
	\brief	The Calendar Module allows to check what day of week is a local date.
			Used also in the Rule for defining a weekly repeating pattern.
*/
typedef enum WEEKDAYS {
	kSunday		= 0x01,
	kMonday		= 0x02,
	kTuesday	= 0x04,
	kWednesday	= 0x08,
	kThursday	= 0x10,
	kFriday		= 0x20,
	kSaturday	= 0x40,
	kAllWeek	= 0x7F,
	kInvalid	= 0x80
} WeekDays;

/*! \class	TimeRepresentation
	\brief	This class is an expanding of the struct tm.
*/
class TimeRepresentation : public struct tm {
private:
	BString fCalendarModule;	//!< String that defines the Calendar module used for the representation

public:
	TimeRepresentation();
	TimeRepresentation(struct tm in);

	const BString GetCalendarModule(void);
	bool SetCalendarModule(const BString module);

	const struct tm GetRepresentedTime(void);
	bigtime_t GetRepresentedTimeInSeconds(void);
};




