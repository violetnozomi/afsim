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
#ifndef RVFACTORY_HPP
#define RVFACTORY_HPP

#include "RvExport.hpp"
#include "WkfFactory.hpp"

class QMainWindow;

namespace rv
{
class RV_EXPORT Factory : public wkf::Factory
{
public:
   Factory()           = default;
   ~Factory() override = default;

protected:
#undef CreateDialog // Avoid conflict with Windows macro
   wkf::Dialog* CreateDialog(const std::string& aDialogType, wkf::MainWindow* aParent = nullptr) const override;
   QDockWidget* CreateDockWidget(const std::string& aDockWidgetType, QMainWindow* aParent = nullptr) const override;
   QWidget*     CreateWidget(const std::string& aWidgetType, QMainWindow* aParent = nullptr) const override;
   void         UserOpenFile(const QStringList& aFileNames) const override;
   vespa::VaOverlay* UserCreateOverlay(const std::string& aOverlayType, const std::string& aOverlayName = "") const override;
};
} // namespace rv
#endif
