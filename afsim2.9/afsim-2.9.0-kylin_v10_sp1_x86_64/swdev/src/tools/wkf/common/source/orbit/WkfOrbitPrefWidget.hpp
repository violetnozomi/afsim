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
#ifndef WKFORBITPREFWIDGET_HPP
#define WKFORBITPREFWIDGET_HPP

#include "wkf_common_export.h"

#include "WkfOrbitPrefObject.hpp"
#include "WkfPrefWidget.hpp"

class QComboBox;
class QSpinBox;

namespace wkf
{
class WKF_COMMON_EXPORT OrbitPrefWidget : public wkf::PrefWidgetT<OrbitPrefObject>
{
public:
   explicit OrbitPrefWidget(bool aRealtime = true);
   ~OrbitPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Space"; }
   void    EditorMode();

private:
   void ReadPreferenceData(const OrbitPrefData& aPrefData) override;
   void WritePreferenceData(OrbitPrefData& aPrefData) override;

   QSpinBox*  mFadeout{nullptr};
   QSpinBox*  mLineWidth{nullptr};
   QComboBox* mColorMode{nullptr};
   QSpinBox*  mPeriods{nullptr};
};
} // namespace wkf
#endif // WKFORBITPREFWIDGET_HPP
