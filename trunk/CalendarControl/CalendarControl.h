#ifndef __CALENDAR_CONTROL_H__
#define __CALENDAR_CONTROL_H__

#include <interface/TextControl.h>
#include <interface/Rect.h>
#include <support/List.h>
#include <InterfaceKit.h>

#include <time.h>
#include <map.h>
using namespace std;

#include "TimeRepresentation.h"
//#include "CalendarModule.h"
//#include "GregorianCalendarModule.h"

extern BList listOfCalendarModules;

class CalendarModule;
class GregorianCalendar;

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
	BTextControl* textControl;
	BTextView* textView;
	TimeRepresentation representedTime;
	BButton* openMenuButton;
	BMenu* dateSelector;
	CalendarModule* calModule;
	char separator;
	DmyOrder orderOfElements;

protected:
	void Init();
	void CreateMenu(void);
	
	BString BuildDateRepresentationString(bool useLongMonthNames = true);

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
};
// <-- end of class CalendarControl

#endif	// __CALENDAR_CONTROL_H__
