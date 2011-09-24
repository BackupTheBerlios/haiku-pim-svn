/*
 * Copyright 2011 AlexeyB Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
 
// C++ includes
#include <stdio.h>
#include <stdlib.h>
 
// System includes

#include <GridLayout.h>
#include <GraphicsDefs.h>
#include <GroupLayout.h>
#include <Layout.h>
#include <LayoutItem.h>
#include <InterfaceDefs.h>
#include <Rect.h>
#include <SeparatorItem.h>
#include <View.h>
 
// Local includes

#include "EmailPreferencesView.h"
#include "EmailPreferences.h"			// Preferences of Email
#include "Utilities.h"

/*==============================================================================
 		Implementation of class EmailPreferencesView
==============================================================================*/

/*!	\brief		Constructor of EmailPreferencesView
 *	\details	Basically, sets up the view, and that's all.
 */
EmailPreferencesView::EmailPreferencesView( BRect frame )
	:
	BView( frame,
		   "Email preferences",
		   B_FOLLOW_LEFT | B_FOLLOW_TOP,
		   B_WILL_DRAW | B_FRAME_EVENTS ),
	senderEmail( NULL ),
	mailServerAddress( NULL ),
	mailServerPort( NULL ),
	explanationString( NULL ),
	explanationString2( NULL )
{
	EmailPreferences* emailPrefs = pref_GetEmailPreferences();
	if ( !emailPrefs )
	{
		/* Panic! */
		exit( 1 );
	}
	
	BLayoutItem *layoutItem = NULL;
	BGroupLayout* groupLayout = new BGroupLayout( B_VERTICAL );
	if ( ! groupLayout ) {
		/* Panic! */
		exit( 1 );
	}
	this->SetLayout( groupLayout );
	groupLayout->SetInsets( 10, 5, 10, 5 );
	this->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	
	explanationString = new BStringView( BRect( 0, 0, 1, 1 ),
										 "Explanation",
										 "Currently, Eventual supports only simpliest E-mail services." );
	if ( ! explanationString )
	{
		/* Panic! */
		exit( 1 );
	}
	explanationString->ResizeToPreferred();
	layoutItem = groupLayout->AddView( explanationString );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_CENTER, B_ALIGN_TOP ) );
	}
	
	explanationString2 = new BStringView( BRect( 0, 0, 1, 1),
										  "Explanation 2",
										  "Neither mail server authentication nor secure transfers, please." );
	if ( ! explanationString2 )
	{
		/* Panic! */
		exit( 1 );
	}
	explanationString2->ResizeToPreferred();
	layoutItem = groupLayout->AddView( explanationString2 );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_CENTER, B_ALIGN_TOP ) );
	}

	senderEmail = new BTextControl( BRect( 0, 0, 1, 1 ),
								    "Sender Email",
								    "Your E-mail address:",
								    ( emailPrefs->GetReplyToAddress() ).String(),
								    NULL );
	if ( !senderEmail ) {
		/* Panic! */
		exit( 1 );
	}
	senderEmail->ResizeToPreferred();
	senderEmail->SetDivider( ( frame.Width() / 2 ) - 5 );
	senderEmail->SetAlignment( B_ALIGN_RIGHT, B_ALIGN_LEFT );
	layoutItem = groupLayout->AddView( senderEmail );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH, B_ALIGN_TOP ) );
	}
	
	mailServerAddress = new BTextControl( BRect( 0, 0, 1, 1 ),
						  			     "SMTP Server",
								        "Outgoing mail server (SMTP) address:",
								    	  ( emailPrefs->GetMailServerAddress() ).String(),
								    	  NULL );
	if ( !mailServerAddress ) {
		/* Panic! */
		exit( 1 );
	}
	mailServerAddress->ResizeToPreferred();
	mailServerAddress->SetDivider( ( frame.Width() / 2 ) - 5 );
	mailServerAddress->SetAlignment( B_ALIGN_RIGHT, B_ALIGN_LEFT );
	layoutItem = groupLayout->AddView( mailServerAddress );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH, B_ALIGN_TOP ) );
	}
	
	mailServerPort = new BTextControl( BRect( 0, 0, 1, 1 ),
						  			      "SMTP Server Port",
								         "Outgoing mail server (SMTP) port:",
								    	  	( emailPrefs->GetMailServerPortAsString() ).String(),
								    	  	NULL );
	if ( !mailServerPort ) {
		/* Panic! */
		exit( 1 );
	}
	mailServerPort->ResizeToPreferred();
	mailServerPort->SetDivider( ( frame.Width() / 2 ) - 5 );
	mailServerPort->SetAlignment( B_ALIGN_RIGHT, B_ALIGN_LEFT );
	layoutItem = groupLayout->AddView( mailServerPort );
	if ( layoutItem ) {
		layoutItem->SetExplicitAlignment( BAlignment( B_ALIGN_USE_FULL_WIDTH, B_ALIGN_TOP ) );
	}
	
}	// <-- end of constructor for EmailPreferencesView



/*!	\brief		Destructor.
 */
EmailPreferencesView::~EmailPreferencesView()
{
	EmailPreferences* prefs = pref_GetEmailPreferences();
	if ( !prefs ) {
		utl_Deb = new DebuggerPrintout( "There's no modified Email preferences!" );
	}
	
	BString sb;
	
	if ( senderEmail ) {
		sb.SetTo( senderEmail->Text() );
		if ( prefs && B_OK != prefs->UpdateReplyToAddress( sb ) )
		{
			utl_Deb = new DebuggerPrintout( "Didn't succeed to update reply-to address!" );	
		}
		senderEmail->RemoveSelf();
		delete( senderEmail );
		senderEmail = NULL;
	}
	
	if ( mailServerAddress ) {
		sb.SetTo( mailServerAddress->Text() );
		if ( prefs && B_OK != prefs->UpdateMailServerAddress( sb ) )
		{
			utl_Deb = new DebuggerPrintout( "Didn't succeed to update mail server address!" );
		}
		mailServerAddress->RemoveSelf();
		delete( mailServerAddress );
		mailServerAddress = NULL;
	}
	
	if ( mailServerPort ) {
		sb.SetTo( mailServerPort->Text() );
		if ( prefs && B_OK != prefs->UpdateMailServerPort( sb ) )
		{
			utl_Deb = new DebuggerPrintout( "Didn't succeed to update mail server port!" );
		}
		mailServerPort->RemoveSelf();
		delete( mailServerPort );
		mailServerPort = NULL;
	}
	
	if ( explanationString ) {
		explanationString->RemoveSelf();
		delete( explanationString );
		explanationString = NULL;
	}
	
	if ( explanationString2 ) {
		explanationString2->RemoveSelf();
		delete( explanationString2 );
		explanationString2 = NULL;
	}
}	// <-- end of destructor	   


/*!	\brief		This function adds BViews's message handler to window's looper.
 */
void		EmailPreferencesView::AttachedToWindow()
{
/*	Well, actually, adding the view to window's looper is not needed, since messages
 * from the text boxes aren't needed.

	BLooper* looper = this->Looper();
	if ( looper && looper->LockLooper() )
	{
		looper->AddHandler( ( BHandler* )this );
		looper->UnlockLooper();	
	}
*/	
	senderEmail->SetTarget( this );
	mailServerAddress->SetTarget( this );
	mailServerPort->SetTarget( this );
	
	BView::AttachedToWindow();
	
}	// <-- end of function EmailPreferencesView::AttachedToWindow()
