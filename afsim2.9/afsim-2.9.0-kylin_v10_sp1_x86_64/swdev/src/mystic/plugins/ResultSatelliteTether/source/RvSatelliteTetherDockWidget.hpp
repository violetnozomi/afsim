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

#ifndef RVSATELLITETETHERDOCKWIDGET
#define RVSATELLITETETHERDOCKWIDGET

#include "satellite_tether/WkfSatelliteTetherDockWidget.hpp"

namespace RvSatelliteTether
{
class DockWidget : public wkf::SatelliteTetherDockWidget
{
public:
   DockWidget(QString         aObjectName,
              QString         aPlatformName,
              unsigned int    aId,
              QMainWindow*    aParent = nullptr,
              Qt::WindowFlags aFlags  = Qt::WindowFlags());

   unsigned int Update(const rv::ResultData& aData, const std::string& aPlatform);

   void AddTrack(wkf::Platform* aPlatformPtr) override;

private:
   wkf::AttachmentSatelliteTetherTrace*
   MakeTrace(wkf::Platform& aSrc, vespa::VaViewer& aViewer, wkf::Platform& aTgt, const QColor& aColor) override;

   std::map<std::string, unsigned int> mOrbitIndex; // keeps track of the active orbit message index
};
} // namespace RvSatelliteTether

#endif
