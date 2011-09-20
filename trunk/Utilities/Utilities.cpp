/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include <ctype.h> 
#include <string.h>

#include <List.h>
#include <String.h>

#include "Utilities.h"


/*!	\brief		Debugging printout - defined once and globally.
 */
DebuggerPrintout		*utl_Deb = NULL;


/*!
 *	\brief		List that holds all categories in the system. 
 *	\details	It's part of the utilities since it's included in all parts of the
 *				program. Every part of the program initializes this object for itself.
 *				Sample initialization is in CategoryItem file.
 *	\sa			::PopulateListOfCategories( BMessage* in )
 */
BList global_ListOfCategories;


BList global_ListOfCalendarModules( NUMBER_OF_CALENDAR_MODULES );


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


/*!	\brief		Find calendar module based on its ID.
 *	\param[in]	id	The identifier of the Calendar Module.
 *	\returns	Valid pointer to calendar module object, if it is found.
 *				NULL, if it's not.
 */
CalendarModule* 	utl_FindCalendarModule( const BString& id )
{
	int index, limit;
	CalendarModule* testing;
	
	limit = global_ListOfCalendarModules.CountItems();
	
	for ( index = 0; index < limit; ++index )
	{
		testing = ( CalendarModule* )global_ListOfCalendarModules.ItemAt( index );
		if ( testing->Identify() == id )
		{
			return testing;	
		}
	}
	
	return NULL;	
}	// <-- end of function FindCalendarModule



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


/*!	\brief		Syntactically verify a string for being an Email address.
 *		\details		This recipe is an adopted code from the book
 *						"Secure Programming Cookbook for C and C++" by 
 *						John Viega and Matt Messier. It can be found as Recipe 3.9
 *						beginning on page 101.
 *		\returns		"true" if the string looks like an Email address, "false" otherwise.
 */
bool utl_VerifyEmailAddress( const char *address )
{
  int        count = 0;
  const char *c, *domain;
  static char *rfc822_specials = "()<>@,;:\\\"[]";

  /* first we validate the name portion (name@domain) */
  for ( c = address;  *c;  c++ ) 
  {
    if ( ( *c == '\"' ) && 
    	   ( c == address || *(c - 1) == '.' || *(c - 1) == '\"' ) )
   {
      while (*++c) {
        if (*c == '\"') break;
        if (*c == '\\' && (*++c == ' ')) continue;
        if (*c < ' ' || *c >= 127) return false;
      }
      if (!*c++) return false;
      if (*c == '@') break;
      if (*c != '.') return false;
      continue;
    }
    if (*c == '@') break;
    if (*c <= ' ' || *c >= 127) return false;
    if ( strchr( rfc822_specials, *c ) ) return false;
  }
  if (c == address || *(c - 1) == '.') return false;

  /* next we validate the domain portion (name@domain) */
  if (!*(domain = ++c)) return false;
  do {
    if (*c == '.') {
      if (c == domain || *(c - 1) == '.') return false;
      count++;
    }
    if (*c <= ' ' || *c >= 127) return false;
    if (strchr(rfc822_specials, *c)) return false;
  } while (*++c);

  return (count >= 1);
}	// <-- end of function utl_VerifyEmailAddress


/*!	\brief		Verify a server address or IP.
 *		\details		Based on the function utl_VerifyEmailAddress.
 *		\note			Implementation details
 *						This function does not test DNS or pings the server. It just
 *						checks the address looks correct.
 *		\returns		"true" if the address looks Ok, "false" otherwise.
 */
bool		utl_VerifySeverAddress( const char* address )
{
  int        count = 0;
  const char *c, *domain;
  static char *rfc822_specials = "()<>@,;:\\\"[]";
	
  if (!*(domain = ++c)) return false;
  do {
    if (*c == '.') {
      if (c == domain || *(c - 1) == '.') return false;
      count++;
    }
    if (*c <= ' ' || *c >= 127) return false;
    if (strchr(rfc822_specials, *c)) return false;
  } while (*++c);

  return (count >= 1);
}	// <-- end of function utl_VerifySeverAddress



/* This message loads upon startup and is saved by the Preferences preflet.
 */
BMessage global_Preferences( kGlobalPreferences );
