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

#ifndef WSFINFRAREDSIGNATURE_HPP
#define WSFINFRAREDSIGNATURE_HPP

#include "wsf_mil_export.h"

class UtScriptTypes;
#include "WsfEM_Types.hpp"
class WsfPlatform;
class WsfScenario;
#include "WsfSignature.hpp"

//! An object that represents the infrared signature of a platform.
class WSF_MIL_EXPORT WsfInfraredSignature : public WsfSignature
{
public:
   enum
   {
      cSIGNATURE_INDEX = 2 //!< Index of signature within WsfSignatureList (must be unique)
   };

   WsfInfraredSignature();
   WsfInfraredSignature* Clone() const override = 0;

   //! Get the infrared cross section.
   //! @param aSimTime   [input] The current simulation time.
   //! @param aStateId   [input] The string ID representing the signature state to be used.
   //! @param aBand      [input] infrared band.
   //! @param aAzimuth   [input] azimuth of the observer with respect to this platform (radians).
   //! @param aElevation [input] elevation of the observer with respect to this platform (radians).
   //! @returns The infrared cross section (m^2)
   virtual float GetSignature(double                    aSimTime,
                              WsfStringId               aStateId,
                              WsfEM_Types::InfraredBand aBand,
                              double                    aAzimuth,
                              double                    aElevation) = 0;

   //! @name Methods to support the actual interface on the platform.
   //! These methods provide the interface from the sensor model to the signature.
   //!
   //! The user makes calls like WsfInfraredSignature::GetValue(...).
   //@{
   static WsfStringId GetInputType(WsfPlatform* aPlatformPtr);

   static WsfStringId GetState(WsfPlatform* aPlatformPtr);

   static bool SetState(WsfPlatform* aPlatformPtr, WsfStringId aState);

   static float GetScaleFactor(WsfPlatform* aPlatformPtr);

   static bool SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor);

   static float GetValue(double                    aSimTime,
                         WsfPlatform*              aPlatformPtr,
                         WsfEM_Types::InfraredBand aBand,
                         double                    aAzimuth,
                         double                    aElevation);
   //@}

   static void RegisterScriptMethods(UtScriptTypes& aScriptTypes);
   static void RegisterInterface(WsfScenario& aScenario);
};

#endif
