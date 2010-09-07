#include "clsApp.h"

clsApp::clsApp()
:
	BApplication(ApplicationSignature),
	iMainWindow(NULL)
{
  iMainWindow = new clsMainWindow("Main Window");
  if (iMainWindow != NULL)
		iMainWindow->Show();
	else
	{
		AppReturnValue = B_NO_MEMORY;
		be_app->PostMessage(B_QUIT_REQUESTED);
	}
}

clsApp::~clsApp()
{
	if (iMainWindow != NULL)
		if (iMainWindow->LockWithTimeout(30000000) == B_OK)	//Must Lock() before calling Quit()
			iMainWindow->Quit();	//Never delete a BWindow, call Quit() instead.
}

void clsApp::ReadyToRun()
{
	//A good place to set up windows, etc., just before your application is about to Run().
}

void clsApp::Pulse()
{
	//Provides a "heartbeat" for your application; a good place to blink cursors, etc.
	//You set the pulse rate in BApplication::SetPulseRate().
}
