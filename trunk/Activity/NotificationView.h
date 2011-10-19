/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _NOTIFICATION_VIEW_H_
#define _NOTIFICATION_VIEW_H_

// OS includes
#include <Box.h>
#include <CheckBox.h>
#include <Control.h>
#include <Message.h>
#include <Rect.h>
#include <ScrollView.h>
#include <StringView.h>
#include <SupportDefs.h>
#include <TextView.h>
#include <View.h>

// Project includes
#include "ActivityData.h"



/*---------------------------------------------------------------------------
 *								Message constants
 *--------------------------------------------------------------------------*/

	/*!	\brief	This const is used as message data in all views. */
const uint32	kNotificationActivityCheckBoxToggled		= 'ChBT';



/*---------------------------------------------------------------------------
 *						Declaration of class NotificationView
 *--------------------------------------------------------------------------*/


/*!	\brief		This class manages work on Notification area of the Activity.
 */
class	NotificationView
	:
	public BControl
{
	public:
		NotificationView( BRect frame, 
								const char* name,
								const char* label,
								ActivityData* toEdit,
								BMessage* message = NULL );
		~NotificationView();

		virtual void	MessageReceived( BMessage* in );
		virtual void	AttachedToWindow( void );
		inline virtual status_t	InitCheck( void ) const { return fLastError; }
		virtual void	SetLabel( const char* labelIn );
		virtual void	Pulse( void );
	
	protected:
		// Information holders
		ActivityData*	fData;
		BString			fNotificationText;
		status_t			fLastError;

		// UI elements
		BCheckBox*		fCheckBox;
		BBox*				fOutline;
		BStringView*	fLabel;
		BTextView*		fTextView;
		BScrollView*	fScroller;

};	// <-- end of class NotificationView


#endif // _NOTIFICATION_VIEW_H_
