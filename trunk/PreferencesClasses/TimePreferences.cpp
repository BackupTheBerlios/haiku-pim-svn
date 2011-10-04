/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include "TimePreferences.h"
#include "TimeRepresentation.h"
#include	"Utilities.h"

#include <stdlib.h>
#include <stdio.h>



/*****************************************************************************
 *				Definitions of global variables
 ****************************************************************************/

	/*!	\brief	Modified Time preferences 	*/
			TimePreferences* pref_TimePreferences_modified;
			
	/*!	\brief	Original Time preferences		
	 *		\details	Declared as static to prevent unnecessary access. */
static	TimePreferences* pref_TimePreferences_original;



/*****************************************************************************
 *				Definitions of global functions
 ****************************************************************************/

/*!	\brief		Obtain Time preferences from the preferences message.
 *		\param[in]	in		BMessage which contains another message with the preferences.
 *		\details		In case of any error, defaults are used.
 *		\note
 *						in may be NULL!
 */
status_t		pref_PopulateTimePreferences( BMessage* in ) {
	BMessage toCheck( kTimePreferences );
	
	if ( in && ( B_OK == in->FindMessage( "Time Preferences", &toCheck ) ) )
	{
		pref_TimePreferences_original = new TimePreferences( &toCheck );
	} else {
		pref_TimePreferences_original = new TimePreferences();	// Defaults
	}
	if ( !pref_TimePreferences_original )	{
		/* Probably error, but hey, let's run anyway.
		 *
		return B_ERROR;
		 */
	}
	
	pref_TimePreferences_modified = new TimePreferences( pref_TimePreferences_original );
	if ( !pref_TimePreferences_modified ) {
		/* Well, now this IS an error. */
		return B_ERROR;
	}
	
	return B_OK;
	
}	// <-- end of function	pref_PopulateTimePreferences



/*!	\brief		Saves the preferences.
 *		\details		The save is performed only if the preferences differ
 *						or if the original preferences did not exist.
 *		\param[out]		out		The BMessage to which the preferences should be added.
 */
status_t		pref_SaveTimePreferences( BMessage* out )
{
	if (  !out ||										// Nowhere to save
			!pref_TimePreferences_modified )		// Nothing to save
	{
		return B_ERROR;		// Duh
	}
	
	status_t		status = B_OK;
	
	BMessage* toAdd = new BMessage( kTimePreferences );
	if ( !toAdd )				// Nothing to save
	{
		return B_ERROR;
	}
	
	/* Save data into the message */
	if ( B_OK != ( status = pref_TimePreferences_modified->Archive( toAdd ) ) )
	{
		return status;
	}
	
	
	/* If there were no original preferences or
	 *	if the preferences were modified or
	 * if the message didn't have information on the preferences
	 *		add the created message to the output
	 */
	if ( ( !pref_TimePreferences_original ) || 
	     ( !pref_TimePreferences_original->Compare( pref_TimePreferences_modified ) ) )
	{
		if ( out->HasMessage( "Time Preferences" ) )
		{
			status = out->ReplaceMessage( "Time Preferences", toAdd );
		}
		else
		{
			status = out->AddMessage( "Time Preferences", toAdd );
		}
	}
	return status;		// Error or B_OK - it doesn't matter here.
	
}	// <-- end of function pref_SaveTimePreferences
 
 

/*****************************************************************************
 *				Implementation of class TimePreferences
 ****************************************************************************/

/*!	\brief				Constructor from BMessage which can be null
 *		\param[in]	in		The BMessage to construct this object from.
 */
TimePreferences::TimePreferences( BMessage* in )
{
	uint8 temp1, temp2;
	
	// Get the 24h clock usage
	if ( ( !in ) || B_OK != in->FindBool( "Use 24 Hours Clock", &use24hClock ) )
	{
		use24hClock = true;
	}
	
	// Get the default appointment length
	if ( ( !in ) || B_OK != in->FindInt8( "Default Appointment Duration - Hours", ( int8* )&temp1 ) )
	{
		temp1 = 0;
	}	
	if ( ( !in ) || ( B_OK != in->FindInt8( "Default Appointment Duration - Minutes", ( int8* )&temp2 ) ) )
	{
		temp2 = 30;
	}	
	SetDefaultAppointmentDuration( temp1, temp2 );
	
	// Get the default reminder firing time
	if ( ( !in ) || B_OK != in->FindInt8( "Default Reminder Time - Hours", ( int8* )&temp1 ) )
	{
		temp1 = 0;
	}
	
	if ( ( !in ) || ( B_OK != in->FindInt8( "Default Reminder Time - Minutes", ( int8* )&temp2 ) ) )
	{
		temp2 = 15;
	}	
	SetDefaultReminderTime( temp1, temp2 );
	
}	// <-- end of constructor from BMessage



/*!	\brief		Copy constructor.
 */
TimePreferences::TimePreferences( const TimePreferences& other )
{
	*this = other;
}	// <-- end of copy constructor


/*!	\brief		Assignment operator.
 */
TimePreferences	TimePreferences::operator= ( const TimePreferences& other )
{
	this->defaultAppointmentDuration = other.defaultAppointmentDuration;
	this->defaultReminderTime = other.defaultReminderTime;
	this->use24hClock = other.use24hClock;

	return *this;	
}	// <-- end of assignment operator.


