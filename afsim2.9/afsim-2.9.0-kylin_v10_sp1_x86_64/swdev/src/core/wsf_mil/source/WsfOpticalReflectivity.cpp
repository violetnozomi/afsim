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

#include "WsfOpticalReflectivity.hpp"

#include "UtLog.hpp"
#include "WsfOpticalReflectivityTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSignatureInterface.hpp"
#include "WsfSignatureList.hpp"

// =================================================================================================
WsfOpticalReflectivity::WsfOpticalReflectivity()
   : WsfSignature()
{
}

// =================================================================================================
// Definition of the default signature to be used if a signature is not defined on a platform
// and a sensor is present that requires the signature.
// =================================================================================================

namespace
{
class DefaultOpticalReflectivitySignature : public WsfOpticalReflectivity
{
public:
   DefaultOpticalReflectivitySignature()
      : WsfOpticalReflectivity()
   {
   }
   WsfOpticalReflectivity* Clone() const override { return new DefaultOpticalReflectivitySignature(); }

   float GetOpticalReflectivity(double      aSimTime,
                                WsfStringId aStateId,
                                double      aWavelength,
                                double      aTgtToXmtrAz,
                                double      aTgtToXmtrEl,
                                double      aTgtToRcvrAz,
                                double      aTgtToRcvrEl) override
   {
      return 1.0F;
   } // non-dimensional
};
} // namespace

// =================================================================================================
// Definition of the signature-specific interface class
// =================================================================================================

namespace
{
std::string sOpticalReflectivityClassName("optical_reflectivity");
std::string sOpticalReflectivityShortName("optical_reflectivity");

class OpticalReflectivityInterface : public WsfSignatureInterface
{
public:
   OpticalReflectivityInterface()
      : WsfSignatureInterface()
   {
   }
   OpticalReflectivityInterface(const OpticalReflectivityInterface& aSrc) = default;
   std::string            GetClassName() const override { return sOpticalReflectivityClassName; }
   std::string            GetShortName() const override { return sOpticalReflectivityShortName; }
   WsfSignatureInterface* Clone() const override { return new OpticalReflectivityInterface(*this); }
   WsfSignature*          CloneSignature(const WsfScenario& aScenario) const override
   {
      return WsfOpticalReflectivityTypes::Get(aScenario).Clone(mInputType.GetString());
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
WsfStringId WsfOpticalReflectivity::GetInputType(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetInputType(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Get the current signature state.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The current signature state.
WsfStringId WsfOpticalReflectivity::GetState(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetState(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signatures state
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aState       [input] The new signatures state.
//! @returns true if successful, false if and error occurred.
bool WsfOpticalReflectivity::SetState(WsfPlatform* aPlatformPtr, WsfStringId aState)
{
   return aPlatformPtr->GetSignatureList().SetState(cSIGNATURE_INDEX, aState);
}

// =================================================================================================
//! Get the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The signature scale factor.
float WsfOpticalReflectivity::GetScaleFactor(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetScaleFactor(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aScaleFactor [input] The new signature scale factor.
//! @returns true if successful, false if and error occurred.
bool WsfOpticalReflectivity::SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor)
{
   return aPlatformPtr->GetSignatureList().SetScaleFactor(cSIGNATURE_INDEX, aScaleFactor);
}

// =================================================================================================
//! Get the signature value for a set of conditions.
//! @param aSimTime      [input] The simulation time.
//! @param aPlatformPtr  [input] The platform from which the signature is to be retrieved.
//! @param aWavelength   [input] The wavelength of the signal (meters).
//! @param aTgtToXmtrAz  [input] The azimuth   of the transmitter with respect to the target (radians).
//! @param aTgtToXmtrEl  [input] The elevation of the transmitter with respect to the target (radians).
//! @param aTgtToRcvrAz  [input] The azimuth   of the receiver    with respect to the target (radians).
//! @param aTgtToRcvrEl  [input] The elevation of the receiver    with respect to the target (radians).
//! @returns The optical reflectivity (1/steradian)
// static
float WsfOpticalReflectivity::GetValue(double       aSimTime,
                                       WsfPlatform* aPlatformPtr,
                                       double       aWavelength,
                                       double       aTgtToXmtrAz,
                                       double       aTgtToXmtrEl,
                                       double       aTgtToRcvrAz,
                                       double       aTgtToRcvrEl)
{
   OpticalReflectivityInterface* interfacePtr =
      static_cast<OpticalReflectivityInterface*>(aPlatformPtr->GetSignatureList().GetInterface(cSIGNATURE_INDEX));
   WsfOpticalReflectivity* signaturePtr = static_cast<WsfOpticalReflectivity*>(interfacePtr->GetSignature());
   if (signaturePtr == nullptr)
   {
      // Signature not defined... use the default signature.
      {
         auto out = ut::log::warning() << "Optical reflectivity no defined. Using default.";
         out.AddNote() << "Platform: " << aPlatformPtr->GetName();
         out.AddNote() << "Type: " << aPlatformPtr->GetType();
      }
      signaturePtr = new DefaultOpticalReflectivitySignature();
      interfacePtr->SetSignature(signaturePtr);
   }
   float value = signaturePtr->GetOpticalReflectivity(aSimTime,
                                                      interfacePtr->GetState(),
                                                      aWavelength,
                                                      aTgtToXmtrAz,
                                                      aTgtToXmtrEl,
                                                      aTgtToRcvrAz,
                                                      aTgtToRcvrEl);
   return value * interfacePtr->GetScaleFactor();
}

// =================================================================================================
//! Register the script methods associated with this class.
//! @note The WsfPlatform script class must be defined before calling this method.
// static
void WsfOpticalReflectivity::RegisterScriptMethods(UtScriptTypes& aScriptTypes)
{
   // This is currently placeholder as there are no script methods for this class.
}

// =================================================================================================
//! Register the platform interface object on the prototype signature list in the specified scenario.
// static
void WsfOpticalReflectivity::RegisterInterface(WsfScenario& aScenario)
{
   aScenario.GetSignatureListPrototype().AddInterface(new OpticalReflectivityInterface(), cSIGNATURE_INDEX);
}
