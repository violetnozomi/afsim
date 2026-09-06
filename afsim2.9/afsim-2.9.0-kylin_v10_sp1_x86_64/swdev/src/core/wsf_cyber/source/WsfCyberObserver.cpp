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

#include "WsfCyberObserver.hpp"

#include "WsfCyberSimulationExtension.hpp"

#define WSF_CYBER_OBSERVER_CALLBACK_DEFINE(EVENT)                                       \
   WsfObserver::CyberCallback& WsfObserver::EVENT(const WsfSimulation* aSimulationPtr)  \
   {                                                                                    \
      return wsf::cyber::SimulationExtension::Get(*aSimulationPtr).GetObserver().EVENT; \
   }

#define WSF_CYBER_OBSERVER_CALLBACK_TRIGGER(EVENT)                                       \
   WsfObserver::EVENT##Callback& WsfObserver::EVENT(const WsfSimulation* aSimulationPtr) \
   {                                                                                     \
      return wsf::cyber::SimulationExtension::Get(*aSimulationPtr).GetObserver().EVENT;  \
   }

WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberAttackInitiated)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberAttackSucceeded)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberAttackFailed)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberAttackDetected)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberAttackAttributed)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberAttackRecovery)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberScanInitiated)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberScanSucceeded)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberScanFailed)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberScanDetected)
WSF_CYBER_OBSERVER_CALLBACK_DEFINE(CyberScanAttributed)
WSF_CYBER_OBSERVER_CALLBACK_TRIGGER(CyberTriggerEvaluation)
WSF_CYBER_OBSERVER_CALLBACK_TRIGGER(CyberTriggerExecution)
