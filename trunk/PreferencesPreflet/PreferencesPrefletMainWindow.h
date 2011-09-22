/*!	\file		PreferencesPrefletMainWindow.h
 *	\brief		Declaration of main window of the Eventual's preferences.
 *	\details	Based on Skeleton application by Kevin H. Patterson.
 */

//Title: Skeleton Main Window Class
//Platform: BeOS 5
//Version: 2001.12.18
//Description:
//	A class that creates the application's "main" window.
//	This class inherits from BWindow, and runs a message loop for the window.
//Copyright (C) 2001, 2002 Kevin H. Patterson

#ifndef _PREFERENCES_PREFLET_MAIN_WINDOW_H_
#define _PREFERENCES_PREFLET_MAIN_WINDOW_H_

#include "globals.h"

#include <Application.h>
#include <Button.h>
#include <TabView.h>
#include <View.h>
#include <Window.h>


#include "CategoryPreferencesView.h"

/*!	\class	PreferencesPrefletMainWindow
 *	\brief	Main window of the preflet.
 */
class PreferencesPrefletMainWindow
	:
	public BWindow
{
public:
	PreferencesPrefletMainWindow();
	~PreferencesPrefletMainWindow();
	virtual void MessageReceived(BMessage *Message);
	virtual bool QuitRequested();
	
private:
	BButton* okButton;
	BButton* revertButton;
	BTabView* mainView;
	
	CategoryPreferencesView* catPrefView;
};

#endif	// _PREFERENCES_PREFLET_MAIN_WINDOW_H_
