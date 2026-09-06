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
#include "RvTimeControllerToolbar.hpp"

#include <functional>

#include <QAction>
#include <QInputDialog>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QStylePainter>
#include <QTimer>

#include "RvEnvironment.hpp"
#include "RvTimeControllerInterface.hpp"
#include "UtMath.hpp"
#include "WkfAction.hpp"
#include "WkfMainWindow.hpp"

RvTimeController::Toolbar::Toolbar(RvTimeController::Interface* aInterfacePtr)
   : QToolBar("Time Controller")
   , mDeferUpdatesToTime(false)
   , mPressed(false)
{
   setIconSize(QSize(16, 16));
   setEnabled(true);
   setObjectName("TimeControllerToolbar");

   mMax  = 0.001;
   mTime = 0.0;

   // Add actions to toolbar to create tool buttons
   mAdvanceTimeActionPtr = new wkf::Action(QIcon::fromTheme("time"), "Jump To Time", aInterfacePtr);
   addAction(mAdvanceTimeActionPtr);
   mFastForwardActionPtr = new wkf::Action(QIcon::fromTheme("fast_forward"), "Set Clock Rate", aInterfacePtr);
   addAction(mFastForwardActionPtr);
   connect(mFastForwardActionPtr, &QAction::triggered, this, &RvTimeController::Toolbar::SetClockRateTriggered);
   addSeparator();

   mRestartActionPtr =
      new wkf::Action(QIcon::fromTheme("restart"), "Restart", aInterfacePtr, QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_F5));
   addAction(mRestartActionPtr);
   connect(mRestartActionPtr, &QAction::triggered, aInterfacePtr, &Interface::SendRestartCommand);

   // Other applications with a controller toolbar only have a forward and pause state, so a one-key state toggle works.
   // This is different in that there are 3 interchangeable states, so separate bindings make more sense
   mReverseActionPtr = new wkf::Action(QIcon::fromTheme("reverse"), "Reverse", aInterfacePtr);
   addAction(mReverseActionPtr);
   connect(mReverseActionPtr, &QAction::triggered, [this]() { GuiSimulationReversing(); });
   connect(mReverseActionPtr, &QAction::triggered, aInterfacePtr, &Interface::SendReverseCommand);
   mPauseActionPtr = new wkf::Action(QIcon::fromTheme("pause"), "Pause", aInterfacePtr);
   addAction(mPauseActionPtr);
   connect(mPauseActionPtr, &QAction::triggered, [this]() { GuiSimulationPausing(); });
   connect(mPauseActionPtr, &QAction::triggered, aInterfacePtr, &Interface::SendPauseCommand);
   mResumeActionPtr = new wkf::Action(QIcon::fromTheme("play"), "Resume", aInterfacePtr);
   addAction(mResumeActionPtr);
   connect(mResumeActionPtr, &QAction::triggered, [this]() { GuiSimulationResuming(); });
   connect(mResumeActionPtr, &QAction::triggered, aInterfacePtr, &Interface::SendResumeCommand);
   SetupPlaybackShortcuts();

   mSliderPtr = new RvTimeController::CachedSlider(this);
   addWidget(mSliderPtr);
   mSliderPtr->setRange(0, 2000);

   mPrevBookmarkActionPtr =
      new wkf::Action(QIcon::fromTheme("skip_last"), "Previous bookmark", this, QKeySequence(Qt::Key_Less));
   addAction(mPrevBookmarkActionPtr);
   connect(mPrevBookmarkActionPtr,
           &QAction::triggered,
           std::bind(&RvTimeController::Toolbar::BookmarkAdvancement, this, aInterfacePtr, false));
   mNextBookmarkActionPtr =
      new wkf::Action(QIcon::fromTheme("skip_next"), "Next bookmark", this, QKeySequence(Qt::Key_Greater));
   addAction(mNextBookmarkActionPtr);
   connect(mNextBookmarkActionPtr,
           &QAction::triggered,
           std::bind(&RvTimeController::Toolbar::BookmarkAdvancement, this, aInterfacePtr, true));

   // Create popup frame and slider to control clock rate
   QFrame* framePtr = new QFrame(this, Qt::Popup);
   mUi.setupUi(framePtr);
   // Clock rate slider is a logarithmic scale
   mUi.mSlider->setValue(UtMath::LinearToDB(aInterfacePtr->GetClockRate()));
   mUi.mSpinBox->setMinimum(UtMath::DB_ToLinear(mUi.mSlider->minimum()));
   mUi.mSpinBox->setMaximum(1000.0); // UtMath::DB_ToLinear(mUi.mSlider->maximum()));
   mUi.mSpinBox->setValue(aInterfacePtr->GetClockRate());

   connect(mUi.mSlider, &QSlider::valueChanged, this, &RvTimeController::Toolbar::ClockRateSliderMoved);
   //           std::bind(&RvTimeController::Interface::SendClockRateCommand, aInterfacePtr, std::placeholders::_1, true));
   connect(mUi.mSlider, &QSlider::sliderMoved, this, &RvTimeController::Toolbar::ClockRateSliderMoved);
   connect(mUi.mSpinBox,
           QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,
           &RvTimeController::Toolbar::ClockRateSpinChanged);
   //           std::bind(&RvTimeController::Interface::SendClockRateCommand, aInterfacePtr, std::placeholders::_1, false));
   connect(this, &RvTimeController::Toolbar::ClockRateChanged, aInterfacePtr, &RvTimeController::Interface::SendClockRateCommand);

   connect(mAdvanceTimeActionPtr,
           &QAction::triggered,
           std::bind(&RvTimeController::Toolbar::AdvanceToTimeTriggered, this, aInterfacePtr));
   connect(mSliderPtr, &QSlider::sliderPressed, this, &RvTimeController::Toolbar::TimeSliderPressed);
   connect(mSliderPtr, &QSlider::sliderMoved, this, &RvTimeController::Toolbar::TimeSliderMoved);
   connect(mSliderPtr, &QSlider::sliderReleased, this, &RvTimeController::Toolbar::TimeSliderReleased);

   connect(&rvEnv, &rv::Environment::PlayStatusUpdate, this, &RvTimeController::Toolbar::PlaybackStatusCallback);

   GuiSimulationPausing();
}

