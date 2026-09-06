// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERENGAGEMENTMANAGER_HPP
#define WSFCYBERENGAGEMENTMANAGER_HPP

#include "wsf_cyber_export.h"

#include <list>
#include <map>
#include <memory>

#include "WsfCyberAttackParameters.hpp"
#include "WsfCyberEngagement.hpp"
#include "effects/WsfCyberEffect.hpp"
class WsfPlatform;
class WsfSimulation;

namespace wsf
{
namespace cyber
{
//! @name wsf::cyber::EngagementManager class
//! This class, owned by the simulation extension, is limited to a single instance per
//! simulation that models and manages the progression of a cyber attack. Since this may
//! occur for many different platforms simultaneously, this class manages the multiple
//! cyber engagements that may be occurring at any given time in the simulation. This class
//! holds all cyber engagements for the simulation, and provides the entry points for
//! cyber usage in the simulation. Generally, any data that occurs due to events managed by
//! the WsfCyberEngagementManager will be stored on the individual engagement object,
//! allowing query of specific engagement details for use elsewhere. Other than the
//! list of engagements itself, this class is stateless, and shall only provide the logic
//! for manipulating engagements via the TSDEER cyber engagement model.
//! @note
//! TSDEER mapping:
//! Target: not modeled, provided by algorithm input (victim)
//! Scan: Initiated by user request via scan script method call.
//!       Modeled via the scan methods.
//! Delivery: Initiated by user request via attack script method call.
//!           Modeled by the CyberAttackInitialize method call (scheduled event)
//! Exploit: Continued automatically from Delivery phase.
//!          Modeled via CyberAttack (protected) method.
//! Effect: Continued automatically from Exploit phase.
//!         Modeled via effect specific implementation initiated
//!         by CyberAttack (protected) method.
//! React: Continued automatically from previous phases.
//!        Modeled via CyberAttackReact (protected) method.
class WSF_CYBER_EXPORT EngagementManager
{
public:
   //! A unified class container for holding all data associated with a particular engagement
   //! used by the manager.
   class EngagementData
   {
   public:
      //! EngagementData requires an engagement. Engagements can be moved, but not copied.
      EngagementData(Engagement aEngagement)
         : mEngagement(std::move(aEngagement))
      {
      }
      ~EngagementData() = default;

      //! No copying of data. Move semantics is allowed.
      //! We prevent copy constructor usage due to WsfScriptContext
      //! using a semantically incorrect implementation - copies require
      //! initialization calls that we don't want to do over again.
      EngagementData(const EngagementData& aSrc) = delete;
      EngagementData(EngagementData&& aSrc)      = default;

      Effect*                 GetEffect(const std::string& aEffectName) const;
      Engagement&             GetEngagement() { return mEngagement; }
      const AttackParameters& GetParameters() const { return mParameters; }
      bool                    IsParametersValid() const { return mParametersValid; }

      Effect& AddEffect(const std::string& aEffectName);
      void    RemoveEffect(const std::string& aEffectName);
      void    RemoveEffects();
      void    AddParameters(const AttackParameters& aParameters);
      void    RemoveParameters();

   protected:
      Engagement mEngagement;

      //! Do not modify this to use another container type.
      //! std::list is used to avoid copies on insertion and removal.
      std::list<UtCloneablePtr<Effect>> mEngagementEffects{};
      AttackParameters                  mParameters{};
      bool                              mParametersValid{false};
   };

   using EngagementMap = std::unordered_map<size_t, EngagementData>;

   //! Allow the scheduled delay events to call the scan and attack methods when a delay is required.
   //! No other classes should have outside access to these methods
   friend class Event;

   static EngagementManager& Get(WsfSimulation& aSimulation);

   EngagementManager() { mEngagements.reserve(500U); }
   ~EngagementManager()                             = default;
   EngagementManager(const EngagementManager& aSrc) = delete;
   const EngagementManager& operator=(const EngagementManager& aRhs) = delete;

   Engagement* FindEngagement(const std::string& aAttackType, const std::string& aAttacker, const std::string& aVictim);
   Engagement* FindEngagement(size_t aKey);

