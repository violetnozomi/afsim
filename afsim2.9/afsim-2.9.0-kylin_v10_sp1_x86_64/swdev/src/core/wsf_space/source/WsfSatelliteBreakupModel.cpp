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

#include "WsfSatelliteBreakupModel.hpp"

#include "UtCalendar.hpp"
#include "UtLog.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagator.hpp"
#include "WsfDateTime.hpp"
#include "WsfPlatform.hpp"
#include "WsfSpaceMover.hpp"
#include "WsfSpaceMoverBase.hpp"

//! Perform an explosive breakup of the given platform.
//!
//! This will perform the breakup of the platform with the given @p aPlatformName
//! according to the model represented by this object. If there is no such
//! platform at the time this is called, then this will have no effect.
//! The @p aParams are any parameters of the platform that are needed by
//! the model. This will generate a number of new platforms in the simulation
//! that this object is associated with during construction. Further, this will
//! remove the named platform from the simulation. If this object has previously
//! been used to successfully model a breakup, then this method will do
//! nothing, and return false.
//!
//! \param aSimTime      - the simulation time at which this method is being called.
//! \param aPlatformName - the name of the platform experiencing the breakup.
//! \param aParams       - the parameters associated with the platform needed by the model.
//! \returns             - true for a successful breakup; false otherwise.
bool WsfSatelliteBreakupModel::ExplosiveBreakup(double                     aSimTime,
                                                const std::string&         aPlatformName,
                                                const std::vector<double>& aParams)
{
   bool retval = !HasGenerated();
   if (retval)
   {
      WsfPlatform* platformPtr = GetSimulation().GetPlatformByName(aPlatformName);
      if (platformPtr)
      {
         platformPtr->Update(aSimTime);

         // NOTE: this relies on short-circuit evaluation.
         retval        = retval && ModelExplosion(*platformPtr, aParams);
         retval        = retval && AddFragmentsToSimulation(aSimTime);
         mHasGenerated = retval;
         if (retval)
         {
            RemoveProgenitor(aPlatformName, aSimTime);
         }
      }
      else
      {
         auto logger = ut::log::warning() << "Unable to find platform. No breakup will be performed.";
         logger.AddNote() << "Platform: " << aPlatformName;
         retval = false;
      }
   }
   else
   {
      ut::log::warning() << "This breakup model has already generated debris. No further breakup will be performed.";
   }
   return retval;
}

//! Perform a collisional breakup of the given platforms.
//!
//! This will perform the breakup of the two platforms due to a collision.
//! The arguments, @p aTargetPlatform and @p aImpactPlatform, specify the two
//! platforms involved in the collision, though the labeling of one as the
//! 'target' and the other as the 'Impactor' should not be taken to be
//! a requirement on the role of the two platforms. The model parameters for
//! the two platforms are given as @p aTargetParams and @p aImpactorParams.
//! If either platform does not exist, or if they are not near
//! to each other when this method is called, then this will do nothing and
//! return false. If the platforms exist, and they are close enough to be
//! in a collision, then this will produce a number of new platforms in the
//! simulation associated with this object when it was constructed. This will
//! also remove the given platforms. If this object has been previously used
//! to model a breakup, then this method will do nothing, and return false.
//!
//! \param aSimTime          - the simulation time of the
//! \param aTargetPlatform   - the name of one platform involved in the collision.
//! \param aTargetParams     - the model parameters for the target platform.
//! \param aImpactorPlatform - the name of the other platform involved in the collision.
//! \param aImpactorParams   - the model parameters for the impactor platform.
bool WsfSatelliteBreakupModel::CollisionalBreakup(double                     aSimTime,
                                                  const std::string&         aTargetPlatform,
                                                  const std::vector<double>& aTargetParams,
                                                  const std::string&         aImpactorPlatform,
                                                  const std::vector<double>& aImpactorParams)
{
   bool retval = !HasGenerated();
   if (retval)
   {
      WsfPlatform* targetPtr   = GetSimulation().GetPlatformByName(aTargetPlatform);
      WsfPlatform* impactorPtr = GetSimulation().GetPlatformByName(aImpactorPlatform);
      if (targetPtr && impactorPtr)
      {
         targetPtr->Update(aSimTime);
         impactorPtr->Update(aSimTime);

         retval        = retval && ModelCollision(*targetPtr, aTargetParams, *impactorPtr, aImpactorParams);
         retval        = retval && AddFragmentsToSimulation(aSimTime);
         mHasGenerated = retval;
         if (retval)
         {
            RemoveProgenitor(aTargetPlatform, aSimTime);
            RemoveProgenitor(aImpactorPlatform, aSimTime);
         }
      }
      else
      {
         if (!targetPtr)
         {
            auto logger = ut::log::warning() << "Unable to find platform. No breakup will be performed.";
            logger.AddNote() << "Platform: " << aTargetPlatform;
         }
         if (!impactorPtr)
         {
            auto logger = ut::log::warning() << "Unable to find platform. No breakup will be performed.";
            logger.AddNote() << "Platform: " << aImpactorPlatform;
         }
         retval = false;
      }
   }
   else
   {
      ut::log::warning() << "This breakup model has already generated debris. No further breakup will be performed.";
   }
   return retval;
}

