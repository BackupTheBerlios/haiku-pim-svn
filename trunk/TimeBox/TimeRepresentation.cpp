#include <string.h>
#include "TimeRepresentation.h"

/*! \function		TimeRepresentation
 *	\brief			Constructor from struct tm and BString for calendar module name.
 *	\details		It is a really bad practice to construct a TimeRepresentation
 *					object without explicitly referencing a CalendarModule.
 *	\param[in]	in			The struct tm to be initialized from.
 *	\param[in]	calModule	The calendar module to be used.
 *	\sa				CalendarModule, struct tm
 */
TimeRepresentation::TimeRepresentation(struct tm& in, BString& calModule ) {
	int length = 0;		// Used later for getting size of the 

	// Hour and minute (and irrelevant second)
	this->tm_hour = in.tm_hour;
	this->tm_min = in.tm_min;
	this->tm_sec = in.tm_sec;

	// Day, month, year
	this->tm_mday = in.tm_mday;
	this->tm_mon = in.tm_mon;
	this->tm_year = in.tm_year;

	// Other irrelevant stuff	
	this->tm_wday = in.tm_wday;
	this->tm_yday = in.tm_yday;
	
	// Copying time zone information
	this->tm_gmtoff = in.tm_gmtoff;
	this->tm_isdst = in.tm_isdst;
	if (!in.tm_zone && (length=strlen(in.tm_zone)) != 0) {
		this->tm_zone = new char[length+1];	// +1 is for NULL character
		if (!this->tm_zone) {
			// Panic!
			exit(1);
		}
		strcpy(this->tm_zone, in.tm_zone);
	} else {
		this->tm_zone = NULL;
	}

	// Copying calendar module information
	this->fCalendarModule.SetTo(calModule);
}
// <-- end of constructor of TimeRepresentation

/*!	\function		TimeRepresentation
 *	\brief			Empty constructor.
 */
TimeRepresentation::TimeRepresentation() {
	this->fCalendarModule.Truncate(0);
	this->tm_zone = NULL;
}
// <-- end of empty constructor of TimeRepresentation

/*! \function		TimeRepresentation
 *	\brief			Copy constructor
 *	\param[in]	in	Reference to TimeRepresentation from which the copy should be performed.
 *	\remarks		The created copy is deep copy. The time zone is created anew.
 */
TimeRepresentation::TimeRepresentation(TimeRepresentation &in) {
	int limit=0;
	// Simply copying the submitted struct, field by field.
	this->fCalendarModule.SetTo(in.fCalendarModule);
	this->tm_hour = in.tm_hour;
	this->tm_min = in.tm_min;
	this->tm_sec = in.tm_sec;
	this->tm_mday = in.tm_mday;
	this->tm_mon = in.tm_mon;
	this->tm_year = in.tm_year;
	this->tm_wday = in.tm_wday;
	this->tm_yday = in.tm_yday;
	this->tm_isdst = in.tm_isdst;
	this->tm_gmtoff = in.tm_gmtoff;
	// Copy the time zone information
	if (in.tm_zone && (limit=strlen(in.tm_zone))) {
		this->tm_zone = new char[limit+1];
		strcpy(this->tm_zone, in.tm_zone);		
	} else {
		this->tm_zone = NULL;
	}
}
// <-- end of copy constructor of TimeRepresentation


/*!	\function		TimeRepresentation::GetRepresentedTime
 *	\brief			Returns the time represetned by the TimeRepresentation.
 *	\details		The created representation is a deep copy. That is, the
 *					tm_zone string is cloned. All other fields are copied
 *					"as is", without any sanity checks of any kind.
 *	\returns		constant Struct tm of the represented time.
 *	\sa				TimeRepresentation constructor, struct tm.
 */
