// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_Observer.hpp"

#include "WsfSimulation.hpp"

#define WSF_COMMON_AIR_OBSERVER_CALLBACK_DEFINE(EVENT)                                                           \
   WsfObserver::EVENT##Callback& WsfObserver::EVENT(const WsfSimulation* aSimulationPtr)                         \
   {                                                                                                             \
      return static_cast<wsf::six_dof::Observer*>(aSimulationPtr->FindExtension("wsf_six_dof_observer"))->EVENT; \
   }

WSF_COMMON_AIR_OBSERVER_CALLBACK_DEFINE(SixDOF_SendMessage)
