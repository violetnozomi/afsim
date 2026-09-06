// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SA_ASSESS_HPP
#define WSF_SA_ASSESS_HPP

#include "wsf_air_combat_export.h"

#include "UtMath.hpp"
#include "WsfSA_Module.hpp"
#include "WsfSA_Perceive.hpp"

class WsfAirMover;
class WsfFuel;
class WsfSA_EntityPerception;
class WsfSA_Group;
class WsfSA_Processor;
class WsfSA_PerceivedItem;

/** This is the assess module of the SA Processor */
class WSF_AIR_COMBAT_EXPORT WsfSA_Assess : WsfSA_Module
{
public:
   friend class WsfSA_Processor;

   WsfSA_Assess(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr);
   WsfSA_Assess& operator=(const WsfSA_Assess&) = delete;
   ~WsfSA_Assess();

   struct AssessEngagementData
   {
      size_t      targetIndex{0};
      std::string targetName{};
      std::string targetType{};
      float       angleOff_deg{0.0f};
      float       aspectAngle_deg{0.0f};
      float       currentRange_nm{0.0f};
      float       currentDeltaAlt_ft{0.0f};
      float       trackQuality{0.0f};
      std::string targetCID{};
      float       threatLevel{0.0f};
      float       targetValue{0.0f};
      float       detectionRange_nm{0.0f};
      float       wezDesiredRange_nm{0.0f};
      float       wezMinimum_nm{0.0f};
      float       wezMaximum_nm{0.0f};
      float       wezNoEscape_nm{0.0f};
      float       risk{0.0f};
      float       defensiveness{0.0f};
      float       urgency{0.0f};
      bool        tgtTracked{false};
      bool        acceptableWez{false};
   };

   /** Process input from script.
    * @param aInput input to parse.
    */
   bool ProcessInput(UtInput& aInput);

   /** Performs initialization during creation. This should be called
    * from the SA Processor's Initialize() function.
    * @param aSimTime sim time when processing
    */
   void Initialize(double aSimTime);

   /** Attempt to update engagement data at a sim time, and return an appropriate time for the next event update.
    * A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdateEngagements(double aSimTime);

   /** Attempt to update prioritized threat and target data at a sim time, and return an appropriate time for the next
    * event update.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdatePrioritizedItemData(double aSimTime);

   /** Attempt to update prioritized threat and target calculations at a sim time, and return an appropriate time for
    * the next event update. A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   double TryUpdatePrioritizedItemCalculation(double aSimTime);

   /** Update engagement data at a sim time, and return an appropriate time for the next event update.
    * A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateEngagements(double aSimTime);

   /** Update prioritized threat and target data at a sim time, and return an appropriate time for the next event update.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdatePrioritizedItemData(double aSimTime);

   /** Update prioritized threat and target calculations at a sim time, and return an appropriate time for the next
    * event update. A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdatePrioritizedItemCalculation(double aSimTime);

   /** Update engagement data at a sim time, and return an appropriate time for the next event update.
    * A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdateEngagementsData(double aSimTime);

   /** Update prioritized threat and target data at a sim time, and return an appropriate time for the next event
    * update. A cognitive delay is factored in.
    * @param aSimTime sim time at which to attempt updating
    */
   void UpdatePrioritizedThreatsAndTargetsData(double aSimTime);

   /** Update and assess the current perceived bogies and bandits groups for the current platform
    * @param aSimTime sim time at which to calculate
    */
   void AssessBogiesBandits(double aSimTime);

   /** Calculate the prioritized threats for the current platform
    * @param aSimTime sim time at which to calculate
    */
   void CalculatePrioritizedThreats(double aSimTime);

   /** Calculate the prioritized targets for the current platform
    * @param aSimTime sim time at which to calculate
    */
   void CalculatePrioritizedTargets(double aSimTime);

   /** This updates the weapon support parameters using an optional script function */
   void UpdateWeaponSupport(double aSimTime);

   /** Return the max prioritized threat count the platform can handle */
   unsigned int MaxPrioritizedThreats() { return mMaxPrioritizedThreats; }

   /** Set the prioritized threat maximum.
    * @param aMax number of threats a platform can prioritize (0 value means no limit)
    */
   void SetMaxPrioritizedThreats(unsigned int aMax) { mMaxPrioritizedThreats = aMax; }

   /** Set the max prioritized target to the value provided during initialization (default is 0 where not provided) */
   void ResetMaxPrioritizedThreats() { mMaxPrioritizedThreats = mMaxPrioritizedThreatsInitial; }

   /** Return the max prioritized target count the platform can handle */
   unsigned int MaxPrioritizedTargets() { return mMaxPrioritizedTargets; }

   /** Set the prioritized target maximum.
    * @param aMax number of targets a platform can prioritize (0 value means no limit)
    */
   void SetMaxPrioritizedTargets(unsigned int aMax) { mMaxPrioritizedTargets = aMax; }

   /** Set the max prioritized target to the value provided during initialization (default is 0 where not provided) */
   void ResetMaxPrioritizedTargets() { mMaxPrioritizedTargets = mMaxPrioritizedTargetsInitial; }

   // .......................................................................................................................

   // These functions support "angles" related calculations (angle off and aspect angle)
   // These functions support "angle off" related calculations

   /** This returns the 2D angle-off between the target and the platform (the difference in headings). This
    * calculates angle-off as the angle between the headings of the target and the platform. A positive
    * angle-off is when the difference in between the target heading and the platform heading is positive
    * and is less than or equal to Pi. If the difference is negative and is greater than negative Pi, the
    * angle-off is negative. The angle is in radians.
    * @param aPlatform platform to calculate 2d angle-off from.
    * @param aTargetPlatform platform to calculate 2d angle-off to.
    */
   static double AngleOffTarget2D(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform);

   /** This returns the 2D angle-off between the target and the platform (the difference in headings). This
    * calculates angle-off as the angle between the headings of the target and the platform. A positive
    * angle-off is when the difference in between the target heading and the platform heading is positive
    * and is less than or equal to Pi. If the difference is negative and is greater than negative Pi, the
    * angle-off is negative. The angle is in radians.
    * @param aTargetPlatform platform to calculate 2d angle-off to.
    */
   double AngleOffTarget2D(const WsfPlatform& aTargetPlatform) const;

   /** This returns the 3D angle-off between the target and the platform (the difference in heading/velocity).
    * This calculates angle-off as the angle between the velocity vectors of the target and the platform. A
    * positive angle-off is when the target is to the right of the platform. This is determined based on the
    * y-vector of the platform. If the dot-product of the target's velocity vector and the platform's
    * right side vector is positive, it is considered a positive angle-off. When the dot-product is negative,
    * the angle-off is a negative value. The angle is in radians.
    * @param aPlatform platform to calculate 3d angle-off from.
    * @param aTargetPlatform platform to calculate 3d angle-off to.
    */
   static double AngleOffTarget3D(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform);

   /** This returns the 3D angle-off between the target and the platform (the difference in heading/velocity).
    * This calculates angle-off as the angle between the velocity vectors of the target and the platform. A
    * positive angle-off is when the target is to the right of the platform. This is determined based on the
    * y-vector of the platform. If the dot-product of the target's velocity vector and the platform's
    * right side vector is positive, it is considered a positive angle-off. When the dot-product is negative,
    * the angle-off is a negative value. The angle is in radians.
    * @param aTargetPlatform platform to calculate 3d angle-off to.
    */
   double AngleOffTarget3D(const WsfPlatform& aTargetPlatform) const;

