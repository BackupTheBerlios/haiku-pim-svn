/*!	\file		PreferencesPrefletMainWindow.cpp
 *	\brief		Implementation of main window of the Eventual's preferences.
 *	\details	Based on Skeleton application by Kevin H. Patterson.
 */

//Title: Skeleton Main Window Class
//Platform: BeOS 5
//Version: 2001.12.18
//Description:
//	A class that creates the application's "main" window.
//	This class inherits from BWindow, and runs a message loop for the window.
//Copyright (C) 2001, 2002 Kevin H. Patterson

#include <Button.h>
#include <Directory.h>
#include <Errors.h>
#include <File.h>
#include <FindDirectory.h>
#include <GridLayout.h>
#include <GroupLayout.h>
#include <LayoutItem.h>
#include <Path.h>
#include <StorageDefs.h>
#include <String.h>
#include <TabView.h>

#include <stdlib.h>

#include "CalendarModule.h"
#include "GregorianCalendarModule.h"
#include "CategoryItem.h"
#include "PreferencesPrefletMainWindow.h"
#include "CalendarModulePreferences.h"
#include "EmailPreferences.h"
#include "Utilities.h"

/*!	
 *	\brief		Constructor for the window
 *	\note
 *				This constructor positiions itself in the center of the screen.
 */
