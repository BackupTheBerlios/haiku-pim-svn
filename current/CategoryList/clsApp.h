#ifndef _clsApp_h
#define _clsApp_h
	
	#include "globals.h"

	#define ApplicationSignature "application/x-vnd.generic-SkeletonApplication"

	#include <Application.h>
	
	#include "clsMainWindow.h"
	
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
	};
	
#endif
