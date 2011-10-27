/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

//Copyright (C) 2001, 2002 Kevin H. Patterson
//Available for use under the terms of the BSD license agreement:
//See the associated file "License.txt" for details.

#ifndef _EVENT_EDITOR_MAIN_WINDOW_H_
#define _EVENT_EDITOR_MAIN_WINDOW_H_

// OS includes
#include <Application.h>
#include <View.h>
#include <Window.h>


// Project includes
#include "EventEditorGeneralView.h"

extern uint32	global_toReturn;
	
class EventEditorMainWindow
	:
	public BWindow
{
protected:
	EventData fData;

	
	EventEditor_GeneralView* genView;
	
	
public:
	EventEditorMainWindow();
	~EventEditorMainWindow();
	virtual void MessageReceived(BMessage *Message);
	virtual bool QuitRequested();
};

#endif	// _EVENT_EDITOR_MAIN_WINDOW_H_
