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

#ifndef WSFINHERENTCONTRAST_HPP
#define WSFINHERENTCONTRAST_HPP

#include "wsf_mil_export.h"

class UtScriptTypes;
class WsfPlatform;
class WsfScenario;
#include "WsfSignature.hpp"

//! An object that represents the inherent contrast of a platform.
class WSF_MIL_EXPORT WsfInherentContrast : public WsfSignature
{
public:
   enum
   {
      cSIGNATURE_INDEX = 4 //!< Index of signature within WsfSignatureList (must be unique)
   };

   WsfInherentContrast();
   WsfInherentContrast* Clone() const override = 0;

   //! Get the inherent contrast.
   //! @param aStateId   [input] The string ID representing the signature state to be used.
   //! @param aAzimuth   [input] azimuth of the observer with respect to this platform (radians).
   //! @param aElevation [input] elevation of the observer with respect to this platform (radians).
   //! @returns The  inherent contrast (non-dimensional)
   virtual float GetInherentContrast(WsfStringId aStateId, double aAzimuth, double aElevation) = 0;

   //! @name Methods to support the actual interface on the platform.
   //! These methods provide the interface from the sensor model to the signature.
   //!
   //! The user makes calls like WsfInherentContrast::GetValue(...).
   //@{
   static WsfStringId GetInputType(WsfPlatform* aPlatformPtr);

   static WsfStringId GetState(WsfPlatform* aPlatformPtr);

   static bool SetState(WsfPlatform* aPlatformPtr, WsfStringId aState);

   static float GetScaleFactor(WsfPlatform* aPlatformPtr);

   static bool SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor);

   static float GetValue(WsfPlatform* aPlatformPtr, double aAzimuth, double aElevation, bool aLogWarning = true);

   static WsfInherentContrast* GetObject(WsfPlatform* aPlatformPtr);
   //@}

   static void RegisterScriptMethods(UtScriptTypes& aScriptTypes);
   static void RegisterInterface(WsfScenario& aScenario);
};

#endif