void RvTimeController::Toolbar::Update(RvTimeController::Interface* aInterface)
{
   float maxTime = aInterface->GetEndTime();
   float time    = aInterface->GetSimTime();
   if (time >= maxTime)
   {
      GuiSimulationPausing();
   }
   std::pair<float, float> crange = aInterface->GetCacheRange();

   if (maxTime > 0)
   {
      int tickLoc  = time / maxTime * mSliderPtr->maximum();
      int cacheMin = crange.first / maxTime * mSliderPtr->maximum();
      int cacheMax = crange.second / maxTime * mSliderPtr->maximum();
      mSliderPtr->setCache(cacheMin, cacheMax);
      std::vector<CachedSlider::BookmarkInfo> bookmarks;
      for (const auto& it : mBookmarks)
      {
         int pos = it.time() / maxTime * mSliderPtr->maximum();
         bookmarks.emplace_back(CachedSlider::BookmarkInfo(pos, it.msgType(), it.msgText()));
      }
      mSliderPtr->SetBookmarks(bookmarks);
      if (!mDeferUpdatesToTime)
      {
         mSliderPtr->setSliderPosition(tickLoc);
      }
      mSliderPtr->update();
   }
}

void RvTimeController::Toolbar::UpdateWrite(RvTimeController::Interface* aInterfacePtr)
{
   if (mDeferUpdatesToTime)
   {
      float time = (float)mSliderPtr->sliderPosition() / (float)mSliderPtr->maximum() * aInterfacePtr->GetEndTime();
      aInterfacePtr->SendAdvanceToTimeCommand(time);
   }
}

QList<wkf::Action*> RvTimeController::Toolbar::GetActions() const
{
   QList<wkf::Action*> list;
   list << mReverseTogglePtr << mResumeTogglePtr << mRestartActionPtr << mAdvanceTimeActionPtr << mFastForwardActionPtr
        << mPrevBookmarkActionPtr << mNextBookmarkActionPtr;
   return list;
}

void RvTimeController::Toolbar::GuiSimulationResuming()
{
   mReverseActionPtr->setVisible(true);
   mPauseActionPtr->setVisible(true);
   mResumeActionPtr->setVisible(false);
}

void RvTimeController::Toolbar::GuiSimulationPausing()
{
   mReverseActionPtr->setVisible(true);
   mPauseActionPtr->setVisible(false);
   mResumeActionPtr->setVisible(true);
}

