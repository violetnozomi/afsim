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

#ifndef WSFEW_EP_HPP
#define WSFEW_EP_HPP

#include "wsf_mil_export.h"

#include <map>
#include <vector>

class UtInput;
class UtInputBlock;
#include "WsfEM_Rcvr.hpp"
#include "WsfEW_EA_EP.hpp"
class WsfEW_Effect;
#include "WsfEW_Technique.hpp"
#include "WsfEW_Types.hpp"

//! A class which models an Electronic Protection procedure in response to an Electronic Attack.
//!
//! An instance of this class is aggregated into a receiver that may be subject to Electronic
//! Attack (EA) (see WsfEW_EA).  The response may be to use mitigating techniques to thwart the
//! attack, including the employment of one or more pre-planned counter-techniques in response
//! to the adversary.

class WSF_MIL_EXPORT WsfEW_EP : public WsfEW_EA_EP
{
public:
   static WsfEW_EA_EP* GetElectronicProtect(const WsfEM_Rcvr& aRcvr);
   static void         SetElectronicProtect(WsfEM_Rcvr& aRcvr, WsfEW_EA_EP* aElectronicProtectPtr);

   WsfEW_EP();
   WsfEW_EP& operator=(const WsfEW_EP&) = delete;
   ~WsfEW_EP() override;

   //! @name Common infrastructure methods.
   //@{
   WsfEW_EA_EP* Clone() const override;
   bool         Initialize(WsfSimulation& aSimulation, WsfEM_XmtrRcvr* aXmtrRcvrPtr) override;
   //@}

   //! Special class to handle the EP specific technique implementations as required.
   class EP_Technique : public WsfEW_Technique
   {
   public:
      EP_Technique();
      EP_Technique(const EP_Technique& aSrc);
      EP_Technique&    operator=(const EP_Technique&) = delete;
      WsfEW_Technique* Clone() const override;
      bool             Initialize(WsfSimulation& aSimulation) override;
      bool             ProcessInput(UtInput& aInput) override;

      WsfEW_Types::TechniqueIdVec*           GetMitigatedTechniqueIds() override { return &mMitigatedTechniqueIds; }
      std::vector<WsfEW_Types::TechniqueId>* GetMitigatedTechniqueClassIds() override
      {
         return &mMitigatedTechniqueClassIds;
      }

      bool IsExternallyControlled() const override { return mExternallyControlled; }

   protected:
      void ProcessMitigatedTechniquesBlock(UtInputBlock& aInputBlock);
      void ProcessMitigatedTechniqueFlagsBlock(UtInputBlock& aInputBlock);

   private:
      //! List of mitigated EA technique(s), by name, this technique can mitigate.
      WsfEW_Types::TechniqueIdVec mMitigatedTechniqueIds;

      //! List of mitigated EA technique classes this technique can mitigate.
      //! The classes are mapped as entered in the by WsfEW_EA::EA_Technique::mMitiationClassId variable.
      WsfEW_Types::TechniqueIdVec mMitigatedTechniqueClassIds;

      //! Specifies whether or not this technique is externally controlled.
      //! @note can be overridden by default_on command.
      bool mExternallyControlled;
   };

   //! @name Technique methods.
   //@{

   //! Map the technique's ID to its location (pointer).
   typedef std::map<WsfEW_Types::TechniqueId, EP_Technique*>                 TechniqueIdToPtrMap;
   typedef std::map<WsfEW_Types::TechniqueId, EP_Technique*>::iterator       TechniqueIdToPtrMapIter;
   typedef std::map<WsfEW_Types::TechniqueId, EP_Technique*>::const_iterator TechniqueIdToPtrMapConstIter;

   bool AddTechnique(WsfEW_Technique* aTechniquePtr) override;

   WsfEW_Technique* GetTechnique(WsfEW_Types::TechniqueId aTechniqueId) const override;
   WsfEW_Technique* GetTechnique(unsigned int aIndex) const override;

   using WsfEW_EA_EP::DeselectTechnique;
   using WsfEW_EA_EP::SelectTechnique;
   virtual bool SelectTechnique(unsigned int aIndex, WsfEW_Effect::SystemFunction aSystemFunction);
   virtual bool SelectTechnique(WsfEW_Types::TechniqueId aId, WsfEW_Effect::SystemFunction aSystemFunction);
   virtual bool DeselectTechnique(unsigned int aIndex, WsfEW_Effect::SystemFunction aSystemFunction);
   virtual bool DeselectTechnique(WsfEW_Types::TechniqueId aId, WsfEW_Effect::SystemFunction aSystemFunction);

   WsfEW_Types::TechniqueIdVec* GetMitigatedTechniqueIds(WsfEW_Types::TechniqueId aId) const;
   //@}

   //! @name Effect methods.
   //@{

   WsfEW_Technique::EffectPtrVec GetMitigatingEffects(const WsfEW_Effect*          aEA_EffectPtr,
                                                      WsfEW_Effect::SystemFunction aAffectingSystemFunction,
                                                      WsfEW_Effect::SystemFunction aAffectedSystemFunction) override;
   //@}

protected:
   WsfEW_EP(const WsfEW_EP& aSrc);

private:
   //! Map of the technique Id to the pointer to the technique.
   TechniqueIdToPtrMap mTechniquePtrMap;
};

#endif
