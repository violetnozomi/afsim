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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFEW_AGILITYEFFECT_HPP
#define WSFEW_AGILITYEFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>

class UtInput;

#include "WsfEM_Interaction.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_Types.hpp"

class WSF_MIL_EXPORT WsfEW_AgilityEffect : public WsfEW_Effect
{
public:
   enum AgilityType
   {
      cAT_MODE      = 1,
      cAT_FREQUENCY = cAT_MODE << 1
   };

   WsfEW_AgilityEffect();
   WsfEW_AgilityEffect(WsfEW_AgilityEffect&&) = default;
   WsfEW_AgilityEffect& operator=(const WsfEW_AgilityEffect&) = delete;
   WsfEW_AgilityEffect& operator=(WsfEW_AgilityEffect&&) = default;
   ~WsfEW_AgilityEffect() override                       = default;
   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   void ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const override;

   void ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const override;

   void UpdateEffects(double                                 aSimTime,
                      const WsfEM_Interaction&               aTargetInteraction,
                      const WsfEM_Interaction&               aJammerToTgtInteraction,
                      const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                      WsfEW_Effects&                         aEW_Effects,
                      WsfEW_Effect*                          aEW_EffectPtr = nullptr) override;

   //! @name Agility Accessor/Mutator methods.
   //@{

   int GetAgilityTypeMask() const { return mAgilityTypeMask; }
   //@}

protected:
   WsfEW_AgilityEffect(const WsfEW_AgilityEffect& aSrc) = default;

private:
   class AgilityDataValues
   {
   public:
      AgilityDataValues()
         : mId()
         , mValue(0.0)
      {
      }

      virtual ~AgilityDataValues() = default;

      bool operator==(const AgilityDataValues& aRhs) const;
      bool operator!=(const AgilityDataValues& aRhs) const;

      WsfStringId mId;
      double      mValue;
   };

   //! Typedef of the Id and corresponding data values for the Id.
   using AgilityDataVec     = std::vector<AgilityDataValues>;
   using AgilityDataVecIter = std::vector<AgilityDataValues>::iterator;

   //! Typedef of the Agility Type Id (enumeration) and the Agility Data map
   using AgilityMap     = std::map<unsigned int, AgilityDataVec>;
   using AgilityMapIter = std::map<unsigned int, AgilityDataVec>::iterator;

   //! Typedef of the Agility Type Id (enumeration) and the delay value
   // typedef std::map<unsigned int, double> AgilityTimeMap;
   // typedef std::map<unsigned int, double>::iterator AgilityTimeMapIter;

   using CurrentIdMap = std::map<unsigned int, int>;

   // Input variables
   //! Map of the Agility Type enumeration to the data required for this technique.
   AgilityMap mAgilityMap;

   //! Type mask for the agilities that this effect exhibits from AgilityType.
   int mAgilityTypeMask = 0;

   //! Flags for the most recent call of UpdateEffects.
   bool mFrequencyChangeScheduled = false;
   bool mModeChangeScheduled      = false;
};

#endif
