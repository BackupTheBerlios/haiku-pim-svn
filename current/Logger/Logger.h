/*
 * Copyright 2010 
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <SupportDefs.h>
#include <String.h>
#include <OS.h>
#include <File.h>
#include <Path.h>
#include <Locker.h>
#include <stdlib.h>

struct DataContainer {
	BPath* file;
	BString* message;
};

union uint32ToChars {
	uint32 number;
	uint8 chars[4];
};

bool init = false;

class Logger {
public:
	static const Logger* Init();

	//! Public interface for the function which will add the message
	static void AddMessage(BString message, uint8 numOfElements = 0,
				uint32 data1=0, uint32 data2=0, uint32 data3=0, 
				uint32 data4=0, uint32 data5=0);				
private:
	static Logger* object;	
	Logger();
	static BLocker *locker;
	static BFile* debugLog;
	static BPath debugLogPath;
	
	//! Service subroutine
	static void AddElementToString(uint32 element, BString& change);
	
	static int32 AddMessage(void* data);	//!< Thread function.
};

// static void AddMessageToDebugLog5(BString message, 
//		uint32 data1, uint32 data2, uint32 data3, uint32 data4, uint32 data5);


#endif // _LOGGER_H_