   // These functions support "aspect angle" related calculations

   /** This returns the aspect-angle for the target relative to the platform's position in 2D, meaning the aspect
    * is based on a horizontal plane that contains the target, with the platform's position projected onto
    * the plane. A positive angle is the "right side" of the target and a negative angle is "left side" of the
    * target. The angle is in radians.
    * @param aPlatform platform to calculate 2d aspect-angle from.
    * @param aTargetPlatform platform to calculate 2d aspect-angle to.
    */
   static double AspectAngleForTarget2D(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform);

   /** This returns the aspect-angle for the target relative to the platform's position in 2D, meaning the aspect
    * is based on a horizontal plane that contains the target, with the platform's position projected onto
    * the plane. A positive angle is the "right side" of the target and a negative angle is "left side" of the
    * target. The angle is in radians.
    * @param aTargetPlatform platform to calculate 2d aspect-angle to.
    */
   double AspectAngleForTarget2D(const WsfPlatform& aTargetPlatform) const;

   /** This returns the aspect-angle for the target relative to the platform's position in 3D, meaning the aspect
    * is based on the angle between a 3D vector opposite of the target's velocity vector and the vector from the
    * target to the platform's position. Right side (positive) and left side (negative) is determined based on
    * the y-vector of the target aircraft. If the vector dot-product of the target y-vector and the vector towards
    * the platform is positive, it is considered "right side". The angle is measured in radians.
    * @param aPlatform platform to calculate 3d aspect-angle from.
    * @param aTargetPlatform platform to calculate 3d aspect-angle to.
    */
   static double AspectAngleForTarget3D(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform);

   /** This returns the aspect-angle for the target relative to the platform's position in 3D, meaning the aspect
    * is based on the angle between a 3D vector opposite of the target's velocity vector and the vector from the
    * target to the platform's position. Right side (positive) and left side (negative) is determined based on
    * the y-vector of the target aircraft. If the vector dot-product of the target y-vector and the vector towards
    * the platform is positive, it is considered "right side". The angle is measured in radians.
    * @param aTargetPlatform platform to calculate 3d aspect-angle to.
    */
   double AspectAngleForTarget3D(const WsfPlatform& aTargetPlatform) const;

   /** This returns the off-boresight angle for the target relative to the platform's x-axis. It is a positive angle
    * between zero (directly in front of the nose) to Pi (directly behind the tail). The angle is measured in radians.
    * @param aPlatform platform to calculate off-boresight aspect-angle from.
    * @param aTargetPlatform platform to calculate angle towards.
    */
   static double OffBoresightAngleForTarget(const WsfPlatform& aPlatform, const WsfPlatform& aTargetPlatform);

   /** This returns the off-boresight angle for the target relative to the platform's x-axis. It is a positive angle
    * between zero (directly in front of the nose) to Pi (directly behind the tail). The angle is measured in radians.
    * @param aTargetPlatform platform to calculate 3d aspect-angle towards.
    */
   double OffBoresightAngleForTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the off-boresight angle for the target location relative to the platform's x-axis. It is a positive angle
    * between zero (directly in front of the nose) to Pi (directly behind the tail). The angle is measured in radians.
    * @param aLocationWCS location to calculate 3d aspect-angle towards.
    */
   double OffBoresightAngleForTarget(const double aLocationWCS[3]) const;

   // .......................................................................................................................

   // These functions support "angles" related calculations (angle off and aspect angle)
   // These functions support "detection" related calculations

   /** This returns the expected detection range (in meters) where the current platform could detect the target
    * platform, with the current relative aspects and states. This will consider all radars and IR sensors, and
    * select the sensor with the best range. If a sensor is turned off, it will not be considered. The function
    * will return zero if no viable sensors are available and operating.
    * @param aTargetPlatform target platform to get the detection range from the current platform.
    */
   double ExpectedDetectionRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected detection range (in meters) where the current platform could detect the
    * target platform, with the current relative aspects and states. This will consider all radars and
    * IR sensors, and select the sensor with the best range. If a sensor is turned off, it will not be
    * considered. The function will return zero if no viable sensors are available and operating.
    * @param aIndex index of the target platform to get the detection range from the current platform.
    */
   double ExpectedDetectionRangeAgainstTarget(size_t aIndex) const;

   /** This returns the expected detection range (in meters) where the target platform could detect the current
    * platform, with the current relative aspects and states. This will consider all radars and IR sensors,
    * and select the sensor with the best range. If a sensor is turned off, it will not be considered. The
    * function will return zero if no viable sensors are available and operating.
    * @param aTargetPlatform target platform to get the detection range to the current platform.
    */
   double ExpectedDetectionRangeByTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected detection range (in meters) where the target platform could detect the current
    * platform, with the current relative aspects and states. This will consider all radars and IR sensors, and
    * select the sensor with the best range. If a sensor is turned off, it will not be considered. The function
    * will return zero if no viable sensors are available and operating.
    * @param aIndex index of the target platform to get the detection range to the current platform.
    */
   double ExpectedDetectionRangeByTarget(size_t aIndex) const;

