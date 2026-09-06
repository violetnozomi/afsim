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

#ifndef PAKINTTYPES_HPP
// Defines data types based on size
// These should be used in any packet structures instead of
// regular integral types
#if defined(_MSC_VER) && (_MSC_VER < 1300)
#define NO_64_BIT_INT
#endif

#define PAKINTTYPES_HPP
#ifndef _WIN32
#include <inttypes.h>
#else
typedef signed char    int8_t;
typedef unsigned char  uint8_t;
typedef signed short   int16_t;
typedef unsigned short uint16_t;
typedef signed int     int32_t;
typedef unsigned int   uint32_t;
#ifndef NO_64_BIT_INT
#ifdef _M_IA64 // microsoft's define for 64 bit machines
typedef signed long    int64_t;
typedef unsigned long  uint64_t;
#else
typedef signed long long   int64_t;
typedef unsigned long long uint64_t;
#endif
#endif
#endif

#undef NO_64_BIT_INT

#endif
