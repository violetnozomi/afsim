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

#include "WkfOrbitInterface.hpp"

#include "UtMath.hpp"
#include "UtMoon.hpp"
#include "UtSolarSystem.hpp"
#include "VaObserver.hpp"
#include "VaUtils.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfUtils.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::OrbitInterface::OrbitInterface(QObject*                  aParentPtr,
                                    wkf::OrbitPrefWidget*     aOrbitPrefWidgetPtr,
                                    wkf::MoonOrbitPrefWidget* aMoonOrbitPrefWidgetPtr)
   : QObject{aParentPtr}
   , mOrbitPrefWidgetPtr{aOrbitPrefWidgetPtr}
   , mMoonOrbitPrefWidgetPtr(aMoonOrbitPrefWidgetPtr)
{
   mMoonOrbit.second = nullptr;
   mCallbacks.Add(vespa::VaObserver::EntityDeleted.Connect(&OrbitInterface::EntityDeletedCB, this));
}

void wkf::OrbitInterface::FadeoutChanged(int aFadeout)
{
   for (auto& it : mOrbitMap)
   {
      it.second->SetFadeout(aFadeout);
   }
}

void wkf::OrbitInterface::ColorModeChanged(wkf::OrbitPrefData::ColorMode aColorMode)
{
   mColorMode = aColorMode;

   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      for (auto& it : mOrbitMap)
      {
         wkf::Platform* plat = scenarioPtr->FindPlatformByIndex(it.first);
         UtColor        color(1.0f, 1.0f, 1.0f, 1.0f);

         if (aColorMode == wkf::OrbitPrefData::eTEAM)
         {
            color = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(plat->GetSide());
         }
         else if (aColorMode == wkf::OrbitPrefData::eNAMEHASH)
         {
            color = wkf::util::GetHashUtColor(plat->GetName());
         }
         else if (aColorMode == wkf::OrbitPrefData::eSCENARIO)
         {
            if (it.second->ColorIsDefined())
            {
               color = it.second->GetDefinedColor();
            }
            else
            {
               color = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(plat->GetSide());
            }
         }
         it.second->SetColor(color);
      }
   }
}

void wkf::OrbitInterface::LineWidthChanged(int aLineWidth)
{
   for (auto& it : mOrbitMap)
   {
      it.second->SetLineWidth(aLineWidth);
   }
}

void wkf::OrbitInterface::PeriodsChanged(unsigned int aPeriods)
{
   for (auto& it : mOrbitMap)
   {
      it.second->SetPeriods(aPeriods);
   }
}

void wkf::OrbitInterface::TeamColorsChanged()
{
   if (mColorMode == wkf::OrbitPrefData::eTEAM || mColorMode == wkf::OrbitPrefData::eSCENARIO)
   {
      ColorModeChanged(mColorMode);
   }
}

UtColor wkf::OrbitInterface::GetTeamColor(size_t aPlatformIndex) const
{
   UtColor teamColor{};

   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      auto platformPtr = scenarioPtr->FindPlatformByIndex(aPlatformIndex);
      if (platformPtr)
      {
         teamColor = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(platformPtr->GetSide());
      }
   }
   return teamColor;
}

bool wkf::OrbitInterface::HasAttachment(unsigned int aPlatformIndex) const
{
   return mOrbitMap.find(aPlatformIndex) != mOrbitMap.end();
}

void wkf::OrbitInterface::ClearAllAttachments()
{
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      for (auto& it : mOrbitMap)
      {
         wkf::Platform* plat = scenarioPtr->FindPlatformByIndex(it.first);
         plat->RemoveAttachment(it.second->GetUniqueId());
      }
   }
   mOrbitMap.clear();
}

wkf::AttachmentOrbit* wkf::OrbitInterface::GetAttachment(unsigned int aPlatformId)
{
   wkf::Scenario* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      wkf::Platform* platformPtr = scenarioPtr->FindPlatformByIndex(aPlatformId);
      if (platformPtr)
      {
         return platformPtr->FindFirstAttachmentOfType<wkf::AttachmentOrbit>();
      }
   }
   return nullptr;
}

wkf::AttachmentOrbit* wkf::OrbitInterface::GetOrAddAttachment(unsigned int aPlatformId)
{
   wkf::Scenario* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      wkf::Platform* platformPtr = scenarioPtr->FindPlatformByIndex(aPlatformId);
      if (platformPtr)
      {
         wkf::AttachmentOrbit* orbitPtr = platformPtr->FindFirstAttachmentOfType<wkf::AttachmentOrbit>();
         if (!orbitPtr)
         {
            return AddAttachment(platformPtr);
         }
      }
   }
   return nullptr;
}

