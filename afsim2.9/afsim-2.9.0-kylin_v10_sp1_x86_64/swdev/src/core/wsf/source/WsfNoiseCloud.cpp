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

#include "WsfNoiseCloud.hpp"

#include <memory>

#include "UtEarth.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtQuaternion.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfEnvironment.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

WsfNoiseCloudTypes::WsfNoiseCloudTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfNoiseCloud>(aScenario, ObjectTypeList::cREDEFINITION_ALLOWED, "noise_cloud")
{
   AddCoreType("WSF_NOISE_CLOUD", ut::make_unique<WsfNoiseCloud>());
}

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfNoiseCloudTypes& WsfNoiseCloudTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetNoiseCloudTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfNoiseCloudTypes& WsfNoiseCloudTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetNoiseCloudTypes();
}

// =================================================================================================
//! Constructor.
WsfNoiseCloud::WsfNoiseCloud()
   : mNoiseFunctionPtr(nullptr)
   , mFrequency(1)
   , mOctaves(1)
   , mThreshold(0.5)
   , mHeight(10000)
   , mThickness(300)
   , mTileScalar(10000)
   , mUseGlobalWind(false)
   , mLat(0.0)
   , mLon(0.0)
   , mAngularSpeed(0.0)
{
   mWindAxis.Set(0, 0, 1);
}

// =================================================================================================
//! Copy constructor.
WsfNoiseCloud::WsfNoiseCloud(const WsfNoiseCloud& aSrc)
   : mRandom(aSrc.mRandom)
   , mNoiseFunctionPtr(nullptr)
   , mFrequency(aSrc.mFrequency)
   , mOctaves(aSrc.mOctaves)
   , mThreshold(aSrc.mThreshold)
   , mHeight(aSrc.mHeight)
   , mThickness(aSrc.mThickness)
   , mTileScalar(aSrc.mTileScalar)
   , mUseGlobalWind(aSrc.mUseGlobalWind)
   , mModifierList(aSrc.mModifierList)
   , mLat(aSrc.mLat)
   , mLon(aSrc.mLon)
   , mAngularSpeed(aSrc.mAngularSpeed)
   , mWindAxis(aSrc.mWindAxis)
{
}

// =================================================================================================
//! Destructor
// virtual
WsfNoiseCloud::~WsfNoiseCloud()
{
   delete mNoiseFunctionPtr;
}

// =================================================================================================
//! Standard Clone() method.
// virtual
WsfNoiseCloud* WsfNoiseCloud::Clone() const
{
   return new WsfNoiseCloud(*this);
}

