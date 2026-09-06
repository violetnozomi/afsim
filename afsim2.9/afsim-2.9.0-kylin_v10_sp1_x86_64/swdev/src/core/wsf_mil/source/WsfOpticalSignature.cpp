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

#include "WsfOpticalSignature.hpp"

#include <cmath>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"
#include "WsfEM_Util.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfInfraredSignature.hpp"
#include "WsfOpticalSignatureTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSignatureInterface.hpp"
#include "WsfSignatureList.hpp"
#include "script/WsfScriptContext.hpp"

// =================================================================================================
WsfOpticalSignature::WsfOpticalSignature()
   : WsfSignature()
{
}

// =================================================================================================
// Definition of the default signature to be used if a signature is not defined on a platform
// and a sensor is present that requires the signature.
// =================================================================================================

namespace
{
class DefaultOpticalSignature : public WsfOpticalSignature
{
public:
   DefaultOpticalSignature()
      : WsfOpticalSignature()
   {
   }
   WsfOpticalSignature* Clone() const override { return new DefaultOpticalSignature(); }
   float                GetSignature(double aSimTime, WsfStringId aStateId, double aAzimuth, double aElevation) override
   {
      return 1000.0F;
   } // m^2
};
} // namespace

// =================================================================================================
// Definition of the signature-specific interface class
// =================================================================================================

namespace
{
std::string sOpticalSignatureClassName("optical_signature");
std::string sOpticalSignatureShortName("optical");

class OpticalSignatureInterface : public WsfSignatureInterface
{
public:
   OpticalSignatureInterface()
      : WsfSignatureInterface()
   {
   }
   OpticalSignatureInterface(const OpticalSignatureInterface& aSrc) = default;
   std::string            GetClassName() const override { return sOpticalSignatureClassName; }
   std::string            GetShortName() const override { return sOpticalSignatureShortName; }
   WsfSignatureInterface* Clone() const override { return new OpticalSignatureInterface(*this); }
   WsfSignature*          CloneSignature(const WsfScenario& aScenario) const override
   {
      return WsfOpticalSignatureTypes::Get(aScenario).Clone(mInputType);
   }
};
} // namespace

// =================================================================================================
bool WsfOpticalSignature::GetProjectedArea(double      aSimTime,
                                           WsfStringId aStateId,
                                           double      aAzimuth,
                                           double      aElevation,
                                           double&     aProjectedArea)
{
   return false;
}

// =================================================================================================
bool WsfOpticalSignature::GetRadiantIntensity(double          aSimTime,
                                              WsfStringId     aState,
                                              const BandList& aBands,
                                              double          aAzimuth,
                                              double          aElevation,
                                              double&         aRadiantIntensity)
{
   return false;
}

// =================================================================================================
// Static methods to support access to the signature on a platform.
// =================================================================================================

