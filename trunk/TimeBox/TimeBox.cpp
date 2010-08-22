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

	toReturn->SetRadioMode(true);	// Set the menu to work as a radio button union
	toReturn->SetLabelFromMarked(true);	// The shown value is currently selected hours
	
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

	toReturn->SetRadioMode(true);	// Making the menu work as a radiobutton union
	toReturn->SetLabelFromMarked(true);	// The shown value will be currently selected minutes
	
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
	char label[3];		// A placeholder for the label - we'll need to update the selected items.
	BMenuItem *toUpdate = NULL;	// A placeholder for the menuitem to be selected after update.

	this->fHours = (int )(toSet & 0xFFFF0000) >> 16;
	// Using rounding to receive the closest multiplication of 5 minutes
	// Then multiplying by 5 to obtain the actual minutes value
	this->fMinutes = ((int )5)*(((int )(toSet & 0x0000FFFF)) + 2)/((int )5);
	
	// Update the hours menu
	sprintf(label, "%d", this->fHours);
	toUpdate = this->fHoursMenu->FindItem(label);
	if (NULL == toUpdate) {	// Didn't find the item to be selected
		exitValue = CANT_FIND_REQUIRED_ITEM;
		ErrorAlert *ex = 
				new ErrorAlert("Can't find the hours menu item to be updated!",
								false);	// No need to stop the program
	} else {
		toUpdate->Invoke();		// Selecting the requested hours
	}
	toUpdate = NULL;

	// Update the minutes menu
	sprintf(label, "%d", this->fMinutes);
	toUpdate = this->fMinutesMenu->FindItem(label);
	if (NULL == toUpdate) {	// Didn't find the item to be selected
		exitValue = CANT_FIND_REQUIRED_ITEM;
		ErrorAlert *ex = 
				new ErrorAlert("Can't find the minutes menu item to be updated!",
								false);	// No need to stop the program
	} else {
		toUpdate->Invoke();		// Selecting the requested minutes
	}

}
// <-- end of function TimeBox::SetValue
	
//! Function IsCheckBoxEnabled
/*! \brief	Returns if the embedded check box is enabled or not
	\return	If the checkbox is embedded, returns its state. Else, returns false.
*/
bool TimeBox::IsCheckBoxEnabled() {
	if (this->fCheckBox != NULL) {
		return this->fCheckBox->IsEnabled();
	} else {
		return false;
	}		
}
// <-- end of function IsCheckBoxEnabled

//! Function SetCheckBoxEnabled
/*! \brief	Changes the state of the embedded checkbox.
	\param	state [in] The new state of the embedded checkbox.
	\note	If there is no embedded checkbox, this function performs no action.
*/
void TimeBox::SetCheckBoxEnabled(bool state) {
	if (this->fCheckBox != NULL) {
		this->fCheckBox->SetEnabled(state);
	}
}
// <-- end of function SetCheckBoxEnabled

//! Function CheckBoxLabel
/*! \brief	Returns the label of the embedded checkbox.
	\return	If the checkbox is not embedded, returns NULL, else returns its label.
	\note	The returned string belongs to the BControl; it can't be altered.
*/
const char* TimeBox::CheckBoxLabel() const {
	char* toReturn = NULL;
	if (this->fCheckBox != NULL) {
		toReturn = this->fCheckBox->Label();
	}
	return toReturn;
}
// <-- end of function CheckBoxLabel

//! Function SetCheckBoxLabel
/*! \brief	Sets the label of the embedded checkbox.
	\param	label [in] The label to be set. It's on caller's responsibility to free it!!!
	\note	If there is no embedded checkbox, this function does nothing.
*/
void TimeBox::SetCheckBoxLabel(const char *label) {
	if (NULL == label) { return; }		// Sanity check
	if (NULL != this->fCheckBox) {
		fCheckBox->SetLabel(label);
	}
}
// <-- end of function SetCheckBoxLabel

//! Function IsEnabled
/*! \brief	Returns a boolean which is "true" if this control is enabled and "false" otherwise
*/
bool TimeBox::IsEnabled() {
	return (this->fLabel->IsEnabled());
}
// <-- end of function IsEnabled

//! Function SetEnabled
/*! \brief	Enables or disables the whole control.
	\param	state [in] If true, enables the control; else, disables it.
*/
void TimeBox::SetEnabled(bool state) {
	if (this->fLabel->IsEnabled() == state) { return; }	// No need to do anything
	this->fCheckBox->SetEnabled(state);
	this->fMenubar->SetEnabled(state);
	this->fHoursMenu->SetEnabled(state);
	this->fMinutesMenu->SetEnabled(state);
		this->fLabel->SetEnabled(state);
}
// <-- end of function SetEnabled

//! Function Draw
/*! \brief	The main drawing function of the control.
	\note	Fills the background with the background color. Then calls recursively Draw 
			of every attached child.
	\param	updateRect [in] The rectangle which needs to be updated - in internal coords.
*/
void TimeBox::Draw(BRect updateRect) {
	
}
// <-- end of function Draw

//! Function MessageReceived
/*! \brief	Performs activities when a message is received.
	\param	message [in] Pointer to the message to be treated.
*/
void TimeBox::MessageReceived(BMessage* message) {
	int8 value = 0;
	if (!msg->HasSpecifiers()) {
		switch (msg->what) {
			case HOUR_CHANGED:
				if (B_OK != message->FindInt8("value", &value)) {
					exitValue = CANT_FIND_REQUIRED_ITEM;
					ErrorAlert *ex = 
						new ErrorAlert("Can't find the selected value of hours!",
								true);	// This is serious error, the program must be stopped
				}
				this->fHours = value;	// No need in type conversion
				break;
			case MIN_CHANGED:
				if (B_OK != message->FindInt8("value", &value)) {
					exitValue = CANT_FIND_REQUIRED_ITEM;
					ErrorAlert *ex = 
						new ErrorAlert("Can't find the selected value of minutes!",
								true);	// This is serious error, the program must be stopped
				}
				this->fMinutes = value;	// No need in type conversion
				break;
			default:
				break;
		}
	}
}
// <-- end of function MessageReceived

