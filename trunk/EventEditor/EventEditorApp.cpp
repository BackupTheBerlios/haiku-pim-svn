/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

// OS includes
#include <SupportDefs.h>

// Project includes
#include "EventEditorApp.h"
#include "Utilities.h"
#include "Preferences.h"


/*!	\brief		Constructor for the Event Editor application.
 *		\param[in]	argc		Number of the command-line arguments
 *		\param[in]	argv		The command-line arguments themselves
 *		\attention	Though the application receives command-line
 *						arguments, it passes them to the Main View untouched,
 *						because it's the Main View that will decide what to do
 *						with this information.
 *		\par			
 *						However, the constructor does check one thing:
 *						it verifies that there is only one additional argument.
 *						Else, it prints a usage message and exits.
 */
EventEditorApplication::EventEditorApplication( int argc, char* argv[] )
	:
	BApplication(ApplicationSignature),
	fMainWindow( NULL )
{
	GregorianCalendar* gregorianCalMod = new GregorianCalendar();
	global_ListOfCalendarModules.AddItem( gregorianCalMod );
	
	status_t status = pref_PopulateAllPreferences();
	if ( status != B_OK )
	{
		utl_Deb = new DebuggerPrintout( "Did not succeed to read the preferences!" );
	}
	
	utl_RegisterFileType();
	
	
	fMainWindow = new EventEditorMainWindow();
	if ( fMainWindow != NULL )
	{
  		fMainWindow->Show();
	}
	else
	{
		be_app->PostMessage( B_QUIT_REQUESTED );
	}
}	// <-- end of constructor for EventEditorApplication



/*!	\brief		Destructor for Event Editor application.
 */
EventEditorApplication::~EventEditorApplication()
{
	if ( fMainWindow != NULL ) {
		if ( fMainWindow->LockWithTimeout( 1000000 ) == B_OK ) {
			fMainWindow->Quit();
		}
	}
}	// <-- end of destructor for Event Editor application



/*!	\brief		Called when everything is ready to go.
 */
void EventEditorApplication::ReadyToRun()
{
}

void EventEditorApplication::Pulse()
{
	//You set the pulse rate in BApplication::SetPulseRate().
}



/*!	\brief
 */
void			EventEditorApplication::ArgvReceived( int32 argc, char* argv[] )
{
	
	
}	// <-- end of function EventEditorApplication::ArgvReceived
