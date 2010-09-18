#include "CalendarModule.h"

BList listOfCalendarModules;

CalendarModule::CalendarModule(const BString& in)
{
	this->id.SetTo(in);
}

CalendarModule::CalendarModule(const CalendarModule &in)
{
	this->fDaysInWeek = in.fDaysInWeek;
	this->fDaysNames = in.fDaysNames;
	this->fMonthsNames = in.fMonthsNames;
	this->fWeekdaysNames = in.fWeekdaysNames;
	this->id.SetTo(in.id);
}

CalendarModule::~CalendarModule(void)
{
	this->id.Truncate(0);
}

const BString CalendarModule::Identify() {
	return this->id;
}
