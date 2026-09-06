// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfNonExportableBase.hpp"

#include "wsf_nx_export.h"

#include "UtMemory.hpp"
#include "WsfALARM_AntennaPattern.hpp"
#include "WsfALARM_AntennaPatternBanded.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfAntennaPatternTypes.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfChaffEjector.hpp"
#include "WsfChaffParcelTypes.hpp"
#include "WsfChaffWeapon.hpp"
#include "WsfCoherentSensorProcessor.hpp"
#include "WsfCorrelationStrategyTypes.hpp"
#include "WsfEM_ALARM_Clutter.hpp"
#include "WsfEM_ALARM_Propagation.hpp"
#include "WsfEM_ALARM_Terrain.hpp"
#include "WsfEM_Attenuation.hpp"
#include "WsfEM_AttenuationTypes.hpp"
#include "WsfEM_Clutter.hpp"
#include "WsfEM_ClutterTypes.hpp"
#include "WsfEM_EARCE_Attenuation.hpp"
#include "WsfEM_Propagation.hpp"
#include "WsfEM_PropagationTypes.hpp"
#include "WsfEM_RF_Type1Attenuation.hpp"
#include "WsfESA_NX_AntennaPattern.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_EffectTypes.hpp"
#include "WsfEW_NX_SLB_Effect.hpp"
#include "WsfElementESA_AntennaPattern.hpp"
#include "WsfEngageLaunchPkTableLethality.hpp"
#include "WsfGENAP_AntennaPattern.hpp"
#include "WsfLink16Correlation.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfRadarMTD_SignalProcessor.hpp"
#include "WsfRadarMTI_SignalProcessor.hpp"
#include "WsfRadarPD_SignalProcessor.hpp"
#include "WsfRadarSTC_SignalProcessor.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfScriptChaffWeaponClass.hpp"
#include "WsfSensorErrorModelTypes.hpp"
#include "WsfSensorSignalProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTRIMSIM_Processor.hpp"
#include "WsfWeaponEffectsTypes.hpp"
#include "WsfWeaponTypes.hpp"

class WsfNonExportableExtension : public WsfScenarioExtension
{
public:
   void AddedToScenario() override
   {
      WsfScenario& scenario(GetScenario());
      WsfAntennaPatternTypes::Get(scenario).AddObjectFactory(WsfALARM_AntennaPattern::ObjectFactory);
      WsfAntennaPatternTypes::Get(scenario).AddObjectFactory(WsfALARM_AntennaPatternBanded::ObjectFactory);
      WsfAntennaPatternTypes::Get(scenario).AddObjectFactory(WsfGENAP_AntennaPattern::ObjectFactory);
      WsfAntennaPatternTypes::Get(scenario).AddObjectFactory(WsfElementESA_AntennaPattern::ObjectFactory);
      WsfAntennaPatternTypes::Get(scenario).AddObjectFactory(WsfESA_NX_AntennaPattern::ObjectFactory);

      WsfCorrelationStrategyTypes::Get(scenario).Add("link16", ut::make_unique<WsfLink16Correlation>());

      WsfEM_AttenuationTypes::Get(scenario).AddObjectFactory(WsfEM_EARCE_Attenuation::ObjectFactory);
      WsfEM_AttenuationTypes::Get(scenario).AddObjectFactory(WsfEM_RF_Type1Attenuation::ObjectFactory);

      WsfEM_ClutterTypes::Get(scenario).AddObjectFactory(WsfEM_ALARM_Clutter::ObjectFactory);

      WsfEM_PropagationTypes::Get(scenario).AddObjectFactory(WsfEM_ALARM_Propagation::ObjectFactory);

      WsfProcessorTypes::Get(scenario).AddCoreType("WSF_TRIMSIM_PROCESSOR",
                                                   ut::make_unique<WsfTRIMSIM_Processor>(scenario));
      WsfSensorErrorModelTypes::Get(scenario).AddObjectFactory(WsfTRIMSIM_ErrorModel::ObjectFactory);

      WsfProcessorTypes::Get(scenario).AddCoreType("WSF_COHERENT_SENSOR_PROCESSOR",
                                                   ut::make_unique<WsfCoherentSensorProcessor>(scenario));

      WsfEW_EffectTypes::Get(scenario).Add("WSF_NX_SLB_EFFECT", ut::make_unique<WsfEW_NX_SLB_Effect>());

      WsfSensorSignalProcessor::AddObjectFactory(WsfRadarSTC_SignalProcessor::ObjectFactory);
      WsfSensorSignalProcessor::AddObjectFactory(WsfRadarMTD_SignalProcessor::ObjectFactory);
      WsfSensorSignalProcessor::AddObjectFactory(WsfRadarMTI_SignalProcessor::ObjectFactory);
      WsfSensorSignalProcessor::AddObjectFactory(WsfRadarPD_SignalProcessor::ObjectFactory);

      WsfWeaponTypes::Get(scenario).AddCoreType("WSF_CHAFF_WEAPON", ut::make_unique<WsfChaffWeapon>(scenario));

      scenario.GetScriptTypes()->Register(
         ut::make_unique<WsfScriptChaffWeaponClass>("WsfChaffWeapon", scenario.GetScriptTypes()));

      scenario.AddTypeList(ut::make_unique<WsfChaffParcelTypes>(scenario));

      // Register the component factory for the chaff ejector component on chaff weapon
      WsfChaffEjector::RegisterComponentFactory(scenario);

      WsfWeaponEffectsTypes::Get(scenario).AddCoreType("WSF_ENGAGE_LAUNCH_PK_TABLE_LETHALITY",
                                                       ut::make_unique<WsfEngageLaunchPkTableLethality>(scenario));
   }

   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfNonExportableBase>());
   }
};

// ================================================================================================
WsfNonExportableBase::~WsfNonExportableBase()
{
   WsfEM_ALARM_Clutter::ResetState();
   WsfEM_ALARM_Propagation::ResetState();
   WsfEM_ALARM_Terrain::ResetState();
}

// ================================================================================================
void WSF_NX_EXPORT Register_wsf_nx(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_nx"))
   {
      aApplication.RegisterFeature("non_exportable", "wsf_nx");
      WSF_REGISTER_EXTENSION(aApplication, wsf_mil); // This extension REQUIRES the "wsf_mil" extension
      aApplication.RegisterExtension("wsf_nx",
                                     ut::make_unique<WsfDefaultApplicationExtension<WsfNonExportableExtension>>());
      aApplication.ExtensionDepends("wsf_nx", "wsf_mil", true);
   }
}
