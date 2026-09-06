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

#include "VaPosition.hpp"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>

#include "EquidistantProjection.hpp"
#include "MapProjection.hpp"
#include "OrthoProjection.hpp"
#include "TMProjection.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtLLAPos.hpp"
#include "UtLLPos.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "UtoMapProjection.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"

namespace vespa
{
MapProjection*                   VaPosition::mProjection(nullptr);
VaPosition::ProjectionMode       VaPosition::mProjectionMode(PmNone);
UtLatPos                         VaPosition::mCenterLat(0.0);
UtLonPos                         VaPosition::mCenterLon(0.0);
bool                             VaPosition::mDefaultPositionIsXY(false);
VaCallbackHolder                 VaPosition::msCallbacks;
std::map<int, UtoMapProjection*> VaPosition::msSceneProjectionMap;


// static
void VaPosition::InitializeStaticSubscriptions()
{
   msCallbacks.Add(VaObserver::MapProjectionChanged.Connect(&VaPosition::ChangeMapProjectionCB));
}

void VaPosition::Shutdown()
{
   msCallbacks.Clear();
}

VaPosition::VaPosition()
   : mLat()
   , mLon()
   , mX()
   , mY()
   , mAlt()
   , mPositionIsXY(false)
   , mElevIsInvalid(true)
   , mECEF_Defined(false)
{
   SetAltFormat(UtLength::FmtFT + 2);

   mECEF[0] = 0.0;
   mECEF[1] = 0.0;
   mECEF[2] = 0.0;

   if (mProjectionMode != PmNone)
   {
      mPositionIsXY = mDefaultPositionIsXY;
   }
}

VaPosition::VaPosition(double aLat, double aLon, double aAlt)
   : mLat(aLat)
   , mLon(aLon)
   , mX()
   , mY()
   , mAlt(aAlt)
   , mPositionIsXY(false)
   , mElevIsInvalid(true)
   , mECEF_Defined(false)
{
   SetAltFormat(UtLength::FmtFT + 2);

   mECEF[0] = 0.0;
   mECEF[1] = 0.0;
   mECEF[2] = 0.0;
}

VaPosition::VaPosition(double aLat, double aLon, double aAlt, double aECEFx, double aECEFy, double aECEFz)
   : mLat(aLat)
   , mLon(aLon)
   , mX()
   , mY()
   , mAlt(aAlt)
   , mPositionIsXY(false)
   , mElevIsInvalid(true)
   , mECEF_Defined(true)
{
   SetAltFormat(UtLength::FmtFT + 2);

   mECEF[0] = aECEFx;
   mECEF[1] = aECEFy;
   mECEF[2] = aECEFz;
}

VaPosition::VaPosition(double aX, double aY)
   : mLat()
   , mLon()
   , mX()
   , mY()
   , mAlt()
   , mPositionIsXY(false)
   , mElevIsInvalid(true)
   , mECEF_Defined(false)
{
   SetX(aX);
   SetY(aY);

   SetAltFormat(UtLength::FmtFT + 2);

   mECEF[0] = 0.0;
   mECEF[1] = 0.0;
   mECEF[2] = 0.0;
}

VaPosition::VaPosition(const VaPosition& aSrc)
   : mLat(aSrc.mLat)
   , mLon(aSrc.mLon)
   , mX(aSrc.mX)
   , mY(aSrc.mY)
   , mAlt(aSrc.mAlt)
   , mPositionIsXY(aSrc.mPositionIsXY)
   , mElevIsInvalid(aSrc.mElevIsInvalid)
   , mECEF_Defined(aSrc.mECEF_Defined)
{
   mECEF[0] = aSrc.mECEF[0];
   mECEF[1] = aSrc.mECEF[1];
   mECEF[2] = aSrc.mECEF[2];
}

VaPosition& VaPosition::operator=(const VaPosition& aPosition)
{
   mLat           = aPosition.mLat;
   mLon           = aPosition.mLon;
   mX             = aPosition.mX;
   mY             = aPosition.mY;
   mAlt           = aPosition.mAlt;
   mPositionIsXY  = aPosition.mPositionIsXY;
   mElevIsInvalid = aPosition.mElevIsInvalid;
   mECEF_Defined  = aPosition.mECEF_Defined;
   mECEF[0]       = aPosition.mECEF[0];
   mECEF[1]       = aPosition.mECEF[1];
   mECEF[2]       = aPosition.mECEF[2];
   return *this;
}

bool VaPosition::operator==(const VaPosition& aRhs) const
{
   return ((fabs(mLat - aRhs.mLat) < 0.0000001) && (fabs(mLon - aRhs.mLon) < 0.0000001) && (fabs(mAlt - aRhs.mAlt) < 0.01));
}

bool VaPosition::operator!=(const VaPosition& aRhs) const
{
   return !(operator==(aRhs));
}

// Marks the position as dirty. The next time data is
// requested, the position will recalculate things.
void VaPosition::DirtyPosition()
{
   mECEF_Defined  = false;
   mElevIsInvalid = true;
}

// Get the current latitude, longitude and altitude.  If the object
// is defined with an X-Y position then the equivalent latitude and
// longitude will be returned.  If the altitude is AGL then the MSL
// altitude will be returned.

void VaPosition::GetLLA(UtLLAPos& aLLA) const
{
   aLLA.SetLat(mLat);
   aLLA.SetLon(mLon);
   aLLA.SetAlt(mAlt);
}

void VaPosition::MoveRelative(UtEntity& aRefBefore, UtEntity& aRefAfter, int aFeedbackFormat, double aFeedbackPrecision)
{
   double lat;
   double lon;
   double alt;
   double ned[3];
   aRefBefore.ConvertLLAToNED(mLat, mLon, 0, ned);
   aRefAfter.ConvertNEDToLLA(ned, lat, lon, alt);

   // A non-positive feedback precision is defined by the Viewer to prevent
   // truncation of the coordinates during dragging.  Truncation is done only
   // when the drag is complete in order to prevent a creeping error.

   if (aFeedbackPrecision > 0.0)
   {
      lat -= fmod(lat, aFeedbackPrecision);
      lon -= fmod(lon, aFeedbackPrecision);
   }
   mLat.Set(lat, aFeedbackFormat);
   mLon.Set(lon, aFeedbackFormat);
   UpdateXY();
   mElevIsInvalid = true;
}

void VaPosition::CalculateGreatCircleHeadingAndDistance(const VaPosition& aPosition, double& aHeading, double& aDistance) const
{
   UtSphericalEarth::GreatCircleHeadingAndDistance(mLat, mLon, aPosition.GetLat(), aPosition.GetLon(), aHeading, aDistance);
}

void VaPosition::CalculateHeadingAndDistance(const VaPosition& aPosition, double& aHeading, double& aDistance) const
{
   double srcXYZ[3];
   GetECEF(srcXYZ);

   double dstXYZ[3];
   aPosition.GetECEF(dstXYZ);

   double vecXYZ[3];
   UtVec3d::Subtract(vecXYZ, srcXYZ, dstXYZ);
   aDistance = UtVec3d::Magnitude(vecXYZ);

   double nedTransform[3][3];
   UtEllipsoidalEarth::ComputeNEDTransform(GetLat(), GetLon(), GetAlt(), nedTransform, srcXYZ);

   double dstNED[3];
   UtEllipsoidalEarth::ConvertECEFToLocal(nedTransform, dstXYZ, dstNED);

   aHeading = 0.0;
   if (dstNED[0] != 0.0 || dstNED[1] != 0.0)
   {
      aHeading = atan2(dstNED[1], dstNED[0]) * UtMath::cDEG_PER_RAD;
   }
}

// 'Translate' is used when moving the entire scenario from one location
// to another.  For relative scenarios players will maintain the same
// x/y spacing.  For absolute scenarios they will maintain the same
// lat/lon spacing.
//
// NOTE: TranslateProjectionCenter should be called prior to this routine.

void VaPosition::Translate(double aDeltaLat, double aDeltaLon)
{
   if (mProjection != nullptr)
   {
      // mX and mY contain the X/Y that corresponded to the Lat/Lon based on the
      // old center of scenario.  Using that X/Y compute the Lat/Lon with the new
      // center of scenario.  This way relative distances remain constant in
      // relative scenarios.

      if (mPositionIsXY)
      {
         UpdateLatLon();
      }
      else
      {
         double lat;
         double lon;
         mProjection->ConvertXYToLatLon(mX, mY, lat, lon);
         mLat.Set(lat, UtAngle::FmtDMS + 1);
         mLon.Set(lon, UtAngle::FmtDMS + 1);
         UpdateXY();
      }
   }
   else
   {
      UtLLPos ll(mLat, mLon);
      ll += UtLLPos(aDeltaLat, aDeltaLon);
      mLat.Set(ll.GetLat(), UtAngle::FmtDMS + 1);
      mLon.Set(ll.GetLon(), UtAngle::FmtDMS + 1);
      UpdateXY();
   }
   mECEF_Defined  = false;
   mElevIsInvalid = true;
}


void VaPosition::Rotate(const UtLLAPos& aCenterLLA, double aAngle)
{
   double lat = GetLat();
   double lon = GetLon();

   if (mProjection != nullptr)
   {
      // mX and mY contain the X/Y that corresponded to the Lat/Lon based on the
      // old center of scenario.  Using that X/Y compute the Lat/Lon with the new
      // center of scenario.  This way relative distances remain constant in
      // relative scenarios.

      if (mPositionIsXY)
      {
         UpdateLatLon();
      }
      else
      {
         double lat;
         double lon;
         mProjection->ConvertXYToLatLon(mX, mY, lat, lon);
         mLat.Set(lat, UtAngle::FmtDMS + 1);
         mLon.Set(lon, UtAngle::FmtDMS + 1);
         UpdateXY();
      }
   }

   double heading  = 0.0;
   double distance = 0.0;
   UtSphericalEarth::GreatCircleHeadingAndDistance(aCenterLLA.GetLat(), aCenterLLA.GetLon(), lat, lon, heading, distance);
   heading += aAngle;
   heading = UtMath::NormalizeAngle0_360(heading);

   double newLat = 0.0;
   double newLon = 0.0;
   UtSphericalEarth::ExtrapolateGreatCirclePosition(aCenterLLA.GetLat(), aCenterLLA.GetLon(), heading, distance, newLat, newLon);
   mLat = newLat;
   mLon = newLon;
   UpdateXY();

   mECEF_Defined  = false;
   mElevIsInvalid = true;
}

void VaPosition::SetLLA(const UtLLAPos& aLLA)
{
   aLLA.GetLat(mLat);
   aLLA.GetLon(mLon);
   aLLA.GetAlt(mAlt);
   UpdateXY();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

void VaPosition::SetLLA(const UtLatPos& aLat, const UtLonPos& aLon, const UtLength& aAlt)
{
   mLat = aLat;
   mLon = aLon;
   mAlt = aAlt;
   UpdateXY();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

void VaPosition::SetAlt(const UtLength& aAlt)
{
   mAlt          = aAlt;
   mECEF_Defined = false;
}

void VaPosition::SetAlt(double aAlt)
{
   mAlt.Set(aAlt, mAlt.GetFormat());
   mECEF_Defined = false;
}

void VaPosition::SetAltFormat(int aFormat)
{
   mAlt.SetFormat(aFormat);
}

void VaPosition::SetLat(const UtLatPos& aLat)
{
   mLat = aLat;
   UpdateXY();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

void VaPosition::SetLat(double aLat)
{
   mLat.Set(aLat, mLat.GetFormat());
   UpdateXY();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

void VaPosition::SetLatFormat(int aFormat)
{
   mLat.SetFormat(aFormat);
}

void VaPosition::SetLon(const UtLonPos& aLon)
{
   mLon = aLon;
   UpdateXY();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

void VaPosition::SetLon(double aLon)
{
   mLon.Set(aLon, mLon.GetFormat());
   UpdateXY();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

void VaPosition::SetLonFormat(int aFormat)
{
   mLon.SetFormat(aFormat);
}

void VaPosition::SetX(double aX)
{
   mX = aX;
   mX.SetFormat(UtLength::FmtM);
   mPositionIsXY = true;
   UpdateLatLon();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

void VaPosition::SetY(double aY)
{
   mY = aY;
   mY.SetFormat(UtLength::FmtM);
   mPositionIsXY = true;
   UpdateLatLon();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

void VaPosition::SetX(const UtLength& aX)
{
   mX            = aX;
   mPositionIsXY = true;
   UpdateLatLon();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

void VaPosition::SetY(const UtLength& aY)
{
   mY            = aY;
   mPositionIsXY = true;
   UpdateLatLon();
   mElevIsInvalid = true;
   mECEF_Defined  = false;
}

// static
void VaPosition::SetProjectionMode(ProjectionMode aProjectionMode)
{
   delete mProjection;
   mProjection     = nullptr;
   mProjectionMode = aProjectionMode;
   switch (aProjectionMode)
   {
   case PmSupOrthographic:
      mProjection = new OrthoProjection();
      break;
   case PmSupTransverseMercator:
      mProjection = new TMProjection();
      break;
   case PmEquidistant:
      mProjection = new EquidistantProjection();
      break;
   case PmIwars:
      mProjection = new EquidistantProjection(6356784.1);
      break;
   case PmNone:
      break;
   default:
      auto out = ut::log::warning() << "SetProjectionMode: Unknown projection mode.";
      out.AddNote() << "Project Mode: " << aProjectionMode;
      break;
   }
   if (mProjection != nullptr)
   {
      mProjection->SetCenter(mCenterLat, mCenterLon);
   }
}

// static
void VaPosition::SetProjectionCenter(const UtLatPos& aCenterLat, const UtLonPos& aCenterLon)
{
   mCenterLat = aCenterLat;
   mCenterLon = aCenterLon;
   if (mProjection != nullptr)
   {
      mProjection->SetCenter(mCenterLat, mCenterLon);
   }
}

// static
void VaPosition::TranslateProjectionCenter(double aDeltaLat, double aDeltaLon)
{
   UtLLPos ll(mCenterLat, mCenterLon);
   ll += UtLLPos(aDeltaLat, aDeltaLon);
   mCenterLat.Set(ll.GetLat(), UtAngle::FmtDMS + 1);
   mCenterLon.Set(ll.GetLon(), UtAngle::FmtDMS + 1);
   if (mProjection != nullptr)
   {
      mProjection->SetCenter(mCenterLat, mCenterLon);
   }
}

// static
void VaPosition::Reset()
{
   mCenterLat = 0.0;
   mCenterLon = 0.0;

   delete mProjection;
   mProjection = nullptr;

   mProjectionMode      = PmNone;
   mDefaultPositionIsXY = false;
}

// Based on the current projection, compute the Lat/Lon given the X/Y.

// private
void VaPosition::UpdateLatLon()
{
   if (mProjection != nullptr)
   {
      double lat;
      double lon;
      mProjection->ConvertXYToLatLon(mX, mY, lat, lon);
      mLat.Set(lat, UtAngle::FmtDMS + 1);
      mLon.Set(lon, UtAngle::FmtDMS + 1);
   }
   mECEF_Defined = false;
}

// Based on the current projection, compute the X/Y given the Lat/Lon.

// private
void VaPosition::UpdateXY()
{
   if (mProjection != nullptr)
   {
      double x;
      double y;
      mProjection->ConvertLatLonToXY(mLat, mLon, x, y);
      mX.Set(x, UtLength::FmtFT + 1);
      mY.Set(y, UtLength::FmtFT + 1);
   }
   mECEF_Defined = false;
}

// this returns a WGS84 earth centered earth fixed coordinate
// assumes LLA is available
const double VaPosition::GetECEF_X() const
{
   if (!mECEF_Defined)
   {
      UtEllipsoidalEarth::ConvertLLAToECEF(mLat, mLon, mAlt, mECEF);
      mECEF_Defined = true;
   }
   return mECEF[0];
}

// this returns a WGS84 earth centered earth fixed coordinate
// assumes LLA is available
const double VaPosition::GetECEF_Y() const
{
   if (!mECEF_Defined)
   {
      UtEllipsoidalEarth::ConvertLLAToECEF(mLat, mLon, mAlt, mECEF);
      mECEF_Defined = true;
   }
   return mECEF[1];
}

// this returns a WGS84 earth centered earth fixed coordinate
// assumes LLA is available
const double VaPosition::GetECEF_Z() const
{
   if (!mECEF_Defined)
   {
      UtEllipsoidalEarth::ConvertLLAToECEF(mLat, mLon, mAlt, mECEF);
      mECEF_Defined = true;
   }
   return mECEF[2];
}

void VaPosition::GetECEF(double aECEF[3]) const
{
   if (!mECEF_Defined)
   {
      UtEllipsoidalEarth::ConvertLLAToECEF(mLat, mLon, mAlt, mECEF);
      mECEF_Defined = true;
   }
   aECEF[0] = mECEF[0];
   aECEF[1] = mECEF[1];
   aECEF[2] = mECEF[2];
}

void VaPosition::ConvertLLAToXYA(const UtLLAPos& aLLA, const UtLLAPos& aRefLLA, double aXYA[3])
{
   // Convert using local ENU from the supplied reference point
   double enuTrans[3][3];
   double enuRef[3];
   double enu[3];

   // NOTE: Both ConvertLLAtoXYA and ConvertXYAToLLA use ignore the altitude in the incoming
   //       reference point and use zero instead.  The rest of the code assumes A is altitude
   //       and this makes it so...

   UtEllipsoidalEarth::ComputeENUTransform(aRefLLA.GetLat(), aRefLLA.GetLon(), 0.0, enuTrans, enuRef);
   UtEllipsoidalEarth::ConvertLLAToLocal(enuRef, enuTrans, aLLA.GetLat(), aLLA.GetLon(), aLLA.GetAlt(), enu);
   UtVec3d::Set(aXYA, enu);
}

void VaPosition::ConvertLLAToXYA(const UtLLAPos& aLLA, const UtLLAPos& aRefLLA, UtVec3d& aXYA)
{
   ConvertLLAToXYA(aLLA, aRefLLA, aXYA.GetData());
}

void VaPosition::ConvertXYAToLLA(const double aXYA[3], const UtLLAPos& aRefLLA, UtLLAPos& aLLA)
{
   // Convert using local ENU from the supplied reference point
   double enuTrans[3][3];
   double enuRef[3];
   double lat;
   double lon;
   double alt;

   // NOTE: Both ConvertLLAtoXYA and ConvertXYAToLLA use ignore the altitude in the incoming
   //       reference point and use zero instead.  The rest of the code assumes A is altitude
   //       and this makes it so...

   UtEllipsoidalEarth::ComputeENUTransform(aRefLLA.GetLat(), aRefLLA.GetLon(), 0.0, enuTrans, enuRef);
   UtEllipsoidalEarth::ConvertLocalToLLA(enuRef, enuTrans, aXYA, lat, lon, alt);
   aLLA.SetLat(lat);
   aLLA.SetLon(lon);
   aLLA.SetAlt(alt);
}

void VaPosition::ConvertXYAToLLA(const UtVec3d& aXYA, const UtLLAPos& aRefLLA, UtLLAPos& aLLA)
{
   ConvertXYAToLLA(aXYA.GetData(), aRefLLA, aLLA);
}

void VaPosition::GetSceneXYZ(double aXYZ[3], int aViewerId) const
{
   double alt = mAlt;
   if (alt < -10000)
   {
      alt = 0;
   }
   std::map<int, UtoMapProjection*>::iterator it = msSceneProjectionMap.find(aViewerId);
   if ((it == msSceneProjectionMap.end()) && (aViewerId == 0) && (!msSceneProjectionMap.empty()))
   {
      it = msSceneProjectionMap.begin();
   }
   if (it != msSceneProjectionMap.end())
   {
      double posXYZ[3];
      it->second->ConvertFromLLA(mLat, mLon, alt, posXYZ);
      aXYZ[0] = posXYZ[0];
      aXYZ[1] = posXYZ[1];
      aXYZ[2] = posXYZ[2];
   }
   else
   {
      double posXYZ[3];
      UtEllipsoidalEarth::ConvertLLAToECEF(mLat, mLon, alt, posXYZ);
      aXYZ[0] = posXYZ[0];
      aXYZ[1] = posXYZ[1];
      aXYZ[2] = posXYZ[2];
   }
}

// static
VaPosition VaPosition::GetPositionOfSceneXYZ(const double aXYZ[3], int aViewerId)
{
   double                                     posXYZ[3] = {0, 0, 0};
   std::map<int, UtoMapProjection*>::iterator it        = msSceneProjectionMap.find(aViewerId);
   if ((it == msSceneProjectionMap.end()) && (aViewerId == 0) && (!msSceneProjectionMap.empty()))
   {
      it = msSceneProjectionMap.begin();
   }
   if (it != msSceneProjectionMap.end())
   {
      it->second->ConvertToLLA(aXYZ, posXYZ);
   }
   return VaPosition(posXYZ[0], posXYZ[1], posXYZ[2]);
}

// static
void VaPosition::ChangeMapProjectionCB(int aViewerId, const UtoMapProjection* aSceneProjection)
{
   UtoMapProjection*                          sceneProjection = (UtoMapProjection*)aSceneProjection;
   std::map<int, UtoMapProjection*>::iterator it              = msSceneProjectionMap.find(aViewerId);
   if (it != msSceneProjectionMap.end())
   {
      msSceneProjectionMap.erase(it);
   }
   if (aSceneProjection)
   {
      msSceneProjectionMap[aViewerId] = sceneProjection;
   }
}

// static
int VaPosition::GetSceneProjectionType(int aViewerId)
{
   UtoMapProjection::Projection               projection = UtoMapProjection::Geocentric;
   std::map<int, UtoMapProjection*>::iterator it         = msSceneProjectionMap.find(aViewerId);
   if (it != msSceneProjectionMap.end())
   {
      projection = it->second->projection;
   }
   return projection;
}

void VaPosition::GetSceneUpVector(double aXYZ[3], int aViewerId) const
{
   std::map<int, UtoMapProjection*>::iterator it = msSceneProjectionMap.find(aViewerId);

   if (it != msSceneProjectionMap.end())
   {
      if (it->second->projection == UtoMapProjection::Equirectangular)
      {
         aXYZ[0] = 0;
         aXYZ[1] = 0;
         aXYZ[2] = 1;
      }
      else if (it->second->projection == UtoMapProjection::Geocentric)
      {
         GetSceneXYZ(aXYZ, aViewerId);
         double invLen = 1 / sqrt(aXYZ[0] * aXYZ[0] + aXYZ[1] * aXYZ[1] + aXYZ[2] * aXYZ[2]);
         aXYZ[0] *= invLen;
         aXYZ[1] *= invLen;
         aXYZ[2] *= invLen;
      }
   }
   else
   {
      GetSceneXYZ(aXYZ, aViewerId);
      double invLen = 1 / sqrt(aXYZ[0] * aXYZ[0] + aXYZ[1] * aXYZ[1] + aXYZ[2] * aXYZ[2]);
      aXYZ[0] *= invLen;
      aXYZ[1] *= invLen;
      aXYZ[2] *= invLen;
   }
}

vespa::VaPosition VaPosition::ComputeCentroid(const std::vector<VaPosition>& aPositionList)
{
   UtVec3d centroid;
   double  altitudeSum = 0;
   for (std::vector<VaPosition>::const_iterator i = aPositionList.begin(); i != aPositionList.end(); ++i)
   {
      altitudeSum += i->GetAlt();
      UtVec3d wcs;
      i->GetECEF(wcs.GetData());
      centroid += wcs;
   }
   if (!aPositionList.empty())
   {
      centroid /= aPositionList.size();
      double avgAlt = altitudeSum / aPositionList.size();
      double lat, lon, alt;
      UtEllipsoidalEarth::ConvertECEFToLLA(centroid.GetData(), lat, lon, alt);
      return VaPosition(lat, lon, avgAlt);
   }
   return VaPosition();
}

UtoMapProjection* VaPosition::GetSceneProjection(int aViewerId)
{
   std::map<int, UtoMapProjection*>::iterator it = msSceneProjectionMap.find(aViewerId);
   if (it != msSceneProjectionMap.end())
   {
      return it->second;
   }
   else if (!msSceneProjectionMap.empty())
   {
      return msSceneProjectionMap.begin()->second;
   }
   else
   {
      return nullptr;
   }
}

} // end namespace vespa
