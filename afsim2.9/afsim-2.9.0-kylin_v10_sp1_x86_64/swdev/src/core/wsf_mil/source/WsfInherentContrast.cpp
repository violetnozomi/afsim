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

#include "WsfInherentContrast.hpp"

#include "UtLog.hpp"
#include "WsfInherentContrastTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSignatureInterface.hpp"
#include "WsfSignatureList.hpp"

// =================================================================================================
WsfInherentContrast::WsfInherentContrast()
   : WsfSignature()
{
}

// =================================================================================================
// Definition of the default signature to be used if a signature is not defined on a platform
// and a sensor is present that requires the signature.
// =================================================================================================

namespace
{
class DefaultContrastSignature : public WsfInherentContrast
{
public:
   DefaultContrastSignature()
      : WsfInherentContrast()
   {
   }
   WsfInherentContrast* Clone() const override { return new DefaultContrastSignature(); }
   float                GetInherentContrast(WsfStringId aStateId, double aAzimuth, double aElevation) override
   {
      return 0.5F;
   } // non-dimensional
};
} // namespace

// =================================================================================================
// Definition of the signature-specific interface class
// =================================================================================================

namespace
{
std::string sInherentContrastClassName("inherent_contrast");
std::string sInherentContrastShortName("inherent_contrast");

class ContrastInterface : public WsfSignatureInterface
{
public:
   ContrastInterface()
      : WsfSignatureInterface()
   {
   }
   ContrastInterface(const ContrastInterface& aSrc) = default;
   std::string            GetClassName() const override { return sInherentContrastClassName; }
   std::string            GetShortName() const override { return sInherentContrastShortName; }
   WsfSignatureInterface* Clone() const override { return new ContrastInterface(*this); }
   WsfSignature*          CloneSignature(const WsfScenario& aScenario) const override
   {
      return WsfInherentContrastTypes::Get(aScenario).Clone(mInputType);
   }
};
} // namespace

// =================================================================================================
// Static methods to support access to the signature on a platform.
// =================================================================================================

// =================================================================================================
//! Get the input type of the signature.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The current signature state.
WsfStringId WsfInherentContrast::GetInputType(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetInputType(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Get the current signature state.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The current signature state.
WsfStringId WsfInherentContrast::GetState(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetState(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signatures state
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aState       [input] The new signatures state.
//! @returns true if successful, false if and error occurred.
bool WsfInherentContrast::SetState(WsfPlatform* aPlatformPtr, WsfStringId aState)
{
   return aPlatformPtr->GetSignatureList().SetState(cSIGNATURE_INDEX, aState);
}

// =================================================================================================
//! Get the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The signature scale factor.
float WsfInherentContrast::GetScaleFactor(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetScaleFactor(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aScaleFactor [input] The new signature scale factor.
//! @returns true if successful, false if and error occurred.
bool WsfInherentContrast::SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor)
{
   return aPlatformPtr->GetSignatureList().SetScaleFactor(cSIGNATURE_INDEX, aScaleFactor);
}

// =================================================================================================
//! Get the signature value for a set of conditions.
//! @param aPlatformPtr [input] The platform from which the signature is to be retrieved.
//! @param aAzimuth     [input] azimuth of the observer with respect to this platform (radians).
//! @param aElevation   [input] elevation of the observer with respect to this platform (radians).
//! @param aLogWarning  [input] Tells function whether to log a warning if signature is not defined.
//! @returns The inherent contrast.
// static
float WsfInherentContrast::GetValue(WsfPlatform* aPlatformPtr, double aAzimuth, double aElevation, bool aLogWarning)
{
   ContrastInterface* interfacePtr =
      static_cast<ContrastInterface*>(aPlatformPtr->GetSignatureList().GetInterface(cSIGNATURE_INDEX));
   WsfInherentContrast* signaturePtr = static_cast<WsfInherentContrast*>(interfacePtr->GetSignature());
   if (signaturePtr == nullptr)
   {
      // Signature not defined... use the default signature.
      if (aLogWarning)
      {
         auto out = ut::log::warning() << "Platform's Inherent Contrast not defined. Using Default.";
         out.AddNote() << "Platform: " << aPlatformPtr->GetName();
         out.AddNote() << "Inherent Contrast: " << aPlatformPtr->GetType();
      }
      signaturePtr = new DefaultContrastSignature();
      interfacePtr->SetSignature(signaturePtr);
   }
   return signaturePtr->GetInherentContrast(interfacePtr->GetState(), aAzimuth, aElevation) *
          interfacePtr->GetScaleFactor();
}

// =================================================================================================
//! Register the script methods associated with this class.
//! @note The WsfPlatform script class must be defined before calling this method.
// static
void WsfInherentContrast::RegisterScriptMethods(UtScriptTypes& aScriptTypes)
{
   // This is currently placeholder as there are no script methods for this class.
}

// =================================================================================================
//! Register the platform interface object on the prototype signature list in the specified scenario.
// static
void WsfInherentContrast::RegisterInterface(WsfScenario& aScenario)
{
   aScenario.GetSignatureListPrototype().AddInterface(new ContrastInterface(), cSIGNATURE_INDEX);
}
