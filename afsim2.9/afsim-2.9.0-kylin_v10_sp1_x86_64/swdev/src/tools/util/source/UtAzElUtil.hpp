// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTAZEL_UTIL_HPP
#define UTAZEL_UTIL_HPP

#include "ut_export.h"

#include <string>

#include "UtAzElTypes.hpp"

//! Utility methods used by the signature classes.

namespace ut
{
namespace azel
{
namespace util
{
UT_EXPORT bool StringToEnum(InterpolationType& aInterpolationType, const std::string& aString);

UT_EXPORT std::string EnumToString(InterpolationType aInterpolationType);
} // namespace util
} // namespace azel
} // namespace ut

#endif
