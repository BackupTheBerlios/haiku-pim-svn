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
#include "../globals.h" 

#define HOUR_CHANGED	0xA000
#define MIN_CHANGED		0xA001

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
		int fHours;
		int fMinutes;
		char* fLabel;
		
		BLabel *fLabel;
		BMenuBar *fMenubar;
		BMenu *fHoursMenu;
		BMenu *fMinutesMenu;
		BCheckBox *fCheckBox;
		BLabel *fCheckBoxLabel;
		
		virtual void BuildHoursMenu();
		virtual void BuildMinutesMenu();
		
	public:
		TimeBox(BRect frame,
				const char* name,
				const char* label,
				BMessage* message = NULL,
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
		virtual void SetValue(int32 toSet);		// hours are first 16 bits,
												// minutes are last 16 bits.
		// Class-specific time setters
		virtual bool SetTime(time_t time);
		virtual bool SetTime(TimeRepresentation time);
		virtual TimeRepresentation GetTime();

		// Enabled / Disabled functionality
		virtual bool IsEnabled();
		virtual void SetEnabled(bool state);
		// Enabled / Disabled functionality of the embedded checkbox
		virtual bool IsCheckBoxEnabled();
		virtual void SetCheckBoxEnabled(bool state);
		
		// Label manipulation. Note: standard BControl's label manipulation
		// works only on control's label; manipulation of the embedded 
		// checkbox label is performed through additional API.
		virtual void SetLabel(const char* label);
		virtual const char* Label() const;
		virtual void SetCheckBoxLabel(const char* label);
		virtual const char* CheckBoxLabel() const;
		
		// Functions required for correct functionality of the control
		virtual void Draw(BRect updateRect);		
};
// <-- end of class BControl

#endif	// __TIME_BOX_H__
