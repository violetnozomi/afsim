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

#ifndef WSFEW_INTERACTION_HPP
#define WSFEW_INTERACTION_HPP

#include "wsf_mil_export.h"

#include "UtLog.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfMilComponentRoles.hpp"

//! A component attached to WsfEM_Interaction that contains the results of EW interactions.
//! This component is common to both comm and sensor interactions as they are so similar
//! the sensor result only contained an additional element for false targets.) This ensures
//! that a component attached in WsfRF_Jammer will be usable by both the EW comm and sensor
//! components.
class WSF_MIL_EXPORT WsfEW_Result : public WsfEM_InteractionComponent
{
public:
   static WsfEW_Result* Find(const WsfEM_Interaction& aEM_Interaction);
   static WsfEW_Result& FindOrCreate(WsfEM_Interaction& aResult);

   WsfEW_Result()           = default;
   ~WsfEW_Result() override = default;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   void Print(ut::log::MessageStream& aMsgStream, WsfEM_Interaction& aEM_Interaction) override;
   void PrintStatus(ut::log::MessageStream& aMsgStream, WsfEM_Interaction& aEM_Interaction) override;
   void Reset() override;
   bool PrintEventOutput(std::ostream& aStream) override;
   void PrintEventOutput_Powers(std::ostream& aStream) override;
   bool PrintCSV_EventOutput(std::ostream& aStream) override;
   bool PrintCSV_EventOutput_Powers(std::ostream& aStream) override;

   //! EW Effects result data and methods.
   WsfEW_Effects mEW_Effects;

   //! Noise jamming power (watts)
   //! @note This is valid only if it is greater than zero.
   double mNoiseJammerPower{0.0};

   //! Pulsed noise jamming power (watts)
   //! @note This is valid only if it is greater than zero.
   double mPulseJammerPower{0.0};

   //! Coherent jamming power (watts)
   //! @note This is valid only if it is greater than zero.
   double mCoherentJammerPower{0.0};

   //! True if coherent (FT) jamming is perceived as determined by the coherent JNR threshold.
   bool mPerceivesCoherentJamming{false};

   //! True if noise type jamming is perceived as determined by the noise JNR threshold.
   bool mPerceivesContJamming{false};

   //! True if Pulse type jamming is perceived as determined by the pulse JNR threshold.
   bool mPerceivesPulseJamming{false};
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfEW_Result, cWSF_COMPONENT_EW_RESULT)

#endif
