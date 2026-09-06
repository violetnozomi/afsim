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

#ifndef WKFORBITPLUGINBASE_HPP
#define WKFORBITPLUGINBASE_HPP

#include "wkf_common_export.h"

#include "WkfAttachmentOrbit.hpp"
#include "WkfMoonOrbitPrefWidget.hpp"
#include "WkfOrbitInterface.hpp"
#include "WkfOrbitPrefWidget.hpp"
#include "WkfPlugin.hpp"

namespace wkf
{

class WKF_COMMON_EXPORT OrbitPluginBase : public Plugin
{
public:
   OrbitPluginBase(const QString& aPluginName, const size_t& aUniqueId, const QFlags<Roles>& aRoles = eNO_ROLE);
   ~OrbitPluginBase() override = 0;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   void           ResetOptionStates() override;
   void           SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr) override;
   Qt::CheckState GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const override;

protected:
   PluginUiPointer<wkf::OrbitPrefWidget>     mOrbitPrefWidgetPtr;
   PluginUiPointer<wkf::MoonOrbitPrefWidget> mMoonOrbitPrefWidgetPtr;
   wkf::OrbitInterface*                      mOrbitInterfacePtr;
};

} // namespace wkf

#endif // WKFORBITPLUGINBASE_HPP
