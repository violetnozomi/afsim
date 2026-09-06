// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfSelectorWidgets.hpp"

#include <QApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMouseEvent>

#include "UtLLAPos.hpp"
#include "VaAttachment.hpp"
#include "VaCamera.hpp"
#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPlatform.hpp"
#include "WkfTrack.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::SelectorWidgetBase::SelectorWidgetBase(QWidget* aParent, const QString& aToolTip)
   : QPushButton(aParent)
   , mSelectionActive(false)
   , mMultiSelectionEnabled(false)
{
   setIcon(QIcon::fromTheme("aim"));
   setCheckable(true);
   setToolTip(aToolTip);

   connect(this, &QPushButton::clicked, this, &SelectorWidgetBase::HandleButtonClicked);
   mEndSelectionCallback = vespa::VaObserver::ButtonClick.Connect(&SelectorWidgetBase::ClickedP, this);
   mEndSelectionCallback->Block();

   mUpdateSelectionCallback = vespa::VaObserver::UpdateCursorPosition.Connect(&SelectorWidgetBase::UpdateSelection, this);
   mUpdateSelectionCallback->Block();
}

wkf::SelectorWidgetBase::~SelectorWidgetBase()
{
   if (mSelectionActive)
   {
      Revert();
   }

   delete mEndSelectionCallback;
   delete mUpdateSelectionCallback;
}

bool wkf::SelectorWidgetBase::eventFilter(QObject* aObject, QEvent* aEvent)
{
   if (mSelectionActive)
   {
      if (aEvent->type() == QEvent::MouseButtonPress)
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(aEvent);
         if (mouseEvent)
         {
            if (mouseEvent->button() == Qt::RightButton)
            {
               CancelSelection();
               return true;
            }
            else if (mouseEvent->button() == Qt::LeftButton)
            {
               Viewer* viewer = vaEnv.GetStandardViewer();
               if (viewer)
               {
                  if (!viewer->GetMouseInViewer())
                  {
                     CancelSelection();
                     return true;
                  }
               }
            }
         }
      }
   }
   return false;
}

void wkf::SelectorWidgetBase::HandleButtonClicked()
{
   if (mSelectionActive)
   {
      CancelSelection();
   }
   else
   {
      StartSelection();
   }
}

void wkf::SelectorWidgetBase::StartSelection()
{
   mSelectionActive = true;

   qApp->installEventFilter(this);
   QApplication::restoreOverrideCursor();
   QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));

   wkfEnv.GetMainWindow()->ShowStatusMessage(toolTip());

   Viewer* viewer = vaEnv.GetStandardViewer();
   if (viewer)
   {
      viewer->SuppressMouseSelectionChanges(true);
   }

   mUpdateSelectionCallback->Unblock();
   mEndSelectionCallback->Unblock();

   emit SelectionStarted();
}

bool wkf::SelectorWidgetBase::ClickedP(vespa::VaViewer* aViewerPtr, int aButton, int aMouseX, int aMouseY, int aState)
{
   if (aButton == 1)
   {
      // Revert the cursor since a selection has been made, unless Multi-select is enabled.
      if (!mMultiSelectionEnabled)
      {
         Revert();
      }

      Clicked(aButton, aMouseX, aMouseY, aState);

      // Terminate the selection unless multi-select is enabled.
      if (!mMultiSelectionEnabled)
      {
         emit SelectionTerminated();
      }

      return true;
   }
   return false;
}

void wkf::SelectorWidgetBase::CancelSelection()
{
   Revert();
   emit SelectionTerminated();
}

void wkf::SelectorWidgetBase::Revert()
{
   mSelectionActive = false;

   if (wkfEnv.GetMainWindow())
   {
      wkfEnv.GetMainWindow()->ShowStatusMessage("");
   }

   qApp->removeEventFilter(this);
   QApplication::restoreOverrideCursor();

   mUpdateSelectionCallback->Block();
   mEndSelectionCallback->Block();

   // Uncheck the Selector button when complete
   setChecked(false);

   if (wkf::VtkEnvironment::Exists())
   {
      vespa::VaViewer* viewer = vaEnv.GetStandardViewer();
      if (viewer)
      {
         viewer->SuppressMouseSelectionChanges(false);
      }
   }
}

void wkf::LocationSelector::UpdateSelection(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY)
{
   double lat, lon, alt;
   aViewerPtr->GetCamera()->ConstConvertScreenToLLA(aMouseX, aMouseY, lat, lon, alt);
   emit LocationChanged(lat, lon);
}

