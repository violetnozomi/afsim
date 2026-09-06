// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFWEAPONTASKOBSERVER_HPP
#define WSFWEAPONTASKOBSERVER_HPP

#include "wsf_mil_export.h"

#include "UtCallback.hpp"
class WsfSimulation;
class WsfTask;

namespace WsfObserver
{
using StartingWeaponTaskCallback = UtCallbackListN<void(double, const WsfTask&)>;

WSF_MIL_EXPORT WsfObserver::StartingWeaponTaskCallback& StartingWeaponTask(const WsfSimulation* aSimulation);
} // namespace WsfObserver

//! The implementation of the Weapon Task observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_MIL_EXPORT WsfWeaponTaskObserver
{
   WsfObserver::StartingWeaponTaskCallback StartingWeaponTask; //! A task has been accepted by a subordinate.
};

#endif
