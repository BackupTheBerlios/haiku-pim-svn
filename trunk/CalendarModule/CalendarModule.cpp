#include "CalendarModule.h"

CalendarModule::CalendarModule(const BString& in)
{
	this->id.SetTo(in);
}

CalendarModule::~CalendarModule(void)
{
	this->id.Truncate(0);
}

const BString CalendarModule::Identify() {
	return this->id;
}