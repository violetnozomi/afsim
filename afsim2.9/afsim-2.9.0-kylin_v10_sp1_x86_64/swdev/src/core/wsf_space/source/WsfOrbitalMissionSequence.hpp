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

#ifndef WSFORBITALMISSIONEVENTSEQUENCE_HPP
#define WSFORBITALMISSIONEVENTSEQUENCE_HPP

#include "wsf_space_export.h"

#include <list>
#include <memory>

#include "UtCloneablePtr.hpp"
class UtOrbitalPropagatorBase;
#include "WsfOrbitalEvent.hpp"
class WsfScenario;
#include "WsfStringId.hpp"

//! A sequence of maneuvers.  A maneuver sequence is aggregated by a WsfManeuvering object,
//! which can execute maneuver sequences for capable derived propagator types, or for
//! space mover types that aggregates a capable derived propagator.
//! Currently the sole capable propagator is the UtOrbitalPropagator, and the
//! only mover capable of executing maneuvers is the WsfSpaceMover.
class WSF_SPACE_EXPORT WsfOrbitalMissionSequence : public WsfOrbitalEvent
{
public:
   explicit WsfOrbitalMissionSequence(const WsfScenario& aScenario); // Scenario object needed to get orbital event types factory,
   // used to populate the sequence (see ProcessInput).
   WsfOrbitalMissionSequence(const WsfOrbitalMissionSequence& aSrc);
   WsfOrbitalMissionSequence& operator=(const WsfOrbitalMissionSequence&) = delete;
   ~WsfOrbitalMissionSequence() override                                  = default;

   static WsfStringId         GetTypeId();
   WsfOrbitalMissionSequence* Clone() const override { return new WsfOrbitalMissionSequence(*this); }
   bool                       ProcessInput(UtInput& aInput) override;
   bool Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   const WsfOrbitalEvent* GetCurrentMissionEvent() const override;
   WsfOrbitalEvent*       GetCurrentMissionEvent() override;
   WsfOrbitalEvent* AdvanceMissionEvent(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;

   UtCalendar ComputeContraintEpoch(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator, bool& aSuccess) const;

   using MissionEvents = std::list<ut::CloneablePtr<WsfOrbitalEvent>>;

   //! Return the actual sequence (std::list) of maneuvers (WsfOrbitalEvent).
   const MissionEvents& GetSequence() const { return mMissionEvents; }

   void AddMissionEvent(std::unique_ptr<WsfOrbitalEvent>&& aMissionEventPtr);

   double GetRequiredDeltaV(const UtOrbitalPropagatorBase& aPropagator) const override;

   double GetDeltaV() const override;

   bool        IsSequence() const override { return true; }
   const char* GetScriptClassName() const override { return "WsfOrbitalMissionSequence"; }

   //! Return the actual sequence (std::list) of maneuvers (WsfOrbitalEvent).
   MissionEvents& GetSequence() { return mMissionEvents; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   bool ProcessInputWithIgnore(UtInput& aInput, bool aIgnoreSomeExceptions);

   //! Perform a pre-order traversal on the mission sequence.
   //!
   //! This will perform a pre-order traversal of the mission sequence, invoking the
   //! given @p aOperation on each mission event. The provided operation must have the
   //! signature bool(WsfOrbitalEvent*), and will return true if the traversal should
   //! proceed beyond the given orbital event, and false if the traversal should halt.
   //!
   //!@param aOperation - A callable entity containing the operation to perform at each event of the sequence.
   //!@returns          - true if every invocation of the operation returns true; false otherwise.
   template<typename Callable>
   bool PreOrderTraversal(Callable aOperation)
   {
      bool retval = aOperation(this);
      auto iter   = mMissionEvents.begin();
      while (retval && iter != mMissionEvents.end())
      {
         if ((*iter)->IsSequence())
         {
            auto sequencePtr = dynamic_cast<WsfOrbitalMissionSequence*>(iter->get());
            if (sequencePtr)
            {
               retval = sequencePtr->PreOrderTraversal(aOperation);
            }
         }
         else
         {
            retval = aOperation(*iter);
         }
         ++iter;
      }
      return retval;
   }

   bool IsEmpty() const;

protected:
   WsfOrbitalEvent* GetNextMissionEvent() const;

   const WsfScenario& GetScenario() const { return mScenario; }

private:
   bool ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext) override;
   bool EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator) override;

   void InitializeOrBreakout(WsfOrbitalEvent*&                        aNextMissionEventPtr,
                             const UtCalendar&                        aEpoch,
                             const wsf::space::OrbitalMissionContext& aContext);

   const WsfScenario&      mScenario;
   MissionEvents           mMissionEvents;
   MissionEvents::iterator mNextMissionEventIter;
   MissionEvents::iterator mMissionEventIter;
};

#endif