void RvTimeController::Toolbar::GuiSimulationReversing()
{
   mReverseActionPtr->setVisible(false);
   mPauseActionPtr->setVisible(true);
   mResumeActionPtr->setVisible(true);
}


void RvTimeController::Toolbar::GuiSimulationClockRateChange(double aClockRate)
{
   // Prevent valueChanged() signal from being emitted causing feedback loop
   mUi.mSpinBox->blockSignals(true);
   mUi.mSlider->blockSignals(true);
   mUi.mSpinBox->setValue(aClockRate);
   mUi.mSlider->setValue(UtMath::LinearToDB(aClockRate));
   mUi.mSpinBox->blockSignals(false);
   mUi.mSlider->blockSignals(false);

   mUi.mSpinBox->ValueChanged(aClockRate);
}

void RvTimeController::Toolbar::AdvanceToTimeTriggered(RvTimeController::Interface* aInterfacePtr)
{
   QInputDialog dlg(nullptr, Qt::Popup);
   dlg.setInputMode(QInputDialog::DoubleInput);
   dlg.setLabelText("Advance to time:");
   dlg.setDoubleDecimals(3);
   dlg.setDoubleRange(0, aInterfacePtr->GetEndTime());
   dlg.setDoubleValue(aInterfacePtr->GetSimTime());
   dlg.setFixedSize(240, 90);
   MovePopup(&dlg, mAdvanceTimeActionPtr);

   if (dlg.exec() == QDialog::Accepted)
   {
      aInterfacePtr->SendAdvanceToTimeCommand(dlg.doubleValue());
   }
}

void RvTimeController::Toolbar::SetClockRateTriggered()
{
   QWidget* framePtr  = mUi.mSlider->parentWidget();
   QAction* actionPtr = qobject_cast<QAction*>(sender());
   MovePopup(framePtr, actionPtr);
   framePtr->show();
}

void RvTimeController::Toolbar::ClockRateSliderMoved(int value)
{
   double lval = UtMath::DB_ToLinear(value);
   mUi.mSpinBox->setValue(lval);
   emit ClockRateChanged(lval, false);
}

void RvTimeController::Toolbar::ClockRateSpinChanged(double aValue)
{
   double dval = UtMath::LinearToDB(aValue);
   mUi.mSlider->blockSignals(true);
   if (dval <= mUi.mSlider->maximum())
   {
      mUi.mSlider->setValue(static_cast<int>(dval));
   }
   else
   {
      mUi.mSlider->setValue(mUi.mSlider->maximum());
   }
   mUi.mSlider->blockSignals(false);
   emit ClockRateChanged(aValue, false);
}

// Move popup widget to correct location in global screen coordinates, based on the action that triggered it
// and the orientation/dock area of the toolbar
void RvTimeController::Toolbar::MovePopup(QWidget* aPopupWidgetPtr, QAction* aActionPtr)
{
   QPoint          localPt;
   Qt::ToolBarArea area = isFloating() ? Qt::TopToolBarArea : wkfEnv.GetMainWindow()->toolBarArea(this);
   switch (area)
   {
   case Qt::LeftToolBarArea:
      localPt.setX(width());
      localPt.setY(0);
      break;

   case Qt::BottomToolBarArea:
      localPt.setX(0);
      localPt.setY(-aPopupWidgetPtr->height());
      break;

   case Qt::RightToolBarArea:
      localPt.setX(-aPopupWidgetPtr->width());
      localPt.setY(0);
      break;

   case Qt::TopToolBarArea:
   default:
      localPt.setX(0);
      localPt.setY(height());
      break;
   }

   QPoint globalPt = widgetForAction(aActionPtr)->mapToGlobal(localPt);
   aPopupWidgetPtr->move(globalPt);
}

void RvTimeController::Toolbar::MaxTimeChanged(float aMax)
{
   if ((aMax > 0) && (aMax != mMax))
   {
      mMax         = aMax;
      int tickLoc  = mTime / mMax * mSliderPtr->maximum();
      int cacheMin = mMinCache / mMax * mSliderPtr->maximum();
      int cacheMax = mMaxCache / mMax * mSliderPtr->maximum();
      mSliderPtr->setCache(cacheMin, cacheMax);
      mSliderPtr->setSliderPosition(tickLoc);
   }
}

