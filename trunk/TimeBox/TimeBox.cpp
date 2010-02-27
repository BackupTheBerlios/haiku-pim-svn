#include "TimeBox.h"

/*Implementation of the TimeBox class */

//!	TimeBox constructor
/*! \brief	A general-purpose constructor for the TimeBox object.
	\param	rectIn	the rectangle in which the object will be located. Passed
			as is to the BControl constructor.
	\param	name	the name of the control, used to uniquely identify it
					afterwards.
	\param	label	a text label to be shown to the left of the control. It
					is copied, therefore the caller may delete it safely.
	\param	message	a message to be constructed when the control is changed.
					The message is NOT copied, and the caller must keep it.
					Default = null.
	\param 	resizingMode	the way this control behaves when the window is
					resized. Default = B_FOLLOW_LEFT | B_FOLLOW_TOP.
	\param	flags	determines the notifications this control receives. 
					Default = B_WILL_DRAW | B_NAVIGABLE | 
					B_FULL_UPDATE_ON_RESIZE | B_NAVIGABLE_JUMP.
	\param	embedCheckBox	boolean parameter that defines if the control
					should include also a checkbox (for example, for AM/PM
					choice). Default = false (no checkbox included).
	\param	checkBoxLabel	the label of the checkbox. If the previous 
					parameter is "false", this one is ignored.
					The label is copied, therefore the caller may delete it
					safely. Default = null.
	\param isCheckBoxChecked	sets if the checkbox defined with parameter
					embedCheckBox is created as checked ("true") or as not
					checked ("false"). If the checkbox is NOT created, this
					parameter is ignored. Default = false (checkbox is NOT
					checked).
	\note	All parameters up to and including flags are passed to the
			BControl constructor "as is", without changes.
*/
TimeBox::TimeBox(BRect frame,
				const char* name,
				const char* label,
				BMessage* message,
				uint32 resizingMode,
				uint32 flags,
				bool embedCheckBox,
				const char* checkBoxLabel,
				bool isCheckBoxChecked))
	: BControl (frame, name, NULL, message, resizingMode, flags)
{
		

}
// <-- end of TimeBox constructor

//! Function that creates and fills the menu with hours
/*! \brief Function that initializes the hours menu. 
  	\note The deriving classes should re-implement it.
  	\return The menu to be added to the control.
*/
BMenu* TimeBox::BuildHoursMenu(void) 
{
	BMenu *toReturn = new BMenu("hoursMenu");
	BMenuItem *toAdd = NULL;
	BMessage* hourMessage = NULL;
	char label[3] = "  ";
	
	if (NULL == toReturn) {
		exitValue = NOT_ENOUGH_MEMORY;
		ErrorAlert *ex = 
			new ErrorAlert("Not enough memory to create menu for hours!", true);
	}
	for (int i=0; i <= 23; i++) {
		sprintf(label, "%02d", i);
		hourMessage = new BMessage(HOUR_CHANGED);
		if (NULL == hourMessage) {
			exitValue = NOT_ENOUGH_MEMORY;
			ErrorAlert *ex = 
				new ErrorAlert("Not enough memory to create message for hours!",
					true);
		}	
		hourMessage->AddInt8("value", i);
		toAdd = new BMenuItem(label, hourMessage);		
		if (NULL == toAdd) {
			exitValue = NOT_ENOUGH_MEMORY;
			ErrorAlert *ex = 
				new ErrorAlert("Not enough memory to create menuitem for hours!",
					true);
		}	
		toReturn->AddItem(toAdd);
	}
	// <-- end of loop on hour values 0 - 23
	
	return(toReturn);
}
// <-- end of function TimeBox::BuildHoursMenu(int)

//! Function that creates and fills the minutes menu
/*! \brief Function that initializes the minutes menu.
	\note The deriving classes should re-implement it.
	\return The menu to be added to the control.
*/
BMenu* TimeBox::BuildMinutesMenu(void)
{
	BMenu *toReturn = new BMenu("minutesMenu");
	BMenuItem *toAdd = NULL;
	BMessage* minuteMessage = NULL;
	char label[3] = "  ";
	
	if (NULL == toReturn) {
		exitValue = NOT_ENOUGH_MEMORY;
		ErrorAlert *ex = 
			new ErrorAlert("Not enough memory to create menu for minutes!", true);
	}
	// The step of minutes is 5
	for (int i = 0; i <= 55; i+=5) {
		sprintf(label, "%02d", i);
		minuteMessage = new BMessage(MINUTE_CHANGED);
		if (NULL == minuteMessage) {
			exitValue = NOT_ENOUGH_MEMORY;
			ErrorAlert *ex = 
				new ErrorAlert("Not enough memory to create minute change message!",
								true);
		}
		minuteMessage->AddInt8("value", i);
		toAdd = new BMenuItem(label, minuteMessage);
		if (NULL == toAdd) {
			exitValue = NOT_ENOUGH_MEMORY;
			ErrorAlert *ex = 
				new ErrorAlert("Not enough memory to create menu item for minutes!",
								true);
		}
		toReturn->AddItem(toAdd);
	}
	// <-- end of loop on minutes 0 - 55 with step of 5
	
	return(toReturn);
}
// <-- end of function TimeBox::BuildMinutesMenu(void)

//! Function Value
/*! \brief Returns the current time set in the control
	\return The current time - first 16 bit is hours, last is minutes.
*/
int32 TimeBox::Value()
{
	int32 toReturn = 0;	// Nullify all bits
	toReturn |= ((this->fHours & 0x00FF) << 16);	// Set the bits 16-24 to hours
	toReturn |= (this->fMinutes & 0x00FF);	// Set the bits 0-8 to minutes
	return (toReturn);
}
// <-- end of function TimeBox::Value

//! Function SetValue
/*!	\brief Set the current hours and minutes to the submitted values.
	\param value [in] Bits 16-24 are hours to be set, bits 0-8 are minutes.
	\return None.
*/
void TimeBox::SetValue(int32 toSet) {
	this->fHours = (int )(toSet & 0xFFFF0000) >> 16;
	this->fMinutes = (int )(toSet & 0x0000FFFF);
}
// <-- end of function TimeBox::SetValue
	
