/*
 * Copyright 2010 Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include "Logger.h"
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Alert.h>
#include <scheduler.h>

/*! \function		Logger::Logger()
 *	\brief			A private constructor of the singletone class.
 *	\remarks		It's assumed that the Logger::semaphore is taken before
 *					entering this function and released after it returns.
 */
Logger::Logger()	
{
//	BPath debugPath;
	Logger::debugLog = NULL;
	BFile debugLog;
	BString sb;
	
	
	// First, we find the user's config directory.
	status_t responce = find_directory(B_USER_CONFIG_DIRECTORY, 
								&Logger::debugLogPath, 
								true);
	if (B_OK != responce) {
		// Panic!
		exit(1);
	}
	
	// Second, from there we build the path to the application's log
	if (B_OK != (responce = Logger::debugLogPath.Append("Eventual/Logs"))) {
		// Panic!
		exit(1);
	}
	
	BDirectory dir(Logger::debugLogPath.Path());
	
	dir.CreateDirectory(Logger::debugLogPath.Path(), &dir);
	
	// Third, we need to create this file if it doesn't exist, or to open it if it does.
	// Here we try to create file. However, if it exists, this call will fail, and the
	// "responce" will be set to "B_FILE_EXISTS", thus we need to catch it.
	responce = dir.CreateFile(Logger::debugLogPath.Path(), Logger::debugLog, true);
	if (B_FILE_EXISTS == responce) {
		return;
	} else if (B_OK != responce) {
		// Panic!
		exit(1);
	}
	
	if (Logger::debugLog->InitCheck() != B_OK) {
		BAlert* al = new BAlert("Test", "The log was not initialized", "Ok");
		if (al) { al->Go(); }
		exit(1);
	}
	
	// Print the standard header.
	sb << "   ID Message and data (from last element to first)\n";
	sb << "===== ==============================================================\n";
	Logger::debugLog->Write((void*)sb.String(), strlen(sb.String()));	
	
	Logger::debugLog->Unset();
	delete Logger::debugLog;
	Logger::debugLog = NULL;
}
// <-- end of the Logger constructor

/*!	\function		Logger::Init
 *	\brief			This function initializes the singletone class.
 *	\remarks		It is the only function that is allowed to call to the Logger's
 *					constructor.
 *	\returns		A constant pointer to the Logger object.
 *					If the object couldn't be created, the program exits.
 */
const Logger* Logger::Init (void) {
	if (init) {
		return Logger::object;
	}
	
	Logger::locker = new BLocker("Logger semaphore");
	if (! Logger::locker) {
		// Panic!
		exit(1);
	}
	Logger::locker->Lock();
	init = true;
	Logger::object = new Logger();	// Creating the logger writer
	if (Logger::object == NULL) {
		// Panic!
		exit(1);
	}
	Logger::locker->Unlock();
	return Logger::object;
}
// <-- end of function Logger::Init

/*!	\function		Logger::AddMessage
 *	\brief			Entry point for adding a message to the log
 *	\param[in]	message			BString which contains the message
 *	\param[in]	numOfElements	Amount of chunks of additional data in the message
 *	\param[in]	data1 thru data5	Different chunks of data to be printed.
 */
void 
Logger::AddMessage(BString message, uint8 numOfElements,
				uint32 data1, uint32 data2, uint32 data3, 
				uint32 data4, uint32 data5)
{
	// Allocating the data container for the new thread
	DataContainer* toWrite = new DataContainer;
	if (!toWrite) {
		// Panic!
		exit(1);
	}
	
	// Adding the data to be printed into the message
	switch (numOfElements) {
			// Here I use intentional fall-through
		case 5:
			Logger::AddElementToString(data5, message);
		case 4:
			Logger::AddElementToString(data4, message);
		case 3:
			Logger::AddElementToString(data3, message);
		case 2:
			Logger::AddElementToString(data2, message);
		case 1:
			Logger::AddElementToString(data1, message);
			break;
		default:		
			;		// Don't do anything
	};
	
	// Stuffing the container with the message
	toWrite->message = new BString(message);
	toWrite->file = new BPath(Logger::debugLogPath);
	if (!toWrite->message || !toWrite->file) {
		// Panic!
		exit(1);
	}
	
	// Starting a new thread
	thread_id writer = spawn_thread(Logger::AddMessage,
		"Logger writer",
		B_VIDEO_PLAYBACK,	// Rising the priority a bit
		(void*)toWrite);
	resume_thread(writer);
}
// <-- end of function Logger:AddMessage

/*!	\function 		Logger::AddMessage
 *	\brief			The function performs actual write to the file.
 *	\param[in]	data	This is actually the DataContainer object.
 *	\remarks		This function will delete the submitted container and 
 *					all of its contains upon the exit.
 */
int32
Logger::AddMessage
	(void* data)
{
	DataContainer* toWrite = (DataContainer*)data;
	status_t returnCode = 0;
	BAlert* al = NULL;
	
	// Sanity check
	if (!data || !toWrite->file || !toWrite->message) { return returnCode; }
	
	// Getting the file object to be updated
	BFile* file = new BFile(toWrite->file->Path(),
		B_WRITE_ONLY | B_OPEN_AT_END);
	
	// Check that it was initialized accordingly.
	if (!file	||
		file->InitCheck() != B_OK)
	{
		// Panic!
		al = new BAlert("Test", "Can't open the log file for writing!", "Ok");
		if (al) { al->Go(); }
		exit(1);
	}
	
	if ( 0 > file->Write((void*)toWrite->message->String(), 
						 strlen(toWrite->message->String())) ||
		0 > file->Write("\n", 1))	// After each message, moving to next line.
		returnCode = 1;
	
	file->Unset();
	delete file;
	file = NULL;
	delete toWrite->message;
	delete toWrite->file;
	delete toWrite;
	
	return returnCode;
}
// <-- end of function Logger::AddMessage

/*!	\function		Logger::AddElementToString
 *	\brief			Service function which helps to build the traces.
 *	\param[in]	element		The uint32 to be added.
 *	\param[out]	change		The reference to BString to be changed.
 */
void 
Logger::AddElementToString(uint32 element, BString& change)
{	
	uint32ToChars u;
	
	change << ' ';		// Adding space to separate data from message
	u.number = element;
	for (int i=0; i<4; ++i) {
		change << (uint8)u.chars[i];
	}
	return;
}
// <-- end of function Logger::AddElementToString
