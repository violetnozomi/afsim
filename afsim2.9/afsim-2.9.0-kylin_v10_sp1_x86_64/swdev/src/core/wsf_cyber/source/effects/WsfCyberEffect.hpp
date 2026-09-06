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

#ifndef WSFCYBEREFFECT_HPP
#define WSFCYBEREFFECT_HPP

#include "wsf_cyber_export.h"

class UtInput;
#include "WsfCyberAttackParameters.hpp"
#include "WsfObject.hpp"

namespace wsf
{
namespace cyber
{

class Engagement;

//! The base cyber effect object interface. Effect usage is defined by the attack object, and then
//! associated with a cyber engagement. Effect removal is conditional, so users should not
//! make any assumptions regarding the lifetime of these objects.
class WSF_CYBER_EXPORT Effect : public WsfObject
{
public:
   enum class InputRequirement : size_t
   {
      cNOT_ALLOWED, // User input is not allowed, and is an error if provided.
      cREQUIRED,    // User input is required, and is an error if not provided.
      cOPTIONAL     // User input may or may not be used.
   };

   Effect()           = default;
   ~Effect() override = default;

   //! Deleted to prevent use.
   Effect& operator=(const Effect& aRhs) = delete;

   //! @name Common infrastructure methods.
   //@{
   //! Clone this object.
   //! @return a pointer to the new object.
   Effect* Clone() const override = 0;

   //! CyberEffect specific clone method, for those effects that require
   //! additional input in order to function.
   //! Usage is implementation dependent. Default behavior is a clone of the effect
   //! without usage of the data.
   virtual Effect* Clone(const AttackParameters& /*aParameters*/) const { return Clone(); }

   virtual bool Initialize(Engagement& aEngagement);
   bool         ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Engagement methods.
   //! These methods are called during the course of the engagement.
   //@{
   //!
   //! Take whatever actions are necessary to model the attack on the victim.
   //! @param aSimTime    The current simulation time.
   //! @param aEngagement Reference to the engagement object.
   virtual void Attack(double aSimTime, Engagement& aEngagement) = 0;

   //! Take whatever actions are necessary to restore the victim to its pre-attack state.
   //! This simulates the victim successfully recovering from an attack.
   //! @param aSimTime    The current simulation time.
   //! @param aEngagement Reference to the engagement object.
   virtual void Restore(double aSimTime, Engagement& aEngagement) = 0;
   //@}

   //! Indicates if this effect requires additional input to function correctly.
   //! The presence and use of such data is handled in the derived class, but the
   //! interface for such a query is provided here.
   virtual InputRequirement RequiredInput() const { return InputRequirement::cNOT_ALLOWED; }

   //! In cases where input is required, provides the number of user
   //! supplied variables that are required to instantiate this
   //! effect. Note that variables/parameters may be optional, such
   //! that they are not included in this value.
   virtual size_t RequiredInputCount() const { return 0U; }

protected:
   Effect(const Effect& aSrc) = default;

private:
   bool mDebug{false};
};

} // namespace cyber
} // namespace wsf

#endif
