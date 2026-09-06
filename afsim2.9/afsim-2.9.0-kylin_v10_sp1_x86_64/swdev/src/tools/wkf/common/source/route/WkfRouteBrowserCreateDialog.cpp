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

#include "WkfRouteBrowserCreateDialog.hpp"

#include <QMessageBox>
#include <QPalette>

#include "UtEarth.hpp"
#include "WkfEnvironment.hpp"
#include "ui_WkfRouteBrowserDialog.h"

wkf::RouteBrowserCreateDialog::RouteBrowserCreateDialog(QWidget* aParent /*= 0*/, Qt::WindowFlags aFlags /*= 0*/)
   : RouteBrowserDialog(aParent, aFlags)
   , mRouteNameCount(1)
{
   setWindowTitle("New Route");

   mRouteNameLabel   = new QLabel("Route Name:", this);
   mRouteNameEdit    = new QLineEdit(this);
   mPlatformComboBox = new QComboBox();

   mUi.waypointPropertiesSheet->setWidget(0, QFormLayout::LabelRole, mRouteNameLabel);
   mUi.waypointPropertiesSheet->setWidget(0, QFormLayout::FieldRole, mRouteNameEdit);

   mUi.waypointPropertiesSheet->addRow("Assign to Platform:", mPlatformComboBox);

   mValidColor   = new QPalette(aParent->palette());
   mInvalidColor = new QPalette(aParent->palette());
   mInvalidColor->setColor(QPalette::Text, Qt::red);

   mUi.buttonBox->button(QDialogButtonBox::Ok)->setText("Create");

   mValidator = new RouteNameValidator(mSavedRoutes);
   connect(mValidator,
           &RouteNameValidator::ValidationStatusChanged,
           this,
           &RouteBrowserCreateDialog::ValidationStatusChangedHandler);
}

wkf::RouteBrowserCreateDialog::~RouteBrowserCreateDialog()
{
   delete mRouteNameLabel;
   delete mRouteNameEdit;

   delete mValidator;
   delete mValidColor;
   delete mInvalidColor;
}

// Add aRouteName to the route validator to prevent it being used again
void wkf::RouteBrowserCreateDialog::AddRouteName(const QString& aRouteName)
{
   if (!mValidator->Contains(aRouteName))
   {
      mValidator->AddRoute(aRouteName);
   }
}

// Remove aRouteName from the route validator to enable its use again
void wkf::RouteBrowserCreateDialog::RemoveRouteName(const QString& aRouteName)
{
   mValidator->RemoveRoute(aRouteName);

   auto r = std::find_if(mSavedRoutes.begin(),
                         mSavedRoutes.end(),
                         [aRouteName](const wkf::RouteBrowserInterface::RouteInfo& routeInfo)
                         { return routeInfo.mName == aRouteName; });

   if (r != mSavedRoutes.end())
   {
      mSavedRoutes.erase(r);
   }
}

void wkf::RouteBrowserCreateDialog::ClearSavedRoutes()
{
   std::vector<QString> routeNames;
   for (auto& r : mSavedRoutes)
   {
      routeNames.push_back(r.mName);
   }

   for (auto& n : routeNames)
   {
      RemoveRouteName(n);
   }

   mRouteNameCount = 1;
}

// Populate the new route dialog with an initial route
void wkf::RouteBrowserCreateDialog::PopulateDialog()
{
   mRouteNameEdit->setValidator(mValidator);

   mCurrentRoute = RouteBrowserInterface::RouteInfo();

   mPlatformComboBox->clear();
   mPlatformComboBox->addItem("none");
   for (auto& p : mPlatforms)
   {
      mPlatformComboBox->addItem(p);
   }

   RouteInfoInit(); // initialize all fields with "0" values
   ClearTree();     // clear left side of dialog

   // if there is only one platform currently selected go ahead and pre-set the platform combo box to this platform
   // assuming the user wants to create a route for this selected platform: also populate the first waypoint with
   // position if its not 0,0
   wkf::PlatformList selectedPlatforms = wkfEnv.GetSelectedPlatforms();
   if (selectedPlatforms.size() == 1)
   {
      mPlatformComboBox->setCurrentText(selectedPlatforms[0]->GetName().c_str());
      vespa::VaPosition pos = selectedPlatforms[0]->GetPosition();
      if (pos.GetLat() != 0 || pos.GetLon() != 0)
      {
         emit CreateRouteStarted(QString::fromStdString(selectedPlatforms[0]->GetName()));
         AppendWaypoint(pos.GetLat(), pos.GetLon(), pos.GetAlt());
      }
   }
   show();
}

