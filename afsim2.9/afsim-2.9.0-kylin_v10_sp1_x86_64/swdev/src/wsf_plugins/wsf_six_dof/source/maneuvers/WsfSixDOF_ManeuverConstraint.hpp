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

#ifndef WSF_SIXDOF_MANEUVERCONSTRAINT_HPP
#define WSF_SIXDOF_MANEUVERCONSTRAINT_HPP

#include "wsf_six_dof_export.h"

namespace wsf
{
namespace six_dof
{
class Mover;

class ManeuverConstraint
{
public:
   ManeuverConstraint() {}
   virtual ~ManeuverConstraint() = default;

   virtual ManeuverConstraint* Clone() const = 0;

   //! Assign this constraint to a maneuver.
   //!
   //! This method is called by a maneuver when the constraint is assigned. This
   //! will happen when the maneuver is assigned for entry constraints, and after
   //! the maneuver's Execute method is called its final time.
   //!
   //! \param aSimTime  - The simulation time of the assignment.
   //! \param aMoverPtr - The mover to which the maneuver was assigned.
   virtual void Assign(double aSimTime, Mover* aMoverPtr) = 0;

   //! Returns if the constraint is satisfied.
   virtual bool Satisfied(double aSimTime) const = 0;

   //! Returns the next evaluation time.
   //!
   //! This returns the time after @p aSimTime at which this constraint should next
   //! be evaluated. It is a requirement that the return value be strictly later
   //! than the provided @p aSimTime.
   //!
   //! \param aSimTime - the current evaluation time.
   //! \returns        - the next time at which the constraint should be evaluated.
   virtual double NextEvaluationTime(double aSimTime) const = 0;

   //! Return a type identifier used in debugging output.
   virtual const char* Type() const = 0;
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_MANEUVERCONSTRAINT_HPP
