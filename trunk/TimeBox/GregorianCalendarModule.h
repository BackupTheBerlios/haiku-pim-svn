#pragma once
#ifndef __GREGORIAN_CALENDAR_H__
#define __GREGORIAN_CALENDAR_H__

#include "CalendarModule.h"
#include "TimeRepresentation.h"

class GregorianCalendar : 
	public CalendarModule
{
private: 

	/*! \brief	This function allows to calculate leap years.
	*			A year is leap if it divides by 4, but not by 100, but yes by 400.
	*/
	static bool IsYearLeap(int year);
	static bool IsYearLeap(TimeRepresentation &date);
	
public:
	GregorianCalendar();
	GregorianCalendar(const GregorianCalendar& in);
}

#endif	// __GREGORIAN_CALENDAR_H__
