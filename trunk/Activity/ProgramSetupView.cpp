/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
 
// Project includes
#include "ActivityData.h"
#include "ProgramSetupView.h"
#include "Utilities.h"

// OS includes
#include <Directory.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <GridLayout.h>
#include <GroupLayout.h>
#include <InterfaceDefs.h>
#include <LayoutItem.h>
#include <Looper.h>
#include <Node.h>


/*---------------------------------------------------------------------------
 *									Messages' constants
 *--------------------------------------------------------------------------*/

const uint32		kProgramActivityCheckBoxToggled				= 'PACB';
const uint32		kProgramActivityCommandLineOptionsChanged	= 'PACO';
const uint32		kProgramActivityStartFileSearch				= 'PAFS';
const uint32		kProgramActivityFileChosen						= 'PAFC';



/*---------------------------------------------------------------------------
 *									Useful colors
 *--------------------------------------------------------------------------*/

const rgb_color	kEnabledTextColor			= ui_color( B_CONTROL_TEXT_COLOR );
const rgb_color	kDisabledTextColor		= { 108, 108, 108, 255 };



/*---------------------------------------------------------------------------
 *						Implementation of class ProgramFileFilter
 *--------------------------------------------------------------------------*/
bool	ProgramFileFilter::Filter( const entry_ref *ref,
											BNode* node,
											struct stat_beos* st,
											const char* filetype )
{
	BString 		fileType( filetype );
	BDirectory 	testDir( ref );
	BEntry		tempEntry;
	BNode			tempNode;
	char			buffer[ 255 ];

	// Allow all directories	
	if ( testDir.InitCheck() == B_OK ) {
		return true;
	}
	
	if ( ( fileType.IFindFirst( "application/x-be-executable" ) == 0 ) ||
	     ( fileType.IFindFirst( "application/x-vnd.Be-elfexecutable" ) == 0 ) )
	{
		return true;
	}
	
	if ( fileType.IFindFirst( "application/x-vnd.Be-symlink" ) == 0 )
	{
		if ( ( B_OK == tempEntry.SetTo( ref, true ) ) &&
		     ( B_OK == tempNode.SetTo( &tempEntry ) ) &&
		     ( 0 != tempNode.ReadAttr( "BEOS:TYPE", B_STRING_TYPE, 0, buffer, 255 ) ) &&
		     ( NULL != fileType.SetTo( buffer ) ) &&		// This check is really unnecessary
		     ( ( fileType.IFindFirst( "application/x-be-executable" ) == 0 ) ||
	     		 ( fileType.IFindFirst( "application/x-vnd.Be-elfexecutable" ) == 0 ) ) )
	   {
			return true;
	   }
	}
	return false;
}	// <-- end of function ProgramFileFilter::Filter


/*---------------------------------------------------------------------------
 *						Implementation of class ProgramSetupView
 *--------------------------------------------------------------------------*/

/*!	\brief		Constructor for the class ProgramSetupView.
 */
