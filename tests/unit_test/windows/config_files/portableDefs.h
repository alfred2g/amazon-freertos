#ifndef PORTABLE_DEFS_H_
#define PORTABLE_DEFS_H_

#define LWIP_ARCH_CC_H

//#include </usr/include/errno.h>
//#include </Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/errno.h>
#include <C:/mingw/include/errno.h>

/* redefine EWOULDBLOCK to be different than EAGAIN to broaden the test coverage
 * and cover more cases
 */
#undef EWOULDBLOCK
#define EWOULDBLOCK    300

#undef ENOTCONN
#define ENOTCONN 301

#define _WINSOCK_H
#define LWIP_TIMEVAL_PRIVATE 1

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "portable.h"


#endif /* ifndef PORTABLE_DEFS_H_ */
