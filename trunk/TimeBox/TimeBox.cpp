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
*/
BMenu* TimeBox::BuildHoursMenu(void) 
{
	BMenu *toReturn = new BMenu("hoursMenu");
	BMenuItem *toAdd = NULL;
	BMessage* hourMessage = NULL;
	char label[8] = "hour_  ";
	
	if (NULL == toReturn) {
		// Error reporting
	}
	for (int i=0; i <= 23; i++) {
		sprintf(label, "hour_%02d", i);
		hourMessage = new BMessage(HOUR_CHANGED);
		if (NULL == hourMessage) {
			// Error reporting
		}	
		hourMessage->AddInt8("value", i);
		toAdd = new BMenuItem(label, hourMessage);		
		if (NULL == toAdd) {
			// Error reporting
		}	
		toReturn->AddItem(toAdd);
	}
	// <-- end of loop on hour values 0 - 23
	
	return toReturn;
}
// <-- end of function TimeBox::BuildHoursMenu(int)