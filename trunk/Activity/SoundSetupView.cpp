/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
 
// Project includes
#include "ActivityData.h"
#include "SoundSetupView.h"
#include "Utilities.h"

// OS includes
#include <Directory.h>
#include <FindDirectory.h>
#include <GridLayout.h>
#include <GroupLayout.h>
#include <InterfaceDefs.h>
#include <LayoutItem.h>
#include <Looper.h>


/*---------------------------------------------------------------------------
 *									Messages' constants
 *--------------------------------------------------------------------------*/

const uint32		kSoundActivityCheckBoxToggled			= 'SACB';
const uint32		kSoundActivityStartFileSearch			= 'SAFS';
const uint32		kSoundActivityFileChosen				= 'SAFC';


/*---------------------------------------------------------------------------
 *									Useful colors
 *--------------------------------------------------------------------------*/

const rgb_color	kEnabledTextColor			= ui_color( B_CONTROL_TEXT_COLOR );
const rgb_color	kDisabledTextColor		= { 108, 108, 108, 255 };



/*---------------------------------------------------------------------------
 *						Implementation of class SoundFileFilter
 *--------------------------------------------------------------------------*/
bool	SoundFileFilter::Filter( const entry_ref *ref,
										 BNode* node,
										 struct stat_beos* st,
										 const char* filetype )
{
	BString fileType( filetype );
	BDirectory testDir( ref );
	
	if ( testDir.InitCheck() == B_OK ) {
		return true;
	}
	
	if ( fileType.IFindFirst( "audio/" ) == 0 ) {
		return true;
	}
	return false;
}	// <-- end of function SoundFileFilter::Filter


/*---------------------------------------------------------------------------
 *						Implementation of class SoundSetupView
 *--------------------------------------------------------------------------*/

/*!	\brief		Constructor for the class SoundSetupView.
 */
SoundSetupView::SoundSetupView( BRect frame, const char *name, ActivityData* data )
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
	fRefFilter( NULL )
{
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
	BMessage* toSend = new BMessage( kSoundActivityCheckBoxToggled );
	if ( !toSend ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}

	fCheckBox = new BCheckBox( BRect( 0, 0, 1, 1 ),
										"Enable sound play",
										"Play a sound file",
										toSend );
	if ( !fCheckBox ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fCheckBox->ResizeToPreferred();
	
	// Create the outline
	fOutline = new BBox( ( this->Bounds() ).InsetBySelf( 5, 5 ),
								"Sound Setup Outline" );
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
	fOutline->SetLayout( gridLayout );
	
	// Create the explanation string
	fLabel = new BStringView( BRect( 0, 0, 1, 1 ),
									  "Sound setup label",
									  "Choose sound file:" );
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
	toSend = new BMessage( kSoundActivityStartFileSearch );
	if ( !toSend ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	
	// Create the button itself
	fOpenFilePanel = new BButton( BRect( 0, 0, 1, 1 ),
											"Sound setup open file panel button",
											"Choose",
											toSend );
	if ( !fOpenFilePanel ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fOpenFilePanel->ResizeToPreferred();
	layoutItem = gridLayout->AddView( fOpenFilePanel, 1, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_RIGHT, B_ALIGN_MIDDLE ) );
	}
	
	// Last, create the BStringView with name of currently chosen file.
	fFileName = new BStringView( BRect( 0, 0, 1, 1 ),
										  "Currently chosen sound file",
										  "No file" );
	if ( !fFileName ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	fFileName->ResizeToPreferred();
	layoutItem = gridLayout->AddView( fFileName, 0, 2, 2, 1 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_CENTER, B_ALIGN_MIDDLE ) );
	}	
	
	UpdateInitialValues();
	fLastError = B_OK;
	
}	// <-- end of constructor for class SoundSetupView



/*!	\brief		Updates initial values of the control upon startup.
 */
