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

#ifndef WSFEW_EA_EP_HPP
#define WSFEW_EA_EP_HPP

#include "wsf_mil_export.h"

#include <map>
#include <vector>

class UtInput;
class WsfEM_XmtrRcvr;
#include "WsfEW_Effect.hpp"
#include "WsfEW_Technique.hpp"
#include "WsfEW_Types.hpp"
#include "WsfObject.hpp"

//! A class which encapsulates data and functionality related to an Electronic Attack
//! and an Electronic Protect techniques.
//!
//! This is an inheritable class which is inherited by WsfEW_EA and WsfEW_EP and
//! and provides the data and functionality related to both of these instances.
//!
//! Please note that the list/map of technique pointers is kept by the
//! inheriting class as the technique type may be a special case and not
//! necessarily the base WsfEW_Technique class.

class WSF_MIL_EXPORT WsfEW_EA_EP : public WsfObject
{
public:
   WsfEW_EA_EP();
   WsfEW_EA_EP& operator=(const WsfEW_EA_EP&) = delete;
   ~WsfEW_EA_EP() override                    = default;

   //! @name Common infrastructure methods.
   //@{
   //! Clone this object.
   //! @return a pointer to the new object.
   WsfEW_EA_EP* Clone() const override = 0;

   virtual bool Initialize(WsfSimulation& aSimulation, WsfEM_XmtrRcvr* aXmtrRcvrPtr);
   bool         ProcessInput(UtInput& aInput) override;
   //@}

   //! Determines if debugging is enabled.
   virtual bool DebugEnabled() const { return mDebug; }

   //! @name Linkage methods.
   //@{
   virtual WsfEM_XmtrRcvr& GetLinkedXmtrRcvr() const { return *mLinkedXmtrRcvrPtr; }
   //@}

   //! @name Jammer management methods.
   //@{
   virtual void RemoveJammerXmtr(unsigned int aXmtrId);
   virtual void RemoveJammerXmtr(WsfEM_Xmtr* aXmtrPtr);
   //@}

   //! @name EA methods.
   //@{
   virtual bool SetDeltaGainTechnique(double      aSimTime,
                                      WsfStringId aTechniqueId,
                                      bool        aOnOff,
                                      bool        aDebug,
                                      WsfStringId aEffectId,
                                      double      aJammingDeltaGain,
                                      WsfStringId aSystemTypeId,
                                      WsfStringId aSystemFunctionId)
   {
      return false;
   }
   //@}

   //! @name EP methods.
   //@{
   virtual WsfEW_Technique::EffectPtrVec GetMitigatingEffects(const WsfEW_Effect*          aEA_EffectPtr,
                                                              WsfEW_Effect::SystemFunction aAffectingSystemFunction,
                                                              WsfEW_Effect::SystemFunction aAffectedSystemFunction);
   //@}

   //! @name Technique methods.
   //@{
   virtual bool AddTechnique(WsfEW_Technique* aTechniquePtr);

   //! Return true if there are techniques available, 0 otherwise.
   bool TechniquesAreAvailable() const { return !mTechniques.empty(); }

   //! Return true if there are techniques in use, 0 otherwise.
   bool TechniquesAreInUse() const { return !mTechniquesInUse.empty(); }

   bool TechniqueIsAvailable(WsfEW_Types::TechniqueId aId) const;

   bool TechniqueIsInUse(WsfEW_Types::TechniqueId aId) const;

   bool TechniqueIsExternallyControlled(WsfEW_Types::TechniqueId aId) const;

   //! Return a list of all the technique Ids that are currently available.
   const WsfEW_Types::TechniqueIdVec& GetTechniquesAvailable() const { return mTechniques; }

   //! Return a list of all the technique Ids that are currently in use.
   const WsfEW_Types::TechniqueIdVec& GetTechniquesInUse() const { return mTechniquesInUse; }

   void SetTechnique(WsfEW_Types::TechniqueId aTechniqueId, bool aOnOff);

   virtual WsfEW_Technique* GetTechnique(WsfEW_Types::TechniqueId aTechniqueId) const;
   virtual WsfEW_Technique* GetTechnique(unsigned int aIndex) const;

   void ResetTechniquesInUseList();

   virtual bool SelectTechnique(unsigned int aIndex);
   virtual bool SelectTechnique(WsfEW_Types::TechniqueId aId);
   virtual bool DeselectTechnique(unsigned int aIndex);
   virtual bool DeselectTechnique(WsfEW_Types::TechniqueId aId);

   virtual bool CanApplyTechnique(WsfEW_Types::TechniqueId aId, WsfEW_Effect::SystemFunction aSystemFunction) const;

   virtual WsfEW_Types::TechniqueIdVec* GetMitigatedTechniques() { return nullptr; }
   //@}

   //! @name Effect methods.
   //@{
   bool HasFrequencyAgilityEffect() const;

   bool EffectIsAvailable(WsfStringId aEffectNameId) const;
   bool EffectIsAvailable(WsfEW_Types::TechniqueId aTechId, WsfStringId aEffectNameId) const;

   virtual WsfEW_Effect* GetEffect(WsfEW_Types::TechniqueId aTechId, unsigned int aIndex) const;
   virtual WsfEW_Effect* GetEffect(WsfEW_Types::TechniqueId aTechId, WsfStringId aEffectId) const;

   virtual const WsfEW_Technique::EffectPtrVec& GetEffects(WsfEW_Types::TechniqueId aId) const;
   //@}

   WsfSimulation* GetSimulation() const;

protected:
   WsfEW_EA_EP(const WsfEW_EA_EP& aSrc);

   bool PrivateSetTechnique(WsfEW_Types::TechniqueId aTechnique, bool aOnOff);

   WsfSimulation*              mSimulationPtr;
   WsfEW_Types::TechniqueIdVec mTechniques;        //!< A collection/list of all the Techniques by Id.
   WsfEW_Types::TechniqueIdVec mTechniquesInUse;   //!< A collection of all the Techniques currently in use by Id.
   bool                        mDebug;             //!< Debug flag 'true' if debug is enabled, 'false' otherwise.
   WsfEM_XmtrRcvr*             mLinkedXmtrRcvrPtr; //!< The transmitter/receiver to which this EA/EP is linked to.
};

#endif