// =================================================================================================
//! Get the input type of the signature.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The current signature state.
WsfStringId WsfOpticalSignature::GetInputType(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetInputType(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Get the current signature state.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The current signature state.
WsfStringId WsfOpticalSignature::GetState(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetState(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signatures state
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aState       [input] The new signatures state.
//! @returns true if successful, false if and error occurred.
bool WsfOpticalSignature::SetState(WsfPlatform* aPlatformPtr, WsfStringId aState)
{
   return aPlatformPtr->GetSignatureList().SetState(cSIGNATURE_INDEX, aState);
}

// =================================================================================================
//! Get the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @returns The signature scale factor.
float WsfOpticalSignature::GetScaleFactor(WsfPlatform* aPlatformPtr)
{
   return aPlatformPtr->GetSignatureList().GetScaleFactor(cSIGNATURE_INDEX);
}

// =================================================================================================
//! Set the signature scale factor.
//! @param aPlatformPtr [input] The pointer to the platform containing the signature.
//! @param aScaleFactor [input] The new signature scale factor.
//! @returns true if successful, false if and error occurred.
bool WsfOpticalSignature::SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor)
{
   return aPlatformPtr->GetSignatureList().SetScaleFactor(cSIGNATURE_INDEX, aScaleFactor);
}

// =================================================================================================
//! Get the signature value for a set of conditions.
//! @param aSimTime     [input] The current simulation time.
//! @param aPlatformPtr [input] The platform from which the signature is to be retrieved.
//! @param aAzimuth     [input] azimuth of the observer with respect to this platform (radians).
//! @param aElevation   [input] elevation of the observer with respect to this platform (radians).
//! @returns The optical signature.
// static
float WsfOpticalSignature::GetValue(double aSimTime, WsfPlatform* aPlatformPtr, double aAzimuth, double aElevation)
{
   OpticalSignatureInterface* interfacePtr =
      static_cast<OpticalSignatureInterface*>(aPlatformPtr->GetSignatureList().GetInterface(cSIGNATURE_INDEX));
   WsfOpticalSignature* signaturePtr = static_cast<WsfOpticalSignature*>(interfacePtr->GetSignature());

   double length = aPlatformPtr->GetLength();
   double width  = aPlatformPtr->GetWidth();
   double height = aPlatformPtr->GetHeight();
   float  sig    = 0.0F;
   if ((signaturePtr == nullptr) && (length > 0.0) && (width > 0.0) && (height > 0.0))
   {
      // An optical signature was not specified, but the length, width and height were.
      // Use a simple orthographic projection of each side to determine the area.

      double sinAz = sin(aAzimuth);
      double cosAz = cos(aAzimuth);
      double sinEl = sin(aElevation);
      double cosEl = cos(aElevation);

      // The projected area of a face (P) is just the area of the face (A) times the cosine of the angle
      // between the view vector (V) and the vector normal to the face (N). Using the dot product:
      //
      //    P = A * dot(V,N) / (mag(V) * mag(N))
      //      = A * dot(V,N)    (assuming V and N are unit vectors).
      //
      // The view unit vector (V) is: { cosAz * cosEl, sinAz * cosEl, - sinEl }
      //
      // The front/back face unit normal vector (Nx) is { +/- 1, 0, 0 }
      // The left/right face unit normal vector (Ny) is { 0, +/- 1, 0 }
      // The top/bottom face unit normal vector (Nz) is { 0, 0, +/- 1 }
      //
      // fabs() is used because we don't care which of a face-pair is being seen (they are the same size).

      double fbSide = width * height * fabs(cosAz * cosEl);
      double lrSide = length * height * fabs(sinAz * cosEl);
      double tbSide = width * length * fabs(sinEl);

      sig = static_cast<float>(fbSide + lrSide + tbSide);
   }
   else
   {
      if (signaturePtr == nullptr)
      {
         // Signature not defined... use the default signature.
         { // RAII block
            auto out = ut::log::warning() << "Optical signature not defined. Using default.";
            out.AddNote() << "Platform: " << aPlatformPtr->GetName();
            out.AddNote() << "Type: " << aPlatformPtr->GetType();
         }
         signaturePtr = new DefaultOpticalSignature();
         interfacePtr->SetSignature(signaturePtr);
      }
      sig = signaturePtr->GetSignature(aSimTime, interfacePtr->GetState(), aAzimuth, aElevation);
   }
   return sig * interfacePtr->GetScaleFactor();
}

// =================================================================================================
//! Get the projected area of the platform from a specified aspect angle.
//! @param aSimTime       [input] The current simulation time.
//! @param aPlatformPtr   [input] The platform from which the projected area is to be retrieved.
//! @param aAzimuth       [input] azimuth of the observer with respect to this platform (radians).
//! @param aElevation     [input] elevation of the observer with respect to this platform (radians).
//! @param aProjectedArea [output] The projected area of platform (m^2).
//! @returns true if successful, false if not.
// static
bool WsfOpticalSignature::GetProjectedArea(double       aSimTime,
                                           WsfPlatform* aPlatformPtr,
                                           double       aAzimuth,
                                           double       aElevation,
                                           double&      aProjectedArea)
{
   OpticalSignatureInterface* interfacePtr =
      static_cast<OpticalSignatureInterface*>(aPlatformPtr->GetSignatureList().GetInterface(cSIGNATURE_INDEX));
   WsfOpticalSignature* signaturePtr = static_cast<WsfOpticalSignature*>(interfacePtr->GetSignature());
   if ((signaturePtr != nullptr) &&
       signaturePtr->GetProjectedArea(aSimTime, interfacePtr->GetState(), aAzimuth, aElevation, aProjectedArea))
   {
      aProjectedArea *= interfacePtr->GetScaleFactor();
   }
   else
   {
      aProjectedArea = GetValue(aSimTime, aPlatformPtr, aAzimuth, aElevation);
   }
   return true;
}

// =================================================================================================
//! Get the radiant intensity of the platform within a specified band from a specified aspect angle.
//! @param aSimTime          [input] The current simulation time.
//! @param aPlatformPtr      [input] The platform from which the radiant intensity is to be retrieved.
//! @param aBands            [input] The list of bands that represent the region of interest within the the optical spectrum.
//! @param aAzimuth          [input] azimuth of the observer with respect to this platform (radians).
//! @param aElevation        [input] elevation of the observer with respect to this platform (radians).
//! @param aRadiantIntensity [output] The radiant intensity of platform (W/sr).
//! @returns true if successful, false if not.
// static
bool WsfOpticalSignature::GetRadiantIntensity(double          aSimTime,
                                              WsfPlatform*    aPlatformPtr,
                                              const BandList& aBands,
                                              double          aAzimuth,
                                              double          aElevation,
                                              double&         aRadiantIntensity)
{
   bool                       ok = false;
   OpticalSignatureInterface* interfacePtr =
      static_cast<OpticalSignatureInterface*>(aPlatformPtr->GetSignatureList().GetInterface(cSIGNATURE_INDEX));
   WsfOpticalSignature* signaturePtr = static_cast<WsfOpticalSignature*>(interfacePtr->GetSignature());
   if ((signaturePtr != nullptr) &&
       signaturePtr->GetRadiantIntensity(aSimTime, interfacePtr->GetState(), aBands, aAzimuth, aElevation, aRadiantIntensity))
   {
      aRadiantIntensity *= interfacePtr->GetScaleFactor();
      ok = true;
   }
   return ok;
}

// =================================================================================================
//! Returns whether the signature of the platform uses inherent_contrast, or ignores it.
//! @param aPlatformPtr   [input] The platform from which the signature is pulled.
//! @returns true if the signature uses inherent_contrast
bool WsfOpticalSignature::UsesInherentContrast(WsfPlatform* aPlatformPtr)
{
   auto interfacePtr = aPlatformPtr->GetSignatureList().GetInterface(cSIGNATURE_INDEX);
   auto signaturePtr = static_cast<WsfOpticalSignature*>(interfacePtr->GetSignature());
   if (signaturePtr != nullptr)
   {
      return signaturePtr->UsesInherentContrast();
   }
   return true;
}

// =================================================================================================
// Script Interface
// =================================================================================================

namespace
{

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, OpticalSigState, 0, "string", "")
{
   aReturnVal.SetString(WsfOpticalSignature::GetState(aObjectPtr));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, SetOpticalSigState, 1, "void", "string")
{
   WsfStringId stateId = WsfStringId(aVarArgs[0].GetString());
   WsfOpticalSignature::SetState(aObjectPtr, stateId);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, OpticalSigScaleFactor, 0, "double", "")
{
   aReturnVal.SetDouble(WsfOpticalSignature::GetScaleFactor(aObjectPtr));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, SetOpticalSigScaleFactor, 1, "void", "double")
{
   float scaleFactor = static_cast<float>(aVarArgs[0].GetDouble());
   WsfOpticalSignature::SetScaleFactor(aObjectPtr, scaleFactor);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, OpticalCrossSection, 2, "double", "WsfGeoPoint, double")
{
   WsfGeoPoint* viewPointPtr     = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       earthRadiusScale = aVarArgs[1].GetDouble();
   earthRadiusScale              = (earthRadiusScale > 0.0) ? earthRadiusScale : 1.0; // Use default if requested

   // Compute the azimuth and elevation of the viewpoint with respect to the target (this platform).
   double viewerLocWCS[3];
   viewPointPtr->GetLocationWCS(viewerLocWCS);
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->Update(simTime);
   double targetToViewerAz;
   double targetToViewerEl;
   WsfEM_Util::ComputeViewerAspect(aObjectPtr, viewerLocWCS, earthRadiusScale, targetToViewerAz, targetToViewerEl);

   double value = WsfOpticalSignature::GetValue(simTime, aObjectPtr, targetToViewerAz, targetToViewerEl);
   aReturnVal.SetDouble(value);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, ProjectedArea, 2, "double", "WsfGeoPoint, double")
{
   WsfGeoPoint* viewPointPtr     = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       earthRadiusScale = aVarArgs[1].GetDouble();
   earthRadiusScale              = (earthRadiusScale > 0.0) ? earthRadiusScale : 1.0; // Use default if requested

   // Compute the azimuth and elevation of the viewpoint with respect to the target (this platform).
   double viewerLocWCS[3];
   viewPointPtr->GetLocationWCS(viewerLocWCS);
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->Update(simTime);
   double targetToViewerAz;
   double targetToViewerEl;
   WsfEM_Util::ComputeViewerAspect(aObjectPtr, viewerLocWCS, earthRadiusScale, targetToViewerAz, targetToViewerEl);

   double projectedArea = 0.0;
   WsfOpticalSignature::GetProjectedArea(simTime, aObjectPtr, targetToViewerAz, targetToViewerEl, projectedArea);
   aReturnVal.SetDouble(projectedArea);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, RadiantIntensity, 3, "double", "WsfGeoPoint, double, string")
{
   WsfGeoPoint* viewPointPtr     = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       earthRadiusScale = aVarArgs[1].GetDouble();
   earthRadiusScale              = (earthRadiusScale > 0.0) ? earthRadiusScale : 1.0; // Use default if requested
   WsfEM_Types::OpticalBand optBand;
   if (!WsfEM_Util::StringToEnum(optBand, aVarArgs[2].GetString()))
   {
      optBand = WsfEM_Types::cOPT_VISUAL;
   }

   // Compute the azimuth and elevation of the viewpoint with respect to the target (this platform).
   double viewerLocWCS[3];
   viewPointPtr->GetLocationWCS(viewerLocWCS);
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->Update(simTime);
   double targetToViewerAz;
   double targetToViewerEl;
   WsfEM_Util::ComputeViewerAspect(aObjectPtr, viewerLocWCS, earthRadiusScale, targetToViewerAz, targetToViewerEl);

   // First attempt to get the radiant intensity from WsfOpticalSignature. If that doesn't work and the band in one of
   // the IR bands then get the signature from WsfInfraredSignature.
   double                        radiantIntensity = 0.0;
   WsfOpticalSignature::BandList bandList;
   bandList.push_back(optBand);
   if (!WsfOpticalSignature::GetRadiantIntensity(simTime, aObjectPtr, bandList, targetToViewerAz, targetToViewerEl, radiantIntensity))
   {
      if (optBand < WsfEM_Types::cOPT_IR_COUNT)
      {
         WsfEM_Types::InfraredBand irBand = static_cast<WsfEM_Types::InfraredBand>(optBand);
         radiantIntensity =
            WsfInfraredSignature::GetValue(simTime, aObjectPtr, irBand, targetToViewerAz, targetToViewerEl);
      }
   }
   aReturnVal.SetDouble(radiantIntensity);
}

} // End of anonymous namespace

// =================================================================================================
//! Register the script methods associated with this class.
//! @note The WsfPlatform script class must be defined before calling this method.
// static
void WsfOpticalSignature::RegisterScriptMethods(UtScriptTypes& aScriptTypes)
{
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<OpticalSigState>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<SetOpticalSigState>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<OpticalSigScaleFactor>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<SetOpticalSigScaleFactor>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<OpticalCrossSection>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<ProjectedArea>());
   aScriptTypes.AddClassMethod("WsfPlatform", ut::make_unique<RadiantIntensity>());
}

// =================================================================================================
//! Register the platform interface object on the prototype signature list in the specified scenario.
// static
void WsfOpticalSignature::RegisterInterface(WsfScenario& aScenario)
{
   aScenario.GetSignatureListPrototype().AddInterface(new OpticalSignatureInterface(), cSIGNATURE_INDEX);
}
