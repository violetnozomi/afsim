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

#ifndef WKFCOMMENTPREFWIDGET_HPP
#define WKFCOMMENTPREFWIDGET_HPP

#include "wkf_common_export.h"

#include <QCheckBox>
#include <QSpinBox>

#include "WkfCommentPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfCommentPrefWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT CommentPrefWidget : public wkf::PrefWidgetT<CommentPrefObject>
{
   Q_OBJECT
public:
   CommentPrefWidget();
   ~CommentPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Map"; }

private:
   void ReadPreferenceData(const CommentPrefData& aPrefData) override;
   void WritePreferenceData(CommentPrefData& aPrefData) override;

   Ui::WkfCommentPrefWidget mUI;
};
} // namespace wkf

#endif
