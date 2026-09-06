// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef P6DofFreezeFlags_HPP
#define P6DofFreezeFlags_HPP

#include "p6dof_export.h"

class P6DOF_EXPORT P6DofFreezeFlags
{
public:
   P6DofFreezeFlags()
      : mMasterFreezeLocation(false)
      , mMasterFreezeAltitude(false)
      , mMasterFreezeVerticalSpeed(false)
      , mMasterFreezeFuelBurn(false)
      , mMasterFreezeSpeed(false)
      , mMasterFreezeYaw(false)
      , mMasterFreezePitch(false)
      , mMasterFreezeRoll(false)
      , mMasterNoLag(false)
      , mMasterNoAlpha(false)
   {
   }

   // This can set/clear the freeze location flag, used by all P6DOF objects.
   // Freeze location freezes objects in Lat/Lon. This is used for testing.
   void SetMasterFreezeLocation(bool aFreeze) { mMasterFreezeLocation = aFreeze; }
   bool GetMasterFreezeLocation() const { return mMasterFreezeLocation; }

   // This can set/clear the freeze altitude flag, used by all P6DOF objects.
   // This is used for testing.
   void SetMasterFreezeAltitude(bool aFreeze) { mMasterFreezeAltitude = aFreeze; }
   bool GetMasterFreezeAltitude() const { return mMasterFreezeAltitude; }

   // This can set/clear the freeze vertical speed flag, used by all P6DOF objects.
   // This is used for testing.
   void SetMasterFreezeVerticalSpeed(bool aFreeze) { mMasterFreezeVerticalSpeed = aFreeze; }
   bool GetMasterFreezeVerticalSpeed() const { return mMasterFreezeVerticalSpeed; }

   // This can set/clear the freeze fuel burn flag, used by all P6DOF objects.
   // When active, no fuel will be consumed. This is used for testing.
   void SetMasterFreezeFuelBurn(bool aFreeze) { mMasterFreezeFuelBurn = aFreeze; }
   bool GetMasterFreezeFuelBurn() const { return mMasterFreezeFuelBurn; }

   // This can set/clear the freeze speed flag, used by all P6DOF objects.
   // When active, the speed will not change. This is used for testing.
   void SetMasterFreezeSpeed(bool aFreeze) { mMasterFreezeSpeed = aFreeze; }
   bool GetMasterFreezeSpeed() const { return mMasterFreezeSpeed; }

   // This can set/clear the freeze speed flag, used by all P6DOF objects.
   // When active, the yaw will not change. This is used for testing.
   void SetMasterFreezeYaw(bool aFreeze) { mMasterFreezeYaw = aFreeze; }
   bool GetMasterFreezeYaw() const { return mMasterFreezeYaw; }

   // This can set/clear the freeze speed flag, used by all P6DOF objects.
   // When active, the pitch will not change. This is used for testing.
   void SetMasterFreezePitch(bool aFreeze) { mMasterFreezePitch = aFreeze; }
   bool GetMasterFreezePitch() const { return mMasterFreezePitch; }

   // This can set/clear the freeze speed flag, used by all P6DOF objects.
   // When active, the roll will not change. This is used for testing.
   void SetMasterFreezeRoll(bool aFreeze) { mMasterFreezeRoll = aFreeze; }
   bool GetMasterFreezeRoll() const { return mMasterFreezeRoll; }

   // This can set/clear the no lag flag, used by all P6DOF objects.
   // When active, no lags will be used in propulsion, flight controls, etc.
   // This is used for testing.
   void SetMasterNoLagTesting(bool aNoLag) { mMasterNoLag = aNoLag; }
   bool GetMasterNoLagTesting() const { return mMasterNoLag; }

   // This can set/clear the no alpha flag, used by all P6DOF objects.
   // When active, alpha will always remain at zero, being artificially
   // modified at the end of the PDofIntegrator::Update().
   // This is used for testing.
   void SetMasterNoAlphaTesting(bool aNoAlpha) { mMasterNoAlpha = aNoAlpha; }
   bool GetMasterNoAlphaTesting() const { return mMasterNoAlpha; }

protected:
   bool mMasterFreezeLocation;
   bool mMasterFreezeAltitude;
   bool mMasterFreezeVerticalSpeed;
   bool mMasterFreezeFuelBurn;
   bool mMasterFreezeSpeed;
   bool mMasterFreezeYaw;
   bool mMasterFreezePitch;
   bool mMasterFreezeRoll;
   bool mMasterNoLag;
   bool mMasterNoAlpha;
};

#endif
