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
#ifndef WSFDISOBSERVER_HPP
#define WSFDISOBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "dis/WsfDisAcknowledgeR.hpp"
#include "dis/WsfDisData.hpp"
#include "dis/WsfDisDesignator.hpp"
#include "dis/WsfDisEmission.hpp"
#include "dis/WsfDisEntityState.hpp"
#include "dis/WsfDisSetData.hpp"
#include "dis/WsfDisSetRecordR.hpp"
#include "dis/WsfDisSignal.hpp"
#include "dis/WsfDisStartResume.hpp"
#include "dis/WsfDisTransferOwnership.hpp"
#include "dis/WsfDisTransmitter.hpp"

class WsfSimulation;

namespace WsfObserver
{
template<class PDU>
using PDU_ReceivedCallback = UtCallbackListN<void(WsfDisInterface*, const PDU&)>;

WSF_EXPORT PDU_ReceivedCallback<WsfDisAcknowledgeR>& DisAcknowledgeRReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisData>& DisDataReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisDesignator>& DisDesignatorReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisEmission>& DisEmissionReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisEntityState>& DisEntityStateReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisSetData>& DisSetDataReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisSetRecordR>& DisSetRecordRReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisSignal>& DisSignalReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisStartResume>& DisStartResumeReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisTransferOwnership>& DisTransferOwnershipReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PDU_ReceivedCallback<WsfDisTransmitter>& DisTransmitterReceived(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the DIS observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfDisObserver
{
   WsfObserver::PDU_ReceivedCallback<WsfDisAcknowledgeR>      AcknowledgeRReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisData>              DataReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisDesignator>        DesignatorReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisEmission>          EmissionReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisEntityState>       EntityStateReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisSetData>           SetDataReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisSetRecordR>        SetRecordRReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisSignal>            SignalReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisStartResume>       StartResumeReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisTransferOwnership> TransferOwnershipReceived;
   WsfObserver::PDU_ReceivedCallback<WsfDisTransmitter>       TransmitterReceived;
};

#endif
