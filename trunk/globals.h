/*!	\file		globals.h
	\brief 		Global constants and definitions for the Eventual project.
	\author		Alexey Burshtein  
	\note		Haifa University - Project course
		\par	A part of the "Eventual" Haiku Calendar / Organizer
		\par 	Distributed under MIT license.
	\date 		2009-2010
*/

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "./Support/ErrorAlert.h"

/*! \enum ReturnCode
  	\brief The code to be returned throughout of the program, 
  		including to the OS upon exit.
*/
enum ReturnCode {
	OK = 0,						// 0
	NOT_ENOUGH_MEMORY,			// 1
	NOT_ENOUGH_DISK_SPACE,		// 2
	CANT_FIND_REQUIRED_ITEM		// 3

};

class ErrorAlert;


// Global variables
enum ReturnCode exitValue;		// The exit value of the program

#endif // __GLOBALS_H__
