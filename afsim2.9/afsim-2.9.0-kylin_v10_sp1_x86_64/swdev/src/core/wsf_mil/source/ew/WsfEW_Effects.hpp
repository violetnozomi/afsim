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

#ifndef WSFEW_EFFECTS_HPP
#define WSFEW_EFFECTS_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <string>

#include "UtLog.hpp"
#include "UtOptional.hpp"

class UtScriptClass;
class UtScriptTypes;
class WsfEM_Interaction;
class WsfEM_Rcvr;
class WsfEM_Xmtr;
class WsfEW_Effect;
class WsfEM_Interaction;

//! A support class for Electronic Warfare (EW) interaction effects.
//!
//! This class is used to provide a common mechanism for performing
//! EW interactions (i.e.: interactions involving EW).
//!
//! Most of the class data is public and represents the results of various
//! calculations performed by the class. Much data is maintained so interested
//! parties can gather data without having to modify the underlying code.

class WSF_MIL_EXPORT WsfEW_Effects
{
public:
   class EW_Error
   {
   public:
      void Reset();

      double mAzimuth{0.0};
      double mElevation{0.0};
      double mRange{0.0};
      double mVelocity{0.0};
      double mBitErrorRate{0.0};
   };

   class EW_JammingEffect
   {
   public:
      void Reset();
      void Zeroize();

      double mBlanking{1.0};
      double mCancelation{1.0};
      double mModulationGain{1.0};
      double mJammingPowerGain{1.0};
      double mJtoXGain{1.0};
      int    mProtectTarget{-1}; // -1=undefined, 0=false, 1=true
      double mPulseSuppression{1.0};
      double mRadiusFactor{1.0};
      double mRepeaterFactor{1.0};
      double mRPJ_Gain{1.0};
   };

   class EW_SignalEffect
   {
   public:
      void Reset();
      void Zeroize();

      double mSignalPowerGain{1.0};
      double mRcvrNoiseGain{1.0};
   };

   class RepeaterData
   {
   public:
      void Reset();

      double mMinGain{-1.0};
      double mGain{-1.0};
      double mPeakGain{-1.0};
   };

   virtual ~WsfEW_Effects() = default;

   // =================================================================================================
   //! Create the 'class' object for the script system.
   //! This is invoked once by WsfScriptManager to create the 'class' object that defines
   //! the interface to instances of this class from the script system.
   // static
   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   // =================================================================================================
   //! Process an electronic warfare interaction between a transmitter and a receiver during
   //! a target interaction.
   //!
   //! @param aSimTime                 [input] The current simulation time.
   //! @param aTargetInteraction       [input] The target interaction for the detection attempt.
   //! @param aJammerToTgtInteraction  [input] The jammer interaction for the jammer xmtr to victim Rcvr.
   //! @param aTgtToJammerInteraction  [input] Optional jammer interaction for the Victim Xmtr to jammer Rcvr.
   //!                                         This will have no value if the jammer Rcvr doesn't exist.
   //! @param aProcessOnlyEA_Effects   [input] Process only EA effects, not EP effects.
   void Process(double                                 aSimTime,
                const WsfEM_Interaction&               aTargetInteraction,
                const WsfEM_Interaction&               aJammerToTgtInteraction,
                const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                bool                                   aProcessOnlyEA_Effects);

   // =================================================================================================
   //! Print the interaction data to standard output.
   void Print(ut::log::MessageStream& aStream) const;
   void Reset();

   unsigned int     mMask{0};
   unsigned int     mEA_CoherencyMask{0};
   EW_Error         mEW_Error;
   EW_JammingEffect mEW_NoiseJammingEffect;
   EW_JammingEffect mEW_PulseJammingEffect;
   EW_JammingEffect mEW_CoherentJammingEffect;
   EW_SignalEffect  mEW_SignalEffect;
   RepeaterData     mCurrentRepeaterData;
};

#endif
