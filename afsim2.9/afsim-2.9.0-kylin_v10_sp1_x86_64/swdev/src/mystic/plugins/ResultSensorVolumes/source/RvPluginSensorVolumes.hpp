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
#ifndef RvPluginSensorVolumes_HPP
#define RvPluginSensorVolumes_HPP

#include <memory>

#include "RvPlatformSensorVolumes.hpp"
#include "RvPlugin.hpp"
#include "VaCallbackHolder.hpp"
#include "misc/WkfImmersiveViewDockWidget.hpp"
#include "sensor_volume/WkfSensorVolumesPrefWidget.hpp"

namespace rv
{
class MsgEmitterModeDefinition;
}
namespace wkf
{
class Platform;
}

namespace RvSensorVolumes
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   void BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr) override;

   void UpdateBoresight(wkf::ImmersiveViewDockWidget* aDockWidget, const std::string& aSpecificSensor);

   void           ResetOptionStates() override;
   void           SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr) override;
   Qt::CheckState GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const override;

private slots:
   void         AdvanceTimeRead(const rv::ResultData& aDataPtr) override;
   void         RegularRead(const rv::ResultData& aData) override;
   void         NewSensorModeDefinition(const rv::MsgEmitterModeDefinition& aNewMode) override;
   void         NewWeaponModeDefinition(const rv::MsgEmitterModeDefinition& aNewMode) override;
   virtual void ModesReset();
   void         DrawModeChanged(const wkf::SensorVolumesPrefData::DrawMode& aState);
   void         ClearScenario(bool aFullReset) override;

protected:
   void ClearIndividualOptions() override;

private:
   void RegisterIndividualOptions(const std::string& aStringId) override;

   vespa::VaCallbackHolder mCallbacks;

   std::map<int, std::map<unsigned int, std::unique_ptr<PlatformVolumes>>> mSensorVolumeMap; // from volume type
                                                                                             // (sensor/weapon) to
                                                                                             // platform to volume

   // Use guarded pointers because objects will be owned by main window
   PluginUiPointer<wkf::SensorVolumesPrefWidget>                  mPrefWidgetPtr;
   QTreeWidgetItem*                                               mSensorTopItem;
   QTreeWidgetItem*                                               mWeaponTopItem;
   int                                                            mIndividualIndex;
   std::set<std::string>                                          mIndividualsSensors;
   std::set<std::string>                                          mIndividualsWeapons;
   std::map<wkf::ImmersiveViewDockWidget*, std::set<std::string>> mWidgetBoresights;
};
} // namespace RvSensorVolumes
#endif
