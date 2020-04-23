/*
 * platform.h
 *
 *  Created on: 2017年10月30日
 *      Author: hj
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#ifdef AIUI_WINDOWS
#define snprintf _snprintf
long getpid();
#endif

void AIUISleep(long millis);

#ifndef AIUI_ANDROID
long gettid();
#endif

#endif /* PLATFORM_H_ */
