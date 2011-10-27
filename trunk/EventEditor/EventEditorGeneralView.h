/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _EVENT_EDITOR_GENERAL_VIEW_H_
#define _EVENT_EDITOR_GENERAL_VIEW_H_

// OS includes
#include <Box.h>
#include <SupportDefs.h>
#include <TextControl.h>
#include <View.h>

// Project includes
#include "CalendarControl.h"
#include "CategoryItem.h"
#include "Event.h"
#include "TimeHourMinControl.h"
#include "Utilities.h"

/*!	\brief	Provides the way to editing some options of the Event data.
 */
class EventEditor_GeneralView
	:
	public BView
{
	public:
		EventEditor_GeneralView( BRect frame, EventData* data );
		~EventEditor_GeneralView();
		
		virtual	status_t		InitCheck() const { return _LastError; }

		virtual	void			MessageReceived( BMessage* in );
		virtual 	void			AttachedToWindow();

	protected:
		// Data holders
		EventData* 	fData;
		status_t		_LastError;
		
		TimeRepresentation	fStartTime;
		TimeRepresentation	fEndTime;
		CalendarModule*		fCalModule;
		time_t					fDuration;
		
		// UI elements
		BTextControl*	_EventName;
		BTextControl*	_Location;
		CategoryMenu*	_CategoryMenu;
		BMenuField*		_CategoryMenuField;
		BCheckBox*		_EventLastsWholeDays;
		BCheckBox*		_EventIsPrivate;
		
		// Start time selector
		BBox*				_StartMomentSelector;
		TimeHourMinControl*		_StartTimeHourMinControl;
		CalendarControl*			_StartDateControl;
		
		// End time selector
		BBox*				_EndMomentSelector;
		TimeHourMinControl*		_EndTimeHourMinControl;
		CalendarControl*			_EndDateControl;
		
		// Service functions
		BBox*				CreateStartMomentSelector();
		
};	// <-- end of class EventEditor_GeneralView

#endif // _EVENT_EDITOR_GENERAL_VIEW_H_
