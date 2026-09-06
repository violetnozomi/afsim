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

#ifndef WKFTYPEVISIBILITYWIDGET_HPP
#define WKFTYPEVISIBILITYWIDGET_HPP

#include "wkf_common_export.h"

#include <set>

#include <QWidget>

#include "VaCallbackHolder.hpp"
#include "ui_WkfTypeVisibilityWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT TypeVisibilityWidget : public QWidget
{
   Q_OBJECT
public:
   TypeVisibilityWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   ~TypeVisibilityWidget() override = default;

   void Update();

private:
   void PopulateDisplay();
   void CreateContextMenu(const QPoint& aPoint);
   void FilterChanged(const QString& aText);

   bool                        mModified{false};
   std::set<std::string>       mTypeSet;
   vespa::VaCallbackHolder     mCallbacks;
   Ui::WkfTypeVisibilityWidget mUI;
};
} // namespace wkf
#endif
