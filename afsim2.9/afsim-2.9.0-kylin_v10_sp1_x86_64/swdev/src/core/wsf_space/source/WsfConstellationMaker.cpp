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

#include "WsfConstellationMaker.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "UtCalendar.hpp"
#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalElements.hpp"
#include "UtPath.hpp"
#include "WsfConstellationManager.hpp"
#include "WsfDateTime.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceConstellationConjunction.hpp"
#include "WsfSpaceConstellationGenerator.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "script/WsfScriptContext.hpp"

namespace // {anonymous}
{

// This class is a way to manage failures during the process of addition of the
// constellation members to the simulation. The addition of the constellation is
// all or nothing, so if the addition of one member fails, all members that have
// been successfully added need to be removed from the simulation. Errors in the
// process are reported back to the user via exceptions, and so this class
// uses RAII to remove tracked platforms from the simulation.
class AddedPlatformsRemover
{
public:
   AddedPlatformsRemover(WsfSimulation& aSimulation, double aCreationTime)
      : mSimulation(aSimulation)
      , mCreationTime{aCreationTime}
   {
   }

   ~AddedPlatformsRemover()
   {
      for (WsfPlatform* platformPtr : mTrackedPlatforms)
      {
         mSimulation.DeletePlatform(mCreationTime, platformPtr);
      }
   }

   void TrackPlatform(WsfPlatform* aPlatformPtr) { mTrackedPlatforms.push_back(aPlatformPtr); }

   void Release() { mTrackedPlatforms.clear(); }

private:
   WsfSimulation&            mSimulation;
   double                    mCreationTime;
   std::vector<WsfPlatform*> mTrackedPlatforms{};
};

} // namespace

//! Create a Walker Delta constellation maker.
//!
//! This method creates a constellation maker object that can be used to create constellations
//! with a Walker Delta design. If the @p aNumTotalSats is not evenly divisible by @p aNumPlanes,
//! or if @p aWalkerF is not less than @p aNumPlanes, then this will return a null object, and
//! will have the side-effect of printing out an error message indicating how the input arguments
//! are invalid.
//!
//! \param aNumTotalSats - The total number of satellites in the constellation.
//! \param aNumPlanes    - The number of orbital planes in the constellation.
//! \param aWalkerF      - The inter-plane phasing of the constellation.
//! \returns             - A new constellation maker object, or nullptr if there is a domain error.
std::unique_ptr<WsfConstellationMaker> WsfConstellationMaker::CreateWalkerDelta(unsigned int aNumTotalSats,
                                                                                unsigned int aNumPlanes,
                                                                                unsigned int aWalkerF)
{
   std::unique_ptr<WsfConstellationMaker> retval{nullptr};
   WsfConstellationOptions options = WsfConstellationOptions::CreateWalkerDelta(aNumTotalSats, aNumPlanes, aWalkerF);
   if (options.IsWalkerDelta())
   {
      retval.reset(new WsfConstellationMaker{options});
   }
   return retval;
}

//! Create a Walker Star constellation maker.
//!
//! This method creates a constellation maker object that can be used to create constellations
//! with a Walker Delta design. If the @p aNumTotalSats is not evenly divisible by @p aNumPlanes,
//! or if @p aWalkerF is not less than @p aNumPlanes, then this will return a null object, and
//! will have the side-effect of printing out an error message indicating how the input arguments
//! are invalid.
//!
//! \param aNumTotalSats - The total number of satellites in the constellation.
//! \param aNumPlanes    - The number of orbital planes in the constellation.
//! \param aWalkerF      - The inter-plane phasing of the constellation.
//! \returns             - A new constellation maker object, or nullptr is there is a domain error.
std::unique_ptr<WsfConstellationMaker> WsfConstellationMaker::CreateWalkerStar(unsigned int aNumTotalSats,
                                                                               unsigned int aNumPlanes,
                                                                               unsigned int aWalkerF)
{
   std::unique_ptr<WsfConstellationMaker> retval{nullptr};
   WsfConstellationOptions options = WsfConstellationOptions::CreateWalkerStar(aNumTotalSats, aNumPlanes, aWalkerF);
   if (options.IsWalkerStar())
   {
      retval.reset(new WsfConstellationMaker{options});
   }
   return retval;
}

