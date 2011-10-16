#ifndef __CALENDAR_CONTROL_H__
#define __CALENDAR_CONTROL_H__

#include <interface/TextControl.h>
#include <interface/Rect.h>
#include <support/List.h>
#include <Menu.h>
#include <PopUpMenu.h>
#include <InterfaceKit.h>

#include <time.h>
#include <map>
#include <stdlib.h>
using namespace std;

#include "TimeRepresentation.h"
#include "CalendarModule.h"
#include "Utilities.h"
#include "Preferences.h"

extern BList listOfCalendarModules;

class CalendarModule;
class GregorianCalendar;

// Message constants
const uint32	kMonthChanged 		= 'MONT';
const uint32	kMonthDecreased	= 'MON-';
const uint32	kMonthIncreased	= 'MON+';
const uint32	kYearChanged		= 'YEAR';
const uint32	kYearDecreased		= 'YEA-';
const uint32	kYearIncreased		= 'YEA+';
const uint32	kOpenDateSelector = 'DATE';
const uint32	kTodayModified		= 'TODY';
const uint32	kReturnToToday		= 'RETD';
const uint32	kHourUpdated		= 'HOUR';
const uint32	kMinuteUpdated		= 'MINU';
const uint32	kPMToggled			= 'PMTG';




class MonthMenu
	:
	public BMenu
{	
	public:
		MonthMenu(const char * name, float width, float height);
		virtual ~MonthMenu();
		
		virtual void MouseDown(BPoint point);
};



/*!	\class	CalendarControl
 *	\brief	The control used to get and set calendar data.
 */
class CalendarControl 
	:
	public BView
{
protected:
	// Placeholder for the data
	TimeRepresentation fRepresentedTime;		//!< Represented time
	
	// UI elements
	BStringView* 	fLabel, *fDateLabel;
	BMenuBar* 		fMenuBar;
	BMenu* 		fDateSelector;

	// Internal configuration data
	BList 	fWeekends;					//!< List of weekends
	uint32 	fFirstDayOfEveryWeek;	//!< Defines when the week starts
	bool 		bIsControlEnabled;		//!< If "true", the control is enabled
	DmyOrder		fDateOrder;				//!< Defines how the date is printed
	CalendarModule* fCalModule;		//!< Which calendar module is used in this control
	rgb_color	fColorForWeekends;	//!< Which color is used to mark the weekends
	rgb_color	fColorForWeekdays;	//!< Which color is used to mark the weekdays
	rgb_color	fColorForServiceItems;	//!< Which color is used to mark the service items
	status_t		fLastError;				//!< Used to indicate problems in initialization

	// Internal functions
	virtual void InitTimeRepresentation( time_t initialSeconds = 0 );
	virtual void CreateMenu(void);
	virtual BPopUpMenu* CreateMonthsMenu(map<int, DoubleNames> &listOfMonths);
	virtual BPopUpMenu* CreateYearsMenu(int localYear);
	virtual void UpdateTargets( BMenu* menuIn = NULL );
	virtual BString BuildDateRepresentationString(bool useLongMonthNames = true);

public:
	CalendarControl(BRect frame,
					const char* name,
					const BString& labelCalendar,
					const BString& calModule,
					time_t initialTime = 0 );
	virtual ~CalendarControl(void);
	
	/*!	\brief		Used to check status of this control.
	 *		\details		Usually, last action is reliable only if this function returns B_OK.
	 */
	inline virtual status_t	InitCheck( void ) { return fLastError; }
	
	virtual void ParsePreferences( void );

	inline virtual void SetOrder( DmyOrder toSet ) { fDateOrder = toSet; this->UpdateText(); }

	virtual void UpdateText();

	virtual void AttachedToWindow();
	
	virtual void MessageReceived (BMessage* in);
	
	virtual void SetEnabled(bool toSet = true);
	inline virtual bool IsEnabled( void ) const { return bIsControlEnabled; }
	
	virtual void UpdateYearsMenu(int prevYear, int curYear);
	
	inline virtual void SetFirstDayOfWeek(uint32 day) {
			fFirstDayOfEveryWeek = day;
	}
	
	virtual uint32 GetFirstDayOfWeek(void) const { return fFirstDayOfEveryWeek; }
};
// <-- end of class CalendarControl

#endif	// __CALENDAR_CONTROL_H__
