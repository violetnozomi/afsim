// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPACEEVENTRESULTS_HPP
#define WSFSPACEEVENTRESULTS_HPP

#include "wsf_space_export.h"

#include <ostream>

class UtOrbitalPropagatorBase;

#include "WsfEventResult.hpp"
class WsfLocalTrack;
class WsfOrbitalEvent;
class WsfPlatform;
class WsfSimulation;
class WsfSpaceMoverBase;

namespace wsf
{
namespace event
{
// ===================================================================================================
class WSF_SPACE_EXPORT EclipseEntry : public Result
{
public:
   static constexpr const char* cNAME = "ECLIPSE_ENTRY";
   EclipseEntry(double aSimTime, WsfSpaceMoverBase* aSpaceMoverPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSpaceMoverPtr(aSpaceMoverPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSpaceMoverBase* GetSpaceMover() const { return mSpaceMoverPtr; }

private:
   WsfSpaceMoverBase* mSpaceMoverPtr;
};

// ===================================================================================================
class WSF_SPACE_EXPORT EclipseExit : public Result
{
public:
   static constexpr const char* cNAME = "ECLIPSE_EXIT";
   EclipseExit(double aSimTime, WsfSpaceMoverBase* aSpaceMoverPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSpaceMoverPtr(aSpaceMoverPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSpaceMoverBase* GetSpaceMover() const { return mSpaceMoverPtr; }

private:
   WsfSpaceMoverBase* mSpaceMoverPtr;
};

// ===================================================================================================
class WSF_SPACE_EXPORT OrbitalManeuverInitiated : public Result
{
public:
   static constexpr const char* cNAME = "ORBITAL_MANEUVER_INITIATED";
   OrbitalManeuverInitiated(double                 aSimTime,
                            WsfSpaceMoverBase*     aSpaceMoverPtr,
                            const WsfOrbitalEvent& aManeuver,
                            Settings               aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSpaceMoverPtr(aSpaceMoverPtr)
      , mManeuver(aManeuver)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSpaceMoverBase* GetSpaceMover() const { return mSpaceMoverPtr; }
   const WsfOrbitalEvent&   GetManeuver() const { return mManeuver; }

private:
   WsfSpaceMoverBase*     mSpaceMoverPtr;
   const WsfOrbitalEvent& mManeuver;
};

// ===================================================================================================
class WSF_SPACE_EXPORT OrbitalManeuverUpdated : public Result
{
public:
   static constexpr const char* cNAME = "ORBITAL_MANEUVER_UPDATED";
   OrbitalManeuverUpdated(double                 aSimTime,
                          WsfSpaceMoverBase*     aSpaceMoverPtr,
                          const WsfOrbitalEvent& aManeuver,
                          Settings               aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSpaceMoverPtr(aSpaceMoverPtr)
      , mManeuver(aManeuver)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSpaceMoverBase* GetSpaceMover() const { return mSpaceMoverPtr; }
   const WsfOrbitalEvent&   GetManeuver() const { return mManeuver; }

private:
   WsfSpaceMoverBase*     mSpaceMoverPtr;
   const WsfOrbitalEvent& mManeuver;
};

// ===================================================================================================
class WSF_SPACE_EXPORT OrbitalManeuverCanceled : public Result
{
public:
   static constexpr const char* cNAME = "ORBITAL_MANEUVER_CANCELED";
   OrbitalManeuverCanceled(double                 aSimTime,
                           WsfSpaceMoverBase*     aSpaceMoverPtr,
                           const WsfOrbitalEvent& aManeuver,
                           Settings               aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSpaceMoverPtr(aSpaceMoverPtr)
      , mManeuver(aManeuver)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSpaceMoverBase* GetSpaceMover() const { return mSpaceMoverPtr; }
   const WsfOrbitalEvent&   GetManeuver() const { return mManeuver; }

private:
   WsfSpaceMoverBase*     mSpaceMoverPtr;
   const WsfOrbitalEvent& mManeuver;
};

// ===================================================================================================
class WSF_SPACE_EXPORT OrbitalManeuverCompleted : public Result
{
public:
   static constexpr const char* cNAME = "ORBITAL_MANEUVER_COMPLETED";
   OrbitalManeuverCompleted(double                 aSimTime,
                            WsfSpaceMoverBase*     aSpaceMoverPtr,
                            const WsfOrbitalEvent& aManeuver,
                            Settings               aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSpaceMoverPtr(aSpaceMoverPtr)
      , mManeuver(aManeuver)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSpaceMoverBase* GetSpaceMover() const { return mSpaceMoverPtr; }
   const WsfOrbitalEvent&   GetManeuver() const { return mManeuver; }

private:
   WsfSpaceMoverBase*     mSpaceMoverPtr;
   const WsfOrbitalEvent& mManeuver;
};

// ===================================================================================================
class WSF_SPACE_EXPORT OrbitDeterminationInitiated : public Result
{
public:
   static constexpr const char* cNAME = "ORBITAL_DETERMINATION_INITIATED";
   OrbitDeterminationInitiated(double                         aSimTime,
                               WsfPlatform*                   aPlatformPtr,
                               const WsfLocalTrack&           aLocalTrack,
                               const UtOrbitalPropagatorBase& aPropagator,
                               Settings                       aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mLocalTrack(aLocalTrack)
      , mPropagator(aPropagator)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform*             GetPlatform() const { return mPlatformPtr; }
   const WsfLocalTrack&           GetLocalTrack() const { return mLocalTrack; }
   const UtOrbitalPropagatorBase& GetPropagator() const { return mPropagator; }

private:
   WsfPlatform*                   mPlatformPtr;
   const WsfLocalTrack&           mLocalTrack;
   const UtOrbitalPropagatorBase& mPropagator;
};

// ===================================================================================================
class WSF_SPACE_EXPORT OrbitDeterminationUpdated : public Result
{
public:
   static constexpr const char* cNAME = "ORBITAL_DETERMINATION_UPDATED";
   OrbitDeterminationUpdated(double                         aSimTime,
                             WsfPlatform*                   aPlatformPtr,
                             const WsfLocalTrack&           aLocalTrack,
                             const UtOrbitalPropagatorBase& aPropagator,
                             Settings                       aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mLocalTrack(aLocalTrack)
      , mPropagator(aPropagator)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform*             GetPlatform() const { return mPlatformPtr; }
   const WsfLocalTrack&           GetLocalTrack() const { return mLocalTrack; }
   const UtOrbitalPropagatorBase& GetPropagator() const { return mPropagator; }

private:
   WsfPlatform*                   mPlatformPtr;
   const WsfLocalTrack&           mLocalTrack;
   const UtOrbitalPropagatorBase& mPropagator;
};

} // namespace event
} // namespace wsf

#endif
