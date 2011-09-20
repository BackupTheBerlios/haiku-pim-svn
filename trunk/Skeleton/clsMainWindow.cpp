#include <String.h>
#include <Alert.h>
#include <Point.h>
#include <Message.h>
#include <InterfaceKit.h>
//#include <ApplicationKit.h>

#include "clsApp.h"
#include "clsMainWindow.h"
#include "CalendarControl.h"
#include "GregorianCalendarModule.h"
#include "CategoryItem.h"
#include "Utilities.h"

#include <stdio.h>

 
MainView::MainView(BRect frame) 
	:
	BView(frame, 
			NULL, 
			B_FOLLOW_TOP|B_FOLLOW_LEFT, 
			B_FRAME_EVENTS|B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE)
{
	this->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	BRect tempRect = this->Bounds();
	tempRect.InsetBy(5, 5);
	tempRect.bottom = tempRect.top+70;
	
	GregorianCalendar* gregCal = new GregorianCalendar();
	if (!gregCal) { exit(1); }
	listOfCalendarModules.AddItem((void*)gregCal);
	
	
	
	CalendarControl* cont = new CalendarControl(tempRect,
				"calendar",
				"Date:");
				
	this->AddChild(cont);
	AttachedToWindow();
	cont->AttachedToWindow();
	
	tempRect.top = tempRect.bottom + 205;
	tempRect.bottom = tempRect.top + 70;
	
	HourMinControl* hmControl = new HourMinControl( tempRect,
													"time",
													"Time:" );
	if ( !hmControl ) { exit(1); }
	printf ("HMcontrol created successfully\n");
	this->AddChild( hmControl );
	AttachedToWindow();
	hmControl->AttachedToWindow();
	
	rgb_color color;
	color.set_to( 0, 0, 1, 255 );
	BString string1("Attempt патамучто Ыыы!"), string2("Category editor");
	
	ColorUpdateWindow* updateWindow = new ColorUpdateWindow( BPoint( 90, 90 ),
															 string1,
															 color,
															 true,
															 string2,
															 ( BLooper*)this->Window() );
	if ( updateWindow )
	{
//		updateWindow->Show();
	}
															 
															 
	
//	cont->SetEnabled(false);
}

void MainView::FrameResized(float width, float height) {
	BView::ResizeTo(width, height);
	BView::FrameResized(width, height);
	for (int i = 0; i < this->CountChildren(); i++) {
		((BView*)(this->ChildAt(i)))->FrameResized(width, height);
	}
}
	
void MainView::AttachedToWindow(void) {
	BView::AttachedToWindow();
	BView* child = this->ChildAt(0);
	while (!child) {
		child->AttachedToWindow();
		child->NextSibling();
	}
}


void MainView::FixupScrollbars()
{
}

clsMainWindow::clsMainWindow(const char *uWindowTitle)
:
	BWindow(
		BRect(64, 64, 320, 560),
		uWindowTitle,
		B_TITLED_WINDOW,
		0	)
{
	mainView = new MainView(BWindow::Bounds());
			
	if (mainView != NULL)
	{
		mainView->AttachedToWindow();
		
		BWindow::AddChild(mainView);
	}
	else
	{
		AppReturnValue = B_NO_MEMORY;
		be_app->PostMessage(B_QUIT_REQUESTED);
	}
}

clsMainWindow::~clsMainWindow()
{
}

BBitmap* MainView::CreateIcon(const rgb_color colorIn)
{
	BRect rect(0, 0, 15, 15);
	BBitmap* toReturn = new BBitmap(rect, B_CMAP8, true);
	BView* drawing = new BView(rect, 
								"drawer", 
								B_FOLLOW_LEFT | B_FOLLOW_TOP,
								B_WILL_DRAW);
	if (!drawing || !toReturn) { return NULL; }	
	toReturn->AddChild(drawing);
	if (toReturn->Lock()) {
		drawing->SetHighColor(kWhite);
		drawing->FillRect(rect);
		drawing->SetHighColor(kBlack);
		drawing->SetPenSize(1);
		drawing->StrokeRect(rect);
		drawing->SetHighColor(colorIn);
		drawing->FillRect(rect.InsetBySelf(1, 1));
		drawing->Sync();
		toReturn->Unlock();			
	}
	toReturn->RemoveChild(drawing);
	delete drawing;
	return toReturn;
}

void clsMainWindow::MessageReceived(BMessage * Message)
{
	DebuggerPrintout* deb = NULL;
	bool dirty = false;
	BString sb, tempString;
	BPoint pt;
	
	sb << "New message has arrived! What = 0x" << Message->what;
	deb = new DebuggerPrintout( sb.String() );
	
	switch(Message->what)
	{
/*		case (B_MOUSE_DOWN):
			pt = Message->FindPoint("where");
			sb << "Mouse down at " << pt.x << " and  " << pt.y;
			al = new BAlert("AAA", sb.String(), "Ok");
			if (al) al->Go();
*/	
		case kColorSelected:
			deb = new DebuggerPrintout("Color selected message arrived!");
			
			Message->FindBool( "Dirty", &dirty );
			Message->FindString( "New string", &tempString );
			
			sb << "Dirty: " << dirty << ", new string: " << tempString;
			deb = new DebuggerPrintout( sb.String() );
			
		default:
			
		  BWindow::MessageReceived(Message);
		  break;
	}
}

bool clsMainWindow::QuitRequested()
{
	be_app->PostMessage( B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}

void clsMainWindow::FrameResized(float w, float h) {
	BWindow::FrameResized(w, h);
	mainView->FrameResized(w, h);
//	BRect frame = this->Bounds();
//	mainView->FrameResized(frame.Width(), frame.Height());	
}