const tm TimeRepresentation::GetRepresentedTime() {
	tm out;
	int length;
	
	// Hour and minute (and second)
	out.tm_hour = this->tm_hour;
	out.tm_min = this->tm_min;
	out.tm_sec = this->tm_sec;
	
	// Day, month and year
	out.tm_year = this->tm_year;
	out.tm_mon = this->tm_mon;
	out.tm_mday = this->tm_mday;

	// Irrelevant stuff
	out.tm_wday = this->tm_wday;
	out.tm_yday = this->tm_yday;

	// Copying time zone information
	out.tm_gmtoff = this->tm_gmtoff;
	out.tm_isdst = this->tm_isdst;
	if (!this->tm_zone && (length=strlen(this->tm_zone))) {
		out.tm_zone = new char[length+1];	// +1 is for NULL character
		if (!out.tm_zone) {
			// Panic!
			exit(1);
		}
		strcpy(out.tm_zone, this->tm_zone);
	} else {
		out.tm_zone = NULL;
	}
	return out;
}
// <-- end of function TimeRepresentation::GetRepresentedTime

/*! \function		TimeRepresentation::~TimeRepresentation
 *	\brief			Default destructor of the TimeRepresentation
 *	\details		Recycles the memory for the dynamically allocated
 *					element of the structure - the time zone name. Also,
 *					removes the calendar module's name.
 *
 */
TimeRepresentation::~TimeRepresentation() {
	if (!this->tm_zone) {
		delete this->tm_zone;
		this->tm_zone = NULL;
	}
	this->fCalendarModule.Truncate(0);
}
// <-- end of destructor of TimeRepresentation

/*! \function		Assignment operator
 *	\brief			Duh?
 *	\param[in]	in	The value to which the assignment should be performed.
 *	\returns		Reference to the object overgone assignment.
 */
TimeRepresentation& TimeRepresentation::operator=(const TimeRepresentation &in) {
	int limit=0;
	// Simply copying the submitted struct, field by field.
	this->fCalendarModule.SetTo(in.fCalendarModule);
	this->tm_hour = in.tm_hour;
	this->tm_min = in.tm_min;
	this->tm_sec = in.tm_sec;
	this->tm_mday = in.tm_mday;
	this->tm_mon = in.tm_mon;
	this->tm_year = in.tm_year;
	this->tm_wday = in.tm_wday;
	this->tm_yday = in.tm_yday;
	this->tm_isdst = in.tm_isdst;
	this->tm_gmtoff = in.tm_gmtoff;
	// Copy the time zone information
	if (this->tm_zone != NULL) {
		delete this->tm_zone;
	}
	if (in.tm_zone && (limit=strlen(in.tm_zone))) {
		this->tm_zone = new char[limit+1];
		strcpy(this->tm_zone, in.tm_zone);		
	} else {
		this->tm_zone = NULL;
	}
}
// <-- end of TimeRepresentation::operator=

/*!	\function		Comparison operator
 *	\brief			Allows field-by-field comparison of two TimeRepresentations
 *	\param[in]	in	The TimeRepresentation object to be compared with.
 *	\returns		true if the objects are equal, else false.
 */
bool TimeRepresentation::operator== (const TimeRepresentation& in) {
	if ((this->fCalendarModule == in.fCalendarModule)	&&
		(this->tm_year == in.tm_year)					&&
		(this->tm_mon == in.tm_mon)						&&
		(this->tm_mday == in.tm_mday)					&&
		(this->tm_hour == in.tm_hour)					&&
		(this->tm_min == in.tm_min)						&&
		(this->tm_sec == in.tm_sec)						&&
		(this->tm_wday == in.tm_wday)					&&
		(this->tm_yday == in.tm_yday)					&&
		(this->tm_isdst == in.tm_isdst)					&&
		(this->tm_gmtoff == in.tm_gmtoff))
	{
		if (((this->tm_zone != NULL) && (in.tm_zone != NULL) && (strcmp(this->tm_zone, in.tm_zone) == 0)) || 
			((this->tm_zone == NULL) && (in.tm_zone == NULL)))
		{
			return true;
		}
	} else {
		return false;
	}
}
// <-- end of TimeRepresentation::operator==