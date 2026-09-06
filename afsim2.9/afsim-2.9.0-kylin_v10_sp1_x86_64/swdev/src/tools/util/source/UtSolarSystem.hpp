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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTSOLARSYSTEM_HPP
#define UTSOLARSYSTEM_HPP

#include "ut_export.h"

#include <map>
#include <string>
#include <vector>

class UtCalendar;
#include "UtCloneablePtr.hpp"
class UtOrbitalElements;
#include "UtOrbitalPropagatorBase.hpp"
#include "UtOrbitalState.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"

//! A class that calculates the positions of solar system bodies.
//! Planets are explicitly supported.  One can also add other bodies
//! (asteroids, comets), if these are also of interest.  Currently only
//! sun-orbiting bodies and the Moon are supported, but this may be expanded to include
//! moons of other planets as basic orbital propagation
//! still applies in these cases.
class UT_EXPORT UtSolarSystem
{
public:
   //! A solar system body is any body that propagates according to a set of orbital elements
   //! and perturbations (deltas) to these elements.  The deltas are defined according to moments,
   //! such that the parameter "aDeltaOE_1" is the change with respect to time, "aDeltaOE_2" is the change
   //! with respect to the time squared, etc.
   //! reference (planets, asteroids, comets, etc.).
   class UT_EXPORT Body
   {
   public:
      Body() = default;

      virtual ~Body() = default;

      virtual Body* Clone() const = 0;

      virtual bool Initialize(const UtCalendar& aEpoch) { return true; }
      virtual void Update(const UtCalendar& aEpoch) = 0;

      virtual void GetLocationEcliptic(const UtCalendar& aEpoch, UtVec3d& aLocationEcliptic);

      void GetLocationECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI);

      void GetLocationRA_Dec(const UtCalendar& aEpoch, double& aRA, double& aDec);

      //! Return the orbital state associated with this body.
      virtual const ut::OrbitalState& GetOrbitalState() const = 0;

   protected:
      bool mInitialized{false};

   private:
   };

   UtSolarSystem();
   virtual ~UtSolarSystem() = default;

   class PlanetPropagator : public UtOrbitalPropagatorBase
   {
   public:
      PlanetPropagator(const ut::OrbitalState&  aOsculatingState,
                       const UtOrbitalElements& aDeltaOE_1,  // plus first second and third order deltas
                       const UtOrbitalElements& aDeltaOE_2,  // plus first second and third order deltas
                       const UtOrbitalElements& aDeltaOE_3); // plus first second and third order deltas

      PlanetPropagator(const ut::OrbitalState&  aOsculatingState,
                       const UtOrbitalElements& aDeltaOE_1); // plus first second and third order deltas

      UtOrbitalPropagatorBase* Clone() const override { return new PlanetPropagator(*this); }

      bool Initialize(const UtCalendar& aEpoch) override;
      using UtOrbitalPropagatorBase::Initialize;

      const ut::OrbitalState::Vector& GetOrbitalStateVectorEcliptic() const
      {
         return mPropagatedOrbitalState.GetOrbitalStateVector();
      }

   protected:
      void Propagate(const UtCalendar& aEpoch) override;
      void UpdateOrbitalState() override;
      bool HyperbolicPropagationAllowed() const override { return false; }

   private:
      std::vector<UtOrbitalElements> mDeltaOE_Vec;
      ut::OrbitalState               mPropagatedOrbitalState;
   };

   class Planet : public Body
   {
   public:
      enum
      {
         cMERCURY = 0,
         cVENUS,
         cEARTH,
         cMARS,
         cJUPITER,
         cSATURN,
         cURANUS,
         cNEPTUNE,
         cNUM_PLANETS
      };

      Planet(const ut::OrbitalState&  aOsculatingState,
             const UtOrbitalElements& aDeltaOE_1,  // plus first second and third order deltas
             const UtOrbitalElements& aDeltaOE_2,  // plus first second and third order deltas
             const UtOrbitalElements& aDeltaOE_3); // plus first second and third order deltas

      Planet(const ut::OrbitalState&  aOsculatingState,
             const UtOrbitalElements& aDeltaOE_1); // plus first second and third order deltas

      Planet(const Planet& aSrc) = default;
      ~Planet() override         = default;

      bool                    Initialize(const UtCalendar& aEpoch) override;
      void                    Update(const UtCalendar& aEpoch) override;
      Planet*                 Clone() const override { return new Planet(*this); }
      const ut::OrbitalState& GetOrbitalState() const override { return mPropagator.GetOrbitalState(); }
      void                    GetLocationEcliptic(const UtCalendar& aEpoch, UtVec3d& aLocationEcliptic) override;

   private:
      PlanetPropagator mPropagator; // to be transferred to the propagator before it is initialized.
   };

   Body* GetBody(const std::string& aBodyName);

   void AddBody(const std::string& aName, std::unique_ptr<Body> aBody);

   static std::unique_ptr<Planet> GetPlanet(int aPlanet);

   static const std::string& GetPlanetName(int aPlanet);

   bool GetLocationEcliptic(const std::string& aBodyName, const UtCalendar& aEpoch, UtVec3d& aLocationEcliptic);

   bool GetLocationECI(const std::string& aBodyName, const UtCalendar& aEpoch, UtVec3d& aLocationECI);

   static void GetMoonLocationECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI);

   static void GetMoonLocationVelocityECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI, UtVec3d& aVelocityECI);

   static void GetSunLocationECI(const UtCalendar& aEpoch, UtVec3d& aLocationECI);

   bool GetLocationRA_Dec(const std::string& aBodyName, const UtCalendar& aEpoch, double& aRA, double& aDec);

   static void ConvertEclipticToEquatorial(const UtCalendar& aEpoch,
                                           const UtVec3d&    aLocationEcliptic,
                                           UtVec3d&          aLocationEquatorial);

   static void ConvertEquatorialToEcliptic(const UtCalendar& aEpoch,
                                           const UtVec3d&    aLocationEquatorial,
                                           UtVec3d&          aLocationEcliptic);

protected:
   using NameToBodyMap = std::map<std::string, ut::CloneablePtr<Body>>;
   NameToBodyMap mNameToBodyMap;
};

#endif