void			SoundSetupView::UpdateInitialValues()
{
	bool enabled;
	BString fileName;
	
	if ( fData ) {
		enabled = fData->GetSound( &fPathToFile );
	}
	else
	{
		enabled = false;
		fileName.SetTo( "No file" );
		find_directory( B_USER_DIRECTORY, &fPathToDirectory );
	}
	
	if ( fPathToFile.InitCheck() == B_OK ) {
		fileName.SetTo( fPathToFile.Leaf() );
		fPathToFile.GetParent( &fPathToDirectory );
		if ( fPathToDirectory.InitCheck() != B_OK ) {
			find_directory( B_USER_DIRECTORY, &fPathToDirectory );
		}
	}
	else
	{
		enabled = false;
		fileName.SetTo( "No file" );
		find_directory( B_USER_DIRECTORY, &fPathToDirectory );
	}
	
	// At this point, fPathToFile may be not initialized, but it doesn't matter
	// as long as the directory is initialized.
	
	fFileName->SetText( fileName.String() );
	
	if ( enabled ) {
		fCheckBox->SetValue( 1 );
		fFileName->SetHighColor( kEnabledTextColor );
		// No need to correct colors - other UI elements still weren't touched.
	} else {
		fCheckBox->SetValue( 0 );
		fFileName->SetHighColor( kDisabledTextColor );
		fOpenFilePanel->SetEnabled( false );
	}
	
	// Create the reference filter
	if ( !fRefFilter ) {
		fRefFilter = new SoundFileFilter();
	}
	if ( !fRefFilter ) {
		/* Panic! */
		fLastError = B_NO_MEMORY;
		return;
	}
	
}	// <-- end of function SoundSetupView::UpdateInitialValues



/*!	\brief		Destructor for the class SoundSetupView
 */
SoundSetupView::~SoundSetupView() {
	
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
}	// <-- end of destructor for SoundSetupView



/*!	\brief		Add current handler to looper
 */
void		SoundSetupView::AttachedToWindow() {
	BView::AttachedToWindow();
	if ( this->Parent() ) {
		this->SetViewColor( this->Parent()->ViewColor() );
	}
	
	if ( this->Looper() && this->Looper()->Lock() ) {
		this->Looper()->AddHandler( this );
		this->Looper()->Unlock();
	}
	
	if ( fCheckBox ) {
		fCheckBox->SetTarget( this );
	}
	if ( fOpenFilePanel ) {
		fOpenFilePanel->SetTarget( this );
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

}	// <-- end of function SoundSetupView::AttachedToWindow



/*!	\brief		Main function of the class
 */
void 		SoundSetupView::MessageReceived( BMessage* in ) {

	entry_ref	tempRef;
	BEntry		soundFile;
	
	if ( !in ) { return; }
	
	switch ( in->what ) {
		case kSoundActivityCheckBoxToggled:
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
					
					// Saved activity changes
					if ( fData ) {
						fData->SetSound( true );
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
					
					// Saved ativity changes
					if ( fData ) {
						fData->SetSound( false );
					}
				}
			}
			break;
			
		case kSoundActivityStartFileSearch:
			fLastError = CreateAndShowFilePanel();
			if ( fLastError != B_OK ) {
				utl_Deb = new DebuggerPrintout( "Couldn't create the file panel!" );
			}
			break;
			
		case kSoundActivityFileChosen:
		
			if ( ( B_OK != in->FindRef( "refs", &tempRef ) ) ||
			     ( B_OK != soundFile.SetTo( &tempRef, true ) ) )
			{
				// Didn't succeed to get the new entry
				break;
			}
			// Ok, now soundFile is initialized to the file user wants to hear.
			
			// Set the paths to file and directory
			if ( B_OK == fPathToFile.SetTo( &soundFile ) )
			{
				fPathToFile.GetParent( &fPathToDirectory );
				
				// Update the saved activity data
				fData->SetSoundFile( fPathToFile );
				
				// Update the name of the file
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
}	// <-- end of function SoundSetupView::MessageReceived



/*!	\brief		Create and open the file panel for selecting the sound file.
 *		\return		B_OK	if everything went good. \n Some other constant, if not.
 */
status_t		SoundSetupView::CreateAndShowFilePanel()
{
	// Note: the message is constructed on the stack, and will be freed when exitting.
	BMessage toSend( kSoundActivityFileChosen );

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
