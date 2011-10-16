/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
 
// Project includes
#include "ActivityData.h"

// OS includes
#include <Errors.h>

/*!	\brief		Read the activity data from the submitted message.
 *		\details		If the data can't be found, then some kind of defaults is used.
 */
void		ActivityData::Instantiate( BMessage* in )
{
	status_t	toReturn = B_OK;
	BString& tempString;
	bool	emailSubjectEmpty 	= false,
			emailContentsEmpty	= false,
			emailAddress1Empty	= false,
			emailAddress2Empty	= false,
			emailAddress3Empty	= false,
			
	/* Notification section */
	if ( ( !in ) || ( ( in->FindBool( "Notification Enabled", &bNotification ) ) != B_OK ) )
	{
		bNotification = false;
	}
	if ( ( !in ) || ( ( in->FindString( "Notification Text", &fNotificationText ) ) != B_OK ) )
	{
		fNotificationText.SetTo( "" );
		bNotification = false;		// No need to launch notification if no text exists
	}
	
	/* Sound play section */
	if ( ( !in ) || ( ( in->FindBool( "Sound Play Enabled", &bSound ) ) != B_OK ) )
	{
		bSound = false;
	}
	if ( ( in ) && ( ( in->FindString( "Sound File Path", &tempString ) ) == B_OK ) )
	{
		// Read the path data successfully.
		// The file may be not there, but it will be checked when the Activity launches.
		fSoundFile.SetTo( tempString.String() );
	}
	else
	{
		fSoundFile.Unset();
		bSound = false;	// No need to play something we didn't find.
	}
	
	/* Program run section */
	if ( ( !in ) || ( ( in->FindBool( "Program Run Enabled", &bProgramRun ) ) != B_OK ) )
	{
		bProgramRun = false;
	}
	if ( ( !in ) || ( ( in->FindBool( "Program Verified", &bVerifiedByUser ) ) != B_OK ) )
	{
		bVerifiedByUser = true;
	}
	if ( ( in ) && ( ( in->FindString( "Program Path", &tempString ) ) == B_OK ) )
	{
		// Read the data successfully.
		// The program file itself may not be there, but it will be checked upon launch.
		fProgramPath.SetTo( tempString.String() );
	}
	else
	{
		fProgramPath.Unset();
		bProgramRun = false;		// No need to launch a program if it couldn't be found.
	}
	if ( ( !in ) || ( ( in->FindString( "Program Command Line Params", &fCommandLineOptions ) ) != B_OK ) )
	{
		// If didn't succeed to read the options, it's ok - just assume there were none.
		fCommandLineOptions.SetTo( "" );	
	}
	
	/* Email sending section */
	if ( ( !in ) || ( ( in->FindBool( "Email Sending Enabled", &bEmailToSend ) ) != B_OK ) )
	{
		bEmailToSend = false;
	}
	if ( ( !in ) ||
		  ( ( in->FindString( "Email Contents", &fEmailContents ) ) != B_OK ) ||
		  ( fEmailContents.CountChars() == 0 ) )
	{
		fEmailContents.SetTo( "" );
		emailContentsEmpty = true;
	}
	if ( ( !in ) ||
		  ( ( in->FindString( "Email Subject", &fEmailSubject ) ) != B_OK ) ||
		  ( fEmailSubject.CountChars() == 0 ) )
	{
		fEmailSubject.SetTo( "" );
		emailSubjectEmpty = true;
	}
	if ( ( !in ) ||
		  ( ( in->FindString( "Email Address 1", &fEmailAddress1 ) ) != B_OK ) ||
		  ( fEmailAddress1.CountChars() == 0 ) )
	{
		fEmailAddress1.SetTo( "" );
		emailAddress1Empty = true;
	}
	if ( ( !in ) ||
		  ( ( in->FindString( "Email Address 2", &fEmailAddress2 ) ) != B_OK ) ||
		  ( fEmailAddress2.CountChars() == 0 ) )
	{
		fEmailAddress2.SetTo( "" );
		emailAddress2Empty = true;
	}
	if ( ( !in ) ||
		  ( ( in->FindString( "Email Address 3", &fEmailAddress3 ) ) != B_OK ) ||
		  ( fEmailAddress3.CountChars() == 0 ) )
	{
		fEmailAddress3.SetTo( "" );
		emailAddress3Empty = true;
	}
	// If there is no Email addresses, or if both contents and subject are empty,
	// disable sending Email.
	if ( ( emailSubjectEmpty && emailContentsEmpty ) ||
		  ( emailAddress1Empty && emailAddress2Empty && emailAddress3Empty ) )
	{
		bEmailToSend = false;
	}

}	// <-- end of function ActivityData::Instantiate



