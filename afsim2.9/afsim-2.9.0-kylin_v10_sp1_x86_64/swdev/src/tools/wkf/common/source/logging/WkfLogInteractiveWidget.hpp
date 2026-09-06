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

#ifndef WKF_LOG_TAB_WIDGET_HPP
#define WKF_LOG_TAB_WIDGET_HPP

#include <QWidget>

#include "WkfLogModel.hpp"
#include "ui_WkfLogInteractiveWidget.h"

namespace wkf
{
namespace log
{
class InteractiveWidget : public QWidget
{
   Q_OBJECT

public:
   InteractiveWidget(MasterModel& aModel, const ut::log::TypeList& aDefaults);

private slots:
   void UpdateErrorCount(int aErrors);
   void UpdateWarningCount(int aWarnings);

private:
   void ConnectFilterOptions() noexcept;
   void ConnectSortingOptions() noexcept;

   Ui::WkfLogInteractiveWidget mUi;
   FilterModel                 mModel;
};
} // namespace log
} // namespace wkf

#endif
