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

#ifndef WSFZONEOBSERVER_HPP
#define WSFZONEOBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "WsfZone.hpp"
class WsfSimulation;

namespace WsfObserver
{
using ZoneFillColorChangedCallback = UtCallbackListN<void(const WsfZone&)>;
using ZoneLineColorChangedCallback = UtCallbackListN<void(const WsfZone&)>;

WSF_EXPORT ZoneFillColorChangedCallback& ZoneFillColorChanged(const WsfSimulation* aSimulation);
WSF_EXPORT ZoneLineColorChangedCallback& ZoneLineColorChanged(const WsfSimulation* aSimulation);
} // namespace WsfObserver

//! The implementation of the Zone observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfZoneObserver
{
   //! Provide a callback to query the zone fill color whenever it is changed.
   WsfObserver::ZoneFillColorChangedCallback ZoneFillColorChanged;

   //! Provide a callback to query the zone line color whenever it is changed.
   WsfObserver::ZoneLineColorChangedCallback ZoneLineColorChanged;
};

#endif
