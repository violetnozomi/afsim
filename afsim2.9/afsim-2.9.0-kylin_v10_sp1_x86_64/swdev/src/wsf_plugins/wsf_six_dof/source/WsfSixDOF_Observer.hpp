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

#ifndef WSFSIXDOFOBSERVER_HPP
#define WSFSIXDOFOBSERVER_HPP

#include "wsf_six_dof_export.h"

#include "UtCallback.hpp"
class WsfPlatform;
class WsfSimulation;
#include "WsfSimulationExtension.hpp"

namespace wsf
{
namespace six_dof
{
class Mover;
}
} // namespace wsf

namespace WsfObserver
{
using SixDOF_SendMessageCallback =
   UtCallbackListN<void(double, const WsfPlatform&, wsf::six_dof::Mover&, const std::string&)>;

/** Callback for sending a SixDOF message.
 * @param aSimulationPtr simulation to own this callback
 */
WSF_SIX_DOF_EXPORT SixDOF_SendMessageCallback& SixDOF_SendMessage(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT Observer : public WsfSimulationExtension
{
public:
   /** Callback for sending a SixDOF message. */
   WsfObserver::SixDOF_SendMessageCallback SixDOF_SendMessage;
};
} // namespace six_dof
} // namespace wsf

#endif
