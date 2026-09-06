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

#ifndef RVPREFERENCESDIALOG_HPP
#define RVPREFERENCESDIALOG_HPP

#include "RvInterpolationPrefWidget.hpp"
#include "RvMemoryPrefWidget.hpp"
#include "WkfPreferencesDialog.hpp"
#include "visibility/WkfTrackVisibilityPrefWidget.hpp"
#include "visibility/WkfTypeVisibilityPrefWidget.hpp"

namespace rv
{
class PreferencesDialog : public wkf::PreferencesDialog
{
public:
   PreferencesDialog(wkf::MainWindow* parent, Qt::WindowFlags f = Qt::WindowFlags())
      : wkf::PreferencesDialog(parent, f)
   {
      mStandardWidgets << new MemoryPrefWidget(this);
      mStandardWidgets << new InterpolationPrefWidget(this);
      mStandardWidgets << new wkf::TrackVisibilityPrefWidget(this);
      mStandardWidgets << new wkf::TypeVisibilityPrefWidget(this);
   }
};
} // namespace rv
#endif
