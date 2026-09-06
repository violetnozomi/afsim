// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvBAT_DockWindow.hpp"

#include <QBoxLayout>
#include <QComboBox>
#include <QEvent>
#include <QGraphicsEffect>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include <QTreeWidget>

#include "RvBAT_Blackboard.hpp"
#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "WkfAction.hpp"

RvBAT::DockWindow::DockWindow(const QString& aObjectName, QMainWindow* parent, Qt::WindowFlags flags)
   : wkf::DockWidget(aObjectName, parent, flags)
   , mPlatformComboBoxPtr(new QComboBox(this))
   , mSceneComboBoxPtr(new QComboBox(this))
   , mDataInterfacePtr(ut::make_unique<Interface>(this))
{
   setWindowTitle("Behavior Analysis Tool");

   // Create the container for the whole window, this will hold the layout that
   // contains all of the child widgets
   auto container = new QWidget(this);

   // Create the container to parent mViewPtr, this will be used to grab
   // resize events using the event filter (QGraphicsView is ignoring
   // the size hint given by the grid layout?)
   auto viewContainer = new QFrame(this);

   // Create a font and use it to set the width of the
   // blackboard to 50 characters wide at 12 pt font
   QFont f;
   f.setPointSize(12);
   QFontMetrics fm      = QFontMetrics(f);
   int          bbWidth = fm.averageCharWidth() * 50;

   // Create the blackboard and hide it by default
   mBlackboardPtr = ut::make_unique<Blackboard>(this, bbWidth);
   mBlackboardPtr->hide();

   // Create a container and a view for a static background
   mViewPtr = ut::make_unique<MovableView>(mBlackboardPtr.get(), this);
   mViewPtr->setStyleSheet(
      QIcon::themeName() == "Dark" ?
         "QGraphicsView { background-color: #222222; background-image: url(:/images/Dark/bg.png); background-repeat: "
         "repeat-xy; } QToolTip { background-color: #222222; color: #BBBBBB; border: 1px solid #777777 }" :
         "QGraphicsView { background-color: #EEEEEE; background-image: url(:/images/Light/bg.png); background-repeat: "
         "repeat-xy; } QToolTip { background-color: #EEEEEE; color: #222222; border: 1px solid #444444 }");

   // Create the blackboard button and connect its toggle function
   mBlackboardButton = new QPushButton(QIcon::fromTheme("left"), "", this);
   mBlackboardButton->setFixedWidth(20);
   mBlackboardButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   connect(mBlackboardButton, &QPushButton::released, this, &DockWindow::ToggleBlackboard);

   // Set the parent container for the movable view and install
   // the event filter that will grab resize events
   mViewPtr->setParent(viewContainer);
   viewContainer->installEventFilter(mViewPtr.get());

   // Create a grid layout that will fit our combo boxes, view, and the blackboard
   // and add said widgets to the layout
   auto gridLayout = new QGridLayout;
   gridLayout->setSpacing(0);
   gridLayout->addWidget(mPlatformComboBoxPtr, 0, 0);
   gridLayout->addWidget(mSceneComboBoxPtr, 1, 0);
   gridLayout->addWidget(viewContainer, 2, 0, 1, 1);
   gridLayout->addWidget(mBlackboardButton, 0, 1, 3, 1);
   gridLayout->addWidget(mBlackboardPtr.get(), 0, 2, 3, 1);
   container->setLayout(gridLayout);

   // Set the widget for the dock window
   setWidget(container);

   // Connect the combo box change events to the appropriate functions
   connect(mPlatformComboBoxPtr,
           QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
           mViewPtr.get(),
           &RvBAT::MovableView::comboBoxChangedIndex);
   connect(mPlatformComboBoxPtr,
           QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
           this,
           &RvBAT::DockWindow::comboBoxChangedIndex);
   connect(mSceneComboBoxPtr,
           QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
           mViewPtr.get(),
           &RvBAT::MovableView::SceneComboBoxChangedIndex);
}

void RvBAT::DockWindow::comboBoxChangedIndex(const QString& aStr)
{
   std::string currentPlatformName = aStr.toUtf8().toStdString();

   // Find the platform index and use that to get the matching trees
   size_t platformIndex;
   if (rv::Environment::Exists())
   {
      rv::ResultData* currentData = rvEnv.GetData();
      if (currentData != nullptr)
      {
         platformIndex = currentData->FindPlatformIndex(currentPlatformName);
      }
   }

   // Block currentIndexChanged temporarily
   mSceneComboBoxPtr->blockSignals(true);

   // Clear the scene combo box
   mSceneComboBoxPtr->clear();

   // Repopulate the scene combo box with the matching trees and fsms for the newly selected platform
   auto platformTrees = mDataInterfacePtr->GetBehaviorTrees().find(platformIndex);
   if (platformTrees != mDataInterfacePtr->GetBehaviorTrees().end())
   {
      const auto& trees = *platformTrees;
      for (auto it = trees.cbegin(); it != trees.cend(); ++it)
      {
         mSceneComboBoxPtr->addItem(QString::fromStdString(it->mName));
      }
   }
   auto platformFSMs = mDataInterfacePtr->GetFSMs().find(platformIndex);
   if (platformFSMs != mDataInterfacePtr->GetFSMs().end())
   {
      const auto& fsms = *platformFSMs;
      for (auto it = fsms.cbegin(); it != fsms.cend(); ++it)
      {
         mSceneComboBoxPtr->addItem(QString("fsm_") + QString::number(it.key()));
      }
   }

   // Done blocking
   mSceneComboBoxPtr->blockSignals(false);
}