//! Create a General design constellation maker.
//!
//! \param aNumPlanes       - The number of orbital planes in the constellation.
//! \param aNumSatsPerPlane - The number of members per plane.
//! \param aAnomalyAlias    - The inter-plane phasing.
//! \param aRAAN_Range      - The range of RAAN over which the orbital planes are distributed.
//! \returns                - A new constellation maker object.
std::unique_ptr<WsfConstellationMaker> WsfConstellationMaker::CreateGeneral(unsigned int aNumPlanes,
                                                                            unsigned int aNumSatsPerPlane,
                                                                            UtAngleValue aAnomalyAlias,
                                                                            UtAngleValue aRAAN_Range)
{
   std::unique_ptr<WsfConstellationMaker> retval{nullptr};
   WsfConstellationOptions                options =
      WsfConstellationOptions::CreateGeneral(aNumPlanes, aNumSatsPerPlane, aAnomalyAlias, aRAAN_Range);
   if (options.IsGeneral())
   {
      retval.reset(new WsfConstellationMaker{options});
   }
   return retval;
}

//! Render a constellation into an input file.
//!
//! This will create an AFSIM input file in which are placed the commands needed to
//! instantiate the constellation in a scenario. If there are any errors during this
//! operation, this will throw an runtime_error with details of the error.
void WsfConstellationMaker::WriteToFile() const
{
   std::string validation = mOptions.ValidateSetup();
   if (validation.empty())
   {
      auto fileName = GetGeneratedName();
      CreateContainingFolder();

      auto logger = ut::log::info() << "Writing constellation to file.";
      logger.AddNote() << "Constellation Name: " << mOptions.GetConstellationName();
      logger.AddNote() << "File: " << fileName;

      std::ofstream outFile{fileName};
      if (outFile.good())
      {
         wsf::space::ConstellationGenerator::Generate(mOptions, outFile);
         outFile.close();
      }
      else
      {
         std::stringstream err{};
         err << "Unable to open '" << fileName << "'.\n";
         throw std::runtime_error(err.str());
      }
   }
   else
   {
      throw std::runtime_error(validation);
   }
}

//! Create the constellation and add it to the simulation.
//!
//! This will create the constellation with options represented by this object. Before instantiating the platforms
//! that make up the constellation, the script @p aFilterScript is called for each member allowing individual
//! members of the constellation to be filtered out when adding the platforms to the simulation. This filtering
//! is not performed if @p aFilterScript is null. Once the platforms are instantiated and added to the simulation,
//! then the script @p aSetupScript is called to allow the user to make any changes to the platforms immediately
//! after their addition to the simulation. This setup operation is not performed if @p aSetupScript is null.
//! The constellation so created will be added to the collection maintained by the WsfConstellationManager.
//! Any errors in the process of instantiating the constellation will result in a std::runtime_error being
//! thrown, and any platforms that have been added to the simulation before the error will be removed from
//! the simulation.
//!
//! \param aCreationTime    - The time at which the platforms will be added to the simulation.
//! \param aSimulation      - The simulation to which the platforms will be added.
//! \param aContext         - The script context leading to this method invocation.
//! \param aFilterScriptPtr - A script used to filter members before adding the members to the simulation.
//! \param aSetupScriptPtr  - A script used to setup the members of the constellation after being added to the simulation.
WsfConstellation* WsfConstellationMaker::Create(double            aCreationTime,
                                                WsfSimulation&    aSimulation,
                                                WsfScriptContext& aContext,
                                                UtScript*         aFilterScriptPtr,
                                                UtScript*         aSetupScriptPtr) const
{
   WsfPlatform* platformType = CheckTypeExistence(aSimulation);
   CheckNameCollisions(aSimulation);
   WsfConstellation* constellationPtr =
      AddMembersToSimulation(aCreationTime, aSimulation, aContext, aFilterScriptPtr, platformType);
   SetupMembers(aContext, aSetupScriptPtr, aCreationTime, constellationPtr);
   return constellationPtr;
}

//! Detect if a constellation design will lead to a conjunction.
//!
//! This method will detect is a constellation design will lead to a conjunction. If this
//! constellation maker is not completely setup, then this will throw a runtime_error
//! with a message that indicates which parameters are not in their allowed range.
//!
//! \returns - true if the design would lead to a conjunction; false otherwise.
bool WsfConstellationMaker::DetectConjunction() const
{
   bool        retval{false};
   std::string validation = mOptions.ValidateSetup();
   if (validation.empty())
   {
      retval = wsf::space::ConstellationConjunction::Assess(mOptions.GetNumPlanes(),
                                                            mOptions.GetSatsPerPlane(),
                                                            mOptions.GetInclination().GetAsUnit(UtUnitAngle::cRADIANS),
                                                            mOptions.GetRAAN_Range().GetAsUnit(UtUnitAngle::cRADIANS),
                                                            mOptions.GetAnomalyAlias().GetAsUnit(UtUnitAngle::cRADIANS));
   }
   else
   {
      throw std::runtime_error(validation.c_str());
   }
   return retval;
}

