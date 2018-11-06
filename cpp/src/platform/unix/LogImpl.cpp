//-----------------------------------------------------------------------------
//
//	LogImpl.cpp
//
//  Unix implementation of message and error logging
//
//	Copyright (c) 2010, Greg Satz <satz@iranger.com>
//	All rights reserved.
//
//	SOFTWARE NOTICE AND LICENSE
//
//	This file is part of OpenZWave.
//
//	OpenZWave is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Lesser General Public License as published
//	by the Free Software Foundation, either version 3 of the License,
//	or (at your option) any later version.
//
//	OpenZWave is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public License
//	along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
#include <string>
#include <cstring>
#include <pthread.h>
#include <iostream>
#include "Defs.h"
#include "LogImpl.h"

#include <android/log.h>

#define APPNAME "(JNI) OpenZWave"

using namespace OpenZWave;

//-----------------------------------------------------------------------------
//	<LogImpl::LogImpl>
//	Constructor
//-----------------------------------------------------------------------------
LogImpl::LogImpl
(
		string const& _filename,
		bool const _bAppendLog,
		bool const _bConsoleOutput,
		LogLevel const _saveLevel,
		LogLevel const _queueLevel,
		LogLevel const _dumpTrigger
):
m_filename( _filename ),					// name of log file
m_saveLevel( _saveLevel ),					// level of messages to log to file
m_queueLevel( _queueLevel ),				// level of messages to log to queue
m_dumpTrigger( _dumpTrigger ),				// dump queued messages when this level is seen
pFile( NULL )
{
}

//-----------------------------------------------------------------------------
//	<LogImpl::~LogImpl>
//	Destructor
//-----------------------------------------------------------------------------
LogImpl::~LogImpl
(
)
{
	// Nothing to do
}

//-----------------------------------------------------------------------------
//	<LogImpl::Write>
//	Write to the log
//-----------------------------------------------------------------------------
void LogImpl::Write
(
		LogLevel _logLevel,
		uint8 const _nodeId,
		char const* _format,
		va_list _args
)
{
	char lineBuf[1024] = {0};
	if( _format != NULL && _format[0] != '\0' )
	{
		va_list saveargs;
		va_copy( saveargs, _args );
		vsnprintf( lineBuf, sizeof(lineBuf), _format, _args );
		va_end( saveargs );
	}

	char queueBuf[1024];
	string threadStr = GetThreadId();
	snprintf( queueBuf, sizeof(queueBuf), "<%s> %s", threadStr.c_str(), lineBuf );

	switch ( _logLevel ) {
	case LogLevel_Invalid:
		__android_log_print(ANDROID_LOG_UNKNOWN, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_None:
		__android_log_print(ANDROID_LOG_SILENT, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_Always:
		__android_log_print(ANDROID_LOG_UNKNOWN, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_Fatal:
		__android_log_print(ANDROID_LOG_FATAL, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_Error:
		__android_log_print(ANDROID_LOG_ERROR, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_Warning:
		__android_log_print(ANDROID_LOG_WARN, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_Alert:
		__android_log_print(ANDROID_LOG_WARN, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_Info:
		__android_log_print(ANDROID_LOG_INFO, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_Detail:
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_Debug:
		__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_StreamDetail:
		__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%s", queueBuf);
		break;
	case LogLevel_Internal:
		__android_log_print(ANDROID_LOG_UNKNOWN, APPNAME, "%s", queueBuf);
		break;
	default:
		__android_log_print(ANDROID_LOG_UNKNOWN, APPNAME, "%s", queueBuf);
	}
}

//-----------------------------------------------------------------------------
//     <LogImpl::QueueDump>
//     Dump the LogQueue to output device
//-----------------------------------------------------------------------------
void LogImpl::QueueDump
(
)
{
	// Nothing to do
}

//-----------------------------------------------------------------------------
//     <LogImpl::Clear>
//     Clear the LogQueue
//-----------------------------------------------------------------------------
void LogImpl::QueueClear
(
)
{
    // Nothing to do
}

//-----------------------------------------------------------------------------
//	<LogImpl::SetLoggingState>
//	Sets the various log state variables
//-----------------------------------------------------------------------------
void LogImpl::SetLoggingState
(
		LogLevel _saveLevel,
		LogLevel _queueLevel,
		LogLevel _dumpTrigger
)
{
	m_saveLevel = _saveLevel;
	m_queueLevel = _queueLevel;
	m_dumpTrigger = _dumpTrigger;
}

//-----------------------------------------------------------------------------
//	<LogImpl::GetTimeStampString>
//	Generate a string with formatted current time
//-----------------------------------------------------------------------------
string LogImpl::GetTimeStampString
(
)
{
	// Get a timestamp
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct tm *tm;
	tm = localtime( &tv.tv_sec );

	// create a time stamp string for the log message
	char buf[100];
	snprintf( buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%03d ",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec, (int)tv.tv_usec / 1000 );
	string str = buf;
	return str;
}

//-----------------------------------------------------------------------------
//	<LogImpl::GetNodeString>
//	Generate a string with formatted node id
//-----------------------------------------------------------------------------
string LogImpl::GetNodeString
(
		uint8 const _nodeId
)
{
	if( _nodeId == 0 )
	{
		return "";
	}
	else
		if( _nodeId == 255 ) // should make distinction between broadcast and controller better for SwitchAll broadcast
		{
			return "contrlr, ";
		}
		else
		{
			char buf[20];
			snprintf( buf, sizeof(buf), "Node%03d, ", _nodeId );
			return buf;
		}
}

//-----------------------------------------------------------------------------
//	<LogImpl::GetThreadId>
//	Generate a string with formatted thread id
//-----------------------------------------------------------------------------
string LogImpl::GetThreadId
(
)
{
	char buf[20];
	snprintf( buf, sizeof(buf), "%08lx", (long unsigned int)pthread_self() );
	string str = buf;
	return str;
}

//-----------------------------------------------------------------------------
//	<LogImpl::SetLogFileName>
//	Provide a new log file name (applicable to future writes)
//-----------------------------------------------------------------------------
void LogImpl::SetLogFileName
(
		const string &_filename
)
{
	m_filename = _filename;
}


//-----------------------------------------------------------------------------
//	<LogImpl::GetLogLevelString>
//	Provide a new log file name (applicable to future writes)
//-----------------------------------------------------------------------------
string LogImpl::GetLogLevelString
(
		LogLevel _level
)
{
	if ((_level >= LogLevel_None) && (_level <= LogLevel_Internal)) {
		char buf[20];
		snprintf( buf, sizeof(buf), "%s, ", LogLevelString[_level] );
		return buf;
	}
	else
		return "Unknown, ";
}
