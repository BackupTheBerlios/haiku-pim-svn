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
#include <GraphicDefs.h>

#include <String.h>
#include <MenuField.h>
#include <PopUpMenu.h>

#include "CalendarModule.h"
#include "Utilities.h"

/*------------------------------------------------------------------
 * 		Constants of the messages
 *-----------------------------------------------------------------*/
const uint32	kCalendarModuleChosen = 'CALM';
const uint32	kCalendarModuleWeekendDaySelected = 'WKDS';


/*------------------------------------------------------------------
 * 		Preferences of an individual CalendarModule
 *-----------------------------------------------------------------*/

/*! \class 		CalendarModulePreferences
 *	\brief		These preferences define every calendar module.
 * 	\details	There's an instance of this class for every
 *				calendar module in system. It defines weekends
 *				and starting day in week. Note that length of
 *				the week is not configurable: weekends can be
 *				different in different countries, but Gregorian
 *				Calendar defines the week as 7 days worldwide.
 *
class CalendarModulePreferences
	:
	public BArchivable
{
protected:
	BList weekends;	//!< List of the weekends for this module
	uint32	uiFirstDayOfWeek;	//!< Duh
	rgb_color	weekendsColor;	//!< Color that's associated with weekends
	rgb_color	weekdaysColor;	//!< Color that's associated with weekdays 
	
	BString id;		//!< ID of the relevant calendar module

public:
	CalendarModulePreferences();
	CalendarModulePreferences( BMessage* archive );
	static BArchivable* Instantiate ( BMessage* archive );
	
	void 	AddToWeekends (uint32 day);
	BList 	GetWeekends (void) const;
	
	void 	SetFirstDayOfWeek(uint32 day);
	uint32	GetFirstDayOfWeek( void ) const;
	
	void 	SetColor( rgb_color	color; bool weekends = true );
	rgb_color	GetColor( bool weekends = true ) const;
	
	virtual status_t Archive( BMessage* archive, bool deep = true );
	virtual bool operator== (const CalendarModulePreferences& other ) const;
	virtual CalendarModulePreferences operator= (const CalendarModulePreferences& other);
	
};	// <-- end of class CalendarControlPreferences


 *---------------------------------------------------------
 * 		Preferences of HourMinControl
 *-------------------------------------------------------*

class HourMinControlPreferences
	:
	public BArchivable
{
protected:
	bool bIsTime24Hours;
	
public:
	HourMinControlPreferences();
	HourMinControlPreferences( BMessage* archive );
	static BArchivable* Instantiate ( BMessage* archive );
	
	virtual bool operator==( const HourMinControlPreferences& other ) const;
	virtual HourMinControlPreferences operator= ( const HourMinControlPreferences& other);
	
};	// <-- end of class HourMinControlPreferences



 *-----------------------------------------------------------
 * 		Overall preferences
 *---------------------------------------------------------*
class Preferences
	:
	public BArchivable
{
	HourMinControlPreferences hmControlPrefs;
	CalendarModulePreferences cModulePrefs[ NUMBER_OF_CALENDAR_MODULES ];
	
	virtual bool operator==( const Preferences& other ) const;
	virtual Preferences operator= ( const Preferences& other );
	
};	// <-- end of class Preferences


/*----------------------------------------------------------------------------
 * 		Application window
 *--------------------------------------------------------------------------*/
 
class PreferencesApplicationWindow
	:
	public BWindow	
{
protected:
	bool bPreferenesChanged;
	
	Preferences		savedPreferences;	//!< A copy read from the disk
	Preferences		actualPreferences;	//!< An object edited by the user
	
public:
	PreferencesApplicationWindow();
	virtual ~PreferencesApplicationWindow();
	
	void MessageReceived( BMessage *in );
};	// <-- class PreferencesApplication


/*--------------------------------------------------------------------------
 *		Main view
 *------------------------------------------------------------------------*/
class PreferencesMainView
	:
	public BView
{
protected:
	CalendarModulePreferencesView*	calPrefs;
	

public:
	PreferencesMainView( BRect frame );
	virtual ~PreferencesMainView();
	
	
};	// <-- end of class PreferencesMainView


/*----------------------------------------------------------------------------
 *		Calendar module preferences view
 *--------------------------------------------------------------------------*
class CalendarModulePreferencesView
	:
	public BView
{
	protected:
		CalendarModulePreferences prefs[ NUMBER_OF_CALENDAR_MODULES ];	//!< The subject of all settings
		
		BMenuField* calendarModuleSelector;
		BPopUpMenu* calendarModules;
		
		BPopUpMenu*	PopulateModulesMenu();
		BBox*	CreateWeekendSelectionBox( BRect frame, BString &label );
		
	public:
		CalendarModulePreferencesView( BFrame frame, const char* name );
		virtual ~CalendarModulePreferencesView();
	
};	// <-- end of class CalendarModulePreferencesView
*/

#endif //  _PREFERENCES_PREFLET_H_
