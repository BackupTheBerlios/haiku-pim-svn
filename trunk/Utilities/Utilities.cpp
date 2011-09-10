/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include <ctype.h> 

#include <List.h>
#include <String.h>

#include "Utilities.h"

/*!
 *	\brief		List that holds all categories in the system. 
 *	\details	It's part of the utilities since it's included in all parts of the
 *				program. Every part of the program initializes this object for itself.
 *				Sample initialization is in CategoryItem file.
 *	\sa			::PopulateListOfCategories( BMessage* in )
 */
BList global_ListOfCategories;


/*!	\function 	utl_CheckStringValidity
 *	\brief		Verify the string submitted by the user is valid.
 *	\details	Perform also some adjustments, described below.
 *	\returns	"true" if the string is valid, "false" if user must correct it.
 *	\param[in]	toCheck	Reference to BString which should be checked. This is also output.
 */
bool utl_CheckStringValidity( BString& toCheck )
{
	/*!	\par	Rules of validity for strings
	 *			1) A string lasts from beginning to first "newline" character.
	 *			   Everything after "newline" character is trimmed.
	 *			2) All whitespaces are replaced with "space" char.
	 *			3) All sequences of more then one "space" are replaced with a single "space".
	 *			4) All leading and trailing space are trimmed.
	 *			5) If the string is empty, it's invalid.
	 *			String that consists of punctuation mark only is allowed!
	 */
	int index;
	
	if ( toCheck.CountChars() == 0 )
	{
		return false;
	}
	
	BString workingCopy( toCheck );
	
	/* Trim everything after first newline character. */
	if ( ( index = workingCopy.FindFirst("\n") ) != B_ERROR )
	{
		// index is the position of the first newline character
		if ( index > 0 )
		{
			workingCopy.Truncate( index - 1 );	
		} else {
			workingCopy.Truncate( 0 );
		}
	}
	
	/* Remove leading and trailing whitespaces. */
	workingCopy = workingCopy.Trim();
	
	/* Replace every whitespace with a "space" character. */
	workingCopy = workingCopy.ReplaceSet( " \t\a\n\r\v", ' ');
	
	/* Replace every two sequentive whitespaces with a single space. */
	while ( ( index = workingCopy.FindFirst("  ") ) != B_ERROR )
	{
		workingCopy = workingCopy.IReplaceAll("  ", " ");
	}	// <-- end of "while ( there are two sequentive spaces )"
	
	if ( workingCopy.CountChars() == 0 ) {
		return false;
	} else {
		toCheck = workingCopy;
		return true;
	}

}	// <-- end of function "utl_CheckStringValidity"



uint32	RepresentColorAsUint32( rgb_color color )
{
	uint32 toReturn, red, green, blue, alpha;
	
	red   = color.red   << 24;
	green = color.green << 16;
	blue  = color.blue  << 8;
	alpha = color.alpha;
	
	toReturn =  ( red   & 0xFF000000 ) 	|
				( green & 0x00FF0000 )  |
				( blue  & 0x0000FF00 )  |
				( alpha & 0x000000FF );
	
	return toReturn;
}


rgb_color RepresentUint32AsColor( uint32 in )
{
	rgb_color toReturn;
	toReturn.red = 		( in & 0xFF000000 ) >> 	24;
	toReturn.green = 	( in & 0x00FF0000 ) >>  16;
	toReturn.blue = 	( in & 0x0000FF00 ) >>  8;
	toReturn.alpha = 	( in & 0x000000FF );
	return toReturn;
}


/* This message loads upon startup and is saved by the Preferences preflet.
 */
BMessage global_Preferences( kGlobalPreferences );
