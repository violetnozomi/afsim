// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfPlatformObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(Platform, Comment)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, CrashedIntoGround)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, ExecuteCallback)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, GroupAdded)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, NavigationStatusChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, PlatformAdded)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, PlatformAppearanceChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, PlatformBroken)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, PlatformDamageChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, PlatformDeleted)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, PlatformInitialized)
WSF_OBSERVER_CALLBACK_DEFINE(Platform, PlatformOmitted)
