/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

//Copyright (C) 2001, 2002 Kevin H. Patterson

// Project includes
#include "EventEditorApp.h"
#include "EventEditorMainWindow.h"

// OS includes
#include <GroupLayout.h>
#include <LayoutItem.h>
#include <TabView.h>

// POSIX includes
#include <stdio.h>


/*!	\brief		Constructor for the class.
 *		\details		This constructs the main window of the class
 *						and possibly loads one of two cases: either the file
 *						to be opened or the moment of time to start the new Event.
 */
EventEditorMainWindow::EventEditorMainWindow( )
	:
	BWindow(	BRect( 0, 0, 400, 500 ),
				"Event Editor",
				B_TITLED_WINDOW,
				0 ),
	genView( NULL )
{
	BView* MainView = new BView( BWindow::Bounds(),
										  "Event Editor Main View",
										  B_FOLLOW_ALL,
										  B_WILL_DRAW | B_FRAME_EVENTS );
	if ( MainView != NULL )
	{
		MainView->SetViewColor( ui_color(B_PANEL_BACKGROUND_COLOR ) );
		BWindow::AddChild( MainView );
	}
	else
	{
		global_toReturn = B_NO_MEMORY;
		be_app->PostMessage( B_QUIT_REQUESTED );
	}
	
	BGroupLayout* layout = new BGroupLayout( B_VERTICAL );
	if ( !layout ) {
		global_toReturn = B_NO_MEMORY;
		be_app->PostMessage( B_QUIT_REQUESTED );
	}
	MainView->SetLayout( layout );
	layout->SetInsets( 5, 5, 5, 5 );
	
	BTabView* tabView = new BTabView( Bounds().InsetBySelf( 5, 5 ),
												 "Tab view" );
	if ( !tabView ) {
		global_toReturn = B_NO_MEMORY;
		be_app->PostMessage( B_QUIT_REQUESTED );
	}
	layout->AddView( tabView );
	
	BRect individualTab = tabView->Bounds();
	individualTab.bottom -= tabView->TabHeight();
		
	genView = new EventEditor_GeneralView( individualTab, &fData );
	if ( !genView || genView->InitCheck() != B_OK ) {
		
		// Debugging!
		printf( "Error\n" );
		
		global_toReturn = B_NO_MEMORY;
		be_app->PostMessage( B_QUIT_REQUESTED );
	}
	
	BTab* tab = new BTab();
	
	tabView->AddTab( genView, tab );
	tab->SetLabel( "General" );
	
	this->CenterOnScreen();
}	// <-- end of constructor for MainWindow


/*!	\brief		Destuctor for the main window
 */
EventEditorMainWindow::~EventEditorMainWindow()
{
}	// <-- end of destructor for main window



/*!	\brief		Main function of the class.
 *		\param[in]	in		BMessage to respond to.
 */
void EventEditorMainWindow::MessageReceived( BMessage *in )
{
	switch( in->what )
	{
		default:
		  BWindow::MessageReceived( in );
		  break;
	}
}	// <-- end of function EventEditorMainWindow::MessageReceived



/*!	\brief		This function is called when the user wants to bail out.
 */
bool EventEditorMainWindow::QuitRequested()
{
	be_app->PostMessage( B_QUIT_REQUESTED );	// Notify the application
	return BWindow::QuitRequested();				// Close the window
}	// <-- end of function EventEditorMainWindow::QuitRequested