void wkf::LocationSelector::Clicked(int aButton, int aMouseX, int aMouseY, int aState)
{
   Viewer* viewer = vaEnv.GetStandardViewer();
   if (viewer)
   {
      // If the user selected an entity, provide the location for the entity
      bool             signalEmitted = false;
      vespa::VaHitList hitList;
      viewer->PickFromScreen(hitList, aMouseX, aMouseY);
      vespa::VaHitEntry entry;
      if (viewer->ChooseHit(hitList, aMouseX, aMouseY, entry))
      {
         if (entry.IsEntity())
         {
            auto* entity = entry.FindEntity();
            if (entity)
            {
               const vespa::VaPosition& position = entity->GetPosition();
               emit                     Selected(position.GetLat(), position.GetLon(), position.GetAlt(), true);
               signalEmitted = true;
            }
         }
         else
         {
            auto* att = entry.FindAttachment();
            if (att)
            {
               const vespa::VaPosition& position = att->GetPosition(entry.GetSubId().mSubId);
               emit                     Selected(position.GetLat(), position.GetLon(), position.GetAlt(), true);
               signalEmitted = true;
            }
         }
      }
      // If the user did not select an entity, provide the cursor location
      if (!signalEmitted)
      {
         double lat, lon, alt;
         viewer->GetCamera()->ConstConvertScreenToLLA(aMouseX, aMouseY, lat, lon, alt);
         emit Selected(lat, lon, alt, false);
      }
   }
}

void wkf::PlatformSelector::Clicked(int aButton, int aMouseX, int aMouseY, int aState)
{
   Viewer* viewer = vaEnv.GetStandardViewer();
   if (viewer)
   {
      vespa::VaHitList hitList;
      vespa::VaHitList filteredHitList;
      viewer->PickFromScreen(hitList, aMouseX, aMouseY);
      ValidateFilter();
      int count = 0;
      for (auto& hit : hitList)
      {
         auto* platform = dynamic_cast<Platform*>(hit.FindEntity());
         if (platform)
         {
            if (Evaluate(platform))
            {
               filteredHitList.push_back(hit);
            }
            else
            {
               ++count;
            }
         }
      }

      if (count > 0)
      {
         QString msg = QString::number(count) + " item(s) ignored due to filter (" + QString::fromStdString(mFilterType) +
                       (mIsInclusive ? " = " : " != ") + QString::fromStdString(mFilterValue) + ").";

         wkfEnv.GetMainWindow()->ShowStatusMessage(msg);
      }
      vespa::VaHitEntry entry;
      if (viewer->ChooseHit(filteredHitList, aMouseX, aMouseY, entry))
      {
         auto* platform = dynamic_cast<Platform*>(entry.FindEntity());
         if (platform)
         {
            emit Selected(platform);
         }
      }
   }
}

void wkf::PlatformSelector::ValidateFilter()
{
   if (!mFilterType.empty() && mFilterType != "Side" && mFilterType != "Category" && mFilterType != "Type")
   {
      QString warning = "Invalid filter type " + QString::fromStdString(mFilterType) + ". Resetting filter.";
      QMessageBox::warning(this, "Invalid Filter Type", warning);
      mFilterType.clear();
      mFilterValue.clear();
      mIsInclusive = true;
   }
}

// aFilterType should be "Side", "Type", or "Category"
void wkf::PlatformSelector::SetFilter(const QString& aFilterType, const QString& aFilterValue, bool aIsInclusive)
{
   mFilterType  = aFilterType.toStdString();
   mFilterValue = aFilterValue.toStdString();
   mIsInclusive = aIsInclusive;
   if (!mFilterType.empty())
   {
      QString str = toolTip();
      if (!str.isEmpty())
      {
         str += "\n";
      }
      str += "Filter: " + aFilterType + (mIsInclusive ? " = " : " != ") + aFilterValue;

      setToolTip(str);
   }
}

bool wkf::PlatformSelector::Evaluate(const Platform* aPlatformPtr)
{
   bool retVal{true};
   if (!mFilterType.empty() && !mFilterValue.empty())
   {
      if (mFilterType == "Category")
      {
         auto grouping = wkfEnv.GetGroupings(aPlatformPtr->GetIndex());
         auto it       = grouping.find(PlatformGrouping(PlatformGrouping::eCategory, mFilterValue));
         retVal        = it != grouping.end();
      }
      else if (mFilterType == "Side")
      {
         retVal = aPlatformPtr->GetSide() == mFilterValue;
      }
      else if (mFilterType == "Type")
      {
         retVal = aPlatformPtr->IsOfType(mFilterValue);
      }
      else
      {
         QString warning = "Invalid filter type " + QString::fromStdString(mFilterType) + ". Resetting filter.";
         QMessageBox::warning(this, "Invalid Filter Type", warning);
         mFilterType.clear();
         mFilterValue.clear();
         mIsInclusive = true;
      }

      if (!mIsInclusive)
      {
         retVal = !retVal;
      }
   }

   return retVal;
}

