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
#ifndef WSFCHAFFWEAPON_HPP
#define WSFCHAFFWEAPON_HPP

#include "wsf_nx_export.h"

#include <map>

#include "WsfChaffCloudInterface.hpp"
#include "WsfChaffEjector.hpp"
#include "WsfExplicitWeapon.hpp"

//! A specialization of WsfExplicitWeapon that models a weapon which fires chaff parcels
class WSF_NX_EXPORT WsfChaffWeapon : public WsfExplicitWeapon
{
public:
   WsfChaffWeapon(WsfScenario& aScenario);
   ~WsfChaffWeapon() override = default;

   class ChaffOptions : public SalvoOptions
   {
   public:
      using Ejectors = std::vector<std::string>;

      ChaffOptions(int             aNumberParcels, // Number of parcels requested (from each ejector)
                   double          aDropInterval,  // Time separation between parcels in cloud
                   const Ejectors& aEjectors)
         : SalvoOptions(aNumberParcels, aDropInterval)
         , mEjectors(aEjectors)
      {
      }

      Ejectors mEjectors; // Names of ejectors that are active
   };

   virtual bool DropChaffCloud(double aSimTime, ChaffOptions& aOptions);

   //! This method is only used to add a single parcel of chaff to a cloud at appropriate time intervals.
   //! It should only be called internally from WsfWeapon::ContinueSalvo.
   //! Public method DropChaffCloud is preferred to schedule salvos for each ejector by calling WsfWeapon::FireSalvo.
   FireResult Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings) override;

   // Total number of chaff parcels remaining across all ejectors
   double GetQuantityRemaining() const override;

   //! @name Common Framework methods.
   //@{
   WsfWeapon*  Clone() const override;
   bool        Initialize(double aSimTime) override;
   bool        ProcessInput(UtInput& aInput) override;
   const char* GetScriptClassName() const override;
   //@}

protected:
   WsfChaffWeapon(const WsfChaffWeapon& aSrc) = default;
   WsfChaffWeapon& operator=(const WsfChaffWeapon& aSrc) = delete;

   void SetLaunchState(double aSimTime, WsfPlatform* aPlatformPtr, const FireOptions& aOptions) override;

   bool WeaponPlatformUserInitialize(double aSimTime, WsfPlatform* aWeaponPlatformPtr, const WsfTrack* aTrackPtr) override;

   bool AddWeaponToSimulation(double             aSimTime,
                              WsfPlatform*       aWeaponPlatformPtr,
                              const WsfTrack*    aLaunchingTrackPtr,
                              const FireOptions& aSettings) override;

private:
   WsfChaffEjector* GetEjectorByName(WsfStringId aEjectorName) const;

   using CloudMap = std::map<int, std::shared_ptr<WsfChaffCloudInterface>>; // Key is cloud id

   int      mNextCloudId; // The next unique ID for a chaff cloud drop
   CloudMap mCloudMap;
   double   mCloudUpdateInterval;     // Update interval for chaff mover to update parcels
   bool     mDrawCloudApproximations; // Use WsfDraw to draw cloud approximations for debugging
};
#endif
