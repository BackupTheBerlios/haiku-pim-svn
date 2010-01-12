#ifndef __TIME_BOX_H__
#define __TIME_BOX_H__

/*!	\file		TimeBox.h
	\brief 		This file defines the TimeBox control.
	\author		Alexey Burshtein  
	\note		Haifa University - Project course
		\par	A part of the "Eventual" Haiku Calendar / Organizer
		\par 	Distributed under MIT license.
	\date 		2009-2010
*/

#include <InterfaceKit.h>

/*!	\struct		TimeRepresentation
	\brief		A simple representation of hours and minutes.
	\note		Note that the hours and minutes may be negative!
*/
typedef struct _timeRep {
	int hours;
	int minutes;
} TimeRepresentation;

/*!	\class 		TimeBox
	\brief		A general class for representing a box with possibility
					to choose time.
	
	This class is intended to be derived from.
*/
class TimeBox : public BControl 
{
	protected:
		int hours;
		int minutes;
		char* label;
		
		BLabel* label;
		BMenuBar *menubar;
		BMenu *hoursMenu;
		BMenu *minutesMenu;
		
		virtual void BuildHoursMenu();
		virtual void BuildMinutesMenu();
		
	public:
		TimeBox(BRect rectIn,
				const char* name,
				const char* label,
				BMessage* message,
				uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				uint32 flags = B_WILL_DRAW | B_NAVIGABLE | 
					B_FULL_UPDATE_ON_RESIZE | B_NAVIGABLE_JUMP,
				bool embedCheckBox = false,
				const char* checkBoxLabel = NULL,
				bool isCheckBoxChecked = false);
		virtual ~TimeBox();

		// Time setters and getters derived from BControl		
		virtual void MessageReceived(BMessage message);
		virtual int32 Value();	// See note of the next function
		virtual void SetValue(int32 valueIn);	// hours are first 16 bits,
												// minutes are last 16 bits.
		// Class-specific time setters
		virtual bool SetTime(time_t timeIn);
		virtual bool SetTime(TimeRepresentation timeIn);
		virtual TimeRepresentation GetTime();

		// Enabled / Disabled functionality
		virtual bool IsEnabled();
		virtual void SetEnabled(bool stateIn);
		// Enabled / Disabled functionality of the embedded checkbox
		virtual bool IsCheckBoxEnabled();
		virtual void SetCheckBoxEnabled(bool stateIn);
		
		// Label manipulation. Note: standard BControl's label manipulation
		// works only on control's label; manipulation of the embedded 
		// checkbox label is performed through additional API.
		virtual void SetLabel(const char* labelIn);
		const char* Label() const;
		virtual void SetCheckBoxLabel(const char* labelIn);
		const char* CheckBoxLabel() const;
};
// <-- end of class BControl

#endif	// __TIME_BOX_H__
