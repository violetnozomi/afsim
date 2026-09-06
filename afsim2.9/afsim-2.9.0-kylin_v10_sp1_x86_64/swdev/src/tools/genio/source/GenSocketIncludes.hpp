// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#if !defined(GENSOCKETINCLUDES_HPP)
#define GENSOCKETINCLUDES_HPP

#if defined(_WIN32)
#include <cstdio>

#include <winsock2.h>

#include <windows.h>

#include <Ws2tcpip.h>
#include <fcntl.h>
#include <io.h>
#include <winbase.h>
// prevent Microsoft's #define SetPort SetPortA
#undef SetPort

void        errorMessage(int errnoA);
typedef int socklen_t;
#else

#if defined(sgi) && (_COMPILER_VERSION >= 720)
#pragma set woff 1253, 3625
#endif
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
// for blocking
#include <sys/ioctl.h>
#endif

#ifdef __sgi
typedef int socklen_t;
#endif

#if defined(sgi) && (_COMPILER_VERSION >= 720)
#pragma reset woff 1253, 3625
#endif

#endif