   /** This returns the expected detection range (in meters) where the current platform could detect the target
    * platform, with the current relative aspects and states. This will consider all radars and IR sensors, and
    * select the sensor with the best range. If a sensor is turned off, it will not be considered. The function
    * will return zero if no viable sensors are available and operating.
    * @param aSourePlatform platform trying to perform the detection.
    * @param aTargetPlatform target platform being detected.
    */
   double ExpectedDetectionRange(const WsfPlatform& aSourePlatform, const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected detection range against a given target platform as a normalized value between 0 and 1.
    * @param aTargetPlatform target platform to get the normalized detection range for.
    */
   double ExpectedNormalizedDetectionRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected detection range against a given target platform as a normalized value between 0 and 1.
    * @param aIndex index of the target platform to get the normalized detection range for.
    */
   double ExpectedNormalizedDetectionRangeAgainstTarget(size_t aIndex) const;

   // .......................................................................................................................

   // These functions support "WEZ" related calculations

   /** This returns the expected WEZ range against a given target platform as a normalized value between 0 and 1.
    * @param aTargetPlatform target platform to get the normalized WEZ range to the current platform.
    */
   double ExpectedNormalizedWezRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected WEZ range against a given target platform as a normalized value between 0 and 1.
    * @param aIndex index of the target platform to get the normalized WEZ range to the current platform.
    */
   double ExpectedNormalizedWezRangeAgainstTarget(size_t aIndex) const;

   /** This returns the expected WEZ range for the current platform against the target platform, with the current
    * relative aspects and states. The WEZ is for the currently selected weapon.
    * @param aTargetPlatform target platform to get the WEZ range to the current platform.
    */
   double ExpectedWezRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected WEZ range for the current platform against the target platform, with the current
    * relative aspects and states. The WEZ is for the currently selected weapon.
    * @param aIndex index of the target platform to get the WEZ range to the current platform.
    */
   double ExpectedWezRangeAgainstTarget(size_t aIndex) const;

   /** This returns the expected minimum engagement range for the current platform against the given target platform.
    * @param aTargetPlatform the target platform to calculate minimum engagement range against.
    */
   double ExpectedMinimumEngagementRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected minimum engagement range for the current platform against the given target platform.
    * @param aIndex index of the target platform to calculate minimum engagement range against.
    */
   double ExpectedMinimumEngagementRangeAgainstTarget(size_t aIndex) const;

   /** This returns the expected maximum engagement range for the current platform against the given target platform.
    * @param aTargetPlatform the target platform to calculate maximum engagement range against.
    */
   double ExpectedMaximumEngagementRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected maximum engagement range for the current platform against the given target platform.
    * @param aIndex index of the target platform to calculate maximum engagement range against.
    */
   double ExpectedMaximumEngagementRangeAgainstTarget(size_t aIndex) const;

   /** This returns the expected "no escape" engagement range for the current platform against the given target platform.
    * @param aTargetPlatform the target platform to calculate "no escape" engagement range against.
    */
   double ExpectedNoEscapeRangeAgainstTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected "no escape" engagement range for the current platform against the given target platform.
    * @param aIndex index of the target platform to calculate "no escape" engagement range against.
    */
   double ExpectedNoEscapeRangeAgainstTarget(size_t aIndex) const;

   /** This returns the expected WEZ range for the target platform against the current platform, with the current
    * relative aspects and states. The WEZ is for the currently selected weapon.
    * @param aTargetPlatform target platform to get the WEZ range from the current platform.
    */
   double ExpectedWezRangeByTarget(const WsfPlatform& aTargetPlatform) const;

   /** This returns the expected WEZ range for the target platform against the current platform, with the current
    * relative aspects and states. The WEZ is for the currently selected weapon.
    * @param aIndex index of the target platform to get the WEZ range from the current platform.
    */
   double ExpectedWezRangeByTarget(size_t aIndex) const;

   // .......................................................................................................................

   // These functions support range-related calculations

   /** This returns the range between the specified source platform and the target platform
    * @param aSourcePlatform source platform to calculate range from
    * @param aTargetPlatform target platform to calculate range to
    */
   double CalcSlantRange(const WsfPlatform& aSourcePlatform, const WsfPlatform& aTargetPlatform) const;

   /** This returns the range between the current platform and the specified target platform.
    * @param aTargetPlatform target platform to calculate range to.
    */
   double CalcSlantRange(const WsfPlatform& aTargetPlatform) const;

   // .......................................................................................................................

   // These functions support altitude-related calculations

   /** This returns the difference in altitude the specified source platform and the target platform. Positive delta
    * means the target platform is higher than the source platform.
    * @param aSourcePlatform source platform to calculate delta altitude from
    * @param aTargetPlatform target platform to calculate delta altitude to
    */
   double CalcDeltaAltitude(const WsfPlatform& aSourcePlatform, const WsfPlatform& aTargetPlatform) const;

   /** This returns the difference in altitude between the current platform and the specified target platform. Positive
    * delta means the target platform is higher than the source platform.
    * @param aTargetPlatform target platform to calculate delta altitude to
    */
   double CalcDeltaAltitude(const WsfPlatform& aTargetPlatform) const;

   // .......................................................................................................................

   // These functions support "weapons" related calculations

   /** A placeholder function that, in the future, will return whether a platform's weapon bay doors are open. */
   bool WeaponBayDoorsOpen() const { return false; }

   /** This returns the state of the weapon bay doors. It will return true if the doors are open. */
   bool GetWeaponBayOpen() { return mWeaponBayOpen; }

   /** This sets the state of the weapon bay doors. True indicates that the doors are open. */
   void SetWeaponBayOpen(bool aWeaponBayOpen) { mWeaponBayOpen = aWeaponBayOpen; }

   /** This returns the name of the current selected weapon. It will return an empty string if no weapon is selected. */
   std::string GetSelectedWeapon() { return mSelectedWeapon; }

   /** This sets the name of the current selected weapon. If no weapon is selected, an empty string should be used. */
   void SetSelectedWeapon(const std::string& aSelectedWeapon) { mSelectedWeapon = aSelectedWeapon; }

   // .......................................................................................................................

   // These functions support "speed" related calculations

   /** A placeholder function that, in the future, will return the minimum speed required to reach the platform's
    * maximum G Load.
    */
   double GetCornerSpeed_KCAS() const { return 0; }

   // .......................................................................................................................

   // These functions support "threat" related calculations

   /** Return a reference to the list of perceived items for the current
    * platform at a given time
    * @param aSimTime The sim time at which to get the list of perceived bogies
    */
   std::vector<WsfSA_PerceivedItem*> PerceivedThreatItems(double aSimTime);

   /** This returns a list of prioritized threat platform ids, beginning with the highest priority threat and working
    * down to the lowest priority threat
    */
   std::vector<size_t> PrioritizedThreatIds() const { return mPrioritizedThreatIds; }

   /** This returns a list of prioritized threat platform names, beginning with the highest priority threat and working
    * down to the lowest priority threat
    */
   std::vector<std::string> PrioritizedThreatNames() const { return mPrioritizedThreatNames; }

   /** This returns a const list of prioritized threat perceived items, beginning with the highest priority threat and
    * working down to the lowest priority threat
    */
   const std::list<UtWeakReference<WsfSA_PerceivedItem>>& GetPrioritizedThreatItems() const
   {
      return mPrioritizedThreatItems;
   }

   /** This returns a list of prioritized threat perceived items, beginning with the highest priority threat and working
    * down to the lowest priority threat
    */
   std::list<UtWeakReference<WsfSA_PerceivedItem>>& GetPrioritizedThreatItems() { return mPrioritizedThreatItems; }

   /** This returns a const list of prioritized threat perceived entities, beginning with the highest priority threat
    * and working down to the lowest priority threat
    */
   const std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedThreatEntities() const
   {
      return mPrioritizedThreatEntities;
   }

   /** This returns a list of prioritized threat perceived entities, beginning with the highest priority threat and
    * working down to the lowest priority threat
    */
   std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedThreatEntities()
   {
      return mPrioritizedThreatEntities;
   }

   /** This returns a const list of prioritized aircraft threat entity perceptions, beginning with the highest priority
    * threat and working down to the lowest priority threat
    */
   const std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedAircraftThreatEntities() const
   {
      return mPrioritizedAircraftThreatEntities;
   }

   /** This returns a list of prioritized aircraft threat entity perceptions, beginning with the highest priority threat
    * and working down to the lowest priority threat
    */
   std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedAircraftThreatEntities()
   {
      return mPrioritizedAircraftThreatEntities;
   }

   /** This returns a const list of prioritized missile threat entity perceptions, beginning with the highest priority
    * threat and working down to the lowest priority threat
    */
   const std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedMissileThreatEntities() const
   {
      return mPrioritizedMissileThreatEntities;
   }

   /** This returns a list of prioritized missile threat entity perceptions, beginning with the highest priority threat
    * and working down to the lowest priority threat
    */
   std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedMissileThreatEntities()
   {
      return mPrioritizedMissileThreatEntities;
   }

   /** This returns a const list of prioritized threat unfocused groups, beginning with the highest priority threat and
    * working down to the lowest priority threat
    */
   const std::list<UtWeakReference<WsfSA_Group>>& GetPrioritizedThreatGroups() const
   {
      return mPrioritizedThreatGroups;
   }

   /** This returns a list of prioritized threat unfocused groups, beginning with the highest priority threat and
    * working down to the lowest priority threat
    */
   std::list<UtWeakReference<WsfSA_Group>>& GetPrioritizedThreatGroups() { return mPrioritizedThreatGroups; }

   /** This returns a pointer to the highest threat or nullptr if no such threat exists.
    * @param aSimTime time at which to get entity
    */
   WsfSA_PerceivedItem* HighestThreat(double aSimTime);

   /** This returns a pointer to the highest entity threat or nullptr if no such threat exists.
    * @param aSimTime time at which to get entity
    */
   WsfSA_EntityPerception* HighestThreatEntity(double aSimTime);

   /** This returns a pointer to the highest aircraft threat or nullptr if no such threat exists.
    * @param aSimTime time at which to get entity
    */
   WsfSA_EntityPerception* HighestThreatAircraft(double aSimTime);

   /** This returns a pointer to the highest missile threat or nullptr if no such threat exists.
    * @param aSimTime time at which to get entity
    */
   WsfSA_EntityPerception* HighestThreatMissile(double aSimTime);

   /** This returns a pointer to the highest group threat or nullptr if no such threat exists.
    * @param aSimTime time at which to get entity
    */
   WsfSA_Group* HighestThreatGroup(double aSimTime);

   // .......................................................................................................................

   // These functions support "target" related calculations

   /** This returns a list of prioritized target platform ids, beginning with the highest priority target and working
    * down to the lowest priority target
    */
   std::vector<size_t> PrioritizedTargetIds() const { return mPrioritizedTargetIds; }

   /** This returns a list of prioritized target platform names, beginning with the highest priority target and working
    * down to the lowest priority target
    */
   std::vector<std::string> PrioritizedTargetNames() const { return mPrioritizedTargetNames; }

   /** This returns a const list of prioritized target perceived items, beginning with the highest priority target and
    * working down to the lowest priority target       */
   const std::list<UtWeakReference<WsfSA_PerceivedItem>>& GetPrioritizedTargetItems() const
   {
      return mPrioritizedTargetItems;
   }

   /** This returns a list of prioritized target perceived items, beginning with the highest priority target and working
    * down to the lowest priority target       */
   std::list<UtWeakReference<WsfSA_PerceivedItem>>& GetPrioritizedTargetItems() { return mPrioritizedTargetItems; }

   /** This returns a const list of prioritized target perceived entities, beginning with the highest priority target
    * and working down to the lowest priority target       */
   const std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedTargetEntities() const
   {
      return mPrioritizedTargetEntities;
   }

   /** This returns a list of prioritized target perceived entities, beginning with the highest priority target and
    * working down to the lowest priority target       */
   std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedTargetEntities()
   {
      return mPrioritizedTargetEntities;
   }

   /** This returns a const list of prioritized aircraft target entity perceptions, beginning with the highest priority
    * target and working down to the lowest priority target       */
   const std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedAircraftTargetEntities() const
   {
      return mPrioritizedAircraftTargetEntities;
   }

   /** This returns a list of prioritized aircraft target entity perceptions, beginning with the highest priority target
    * and working down to the lowest priority target       */
   std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedAircraftTargetEntities()
   {
      return mPrioritizedAircraftTargetEntities;
   }

   /** This returns a const list of prioritized missile target entity perceptions, beginning with the highest priority
    * target and working down to the lowest priority target       */
   const std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedMissileTargetEntities() const
   {
      return mPrioritizedMissileTargetEntities;
   }

   /** This returns a list of prioritized missile target entity perceptions, beginning with the highest priority target
    * and working down to the lowest priority target       */
   std::list<UtWeakReference<WsfSA_EntityPerception>>& GetPrioritizedMissileTargetEntities()
   {
      return mPrioritizedMissileTargetEntities;
   }

   /** This returns a const list of prioritized target unfocused groups, beginning with the highest priority target and
    * working down to the lowest priority target       */
   const std::list<UtWeakReference<WsfSA_Group>>& GetPrioritizedTargetGroups() const
   {
      return mPrioritizedTargetGroups;
   }

   /** This returns a list of prioritized target unfocused groups, beginning with the highest priority target and
    * working down to the lowest priority target       */
   std::list<UtWeakReference<WsfSA_Group>>& GetPrioritizedTargetGroups() { return mPrioritizedTargetGroups; }

   /** This returns a pointer to the best target or nullptr if no such target exists.
    * @param aSimTime time at which to get the item
    */
   WsfSA_PerceivedItem* BestTarget(double aSimTime);

   /** This returns a pointer to the best target entity or nullptr if no such target exists.
    * @param aSimTime time at which to get entity
    */
   WsfSA_EntityPerception* BestTargetEntity(double aSimTime);

   /** This returns a pointer to the best aircraft target or nullptr if no such target exists.
    * @param aSimTime time at which to get entity
    */
   WsfSA_EntityPerception* BestTargetAircraft(double aSimTime);

   /** This returns a pointer to the best missile target or nullptr if no such target exists.
    * @param aSimTime time at which to get entity
    */
   WsfSA_EntityPerception* BestTargetMissile(double aSimTime);

   /** This returns a pointer to the best group target or nullptr if no such target exists.
    * @param aSimTime time at which to get group
    */
   WsfSA_Group* BestTargetGroup(double aSimTime);

   // .......................................................................................................................

   // These functions support "track" related calculations

   /** This returns a list of platform ids that are currently being tracked */
   std::vector<std::size_t> CurrentlyTrackedPlatformIds() const { return mCurrentlyTrackedPlatformIds; }

   /** This returns a list of platform names that are currently being tracked */
   std::vector<std::string> CurrentlyTrackedPlatformNames() const { return mCurrentlyTrackedPlatformNames; }

   /** This returns true if the specified platform id is in the list of platforms that are currently being tracked */
   bool IsCurrentlyBeingTracked(size_t aPlatformId) const;

   // .......................................................................................................................

   // These functions support perceived groups

   /** This returns a list of SA_PerceivedGroups that are currently perceived and assessed */
   const std::list<WsfSA_Group*>& PerceivedGroups() const { return GetSAP()->GetData()->mCurrentlyPerceivedGroups; }

   /** This returns a list of WsfSA_Groups that are focused. */
   const std::list<WsfSA_Group*>& FocusedGroups() const { return mFocusedGroups; }

   /** This returns a list of WsfSA_Groups that are unfocused. */
   const std::list<WsfSA_Group*>& UnfocusedGroups() const { return mUnfocusedGroups; }

   /** Update mFocusedGroups and mUnfocusedGroups. */
   void UpdateGroupFocusLists();

   /** This returns all groups recognized by the SAP, sorted according to the given enumeration.
    * @param aSortingEnum  The enumeration which describes how the list should be sorted.
    * @param aGroups       The output variable. This should be supplied with an empty vector.
    */
   void SortedGroups(int aSortingEnum, std::vector<WsfSA_Group*>& aGroups) const;

   /** This returns a pointer to the group with the specified name or null, if no group exists.
    * @param aName name of the desired group.
    */
   WsfSA_Group* GetGroup(const std::string& aName) const;

   // .......................................................................................................................

   // These functions support situation assessment calculations

   /** This returns a string indicating the primary mission task */
   std::string MissionTask() const { return mMissionTask; }

   enum AcceptableRiskLevel
   {
      LOW    = 0,
      MEDIUM = 1,
      HIGH   = 2
   };

   /** This returns the acceptable level of risk, expressed as an enum */
   AcceptableRiskLevel AcceptableLevelOfRisk() const { return mAcceptableLevelOfRisk; }

   /** This returns the acceptable level of risk, expressed as an uint8_t */
   uint8_t AcceptableLevelOfRiskByte() const { return static_cast<uint8_t>(mAcceptableLevelOfRisk); }

   /** This returns a normalized risk level, ranging from 0 (no risk) to 1 (maximum risk) */
   double Risk() const { return mRisk; }

   /** This returns a normalized defensiveness level, ranging from 0 (fully non-defensive) to 1 (fully defensive) */
   double Defensiveness() const { return mDefensiveness; }

   /** This returns a normalized urgency level, ranging from 0 (no urgency) to 1 (maximum urgency) */
   double Urgency() const { return mUrgency; }

   /** This returns a normalized self-risk level, ranging from 0 (no risk) to 1 (maximum risk) */
   double SelfRisk() const { return mSelfRisk; }

   /** This returns a normalized flight-risk level, ranging from 0 (no risk) to 1 (maximum risk) */
   double FlightRisk() const { return mFlightRisk; }

   /** This returns a normalized package-risk level, ranging from 0 (no risk) to 1 (maximum risk) */
   double PackageRisk() const { return mPackageRisk; }

   /** This returns a normalized mission-risk level, ranging from 0 (no risk) to 1 (maximum risk) */
   double MissionRisk() const { return mMissionRisk; }

   /** This returns true if at least one weapon is being supported */
   bool SupportingWeapon() const { return mSupportingWeapon; }

   /** This returns true if missiles should be ignored as threats */
   bool IgnoreMissilesAsThreats() const { return mIgnoreMissilesAsThreats; }

   /** This returns true if bogies should be ignored as threats */
   bool IgnoreBogiesAsThreats() const { return mIgnoreBogiesAsThreats; }

   /** This returns true if missiles should be ignored as targets */
   bool IgnoreMissilesAsTargets() const { return mIgnoreMissilesAsTargets; }

   /** This returns true if bogies should be ignored as targets */
   bool IgnoreBogiesAsTargets() const { return mIgnoreBogiesAsTargets; }

   struct AircraftSignatureParameters
   {
      AircraftSignatureParameters()
         : fuselageFrontalArea(0.0)
         , wingArea(0.0)
         , tailArea(0.0)
         , engineInletArea(0.0)
         , engineExitArea(0.0)
         , sweepAngle(0.0)
         , signatureReductionFactorOverall(0.0)
         , signatureReductionFactorNose(0.0)
         , noseSignatureReductionAngle(0.0)

      {
      }
      AircraftSignatureParameters(double aFuselageFrontalArea,
                                  double aWingArea,
                                  double aTailArea,
                                  double aEngineInletArea,
                                  double aEngineExitArea,
                                  double aSweepAngle,
                                  double aSignatureReductionFactorOverall,
                                  double aSignatureReductionFactorNose,
                                  double aNoseSignatureReductionAngle)
         : fuselageFrontalArea(aFuselageFrontalArea)
         , wingArea(aWingArea)
         , tailArea(aTailArea)
         , engineInletArea(aEngineInletArea)
         , engineExitArea(aEngineExitArea)
         , sweepAngle(aSweepAngle)
         , signatureReductionFactorOverall(aSignatureReductionFactorOverall)
         , signatureReductionFactorNose(aSignatureReductionFactorNose)
         , noseSignatureReductionAngle(aNoseSignatureReductionAngle)
      {
      }
      double fuselageFrontalArea             = 0.0;
      double wingArea                        = 0.0;
      double tailArea                        = 0.0;
      double engineInletArea                 = 0.0;
      double engineExitArea                  = 0.0;
      double sweepAngle                      = 0.0;
      double signatureReductionFactorOverall = 0.0;
      double signatureReductionFactorNose    = 0.0;
      double noseSignatureReductionAngle     = 0.0;
   };

   // .......................................................................................................................

protected:
   WsfSA_Assess(const WsfSA_Assess&) = default;

   /** Calculate the currently tracked platforms for the current platform
    * @param aSimTime sim time at which to calculate
    */
   void CalculateTrackedPlatforms(double aSimTime);

   /** This returns true if the specified target platform index is in the list of currently tracked platforms
    * @param aTgtPlatformIndex target platform index to consider
    */
   bool PlatformIsCurrentlyBeingTracked(size_t aTgtPlatformIndex) const;

   /** This returns a pointer to the first track containing the specified target platform index,
    * or returns null. Note that this uses the perceived master track processor if available,
    * otherwise it uses the master track processor.
    * @param aTgtPlatformIndex target platform index to consider
    */
   WsfLocalTrack* GetPerceivedTrackedPlatformWithTarget(size_t aTgtPlatformIndex);

   /** Calculate the overall risk, self-risk, flight-risk, package-risk, and mission-risk level for the current
    * platform. Risks are normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    */
   void CalculateOverallRisks(double aSimTime);

   /** Calculate the overall defensiveness level for the current platform
    * Defensiveness is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    */
   void CalculateOverallDefensiveness(double aSimTime);

   /** Calculate the overall sense of urgency for the current platform
    * Urgency is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    */
   void CalculateOverallUrgency(double aSimTime);

   /** Calculate the threat level posed by the specified threat SA entity to the current platform.
    * Threat level is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aThreatItem threat SA entity to evaluate
    * @param aIsBogie true if entity is a bogie (this adjusts the threat level scoring)
    */
   double CalculateThreatLevel(double aSimTime, WsfSA_PerceivedItem* aThreatItem, bool aIsBogie);

   /** Calculate the target value presented by the specified threat SA entity to the current platform
    * Target value is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aTargetItem target SA entity to evaluate
    * @param aIsBogie true if entity is a bogie (this adjusts the target value scoring)
    */
   double CalculateTargetValue(double aSimTime, WsfSA_PerceivedItem* aTargetItem, bool aIsBogie);

   /** Calculate the threat level posed by the specified missile threat SA entity to the current platform.
    * Threat level is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aThreatItem threat SA entity to evaluate
    * @param aIsBogie true if entity is a bogie (this adjusts the threat level scoring)
    */
   double CalculateMissileThreatLevel(double aSimTime, WsfSA_EntityPerception* aThreatItem, bool aIsBogie);

   /** Calculate the target value presented by the specified missile SA entity to the current platform
    * Target value is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aTargetItem target SA entity to evaluate
    * @param aIsBogie true if entity is a bogie (this adjusts the target value scoring)
    */
   double CalculateMissileTargetValue(double aSimTime, WsfSA_EntityPerception* aTargetItem, bool aIsBogie);

   /** Calculate the risk posed by the specified threat SA entity to the current platform
    * Risk is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aThreatItem threat SA entity to evaluate
    */
   double CalculateRisk(double aSimTime, WsfSA_EntityPerception* aThreatItem);

   /** Calculate the defensiveness caused by the specified threat SA entity to the current platform.
    * Defensiveness is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aThreatItem threat SA entity to evaluate
    */
   double CalculateDefensiveness(double aSimTime, WsfSA_EntityPerception* aThreatItem);

   /** Calculate the urgency created by the specified threat SA entity to the current platform
    * Urgency is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aThreatItem threat SA entity to evaluate
    */
   double CalculateUrgency(double aSimTime, WsfSA_EntityPerception* aThreatItem);

   // This structure is used internally by threat and target scoring and prioritization
   struct PlatormScorePairing
   {
      PlatormScorePairing() {}
      explicit PlatormScorePairing(WsfPlatform* aPlatform) { platform = aPlatform; }
      PlatormScorePairing(WsfPlatform* aPlatform, double aScore)
      {
         platform = aPlatform;
         score    = aScore;
      }

      WsfPlatform* platform = nullptr;
      double       score    = 0.0;
   };

   static bool CompareScore(const PlatormScorePairing& first, const PlatormScorePairing& second);

   /** This will call an AFSIM script function "CalculateRisk". It returns the value, or if the function is not
     * present, it will return zero. Risk is normalized, and limited to 0.0 to 1.0.

     * @param aSimTime sim time at which to calculate
     */
   double ExecuteCalculateRisk(double aSimTime);

   /** This will call an AFSIM script function "CalculateSelfRisk". It returns the value, or if the function is not
    * present, it will return zero. Self risk is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * */
   double ExecuteCalculateSelfRisk(double aSimTime);

   /** This will call an AFSIM script function "CalculateFlightRisk". It returns the value, or if the function is not
    * present, it will return zero. Flight risk is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * */
   double ExecuteCalculateFlightRisk(double aSimTime);

   /** This will call an AFSIM script function "CalculatePackageRisk". It returns the value, or if the function is not
    * present, it will return zero. Package risk is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * */
   double ExecuteCalculatePackageRisk(double aSimTime);

   /** This will call an AFSIM script function "CalculateMissionRisk". It returns the value, or if the function is not
    * present, it will return zero. Mission risk is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * */
   double ExecuteCalculateMissionRisk(double aSimTime);

   /** This will call an AFSIM script function "CalculateDefensiveness". It returns the value, or if the function is not
    * present, it will return zero. Defensiveness is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * */
   double ExecuteCalculateDefensiveness(double aSimTime);

   /** This will call an AFSIM script function "CalculateUrgency". It returns the value, or if the function is not
    * present, it will return zero. Urgency is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * */
   double ExecuteCalculateUrgency(double aSimTime);

   /** This will call an AFSIM script function "CalculateThreatLevel". It returns the value, or if the function is not
    * present, it will return zero. Threat level is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aThreatItem entity being evaluated
    * @param aIsBogie true if this entity is a bogie
    * */
   double ExecuteCalculateEntityThreatLevel(double aSimTime, WsfSA_EntityPerception* aThreatItem, bool aIsBogie);

   /** This will call an AFSIM script function "CalculateTargetValue". It returns the value, or if the function is not
    * present, it will return zero. Target value is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aTargetItem entity being evaluated
    * @param aIsBogie true if this entity is a bogie
    * */
   double ExecuteCalculateEntityTargetValue(double aSimTime, WsfSA_EntityPerception* aTargetItem, bool aIsBogie);

   /** This will call an AFSIM script function "CalculateMissileThreatLevel". It returns the value, or if the function
    * is not present, it will return zero. Threat level is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aThreatItem entity being evaluated
    * @param aIsBogie true if this entity is a bogie
    * */
   double ExecuteCalculateMissileThreatLevel(double aSimTime, WsfSA_EntityPerception* aThreatItem, bool aIsBogie);

   /** This will call an AFSIM script function "CalculateMissileTargetValue". It returns the value, or if the function
    * is not present, it will return zero. Target value is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aTargetItem entity being evaluated
    * @param aIsBogie true if this entity is a bogie
    * */
   double ExecuteCalculateMissileTargetValue(double aSimTime, WsfSA_EntityPerception* aTargetItem, bool aIsBogie);

   /** This will call an AFSIM script function "CalculateGroupThreatLevel". It returns the value, or if the function is
    * not present, it will return zero. Threat level is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aThreatItem entity being evaluated
    * @param aIsBogie true if this entity is a bogie
    * */
   double ExecuteCalculateGroupThreatLevel(double aSimTime, WsfSA_Group* aThreatItem, bool aIsBogie);

   /** This will call an AFSIM script function "CalculateGroupTargetValue". It returns the value, or if the function is
    * not present, it will return zero. Target value is normalized, and limited to 0.0 to 1.0.
    * @param aSimTime sim time at which to calculate
    * @param aTargetItem entity being evaluated
    * @param aIsBogie true if this entity is a bogie
    * */
   double ExecuteCalculateGroupTargetValue(double aSimTime, WsfSA_Group* aTargetItem, bool aIsBogie);

   /** This will call an AFSIM script function "CalcWeaponSupport". It returns the value, or if the function is not
    * present, it will return false. * */
   bool ExecuteCalcWeaponSupport(double aSimTime);

private:
   struct MissilePkParameters
   {
      MissilePkParameters(double aNominalPk, double aHeadOnReduction, double aBroadsideReduction)
         : nominalPk(aNominalPk)
         , headOnReduction(aHeadOnReduction)
         , broadsideReduction(aBroadsideReduction)
      {
      }
      double nominalPk          = 0.0;
      double headOnReduction    = 0.0;
      double broadsideReduction = 0.0;
   };

   struct MissileWezParameters
   {
      MissileWezParameters(double                     aAvgSpeed,
                           double                     aMaxTimeFlight,
                           double                     aMaxOffBoresightAngle,
                           double                     aMinTimeFlight,
                           const MissilePkParameters& aPkData)
         : avgSpeed(aAvgSpeed)
         , maxTimeFlight(aMaxTimeFlight)
         , maxOffBoresightAngle(aMaxOffBoresightAngle)
         , minTimeFlight(aMinTimeFlight)
         , pkData(aPkData)
      {
      }
      MissileWezParameters(double aAvgSpeed,
                           double aMaxTimeFlight,
                           double aMaxOffBoresightAngle,
                           double aMinTimeFlight,
                           double aNominalPk,
                           double aHeadOnReduction,
                           double aBroadsideReduction)
         : avgSpeed(aAvgSpeed)
         , maxTimeFlight(aMaxTimeFlight)
         , maxOffBoresightAngle(aMaxOffBoresightAngle)
         , minTimeFlight(aMinTimeFlight)
         , pkData(aNominalPk, aHeadOnReduction, aBroadsideReduction)
      {
      }
      double              avgSpeed             = 0.0;
      double              maxTimeFlight        = 0.0;
      double              maxOffBoresightAngle = UtMath::cPI;
      double              minTimeFlight        = 0.0;
      MissilePkParameters pkData;
   };

   struct MissileRangeParameters
   {
      MissileRangeParameters()
         : range(0.0)
         , time(0.0)
         , pk(0.0)
      {
      }
      MissileRangeParameters(double aRange, double aTime, double aPk)
         : range(aRange)
         , time(aTime)
         , pk(aPk)
      {
      }
      double range = 0.0;
      double time  = 0.0;
      double pk    = 0.0;
   };

   /** This performs a simple WEZ calculation for a generic aircraft (assumed to be a fighter), providing the Rmax, Rne,
    * and Rmin ranges plus time of flight and Pk values for each. It returns true if the data is valid.
    * @param aTargetPlatform target platform being evaluated.
    * @param aRmax Rmax values for range, time, and Pk.
    * @param aRne Rne values for range, time, and Pk.
    * @param aRmin Rmin values for range, time, and Pk.
    * @param aCurrentRange current range to target.
    * @param aEngageable returns true if within engagement limits.
    */
   bool SimpleWezCalculationAircraft(const WsfPlatform&      aTargetPlatform,
                                     MissileRangeParameters& aRmax,
                                     MissileRangeParameters& aRne,
                                     MissileRangeParameters& aRmin,
                                     double&                 aCurrentRange,
                                     bool&                   aEngageable) const;

   /** This performs a simple WEZ calculation for a generic aircraft (assumed to be a fighter), providing the Rmax, Rne,
    * and Rmin ranges plus time of flight and Pk values for each. It returns true if the data is valid.
    * @param aAircraftPlatform aircraft platform launching weapons.
    * @param aTargetPlatform target platform being evaluated.
    * @param aRmax Rmax values for range, time, and Pk.
    * @param aRne Rne values for range, time, and Pk.
    * @param aRmin Rmin values for range, time, and Pk.
    * @param aCurrentRange current range to target.
    * @param aEngageable returns true if within engagement limits.
    */
   bool SimpleWezCalculationAircraft(const WsfPlatform&      aAircraftPlatform,
                                     const WsfPlatform&      aTargetPlatform,
                                     MissileRangeParameters& aRmax,
                                     MissileRangeParameters& aRne,
                                     MissileRangeParameters& aRmin,
                                     double&                 aCurrentRange,
                                     bool&                   aEngageable) const;

   /** This performs a simple WEZ calculation for a generic air-to-air missile, providing the Rmax, Rne, and Rmin ranges
    * plus time of flight and Pk values for each. It returns true if the data is valid.
    * @param aTargetPlatform target platform being evaluated.
    * @param aMissileParameters engagement parameters of the missile.
    * @param aRmax Rmax values for range, time, and Pk.
    * @param aRne Rne values for range, time, and Pk.
    * @param aRmin Rmin values for range, time, and Pk.
    * @param aCurrentRange current range to target.
    * @param aEngageable returns true if within engagement limits.
    */
   bool SimpleMissileWezCalculation(const WsfPlatform&          aTargetPlatform,
                                    const MissileWezParameters& aMissileParameters,
                                    MissileRangeParameters&     aRmax,
                                    MissileRangeParameters&     aRne,
                                    MissileRangeParameters&     aRmin,
                                    double&                     aCurrentRange,
                                    bool&                       aEngageable) const;

   /** This performs a simple WEZ calculation for a generic air-to-air missile, providing the Rmax, Rne, and Rmin
    * ranges plus time of flight and Pk values for each. It returns true if the data is valid.
    * @param aAircraftPlatform aircraft platform launching weapons.
    * @param aTargetPlatform target platform being evaluated.
    * @param aMissileParameters engagement parameters of the missile.
    * @param aRmax Rmax values for range, time, and Pk.
    * @param aRne Rne values for range, time, and Pk.
    * @param aRmin Rmin values for range, time, and Pk.
    * @param aCurrentRange current range to target.
    * @param aEngageable returns true if within engagement limits.
    */
   bool SimpleMissileWezCalculation(const WsfPlatform&          aAircraftPlatform,
                                    const WsfPlatform&          aTargetPlatform,
                                    const MissileWezParameters& aMissileParameters,
                                    MissileRangeParameters&     aRmax,
                                    MissileRangeParameters&     aRne,
                                    MissileRangeParameters&     aRmin,
                                    double&                     aCurrentRange,
                                    bool&                       aEngageable) const;

   /** This performs a WEZ lookup using the air-to-air launch computer for a given air-to-air missile, providing the
    * Rmax, Rne, and Rmin ranges plus time of flight and Pk values for each. It returns true if the data is valid.
    * @param aAircraftPlatform aircraft platform launching weapons.
    * @param aTargetPlatform target platform being evaluated.
    * @param aMissileParameters engagement parameters of the missile.
    * @param aRmax Rmax values for range, time, and Pk.
    * @param aRne Rne values for range, time, and Pk.
    * @param aRmin Rmin values for range, time, and Pk.
    * @param aCurrentRange current range to target.
    * @param aEngageable returns true if within engagement limits.
    */
   bool LaunchComputerMissileWezLookup(const WsfPlatform&          aAircraftPlatform,
                                       const WsfPlatform&          aTargetPlatform,
                                       const MissileWezParameters& aMissileParameters,
                                       MissileRangeParameters&     aRmax,
                                       MissileRangeParameters&     aRne,
                                       MissileRangeParameters&     aRmin,
                                       double&                     aCurrentRange,
                                       bool&                       aEngageable) const;

   /** This performs a simple WEZ calculation for a generic short-range air-to-air missile (SRM).
    * It returns true if an intercept can occur.
    * @param aMissilePlatform missile platform being evaluated.
    * @param aTargetPlatform target platform being evaluated.
    * @param aMissileParameters engagement parameters of the missile.
    * @param aRemainingRange remaining range of the weapon.
    * @param aRemainingTime estimated time to intercept.
    * @param aPk estimated Pk of the weapon.
    */
   bool SimpleMissileWezCalculationInflight(const WsfPlatform&          aMissilePlatform,
                                            WsfPlatform&                aTargetPlatform,
                                            const MissileWezParameters& aMissileParameters,
                                            double&                     aRemainingRange,
                                            double&                     aRemainingTime,
                                            double&                     aPk) const;

   /** This performs a simple Pk calculation for a generic missile.
    * It returns the Pk value based on the parameters.
    * @param aNominalPk baseline Pk for most favorable conditions.
    * @param aHeadOnReduction reduction factor (0.0 to 1.0) for head-on angles.
    * @param aBroadsideReduction reduction factor (0.0 to 1.0) for broadside angles.
    * @param aAngleOff angle off between missile and target.
    */
   double SimpleCalcPkBasedOnAngleOff(const MissilePkParameters& aParameters, double aAngleOff) const;

   /** This returns true if the selected weapon string is considered to be a medium-range missile.
    * It returns the Pk value based on the parameters.
    * @param aName name of the weapon
    */
   bool IsMediumRangeMissile(const std::string& aName) const;

   /** This returns true if the selected weapon string is considered to be a short-range missile.
    * It returns the Pk value based on the parameters.
    * @param aName name of the weapon
    */
   bool IsShortRangeMissile(const std::string& aName) const;

   /** This returns the cross section (in sq meters) for the specified target at the specified azimuth and elevation.
    * @param aTargetPlatform reference to the target platform.
    * @param aAzimuth azimuth angle (radians) from the target to the sensor.
    * @param aElevation elevation angle (radians) from the target to the sensor.
    */
   double TargetCrossSection(const WsfPlatform& aTargetPlatform, double aAzimuth, double aElevation) const;

   // Script-based Functions
   // ..........................................................................................................

   UtScript* mCalculateRiskScriptPtr{nullptr};          ///< Script for calculating overall risk
   UtScript* mCalculateSelfRiskScriptPtr{nullptr};      ///< Script for calculating self risk
   UtScript* mCalculateFlightRiskScriptPtr{nullptr};    ///< Script for calculating flight risk
   UtScript* mCalculatePackageRiskScriptPtr{nullptr};   ///< Script for calculating package risk
   UtScript* mCalculateMissionRiskScriptPtr{nullptr};   ///< Script for calculating mission risk
   UtScript* mCalculateDefensivenessScriptPtr{nullptr}; ///< Script for calculating defensiveness
   UtScript* mCalculateUrgencyScriptPtr{nullptr};       ///< Script for calculating urgency
   UtScript* mCalculateEntityThreatLevelScriptPtr{
      nullptr}; ///< Script for calculating threat level for a specified WsfSA_PerceivedItem
   UtScript* mCalculateEntityTargetValueScriptPtr{
      nullptr}; ///< Script for calculating target value for a specified WsfSA_PerceivedItem
   UtScript* mCalculateMissileThreatLevelScriptPtr{
      nullptr}; ///< Script for calculating missile threat level for a specified WsfSA_PerceivedItem
   UtScript* mCalculateMissileTargetValueScriptPtr{
      nullptr}; ///< Script for calculating missile target value for a specified WsfSA_PerceivedItem
   UtScript* mCalculateGroupThreatLevelScriptPtr{
      nullptr}; ///< Script for calculating threat level for a specified WsfSA_PerceivedItem
   UtScript* mCalculateGroupTargetValueScriptPtr{
      nullptr}; ///< Script for calculating target value for a specified WsfSA_PerceivedItem
   UtScript* mCalculateRiskPosedByEntityScriptPtr{
      nullptr}; ///< Script for calculating risk posed by a specified WsfSA_PerceivedItem
   UtScript* mCalculateDefensivenessInducedByEntityScriptPtr{
      nullptr}; ///< Script for calculating defensiveness induced by a specified WsfSA_PerceivedItem
   UtScript* mCalculateUrgencyInducedByEntityScriptPtr{
      nullptr}; ///< Script for calculating urgency induced by a specified WsfSA_PerceivedItem

   // Movers and fuel objects
   // ..........................................................................................................

   WsfP6DOF_Mover*      mP6DofMover{nullptr};   ///< Pointer to the P6DOF mover for the current platform
   wsf::six_dof::Mover* mSixDOF_Mover{nullptr}; ///< Pointer to the SixDOF mover for the current platform
   WsfBrawlerMover*     mBrawlerMover{nullptr}; ///< Pointer to the Brawler mover for the current platform
   WsfBrawlerFuel*      mBrawlerFuel{nullptr};  ///< Pointer to the Brawler fuel for the current platform
   WsfAirMover*         mAirMover{nullptr};     ///< Pointer to the AirMover for the current platform
   WsfFuel*             mAirMoverFuel{nullptr}; ///< Pointer to the AirMover fuel for the current platform

   // Weapon data
   // ......................................................................................................................

   UtScript*   mCalculateWeaponSupport{nullptr}; ///< Script for calculating weapon support
   std::string mSelectedWeapon;                  ///< Currently selected weapon name
   bool        mWeaponBayOpen{false};            ///< Weapon bay door status - true is open
   bool        mSupportingWeapon{false};         ///< Weapon support status - true if supporting at least one weapon

   // Perceived data and groups
   // ........................................................................................................

   const WsfSA_Processor::PerceiveData* mPerceivedDataSummary{
      nullptr}; ///< Pointer to the perceived data in the perceive module
   std::list<WsfSA_Group*> mFocusedGroups;
   std::list<WsfSA_Group*> mUnfocusedGroups;

   // Mission task, risk, defensiveness and urgency
   // ....................................................................................

   std::string         mMissionTask;                ///< Primary mission task
   AcceptableRiskLevel mAcceptableLevelOfRisk{LOW}; ///< Acceptable level of risk (enum)
   double              mRisk{0.0};                  ///< Normalized value for the overall risk of the current platform
   double              mDefensiveness{0.0};         ///< Normalized value for the defensiveness of the current platform
   double              mUrgency{0.0};               ///< Normalized value for the urgency of the current platform
   double              mSelfRisk{0.0};              ///< Normalized value for the self-risk of the current platform
   double              mFlightRisk{0.0};            ///< Normalized value for the flight-risk of the current platform
   double              mPackageRisk{0.0};           ///< Normalized value for the package-risk of the current platform
   double              mMissionRisk{0.0};           ///< Normalized value for the mission-risk of the current platform

   // Prioritized threats and targets
   // ..................................................................................................

   unsigned int mMaxPrioritizedThreats{0};        /// prioritized threat count limit (0 is no limit)
   unsigned int mMaxPrioritizedThreatsInitial{0}; /// prioritized threat count limit as initialized by user
   unsigned int mMaxPrioritizedTargets{0};        /// prioritized target count limit (0 is no limit)
   unsigned int mMaxPrioritizedTargetsInitial{0}; /// prioritized target count limit as initialized by user

   std::list<UtWeakReference<WsfSA_PerceivedItem>>    mPrioritizedThreatItems;    ///< List of prioritized threat items
   std::list<UtWeakReference<WsfSA_EntityPerception>> mPrioritizedThreatEntities; ///< List of prioritized threat entities
   std::list<UtWeakReference<WsfSA_EntityPerception>> mPrioritizedAircraftThreatEntities; ///< List of prioritized
                                                                                          ///< aircraft threat entities
   std::list<UtWeakReference<WsfSA_EntityPerception>> mPrioritizedMissileThreatEntities;  ///< List of prioritized
                                                                                          ///< missile threat entities
   std::list<UtWeakReference<WsfSA_Group>>            mPrioritizedThreatGroups;   ///< List of prioritized threat groups
   std::list<UtWeakReference<WsfSA_PerceivedItem>>    mPrioritizedTargetItems;    ///< List of prioritized target items
   std::list<UtWeakReference<WsfSA_EntityPerception>> mPrioritizedTargetEntities; ///< List of prioritized target entitie
   std::list<UtWeakReference<WsfSA_EntityPerception>> mPrioritizedAircraftTargetEntities; ///< List of prioritized
                                                                                          ///< aircraft target entities
   std::list<UtWeakReference<WsfSA_EntityPerception>> mPrioritizedMissileTargetEntities;  ///< List of prioritized
                                                                                          ///< missile target entities
   std::list<UtWeakReference<WsfSA_Group>> mPrioritizedTargetGroups; ///< List of prioritized target groups

   double mBogieThreatScoreMultiplier{1.0}; ///< Multiplier that is used when scoring bogies for threats
   double mBogieTargetScoreMultiplier{1.0}; ///< Multiplier that is used when scoring bogies for targets

   // Engagement data
   // ..................................................................................................................

   // Prioritized threats
   std::vector<size_t>      mPrioritizedThreatIds;   ///< List of prioritized threat ids
   std::vector<std::string> mPrioritizedThreatNames; ///< List of prioritized threat names

   // Prioritized targets
   std::vector<size_t>      mPrioritizedTargetIds;   ///< List of prioritized target ids
   std::vector<std::string> mPrioritizedTargetNames; ///< List of prioritized target names

   // Currently tracked platforms
   std::vector<size_t>      mCurrentlyTrackedPlatformIds;   ///< List of currently tracked platform ids
   std::vector<std::string> mCurrentlyTrackedPlatformNames; ///< List of currently tracked platform names

   // List of engagements
   std::vector<AssessEngagementData> mEngagementDataList; ///< List of engagement data

   // Temporary detection and WEZ range data
   // ...........................................................................................

   MissileWezParameters        mBlueMissileParametersMRM; ///< Blue MRM parameters
   MissileWezParameters        mBlueMissileParametersSRM; ///< Blue MRM parameters
   MissileWezParameters        mRedMissileParametersMRM;  ///< Red MRM parameters
   MissileWezParameters        mRedMissileParametersSRM;  ///< Red MRM parameters
   AircraftSignatureParameters mBlueAircraftSignature;    ///< Blue aircraft signature estimate
   AircraftSignatureParameters mRedAircraftSignature;     ///< Red aircraft signature estimate

   // Other data
   // .......................................................................................................................

   bool   mIgnoreMissilesAsThreats{false}; ///< Flag for testing missiles as threats (missiles are typically a threat)
   bool   mIgnoreBogiesAsThreats{false};   ///< Flag for testing bogies as threats (bogies are typically a threat)
   bool   mIgnoreMissilesAsTargets{true};  ///< Flag for testing missiles as targets (missiles are typically ignored)
   bool   mIgnoreBogiesAsTargets{true};    ///< Flag for testing bogies as targets (bogies are typically ignored)
   double mLastTimeCalculatedTrackedPlatforms{0.0}; ///< Last time tracked platforms were updated

   // ----------------------------------------------------------------------------------------------------------------------------------
};

#endif
