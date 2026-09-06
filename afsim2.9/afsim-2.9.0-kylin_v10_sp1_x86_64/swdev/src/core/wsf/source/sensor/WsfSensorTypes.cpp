// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSensorTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

// Include files for built-in types
#include "WsfCompositeSensor.hpp"
#include "WsfGeometricSensor.hpp"
#include "WsfNullSensor.hpp"
#include "WsfPassiveSensor.hpp"
#include "WsfRadarSensor.hpp"

// Signal Processors
#include "WsfRadarMTI_AdjSignalProcessor.hpp"
#include "WsfSimpleDopplerSignalProcessor.hpp"

// Detectors
#include "WsfBinaryDetector.hpp"
#include "WsfDetectionProbabilityTable.hpp"
#include "WsfMarcumSwerling.hpp"
#include "WsfSensorDetector.hpp"

// Schedulers
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfPhysicalScanSensorScheduler.hpp"
#include "WsfSectorScanSensorScheduler.hpp"
#include "WsfSensorSchedulerTypes.hpp"
#include "WsfSpinSensorScheduler.hpp"

namespace
{
//! Component factory to process platform input.
class SensorComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      WsfSensorTypes& types(WsfSensorTypes::Get(GetScenario()));
      return types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<WsfSensor>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfSensorTypes& types(WsfSensorTypes::Get(GetScenario()));
      return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<WsfSensor>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfSensorTypes& WsfSensorTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetSensorTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfSensorTypes& WsfSensorTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetSensorTypes();
}

// =================================================================================================
WsfSensorTypes::WsfSensorTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfSensor>(aScenario, cREDEFINITION_ALLOWED, "sensor")
{
   aScenario.RegisterComponentFactory(ut::make_unique<SensorComponentFactory>());

   AddCoreType("WSF_COMPOSITE_SENSOR", ut::make_unique<WsfCompositeSensor>(aScenario));
   AddCoreType("WSF_PASSIVE_SENSOR", ut::make_unique<WsfPassiveSensor>(aScenario));
   AddCoreType("WSF_GEOMETRIC_SENSOR", ut::make_unique<WsfGeometricSensor>(aScenario));
   AddCoreType("WSF_NULL_SENSOR", ut::make_unique<WsfNullSensor>(aScenario));
   AddCoreType("WSF_RADAR_SENSOR", ut::make_unique<WsfRadarSensor>(aScenario));

   WsfSensorSignalProcessor::AddObjectFactory(wsf::SimpleDopplerSignalProcessor::ObjectFactory);
   WsfSensorSignalProcessor::AddObjectFactory(WsfRadarMTI_AdjSignalProcessor::ObjectFactory);

   wsf::SensorDetectorTypes::AddObjectFactory(wsf::BinaryDetector::ObjectFactory);
   wsf::SensorDetectorTypes::AddObjectFactory(wsf::MarcumSwerling::ObjectFactory);
   wsf::SensorDetectorTypes::AddObjectFactory(wsf::DetectionProbabilityTable::ObjectFactory);

   WsfSensorSchedulerTypes::AddObjectFactory(WsfDefaultSensorScheduler::ObjectFactory);
   WsfSensorSchedulerTypes::AddObjectFactory(WsfPhysicalScanSensorScheduler::ObjectFactory);
   WsfSensorSchedulerTypes::AddObjectFactory(WsfSectorScanSensorScheduler::ObjectFactory);
   WsfSensorSchedulerTypes::AddObjectFactory(WsfSpinSensorScheduler::ObjectFactory);
}
