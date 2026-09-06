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
#ifndef WSFMTT_REFERENCEPOINT_HPP
#define WSFMTT_REFERENCEPOINT_HPP

#include "UtEntity.hpp"

//! A utility class that facilitates coordinate conversion between MTT and WSF.
//!
//! @note The NED frame referred to by this class is a local flat-earth frame whose
//!       origin is at the 'reference point'. It is NOT a local frame that is tangent
//!       to the surface of the earth!!!
//!
//! @note This uses the transverse Mercator projection algorithms as implemented
//!       in SUPPRESSOR.

class WsfMTT_ReferencePoint
{
public:
   WsfMTT_ReferencePoint();

   void SetReferenceLocationLLA(double aLat, double aLon, double aAlt);

   void ConvertNEDToLLA(const double aLocNED[3], double& aLat, double& aLon, double& aAlt) const;

   void ConvertNEDToWCS(const double aLocNED[3], double aLocWCS[3]) const;

   void ConvertWCSToNED(const double aLocWCS[3], double aLocNED[3]) const;

   void ConvertWCSVectorToNED(double aVectorNED[3], const double aVectorWCS[3]) const;

   void ConvertNEDVectorToWCS(double aVectorWCS[3], const double aVectorNED[3]) const;

   //! Returns a reference to an entity at the center of this local coordinate system
   UtEntity& GetReferenceEntity() { return mCenterEntity; }

private:
   UtEntity mCenterEntity;

   double mCenterLat;
   double mCenterLon;
   double mCenterAlt;
   double mCosCenterLat;
   double mSinCenterLat;
   double mCosCenterLon;
   double mSinCenterLon;
   double mRadius;
};
#endif
