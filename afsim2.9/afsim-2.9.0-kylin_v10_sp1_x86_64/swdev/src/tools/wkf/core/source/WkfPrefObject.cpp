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

#include "WkfPrefObject.hpp"

#include <exception>

#include "WkfEnvironment.hpp"

wkf::PrefObject::PrefObject(QObject* parent, const QString& aName)
   : QObject(parent)
   , mName(aName)
{
   wkfEnv.RegisterPrefObject(this);
}

wkf::PrefObject::~PrefObject()
{
   wkfEnv.UnregisterPrefObject(this);
}

wkf::detail::PrefObjectScopeGuard::PrefObjectScopeGuard(QSettings& aSettings, const PrefObject& aObject) noexcept
   : mSettings(aSettings)
{
   aSettings.beginGroup(aObject.GetName());
   mGroup = aSettings.group();
}

wkf::detail::PrefObjectScopeGuard::~PrefObjectScopeGuard() noexcept
{
   // Check to make sure the current group is the expected group
   //    if the current group does not match, warn user about the offending PrefObject/Plugin
   if (mGroup != mSettings.group() || std::current_exception() != nullptr)
   {
      // If this warning is appearing, that means the current PrefObject does not have matching beginGroup/endGroup or
      // beginArray/endArray calls.
      QMessageBox::warning(nullptr,
                           "Load Preferences Error",
                           QString("The %1 preference object encountered an error when loading "
                                   "preferences.\nPreferences may not have been loaded.")
                              .arg(mGroup));

      // End groups until back to root level.
      while (!mSettings.group().isEmpty())
      {
         mSettings.endGroup();
      }
   }
   else
   {
      // If all goes well, we only need to end one group.
      mSettings.endGroup();
   }
}
