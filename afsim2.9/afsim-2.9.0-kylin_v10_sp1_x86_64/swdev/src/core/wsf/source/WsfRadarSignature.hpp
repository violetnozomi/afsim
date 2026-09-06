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

#ifndef WSFRADARSIGNATURE_HPP
#define WSFRADARSIGNATURE_HPP

#include "wsf_export.h"

class UtScriptTypes;
#include "WsfEM_Types.hpp"
class WsfEM_Rcvr;
class WsfEM_Xmtr;
class WsfPlatform;
class WsfScenario;
#include "WsfSignature.hpp"

//! An object that represent the radar signature of a platform.
class WSF_EXPORT WsfRadarSignature : public WsfSignature
{
public:
   enum
   {
      cSIGNATURE_INDEX = 0 //!< Index of signature within WsfSignatureList (must be unique)
   };

   WsfRadarSignature();
   WsfRadarSignature* Clone() const override = 0;

   // =================================================================================================
   //! Get the radar signature for a given set of conditions.
   //!
   //! @param aStateId      [input] The string ID representing the signature state to be used.
   //! @param aPolarization [input] The polarization of the signal.
   //! @param aFrequency    [input] The frequency of the signal (Hz).
   //! @param aTgtToXmtrAz  [input] The azimuth   of the transmitter with respect to the target.
   //! @param aTgtToXmtrEl  [input] The elevation of the transmitter with respect to the target.
   //! @param aTgtToRcvrAz  [input] The azimuth   of the receiver    with respect to the target.
   //! @param aTgtToRcvrEl  [input] The elevation of the receiver    with respect to the target.
   //! @param aXmtrPtr      [input] Optional pointer to the transmitter
   //! @param aRcvrPtr      [input] Optional pointer to the receiver
   //! @returns The radar cross section (m^2)
   virtual float GetSignature(WsfStringId               aStateId,
                              WsfEM_Types::Polarization aPolarization,
                              double                    aFrequency,
                              double                    aTgtToXmtrAz,
                              double                    aTgtToXmtrEl,
                              double                    aTgtToRcvrAz,
                              double                    aTgtToRcvrEl,
                              WsfEM_Xmtr*               aXmtrPtr = nullptr,
                              WsfEM_Rcvr*               aRcvrPtr = nullptr) = 0;

   //! Get the min and max radar cross section values that can be returned
   //! by GetSignature for the given signature state and polarization.
   virtual std::pair<float, float> GetSignatureLimits(WsfStringId aStateId, WsfEM_Types::Polarization aPolarization) const;

   //! @name Methods to support the actual interface on the platform.
   //! These methods provide the interface from the sensor model to the signature.
   //!
   //! The user makes calls like WsfOpticalReflectivity::GetSignature(...).
   //@{
   static WsfStringId GetInputType(WsfPlatform* aPlatformPtr);

   static WsfStringId GetState(WsfPlatform* aPlatformPtr);

   static bool SetState(WsfPlatform* aPlatformPtr, WsfStringId aState);

   static float GetScaleFactor(WsfPlatform* aPlatformPtr);

   static bool SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor);

   static float GetValue(WsfPlatform*              aPlatformPtr,
                         WsfEM_Types::Polarization aPolarization,
                         double                    aFrequency,
                         double                    aTgtToXmtrAz,
                         double                    aTgtToXmtrEl,
                         double                    aTgtToRcvrAz,
                         double                    aTgtToRcvrEl);

   static float GetValue(WsfPlatform* aPlatformPtr,
                         WsfEM_Xmtr*  aXmtrPtr,
                         WsfEM_Rcvr*  aRcvrPtr,
                         double       aTgtToXmtrAz,
                         double       aTgtToXmtrEl,
                         double       aTgtToRcvrAz,
                         double       aTgtToRcvrEl);

   static WsfRadarSignature* GetSignature(WsfPlatform* aPlatformPtr);

   static bool SetSignature(WsfPlatform* aPlatformPtr, WsfRadarSignature* aSignaturePtr);
   //@}

   static void RegisterScriptMethods(UtScriptTypes& aScriptTypes);
   static void RegisterInterface(WsfScenario& aScenario);
};

#endif
