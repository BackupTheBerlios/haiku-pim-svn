#ifndef _clsApp_h
#define _clsApp_h
	
	#include "globals.h"

	#define ApplicationSignature "application/x-vnd.Hitech.Skeleton"

	#include <Application.h>
	
	#include "clsMainWindow.h"
	
	class MainView;
	
	class clsApp
	:
		public BApplication
	{
	public:
		clsApp();
		~clsApp();
		virtual void ReadyToRun();
		virtual void Pulse();
	private:
		clsMainWindow *iMainWindow;
		
	friend class MainView;
	};
	
#endif