void wkf::TrackSelector::Clicked(int aButton, int aMouseX, int aMouseY, int aState)
{
   Viewer* viewer = vaEnv.GetStandardViewer();
   if (viewer)
   {
      vespa::VaHitList hitList;
      vespa::VaHitList filteredHitList;
      viewer->PickFromScreen(hitList, aMouseX, aMouseY);
      ValidateFilter();
      int count = 0;
      for (auto& hit : hitList)
      {
         Track* track = dynamic_cast<Track*>(hit.FindEntity());
         if (track && Evaluate(track))
         {
            filteredHitList.push_back(hit);
         }
      }

      if (count > 0)
      {
         QString msg = QString::number(count) + " item(s) ignored due to filter (" + QString::fromStdString(mFilterType) +
                       (mIsInclusive ? " = " : " != ") + QString::fromStdString(mFilterValue) + ").";

         wkfEnv.GetMainWindow()->ShowStatusMessage(msg);
      }
      vespa::VaHitEntry entry;
      if (viewer->ChooseHit(filteredHitList, aMouseX, aMouseY, entry))
      {
         Track* track = dynamic_cast<Track*>(entry.FindEntity());
         if (track)
         {
            emit Selected(track);
         }
      }
   }
}

void wkf::TrackSelector::ValidateFilter()
{
   if (!mFilterType.empty() && mFilterType != "Side")
   {
      QString warning = "Invalid filter type " + QString::fromStdString(mFilterType) + ". Resetting filter.";
      QMessageBox::warning(this, "Invalid Filter Type", warning);
      mFilterType.clear();
      mFilterValue.clear();
      mIsInclusive = true;
   }
}

// aFilterType should be "Side"
void wkf::TrackSelector::SetFilter(const QString& aFilterType, const QString& aFilterValue, bool aIsInclusive)
{
   mFilterType  = aFilterType.toStdString();
   mFilterValue = aFilterValue.toStdString();
   mIsInclusive = aIsInclusive;
   if (!mFilterType.empty())
   {
      QString str = toolTip();
      if (!str.isEmpty())
      {
         str += "\n";
      }
      str += "Filter: " + aFilterType + (mIsInclusive ? " = " : " != ") + aFilterValue;

      setToolTip(str);
   }
}

bool wkf::TrackSelector::Evaluate(const Track* aTrackPtr)
{
   bool retVal{true};
   if (mFilterType == "Side")
   {
      retVal = aTrackPtr->GetSide() == mFilterValue;

      if (!mIsInclusive)
      {
         retVal = !retVal;
      }
   }
   else if (!mFilterType.empty())
   {
      QString warning = "Invalid filter type " + QString::fromStdString(mFilterType) + ". Resetting filter.";
      QMessageBox::warning(this, "Invalid Filter Type", warning);
      mFilterType.clear();
      mFilterValue.clear();
      mIsInclusive = true;
   }

   return retVal;
}

wkf::PlatformWidget::PlatformWidget(QWidget* aParent)
   : QWidget(aParent)
   , mSelectorPtr(new wkf::PlatformSelector())
   , mLineEdit(new QLineEdit())
{
   auto* layout = new QHBoxLayout(this);
   layout->addWidget(mLineEdit);
   layout->addWidget(mSelectorPtr);
   setLayout(layout);
   layout->setContentsMargins(0, 0, 0, 0);

   connect(mSelectorPtr,
           &wkf::PlatformSelector::Selected,
           this,
           [this](wkf::Platform* aPlatform) { SetPlatformName(QString::fromStdString(aPlatform->GetName())); });
   connect(mLineEdit, &QLineEdit::textEdited, this, &wkf::PlatformWidget::PlatformChanged);
}

wkf::TrackWidget::TrackWidget(QWidget* aParent)
   : QWidget(aParent)
   , mSelectorPtr(new wkf::TrackSelector())
   , mLineEdit(new QLineEdit())
{
   auto* layout = new QHBoxLayout(this);
   layout->addWidget(mLineEdit);
   layout->addWidget(mSelectorPtr);
   setLayout(layout);

   connect(mSelectorPtr,
           &wkf::TrackSelector::Selected,
           this,
           [this](wkf::Track* aTrack) { SetTrackId(aTrack->GetTrackId()); });
}

