// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "TargetSelectionConfigWidget.hpp"

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "OrbitalEventConfigKeys.hpp"
#include "WkfPlatform.hpp"
#include "WkfSelectorWidgets.hpp"
#include "WkfTrack.hpp"
#include "WsfLibrationPoint.hpp"

namespace
{

const QString cTARGETSELECTIONTYPE{"target_selection"};

}

namespace wsfg
{

namespace spaceg
{

PlatformSelectionWidget::PlatformSelectionWidget(QWidget* aParentPtr)
   : QWidget(aParentPtr)
   , mValuePtr{new QLineEdit{}}
   , mSelectorPtr{new wkf::PlatformSelector{}}
{
   QHBoxLayout* valueLayoutPtr = new QHBoxLayout{};
   valueLayoutPtr->addWidget(mValuePtr, 1);
   valueLayoutPtr->addWidget(mSelectorPtr, 0);
   setLayout(valueLayoutPtr);

   connect(mSelectorPtr, &wkf::PlatformSelector::Selected, this, &PlatformSelectionWidget::HandlePlatformSelection);
}

QString PlatformSelectionWidget::GetPlatformName() const
{
   return mValuePtr->text();
}

void PlatformSelectionWidget::SetPlatformName(const QString& aPlatformName)
{
   mValuePtr->setText(aPlatformName);
}

void PlatformSelectionWidget::Clear()
{
   SetPlatformName(QString(""));
}

void PlatformSelectionWidget::HandlePlatformSelection(wkf::Platform* aPlatformPtr)
{
   if (aPlatformPtr)
   {
      mValuePtr->setText(QString::fromStdString(aPlatformPtr->GetName()));
   }
}

TrackSelectionWidget::TrackSelectionWidget(QWidget* aParentPtr)
   : QWidget(aParentPtr)
   , mValuePtr{new QLineEdit{}}
   , mSelectorPtr{new wkf::TrackSelector{}}
{
   QHBoxLayout* valueLayoutPtr = new QHBoxLayout{};
   valueLayoutPtr->addWidget(mValuePtr, 1);
   valueLayoutPtr->addWidget(mSelectorPtr, 0);
   setLayout(valueLayoutPtr);

   connect(mSelectorPtr, &wkf::TrackSelector::Selected, this, &TrackSelectionWidget::HandleTrackSelection);
}

QString TrackSelectionWidget::GetTrackString() const
{
   return mValuePtr->text();
}

void TrackSelectionWidget::SetTrackString(const QString& aTrackString)
{
   wkf::TrackId track(aTrackString);
   if (track.IsNull())
   {
      Clear();
   }
   else
   {
      mValuePtr->setText(track.toString());
   }
}

void TrackSelectionWidget::Clear()
{
   mValuePtr->setText(QString(""));
}

void TrackSelectionWidget::HandleTrackSelection(wkf::Track* aTrackPtr)
{
   if (aTrackPtr)
   {
      mValuePtr->setText(aTrackPtr->GetTrackId().toString());
   }
}

LibrationPointSelectionWidget::LibrationPointSelectionWidget(QWidget* aParentPtr)
   : QWidget(aParentPtr)
   , mSystemComboBoxPtr{new QComboBox{}}
   , mPointComboBoxPtr{new QComboBox{}}
{
   SetupSystemWidget();
   SetupPointWidget();

   QHBoxLayout* layoutPtr = new QHBoxLayout{};
   layoutPtr->addWidget(mSystemComboBoxPtr, 3);
   layoutPtr->addWidget(mPointComboBoxPtr, 1);
   setLayout(layoutPtr);
}

QString LibrationPointSelectionWidget::GetSystem() const
{
   return mSystemComboBoxPtr->currentText();
}

QString LibrationPointSelectionWidget::GetPoint() const
{
   return mPointComboBoxPtr->currentText();
}

void LibrationPointSelectionWidget::SetSystem(const QString& aSystem)
{
   auto index = mSystemComboBoxPtr->findText(aSystem);
   if (index != -1)
   {
      mSystemComboBoxPtr->setCurrentIndex(index);
   }
}

void LibrationPointSelectionWidget::SetPoint(const QString& aPoint)
{
   auto index = mPointComboBoxPtr->findText(aPoint);
   if (index != -1)
   {
      mPointComboBoxPtr->setCurrentIndex(index);
   }
}

void LibrationPointSelectionWidget::Clear()
{
   mSystemComboBoxPtr->setCurrentIndex(-1);
   mPointComboBoxPtr->setCurrentIndex(-1);
}

void LibrationPointSelectionWidget::SetupSystemWidget()
{
   auto vals = wsf::space::LibrationPoint::GetSystemIdentifiers();
   for (auto& system : vals)
   {
      mSystemComboBoxPtr->addItem(QString::fromStdString(system));
   }
}

void LibrationPointSelectionWidget::SetupPointWidget()
{
   auto vals = wsf::space::LibrationPoint::GetPointIdentifiers();
   for (auto& point : vals)
   {
      mPointComboBoxPtr->addItem(QString::fromStdString(point));
   }
}

KinematicStateSelectionWidget::KinematicStateSelectionWidget(QWidget* aParentPtr)
   : QWidget{aParentPtr}
   , mPositionX_Ptr{new UtQtLengthValueEdit{this}}
   , mPositionY_Ptr{new UtQtLengthValueEdit{this}}
   , mPositionZ_Ptr{new UtQtLengthValueEdit{this}}
   , mVelocityX_Ptr{new UtQtSpeedValueEdit{this}}
   , mVelocityY_Ptr{new UtQtSpeedValueEdit{this}}
   , mVelocityZ_Ptr{new UtQtSpeedValueEdit{this}}
{
   QVBoxLayout* overallLayoutPtr = new QVBoxLayout{};
   QLabel*      positionLabelPtr = new QLabel{QString{"ECI Position:"}};
   overallLayoutPtr->addWidget(positionLabelPtr);
   QHBoxLayout* positionComponentLayoutPtr = new QHBoxLayout{};
   positionComponentLayoutPtr->setSpacing(5);
   positionComponentLayoutPtr->addWidget(mPositionX_Ptr);
   positionComponentLayoutPtr->addWidget(mPositionY_Ptr);
   positionComponentLayoutPtr->addWidget(mPositionZ_Ptr);
   overallLayoutPtr->addItem(positionComponentLayoutPtr);
   QLabel* velocityLabelPtr = new QLabel{QString{"ECI Velocity:"}};
   overallLayoutPtr->addWidget(velocityLabelPtr);
   QHBoxLayout* velocityComponentLayoutPtr = new QHBoxLayout{};
   velocityComponentLayoutPtr->setSpacing(5);
   velocityComponentLayoutPtr->addWidget(mVelocityX_Ptr);
   velocityComponentLayoutPtr->addWidget(mVelocityY_Ptr);
   velocityComponentLayoutPtr->addWidget(mVelocityZ_Ptr);
   overallLayoutPtr->addItem(velocityComponentLayoutPtr);
   setLayout(overallLayoutPtr);
}

QString KinematicStateSelectionWidget::GetX_Position() const
{
   return mPositionX_Ptr->text();
}

QString KinematicStateSelectionWidget::GetY_Position() const
{
   return mPositionY_Ptr->text();
}

QString KinematicStateSelectionWidget::GetZ_Position() const
{
   return mPositionZ_Ptr->text();
}

QString KinematicStateSelectionWidget::GetX_Velocity() const
{
   return mVelocityX_Ptr->text();
}

QString KinematicStateSelectionWidget::GetY_Velocity() const
{
   return mVelocityY_Ptr->text();
}

QString KinematicStateSelectionWidget::GetZ_Velocity() const
{
   return mVelocityZ_Ptr->text();
}

bool KinematicStateSelectionWidget::HasVelocity() const
{
   return !(GetX_Velocity().isEmpty() && GetY_Velocity().isEmpty() && GetZ_Velocity().isEmpty());
}

void KinematicStateSelectionWidget::SetX_Position(const QString& aPosition)
{
   mPositionX_Ptr->setText(aPosition);
}

void KinematicStateSelectionWidget::SetY_Position(const QString& aPosition)
{
   mPositionY_Ptr->setText(aPosition);
}

void KinematicStateSelectionWidget::SetZ_Position(const QString& aPosition)
{
   mPositionZ_Ptr->setText(aPosition);
}

void KinematicStateSelectionWidget::SetX_Velocity(const QString& aVelocity)
{
   mVelocityX_Ptr->setText(aVelocity);
}

void KinematicStateSelectionWidget::SetY_Velocity(const QString& aVelocity)
{
   mVelocityY_Ptr->setText(aVelocity);
}

void KinematicStateSelectionWidget::SetZ_Velocity(const QString& aVelocity)
{
   mVelocityZ_Ptr->setText(aVelocity);
}

void KinematicStateSelectionWidget::Clear()
{
   mPositionX_Ptr->clear();
   mPositionY_Ptr->clear();
   mPositionZ_Ptr->clear();
   mVelocityX_Ptr->clear();
   mVelocityY_Ptr->clear();
   mVelocityZ_Ptr->clear();
}

TargetSelectionConfigWidget::TargetSelectionConfigWidget(bool aAllowTrackSelection, QWidget* aParentPtr)
   : ConfigWidget(aParentPtr)
{
   QGroupBox*   groupBoxPtr = new QGroupBox{QString{"Target Type"}};
   QHBoxLayout* gbLayoutPtr = new QHBoxLayout{};
   gbLayoutPtr->addWidget(groupBoxPtr);
   setLayout(gbLayoutPtr);

   mComboBoxPtr           = new QComboBox{};
   mStackedWidgetPtr      = new QStackedWidget{};
   QVBoxLayout* layoutPtr = new QVBoxLayout{};
   layoutPtr->addWidget(mComboBoxPtr);
   layoutPtr->addWidget(mStackedWidgetPtr);
   if (aAllowTrackSelection)
   {
      SetupTrackSelectionPanel();
   }
   SetupPlatformSelectionPanel();
   SetupKinematicStateSelectionPanel();
   groupBoxPtr->setLayout(layoutPtr);

   // The 0th panel, whatever it happens to be, will be shown in the stacked widget
   // and the 0th index will be selected in the combo box. This call to ChangePanel
   // assures that the correct size policy is set on the active panel of the
   // stacked widget.
   ChangePanel(0);

   connect(mComboBoxPtr, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TargetSelectionConfigWidget::ChangePanel);
}

void TargetSelectionConfigWidget::Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration)
{
   if (ValidateObject(aConfiguration))
   {
      mPropertyName = aConfiguration.constFind(cNAME)->toString();

      if (aConfiguration.contains(EventConfigKeys::cALLOW_LP_TARGET))
      {
         if (aConfiguration.constFind(EventConfigKeys::cALLOW_LP_TARGET)->toBool())
         {
            SetupLibrationPointSelectionPanel();
         }
      }
      else
      {
         SetupLibrationPointSelectionPanel();
      }
   }
}

QJsonObject TargetSelectionConfigWidget::GetValues() const
{
   QJsonObject group{};
   QWidget*    currentWidget = mStackedWidgetPtr->currentWidget();
   if (currentWidget == mPlatformWidgetPtr)
   {
      group[EventConfigKeys::cPLATFORM] = mPlatformWidgetPtr->GetPlatformName();
   }
   else if (currentWidget == mTrackWidgetPtr)
   {
      group[EventConfigKeys::cTRACK] = mTrackWidgetPtr->GetTrackString();
   }
   else if (currentWidget == mLibrationWidgetPtr)
   {
      group[EventConfigKeys::cLIBRATION_SYSTEM] = mLibrationWidgetPtr->GetSystem();
      group[EventConfigKeys::cLIBRATION_POINT]  = mLibrationWidgetPtr->GetPoint();
   }
   else if (currentWidget == mKinematicStateWidgetPtr)
   {
      QJsonObject kinematicState{};

      QJsonObject position{};
      position[EventConfigKeys::cX_COMPONENT]          = mKinematicStateWidgetPtr->GetX_Position();
      position[EventConfigKeys::cY_COMPONENT]          = mKinematicStateWidgetPtr->GetY_Position();
      position[EventConfigKeys::cZ_COMPONENT]          = mKinematicStateWidgetPtr->GetZ_Position();
      kinematicState[EventConfigKeys::cSTATE_POSITION] = position;

      if (mKinematicStateWidgetPtr->HasVelocity())
      {
         QJsonObject velocity{};
         velocity[EventConfigKeys::cX_COMPONENT]          = mKinematicStateWidgetPtr->GetX_Velocity();
         velocity[EventConfigKeys::cY_COMPONENT]          = mKinematicStateWidgetPtr->GetY_Velocity();
         velocity[EventConfigKeys::cZ_COMPONENT]          = mKinematicStateWidgetPtr->GetZ_Velocity();
         kinematicState[EventConfigKeys::cSTATE_VELOCITY] = velocity;
      }

      group[EventConfigKeys::cKINEMATIC_STATE] = kinematicState;
   }

   QJsonObject retval{};
   retval[mPropertyName] = group;
   return retval;
}

bool TargetSelectionConfigWidget::SetValues(const QJsonObject& aData)
{
   bool retval{false};

   auto iter = aData.constFind(mPropertyName);
   if (iter != aData.constEnd())
   {
      QJsonObject groupData = iter->toObject();
      if (!groupData.empty())
      {
         auto platIter  = groupData.constFind(EventConfigKeys::cPLATFORM);
         auto trackIter = groupData.constFind(EventConfigKeys::cTRACK);
         auto stateIter = groupData.constFind(EventConfigKeys::cKINEMATIC_STATE);

         if (platIter != groupData.constEnd() && platIter->isString())
         {
            if (mPlatformPanelIndex != -1)
            {
               mComboBoxPtr->setCurrentIndex(mPlatformPanelIndex);
               mPlatformWidgetPtr->SetPlatformName(platIter->toString());
               retval = true;
            }
         }
         else if (trackIter != groupData.constEnd() && trackIter->isString())
         {
            if (mTrackPanelIndex != -1)
            {
               mComboBoxPtr->setCurrentIndex(mTrackPanelIndex);
               mTrackWidgetPtr->SetTrackString(trackIter->toString());
               retval = true;
            }
         }
         else if (stateIter != groupData.constEnd() && stateIter->isObject())
         {
            if (mKinematicStatePanelIndex != -1)
            {
               mComboBoxPtr->setCurrentIndex(mKinematicStatePanelIndex);
               QJsonObject state   = stateIter->toObject();
               auto        posIter = state.constFind(EventConfigKeys::cSTATE_POSITION);
               if (posIter != state.constEnd() && posIter->isObject())
               {
                  QJsonObject position  = posIter->toObject();
                  auto        xCompIter = position.constFind(EventConfigKeys::cX_COMPONENT);
                  if (xCompIter != position.constEnd() && xCompIter->isString())
                  {
                     mKinematicStateWidgetPtr->SetX_Position(xCompIter->toString());
                  }
                  auto yCompIter = position.constFind(EventConfigKeys::cY_COMPONENT);
                  if (yCompIter != position.constEnd() && yCompIter->isString())
                  {
                     mKinematicStateWidgetPtr->SetY_Position(yCompIter->toString());
                  }
                  auto zCompIter = position.constFind(EventConfigKeys::cZ_COMPONENT);
                  if (zCompIter != position.constEnd() && zCompIter->isString())
                  {
                     mKinematicStateWidgetPtr->SetZ_Position(zCompIter->toString());
                  }
               }

               auto velIter = state.constFind(EventConfigKeys::cSTATE_VELOCITY);
               if (velIter != state.constEnd() && velIter->isObject())
               {
                  QJsonObject velocity  = velIter->toObject();
                  auto        xCompIter = velocity.constFind(EventConfigKeys::cX_COMPONENT);
                  if (xCompIter != velocity.constEnd() && xCompIter->isString())
                  {
                     mKinematicStateWidgetPtr->SetX_Velocity(xCompIter->toString());
                  }
                  auto yCompIter = velocity.constFind(EventConfigKeys::cY_COMPONENT);
                  if (yCompIter != velocity.constEnd() && yCompIter->isString())
                  {
                     mKinematicStateWidgetPtr->SetY_Velocity(yCompIter->toString());
                  }
                  auto zCompIter = velocity.constFind(EventConfigKeys::cZ_COMPONENT);
                  if (zCompIter != velocity.constEnd() && zCompIter->isString())
                  {
                     mKinematicStateWidgetPtr->SetZ_Velocity(zCompIter->toString());
                  }
               }
            }
         }
         else
         {
            auto systemIter = groupData.constFind(EventConfigKeys::cLIBRATION_SYSTEM);
            auto pointIter  = groupData.constFind(EventConfigKeys::cLIBRATION_POINT);
            if (systemIter != groupData.constEnd() && systemIter->isString() && pointIter != groupData.constEnd() &&
                pointIter->isString())
            {
               if (mLibrationPanelIndex != -1)
               {
                  mComboBoxPtr->setCurrentIndex(mLibrationPanelIndex);
                  mLibrationWidgetPtr->SetSystem(systemIter->toString());
                  mLibrationWidgetPtr->SetPoint(pointIter->toString());
                  retval = true;
               }
            }
         }
      }
   }

   return retval;
}

void TargetSelectionConfigWidget::ClearValues()
{
   mComboBoxPtr->setCurrentIndex(0);
   mPlatformWidgetPtr->Clear();
   if (mTrackWidgetPtr)
   {
      mTrackWidgetPtr->Clear();
   }
   if (mLibrationWidgetPtr)
   {
      mLibrationWidgetPtr->Clear();
   }
   if (mKinematicStateWidgetPtr)
   {
      mKinematicStateWidgetPtr->Clear();
   }
}

const QString& TargetSelectionConfigWidget::Type()
{
   return cTARGETSELECTIONTYPE;
}

bool TargetSelectionConfigWidget::ValidateObject(const QJsonObject& aConfiguration)
{
   return aConfiguration.constFind(cTYPE)->toString() == cTARGETSELECTIONTYPE &&
          aConfiguration.constFind(cNAME)->isString();
}

void TargetSelectionConfigWidget::SetupPlatformSelectionPanel()
{
   mPlatformWidgetPtr = new PlatformSelectionWidget{};
   mPlatformWidgetPtr->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   mPlatformPanelIndex = mStackedWidgetPtr->addWidget(mPlatformWidgetPtr);
   mComboBoxPtr->addItem(QString("Platform"));
}

void TargetSelectionConfigWidget::SetupTrackSelectionPanel()
{
   mTrackWidgetPtr = new TrackSelectionWidget{};
   mTrackWidgetPtr->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   mTrackPanelIndex = mStackedWidgetPtr->addWidget(mTrackWidgetPtr);
   mComboBoxPtr->addItem(QString("Track"));
}

void TargetSelectionConfigWidget::SetupLibrationPointSelectionPanel()
{
   mLibrationWidgetPtr = new LibrationPointSelectionWidget{};
   mLibrationWidgetPtr->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   mLibrationPanelIndex = mStackedWidgetPtr->addWidget(mLibrationWidgetPtr);
   mComboBoxPtr->addItem(QString("Libration point"));
}

void TargetSelectionConfigWidget::SetupKinematicStateSelectionPanel()
{
   mKinematicStateWidgetPtr = new KinematicStateSelectionWidget{};
   mKinematicStateWidgetPtr->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   mKinematicStatePanelIndex = mStackedWidgetPtr->addWidget(mKinematicStateWidgetPtr);
   mComboBoxPtr->addItem(QString("Kinematic state"));
}

void TargetSelectionConfigWidget::ChangePanel(int aIndex)
{
   mStackedWidgetPtr->currentWidget()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   mStackedWidgetPtr->setCurrentIndex(aIndex);
   mStackedWidgetPtr->currentWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
   mStackedWidgetPtr->adjustSize();
   adjustSize();
}

} // namespace spaceg

} // namespace wsfg
