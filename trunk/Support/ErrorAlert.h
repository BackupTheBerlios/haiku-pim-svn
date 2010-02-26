#ifndef __ERROR_ALERT_H__
#define __ERROR_ALERT_H__

/*!	\file		ErrorAlert.h
	\brief 		This file defines the ErrorAlert - 
				class used to notify the user about any errors.
	\author		Alexey Burshtein  
	\note		Haifa University - Project course
		\par	A part of the "Eventual" Haiku Calendar / Organizer
		\par 	Distributed under MIT license.
	\date 		2009-2010
*/

#include <InterfaceKit.h>

/*!	\class 		ErrorAlert
	\brief		A simple class for representing an information box 
					informing the user about any errors.
*/
class ErrorAlert : public BAlert 
{
	public ErrorAlert(const char *title, 
					  const char *text,
					  const char *button0Label,
					  const char *button1Label = NULL,
					  const char *button2Label = NULL,
					  alert_type type = B_STOP_ALERT) :
		BAlert(title, text, button0Label, button1Label, button2Label,
				B_WIDTH_AS_USUAL, type)
	{
		this->Go();
	}
	
	public ErrorAlert(const char *text,
					  bool stop = true)
		: BAlert("Eventual error", text, "Crap!", NULL, NULL, 
				B_WIDTH_AS_USUAL, B_STOP_ALERT)
	{
		this->Go();
		if (stop) {
			exit(1);
		}	
	}
		
};
// <-- end of class ErrorAlert

#endif	// __ERROR_ALERT_H__
