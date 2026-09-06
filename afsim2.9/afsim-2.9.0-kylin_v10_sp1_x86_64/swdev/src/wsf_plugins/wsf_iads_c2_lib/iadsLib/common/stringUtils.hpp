// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef HELIOS_C2_STRING_UTILS
#define HELIOS_C2_STRING_UTILS

#include <string>

#include "iadslib_export.h"

namespace il
{

IADSLIB_EXPORT bool equalsIgnoreCase(const char *a, const char *b);
IADSLIB_EXPORT bool equalsIgnoreCase(const std::string& a, const char *b);
IADSLIB_EXPORT bool equalsIgnoreCase(const char *a, const std::string& b);
IADSLIB_EXPORT bool equalsIgnoreCase(const std::string& a, const std::string& b);

}

#endif
