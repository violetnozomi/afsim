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
#ifndef WSFCHAFFEJECTOR_HPP
#define WSFCHAFFEJECTOR_HPP

#include "wsf_nx_export.h"

#include "UtEntityPart.hpp"
#include "WsfChaffParcel.hpp"
#include "WsfNonExportableComponentRoles.hpp"
class WsfScenario;
#include "WsfSinglePlatformObserver.hpp"
#include "WsfWeaponComponent.hpp"

class WSF_NX_EXPORT WsfChaffEjector : public WsfSimpleWeaponComponent, public UtEntityPart, public WsfSinglePlatformObserver
{
public:
   static void RegisterComponentFactory(WsfScenario& aScenario);

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   WsfChaffEjector();
   WsfChaffEjector(const std::string& aName);
   ~WsfChaffEjector() override;
   WsfChaffEjector& operator=(const WsfChaffEjector& aRhs) = delete;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;

   void SetLaunchState(double aSimTime, WsfPlatform* aPlatformPtr);

   std::unique_ptr<WsfChaffParcel> EjectParcel(double aSimTime);

   unsigned int GetQuantityRemaining() const { return mQuantityRemaining; }

protected:
   WsfChaffEjector(const WsfChaffEjector& aSrc);

   //! @name Overrides of WsfSinglePlatformObserver.
   //@{
   //! A callback from the platform subject.
   //! This class attaches itself as an observer to the associated platform and
   //! through this method is notified whenever the platform is updated (moved).
   void OnPlatformUpdated(double /* aSimTime */, WsfPlatform* /* aPlatformPtr */) override { InvalidateTransform(); }
   //@}

private:
   unsigned int mQuantityRemaining;
   WsfStringId  mParcelType;

   double mEjectionVelocity;
   double mEjectionElevation;
   double mEjectionAzimuth;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfChaffEjector, cWSF_COMPONENT_CHAFF_EJECTOR)

#endif