// Edit route in the list of saved global routes
void wkf::RouteBrowserCreateDialog::EditGlobalRoute(const RouteBrowserInterface::RouteInfo& aRouteInfo)
{
   emit GlobalRouteEdited(aRouteInfo);
   for (auto& route : mSavedRoutes)
   {
      if (QString::compare(route.mName, aRouteInfo.mName) == 0)
      {
         route = aRouteInfo;
         break;
      }
   }
}

// Add aPlatformName to the list of platforms in the "assign to route" combobox
void wkf::RouteBrowserCreateDialog::AddPlatform(const QString& aPlatformName)
{
   mPlatforms.push_back(aPlatformName);
}

// Remove aPlatformName from the list of platforms in the "assign to route" combobox
void wkf::RouteBrowserCreateDialog::RemovePlatform(const QString& aPlatformName)
{
   auto iter = std::find(mPlatforms.begin(), mPlatforms.end(), aPlatformName);
   if (iter != mPlatforms.end())
   {
      mPlatforms.erase(iter);
   }
}

void wkf::RouteBrowserCreateDialog::RouteInfoInit()
{
   mCurrentRoute.mName = GetDefaultName();
   mRouteNameEdit->setText(mCurrentRoute.mName);
}

void wkf::RouteBrowserCreateDialog::ClearTree()
{
   mUi.treeWidget->clear();
}

QString wkf::RouteBrowserCreateDialog::GetDefaultName()
{
   QString name;
   do
   {
      name = mDefaultRouteNamePrefix + QString::number(mRouteNameCount);
      mRouteNameCount++;
   } while (mValidator->Contains(name));
   return name;
}

void wkf::RouteBrowserCreateDialog::ValidationStatusChangedHandler(const QValidator::State& aState) const
{
   mValidator->SetState(aState);
   if (aState == QValidator::Acceptable)
   {
      mRouteNameEdit->setPalette(*mValidColor);
   }
   else
   {
      mRouteNameEdit->setPalette(*mInvalidColor);
   }
}

bool wkf::RouteBrowserCreateDialog::ApplyChanges()
{
   bool retVal = false;
   if (mRouteNameEdit->text().isEmpty())
   {
      QMessageBox::warning(nullptr, "Route Name", QString("Route must be given a name"), QMessageBox::Ok);
      mRouteNameEdit->setFocus();
   }
   else if (!mRouteNameEdit->hasAcceptableInput())
   {
      QMessageBox::warning(nullptr,
                           "Route Name",
                           QString("Route name \"%1\" already in use").arg(mRouteNameEdit->text()),
                           QMessageBox::Ok);
      mRouteNameEdit->setFocus();
   }
   else if (mCurrentRoute.mWaypoints.empty())
   {
      QMessageBox::warning(nullptr, "Empty Route", QString("Route must contain waypoints."), QMessageBox::Ok);
   }
   else if (CheckValidWaypointProperties())
   {
      mCurrentRoute.mName = mRouteNameEdit->text();

      mSavedRoutes.push_back(mCurrentRoute); // add route to mSavedRoutes

      // if route was assigned to platform, emit signal for sim
      if (mPlatformComboBox->currentText().compare("none") != 0)
      {
         emit RouteAssignedToPlatform(mPlatformComboBox->currentText(), mCurrentRoute);
      }
      else
      {
         emit GlobalRouteCreated(mCurrentRoute); // tell dock widget to add new route to Global Routes
      }
      if (!mCurrentRoute.mName.isEmpty())
      {
         mValidator->AddRoute(mCurrentRoute.mName);
      }
      if (mCurrentRoute.mName == mDefaultRouteNamePrefix + QString::number(mRouteNameCount))
      {
         ++mRouteNameCount;
      }
      retVal = true;
   }

   if (retVal)
   {
      accept();
   }
   return retVal;
}

void wkf::RouteBrowserCreateDialog::closeEvent(QCloseEvent* aEvent)
{
   CreateRouteTerminated(mCurrentRoute.mName);
   if (result() != QDialog::Accepted)
   {
      --mRouteNameCount;
   }
   QDialog::reject();
}
