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

#ifndef WSFCYBERRANDOM_HPP
#define WSFCYBERRANDOM_HPP

#include <map>

#include "WsfCyberAttack.hpp"
class WsfRandomVariable;
class WsfSimulation;

namespace wsf
{
namespace cyber
{
namespace random // Extended from "WsfCyberAttack.hpp"
{
//! Enumerated probability types that can be drawn for in Cyber.
enum ProbabilityType
{
   cSCAN_DETECTION,
   cSCAN_ATTRIBUTION,
   cATTACK_SUCCESS,
   cSTATUS_REPORT,
   cATTACK_DETECTION,
   cATTACK_ATTRIBUTION,
   cFUTURE_IMMUNITY
};

//! @name RandomManager class
//! RandomManager holds data related to when and if draws have occurred, and allows
//! referencing to know when a new draw is required or an old draw should be reused. It essentially
//! caches draws for ongoing use in the simulation related to cyber activities.
class RandomManager
{
public:
   //! @name DrawData container
   //! Basic data container providing a draw value and the sim time of the last draw.
   struct DrawData
   {
      double mDraw{-1.0};
      double mDrawTime{-1.0};
   };

   //! @name DrawCache container
   //! Cache data for a specific probability type of a specific named attack. Only a single
   //! value type will be used in any given DrawCache, corresponding to the frequency type dictated
   //! by user input.
   //! @note "cALWAYS" draws are not kept here, as they do not require data to be cached. Draws
   //! in the "cALWAYS" case will be provided at will, and can be queried locally on the
   //! WsfCyberEngagement object.
   struct DrawCache
   {
      double                          mSimulationDraw{-1.0};     //! cONCE_PER_SIMULATION
      std::map<std::string, double>   mTargetDraw{};             //! cONCE_PER_TARGET
      DrawData                        mSimulationIntervalDraw{}; //! cINTERVAL_PER_SIMULATION
      std::map<std::string, DrawData> mTargetIntervalDraw{};     //! cINTERVAL_PER_TARGET
   };

   //! A container for cached draw data, specific to each probability type available on a
   //! named attack.
   struct AttackDistributionCache
   {
      DrawCache mScanDetectionCache{};
      DrawCache mScanAttributionCache{};
      DrawCache mAttackSuccessCache{};
      DrawCache mStatusReportCache{};
      DrawCache mAttackDetectionCache{};
      DrawCache mAttackAttributionCache{};
      DrawCache mFutureImmunityCache{};
   };

   using AttackCacheMap = std::map<std::string, AttackDistributionCache>;

   RandomManager(WsfSimulation* aSimulation);

   //! @name Draw method
   //! Returns a continuous random variable draw for a named attack, against a specific target,
   //! for the required probability type. Queries the draw cache to see if a value is simply
   //! fetched and served from the cache, or a new draw is made (and cached for later use).
   //! Returns a value of -1.0 on an invalid passed parameter.
   double Draw(const std::string& aAttackType, const std::string& aTarget, ProbabilityType aType);

   const AttackCacheMap& GetCacheMap() const { return mAttackDrawCache; }

private:
   //! @name Private Draw methods for specific draw Frequency types.
   //@{
   //! @name cONCE_PER_SIMULATION draw method
   double pDraw(const std::string& aAttackType, ProbabilityType aType);

   //! @name cONCE_PER_TARGET draw method
   double pDraw(const std::string& aAttackType, const std::string& aTargetName, ProbabilityType aType);

   //! @name cINTERVAL_PER_SIMULATION draw method
   double pDraw(const std::string& aAttackType, ProbabilityType aType, WsfRandomVariable& aInterval);

   //! @name cINTERVAL_PER_TARGET draw method
   double pDraw(const std::string& aAttackType,
                const std::string& aTargetName,
                ProbabilityType    aType,
                WsfRandomVariable& aInterval);
   //@}

   //! @name A map caching draws to their associated cyber_attack.
   AttackCacheMap mAttackDrawCache{};
   WsfSimulation* mSimulation{nullptr};
};

} // namespace random
} // namespace cyber
} // namespace wsf

#endif
