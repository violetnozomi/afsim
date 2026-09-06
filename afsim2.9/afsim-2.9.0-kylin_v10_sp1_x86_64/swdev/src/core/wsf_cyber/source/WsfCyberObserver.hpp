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

#ifndef WSFCYBEROBSERVER_HPP
#define WSFCYBEROBSERVER_HPP

#include "wsf_cyber_export.h"

#include "UtCallback.hpp"
class WsfSimulation;

namespace wsf
{
namespace cyber
{

class Engagement;

namespace trigger
{

class Trigger;

} // namespace trigger
} // namespace cyber
} // namespace wsf

//! Observer definitions for cyber events.
namespace WsfObserver
{
using CyberCallback                  = UtCallbackListN<void(double, const wsf::cyber::Engagement&)>;
using CyberTriggerEvaluationCallback = UtCallbackListN<void(double, const wsf::cyber::trigger::Trigger&, bool)>;
using CyberTriggerExecutionCallback  = UtCallbackListN<void(double, const wsf::cyber::trigger::Trigger&)>;

WSF_CYBER_EXPORT CyberCallback&                  CyberAttackInitiated(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberAttackSucceeded(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberAttackFailed(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberAttackDetected(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberAttackAttributed(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberAttackRecovery(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberScanInitiated(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberScanSucceeded(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberScanFailed(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberScanDetected(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberCallback&                  CyberScanAttributed(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberTriggerEvaluationCallback& CyberTriggerEvaluation(const WsfSimulation* aSimulationPtr);
WSF_CYBER_EXPORT CyberTriggerExecutionCallback&  CyberTriggerExecution(const WsfSimulation* aSimulationPtr);

} // namespace WsfObserver

namespace wsf
{
namespace cyber
{

struct WSF_CYBER_EXPORT Observer
{
   WsfObserver::CyberCallback CyberAttackInitiated;  //! A cyber attack has been initiated.
   WsfObserver::CyberCallback CyberAttackSucceeded;  //! A cyber attack has succeeded.
   WsfObserver::CyberCallback CyberAttackFailed;     //! A cyber attack has failed.
   WsfObserver::CyberCallback CyberAttackDetected;   //! A cyber attack has been detected by the victim.
   WsfObserver::CyberCallback CyberAttackAttributed; //! A cyber attack has been attributed by the victim.
   WsfObserver::CyberCallback CyberAttackRecovery;   //! The victim of a cyber attack has recovered from the attack.
   WsfObserver::CyberCallback CyberScanInitiated;    //! A cyber scan has been initiated.
   WsfObserver::CyberCallback CyberScanSucceeded;    //! A cyber scan has succeeded.
   WsfObserver::CyberCallback CyberScanFailed;       //! A cyber scan has failed.
   WsfObserver::CyberCallback CyberScanDetected;     //! A cyber scan has been detected.
   WsfObserver::CyberCallback CyberScanAttributed;   //! A cyber scan has been attributed.
   WsfObserver::CyberTriggerEvaluationCallback CyberTriggerEvaluation; //! A cyber trigger is evaluating.
   WsfObserver::CyberTriggerExecutionCallback  CyberTriggerExecution;  //! A cyber trigger is executing.
};

} // namespace cyber
} // namespace wsf

#endif
