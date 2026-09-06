// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#pragma once


#include "iadslib_export.h"

namespace NumericUtils
{
    // @return true if the lhs and rhs are equal within tolerance based on the
    //              type storage capability and the magnitude of the numbers.
    IADSLIB_EXPORT bool NearlyEqual(double lhs, double rhs);

    // @return true if the lhs is strictly less than the rhs
    //              A check is made for nearly equal before the less than check is made.
    //              If lhs is nearly equal to rhs, returns false.
    IADSLIB_EXPORT bool LessThan(double lhs, double rhs);

    // @return true if lhs is nearly equal to or less than the rhs.
    IADSLIB_EXPORT bool LessEqual(double lhs, double rhs);

    // @return true if the rhs is strictly greater than the rhs
    //              A check is made for nearly equal before the greater than check is made.
    //              If lhs is nearly equal to rhs, returns false.
    IADSLIB_EXPORT bool GreaterThan(double lhs, double rhs);

    // @return true if lhs is nearly equal to or greater than the rhs.
    IADSLIB_EXPORT bool GreaterThanEqual(double lhs, double rhs);
}