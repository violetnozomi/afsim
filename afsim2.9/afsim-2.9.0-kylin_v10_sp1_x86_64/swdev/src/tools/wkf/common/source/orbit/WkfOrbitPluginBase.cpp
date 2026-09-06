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

#include "WkfOrbitPluginBase.hpp"

#include "WkfEnvironment.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfUtils.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::OrbitPluginBase::OrbitPluginBase(const QString& aPluginName, const size_t& aUniqueId, const QFlags<Roles>& aRoles)
   : wkf::Plugin(aPluginName, aUniqueId, aRoles)
   , mOrbitPrefWidgetPtr{new wkf::OrbitPrefWidget{}}
   , mMoonOrbitPrefWidgetPtr{new wkf::MoonOrbitPrefWidget{}}
   , mOrbitInterfacePtr{new OrbitInterface{this, mOrbitPrefWidgetPtr.data(), mMoonOrbitPrefWidgetPtr.data()}}
{
   connect(mOrbitPrefWidgetPtr->GetPreferenceObject(),
           &wkf::OrbitPrefObject::FadeoutChanged,
           mOrbitInterfacePtr,
           &OrbitInterface::FadeoutChanged);
   connect(mOrbitPrefWidgetPtr->GetPreferenceObject(),
           &wkf::OrbitPrefObject::ColorModeChanged,
           mOrbitInterfacePtr,
           &OrbitInterface::ColorModeChanged);
   connect(mOrbitPrefWidgetPtr->GetPreferenceObject(),
           &wkf::OrbitPrefObject::LineWidthChanged,
           mOrbitInterfacePtr,
           &OrbitInterface::LineWidthChanged);
   connect(mOrbitPrefWidgetPtr->GetPreferenceObject(),
           &wkf::OrbitPrefObject::PeriodsChanged,
           mOrbitInterfacePtr,
           &OrbitInterface::PeriodsChanged);
   connect(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>(),
           &wkf::TeamVisibilityPrefObject::TeamColorChanged,
           mOrbitInterfacePtr,
           &OrbitInterface::TeamColorsChanged);
   connect(mMoonOrbitPrefWidgetPtr->GetPreferenceObject(),
           &wkf::MoonOrbitPrefObject::VisibilityChanged,
           mOrbitInterfacePtr,
           &OrbitInterface::MoonOrbitVisibilityChanged);
   connect(mMoonOrbitPrefWidgetPtr->GetPreferenceObject(),
           &wkf::MoonOrbitPrefObject::ColorChanged,
           mOrbitInterfacePtr,
           &OrbitInterface::MoonOrbitColorChanged);
   connect(mMoonOrbitPrefWidgetPtr->GetPreferenceObject(),
           &wkf::MoonOrbitPrefObject::LineWidthChanged,
           mOrbitInterfacePtr,
           &OrbitInterface::MoonOrbitLineWidthChanged);
   RegisterOption(nullptr, "Orbit");
}

wkf::OrbitPluginBase::~OrbitPluginBase() = default;

QList<wkf::PrefWidget*> wkf::OrbitPluginBase::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> retval{};
   retval.push_back(mMoonOrbitPrefWidgetPtr);
   retval.push_back(mOrbitPrefWidgetPtr);
   return retval;
}

void wkf::OrbitPluginBase::ResetOptionStates()
{
   mOrbitInterfacePtr->ClearAllAttachments();
}

void wkf::OrbitPluginBase::SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr)
{
   if (aState)
   {
      wkf::AttachmentOrbit* orbitPtr = aPlatformPtr->FindFirstAttachmentOfType<wkf::AttachmentOrbit>();
      if (!orbitPtr)
      {
         mOrbitInterfacePtr->AddAttachment(aPlatformPtr);
      }
   }
   else
   {
      wkf::AttachmentOrbit* orbitPtr = aPlatformPtr->FindFirstAttachmentOfType<wkf::AttachmentOrbit>();
      if (orbitPtr) // if there is an attachment
      {
         mOrbitInterfacePtr->RemoveAttachment(aPlatformPtr->GetIndex());
         aPlatformPtr->RemoveAttachment(orbitPtr->GetUniqueId());
         // remove the attachment
      }
   }
}

Qt::CheckState wkf::OrbitPluginBase::GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const
{
   if (mOrbitInterfacePtr->HasAttachment(aPlatformPtr->GetIndex()))
   {
      return Qt::Checked;
   }
   return Qt::Unchecked;
}
