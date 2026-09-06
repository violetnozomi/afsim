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

#ifndef WSFOPTICALREFLECTIVITY_HPP
#define WSFOPTICALREFLECTIVITY_HPP

#include "wsf_mil_export.h"

class UtScriptTypes;
class WsfPlatform;
class WsfScenario;
#include "WsfSignature.hpp"

//! An object that represents the reflectivity of a platform.
class WSF_MIL_EXPORT WsfOpticalReflectivity : public WsfSignature
{
public:
   enum
   {
      cSIGNATURE_INDEX = 3 //!< Index of signature within WsfSignatureList (must be unique)
   };

   WsfOpticalReflectivity();
   WsfOpticalReflectivity* Clone() const override = 0;

   //! Get the optical reflectivity for a given set of conditions.
   //!
   //! @param aSimTime      [input] The simulation time.
   //! @param aStateId      [input] The string ID representing the signature state to be used.
   //! @param aWavelength   [input] The wavelength of the signal (meters).
   //! @param aTgtToXmtrAz  [input] The azimuth   of the transmitter with respect to the target (radians).
   //! @param aTgtToXmtrEl  [input] The elevation of the transmitter with respect to the target (radians).
   //! @param aTgtToRcvrAz  [input] The azimuth   of the receiver    with respect to the target (radians).
   //! @param aTgtToRcvrEl  [input] The elevation of the receiver    with respect to the target (radians).
   //! @returns The optical reflectivity (1/steradian)
   virtual float GetOpticalReflectivity(double      aSimTime,
                                        WsfStringId aStateId,
                                        double      aWavelength,
                                        double      aTgtToXmtrAz,
                                        double      aTgtToXmtrEl,
                                        double      aTgtToRcvrAz,
                                        double      aTgtToRcvrEl) = 0;

   //! @name Methods to support the actual interface on the platform.
   //! These methods provide the interface from the sensor model to the signature.
   //!
   //! The user makes calls like WsfOpticalReflectivity::GetValue(...).
   //@{
   static WsfStringId GetInputType(WsfPlatform* aPlatformPtr);

   static WsfStringId GetState(WsfPlatform* aPlatformPtr);

   static bool SetState(WsfPlatform* aPlatformPtr, WsfStringId aState);

   static float GetScaleFactor(WsfPlatform* aPlatformPtr);

   static bool SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor);

   static float GetValue(double       aSimTime,
                         WsfPlatform* aPlatformPtr,
                         double       aWavelength,
                         double       aTgtToXmtrAz,
                         double       aTgtToXmtrEl,
                         double       aTgtToRcvrAz,
                         double       aTgtToRcvrEl);

   static WsfOpticalReflectivity* GetObject(WsfPlatform* aPlatformPtr);
   //@}

   static void RegisterScriptMethods(UtScriptTypes& aScriptTypes);
   static void RegisterInterface(WsfScenario& aScenario);
};

#endif
