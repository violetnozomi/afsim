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

#ifndef WSFWEAPONTASKRESOURCE_HPP
#define WSFWEAPONTASKRESOURCE_HPP

#include "wsf_mil_export.h"

#include "WsfStringId.hpp"
#include "WsfTaskResource.hpp"

enum // reserve 3-5 for wsf_mil
{
   cTASK_RESOURCE_TYPE_WEAPON = 3, //!< resource is a weapon
   cTASK_RESOURCE_TYPE_JAMMER = 4, //!< resource is a jammer
   cTASK_RESOURCE_TYPE_UPLINK = 5  //!< resource is an uplink
};

//! Attributes of a task-able weapon resource.
class WSF_MIL_EXPORT WsfWeaponTaskResource : public WsfTaskResource
{
public:
   WsfWeaponTaskResource()
      : WsfTaskResource(cTASK_RESOURCE_TYPE_WEAPON, true)
      , mCount(1)
   {
   }
   WsfWeaponTaskResource(const WsfWeaponTaskResource& aSrc) = default;
   ~WsfWeaponTaskResource() override                        = default;
   WsfTaskResource* Clone() const override;

   WsfStringId GetTypeName() const override;

   //! For XIO (de)serialization.
   template<typename AR>
   void Serialize(AR& aAr)
   {
      WsfTaskResource::Serialize(aAr);
      aAr& mCount;
   }
   //! The number of shots to be fired.
   int mCount;
};

//! Attributes of a task-able jammer resource.
class WSF_MIL_EXPORT WsfJammerTaskResource : public WsfTaskResource
{
public:
   WsfJammerTaskResource()
      : WsfTaskResource(cTASK_RESOURCE_TYPE_JAMMER, true)
      , mBeamNumber(0)
      , mFrequency(0.0)
      , mBandwidth(0.0)
   {
   }
   WsfJammerTaskResource(const WsfJammerTaskResource& aSrc) = default;
   ~WsfJammerTaskResource() override                        = default;
   WsfTaskResource* Clone() const override;

   WsfStringId GetTypeName() const override;

   bool IsMatch(const WsfTaskResource& aRhs) const override;

   //! For XIO (de)serialization.
   template<typename AR>
   void Serialize(AR& aAr)
   {
      WsfTaskResource::Serialize(aAr);
      aAr& mBeamNumber& mFrequency& mBandwidth& mTechniqueName;
   }

   //! The beam number of interest
   int mBeamNumber;

   //! The frequency to use.
   double mFrequency;

   //! The bandwidth to use.
   double mBandwidth;

   //! The technique to use.
   WsfStringId mTechniqueName;
};

//! Attributes of a task-able uplink resource.
class WSF_MIL_EXPORT WsfUplinkTaskResource : public WsfTaskResource
{
public:
   WsfUplinkTaskResource()
      : WsfTaskResource(cTASK_RESOURCE_TYPE_UPLINK, true)
      , mUplinkDelay(-1.0)
   {
   }
   WsfUplinkTaskResource(const WsfUplinkTaskResource& aSrc) = default;
   ~WsfUplinkTaskResource() override                        = default;
   WsfTaskResource* Clone() const override;

   WsfStringId GetTypeName() const override;

   //! For XIO (de)serialization.
   template<typename AR>
   void Serialize(AR& aAr)
   {
      WsfTaskResource::Serialize(aAr);
      aAr& mUplinkDestination& mUplinkSource& mUplinkCommName& mUplinkDelay;
   }

   //! The platform name which is to receive the uplink.  If null, the uplink destination
   //! is assumed to be a future-launched weapon
   WsfStringId mUplinkDestination;
   //! Source of the uplink.  If it is a sensor name, the sensor's tracks are sent.
   //! If it is null, local track updates are sent
   WsfStringId mUplinkSource;
   //! Comm device used to send the uplink.
   WsfStringId mUplinkCommName;

   double mUplinkDelay;
};

#endif
