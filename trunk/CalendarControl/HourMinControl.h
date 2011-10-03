/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _HOUR_MIN_CONTROL_H_
#define _HOUR_MIN_CONTROL_H_

// System includes
#include <CheckBox.h>
#include <Menu.h>
#include <StringView.h>
#include <SupportDefs.h>

// Project includes
#include "CalendarControl.h"
#include "Preferences.h"

// Messages constant
const uint32		kTimeControlUpdated	= 'TCUP';

const uint32		kHoursUpdated			= 'HoUP';
const uint32		kMinutesUpdated		= 'MiUP';
const uint32		kCheckBoxToggled		= 'CBTG';



#ifndef	SPACING
#define	SPACING	( 2 )
#endif

/*!
 * 	\brief	Allows to choose hour and minute in 5-mins steps
 */
class HourMinControl
	:
	public BView,
	public virtual CalendarBasics
{
protected:
	virtual BMenu* CreateHoursMenu();
	virtual BMenu* CreateAMHoursMenu( bool *pbPM = NULL );
	virtual BMenu* CreateMinutesMenu();
	virtual BMenuBar* CreateMenuBar();
	virtual BCheckBox* CreateCheckBox();
	virtual void BuildMatrixMenu( BMenu* toEdit );
	
	virtual void TogglePM( bool* toSet = NULL );
	
	bool fTwentyFourHoursClock;
	unsigned int hoursLimit;
	unsigned int minutesLimit;
	bool representingTime;
	bool hasCheckBox;
	BMessage* invocationMessage;
	BString fCheckBoxLabel;
	BString fLabel;
	
	
	BMenu* hoursMenu;
	BMenu* minutesMenu;
	BStringView* labelView;
	BMenuBar* chooserMenuBar;
	BCheckBox* fCheckBox;
	
		/* This is the main constructor of the class */
	virtual void InitUI( BRect bounds, BString label );

public:	
	HourMinControl( BRect bounds,
					const char* name,
					const BString& label,
					const TimeRepresentation *trIn = NULL,
					bool representingTime = false,
					const char* checkBoxLabel = NULL,
					bool initialCheckBoxValue = false,				
					unsigned int hoursLimit = 23,
					unsigned int minutesLimit = 55,
					BMessage* invocationMessage = NULL );
	~HourMinControl();

	virtual void UpdateTime(const TimeRepresentation *trIn );
	virtual void ParsePreferences(const BMessage* in);
	virtual void MessageReceived( BMessage* in);
	virtual void AttachedToWindow();
};

#endif // _HOUR_MIN_CONTROL_H_