void RvBAT::DockWindow::changeEvent(QEvent* aEvent)
{
   DockWidget::changeEvent(aEvent);

   // If we get a StyleChange event (theme has changed), set a flag
   // so that the scene will be repainted on the next update
   if (aEvent->type() == QEvent::StyleChange && mViewPtr->IsLoaded())
   {
      mThemeChanged = true;
   }
}

void RvBAT::DockWindow::Update(const rv::ResultData& aData)
{
   // Only update when our window is visible.
   if (isVisible() || mNeedsUpdate)
   {
      // Check if the theme has changed, and if so update the current theme and send out a redraw.
      // We cannot redraw in changeEvent, because if we attempt to redraw then
      // QIcon::themeName won't have changed yet, and we will draw the wrong colors.
      if (mThemeChanged)
      {
         mThemeChanged = false;
         mViewPtr->RedrawVisuals();
         repaint();
      }

      // Reset our update bool
      mNeedsUpdate = false;

      // Update our interface.
      mDataInterfacePtr->AdvanceTimeRead(aData);

      // If our view is loaded, update it.
      if (mViewPtr->IsLoaded())
      {
         mViewPtr->Update(aData);
      }

      // If our interface has loaded trees, but we haven't made the scenes in the view.
      if (mDataInterfacePtr->IsLoaded() && !mViewPtr->IsLoaded())
      {
         mViewPtr->SetInterfacePtr(mDataInterfacePtr.get());
         // Initialize the combo box's list to all available platforms
         mPlatformComboBoxPtr->clear();
         mSceneComboBoxPtr->clear();

         // Then add only the default platforms trees
         rv::ResultPlatform* plat = aData.FindPlatform((int)mDataInterfacePtr->GetBehaviorTrees().firstKey());
         if (plat != nullptr)
         {
            mViewPtr->SetCurrentPlatform(plat->GetName());
            for (const auto& kv : mDataInterfacePtr->GetBehaviorTrees().value(plat->GetPlatformIndex()).toStdMap())
            {
               mSceneComboBoxPtr->addItem(QString::fromStdString(kv.second.mName));
            }
            for (const auto& kv : mDataInterfacePtr->GetFSMs().value(plat->GetPlatformIndex()).toStdMap())
            {
               mSceneComboBoxPtr->addItem(QString("fsm_") + QString::number(kv.first));
            }
         }

         // Add all platforms to the combo box
         for (auto platformIndex : mDataInterfacePtr->GetBehaviorTrees().keys())
         {
            plat = aData.FindPlatform(static_cast<int>(platformIndex));
            if (plat != nullptr)
            {
               mPlatformComboBoxPtr->addItem(QString::fromStdString(plat->GetName()));
            }
         }

         mViewPtr->SetLoaded(true);
         mViewPtr->CenterSceneInWindow();
         mViewPtr->setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
      }
   }
}

void RvBAT::DockWindow::Reset()
{
   mDataInterfacePtr->Reset();
   mViewPtr->Reset();
}

void RvBAT::DockWindow::GoToComboBox(const QString& aStr)
{
   // If a combo box index for aStr is found, swap to it
   auto strIndex = mSceneComboBoxPtr->findText(aStr);
   if (strIndex != -1)
   {
      mSceneComboBoxPtr->setCurrentIndex(strIndex);
      mSceneComboBoxPtr->currentIndexChanged(aStr);
   }
}

void RvBAT::DockWindow::resizeEvent(QResizeEvent* aEvent)
{
   // A resize event will get called when the window is created, handle it and set the default size
   if (aEvent->oldSize().width() == -1)
   {
      resize(cDEFAULT_WINDOW_SIZE);
      if (mViewPtr != nullptr) // Make sure our view exists before we try to resize it
      {
         mViewPtr->ResizeWindow(cDEFAULT_WINDOW_SIZE);
      }
   }
   else // If we aren't dealing with the initial size, resize according to the event size
   {
      if (mViewPtr != nullptr && mViewPtr->parentWidget() != nullptr) // Make sure our view exists before we try to resize it
      {
         mViewPtr->ResizeWindow(mViewPtr->parentWidget()->size()); // Set to window size, not the event size, we only
                                                                   // care about matching the window
      }
   }
}

bool RvBAT::DockWindow::BuildContextMenu(QMenu& aMenu, const QPoint& aPos)
{
   return mViewPtr->BuildContextMenu(aMenu, aPos);
}

void RvBAT::DockWindow::ToggleBlackboard()
{
   if (mBlackboardPtr->isVisible())
   {
      mBlackboardPtr->hide();
      mBlackboardButton->setIcon(QIcon::fromTheme("left"));
   }
   else
   {
      mBlackboardPtr->show();
      mBlackboardButton->setIcon(QIcon::fromTheme("right"));
   }
}