wkf::AttachmentOrbit* wkf::OrbitInterface::AddAttachment(wkf::Platform* aPlatformPtr)
{
   auto retvalPtr = vespa::make_attachment<wkf::AttachmentOrbit>(*aPlatformPtr, vaEnv.GetStandardViewer());
   retvalPtr->SetFadeout(mOrbitPrefWidgetPtr->GetPreferenceObject()->GetFadeout());
   retvalPtr->SetPeriods(mOrbitPrefWidgetPtr->GetPreferenceObject()->GetPeriods());
   UtColor color{1.0f, 1.0f, 1.0f, 1.0f};

   if (mColorMode == wkf::OrbitPrefData::eTEAM || mColorMode == wkf::OrbitPrefData::eSCENARIO)
   {
      color = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(aPlatformPtr->GetSide());
   }
   else if (mColorMode == wkf::OrbitPrefData::eNAMEHASH)
   {
      color = wkf::util::GetHashUtColor(aPlatformPtr->GetName());
   }
   retvalPtr->SetColor(color);
   retvalPtr->SetLineWidth(mOrbitPrefWidgetPtr->GetPreferenceObject()->GetLineWidth());
   mOrbitMap[aPlatformPtr->GetIndex()] = retvalPtr;
   return retvalPtr;
}

void wkf::OrbitInterface::RemoveAttachment(unsigned int aPlatformIndex)
{
   mOrbitMap.erase(aPlatformIndex);
}

void wkf::OrbitInterface::AddMoonOrbit(double aSimTime)
{
   auto  prefObject  = mMoonOrbitPrefWidgetPtr->GetPreferenceObject();
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr && prefObject->GetVisibility())
   {
      RemoveMoonOrbit();
      if (!mMoonOrbit.first)
      {
         mMoonOrbit.first = ut::make_unique<vespa::VaEntity>("moon_anchor");
         // we initialize rather than adding to the scenario,
         // since we are really just using the entity as an anchor point
         mMoonOrbit.first->Initialize();
      }
      mMoonOrbit.second = vespa::make_attachment<wkf::AttachmentOrbit>(*mMoonOrbit.first, vaEnv.GetStandardViewer(), true);
      QColor  qColor    = prefObject->GetColor();
      UtColor color(qColor.redF(), qColor.greenF(), qColor.blueF(), qColor.alphaF());
      mMoonOrbit.second->SetColor(color);
      mMoonOrbit.second->SetLineWidth(prefObject->GetLineWidth());
      UpdateMoonOrbit(aSimTime);
   }
}

void wkf::OrbitInterface::UpdateMoonOrbit(double aSimTime)
{
   if (mMoonOrbit.second)
   {
      UtVec3d    locECI;
      UtVec3d    velECI;
      UtCalendar curTime = mStartDateTime;
      curTime.AdvanceTimeBy(aSimTime);
      UtSolarSystem::GetMoonLocationVelocityECI(curTime, locECI, velECI);
      ut::OrbitalState orbitState(curTime,
                                  ut::EarthWGS84(),
                                  ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                  ut::OrbitalState::ReferenceFrame::cECI,
                                  ut::OrbitalState::Vector(locECI, velECI));
      const auto&      elements = orbitState.GetOrbitalElementsTOD();
      mMoonOrbit.second->Add(aSimTime,
                             elements.GetSemiMajorAxis(),
                             elements.GetEccentricity(),
                             elements.GetRAAN(),
                             elements.GetInclination(),
                             elements.GetArgumentOfPeriapsis(),
                             elements.GetTrueAnomaly(),
                             false);
      mMoonOrbit.second->UpdateTimeAngle(curTime.GetEarthAngleApprox(), aSimTime);
   }
}

void wkf::OrbitInterface::MoonOrbitVisibilityChanged(bool aVisible)
{
   if (aVisible)
   {
      AddMoonOrbit();
   }
   else
   {
      RemoveMoonOrbit();
   }
}

void wkf::OrbitInterface::MoonOrbitColorChanged(const QColor& aColor)
{
   if (mMoonOrbit.first && mMoonOrbit.second)
   {
      mMoonOrbit.second->SetColor(UtColor(aColor.redF(), aColor.greenF(), aColor.blueF(), aColor.alphaF()));
   }
}

void wkf::OrbitInterface::MoonOrbitLineWidthChanged(int aLineWidth)
{
   if (mMoonOrbit.first && mMoonOrbit.second)
   {
      mMoonOrbit.second->SetLineWidth(aLineWidth);
   }
}

void wkf::OrbitInterface::RemoveMoonOrbit()
{
   if (mMoonOrbit.first && mMoonOrbit.second)
   {
      mMoonOrbit.first.reset();
      mMoonOrbit.second = nullptr;
   }
}

double wkf::OrbitInterface::GetAngleForTime(double aSimTime)
{
   UtCalendar curTime = mStartDateTime;
   curTime.AdvanceTimeBy(aSimTime);
   return curTime.GetEarthAngle();
}


void wkf::OrbitInterface::UpdateOrbitAngles(double aSimTime)
{
   double angle = GetAngleForTime(aSimTime);
   for (auto& it : mOrbitMap)
   {
      it.second->UpdateTimeAngle(angle, aSimTime);
   }
}

void wkf::OrbitInterface::EntityDeletedCB(vespa::VaEntity* aEntityPtr)
{
   wkf::Platform* p = dynamic_cast<wkf::Platform*>(aEntityPtr);
   if (p)
   {
      mOrbitMap.erase(p->GetIndex());
   }
   if (mMoonOrbit.first && aEntityPtr->GetUniqueId() == mMoonOrbit.first->GetUniqueId())
   {
      mMoonOrbit.first.reset();
      mMoonOrbit.second = nullptr;
   }
}
