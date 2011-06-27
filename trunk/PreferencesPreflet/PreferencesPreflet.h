/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _PREFERENCES_PREFLET_H_
#define _PREFERENCES_PREFLET_H_

#include <SupportDefs.h>

#include <Archivable.h>
#include <List.h>
#include <Message.h>

/*! \class 		CalendarModulePreferences
 *	\brief		These preferences define every calendar module.
 * 	\details	There's an instance of this class for every
 *				calendar module in system. It defines weekends
 *				and starting day in week. Note that length of
 *				the week is not configurable: weekends can be
 *				different in different countries, but Gregorian
 *				Calendar defines the week as 7 days worldwide.
 */
class CalendarModulePreferences
	:
	public BArchivable
{
private:
	BList weekends;	//!< List of the weekends for this module
	uint32	uiFirstDayOfWeek;	//!< Duh
	BString id;		//!< ID of the relevant calendar module
public:
	CalendarControlPreferences();
	CalendarControlPreferences( BMessage* archive );
	static BArchivable* Instantiate ( BMessage* archive );
	
	void 	AddToWeekends (uint32 day);
	BList 	GetWeekends (void);
	
	void 	SetFirstDayOfWeek(uint32 day);
	uint32	GetFirstDayOfWeek( void );
	
	virtual status_t Archive( BMessage* archive, bool deep = true );	
};	// <-- end of class CalendarControlPreferences


class HourMinControlPreferences
	:
	public BArchivable
{
private:
	bool bIsTime24Hours;
	
public:
	
	
};	// <-- end of class HourMinControlPreferences


class Preferences
	:
	public BArchivable
{
	HourMinControlPreferences hmControlPrefs;
	
	
	
	
};	// <-- end of class Preferences



class PreferencesApplicationWindow
	:
	public BWindow	
{
public:
	
	
private:

};	// <-- class PreferencesApplication

#endif //  _PREFERENCES_PREFLET_H_
