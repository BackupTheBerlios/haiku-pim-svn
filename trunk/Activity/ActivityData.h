/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _ACTIVITY_DATA_H_
#define _ACTIVITY_DATA_H_


// OS includes
#include <Path.h>
#include <String.h>
#include <SupportDefs.h>

// Project includes



/*---------------------------------------------------------------------------
 *								Message constants
 *--------------------------------------------------------------------------*/

const uint32	kActivityData = 'ACTV';


/*---------------------------------------------------------------------------
 *								Declaration of class ActivityData
 *--------------------------------------------------------------------------*/

/*!	\brief		Holds data about the activity to be performed when time arrives.
 */
class ActivityData 
{
public:
	ActivityData( BMessage* in );
	virtual	~ActivityData();

	// Archive and unarchive functions
	virtual status_t	Archive( BMessage* out );
	virtual void		Instantiate( BMessage* in = NULL );
	
	// Getters and setters for the Notification
	inline virtual void	SetNotification( bool toSet ) { bNotification = toSet; }
	inline virtual void	SetNotification( bool toSet, const BString& text ) {
		bNotification = toSet;
		fNotificationText = textIn;
	}
	inline virtual void	SetNotificationText( const BString& textIn ) {
		fNotificationText = textIn;
	}
	inline virtual bool	GetNotification( BString* textOut = NULL ) const {
		if ( textOut ) { textOut->SetTo( fNotificationText; }
		return bNotification;
	}
	
	// Getters and setters for sound play
	inline virtual void	SetSound( bool toSet ) { bSound = toSet; }
	inline virtual void	SetSound( bool toSet, const BPath& pathIn ) {
		bSound = toSet;
		fSoundFile = pathIn;
	}
	inline virtual void	SetSoundFile( const BPath& pathIn ) { fSoundFile = pathIn; }
	inline virtual bool	GetSound( BPath* pathOut = NULL ) const {
		if ( pathOut ) { pathOut->SetTo( fSoundFile ); }
		return bSound;
	}
	
	// Getters and setters for the program
	inline virtual void	SetProgram( bool toSet ) { bProgramRun = toSet; }
	inline virtual void	SetProgram( bool toSet, const BPath& pathIn ) {
		bProgramRun = toSet; fProgramPath = pathIn;
	}
	inline virtual void 	SetProgram( bool toSet, const BPath& pathIn, const BString& paramsIn ) {
		bProgramRun = toSet; fProgramPath = pathIn; fCommandLineOptions = paramsIn;
	}
	inline virtual void	SetProgramPath( const BPath& pathIn ) {
		fProgramPath = pathIn;
	}
	inline virtual void	SetProgramOptions( const BString& paramsIn ) {
		fCommandLineOptions = paramsIn;
	}
	inline virtual bool	GetProgram( BPath* pathOut = NULL, BString* paramsOut = NULL ) const {
		if ( pathOut ) 	{ pathOut.SetTo( fProgramPath ); }
		if ( paramsOut )	{ paramsOut.SetTo( fCommandLineOptions ); }
		return bProgramRun;
	}
	inline virtual void	SetProgramVerified( bool toSet ) { bVerifiedByUser = toSet; }
	inline virtual void 	GetProgramVerified( void ) const { return bVerifiedByUser; }
	
	// Getters and setters for Email
	inline virtual void	SetEmail( bool toSet ) { bEmailToSend = toSet; }
	inline virtual void	SetEmailSubject( const BString& subjIn ) { fEmailSubject = subjIn; }
	inline virtual void	SetEmailContents( const BString& contIn ) { fEmailContents = contIn; }
	inline virtual void	SetEmailAddress1( const BString& addrIn ) { fEmailAddress1 = addrIn; }
	inline virtual void	SetEmailAddress2( const BString& addrIn ) { fEmailAddress2 = addrIn; }
	inline virtual void	SetEmailAddress3( const BString& addrIn ) { fEmailAddress3 = addrIn; }
	inline virtual bool	GetEmail( BString* subjOut = NULL,
											 BString* contOut = NULL,
											 BString* addr1 = NULL,
											 BString* addr2 = NULL,
											 BString* addr3 = NULL ) const
	{
		if ( subjOut ) { subjOut.SetTo( fEmailSubject ); }
		if ( contOut ) { contOut.SetTo( fEmailContents ); }
		if ( addr1 )	{ addr1.SetTo( fEmailAddress1 ); }
		if ( addr2 )	{ addr1.SetTo( fEmailAddress2 ); }
		if ( addr3 )	{ addr1.SetTo( fEmailAddress3 ); }
		return bEmailToSend;
	}
	
	
protected:

	/* Data holders */
	bool		bNotification;			//!< Should a notification be displayed?
	BString	fNotificationText;	//!< What should be the text of notification?
	
	bool		bSound;					//!< Should a sound file be played?
	BPath		fSoundFile;				//!< What is the file to play?
	
	bool		bEmailToSend;			//!< Should the Email be sent?
	BString	fEmailContents;		//!< Contents of the Email (message body)
	BString	fEmailSubject;			//!< Subject of the Email
	BString	fEmailAddress1;		//!< Address of receiver 1
	BString	fEmailAddress2;		//!< Address of receiver 2
	BString	fEmailAddress3;		//!< Address of receiver 3
	
	bool		bProgramRun;			//!< Should a program be run?
	bool		bVerifiedByUser;		//!< Did the user check the program?
	BPath		fProgramPath;			//!< Path to the program to be run
	BString	fCommandLineOptions;	//!< Additional options for the program
	
	/* Service functions */
	
};	// <-- end of class ActivityData

#endif // _ACTIVITY_DATA_H_
