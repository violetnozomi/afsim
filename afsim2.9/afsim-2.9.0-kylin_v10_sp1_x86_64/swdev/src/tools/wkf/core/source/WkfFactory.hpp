// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFVAFACTORY_HPP
#define WKFVAFACTORY_HPP

class QDockWidget;
class QMainWindow;
class QStringList;
class QWidget;
#include "wkf_export.h"

#include "VaFactory.hpp"

#ifdef CreateDialog
#undef CreateDialog
#endif

namespace wkf
{
class Dialog;
class MainWindow;

class WKF_EXPORT Factory : public vespa::VaFactory
{
public:
   Factory()           = default;
   ~Factory() override = default;

   virtual Dialog* CreateDialog(const std::string& aDialogType, MainWindow* aParent = nullptr) const;

   template<class T>
   T*                   CreateDockWidget(const std::string& aDockWidgetType, QMainWindow* aParent = nullptr) const;
   virtual QDockWidget* CreateDockWidget(const std::string& aDockWidgetType, QMainWindow* aParent = nullptr) const;

   virtual QWidget* CreateWidget(const std::string& aWidgetType, QMainWindow* aParent = nullptr) const;
   virtual void     UserOpenFile(const QStringList& aFileNames) const {}

protected:
   vespa::VaEntity*     UserCreateEntity(const std::string& aEntityType) const override;
   vespa::VaScenario*   UserCreateScenario(const std::string& aScenarioType) const override;
   vespa::VaOverlay*    UserCreateOverlay(const std::string& aOverlayType,
                                          const std::string& aOverlayName /* = "" */) const override;
   vespa::VaAttachment* UserCreateAttachment(const std::string& aAttachmentType,
                                             vespa::VaEntity&   aEntity,
                                             vespa::VaViewer*   aViewerPtr) const override;
};

template<class T>
T* Factory::CreateDockWidget(const std::string& aDockWidgetType, QMainWindow* aParent /* = nullptr */) const
{
   return static_cast<T*>(CreateDockWidget(aDockWidgetType, aParent));
}
} // namespace wkf
#endif
