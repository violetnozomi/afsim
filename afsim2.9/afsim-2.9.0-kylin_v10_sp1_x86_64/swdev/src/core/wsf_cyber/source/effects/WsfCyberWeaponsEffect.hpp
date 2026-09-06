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

#ifndef WSFCYBERWEAPONSEFFECT_HPP
#define WSFCYBERWEAPONSEFFECT_HPP

#include "WsfCyberEffect.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfTrack.hpp"

namespace wsf
{
namespace cyber
{

class WeaponsEffect : public Effect
{
public:
   enum EffectType
   {
      cRETARGET,
      cUNTARGETED,
      cOTHER
   };

   WeaponsEffect(EffectType aType);
   WeaponsEffect(const WeaponsEffect& aSrc) = default;
   ~WeaponsEffect() override                = default;

   //! @name Common infrastructure methods.
   //@{
   Effect* Clone() const override;
   bool    Initialize(Engagement& aEngagement) override;
   bool    ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Modified Clone Method
   //! @note This clone variation should only be used for specific
   //! instances that require user driven input, and never for a type.
   //! It should only be called by WsfCyberEngagementManager for creating
   //! a specific instance with the user supplied data (a track) for use
   //! in the attack and recover calls for this effect
   Effect* Clone(const AttackParameters& aParameters) const override;

   //! @name Engagement methods.
   //! These methods are called during the course of the engagement.
   //@{
   void Attack(double aSimTime, Engagement& aEngagement) override;

   void Restore(double aSimTime, Engagement& aEngagement) override;
   //@}

   InputRequirement RequiredInput() const override;
   size_t           RequiredInputCount() const override;

protected:
   //! Protected constructor for internal usage.
   WeaponsEffect(const WeaponsEffect& aSrc, const AttackParameters& aParameters);

private:
   //! @note This track reference is not owned by this object, it is only
   //! stored for convenience, and care must be taken with its use. This
   //! value will be null for type instances, and only filled using the
   //! constructor for short lived instances of effect objects. See
   //! WsfCyberEngagementManager for more details on effect usage
   //! and instantiation particulars.
   WsfTrack*  mTargetTrack;
   WsfTrackId mPreviousTargetTrack;
   EffectType mType;
};

} // namespace cyber
} // namespace wsf

#endif
