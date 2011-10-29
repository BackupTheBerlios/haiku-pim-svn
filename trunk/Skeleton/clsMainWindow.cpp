#include <GroupLayout.h>
#include <LayoutItem.h>
#include <String.h>
#include <Alert.h>
#include <Point.h>
#include <Message.h>
#include <InterfaceKit.h>
//#include <ApplicationKit.h>

#include "clsApp.h"
#include "clsMainWindow.h"
#include "CalendarControl.h"
#include "GeneralHourMinControl.h"
#include "TimeHourMinControl.h"
#include "GregorianCalendarModule.h"
#include "CategoryItem.h"
#include "Utilities.h"
#include "Preferences.h"

#include "ActivityData.h"
#include "ActivityView.h"

#include <stdio.h>

 
MainView::MainView(BRect frame) 
	:
	BView(frame, 
			NULL, 
			B_FOLLOW_TOP|B_FOLLOW_LEFT, 
			B_FRAME_EVENTS|B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE)
{
	this->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	BString sb;

	GregorianCalendar* gregCal = new GregorianCalendar();
	if (!gregCal) { exit(1); }
	global_ListOfCalendarModules.AddItem((void*)gregCal);

	if ( B_OK != pref_PopulateAllPreferences() )
	{
		utl_Deb = new DebuggerPrintout( "Didn't succeed to parse preferences!" );	
	}
	
	
	BMessage actDataMess;
	actDataMess.AddBool( "Notification Enabled", true );
	actDataMess.AddString( "Notification Text", "Hahhaaha" );
	
	actDataMess.AddBool( "Sound Play Enabled", true );
	actDataMess.AddString( "Sound File Path", "/boot/home/Desktop/" );
	ActivityData* actData = new ActivityData( &actDataMess );
	
	
	
	
	
	
	
	BGroupLayout* layout = new BGroupLayout( B_VERTICAL );
	if ( !layout ) { 
		// Panic! 
		exit( 1 );
	}
	this->SetLayout( layout );
	layout->SetInsets( 10, 5, 10, 5 );
	
	BRect tempRect = this->Bounds();
	tempRect.InsetBy(5, 5);
	tempRect.bottom = tempRect.top+20;
	
	
	
	
	CalendarControl* cont = new CalendarControl(tempRect,
				"calendar",
				"Date:",
				"Gregorian" );
	if ( !cont || cont->InitCheck() != B_OK ) {
		sb << "Error in creating the control! " << cont->InitCheck();
		utl_Deb = new DebuggerPrintout( sb.String() );
	} else {
		layout->AddView( cont );
	}
	
	
	cont->InitTimeRepresentation( time( NULL ) + 259200 );
	
	tempRect.top = tempRect.bottom + 10;
	tempRect.bottom = tempRect.top + 20;
	
	GeneralHourMinControl* ghmControl = new GeneralHourMinControl( tempRect,
																		"time",
																		"Time:"
																		);
	if ( !ghmControl ) { exit(1); }
	
	ghmControl->SetMinutesLimit( 38 );
	ghmControl->SetHoursLimit( 72 );
	layout->AddView( ghmControl );
	
	ghmControl->SetCurrentTime( 16, 25 );
	ghmControl->SetCheckBoxLabel( BString( "A!" ) );
	ghmControl->SetCheckBoxValue( true );

	BStringView* stringView = new BStringView( BRect( 0, 0, 1, 1 ),
															"timeDisplay",
															"00:00 false" );
	if ( ! stringView )
	{
		// Panic! 
		exit( 1 );
	}
	stringView->ResizeToPreferred();
	layout->AddView( stringView );
	
	tempRect.top = tempRect.bottom + 10;
	tempRect.bottom = tempRect.top + 20;
	
	
	BMessage* toSend = new BMessage( kTimeControlUpdated );
	TimeHourMinControl* timeControl = new TimeHourMinControl( tempRect,
															"HourMin",
															"Start time:");
	if ( ! timeControl ) { exit(1); }
	layout->AddView( timeControl );
	timeControl->SetMessage( toSend );
	
	tempRect.top = tempRect.bottom + 10;
	tempRect.bottom = tempRect.top + 130;
	ActivityView* nV = new ActivityView( tempRect,
													  "Activity View",
														actData );
	if ( !nV || nV->InitCheck() != B_OK ) {
		utl_Deb = new DebuggerPrintout( "Couldn't create view!" );
	}
	nV->ResizeToPreferred();
	layout->AddView( nV );
															
		
	AttachedToWindow();
	
//	cont->SetEnabled(false);
}

void MainView::FrameResized(float width, float height) {
	GeneralHourMinControl* child;
	
	BView::ResizeTo(width, height);
	BView::FrameResized(width, height);
	for (int i = 0; i < this->CountChildren(); i++) {
		((BView*)(this->ChildAt(i)))->FrameResized(width, height);
	}
	
	child = ( GeneralHourMinControl* )this->FindView( "time" );
		
	if (	child )
		child->SetCheckBoxLabel( BString( "A!" ) );

}
	
void MainView::AttachedToWindow(void) {
	GeneralHourMinControl* ghmControl = ( GeneralHourMinControl* )this->FindView( "time" );
	TimeHourMinControl* timeControl = ( TimeHourMinControl* )this->FindView( "HourMin" );
	BView::AttachedToWindow();
	BView* child = this->ChildAt(0);
	while (!child) {
		child->AttachedToWindow();
		child->NextSibling();
	}
	
	if ( Looper() ) {
		Looper()->AddHandler( this );
	}
	
	if ( ghmControl ) {
		ghmControl->SetTarget( this->Window() );
	}
	if ( timeControl ) {
		timeControl->SetTarget( this->Window() );
	}
}


void MainView::FixupScrollbars()
{
}

clsMainWindow::clsMainWindow(const char *uWindowTitle)
:
	BWindow(
		BRect(64, 64, 500, 560),
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
	BString sb;
	uint32	tempUint32;
	bool		tempBool;
	BStringView* stringView = NULL;
	
	switch(Message->what)
	{
		case kGeneralHourMinControlUpdated:
		case kTimeControlUpdated:
			sb.Truncate( 0 );
			
			if ( Message->FindInt32( kHoursValueKey.String(), ( int32* )&tempUint32 ) == B_OK ) {
				sb << tempUint32 << ':';
			}
			if ( Message->FindInt32( kMinutesValueKey.String(), ( int32* )&tempUint32 ) == B_OK ) {
				sb << tempUint32;
			}
			sb << ' ';
			if ( Message->FindBool( kCheckBoxValueKey.String(), &tempBool ) == B_OK ) {
				sb << ( tempBool ? "true" : "false" );
			} else {
				sb << "false(?)";
			}
			
			stringView = ( BStringView* )this->FindView( "timeDisplay" );
			if ( stringView ) {
				stringView->SetText( sb.String() );
			}
			
			break;
			
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
