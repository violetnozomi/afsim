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

#ifndef WSFASSETPERCEPTION_HPP
#define WSFASSETPERCEPTION_HPP

#include "wsf_mil_export.h"

class WsfLocalTrack;
#include "WsfObject.hpp"
class WsfPlatform;
#include "script/WsfScriptObjectClass.hpp"
class WsfSensor;
#include "WsfTaskResource.hpp"
class WsfWeapon;

//! The WsfAssetPerception class is used for perception of a platform.
//! It can almost be thought of like a "track" for friendly platforms.
//! It is different from a track in that friendlies will send you an asset perception of themselves.
//! The only error in the perception could be its time offset (truth can change since last received message).
//! The only omission in the perception could be intentionally created by the sender (lying to allies).
//! Asset perceptions are passed around through WsfAssetMessages, by the WsfPerceptionProcessor.
class WSF_MIL_EXPORT WsfAssetPerception : public WsfObject
{
public:
   class AssetSystem
   {
   public:
      AssetSystem();
      AssetSystem(WsfWeapon& aWeapon);
      AssetSystem(WsfSensor& aSensor);
      AssetSystem(AssetSystem& aSrc, unsigned int aReady, double aQuantity);

      // cTASK_RESOURCE_TYPE_SENSOR, cTASK_RESOURCE_TYPE_WEAPON, cTASK_RESOURCE_TYPE_JAMMER,
      // cTASK_RESOURCE_TYPE_PROCESSOR, cTASK_RESOURCE_TYPE_UPLINK, cTASK_RESOURCE_TYPE_NONE
      unsigned int mSystemKind{cTASK_RESOURCE_TYPE_NONE};
      WsfStringId  mSystemType{"<type>"}; // WSF type definition (like "RADAR_MISSILE")
      WsfStringId  mSystemName{"<name>"}; // WSF object name
      unsigned int mReadyAssignment{0U};  // similar to "channels" or "available request count", represents number of
                                          // tasks this system can still receive
      double mQuantityRemaining{0.0};
   };

   WsfAssetPerception();
   WsfAssetPerception(const WsfAssetPerception& aSrc);
   WsfAssetPerception& operator=(const WsfAssetPerception&) = delete;
   WsfAssetPerception(const WsfAssetPerception& aSrc, const AssetSystem& aSys);
   WsfAssetPerception(double aSimTime, WsfPlatform& aPlatform);
   WsfAssetPerception* Clone() const override { return new WsfAssetPerception(*this); }

   const char* GetScriptClassName() const override { return "WsfAssetPerception"; }
   bool        operator<(const WsfAssetPerception& aRhs) const { return (mIndex < aRhs.mIndex); }
   bool        operator==(const WsfAssetPerception& aRhs) const;
               operator int() { return (int)mIndex; }
               operator const int() const { return (int)mIndex; }
               operator size_t() { return mIndex; }
               operator const size_t() const { return mIndex; }

   size_t mIndex;             // unique simulation platform index
   double mTime;              // seconds
   double mLocationWCS[3];    // Meters
   double mVelocityWCS[3];    // Meters / Second
   double mOrientationNED[3]; // Radians   (heading, pitch, roll)

   std::vector<AssetSystem> mSystems;
};


class WSF_MIL_EXPORT WsfSortedPerception
{
public:
   WsfSortedPerception(WsfPlatform* aPlatform, WsfPlatform* aReference);
   WsfSortedPerception(WsfLocalTrack* aTrack, WsfPlatform* aReference);
   WsfSortedPerception(WsfAssetPerception* aAsset, WsfPlatform* aReference);

   WsfSortedPerception(WsfPlatform* aPlatform, double distance);
   WsfSortedPerception(WsfLocalTrack* aTrack, double distance);
   WsfSortedPerception(WsfAssetPerception* aAsset, double distance);

   inline bool operator<(const WsfSortedPerception& aRhs) const { return (mDistance < aRhs.mDistance); }

   double              mDistance;
   WsfPlatform*        mPlatformPtr;
   WsfLocalTrack*      mTrackPtr;
   WsfAssetPerception* mAssetPtr;
};


//! WsfScriptAssetPerceptionClass is a WsfScriptObjectClass that defines the methods in
//! WsfAssetPerception.  This makes the methods available for use in script.
class WSF_MIL_EXPORT WsfScriptAssetPerceptionClass : public WsfScriptObjectClass
{
public:
   WsfScriptAssetPerceptionClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptAssetPerceptionClass() override = default;

   void*       Create(const UtScriptContext& aInstance) override;
   void*       Clone(void* aObjectPtr) override;
   void        Destroy(void* aObjectPtr) override;
   std::string ToString(void* aObjectPtr) const override;
   bool        LessThan(void* aLHS, void* aRHS) override;
   void        OnNewScriptRef(UtScriptRef& aReference) override;

   UT_DECLARE_SCRIPT_METHOD(Index); // truth data - platform simulation index
   UT_DECLARE_SCRIPT_METHOD(Time);

   UT_DECLARE_SCRIPT_METHOD(Location);       // WsfGeoPoint (can get LLA or WCS from geo point)
   UT_DECLARE_SCRIPT_METHOD(VelocityWCS);    // meters/second
   UT_DECLARE_SCRIPT_METHOD(VelocityNED);    // meters/second
   UT_DECLARE_SCRIPT_METHOD(Speed);          // meters/second
   UT_DECLARE_SCRIPT_METHOD(OrientationNED); // degrees
   UT_DECLARE_SCRIPT_METHOD(OrientationWCS); // degrees
   UT_DECLARE_SCRIPT_METHOD(Heading);        // degrees
   UT_DECLARE_SCRIPT_METHOD(Pitch);          // degrees
   UT_DECLARE_SCRIPT_METHOD(Roll);           // degrees

   // UT_DECLARE_SCRIPT_METHOD(Fuel);
   // UT_DECLARE_SCRIPT_METHOD(Sensor);
   // UT_DECLARE_SCRIPT_METHOD(SensorCount);
   // UT_DECLARE_SCRIPT_METHOD(SensorEntry);
   // UT_DECLARE_SCRIPT_METHOD(Weapon);
   // UT_DECLARE_SCRIPT_METHOD(WeaponCount);
   // UT_DECLARE_SCRIPT_METHOD(WeaponEntry);
   UT_DECLARE_SCRIPT_METHOD(SystemCount);
   UT_DECLARE_SCRIPT_METHOD(SystemKind);              // system at index
   UT_DECLARE_SCRIPT_METHOD(SystemType);              // system at index
   UT_DECLARE_SCRIPT_METHOD(SystemName);              // system at index
   UT_DECLARE_SCRIPT_METHOD(SystemReadyAssignment);   // system at index
   UT_DECLARE_SCRIPT_METHOD(SystemQuantityRemaining); // system at index

   UT_DECLARE_SCRIPT_METHOD(InterceptLocation);
   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo);     // degrees
   UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo); // degrees
   UT_DECLARE_SCRIPT_METHOD(ClosestApproachOf);
   UT_DECLARE_SCRIPT_METHOD(ClosingSpeedOf);
};

#endif
