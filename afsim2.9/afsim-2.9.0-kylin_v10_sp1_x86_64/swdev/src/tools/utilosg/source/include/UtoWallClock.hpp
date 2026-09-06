// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTOWALLCLOCK_HPP
#define UTOWALLCLOCK_HPP

/* UtoWallClock Time Functions
 * This class provides methods to determine the amount of wall clock time that
 * has elapsed.
 */

#include "utilosg_export.h"

namespace UtoWallClock
{
/* Return the elapsed wall clock time in seconds since program start.
 */
UTILOSG_EXPORT double GetClock();
} // namespace UtoWallClock

#endif