ProgramSetupView::ProgramSetupView( BRect frame, const char *name, ActivityData* data )
	:
	BView( frame, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS ),
	fData( data ),
	fLastError( B_OK ),
	fCheckBox( NULL ),
	fOutline( NULL ),
	fLabel( NULL ),
	fFileName( NULL ),
	fOpenFilePanel( NULL ),
	fFilePanel( NULL ),
	fRefFilter( NULL ),
	fCommandLineOptionsInput( NULL )
{
	fCommandLineOptions.Truncate( 0 );
	
	// Set the layout for the global view
	BGroupLayout* groupLayout = new BGroupLayout( B_VERTICAL );
	if ( !groupLayout ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	groupLayout->SetInsets( 5, 5, 5, 5 );
	this->SetLayout( groupLayout );
	
	// Create the enable / disable checkbox
	BMessage* toSend = new BMessage( kProgramActivityCheckBoxToggled );
	if ( !toSend ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}

	fCheckBox = new BCheckBox( BRect( 0, 0, 1, 1 ),
										"Enable running program",
										"Run a program",
										toSend );
	if ( !fCheckBox ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fCheckBox->ResizeToPreferred();
	
	// Create the outline
	fOutline = new BBox( ( this->Bounds() ).InsetBySelf( 5, 5 ),
								"Program Setup Outline" );
	if ( !fOutline ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fOutline->SetLabel( fCheckBox );
	BLayoutItem* layoutItem = groupLayout->AddView( fOutline );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH,
																	 B_ALIGN_USE_FULL_HEIGHT ) );
	}
	
	// Create the internal layout
	BGridLayout* gridLayout = new BGridLayout();
	if ( !gridLayout ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	gridLayout->SetInsets( 10, ( fCheckBox->Bounds() ).Height(), 10, 10 );
	gridLayout->SetSpacing( 5, 2 );
	gridLayout->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH,
																 B_ALIGN_USE_FULL_HEIGHT ) );
	fOutline->SetLayout( gridLayout );
	
	// Create the explanation string
	fLabel = new BStringView( BRect( 0, 0, 1, 1 ),
									  "Program setup label",
									  "Select the program:" );
	if ( !fLabel ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fLabel->ResizeToPreferred();
	layoutItem = gridLayout->AddView( fLabel, 0, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_LEFT, B_ALIGN_MIDDLE ) );
	}
	
	// Create the message for button which opens the file panel
	toSend = new BMessage( kProgramActivityStartFileSearch );
	if ( !toSend ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	
	// Create the button itself
	fOpenFilePanel = new BButton( BRect( 0, 0, 1, 1 ),
											"Program setup open file panel button",
											"Choose",
											toSend );
	if ( !fOpenFilePanel ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fOpenFilePanel->ResizeToPreferred();
	layoutItem = gridLayout->AddView( fOpenFilePanel, 2, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_RIGHT, B_ALIGN_MIDDLE ) );
	}
	
	// Create the BStringView with name of currently chosen file.
	fFileName = new BStringView( BRect( 0, 0, 1, 1 ),
										  "Currently chosen program file",
										  "No file" );
	if ( !fFileName ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fFileName->ResizeToPreferred();
	layoutItem = gridLayout->AddView( fFileName, 0, 2, 3, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_CENTER, B_ALIGN_MIDDLE ) );
	}	
	
	// Last, create the BTextControl with command line options
	toSend = new BMessage( kProgramActivityCommandLineOptionsChanged );
	if ( ! toSend ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fCommandLineOptionsInput = new BTextControl( BRect( 0, 0, 1, 1 ),
																"Command line options editor",
																"Command line params:",
																NULL,		// Contents will be set later
																toSend );
	if ( !fCommandLineOptionsInput ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fCommandLineOptionsInput->ResizeToPreferred();
	( fCommandLineOptionsInput->TextView() )->SetMaxBytes( ACTIVITY_MAX_ALLOWED_COMMAND_LINE_OPTIONS_LENGTH );
	layoutItem = gridLayout->AddView( fCommandLineOptionsInput, 0, 3, 3, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH, B_ALIGN_MIDDLE ) );
	}	

	UpdateInitialValues();
	
	gridLayout->SetColumnWeight( 0, 100 );
	gridLayout->SetColumnWeight( 1, 1 );
	
	fLastError = B_OK;
	
}	// <-- end of constructor for class ProgramSetupView



/*!	\brief		Updates initial values of the control upon startup.
 */
