// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFNONEXPORTABLEUTIL_HPP
#define WSFNONEXPORTABLEUTIL_HPP

#include "wsf_nx_export.h"

#include <vector>

class UtCovariance;
#include "UtMatrix.hpp"
#include "UtVec3.hpp"
class WsfTrack;

//! A set of utilities that perform generic calculations among WSF objects.
//! The non-exportable version contains link-16 specific algorithms.
class WSF_NX_EXPORT WsfNonExportableUtil
{
public:
   static double GetLink16_TrackQuality(double aSimTime, const WsfTrack& aTrack);

   static double
   GetLink16_TrackQuality(double aSimTime, double aTrackUpdateTime, const UtCovariance& aCovar, double aSpeed = 0.0);

   static double GetLink16_TrackQuality(double aSimTime, double aTrackUpdateTime, double aVarTp, double aSpeed = 0.0);

   static UtCovariance* ConvertLink16_TrackQualityToSphericalCovariance(double aLink16_TrackQuality);

}; // end namespace WsfNonExportableUtil

#endif
