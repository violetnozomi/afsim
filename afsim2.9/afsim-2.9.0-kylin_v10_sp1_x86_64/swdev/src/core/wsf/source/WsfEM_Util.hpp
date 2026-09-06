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

#ifndef WSFEM_UTIL_HPP
#define WSFEM_UTIL_HPP

#include "wsf_export.h"

#include <string>

class UtEntity;
#include "WsfEM_Types.hpp"

//! Utility methods used by the electromagnetic classes.

class WSF_EXPORT WsfEM_Util
{
public:
   static bool ComputeApparentPosition(double       aEarthRadiusScale,
                                       const double aSrcLocWCS[3],
                                       const double aTgtLocWCS[3],
                                       double       aApparentSrcLocWCS[3],
                                       double       aApparentTgtLocWCS[3]);

   static bool ComputeApparentPosition(double       aEarthRadiusScale,
                                       const double aSrcLocWCS[3],
                                       const double aTgtLocWCS[3],
                                       double       aSrcAlt,
                                       double       aTgtAlt,
                                       double       aApparentSrcLocWCS[3],
                                       double       aApparentTgtLocWCS[3]);

   static void ComputeViewerAspect(UtEntity*    aTargetPtr,
                                   const double aViewerLocWCS[3],
                                   double       aEarthRadiusScale,
                                   double&      aTargetToViewerAz,
                                   double&      aTargetToViewerEl);

   static bool MaskedByHorizon(double aLat1,
                               double aLon1,
                               double aAlt1,
                               double aTrnHgt1,
                               double aLat2,
                               double aLon2,
                               double aAlt2,
                               double aTrnHgt2,
                               double aEarthRadiusScale);

   static bool StringToEnum(WsfEM_Types::Polarization& aPolarization, const std::string& aString);

   static bool StringToEnum(WsfEM_Types::InfraredBand& aBand, const std::string& aString);

   static bool StringToEnum(WsfEM_Types::OpticalBand& aBand, const std::string& aString);

   static std::string EnumToString(WsfEM_Types::Polarization aPolarization);

   static std::string EnumToString(WsfEM_Types::InfraredBand aBand);

   static std::string EnumToString(WsfEM_Types::OpticalBand aBand);
};

#endif
