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

#ifndef DISTYPES_HPP
#define DISTYPES_HPP

#ifdef _WIN32
// Prevent compiler warnings about debugging information getting truncated.
#pragma warning(disable : 4786)
#endif

#if defined(__host_mips) && _COMPILER_VERSION >= 740
// The template parameter "..." is not used in declaring the argument types of
//          function template "...".
#pragma set woff 1424
// an include directive is appearing within the scope of extern "C"
#pragma set woff 3625
#endif

typedef unsigned char      DisEnum8;
typedef unsigned short int DisEnum16;
typedef unsigned int       DisEnum32;
typedef signed char        DisInt8;
typedef short int          DisInt16;
typedef int                DisInt32;
typedef unsigned char      DisUint8;
typedef unsigned short int DisUint16;
typedef unsigned int       DisUint32;
#if defined(__vms) || defined(_WIN32)
typedef __int64          DisInt64;
typedef unsigned __int64 DisUint64;
#elif defined(__linux__)
#include <stdint.h>
typedef int64_t  DisInt64;
typedef uint64_t DisUint64;
#else
typedef long long int          DisInt64;
typedef unsigned long long int DisUint64;
#endif
typedef float  DisFloat32;
typedef double DisFloat64;

#endif
