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

#ifndef WSFACOUSTICSIGNATURE_HPP
#define WSFACOUSTICSIGNATURE_HPP

#include "wsf_mil_export.h"

class UtScriptTypes;
class WsfPlatform;
class WsfScenario;
#include "WsfSignature.hpp"

//! An object that represents the acoustic signature of a platform.
class WSF_MIL_EXPORT WsfAcousticSignature : public WsfSignature
{
public:
   enum
   {
      cSIGNATURE_INDEX = 5 //!< Index of signature within WsfSignatureList (must be unique)
   };
   WsfAcousticSignature()                       = default;
   WsfAcousticSignature* Clone() const override = 0;

   //! Get the acoustic signature for a given signature state and signal frequency.
   //! @param aStateId [input] The string ID representing the signature state to be used.
   //! @param aFrequency [input] The frequency of the desired noise pressure
   virtual double GetNoisePressure(WsfStringId aStateId, double aFrequency) = 0;

   //! Get the acoustic signature for a given signature state and signal frequency at
   //! the given azimuth and elevation.
   //! @param aStateId [input] The string ID representing the signature state to be used.
   //! @param aFrequency [input] The frequency of the desired noise pressure
   //! @param aTgtToRcvrAz [input] The azimuth from the target to the receiver
   //! @param aTgtToRcvrEl [input] the elevation from the target to the receiver
   virtual double GetNoisePressure(WsfStringId aStateId, double aFrequency, double aTgtToRcvrAz, double aTgtToRcvrEl) = 0;

   //! Get the min and max noise pressure values that can be returned
   //! by GetNoisePressure for the given signature state.
   virtual std::pair<double, double> GetNoisePressureLimits(WsfStringId aStateId) const;

   //! @name Methods to support the actual interface on the platform.
   //! These methods provide the interface from the sensor model to the signature.
   //!
   //! The user makes calls like WsfAcousticSignature::GetValue(...).
   //@{
   static WsfStringId GetInputType(WsfPlatform* aPlatformPtr);

   static WsfStringId GetState(WsfPlatform* aPlatformPtr);

   static bool SetState(WsfPlatform* aPlatformPtr, WsfStringId aState);

   static float GetScaleFactor(WsfPlatform* aPlatformPtr);

   static bool SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor);

   static double GetValue(WsfPlatform* aPlatformPtr, double aFrequency);

   static double GetValue(WsfPlatform* aPlatformPtr, double aFrequency, double aTgtToRcvrAz, double aTgtToRcvrEl);
   //@}

   static void RegisterScriptMethods(UtScriptTypes& aScriptTypes);
   static void RegisterInterface(WsfScenario& aScenario);
};

#endif