void WsfSatelliteBreakupModel::SetFragmentPlatformType(const std::string& aPlatformType)
{
   WsfObject* typePtr = mScenario.FindType("platform_type", aPlatformType);
   if (typePtr)
   {
      mFragmentPlatformType = aPlatformType;
   }
   else
   {
      auto logger =
         ut::log::warning() << "Unable to find platform type. Reverting to the default platform type for fragments.";
      logger.AddNote() << "Platform: " << aPlatformType;
      mFragmentPlatformType.clear();
   }
}

std::unique_ptr<WsfPlatform> WsfSatelliteBreakupModel::GetDefaultFragmentPlatformTemplate() const
{
   std::unique_ptr<WsfPlatform> retvalPtr{dynamic_cast<WsfPlatform*>(mScenario.CloneType("platform_type", "WSF_PLATFORM"))};
   if (retvalPtr)
   {
      retvalPtr->SetIcon("wedge");
      WsfSpaceMover* moverPtr = new WsfSpaceMover{mScenario};
      static_cast<UtOrbitalPropagator&>(moverPtr->GetPropagator()).SetPrecession(true);
      retvalPtr->SetMover(moverPtr);
   }
   return retvalPtr;
}

//! Return the template platform for the fragments added by this object.
//!
//! If no fragment platform type is selected, this will return the default
//! platform type. If there is an issue finding the referenced platform
//! type, then this will return nullptr.
//!
//! \returns - A template platform to use for the generated fragments; nullptr if
//!            there is trouble generating the fragment platform template.
std::unique_ptr<WsfPlatform> WsfSatelliteBreakupModel::GetFragmentPlatformTemplate() const
{
   std::unique_ptr<WsfPlatform> retvalPtr{nullptr};

   if (mFragmentPlatformType.empty())
   {
      retvalPtr = GetDefaultFragmentPlatformTemplate();
   }
   else
   {
      retvalPtr.reset(dynamic_cast<WsfPlatform*>(mScenario.CloneType("platform_type", mFragmentPlatformType)));
   }

   return retvalPtr;
}

bool WsfSatelliteBreakupModel::AddFragment(std::unique_ptr<WsfPlatform> aPlatformPtr,
                                           const Fragment&              aFragment,
                                           const UtCalendar&            aEpoch,
                                           double                       aSimTime)
{
   bool retval{false};
   if (aPlatformPtr)
   {
      aPlatformPtr->SetName(aFragment.mName);
      aPlatformPtr->AssignToSimulation(&mSimulation);

      WsfSpaceMoverBase* moverPtr = dynamic_cast<WsfSpaceMoverBase*>(aPlatformPtr->GetMover());
      if (moverPtr)
      {
         ut::OrbitalState initialState(aEpoch,
                                       aPlatformPtr->GetSimulation()->GetEnvironment().GetCentralBody(),
                                       ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                       ut::OrbitalState::ReferenceFrame::cECI,
                                       ut::OrbitalState::Vector(aFragment.mLocationECI, aFragment.mVelocityECI));
         moverPtr->SetInitialOrbitalState(initialState);
         if (mSimulation.AddPlatform(aSimTime, aPlatformPtr.get()))
         {
            aPlatformPtr.release();
            retval = true;
         }
      }
   }
   return retval;
}

//! Add generated fragments to the simulation.
bool WsfSatelliteBreakupModel::AddFragmentsToSimulation(double aSimTime)
{
   auto templatePtr = GetFragmentPlatformTemplate();
   bool retval      = templatePtr != nullptr;
   if (templatePtr)
   {
      UtCalendar epoch{mSimulation.GetDateTime().GetStartDateAndTime()};
      epoch.AdvanceTimeBy(aSimTime);

      size_t count = GetFragmentCount();
      for (size_t i = 0; i < count; ++i)
      {
         Fragment fragment    = GetFragment(i);
         auto     instancePtr = ut::clone(templatePtr);
         if (!AddFragment(std::move(instancePtr), fragment, epoch, aSimTime))
         {
            retval = false;
            break;
         }
      }
   }
   return retval;
}

void WsfSatelliteBreakupModel::RemoveProgenitor(std::string aPlatformName, double aSimTime)
{
   WsfPlatform* platformPtr = mSimulation.GetPlatformByName(aPlatformName);
   if (platformPtr)
   {
      mSimulation.DeletePlatform(aSimTime, platformPtr);
   }
}