std::string WsfConstellationMaker::GetGeneratedName() const
{
   std::stringstream name{};
   name << mOptions.GetGeneratedFilePrefix() << "_autogen.txt";
   return name.str();
}

void WsfConstellationMaker::CreateContainingFolder() const
{
   if (!mOptions.GetBasePath().Mkdir())
   {
      std::stringstream err{};
      err << "Unable to create folder '" << mOptions.GetBasePath().GetNormalizedPath() << "'.";
      throw std::runtime_error(err.str());
   }
}

WsfPlatform* WsfConstellationMaker::CheckTypeExistence(WsfSimulation& aSimulation) const
{
   WsfPlatform* typePtr =
      dynamic_cast<WsfPlatform*>(aSimulation.GetScenario().FindType("platform_type", mOptions.GetPlatformType()));
   if (typePtr == nullptr)
   {
      std::stringstream err{};
      err << "No platform type '" << mOptions.GetPlatformType() << "' defined.";
      throw std::runtime_error(err.str());
   }

   WsfSpaceMoverBase* moverPtr = dynamic_cast<WsfSpaceMoverBase*>(typePtr->GetMover());
   if (moverPtr == nullptr)
   {
      std::stringstream err{};
      err << "platform type '" << mOptions.GetPlatformType() << "' does not have a space mover.";
      throw std::runtime_error(err.str());
   }

   return typePtr;
}

void WsfConstellationMaker::CheckNameCollisions(const WsfSimulation& aSimulation) const
{
   // Make sure we are not trying to add a constellation with a repeat name.
   WsfConstellationManager& manager = WsfConstellationManager::Get(&aSimulation);
   if (manager.FindConstellation(GetConstellationName()))
   {
      std::stringstream err{};
      err << "A constellation with the name '" << GetConstellationName() << "' already exists.";
      throw std::runtime_error(err.str());
   }

   // Make sure all possible platforms added do not already exist.
   for (unsigned int plane = 0; plane < GetNumPlanes(); ++plane)
   {
      for (unsigned int sat = 0; sat < GetSatsPerPlane(); ++sat)
      {
         std::string name = mOptions.GetMemberName(plane, sat);
         if (aSimulation.GetPlatformByName(name))
         {
            std::stringstream err{};
            err << "A platform with the name '" << name << "' already exists.";
            throw std::runtime_error(err.str());
         }
      }
   }
}

bool WsfConstellationMaker::FilterMember(WsfScriptContext& aContext,
                                         UtScript*         aFilterScriptPtr,
                                         double            aSimTime,
                                         unsigned int      aPlane,
                                         unsigned int      aSatellite) const
{
   bool retval{false};
   if (aFilterScriptPtr)
   {
      UtScriptDataList scriptArgs{};
      scriptArgs.emplace_back(ut::safe_cast<int, unsigned int>(aPlane));
      scriptArgs.emplace_back(ut::safe_cast<int, unsigned int>(aSatellite));

      UtScriptData scriptRetval;
      aContext.ExecuteScript(aSimTime, aFilterScriptPtr, scriptRetval, scriptArgs);
      retval = scriptRetval.GetBool();
   }
   return retval;
}

