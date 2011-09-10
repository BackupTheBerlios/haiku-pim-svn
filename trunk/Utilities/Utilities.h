/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <Alert.h>
#include <GraphicsDefs.h>
#include <SupportDefs.h>



#define		NUMBER_OF_CALENDAR_MODULES		1

extern BList global_ListOfCategories;	//!< List that holds all categories in the system.

const uint32 kGlobalPreferences = 'PREF';	//!< ID of the global preferences.
extern BMessage global_Preferences;		//!< Message with all existing preferences.


/*!	\funciton	RepresentColorAsUint32
 *	\brief		Allows to refer to rgb_color to as uint32
 */
uint32			RepresentColorAsUint32( rgb_color color );
rgb_color		RepresentUint32AsColor( uint32 set );

/*!	\class		DebuggerPrintout
 *	\brief		A short debugging message
 *	\details	Based on BAlert class
 */
class DebuggerPrintout
	:
	public BAlert
{
public:
	inline DebuggerPrintout(const char* message)
		:
		BAlert("Printout", message, "Ok")
	{
		this->Go();
	}				
private:

};

bool utl_CheckStringValidity( BString& input );




union UintToChar {
	char    chars[4];
	uint32	integer;	
};



#endif // _UTILITIES_H_