void RvTimeController::Toolbar::CurrentTimeChanged(double aTime)
{
   if (mTime != aTime)
   {
      mTime       = aTime;
      int tickLoc = mTime / mMax * mSliderPtr->maximum();
      mSliderPtr->setSliderPosition(tickLoc);
   }
}

void RvTimeController::Toolbar::TimeInCacheChanged(float aMin, float aMax)
{
   if ((mMinCache != aMin) && (mMaxCache != aMax))
   {
      mMinCache    = aMin;
      mMaxCache    = aMax;
      int cacheMin = mMinCache / mMax * mSliderPtr->maximum();
      int cacheMax = mMaxCache / mMax * mSliderPtr->maximum();
      if (cacheMax > cacheMin)
      {
         mSliderPtr->setCache(cacheMin, cacheMax);
      }
   }
}

void RvTimeController::Toolbar::TimeSliderPressed()
{
   mDeferUpdatesToTime = true;
   mPressed            = true;
}

void RvTimeController::Toolbar::TimeSliderMoved(int aValue)
{
   if (mPressed)
   {
      mTime = (double)aValue / (double)mSliderPtr->maximum() * mMax;
   }
}

void RvTimeController::Toolbar::TimeSliderReleased()
{
   mDeferUpdatesToTime = false;
   mPressed            = false;
}

void RvTimeController::Toolbar::SpeedLimitChanged(int aBase10Exponent)
{
   const double multiple = std::pow(10.0, aBase10Exponent);
   mUi.mSlider->setMaximum(10.0 * aBase10Exponent);
   mUi.mSpinBox->setMaximum(multiple);
}

void RvTimeController::Toolbar::PlaybackStatusCallback(rv::PlaybackStatus aStatus)
{
   if (aStatus != rv::PlaybackStatus::cRESTART)
   {
      mCurrentStatus =
         aStatus; // When the scenario is restarted, it preserves whether it's paused, forward or reverse to the new run
   }
   switch (aStatus)
   {
   case rv::PlaybackStatus::cPAUSE:
      GuiSimulationPausing();
      break;
   case rv::PlaybackStatus::cPLAY:
      GuiSimulationResuming();
      break;
   case rv::PlaybackStatus::cREVERSE:
      GuiSimulationReversing();
      break;
   case rv::PlaybackStatus::cRESTART:
      CurrentTimeChanged(0);
      break;
   }
   // do nothing by default
}

void RvTimeController::Toolbar::BookmarkAdvancement(Interface* aInterfacePtr, bool aNext)
{
   rv::MsgBookmark tempTime;
   tempTime.time(aInterfacePtr->GetSimTime());
   float time = 0.0;
   if (aNext)
   {
      auto bookIt = mBookmarks.upper_bound(tempTime);
      time        = (bookIt != mBookmarks.end()) ? bookIt->time() : aInterfacePtr->GetEndTime();
   }
   else
   {
      auto it = mBookmarks.lower_bound(tempTime);
      time    = (it != mBookmarks.begin()) ? (--it)->time() : 0.0;
   }
   aInterfacePtr->SendAdvanceToTimeCommand(time);
}

void RvTimeController::Toolbar::SetupPlaybackShortcuts()
{
   mReverseTogglePtr = new wkf::Action("Reverse/Pause", wkfEnv.GetMainWindow(), QKeySequence(Qt::Key_F5));
   mResumeTogglePtr  = new wkf::Action("Resume/Pause", wkfEnv.GetMainWindow(), QKeySequence(Qt::Key_F6));
   connect(mReverseTogglePtr,
           &QAction::triggered,
           [this]()
           {
              if (mCurrentStatus == rv::PlaybackStatus::cPAUSE || mCurrentStatus == rv::PlaybackStatus::cPLAY)
              {
                 mReverseActionPtr->trigger();
              }
              else
              {
                 mPauseActionPtr->trigger();
              }
           });
   connect(mResumeTogglePtr,
           &QAction::triggered,
           [this]()
           {
              if (mCurrentStatus == rv::PlaybackStatus::cPAUSE || mCurrentStatus == rv::PlaybackStatus::cREVERSE)
              {
                 mResumeActionPtr->trigger();
              }
              else
              {
                 mPauseActionPtr->trigger();
              }
           });
}
