#include "HApp.h"
#include "HWindow.h"

#define APP_SIG "application/x-vnd.takamatsu.iconmenu"

HApp::HApp() :BApplication(APP_SIG)
{
	HWindow *win = new HWindow(BRect(50,50,300,300),"IconMenu");
	win->Show();
}	

HApp::~HApp()
{

}

