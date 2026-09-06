// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFQUANTUMTASKEROBSERVER_HPP
#define WSFQUANTUMTASKEROBSERVER_HPP

#include "wsf_mil_export.h"

#include "UtCallback.hpp"
#include "WsfQuantumTaskerProcessor.hpp"
class WsfSimulation;

namespace WsfObserver
{
using QuantumTaskerUpdateCallback = UtCallbackListN<void(double, WsfQuantumTaskerProcessor*)>;

WSF_MIL_EXPORT QuantumTaskerUpdateCallback& QuantumTaskerUpdate(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the QuantumTasker observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_MIL_EXPORT WsfQuantumTaskerObserver
{
   WsfObserver::QuantumTaskerUpdateCallback QuantumTaskerUpdate; //! A task has been accepted by a subordinate.
};

#endif
