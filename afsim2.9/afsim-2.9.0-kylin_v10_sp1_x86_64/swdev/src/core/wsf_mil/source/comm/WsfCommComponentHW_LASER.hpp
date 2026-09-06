// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMCOMPONENTHWLASER_HPP
#define WSFCOMMCOMPONENTHWLASER_HPP

#include "wsf_mil_export.h"

#include "WsfAtmosphericTurbulence.hpp"
#include "WsfCommComponentHW.hpp"
class WsfCommResult;
#include "WsfMilComponentRoles.hpp"
class WsfScenario;

namespace wsf
{
namespace comm
{

//! Perform propagation effects and signal-to-noise calculations between lasercomm transmitters and receivers.
//! Attenuation and turbulence effects are handled either by simple loss / transmission factor terms,
//! or though built-in models (WsfOpticalAttenuation and wsf::AtmosphericTurbulence).  A simple loss /
//! transmission factor for aero-optic turbulence is also available.  Environmental
//! factors for background radiance and irradiance are stored here and fed to the laser
//! receiver's WsfLASER_RcvrComponent::ComptuteDetectionData method, while signal-to-noise
//! is calculated based on the signal and noise in the
//! WsfLASER_RcvrComponent::DetectionData return value of that method.
//! Optionally, the result of these calculations is displayed in a link budget.
class WSF_MIL_EXPORT ComponentHW_LASER : public ComponentHW
{
public:
   static ComponentHW_LASER* Find(const Comm& aParent);
   static ComponentHW_LASER* FindOrCreate(Comm& aParent);

   explicit ComponentHW_LASER(const WsfScenario& aScenario);
   ComponentHW_LASER(const ComponentHW_LASER& aSrc) = default;
   ~ComponentHW_LASER() override                    = default;

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Interface from WsfCommComponent.
   //@{
   bool ProcessInput(UtInput& aInput) override;
   //@}

   bool CanSendTo(double aSimTime, Comm* aRcvrPtr, Result& aResult) override;

private:
   AtmosphericTurbulence mTurbulence;
   double                mTurbulenceTransmissionFactor;
   double                mAttenuationTransmissionFactor;
   double                mAeroOpticTransmissionFactor;
   double                mBackgroundSpectralRadiance;
   double                mBackgroundSpectralIrradiance;
   bool                  mShowLinkBudget;
};

} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::ComponentHW_LASER, cWSF_COMPONENT_COMM_HW_LASER)

#endif
