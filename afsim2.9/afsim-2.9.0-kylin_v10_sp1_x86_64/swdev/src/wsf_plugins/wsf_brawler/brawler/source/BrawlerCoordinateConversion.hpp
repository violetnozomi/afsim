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

#ifndef BRAWLERCOORDINATECONVERSION_HPP
#define BRAWLERCOORDINATECONVERSION_HPP

#include "brawler_export.h"

#include "UtEntity.hpp"
#include "UtVec3.hpp"

///////////////////////////////////////////////////////////////////////////////
//
// This class converts between a geodetic (LLA) coordinate system and
// a flat-earth (NED) coordinate system.  Right now a simple round earth
// approximation is being used, but a more robust conversion should be
// implemented: LLC, TM, or EQR.
//
///////////////////////////////////////////////////////////////////////////////
class BRAWLER_EXPORT BrawlerCoordinateConversion
{
public:
   // LLA in (degrees, degrees, meters)
   // NED in (feet, feet, feet)
   static UtVec3d ConvertLLAtoNED(UtVec3d& lla);
   static UtVec3d ConvertNEDtoLLA(UtVec3d& ned);

   static UtVec3d GetPositionNED(UtEntity& ref);
   static UtVec3d GetVelocityNED(UtEntity& ref);     // TODO - test this
   static UtVec3d GetAccelerationNED(UtEntity& ref); // TODO - test this
   static UtVec3d GetOrientationNED(UtEntity& ref);  // TODO - test this

   static UtVec3d RelativePositionNED(UtEntity& ref, UtEntity& tgt);
   static UtVec3d RelativePositionNED(UtVec3d& llaRef, UtVec3d& llaTgt);
   static UtVec3d RelativeVelocityNED(UtEntity& ref, UtEntity& tgt);

   static void RegisterLocationLLA(UtVec3d& lla);
   static void RegisterLocationEntity(UtEntity& ref);

   // TODO - these are place holders until we get a proper flat earth coordinate conversion in place
   static bool   mReferenceSet;
   static double mRefLat;
   static double mRefLon;
};

#endif
