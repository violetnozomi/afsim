// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   enum.cpp
 *
 **  Classification: UNCLASSIFIED
 *
 */

#include <iadsLib/enum.hpp>

#ifndef IADSLIB_ENUM_CPP
#define IADSLIB_ENUM_CPP

namespace il
{

std::string systemStatusToString (il::SYSTEM_STATUS statusEnum)
{
	switch (statusEnum)
	{
	case STATUS_GREEN:
		return "GREEN";
	case STATUS_YELLOW:
		return "YELLOW";
	case STATUS_RED:
		return "RED";
	case STATUS_WHITE:
		return "WHITE";
	default:
		return "Unknown";
   }
}
}

#endif
