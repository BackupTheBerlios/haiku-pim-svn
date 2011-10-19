/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _ACTIVITY_VIEW_H_
#define _ACTIVITY_VIEW_H_

#include "NotificationView.h"
#include "SoundSetupView.h"
#include "ProgramSetupView.h"


/*---------------------------------------------------------------------------
 *						Declaration of class ActivityView
 *--------------------------------------------------------------------------*

 *!	\brief		This class provides ways to edit the activities.
 *
class ActivityView
	:
	public BView
{
public:
	// Constructor and destructor
	ActivityView( BRect frame,
					  const char* name,
					  ActivityData* toEdit );
	virtual ~ActivityView();
	
	// Main function of the class
	virtual void MessageReceived( BMessage* in );
	
	// Register the messages handler
	virtual void AttachedToWindow( void );
	
	// Set the Activity Data
	virtual void	SetActivityData( ActivityData* toSet );
	// Get the Activity Data
	inline virtual ActivityData*	GetActivityData( void ) const { return fData; }
	
	// Force update of the contents
	virtual void ForceUpdate( void );
	
protected:
	// Data placeholders
	ActivityData* fData;
	
	// Service functions
	virtual void 	BuildUI( void );
	virtual BBox*	BuildNotificationBox( void );

};	// <-- end of class ActivityView

*/

#endif // _ACTIVITY_VIEW_H_
