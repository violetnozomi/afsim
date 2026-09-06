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

#ifndef WKFRELATIVEGEOMETRYUTIL_HPP
#define WKFRELATIVEGEOMETRYUTIL_HPP

#include "ut_export.h"

#include "UtVec3.hpp"

class UtEntity;

namespace UtRelativeGeometry
{
double UT_EXPORT CalculateLinearRange(const UtEntity& aFrom, const UtEntity& aTo);
double UT_EXPORT CalculateRadial(const UtEntity& aFrom, const UtEntity& aTo);
double UT_EXPORT CalculateInTrack(const UtEntity& aFrom, const UtEntity& aTo);
double UT_EXPORT CalculateCrossTrack(const UtEntity& aFrom, const UtEntity& aTo);
void UT_EXPORT   CalculateRIC(const UtEntity& aFrom, const UtEntity& aTo, UtVec3d& aRIC);
double UT_EXPORT CalculateBearing(const UtEntity& aFrom, const UtEntity& aTo);
double UT_EXPORT CalculateElevation(const UtEntity& aFrom, const UtEntity& aTo);
double UT_EXPORT CalculateDownRange(const UtEntity& aFrom, const UtEntity& aTo);
double UT_EXPORT CalculateCrossRange(const UtEntity& aFrom, const UtEntity& aTo);
double UT_EXPORT CalculateRangeRate(const UtEntity& aFrom, const UtEntity& aTo);
} // namespace UtRelativeGeometry

#endif
