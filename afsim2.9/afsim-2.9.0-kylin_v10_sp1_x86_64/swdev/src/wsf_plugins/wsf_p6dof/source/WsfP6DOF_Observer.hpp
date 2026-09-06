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

#ifndef WSFP6DOF_OBSERVER_HPP
#define WSFP6DOF_OBSERVER_HPP

#include "wsf_p6dof_export.h"

#include "UtCallback.hpp"
class WsfP6DOF_Mover;
class WsfPlatform;
class WsfSimulation;
#include "WsfSimulationExtension.hpp"

namespace WsfObserver
{
using P6DofSendMessageCallback = UtCallbackListN<void(double, const WsfPlatform&, WsfP6DOF_Mover&, const std::string&)>;

/** Callback for sending a P6Dof message.
 * @param aSimulationPtr simulation to own this callback
 */
WSF_P6DOF_EXPORT P6DofSendMessageCallback& P6DofSendMessage(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

class WSF_P6DOF_EXPORT WsfP6DOF_Observer : public WsfSimulationExtension
{
public:
   /** Callback for sending a P6Dof message. */
   WsfObserver::P6DofSendMessageCallback P6DofSendMessage;
};

#endif
