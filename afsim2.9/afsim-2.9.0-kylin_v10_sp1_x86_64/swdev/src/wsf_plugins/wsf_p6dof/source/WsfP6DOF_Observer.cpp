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

#include "WsfP6DOF_Observer.hpp"

#include "WsfSimulation.hpp"

#define WSF_P6DOF_OBSERVER_CALLBACK_DEFINE(EVENT)                                                     \
   WsfObserver::EVENT##Callback& WsfObserver::EVENT(const WsfSimulation* aSimulationPtr)              \
   {                                                                                                  \
      return static_cast<WsfP6DOF_Observer*>(aSimulationPtr->FindExtension("p6dof_observer"))->EVENT; \
   }

WSF_P6DOF_OBSERVER_CALLBACK_DEFINE(P6DofSendMessage)
