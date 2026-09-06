// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfDisObserver.hpp"

#include "WsfSimulation.hpp"

#define WSF_DIS_OBSERVER_CALLBACK_DEFINE(PDU)                                                                           \
   WsfObserver::PDU_ReceivedCallback<WsfDis##PDU>& WsfObserver::Dis##PDU##Received(const WsfSimulation* aSimulationPtr) \
   {                                                                                                                    \
      return aSimulationPtr->GetDisObserver().PDU##Received;                                                            \
   }

WSF_DIS_OBSERVER_CALLBACK_DEFINE(AcknowledgeR)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(Data)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(Designator)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(EntityState)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(Emission)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(SetData)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(SetRecordR)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(Signal)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(StartResume)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(TransferOwnership)
WSF_DIS_OBSERVER_CALLBACK_DEFINE(Transmitter)