/*!	\brief		Another copy constructor.
 */
TimePreferences::TimePreferences( const TimePreferences* other )
{
	if ( other )
	{
		this->defaultAppointmentDuration = other->defaultAppointmentDuration;
		this->defaultReminderTime = other->defaultReminderTime;
		this->use24hClock  = other->use24hClock;
	}
	else		// Setting default preferences
	{
		SetDefaultAppointmentDuration( 0, 30 );
		SetDefaultReminderTime( 0, 15 );
		this->use24hClock  = true;
	}
}	// <-- end of copy constructor



/*!	\brief		Pack the information from this object into the submitted message.
 *		\param[out]	out	The BMessage to add the information to.
 *		\note			Note:
 *						This function uses obsolete code!
 *		\returns		B_OK if everything was Ok.
 */
status_t		TimePreferences::Archive( BMessage* out ) const
{
	status_t	status;
	if ( !out ) return B_ERROR;
	
	// Pack the 24h clock usage
	if ( out->HasBool( "Use 24 Hours Clock" ) ) {
		status = out->ReplaceBool( "Use 24 Hours Clock", this->use24hClock );
	} else {
		status = out->AddBool( "Use 24 Hours Clock", this->use24hClock );
	}
	if ( status != B_OK ) { return status; }
	
	// Pack the default appointment duration time
	if ( out->HasInt8( "Default Appointment Duration - Hours" ) )	{
		status = out->ReplaceInt8( "Default Appointment Duration - Hours", ( uint8 )this->defaultAppointmentDuration.tm_hour );
	} else {
		status = out->AddInt8( "Default Appointment Duration - Hours", ( uint8 )this->defaultAppointmentDuration.tm_hour );
	}
	if ( out->HasInt8( "Default Appointment Duration - Minutes" ) )	{
		status = out->ReplaceInt8( "Default Appointment Duration - Minutes", ( uint8 )this->defaultAppointmentDuration.tm_min );
	} else {
		status = out->AddInt8( "Default Appointment Duration - Minutes", ( uint8 )this->defaultAppointmentDuration.tm_min );
	}

	// Pack the default reminder firing time
	if ( out->HasInt8( "Default Reminder Time - Hours" ) )	{
		status = out->ReplaceInt8( "Default Reminder Time - Hours", ( uint8 )this->defaultReminderTime.tm_hour );
	} else {
		status = out->AddInt8( "Default Reminder Time - Hours", ( uint8 )this->defaultReminderTime.tm_hour );
	}
	if ( out->HasInt8( "Default Reminder Time - Minutes" ) )	{
		status = out->ReplaceInt8( "Default Reminder Time - Minutes", ( uint8 )this->defaultReminderTime.tm_min );
	} else {
		status = out->AddInt8( "Default Reminder Time - Minutes", ( uint8 )this->defaultReminderTime.tm_min );
	}

	return status;	
	
}	// <-- end of function TimePreferences::Archive



/*!	\brief		Comparison operator.
 *		\details		Returns "true" if objects are exactly equal, else returns "false".
 *		\param[in]	other	The object to compare with.
 */
bool		TimePreferences::operator== ( const TimePreferences& other ) const
{
	if ( ( this->defaultAppointmentDuration.tm_min == other.defaultAppointmentDuration.tm_min ) &&
		  ( this->defaultAppointmentDuration.tm_hour == other.defaultAppointmentDuration.tm_hour ) &&
		  ( this->use24hClock == other.use24hClock ) )
	{
		return true;
	}
	return false;	
}	// <-- end of comparison operator


/*!	\brief		Comparison operator on pointers.
 *		\details		Returns "true" if objects are exactly equal, else returns "false".
 *		\param[in]	other	The pointer to object to compare with.
 */
bool		TimePreferences::Compare ( const TimePreferences* other ) const
{
	if ( ( other ) &&
		  ( this->defaultAppointmentDuration.tm_min == other->defaultAppointmentDuration.tm_min ) &&
		  ( this->defaultAppointmentDuration.tm_hour == other->defaultAppointmentDuration.tm_hour ) &&
		  ( this->use24hClock == other->use24hClock ) )
	{
		return true;
	}
	return false;	
}	// <-- end of comparison operator



/*!	\brief		SetDefaultAppointmentDuration
 *		\details		Receives only hours and minutes.
 *		\param[in]	hours		How many hours default duration lasts?
 *		\param[in]	mins		How many minutes default duration lasts?
 */
void		TimePreferences::SetDefaultAppointmentDuration( int hours, int mins )
{
	this->defaultAppointmentDuration.tm_hour = hours;
	this->defaultAppointmentDuration.tm_min = mins;
	this->defaultAppointmentDuration.SetIsRepresentingRealDate( false );	
}	// <-- end of function TimePreferences::SetDefaultAppointmentDuration



/*!	\brief		SetDefaultReminderTime
 *		\details		Receives only hours and minutes.
 *		\param[in]	hours		How many hours before the event reminder fires?
 *		\param[in]	mins		How many minutes before the event reminder fires?
 */
void		TimePreferences::SetDefaultReminderTime( int hours, int mins )
{
	this->defaultReminderTime.tm_hour = hours;
	this->defaultReminderTime.tm_min = mins;
	this->defaultReminderTime.SetIsRepresentingRealDate( false );	
}	// <-- end of function TimePreferences::SetDefaultReminderTime