/*!	\brief		Save the activity data to the submitted message.
 *		\param[out]	out	The BMessage to be filled.
 *		\returns		B_OK if everything was Ok.
 */
status_t		ActivityData::Archive( BMessage* out )
{
	status_t	toReturn = B_OK;

	/* Sanity check */	
	if ( !out ) {
		toReturn = B_NO_INIT;
		return toReturn;
	}
	
	/* Clear the message */
	if ( ( toReturn = out->MakeEmpty() ) != B_OK ) {
		return toReturn;
	}
	
	/* Adding data about notification */
	if ( ( ( toReturn = out->AddBool( "Notification Enabled", bNotification ) ) != B_OK ) ||
	     ( ( toReturn = out->AddString( "Notification Text", fNotificationText ) ) != B_OK ) )
	{
		return toReturn;
	}
	
	/* Adding data about sound play */
	if ( ( toReturn = out->AddBool( "Sound Play Enabled", bSound ) ) != B_OK )
	{
		return toReturn;
	}
	if ( fSoundFile.InitCheck() == B_OK ) {
		if ( ( toReturn = out->AddString( "Sound File Path", fSoundFile.Path() ) ) != B_OK )
		{
			return toReturn;
		}
	}
	
	/* Adding data about program to run */
	if ( ( ( toReturn = out->AddBool( "Program Run Enabled", bProgramRun ) ) != B_OK ) ||
		  ( ( toReturn = out->AddBool( "Program Verified", bVerifiedByUser ) ) != B_OK ) ||
		  ( ( toReturn = out->AddString( "Program Command Line Params", fCommandLineOptions ) ) != B_OK ) )
	{
		return toReturn;
	}
	if ( fProgramPath.InitCheck() == B_OK ) {
		if ( ( toReturn = out->AddString( "Program Path", fProgramPath.Path() ) ) != B_OK )
		{
			return toReturn;
		}
	}
	
	/* Adding data about Email */
	if ( ( toReturn = out->AddBool( "Email Sending Enabled", bEmailToSend ) ) != B_OK )
	{
		return toReturn;
	}
	if ( fEmailContents.CountChars() > 0 ) {
		if ( ( toReturn = out->AddString( "Email Contents", fEmailContents ) ) != B_OK )
		{
			return toReturn;
		}
	}
	if ( fEmailSubject.CountChars() > 0 ) {
		if ( ( toReturn = out->AddString( "Email Subject", fEmailSubject ) ) != B_OK )
		{
			return toReturn;
		}
	}
	if ( fEmailAddress1.CountChars() > 0 ) {
		if ( ( toReturn = out->AddString( "Email Address 1", fEmailAddress1 ) ) != B_OK )
		{
			return toReturn;
		}
	}
	if ( fEmailAddress2.CountChars() > 0 ) {
		if ( ( toReturn = out->AddString( "Email Address 2", fEmailAddress2 ) ) != B_OK )
		{
			return toReturn;
		}
	}
	if ( fEmailAddress3.CountChars() > 0 ) {
		if ( ( toReturn = out->AddString( "Email Address 3", fEmailAddress3 ) ) != B_OK )
		{
			return toReturn;
		}
	}
	
	return toReturn;
}	// <-- end of function ActivityData::Archive
