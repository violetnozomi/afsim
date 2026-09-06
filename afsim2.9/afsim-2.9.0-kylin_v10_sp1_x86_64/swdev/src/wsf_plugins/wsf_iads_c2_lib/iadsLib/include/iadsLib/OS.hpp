// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   OS.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#ifndef OS_HPP
#define OS_HPP

#include <time.h>
#include <sys/types.h>

/* pi */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* pi/2 */
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

/* pi/4 */
#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

// all platforms now have stdint.h
#include <stdint.h>

/* Automatically defined by gcc */
#ifdef __linux__
#include <endian.h>

#endif /* End Linux */

#ifdef __unix__
#include <sys/time.h>
#endif





#ifdef WIN32

#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN    4321
#define BYTE_ORDER LITTLE_ENDIAN

//bc typedef int64_t _int64;
//bc typedef uint64_t unsigned _int64;

#include <sys/timeb.h>

#endif /* End Win32 */





#endif
