#ifndef _clsMainWindow_h
#define _clsMainWindow_h

	#include "globals.h"

	#include <Application.h>
	#include <View.h>
	#include <Window.h>
	#include <InterfaceKit.h>
	
	#include "IconListItem.h"


	class MainView : public BView {
	public:	
		MainView(BRect);
		virtual void FrameResized(float width, float height);
		virtual void AttachedToWindow();
		virtual void FixupScrollbars();
	private:
		BScrollView* scrollView;
		BBitmap* CreateIcon(const rgb_color colorIn);
	};
	


	
	class clsMainWindow
	:
		public BWindow
	{
	public:
		clsMainWindow(const char *uWindowTitle);
		~clsMainWindow();
		virtual void FrameResized(float width, float height);
		virtual void MessageReceived(BMessage *Message);
		virtual bool QuitRequested();
		inline virtual void SetKeyMenuBar(BMenuBar* in = NULL) { }
	private:
		MainView* mainView;
	};


#endif
