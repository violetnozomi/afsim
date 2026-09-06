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

#ifndef WKFROUTEBROWSERCREATEDIALOG_HPP
#define WKFROUTEBROWSERCREATEDIALOG_HPP

#include "wkf_common_export.h"

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>

#include "WkfRouteBrowserDialog.hpp"
#include "WkfRouteBrowserInterface.hpp"

namespace wkf
{
class RouteNameValidator : public QValidator
{
   Q_OBJECT
public:
   RouteNameValidator(std::vector<RouteBrowserInterface::RouteInfo> aRoutes, QObject* parent = nullptr)
      : QValidator(parent)
   {
      for (auto& route : aRoutes)
      {
         mRouteNames.emplace_back(route.mName);
      }
   }

   void SetState(QValidator::State aState) { mState = aState; }

   void AddRoute(const QString& aName) { mRouteNames.push_back(aName); }

   void RemoveRoute(const QString& aName)
   {
      auto r = std::find(mRouteNames.begin(), mRouteNames.end(), aName);
      if (r != mRouteNames.end())
      {
         mRouteNames.erase(r);
      }
   }

   bool Contains(const QString& aName) const
   {
      for (auto& route : mRouteNames)
      {
         if (aName == route)
         {
            return true;
         }
      }
      return false;
   }

   QValidator::State validate(QString& s, int& i) const override
   {
      QValidator::State valid = QValidator::Acceptable;

      for (auto& route : mRouteNames)
      {
         if (QString::compare(s, route) == 0)
         {
            valid = QValidator::Intermediate;
            break;
         }
      }
      if (valid != mState)
      {
         emit ValidationStatusChanged(valid);
      }
      return valid;
   }

signals:
   void ValidationStatusChanged(const QValidator::State& aState) const;

private:
   std::vector<QString> mRouteNames;
   QValidator::State    mState{QValidator::Acceptable};
};

class WKF_COMMON_EXPORT RouteBrowserCreateDialog : public RouteBrowserDialog
{
   Q_OBJECT
public:
   RouteBrowserCreateDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   virtual ~RouteBrowserCreateDialog() override;

   //! Populates the dialog with information from the selected route
   virtual void PopulateDialog();
   void         AddRouteName(const QString& aRouteName);
   void         RemoveRouteName(const QString& aRouteName);
   void         ClearSavedRoutes();

   void                                          AddPlatform(const QString& aPlatformName);
   void                                          RemovePlatform(const QString& aPlatformName);
   std::vector<RouteBrowserInterface::RouteInfo> GetSavedRoutes() const { return mSavedRoutes; }
   void AddSavedRoute(const RouteBrowserInterface::RouteInfo& aRoute) { mSavedRoutes.emplace_back(aRoute); }

   void EditGlobalRoute(const RouteBrowserInterface::RouteInfo& aRouteInfo);

signals:
   void RouteAssignedToPlatform(const QString& aPlatformName, const RouteBrowserInterface::RouteInfo& aRouteInfo);
   void GlobalRouteCreated(const RouteBrowserInterface::RouteInfo& aRouteInfo);
   void GlobalRouteEdited(const RouteBrowserInterface::RouteInfo& aRouteInfo);

protected:
   void RouteInfoInit();
   //! Clears the waypoint list in the dialog
   void    ClearTree();
   QString GetDefaultName();

   int                                           mRouteNameCount;
   QLabel*                                       mRouteNameLabel;
   QLineEdit*                                    mRouteNameEdit;
   QLabel*                                       mPlatformLabel;
   QComboBox*                                    mPlatformComboBox;
   std::vector<QString>                          mPlatforms;   // list of all platforms in the scenario
   std::vector<RouteBrowserInterface::RouteInfo> mSavedRoutes; // routes we have created
   RouteNameValidator*                           mValidator;

private:
   void ValidationStatusChangedHandler(const QValidator::State& aState) const;
   bool ApplyChanges() override;
   void closeEvent(QCloseEvent* aEvent) override;

   QPalette* mInvalidColor;
   QPalette* mValidColor;
};
} // namespace wkf
#endif