void wkf::TrackWidget::SetTrackId(const wkf::TrackId& aId)
{
   mLineEdit->setText(aId.toString());
}

wkf::TrackId wkf::TrackWidget::GetTrackId() const
{
   QString      text = mLineEdit->text();
   wkf::TrackId trackId(text);
   if (trackId.IsNull() && !text.isEmpty())
   {
      QMessageBox::warning(nullptr,
                           "Invalid Track Id",
                           QString("Track Id \"%1\" is not in the expected format <platform name>:<track number>").arg(text));
   }
   return trackId;
}

wkf::LocationWidget::LocationWidget(QWidget* aParent)
   : QWidget(aParent)
{
   auto* layout      = new QGridLayout(this);
   mLatLineEdit      = new UtQtLatPosLineEdit();
   mLonLineEdit      = new UtQtLonPosLineEdit();
   mAltitudeLineEdit = new QLineEdit("0");
   layout->addWidget(new QLabel("Lat"), 0, 0);
   layout->addWidget(new QLabel("Lon"), 1, 0);
   layout->addWidget(new QLabel("Alt"), 2, 0);
   layout->addWidget(mLatLineEdit, 0, 1);
   layout->addWidget(mLonLineEdit, 1, 1);
   layout->addWidget(mAltitudeLineEdit, 2, 1);
   auto* selector = new wkf::LocationSelector();
   layout->addWidget(selector, 0, 2);

   wkf::UnitsObject* unitsObject = wkfEnv.GetPreferenceObject<UnitsObject>();
   mAltitudeUnitLabel = new QLabel(QString::fromStdString(UtUnitLength::FindUnitName(unitsObject->GetLowAltitudeUnit())));
   layout->addWidget(mAltitudeUnitLabel, 2, 2);
   setLayout(layout);

   // listen for units change
   connect(unitsObject,
           &wkf::UnitsObject::LowAltitudeUnitChanged,
           this,
           [this](UtUnitLength::BaseUnit aUnit)
           { mAltitudeUnitLabel->setText(QString::fromStdString(UtUnitLength::FindUnitName(aUnit))); });

   connect(selector,
           &wkf::LocationSelector::LocationChanged,
           this,
           [this](double aLatitude, double aLongitude)
           {
              mLatLineEdit->SetValue(aLatitude);
              mLonLineEdit->SetValue(aLongitude);
           });

   connect(selector,
           &wkf::LocationSelector::Selected,
           this,
           [this](double aLatitude, double aLongitude, double aAltitude_m, bool aAltitudeValid)
           {
              mLatLineEdit->SetValue(aLatitude);
              mLonLineEdit->SetValue(aLongitude);

              if (aAltitudeValid)
              {
                 UtUnitLength::BaseUnit unit = wkfEnv.GetPreferenceObject<UnitsObject>()->GetAltitudeUnit(aAltitude_m);
                 mAltitudeUnitLabel->setText(QString::fromStdString(UtUnitLength::FindUnitName(unit)));
                 mAltitudeLineEdit->setText(QString::number(UtUnitLength::ConvertFromStandard(aAltitude_m, unit)));
              }
           });
}

QVector<double> wkf::LocationWidget::GetLocation() const
{
   QVector<double> retVal;
   retVal.push_back(mLatLineEdit->GetValue());
   retVal.push_back(mLonLineEdit->GetValue());
   // Convert the altitude to the standard Length unit (meters)
   retVal.push_back(UtUnitLength::ConvertToStandard(mAltitudeLineEdit->text().toDouble(),
                                                    UtUnitLength::ReadUnit(mAltitudeUnitLabel->text().toStdString())));
   return retVal;
}

void wkf::LocationWidget::SetLocation(const QVector<double>& aLLA)
{
   mLatLineEdit->SetValue(aLLA[0]);
   mLonLineEdit->SetValue(aLLA[1]);

   UtUnitLength::BaseUnit unit = wkfEnv.GetPreferenceObject<UnitsObject>()->GetAltitudeUnit(aLLA[2]);
   mAltitudeUnitLabel->setText(QString::fromStdString(UtUnitLength::FindUnitName(unit)));
   mAltitudeLineEdit->setText(QString::number(UtUnitLength::ConvertFromStandard(aLLA[2], unit)));
}
