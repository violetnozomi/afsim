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

#include "WsfAcousticSignature.hpp"

#include "UtLog.hpp"
#include "WsfAcousticSignatureTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSignatureInterface.hpp"
#include "WsfSignatureList.hpp"

// =================================================================================================
std::pair<double, double> WsfAcousticSignature::GetNoisePressureLimits(WsfStringId aStateId) const
{
   return {100.0, 100.0};
}

// =================================================================================================
// Definition of the default signature to be used if a signature is not defined on a platform
// and a sensor is present that requires the signature.
// =================================================================================================

namespace
{
class DefaultAcousticSignature : public WsfAcousticSignature
{
public:
   DefaultAcousticSignature() = default;
   WsfAcousticSignature* Clone() const override { return new DefaultAcousticSignature(); }
   double                GetNoisePressure(WsfStringId aState, double aFrequency) override { return 100.0; } // dB-20uPa
   double GetNoisePressure(WsfStringId aState, double aFrequency, double aTgtToRcvrAz, double aTgtToRcvrEl) override
   {
      return 100.0;
   } // dB-20uPa
};
} // namespace

// =================================================================================================
// Definition of the signature-specific interface class
// =================================================================================================

namespace
{
std::string sAcousticSignatureClassName("acoustic_signature");
std::string sAcousticSignatureShortName("acoustic");

class AcousticInterface : public WsfSignatureInterface
{
public:
   AcousticInterface()
      : WsfSignatureInterface()
   {
   }
   AcousticInterface(const AcousticInterface& aSrc) = default;
   std::string            GetClassName() const override { return sAcousticSignatureClassName; }
   std::string            GetShortName() const override { return sAcousticSignatureShortName; }
   WsfSignatureInterface* Clone() const override { return new AcousticInterface(*this); }
   WsfSignature*          CloneSignature(const WsfScenario& aScenario) const override
   {
      return WsfAcousticSignatureTypes::Get(aScenario).Clone(mInputType);
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
WsfStringId WsfAcousticSignature::GetInputType(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetInputType(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Get the current signature state.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The current signature state.
WsfStringId WsfAcousticSignature::GetState(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetState(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signatures state
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aState       [input] The new signatures state.
//! @returns true if successful, false if and error occurred.
bool WsfAcousticSignature::SetState(WsfPlatform* aPlatformPtr, WsfStringId aState)
{
   return aPlatformPtr->GetSignatureList().SetState(cSIGNATURE_INDEX, aState);
}

// =================================================================================================
//! Get the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The signature scale factor.
float WsfAcousticSignature::GetScaleFactor(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetScaleFactor(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aScaleFactor [input] The new signature scale factor.
//! @returns true if successful, false if and error occurred.
bool WsfAcousticSignature::SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor)
{
   return aPlatformPtr->GetSignatureList().SetScaleFactor(cSIGNATURE_INDEX, aScaleFactor);
}

// =================================================================================================
//! Get the signature value for a set of conditions.
//! @param aPlatformPtr [input] The platform from which the signature is to be retrieved.
//! @param aFrequency   [input] The frequency value of interest for lookup of signature value..
//! @returns The inherent contrast.
// static
double WsfAcousticSignature::GetValue(WsfPlatform* aPlatformPtr, double aFrequency)
{
   AcousticInterface* interfacePtr =
      static_cast<AcousticInterface*>(aPlatformPtr->GetSignatureList().GetInterface(cSIGNATURE_INDEX));
   WsfAcousticSignature* signaturePtr = static_cast<WsfAcousticSignature*>(interfacePtr->GetSignature());
   if (signaturePtr == nullptr)
   {
      // Signature not defined... use the default signature.
      { // RAII block
         auto out = ut::log::warning() << "Acoustic_signature not defined. Using default.";
         out.AddNote() << "Platform: " << aPlatformPtr->GetName();
         out.AddNote() << "Acoustic_Signature: " << aPlatformPtr->GetType();
      }
      signaturePtr = new DefaultAcousticSignature();
      interfacePtr->SetSignature(signaturePtr);
   }
   return signaturePtr->GetNoisePressure(interfacePtr->GetState(), aFrequency) * interfacePtr->GetScaleFactor();
}

double WsfAcousticSignature::GetValue(WsfPlatform* aPlatformPtr, double aFrequency, double aTgtToRcvrAz, double aTgtToRcvrEl)
{
   AcousticInterface* interfacePtr =
      static_cast<AcousticInterface*>(aPlatformPtr->GetSignatureList().GetInterface(cSIGNATURE_INDEX));
   WsfAcousticSignature* signaturePtr = static_cast<WsfAcousticSignature*>(interfacePtr->GetSignature());
   if (signaturePtr == nullptr)
   {
      // Signature not defined... use the default signature.
      { // RAII block
         auto out = ut::log::warning() << "'acoustic_signature' not defined. Using default.";
         out.AddNote() << "Platform: " << aPlatformPtr->GetName();
         out.AddNote() << "Platform Type: " << aPlatformPtr->GetType();
      }

      ut::log::warning() << aPlatformPtr->GetName() << "." << aPlatformPtr->GetType()
                         << ": acoustic_signature not defined; using default.";
      signaturePtr = new DefaultAcousticSignature();
      interfacePtr->SetSignature(signaturePtr);
   }
   return signaturePtr->GetNoisePressure(interfacePtr->GetState(), aFrequency, aTgtToRcvrAz, aTgtToRcvrEl) *
          interfacePtr->GetScaleFactor();
}

// =================================================================================================
//! Register the script methods associated with this class.
//! @note The WsfPlatform script class must be defined before calling this method.
// static
void WsfAcousticSignature::RegisterScriptMethods(UtScriptTypes& aScriptTypes)
{
   // This is currently placeholder as there are no script methods for this class.
}

// =================================================================================================
//! Register the platform interface object on the prototype signature list in the specified scenario.
// static
void WsfAcousticSignature::RegisterInterface(WsfScenario& aScenario)
{
   aScenario.GetSignatureListPrototype().AddInterface(new AcousticInterface(), cSIGNATURE_INDEX);
}