void			ProgramSetupView::UpdateInitialValues()
{
	bool enabled;
	BString fileName;
	
	if ( fData ) {
		enabled = fData->GetProgram( &fPathToFile, &fCommandLineOptions );
	}
	else
	{
		enabled = false;
		fileName.SetTo( "No program" );
		fCommandLineOptions.Truncate( 0 );
		find_directory( B_BEOS_BIN_DIRECTORY, &fPathToDirectory );
	}
	
	if ( fPathToFile.InitCheck() == B_OK ) {
		fileName.SetTo( fPathToFile.Leaf() );
		fPathToFile.GetParent( &fPathToDirectory );
		if ( fPathToDirectory.InitCheck() != B_OK ) {
			find_directory( B_BEOS_BIN_DIRECTORY, &fPathToDirectory );
		}
	}
	else
	{
		enabled = false;
		fileName.SetTo( "No program" );
		find_directory( B_BEOS_BIN_DIRECTORY, &fPathToDirectory );
	}
	
	// At this point, fPathToFile may be not initialized, but it doesn't matter
	// as long as the directory is initialized.
	
	fFileName->SetText( fileName.String() );
	
	if ( enabled ) {
		fCheckBox->SetValue( 1 );
		fCommandLineOptionsInput->SetEnabled( true );
		fFileName->SetHighColor( kEnabledTextColor );
		// No need to correct colors - other UI elements still weren't touched.
	} else {
		fCheckBox->SetValue( 0 );
		fFileName->SetHighColor( kDisabledTextColor );
		fCommandLineOptionsInput->SetEnabled( false );
		fOpenFilePanel->SetEnabled( false );
	}
	
	// Create the reference filter
	if ( !fRefFilter ) {
		fRefFilter = new ProgramFileFilter();
	}
	if ( !fRefFilter ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	
}	// <-- end of function ProgramSetupView::UpdateInitialValues



/*!	\brief		Destructor for the class ProgramSetupView
 */
ProgramSetupView::~ProgramSetupView() {
	
	if ( fOutline ) {
		fOutline->RemoveSelf();
		delete fOutline;
	}
	
	if ( fFilePanel ) {
		delete fFilePanel;
	}
	
	if ( fRefFilter ) {
		delete fRefFilter;
	}
	
	if ( fThisMessenger ) {
		delete fThisMessenger;
	}
	
	if ( fCommandLineOptionsInput ) {
		delete fCommandLineOptionsInput;
	}	
}	// <-- end of destructor for ProgramSetupView



/*!	\brief		Add current handler to looper
 */
void		ProgramSetupView::AttachedToWindow() {
	BView::AttachedToWindow();
	if ( this->Parent() ) {
		this->SetViewColor( this->Parent()->ViewColor() );
	}
	
	if ( this->Looper() && this->Looper()->Lock() ) {
		this->Looper()->AddHandler( this );
		this->Looper()->Unlock();
	}
	
	// Set targets
	if ( fCheckBox ) {
		fCheckBox->SetTarget( this );
	}
	if ( fOpenFilePanel ) {
		fOpenFilePanel->SetTarget( this );
	}
	if ( fCommandLineOptionsInput ) {
		fCommandLineOptionsInput->SetTarget( this );
	}
	
	// This is the first place where we can construct our own BMessenger, since
	// we didn't have the Looper earlier. 
	if ( !fThisMessenger ) {
		fThisMessenger = new BMessenger( this, this->Looper() );
	}
	if ( !fThisMessenger ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}

}	// <-- end of function ProgramSetupView::AttachedToWindow



/*!	\brief		Main function of the class
 */
void 		ProgramSetupView::MessageReceived( BMessage* in ) {

	entry_ref	tempRef;
	BEntry		programFile;
	
	if ( !in ) { return; }
	
	switch ( in->what ) {
		case kProgramActivityCheckBoxToggled:
			if ( this->fCheckBox ) {
				if ( this->fCheckBox->Value() != 0 ) {
					
					// UI changes
					if ( this->fOpenFilePanel ) {
						fOpenFilePanel->SetEnabled( true );
					}
					if ( this->fFileName ) {
						this->fFileName->SetHighColor( kEnabledTextColor );
						this->fFileName->Invalidate();
					}
					if ( this->fCommandLineOptionsInput ) {
						fCommandLineOptionsInput->SetEnabled( true );
					}
					
					// Saved activity changes
					if ( fData ) {
						fData->SetProgram( true );
					}
				}
				else
				{
					// UI changes
					if ( this->fOpenFilePanel ) {
						fOpenFilePanel->SetEnabled( false );
					}
					if ( this->fFileName ) {
						fFileName->SetHighColor( kDisabledTextColor );
						this->fFileName->Invalidate();
					}
					if ( this->fCommandLineOptionsInput ) {
						fCommandLineOptionsInput->SetEnabled( false );
					}
					
					// Saved ativity changes
					if ( fData ) {
						fData->SetProgram( false );
					}
				}
			}
			break;
		
		case kProgramActivityCommandLineOptionsChanged:
		
			if ( fData && fCommandLineOptionsInput ) {
				fData->SetProgramOptions( BString( fCommandLineOptionsInput->Text() ) );
			}
		
			break;
			
		case kProgramActivityStartFileSearch:
			fLastError = CreateAndShowFilePanel();
			if ( fLastError != B_OK ) {
				utl_Deb = new DebuggerPrintout( "Couldn't create the file panel!" );
			}
			break;
			
		case kProgramActivityFileChosen:
		
			if ( ( B_OK != in->FindRef( "refs", &tempRef ) ) ||
			     ( B_OK != programFile.SetTo( &tempRef, true ) ) )
			{
				// Didn't succeed to get the new entry
				break;
			}
			// Ok, now soundFile is initialized to the file user wants to hear.
			
			// Set the paths to file and directory
			if ( B_OK == fPathToFile.SetTo( &programFile ) )
			{
				fPathToFile.GetParent( &fPathToDirectory );
				
				// Update the saved activity data
				fData->SetProgramPath( fPathToFile );
				
				// Update the name of the program
				if ( this->fFileName ) {
					fFileName->SetText( fPathToFile.Leaf() );
					fFileName->ResizeToPreferred();
					fFileName->Invalidate();
				}
			}
			
			break;
			
		case B_CANCEL:		// Intentional fall-through
		default:
			BView::MessageReceived( in );
	};	
}	// <-- end of function ProgramSetupView::MessageReceived



/*!	\brief		Create and open the file panel for selecting the sound file.
 *		\return		B_OK	if everything went good. \n Some other constant, if not.
 */
status_t		ProgramSetupView::CreateAndShowFilePanel()
{
	// Note: the message is constructed on the stack, and will be freed when exitting.
	BMessage toSend( kProgramActivityFileChosen );

	if ( !fFilePanel )
	{
		fFilePanel = new BFilePanel( B_OPEN_PANEL,
											  fThisMessenger,	// Hopefully this was set earier
											  NULL,			// Panel directory will be set later
											  0,				// B_FILE_NODE only
											  false,			// Disallow multiple selection
											  &toSend,		// Message to be sent
											  fRefFilter );// Filter of the entries
		if ( !fFilePanel ) {
			/* Panic! */
			fLastError = B_NO_MEMORY;
			return B_NO_MEMORY;
		}
		fFilePanel->SetPanelDirectory( fPathToDirectory.Path() );
	}
	
	fFilePanel->Show();
	
	return B_OK;
}	// <-- end of function "CreateAndShowFilePanel"



void		ProgramSetupView::FrameResized( float width, float height )
{
	BView::FrameResized( width, height );
	
	if ( fOutline && fOutline->GetLayout() )
	{
		( fOutline->GetLayout() )->SetFrame( this->Frame() );
		this->Invalidate();
		this->Relayout();
	}
	
}
