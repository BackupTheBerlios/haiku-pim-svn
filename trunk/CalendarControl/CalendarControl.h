#ifndef __CALENDAR_CONTROL_H__
#define __CALENDAR_CONTROL_H__

#include <interface/TextControl.h>
#include <interface/Rect.h>
#include <InterfaceKit.h>

#include <time.h>

#include "TimeRepresentation.h"
#include "CalendarModule.h"
#include "GregorianCalendarModule.h"

extern BList listOfCalendarModules;

/*!	\enum	DMY_ORDER
 *	\brief	Defines the order of the elements in the control string.
 */
typedef enum {
	kDayMonthYear,
	kMonthDayYear,
	kYearMonthDay	
} DmyOrder;

/*!	\class	CalendarControl
 *	\brief	The control used to get and set calendar data.
 */
class CalendarControl 
	:
	public BTextControl
{
private:
	TimeRepresentation representedTime;
	BButton openMenuButton;
	BPopUpMenu dateSelector;
	CalendarModule* calModule;
	char separator;
	DmyOrder orderOfElements;

protected:
	void Init();

public:
	CalendarControl(BRect frame,
					const char* name,
					const char* label);
	virtual ~CalendarControl(void);

	inline virtual void SetSeparator(char sep) { separator = sep; this->UpdateText(); }
	inline virtual void SetOrder(DmyOrder toSet) { orderOfElements = toSet; this->UpdateText(); }

	virtual void UpdateText();

	inline virtual void SelectTime(const TimeRepresentation &toSet) {};
};
// <-- end of class CalendarControl

#endif	// __CALENDAR_CONTROL_H__