// =================================================================================================
// virtual
bool WsfNoiseCloud::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command;
   aInput.GetCommand(command);
   if (command == "noise_frequency")
   {
      aInput.ReadValue(mFrequency);
   }
   else if (command == "noise_octaves")
   {
      aInput.ReadValue(mOctaves);
   }
   else if (command == "threshold")
   {
      aInput.ReadValue(mThreshold);
   }
   else if (command == "comm_modifier")
   {
      // comm_modifier <category-name> <modifier-value>
      std::string categoryName;
      aInput.ReadCommand(categoryName);
      double modifierValue;
      aInput.ReadValue(modifierValue);
      mModifierList[categoryName] = modifierValue;
   }
   else if (command == "sensor_modifier")
   {
      // sensor_modifier <category-name> <modifier-value>
      std::string categoryName;
      aInput.ReadCommand(categoryName);
      double modifierValue;
      aInput.ReadValue(modifierValue);
      mModifierList[categoryName] = modifierValue;
   }
   else if (command == "constant_height")
   {
      aInput.ReadValueOfType(mHeight, UtInput::cLENGTH);
   }
   else if (command == "thickness")
   {
      aInput.ReadValueOfType(mThickness, UtInput::cLENGTH);
   }
   else if (command == "scalar")
   {
      aInput.ReadValueOfType(mTileScalar, UtInput::cLENGTH);
   }
   else if (command == "origin")
   {
      aInput.ReadValueOfType(mLat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(mLon, UtInput::cLONGITUDE);
   }
   else if (command == "use_global_wind")
   {
      mUseGlobalWind = true;
      // calculate the rotation vector
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
double WsfNoiseCloud::CheckIntersections(double aSimTime, const double aLLA1[3], const double aLLA2[3])
{
   assert(mNoiseFunctionPtr); // null if not initialized
   double      ellipseMaj = UtEarth::cA + mHeight;
   double      ellipseMin = UtEarth::cB + mHeight;
   WsfGeoPoint a;
   double      xyz1[3];
   a.SetLocationLLA(aLLA1[0], aLLA1[1], aLLA1[2]);
   a.GetLocationWCS(xyz1);
   WsfGeoPoint b;
   b.SetLocationLLA(aLLA2[0], aLLA2[1], aLLA2[2]);
   double xyz2[3];
   b.GetLocationWCS(xyz2);

   xyz1[0] /= ellipseMaj;
   xyz1[1] /= ellipseMaj;
   xyz1[2] /= ellipseMin;
   xyz2[0] /= ellipseMaj;
   xyz2[1] /= ellipseMaj;
   xyz2[2] /= ellipseMin;
   double dxyz[3] = {xyz2[0] - xyz1[0], xyz2[1] - xyz1[1], xyz2[2] - xyz1[2]};
   // find the intersection of the unit sphere
   double A = dxyz[0] * dxyz[0] + dxyz[1] * dxyz[1] + dxyz[2] * dxyz[2];
   double B = 2 * (xyz1[0] * dxyz[0] + xyz1[1] * dxyz[1] + xyz1[2] * dxyz[2]);
   double C = xyz1[0] * xyz1[0] + xyz1[1] * xyz1[1] + xyz1[2] * xyz1[2] - 1;

   double disc = B * B - 4 * A * C;
   if (disc < 0)
   {
      return 0.0;
   }
   else
   {
      double penetration = 0.0;
      // bool isect = false;
      double t1   = (-B + sqrt(disc)) / (2 * A);
      double t2   = (-B - sqrt(disc)) / (2 * A);
      double time = aSimTime;
      if ((t1 >= 0) && (t1 <= 1))
      {
         // isect = true;
         double       ixyz[3] = {ellipseMaj * (xyz1[0] + t1 * dxyz[0]),
                           ellipseMaj * (xyz1[1] + t1 * dxyz[1]),
                           ellipseMin * (xyz1[2] + t1 * dxyz[2])};
         UtQuaternion windQuat(time * -mAngularSpeed, mWindAxis.GetData());
         double       rxyz[3];
         windQuat.Rotate(ixyz, rxyz);
         float x = (float)(fmod(rxyz[0], mTileScalar) / mTileScalar);
         if (x < 0.0)
         {
            x += 1.0;
         }
         float y = (float)(fmod(rxyz[1], mTileScalar) / mTileScalar);
         if (y < 0.0)
         {
            y += 1.0;
         }
         float z = (float)(fmod(rxyz[2], mTileScalar) / mTileScalar);
         if (z < 0.0)
         {
            z += 1.0;
         }

         if (mNoiseFunctionPtr->QueryValue(x, y, z) > mThreshold)
         {
            penetration += mThickness;
         }
      }
      if ((t2 >= 0) && (t2 <= 1))
      {
         // isect = true;
         double       ixyz[3] = {ellipseMaj * (xyz1[0] + t2 * dxyz[0]),
                           ellipseMaj * (xyz1[1] + t2 * dxyz[1]),
                           ellipseMin * (xyz1[2] + t2 * dxyz[2])};
         UtQuaternion windQuat(time * -mAngularSpeed, mWindAxis.GetData());
         double       rxyz[3];
         windQuat.Rotate(ixyz, rxyz);

         double x = fmod(rxyz[0], mTileScalar) / mTileScalar;
         if (x < 0.0)
         {
            x += 1.0;
         }
         double y = fmod(rxyz[1], mTileScalar) / mTileScalar;
         if (y < 0.0)
         {
            y += 1.0;
         }
         double z = fmod(rxyz[2], mTileScalar) / mTileScalar;
         if (z < 0.0)
         {
            z += 1.0;
         }
         if (mNoiseFunctionPtr->QueryValue((float)x, (float)y, (float)z) > (float)mThreshold)
         {
            penetration += mThickness;
         }
      }
      return penetration;
   }
}

void WsfNoiseCloud::Initialize(WsfSimulation& aSimulation)
{
   assert(mNoiseFunctionPtr == nullptr);

   if (mUseGlobalWind)
   {
      double angle  = aSimulation.GetScenario().GetEnvironment().GetWindDirection() * UtMath::cDEG_PER_RAD;
      double speed  = aSimulation.GetScenario().GetEnvironment().GetWindSpeed();
      mAngularSpeed = speed / UtSphericalEarth::cEARTH_RADIUS; // an approximation of the angular speed
      // the actual speed will vary with latitude and direction
      double elat;
      double elon;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(mLat,
                                                       mLon,
                                                       angle,
                                                       UtSphericalEarth::cEARTH_RADIUS * UtMath::cPI_OVER_2,
                                                       elat,
                                                       elon);
      double axis[3];
      UtSphericalEarth::ConvertLLAToECEF(elat, elon, 0.0, axis);
      double normer = 1 / sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
      mWindAxis[0]  = axis[0] * normer;
      mWindAxis[1]  = axis[1] * normer;
      mWindAxis[2]  = axis[2] * normer;
   }

   mRandom.SetSeed(aSimulation.GetRandom().GetSeed());
   mNoiseFunctionPtr = new UtNoise(mRandom, mFrequency, mOctaves);
}