PreferencesPrefletMainWindow::PreferencesPrefletMainWindow()
	:
	BWindow(
		BRect( 0, 0, 639, 479 ),	// The window will be centered on screen later
		"Eventual Preferences",
		B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE,
		B_ALL_WORKSPACES )
{	
	/*!	\note		Function contents
	 *				This function consists of two parts. First loads the old
	 *				preferences; second sets up the window look.
	 */
	/* Part 1.	Load old preferences. */
	status_t status = B_OK;
	BPath path;
	BDirectory eventualSettingsDir;
	BFile eventualSettingsFile;
	DebuggerPrintout *deb = NULL;
	
	GregorianCalendar* gregorianCalMod = new GregorianCalendar();
	global_ListOfCalendarModules.AddItem( gregorianCalMod );
	
	pref_PopulateCalendarModulePreferences( NULL );
	
	pref_PopulateEmailPreferences( NULL );
	
//	// Access the overall settings directory
//	status = find_directory( B_USER_SETTINGS_DIRECTORY,
//							 &path,
//							 true );	// Create directory if necessary
//	
//	// Descent to application's settings directory
//	path.Append( "Eventual" );
//	eventualSettingsDir.SetTo( path.Path() );
//	status = eventualSettingsDir.InitCheck();
//	switch ( status )
//	{
//		case B_ENTRY_NOT_FOUND:
//			// The directory does not exist. Create it!
//			status = eventualSettingsDir.CreateDirectory( path.Path(),
//														  &eventualSettingsDir );
//			if ( status != B_OK )
//			{
//				/* Panic! */
//				BString sb;
//				sb << "Error in creating Eventual Settings directory! Error = " << ( uint32 )status;
//				deb = new DebuggerPrintout( sb.String() );
//				exit( 1 );
//			}
//			
//			/* I assume at this point the directory is set. */
//			break;
//		
//		case B_NAME_TOO_LONG:	/* Intentional fall-through */
//		case B_BAD_VALUE:
//		case B_FILE_ERROR:
//		case B_NO_MORE_FDS:
//			deb = new DebuggerPrintout( "Name is too long, input is invalid or node is busy." );
//			exit( 1 );
//			break;
//			
//		case B_LINK_LIMIT:
//			deb = new DebuggerPrintout( "Loop is detected in the filesystem!" );
//			exit( 1 );
//			break;
//			
//		case B_BUSY:
//			deb = new DebuggerPrintout( "The directory does not exist!" );
//			exit( 1 );
//			break;
//		
//		case B_OK:				/* Everything went smooth */
//			break;
//			
//		default:
//			deb = new DebuggerPrintout( "Unknown error has occurred." );
//			break;
//		
//	};
//	
//	/* Anyway, at this point the directory is set, or we have exitted. */
//	path.Append( "Preferences" );
//	eventualSettingsFile.SetTo( path.Path(),
//							    B_READ_ONLY | B_CREATE_FILE );
//							   
//	if ( eventualSettingsFile.InitCheck() != B_OK )
//	{
//		/* Panic! */
//		exit( 1 );
//	}
//	
//	status = global_Preferences.Unflatten( &eventualSettingsFile );
//	switch ( status )
//	{
//		case B_NO_MEMORY:
//			// Not enough memory to load the preferences. Exitting...
//			exit( 1 );
//			break;
//			
//		case B_BAD_VALUE:
//		default:
//			// Set default preferences
//			deb = new DebuggerPrintout( "Populating list of categories with NULL." );
//			
//			PopulateListOfCategories( NULL );
//			
//			break;
//			
//		case B_OK:
//			// Read preferences successfully.
//			deb = new DebuggerPrintout( "Populating list of categories with Preferences message." );
//			PopulateListOfCategories( &global_Preferences );
//			break;
//	};
//	eventualSettingsFile.Unset();
	
	
	/* Part 2.  Set up the window. */
	BLayoutItem* layoutItem = NULL;
	BView* background = new BView( BWindow::Bounds(), "Background", B_FOLLOW_ALL, 0 );
	BGroupLayout* backgroundLayout = new BGroupLayout( B_VERTICAL );
	if ( !background || !backgroundLayout )
	{
		/* Panic! */
		exit(1);
	}
	
	// Set background color and prepare placeholder for the main view
	background->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	background->SetLayout( backgroundLayout );
	backgroundLayout->SetInsets( 5, 5, 5, 5 );

	// Another layout for the buttons
	BGridLayout* gridLayout = new BGridLayout();
	if ( !gridLayout )
	{
		/* Panic! */
		exit( 1 );
	}
	gridLayout->SetInsets( 0, 0, 0, 0 );
	
	backgroundLayout->AddItem( 1, gridLayout, 0 );
	gridLayout->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH, B_ALIGN_BOTTOM ) );

	
	okButton = new BButton( BRect( 0, 0, 1, 1),
				  		     "Ok button",
					   		 "Ok",
					   		 NULL );
	revertButton = new BButton( BRect( 0, 0, 1, 1),
					 	 		 "Revert button",
							     "Revert",
							     NULL );
	if ( !okButton || !revertButton )
	{
		/* Panic! */
		exit( 1 );
	}
	okButton->ResizeToPreferred();
	revertButton->ResizeToPreferred();
	
	// Lay out all items
	layoutItem = gridLayout->AddView( revertButton, 0, 0 );
	layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT, B_ALIGN_MIDDLE ) );
	
	layoutItem = gridLayout->AddView( okButton, 1, 0 );
	layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_RIGHT, B_ALIGN_MIDDLE ) );

	gridLayout->SetMaxRowHeight( 0, ( okButton->Bounds() ).Height() + 6 );
	gridLayout->SetExplicitMinSize( BSize( Bounds().Width() - 10, ( okButton->Bounds() ).Height() + 6 ) );
	gridLayout->InvalidateLayout();
	
	/* Constructing the Tab View */
	BRect r = background->Bounds();
	r.InsetBySelf( 5, 5 );
	r.bottom -= ( ( okButton->Bounds() ).Height() + 6 ) ;
	r.bottom -= 10;
	mainView = new BTabView( r, "Main view", B_WIDTH_FROM_LABEL );
	if ( !mainView ) {
		/* Panic! */
		exit( 1 );
	}
	mainView->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	
	/* Constructing the first tab */
	r.InsetBySelf( 5, 5 );
	r.bottom -= mainView->TabHeight();
	r.right -= 10;
	BTab* tab = new BTab();
	
	catPrefView = new CategoryPreferencesView( r );
	if ( !catPrefView )
	{
		/* Panic! */
		exit( 1 );
	}
	if ( this->LockLooper() )
	{
		AddHandler( catPrefView );
		this->UnlockLooper();	
	}
	mainView->AddTab( catPrefView, tab );
	tab->SetLabel( "Categories" );
	
	/* Constructing the second tab */
	emailPrefView = new EmailPreferencesView( r );
	if ( !emailPrefView ) {
		/* Panic! */
		exit( 1 );
	}
	tab = new BTab();
	mainView->AddTab( emailPrefView, tab );
	tab->SetLabel( "E-mail settings" );
	
	// Constructing the third tab
	calModPrefView = new CalendarModulePreferencesView( r );
	if ( !calModPrefView ) {
		/* Panic! */
		exit( 1 );
	}
	tab = new BTab();
	mainView->AddTab( calModPrefView, tab );
	tab->SetLabel( "Calendar Modules\' preferenences" );
	
	
	mainView->Select( 0 );	// Selecting the first tab	
	backgroundLayout->AddView( 0, mainView, 1 );
	backgroundLayout->InvalidateLayout();
	
	BWindow::AddChild( background );
	this->SetDefaultButton( okButton );
	
	this->ResizeTo( 640, 480 );
	this->CenterOnScreen();
}


/*!	
 *	\brief			Window's destructor.
 */
PreferencesPrefletMainWindow::~PreferencesPrefletMainWindow()
{
	
	if ( emailPrefView )
	{
		emailPrefView->RemoveSelf();
		delete emailPrefView;
		emailPrefView = NULL;	
	}
}


/*!	
 *	\brief			Main function of the program
 *	\param[in]	message		The received message.
 */
void PreferencesPrefletMainWindow::MessageReceived(BMessage* message)
{
	switch( message->what )
	{
		default:
		  BWindow::MessageReceived( message );
		  break;
	}
}	// <-- end of function PreferencesPrefletMainWindow::MessageReceived



/*!	
 *	\brief			Closes the application
 */
bool PreferencesPrefletMainWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}	// <-- end of function PreferencesPrefletMainWindow::QuitRequested
