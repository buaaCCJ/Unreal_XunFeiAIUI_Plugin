/*
 * macro.h
 *
 *  Created on: 2017年9月7日
 *      Author: hj
 */

#ifndef MACRO_H_
#define MACRO_H_

#if defined(AIUI_WINDOWS)
#define PATH_SEPERATOR "\\"
#define CHAR_PATH_SEPERATOR '\\'
#else
#define PATH_SEPERATOR "/"
#define CHAR_PATH_SEPERATOR '/'
#endif

#ifdef AIUI_DEBUG
#define DEBUG_ON 1
#else
#define DEBUG_ON 0
#endif

#define DEBUG_CONSOLE_PRINT(info) \
do\
{\
	if (DEBUG_ON)\
	{\
		cout << info << endl;\
	}\
} while(0)

#define DEBUG_OBJ_CREATE(name) \
do \
{\
	if (DEBUG_ON) \
	{\
		ObjCreateStat::getInstance().createObj(name);\
	}\
} while(0)

#define DEBUG_OBJ_RELEASE(name) \
do \
{\
	if (DEBUG_ON) \
	{\
		ObjCreateStat::getInstance().releaseObj(name);\
	}\
} while(0)

#endif /* MACRO_H_ */
