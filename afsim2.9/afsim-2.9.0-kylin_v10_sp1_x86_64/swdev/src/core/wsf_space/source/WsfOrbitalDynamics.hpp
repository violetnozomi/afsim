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

#ifndef WSFORBITALDYNAMICS_HPP
#define WSFORBITALDYNAMICS_HPP

#include <vector>

class UtCalendar;
#include "UtCloneablePtr.hpp"
#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
class UtInput;
#include "UtLLAPos.hpp"
#include "UtVec3.hpp"

class WsfIntegratingPropagator;
#include "WsfObject.hpp"
#include "WsfOrbitalDynamicsTerm.hpp"
class WsfScenario;
class WsfSimulation;

class WsfOrbitalDynamics : public WsfObject
{
public:
   explicit WsfOrbitalDynamics(const WsfScenario& aScenario);
   WsfOrbitalDynamics(const WsfOrbitalDynamics& aOther);
   ~WsfOrbitalDynamics() override = default;

   WsfOrbitalDynamics& operator=(const WsfOrbitalDynamics& aOther) = delete;

   WsfOrbitalDynamics* Clone() const override;
   bool                ProcessInput(UtInput& aInput) override;

   const char* GetScriptClassName() const override { return "WsfOrbitalDynamics"; }

   UtVec3d ComputeAcceleration(double aMass, const UtCalendar& aTime, const UtVec3d& aPosition, const UtVec3d& aVelocity) const;

   bool Initialize(const WsfSimulation*            aSimulationPtr,
                   const WsfIntegratingPropagator& aPropagator,
                   const UtCalendar&               aInitialTime);

   const WsfIntegratingPropagator* GetPropagator() const { return mPropagatorPtr; }

   UtVec3d  GetWCS_PositionFromECI(const UtVec3d& aPositionECI) const;
   UtVec3d  GetECI_AccelerationFromWCS(const UtVec3d& aAccelerationWCS) const;
   UtVec3d  GetWCS_VelocityFromECI(const UtVec3d& aPositionECI, const UtVec3d& aVelocityECI) const;
   UtLLAPos GetLLA_FromWCS(const UtVec3d& aPositionWCS) const;
   void     UpdateConverterTime(const UtCalendar& aTime) const;

   size_t GetNumTerms() const { return mTerms.size(); }

   void                          AddTerm(std::unique_ptr<WsfOrbitalDynamicsTerm> aTermPtr);
   const WsfOrbitalDynamicsTerm* GetTerm(size_t aIndex) const;

   template<class T>
   std::vector<T*> GetTerms() const
   {
      std::vector<T*> terms;
      for (const auto& term : mTerms)
      {
         T* termT = dynamic_cast<T*>(term.get());
         if (termT)
         {
            terms.push_back(termT);
         }
      }
      return terms;
   }

   bool RemoveTerm(size_t aIndex);

   void GetTimeConstants(UtCalendar& aTime) const;

   const WsfScenario&   GetScenario() const { return mScenario; }
   const WsfSimulation* GetSimulation() const { return mSimulationPtr; }

protected:
   mutable UtECI_Conversion mConverter{UtCalendar(), ut::EarthEGM96()};
   mutable UtCalendar       mInitialEpoch{};
   mutable bool             mEpochSet{false};

private:
   const WsfScenario&                                  mScenario;
   const WsfSimulation*                                mSimulationPtr{nullptr};
   std::vector<UtCloneablePtr<WsfOrbitalDynamicsTerm>> mTerms{};
   const WsfIntegratingPropagator*                     mPropagatorPtr{nullptr};

   // These are set during Initialize so that terms needing accurate timing
   // can have a UtCalendar's time constants updated if it matters.
   double mDeltaUT1{0.0};
   int    mDeltaAT{32};
};

#endif // WSFORBITALDYNAMICS_HPP
