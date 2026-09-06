// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFMANEUVERSEQUENCE_HPP
#define WSFMANEUVERSEQUENCE_HPP

#include "wsf_p6dof_export.h"

#include <list>
#include <memory>

#include "UtCloneablePtr.hpp"
#include "WsfManeuver.hpp"

//! A maneuver sequence.
//!
//! A maneuver sequence is a collection of maneuvers that are executed in order.
//! In addition to the entry and exit constraints on each of the individual member
//! maneuvers in the sequence, a maneuver sequence supports an overall entry and
//! exit constraint. A maneuver sequence can also be a looping sequence, in which
//! case the sequence will repeat indefinitely. For such a sequence to end, it
//! must be explicitly canceled. Any object deriving from WsfManeuver can be
//! used in a maneuver sequence, including other maneuver sequences.
class WSF_P6DOF_EXPORT WsfManeuverSequence : public WsfManeuver
{
public:
   WsfManeuverSequence();
   WsfManeuverSequence(const WsfManeuverSequence& aOther);
   ~WsfManeuverSequence() override = default;

   WsfManeuverSequence* Clone() const override;
   WsfManeuverSequence& operator=(const WsfManeuverSequence& aOther) = delete;

   const char* GetScriptClassName() const override { return "WsfManeuverSequence"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SEQUENCE"; }

   void Clear();
   void Append(std::unique_ptr<WsfManeuver>&& aManeuverPtr);
   void InsertNext(std::unique_ptr<WsfManeuver>&& aManeuverPtr);

   bool IsSequence() const override { return true; }

   //! Returns true if the sequence of maneuvers repeats.
   bool IsLoop() const { return mLoop; }

   void SetLoop(bool aLoop);

   WsfManeuver* GetCurrentManeuver() const;

private:
   using Iterator = std::list<ut::CloneablePtr<WsfManeuver>>::iterator;

   Iterator Advance(const Iterator& aIterator);

   std::list<ut::CloneablePtr<WsfManeuver>> mManeuvers;
   Iterator                                 mCurrentManeuver;
   bool                                     mLoop;
};

#endif // WSFMANEUVERSEQUENCE_HPP
