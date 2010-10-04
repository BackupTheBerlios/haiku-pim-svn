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
const uint32	kYearChanged		= 'YEAR';
const uint32	kOpenDateSelector 	= 'DATE';
const uint32	kTextModified		= 'MODI';

/*!	\enum	DmyOrder
 *	\brief	Defines the order of the elements in the control string.
 */
typedef enum DMY_ORDER {
	kDayMonthYear,
	kMonthDayYear,
	kYearMonthDay
} DmyOrder;


/*
class DayItem 
	: 
	public BMenuItem
{
	public:
//		DayItem(BString date) {};
	
	
};
*/

class MonthMenu
	:
	public BMenu
{	
	public:
		MonthMenu(BRect frame);
		
		
};

/*!	\class	CalendarControl
 *	\brief	The control used to get and set calendar data.
 */
class CalendarControl 
	:
	public BView
{
private:
	BStringView* label;
	BMenuField* menuField;
//	BTextView* textView;
	TimeRepresentation representedTime;
//	BButton* openMenuButton;
	BMenu* dateSelector;
//	BMenuBar* openMenuButton;
	CalendarModule* calModule;
	char separator;
	DmyOrder orderOfElements;
	void UpdateTargets(BView* in);

protected:
	virtual void Init();
	virtual void CreateMenu(void);
	virtual BMenu* CreateMonthsMenu(map<int, DoubleNames> &listOfMonths);
	virtual BMenu* CreateYearsMenu(int localYear);
	
	
	virtual BString BuildDateRepresentationString(bool useLongMonthNames = true);

public:
	CalendarControl(BRect frame,
					const char* name,
					const char* label);
	virtual ~CalendarControl(void);

	inline virtual void SetSeparator(char sep) { separator = sep; this->UpdateText(); }
	inline virtual void SetOrder(DmyOrder toSet) { orderOfElements = toSet; this->UpdateText(); }

	virtual void UpdateText();
	
	virtual void FrameResized(float width, float height);
	virtual void AttachedToWindow();

	inline virtual void SelectTime(const TimeRepresentation &toSet) {};
	
	virtual void MakeFocus(bool focused = true);
	
	virtual void MessageReceived (BMessage* in);
};
// <-- end of class CalendarControl

#endif	// __CALENDAR_CONTROL_H__
