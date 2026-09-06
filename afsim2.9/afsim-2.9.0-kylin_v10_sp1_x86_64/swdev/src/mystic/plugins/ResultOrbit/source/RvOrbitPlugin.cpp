// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvOrbitPlugin.hpp"

#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvSpaceEventPipeClasses.hpp"
#include "UtCalendar.hpp"
#include "UtMath.hpp"
#include "WkfScenario.hpp"
#include "WkfUtils.hpp"
#include "WkfVtkEnvironment.hpp"
#include "orbit/WkfAttachmentOrbit.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvOrbit::Plugin, "Orbit", "Draws satellite orbits", "mystic")

RvOrbit::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::PluginT<wkf::OrbitPluginBase>(aPluginName, aUniqueId)
{
}

void RvOrbit::Plugin::SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr)
{
   if (!aState)
   {
      wkf::AttachmentOrbit* orbit = aPlatformPtr->FindFirstAttachmentOfType<wkf::AttachmentOrbit>();
      if (orbit) // if there is an attachment
      {
         mOrbitIndex.erase(aPlatformPtr->GetIndex());
      }
   }
   wkf::OrbitPluginBase::SetPlatformOptionState(aOptionId, aState, aPlatformPtr);
}

void RvOrbit::Plugin::ResetOptionStates()
{
   wkf::OrbitPluginBase::ResetOptionStates();
   mOrbitIndex.clear();
}

void RvOrbit::Plugin::ClearScenario(bool aFullReset)
{
   mOrbitInterfacePtr->ClearAllAttachments();
   mOrbitIndex.clear();
}

void RvOrbit::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvOrbit::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   auto op = [this, &aData](unsigned int aIndex, wkf::AttachmentOrbit* aOrbitPtr)
   {
      std::string                   pname;
      rv::ResultPlatform*           p     = aData.GetDb()->FindPlatform(aIndex);
      const rv::MsgOrbitalElements* orbel = p->FindFirstBefore<rv::MsgOrbitalElements>(aData.GetSimTime());
      if (!orbel)
      {
         aOrbitPtr->Clear();
         mOrbitIndex[aIndex] = 0;
      }
      else
      {
         unsigned int index = orbel->GetMessageIndex();
         if (index < mOrbitIndex[aIndex])
         {
            aOrbitPtr->Clear();
         }
         if (index != mOrbitIndex[aIndex])
         {
            if (mOrbitInterfacePtr->IsScenarioColorMode())
            {
               rv::Vec3f orbitColor = orbel->orbitColor();
               UtColor   color(orbitColor.x(), orbitColor.y(), orbitColor.z());
               if (orbel->orbitColorValid())
               {
                  aOrbitPtr->SetDefinedColor(color);
               }
               else
               {
                  color = mOrbitInterfacePtr->GetTeamColor(aIndex);
               }
               aOrbitPtr->SetColor(color);
            }
            aOrbitPtr->Add(orbel->simTime(),
                           orbel->semiMajorAxis(),
                           orbel->eccentricity(),
                           orbel->raan(),
                           orbel->inclination(),
                           orbel->argumentOfPeriapsis(),
                           orbel->trueAnomaly(),
                           orbel->resultOfManeuver());
         }
         aOrbitPtr->UpdateTimeAngle(mOrbitInterfacePtr->GetAngleForTime(aData.GetSimTime()), aData.GetSimTime());
         mOrbitIndex[aIndex] = orbel->GetMessageIndex();
      }
   };
   // find latest orbits for each platform, pass them with time
   mOrbitInterfacePtr->ForEach(op);
   mOrbitInterfacePtr->UpdateMoonOrbit(aData.GetSimTime());
}

void RvOrbit::Plugin::SetEpoch(const UtCalendar& aEpoch)
{
   mOrbitInterfacePtr->SetStartDateTime(aEpoch);
   mOrbitInterfacePtr->AddMoonOrbit();
}
