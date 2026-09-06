// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SIXDOF_MANEUVERSEQUENCE_HPP
#define WSF_SIXDOF_MANEUVERSEQUENCE_HPP

#include "wsf_six_dof_export.h"

#include <list>
#include <memory>

#include "UtCloneablePtr.hpp"
#include "WsfSixDOF_Maneuver.hpp"

namespace wsf
{
namespace six_dof
{
//! A maneuver sequence.
//!
//! A maneuver sequence is a collection of maneuvers that are executed in order.
//! In addition to the entry and exit constraints on each of the individual member
//! maneuvers in the sequence, a maneuver sequence supports an overall entry and
//! exit constraint. A maneuver sequence can also be a looping sequence, in which
//! case the sequence will repeat indefinitely. For such a sequence to end, it
//! must be explicitly canceled. Any object deriving from Maneuver can be
//! used in a maneuver sequence, including other maneuver sequences.
class WSF_SIX_DOF_EXPORT ManeuverSequence : public Maneuver
{
public:
   ManeuverSequence();
   ManeuverSequence(const ManeuverSequence& aOther);
   ~ManeuverSequence() override = default;

   ManeuverSequence* Clone() const override;
   ManeuverSequence& operator=(const ManeuverSequence& aOther) = delete;

   const char* GetScriptClassName() const override { return "WsfSixDOF_ManeuverSequence"; }

   double Execute(double aSimTime) override;

   const char* Type() const override { return "SEQUENCE"; }

   void Clear();
   void Append(std::unique_ptr<Maneuver>&& aManeuverPtr);
   void InsertNext(std::unique_ptr<Maneuver>&& aManeuverPtr);

   bool IsSequence() const override { return true; }

   //! Returns true if the sequence of maneuvers repeats.
   bool IsLoop() const { return mLoop; }

   void SetLoop(bool aLoop);

   Maneuver* GetCurrentManeuver() const;

private:
   using Iterator = std::list<ut::CloneablePtr<Maneuver>>::iterator;

   Iterator Advance(const Iterator& aIterator);

   std::list<ut::CloneablePtr<Maneuver>> mManeuvers;
   Iterator                              mCurrentManeuver;
   bool                                  mLoop;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_MANEUVERSEQUENCE_HPP
