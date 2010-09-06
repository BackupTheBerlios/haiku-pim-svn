#pragma once

#include "CalendarModule.h"
#include "TimeRepresentation.h"

class GregorianCalendar : 
	public CalendarModule
{
private: 
	GregorianCalendar();

	/*! \brief	This function allows to calculate leap years.
	*			A year is leap if it divides by 4, but not by 100, but yes by 400.
	*/
	static bool IsYearLeap(int year);

	
public:
	Init();
}