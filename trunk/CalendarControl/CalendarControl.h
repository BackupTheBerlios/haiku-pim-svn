#ifndef __CALENDAR_CONTROL_H__
#define __CALENDAR_CONTROL_H__

#include <interface/TextControl.h>
#include <interface/Rect.h>
#include <support/List.h>
#include <InterfaceKit.h>

#include <time.h>
#include <map>
#include <stdlib.h>
using namespace std;

#include "TimeRepresentation.h"
#include "CalendarModule.h"
//#include "GregorianCalendarModule.h"

extern BList listOfCalendarModules;

class CalendarModule;
class GregorianCalendar;

// Message constants
const uint32	kMonthChanged 		= 'MONT';
const uint32	kMonthDecreased		= 'MON-';
const uint32	kMonthIncreased		= 'MON+';
const uint32	kYearChanged		= 'YEAR';
const uint32	kYearDecreased		= 'YEA-';
const uint32	kYearIncreased		= 'YEA+';
const uint32	kOpenDateSelector 	= 'DATE';
const uint32	kTodayModified		= 'TODY';
const uint32	kReturnToToday		= 'RETD';
const uint32	kHourUpdated		= 'HOUR';
const uint32	kMinuteUpdated		= 'MINU';

/*!	\enum	DmyOrder
 *	\brief	Defines the order of the elements in the control string.
 */
typedef enum DMY_ORDER {
	kDayMonthYear,
	kMonthDayYear,
	kYearMonthDay
} DmyOrder;



class DayItem 
	: 
	public BMenuItem
{
	friend class MonthMenu;
	friend class BMenu;
	
	protected:
		bool isToday;
		bool isServiceItem;
		rgb_color fFrontColor;
		rgb_color fBackColor;
		BMessage* invocationMessage;
		BHandler* targetHandler; 
	public:
		DayItem(BString date, BMessage* message);
		DayItem(const char* date, BMessage *message);
		virtual ~DayItem() {}
		
		virtual void DrawContent();
		virtual void SetTarget(BHandler* target);
		
		virtual void Fire();

//		virtual status_t Invoke(BMessage* message = NULL);
	
		virtual void SetToday(bool in = false) { isToday = in; }
		virtual void SetServiceItem(bool in = false) { isServiceItem = in; }
		inline virtual bool IsServiceItem(void) {return isServiceItem;}
		inline virtual rgb_color	GetFrontColor(void) { return fFrontColor; }
		virtual void SetFrontColor(rgb_color in);
		inline virtual rgb_color GetBackColor(void) {return fBackColor;}
		virtual void SetBackColor(rgb_color in) { fBackColor = in; }
		
//		virtual void GetContentSize(float* w, float* h);
};


class MonthMenu
	:
	public BMenu
{	
	public:
		MonthMenu(const char * name, float width, float height);
		virtual ~MonthMenu();
		
		virtual void MouseDown(BPoint point);
		virtual void Open() { 
//			Show(); 
//			AllAttached(); 
//			InvalidateLayout();
//			Track();
		}
};

/*!	\class	CalendarControl
 *	\brief	The control used to get and set calendar data.
 */
class CalendarControl 
	:
	public BView
{
private:
	BStringView* label, *dateLabel, *timeLabel;
	BMenuBar* menuBar;
	BMenuBar* timeSetting;
	BCheckBox* pm;
	
	TimeRepresentation representedTime;
	MonthMenu* dateSelector;
	CalendarModule* calModule;
	char separator;
	DmyOrder orderOfElements;
	void UpdateTargets(BMenu* in);

protected:
	BList weekends;
	uint32 firstDayOfEveryWeek;
	bool isControlEnabled;

	virtual void Init();
	virtual void CreateMenu(void);
	virtual BMenu* CreateMonthsMenu(map<int, DoubleNames> &listOfMonths);
	virtual BMenu* CreateYearsMenu(int localYear);
	
	
	virtual BString BuildDateRepresentationString(bool useLongMonthNames = true);

public:
	CalendarControl(BRect frame,
					const char* name,
					const char* labelCalendar);
	virtual ~CalendarControl(void);

	inline virtual void SetSeparator(char sep) { separator = sep; this->UpdateText(); }
	inline virtual void SetOrder(DmyOrder toSet) { orderOfElements = toSet; this->UpdateText(); }

	virtual void UpdateText();
	
	virtual void FrameResized(float width, float height);
	virtual void AttachedToWindow();

	inline virtual void SelectTime(const TimeRepresentation &toSet) {};
	
	virtual void MakeFocus(bool focused = true);
	
	virtual void MessageReceived (BMessage* in);
	
	virtual void SetEnabled(bool toSet = true);
	inline virtual bool IsEnabled(void) { return isControlEnabled; }
	
	virtual void UpdateYearsMenu(int prevYear, int curYear);
	
	inline virtual void SetFirstDayOfWeek(uint32 day) {
			firstDayOfEveryWeek = day;
	}
	
	virtual uint32 GetFirstDayOfWeek(void) { return firstDayOfEveryWeek; }
	
	virtual void AddDayToWeekends(uint32 day);
	virtual void RemoveDayFromWeekends(uint32 day);
	virtual BList GetWeekends(void) { return weekends; }
};
// <-- end of class CalendarControl

#endif	// __CALENDAR_CONTROL_H__
