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

#ifndef WSFEW_EA_HPP
#define WSFEW_EA_HPP

#include "wsf_mil_export.h"

#include <map>
#include <vector>

class UtInput;
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA_EP.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_Technique.hpp"
#include "WsfEW_Types.hpp"

//! A class which encapsulates data and functionality related to an Electronic Attack.
//!
//! An instance of this class is aggregated into a weapon whose intent is to defeat an
//! adversary using electromagnetic emissions.  The attack may be designed to confuse,
//! disorient, blind, or otherwise deceive an opponents sensors, or overwhelm his
//! ability to process a voluminous amount of mis-information.  The attack may include
//! employing one or more preplanned counter-counter-techniques in response to the
//! adversary's recognition of the attack, and a corresponding attempt to nullify it.

class WSF_MIL_EXPORT WsfEW_EA : public WsfEW_EA_EP
{
public:
   static WsfEW_EA_EP* GetElectronicAttack(const WsfEM_Xmtr& aXmtr);
   static void         SetElectronicAttack(WsfEM_Xmtr& aXmtr, WsfEW_EA_EP* aElectronicAttackPtr);

   WsfEW_EA();
   WsfEW_EA& operator=(const WsfEW_EA&) = delete;
   ~WsfEW_EA() override;

   //! @name Common infrastructure methods.
   //@{
   WsfEW_EA_EP* Clone() const override;
   bool         Initialize(WsfSimulation& aSimulation, WsfEM_XmtrRcvr* aXmtrRcvrPtr) override;
   //@}

   //! Special class to handle the EA specific technique implementations as required.
   class EA_Technique : public WsfEW_Technique
   {
   public:
      EA_Technique() = default;
      WsfEW_Technique* Clone() const override;
      bool             Initialize(WsfSimulation& aSimulation) override;
      bool             ProcessInput(UtInput& aInput) override;

      WsfStringId GetMitigationClassId() const override { return mMitigationClassId; }

   private:
      //!< User input mitigation class name Id to provide method to map mitigating EP techniques with.
      WsfStringId mMitigationClassId{nullptr};
   };

   //! @name Technique methods.
   //@{

   //! Map the technique's ID to its location (pointer).
   typedef std::map<WsfEW_Types::TechniqueId, EA_Technique*>                 TechniqueIdToPtrMap;
   typedef std::map<WsfEW_Types::TechniqueId, EA_Technique*>::iterator       TechniqueIdToPtrMapIter;
   typedef std::map<WsfEW_Types::TechniqueId, EA_Technique*>::const_iterator TechniqueIdToPtrMapConstIter;

   bool AddTechnique(WsfEW_Technique* aTechniquePtr) override;

   WsfEW_Technique* GetTechnique(WsfEW_Types::TechniqueId aTechniqueId) const override;
   WsfEW_Technique* GetTechnique(unsigned int aIndex) const override;

   using WsfEW_EA_EP::DeselectTechnique;
   using WsfEW_EA_EP::SelectTechnique;
   virtual bool SelectTechnique(unsigned int aIndex, WsfEW_Effect::SystemFunction aSystemFunction);
   virtual bool SelectTechnique(WsfEW_Types::TechniqueId aId, WsfEW_Effect::SystemFunction aSystemFunction);
   virtual bool DeselectTechnique(unsigned int aIndex, WsfEW_Effect::SystemFunction aSystemFunction);
   virtual bool DeselectTechnique(WsfEW_Types::TechniqueId aId, WsfEW_Effect::SystemFunction aSystemFunction);

   bool SetDeltaGainTechnique(double      aSimTime,
                              WsfStringId aTechniqueId,
                              bool        aOnOff,
                              bool        aDebug,
                              WsfStringId aEffectId,
                              double      aJammingDeltaGain,
                              WsfStringId aSystemTypeId,
                              WsfStringId aSystemFunctionId) override;
   //@}

protected:
   WsfEW_EA(const WsfEW_EA& aSrc);

private:
   //! Map of the technique Id to the pointer to the technique.
   TechniqueIdToPtrMap mTechniquePtrMap;
};

#endif