WsfConstellation* WsfConstellationMaker::AddMembersToSimulation(double            aCreationTime,
                                                                WsfSimulation&    aSimulation,
                                                                WsfScriptContext& aContext,
                                                                UtScript*         aFilterScriptPtr,
                                                                WsfPlatform*      aPlatformType) const
{
   UtCalendar epoch = aSimulation.GetDateTime().GetStartDateAndTime();
   epoch.AdvanceTimeBy(aCreationTime);

   AddedPlatformsRemover remover{aSimulation, aCreationTime};
   for (unsigned int plane = 0; plane < GetNumPlanes(); ++plane)
   {
      for (unsigned int sat = 0; sat < GetSatsPerPlane(); ++sat)
      {
         if (!FilterMember(aContext, aFilterScriptPtr, aCreationTime, plane, sat))
         {
            auto memberPtr = std::unique_ptr<WsfPlatform>(aPlatformType->Clone());
            SetupMemberElements(memberPtr.get(), plane, sat, epoch);
            if (aSimulation.AddPlatform(aCreationTime, memberPtr.get()))
            {
               // The simulation now owns the platform, but we want to be able to
               // remove it from the simulation if something goes wrong with a later platform,
               // so we pass a handle to it to the AddedPlatformRemover.
               remover.TrackPlatform(memberPtr.release());
            }
            else
            {
               // memberPtr is still owned, so we do nothing with the smart pointer and
               // throw. The destructors of memberPtr and remover will clean up both
               // the allocated memberPtr that failed to add, and remove any platforms
               // added to the simulation.
               std::stringstream err{};
               err << "Unable to add member '" << memberPtr->GetName() << "'";
               throw std::runtime_error{err.str()};
            }
         }
      }
   }

   WsfConstellationManager& manager          = WsfConstellationManager::Get(&aSimulation);
   auto                     constellationPtr = ut::make_unique<WsfConstellation>(manager, mOptions);
   if (!manager.AddConstellation(std::move(constellationPtr)))
   {
      throw std::runtime_error{"Unable to add constellation to constellation manager."};
   }

   // If execution reaches this point, then all platforms have been successfully added,
   // so we need to release the tracking in remover, so that the added platforms are not
   // removed from the simulation when this function returns to the calling context.
   remover.Release();

   return manager.FindConstellation(GetConstellationName());
}

void WsfConstellationMaker::SetupMemberElements(WsfPlatform*      aMemberPtr,
                                                unsigned int      aPlane,
                                                unsigned int      aSat,
                                                const UtCalendar& aEpoch) const
{
   WsfSpaceMoverBase* moverPtr = dynamic_cast<WsfSpaceMoverBase*>(aMemberPtr->GetMover());
   if (moverPtr == nullptr)
   {
      throw std::runtime_error("Cloned member of constellation does not have the same mover type.");
   }

   // Change the mover's initial orbital state to include the specified orbital elements.
   UtOrbitalElements elements = moverPtr->GetInitialOrbitalState().GetOrbitalElements();
   elements.SetEccentricity(0.0);
   elements.SetSemiMajorAxis(mOptions.GetSemiMajorAxis().GetAsUnit(UtUnitLength::cMETERS));
   elements.SetInclination(mOptions.GetInclination().GetAsUnit(UtUnitAngle::cRADIANS));
   elements.SetRAAN(mOptions.GetMemberRAAN(aPlane).GetAsUnit(UtUnitAngle::cRADIANS));
   elements.SetTrueAnomaly(mOptions.GetMemberAnomaly(aPlane, aSat).GetAsUnit(UtUnitAngle::cRADIANS));
   elements.SetEpoch(aEpoch);
   ut::OrbitalState initialState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                 ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE,
                                 elements);
   moverPtr->SetInitialOrbitalState(initialState);
   aMemberPtr->SetName(mOptions.GetMemberName(aPlane, aSat));
}

void WsfConstellationMaker::SetupMembers(WsfScriptContext& aContext,
                                         UtScript*         aFilterScriptPtr,
                                         double            aSimTime,
                                         WsfConstellation* aConstellationPtr) const
{
   if (aConstellationPtr == nullptr || aFilterScriptPtr == nullptr)
   {
      return;
   }

   for (unsigned int plane = 0; plane < GetNumPlanes(); ++plane)
   {
      for (unsigned int sat = 0; sat < GetSatsPerPlane(); ++sat)
      {
         std::string  platformName = mOptions.GetMemberName(plane, sat);
         WsfPlatform* platformPtr  = aContext.GetSimulation()->GetPlatformByName(platformName);
         if (platformPtr)
         {
            UtScriptDataList scriptArgs;
            scriptArgs.emplace_back(new UtScriptRef(aConstellationPtr, aContext.GetClass("WsfConstellation")));
            scriptArgs.emplace_back(ut::safe_cast<int, unsigned int>(plane));
            scriptArgs.emplace_back(ut::safe_cast<int, unsigned int>(sat));
            scriptArgs.emplace_back(new UtScriptRef(platformPtr, aContext.GetClass("WsfPlatform")));

            UtScriptData scriptRetval;
            aContext.ExecuteScript(aSimTime, aFilterScriptPtr, scriptRetval, scriptArgs);
         }
      }
   }
}