   bool EngagementExists(const std::string& aAttackType, const std::string& aAttacker, const std::string& aVictim) const;
   bool EngagementExists(size_t aKey) const;

   //! @name CullEngagements methods
   //! Removes engagements predicated by the existence of either the attacker or victim.
   //! Typically, only the victim's existence is required for an ongoing engagement.
   //! An attacker specific version of this method is also provided for use cases where
   //! users want to also end engagements where the attacker is required beyond the initiation
   //! of the engagement.
   //@{
   void CullVictimEngagements(const std::string& aVictim);
   void CullAttackerEngagements(const std::string& aAttacker);
   //@}

   bool CyberAttack(const std::string& aAttackType,
                    const std::string& aAttacker,
                    const std::string& aVictim,
                    WsfSimulation&     aSimulation,
                    AttackParameters*  aParameters = nullptr);

   //! @name CyberScan method
   //! Initiates a scan with the given parameters. This method initially checks for the existence of
   //! current engagement objects that match the provided arguments and ensures that a scan request
   //! is valid, while performing the necessary bookkeeping. If a scan request is valid, then
   //! the next method in the scanning chain will be called - ScanInitialize()
   bool CyberScan(const std::string& aAttackType,
                  const std::string& aAttacker,
                  const std::string& aVictim,
                  WsfSimulation&     aSimulation);

   //! @name Cancel method
   //! Stops the cyber attack progression via external request.
   bool Cancel(size_t aKey);

protected:
   //! Internal use only - wrapper for code reuse when searching for a victim or
   //! attacker by name.
   EngagementMap::iterator FindEngagementByPlatform(const std::string& aName, bool aByVictim);

   EngagementData* FindEngagementData(const std::string& aAttackType,
                                      const std::string& aAttacker,
                                      const std::string& aVictim);
   EngagementData* FindEngagementData(size_t aKey);

   //! @name Add Engagement method
   //! Adds an engagement with required data to the list of maintained
   //! engagements. If an engagement already exists with these parameters,
   //! it is returned instead. Always returns a reference with the found
   //! engagement or the newly created engagement.
   EngagementData& AddEngagement(const std::string& aAttackType,
                                 const std::string& aAttacker,
                                 const std::string& aVictim,
                                 WsfSimulation&     aSimulation);

   //! @name Scan implementation methods
   //! CyberScanInitialize() determines if a delay is required due to user input. CyberScan()
   //! implements the scan phase logic after any possible delay in execution in the previous method call.
   //! @note Once updated, the scan data exists for the lifetime of the engagement object. If
   //! any subsequent scans are attempted by the same attacker/victim/attack type combination,
   //! the completion of that scan will clobber any previous results.
   //@{
   void CyberScanInitialize(EngagementData& aEngagementData);
   void CyberScan(EngagementData& aEngagementData);
   //@}

   //! @name Attack implementation methods
   //! CyberAttackInitialize() processes any delivery delays in the cyber engagement model.
   //! CyberAttack() picks up with the exploit phase upon completion of the prior phase.
   //! CyberAttackEffect() is called on a successful attack to employ all associated effects.
   //! CyberAttackReact() is called upon the completion of the CyberAttackEffect() algorithm to model
   //! the reaction algorithm of the target.
   //! @note Once updated, the attack data exists for the lifetime of the engagement object. If
   //! any subsequent attacks are attempted by the same attacker/victim/attack type combination,
   //! the completion of that attack will clobber any previous results.
   //! (We could have something like an WsfCyberEngagementHistory class in the future that could
   //! log lightweight engagement data for unique attacker/victim/attack type combinations for later recall,
   //! if this is later determined to be necessary.)
   //@{
   void CyberAttackInitialize(EngagementData& aEngagementData);
   void CyberAttack(EngagementData& aEngagementData);
   void CyberAttackEffect(EngagementData& aEngagementData);
   void CyberAttackDetectionDelay(EngagementData& aEngagementData);
   void CyberAttackRecoveryDelay(EngagementData& aEngagementData);
   //@}

private:
   EngagementMap mEngagements;
};

} // namespace cyber
} // namespace wsf

#endif
