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

#include "RvBAT_MovableView.hpp"

#include <QApplication>
#include <QIcon>
#include <QKeyEvent>
#include <QMouseEvent>

#include "RvBAT_ABTScene.hpp"
#include "RvBAT_Blackboard.hpp"
#include "RvBAT_DockWindow.hpp"
#include "RvBAT_FSMScene.hpp"
#include "RvBAT_GraphicsNode.hpp"
#include "RvBAT_GraphicsState.hpp"
#include "RvBAT_Interface.hpp"
#include "RvEnvironment.hpp"
#include "RvResultData.hpp"

RvBAT::MovableView::MovableView(Blackboard* aBlackboardPtr, DockWindow* aDockWindowPtr)
   : QGraphicsView()
   , mTargetViewportPos(QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0))
   , mScreenWidth(viewport()->rect().width())
   , mDockWindowPtr(aDockWindowPtr)
   , mBlackboardPtr(aBlackboardPtr)
{
   // Set some things for panning the window
   setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   centerOn(0, 0);
   mTargetScenePos = mapToScene(mTargetViewportPos.x(), mTargetViewportPos.y());
}

void RvBAT::MovableView::SceneComboBoxChangedIndex(const QString& aStr)
{
   // Reset some blackboard data vars
   mBlackboardPtr->ClearBlackboardData();
   mBlackboardPtr->SetBlackboardTitle("Shared");
   if (mSelectedNode != nullptr)
   {
      mSelectedNode->ToggleSelected();
      mSelectedNode->UpdateTextColor();
      mSelectedNode->update();
      mSelectedNode = nullptr;
   }

   // Find the platform index
   size_t platformIndex;
   if (rv::Environment::Exists())
   {
      rv::ResultData* currentData = rvEnv.GetData();
      if (currentData != nullptr)
      {
         platformIndex = currentData->FindPlatformIndex(mCurrentPlatform);
      }
   }

   // Attempt to find the current tree or fsm from the combo box string
   RvBAT::BehaviorTree bt;
   size_t              sceneId;
   SceneType           sceneType;
   bool                sceneFound = false;
   for (const auto& kv : mInterfacePtr->GetBehaviorTrees().value(platformIndex).toStdMap())
   {
      if (kv.second.mName == aStr.toUtf8().toStdString())
      {
         sceneId    = kv.first;
         sceneType  = SceneType::cABT;
         sceneFound = true;
         break;
      }
   }

   // If we didnt find a tree check for an fsm
   if (!sceneFound)
   {
      for (const auto& kv : mInterfacePtr->GetFSMs().value(platformIndex).toStdMap())
      {
         // Temp solution by adding fsm_ with the id
         auto   splitStr = aStr.split('_');
         size_t fsmId    = 0;
         if (splitStr.size() > 1)
         {
            fsmId = splitStr.back().toInt();
         }

         if (kv.first == fsmId)
         {
            sceneId    = fsmId;
            sceneType  = SceneType::cFSM;
            sceneFound = true;
            break;
         }
      }
   }

   // If we found the scene
   if (sceneFound)
   {
      // Check if a scene has already been generated for this id and scene type, if so load it. Otherwise create it.
      auto it = std::find_if(mScenes.begin(),
                             mScenes.end(),
                             [sceneId, sceneType](const Scene* scene)
                             { return scene->Id() == sceneId && scene->GetSceneType() == sceneType; });
      if (it == mScenes.end())
      {
         CreateScene(sceneId, sceneType, platformIndex);
      }
      else
      {
         LoadScene(sceneId, sceneType);
      }

      // Set the new current scene for the platform
      mCurrentScene[mCurrentPlatform] = aStr;

      // Load the scenes config.  Use a single shot event because the
      // first time centerOn/scale gets called some sort of race condition
      // occurs and the QGraphicsView doesn't update properly.
      QTimer::singleShot(0, this, SLOT(LoadSceneConfig()));
   }
}

void RvBAT::MovableView::LoadSceneConfig()
{
   // Reset matrix before making viewRect calculations
   resetMatrix();

   // If we don't have a config for a tree yet (not initialized)
   // set the center point as the center of the tree, and set the zoom to fit the whole tree in view.
   // Set the initialization flag so it only runs once this layout setup has been run before
   if (!mActiveScene->Initialized())
   {
      auto sceneRect = mActiveScene->GetSceneRect();
      auto viewRect  = mapToScene(viewport()->rect()).boundingRect();
      mActiveScene->SetCenterPoint(sceneRect.center());

      // Target the tree being 90% of the window. Use the smaller scaling to guarantee fit.
      if (sceneRect.width() > viewRect.width() || sceneRect.height() > viewRect.height())
      {
         double widthScaled  = (viewRect.width() * 0.9) / sceneRect.width();
         double heightScaled = (viewRect.height() * 0.9) / sceneRect.height();
         mActiveScene->SetZoom(widthScaled < heightScaled ? widthScaled : heightScaled);
      }
      mActiveScene->SetInitialized(true);
   }
   // Set center pos based on the config
   centerOn(mActiveScene->GetCenterPoint());

   // Set zoom based on the config
   mViewScale = mActiveScene->GetZoom();
   scale(mViewScale, mViewScale);

   // Set the view scene
   setScene(mActiveScene);
}

void RvBAT::MovableView::LoadSceneComboBox()
{
   GoToComboBox(mCurrentScene[mCurrentPlatform]);
}

bool RvBAT::MovableView::eventFilter(QObject* aWatchedObject, QEvent* aEvent)
{
   // Catch resize events from the parent QFrame.
   // The only events grabbed here will be when the blackboard
   // button is toggled
   auto resizeEvent = dynamic_cast<QResizeEvent*>(aEvent);
   if (resizeEvent != nullptr)
   {
      resize(resizeEvent->size());
      if (mActiveScene != nullptr)
      {
         // Move the scene over when the blackboard opens
         auto   dist = (mBlackboardPtr->isHidden() ? -mBlackboardPtr->width() : mBlackboardPtr->width()) / 2;
         QPoint newCenter(viewport()->rect().width() / 2 + dist, mapFromScene(mActiveScene->GetCenterPoint()).y());
         centerOn(mapToScene(newCenter));
      }
   }
   return false;
}

void RvBAT::MovableView::keyPressEvent(QKeyEvent* aEvent)
{
   qint32 key = aEvent->key();

   if (key == mZoomKey)
   {
      mIsZooming = true;
   }

   if (mIsZooming)
   {
      if (key == Qt::Key_Up)
      {
         ZoomIn();
      }

      else if (key == Qt::Key_Down)
      {
         ZoomOut();
      }
   }

   // Center the window on the behavior tree
   if (key == Qt::Key_Home)
   {
      CenterSceneInWindow();
   }

   else
   {
      QGraphicsView::keyPressEvent(aEvent);
   }
}

void RvBAT::MovableView::keyReleaseEvent(QKeyEvent* aEvent)
{
   if (aEvent->key() == mZoomKey)
   {
      mIsZooming = false;
   }

   QGraphicsView::keyReleaseEvent(aEvent);
}

void RvBAT::MovableView::mouseMoveEvent(QMouseEvent* aEvent)
{
   // Check if the mouse has moved
   if (mLastMouseMove != aEvent->pos())
   {
      if (aEvent->buttons() & Qt::LeftButton && mMouseButton == Qt::LeftButton)
      {
         // We are dragging a node.
         if (mObjectClicked)
         {
            if ((aEvent->pos() - mLastObjectPos).manhattanLength() < QApplication::startDragDistance())
            {
               mObjectDragging = true;
            }
         }
         // We are dragging the window.
         else if (mIsPanning)
         {
            if (mLastMousePos != aEvent->pos())
            {
               QPointF mouseDelta = mapToScene(aEvent->pos()) - mapToScene(mLastMousePos);
               Pan(mouseDelta);
               setCursor(Qt::SizeAllCursor);
            }
         }
      }

      mLastMouseMove = aEvent->pos();
   }

   // Check if we've moved enough
   QPointF delta = mTargetViewportPos - aEvent->pos();
   if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5)
   {
      mTargetViewportPos = aEvent->pos();
      mTargetScenePos    = mapToScene(aEvent->pos());
   }

   // Propogate the event
   QGraphicsView::mouseMoveEvent(aEvent);
   mLastMousePos = aEvent->pos();
}

void RvBAT::MovableView::mousePressEvent(QMouseEvent* aEvent)
{
   // If there is a scene and the mouse is down
   if ((aEvent->buttons() & (Qt::LeftButton | Qt::RightButton)) && scene() != nullptr)
   {
      auto lastButton = mMouseButton;
      if (aEvent->buttons() & Qt::LeftButton)
      {
         mMouseButton = Qt::LeftButton;
      }
      else if (aEvent->buttons() & Qt::RightButton)
      {
         mMouseButton = Qt::RightButton;
      }

      // If a scene relevant object was pressed on
      if (mActiveScene->IsObjectAtPoint(mapToScene(aEvent->pos())))
      {
         mObjectClicked = true;
         mLastObjectPos = aEvent->pos();
      }

      mLastMousePos = aEvent->pos();
      if (!mObjectClicked && lastButton != Qt::RightButton)
      {
         mIsPanning = true;
      }
   }

   // Propogate the mouse press event
   QGraphicsView::mousePressEvent(aEvent);
}

void RvBAT::MovableView::mouseReleaseEvent(QMouseEvent* aEvent)
{
   if (!(aEvent->buttons() & Qt::LeftButton) && (mMouseButton == Qt::LeftButton))
   {
      // If we released a click on a node, handle if it
      // was drag release or a click.
      if (mObjectClicked)
      {
         mObjectClicked = false;
         if (!mObjectDragging)
         {
            // See if we released the mouse on a node.
            bool          nodeFound = false;
            GraphicsNode* node      = nullptr;
            for (auto item : scene()->items(mapToScene(aEvent->pos())))
            {
               node = dynamic_cast<GraphicsNode*>(item);
               // If we released the mouse on a node, break out and handle it.
               if (node != nullptr)
               {
                  nodeFound = true;
                  break;
               }
            }

            // If we found a node to process.
            if (nodeFound)
            {
               // If there's a currently selected node
               if (mSelectedNode != nullptr)
               {
                  // Deselect the current node.
                  mSelectedNode->ToggleSelected();
                  mSelectedNode->UpdateTextColor();
                  mSelectedNode->update();

                  // If the node we clicked on was already selected,
                  // set nullptr
                  if (mSelectedNode == node)
                  {
                     mSelectedNode = nullptr;
                  }
                  // Otherwise we've clicked on new node, select the new one.
                  else
                  {
                     node->ToggleSelected();
                     node->UpdateTextColor();
                     node->update();
                     mSelectedNode = node;
                  }
               }
               // If there isn't a currently selected node, select the new one.
               else
               {
                  node->ToggleSelected();
                  node->UpdateTextColor();
                  node->update();
                  mSelectedNode = node;
               }
            }
         }
         else
         {
            mObjectDragging = false;
         }
      }
      // If we didn't click on a node but were panning,
      // Reset the state and set the mouse cursor back to normal.
      else if (mIsPanning)
      {
         mIsPanning = false;
         setCursor(Qt::ArrowCursor);
      }
      // If a node wasn't clicked, we clicked on background.
      // Deselect the current node and set nullptr
      else
      {
         if (mSelectedNode != nullptr)
         {
            mSelectedNode->ToggleSelected();
            mSelectedNode->UpdateTextColor();
            mSelectedNode->update();
            mSelectedNode = nullptr;
         }
      }

      // Update the blackboard here.
      // If we have a selected node, set its blackboard data.
      auto abtScene = dynamic_cast<ABTScene*>(mActiveScene);
      if (abtScene != nullptr)
      {
         if (mSelectedNode != nullptr && mSelectedNode != abtScene->GetRootNode())
         {
            mBlackboardPtr->ClearBlackboardData();
            mBlackboardPtr->SetBlackboardTitle(mSelectedNode->GetNodeText()->toPlainText());
            // If we have blackboard data.
            mBlackboardPtr->SetBlackboardData(mSelectedNode->GetBlackboardData());
         }
         // Otherwise, use the shared blackboard data.
         else
         {
            mBlackboardPtr->ClearBlackboardData();
            mBlackboardPtr->SetBlackboardTitle("Shared");
            mBlackboardPtr->SetBlackboardData(abtScene->GetRootNode()->GetBlackboardData());
         }
      }
   }
   else if (!(aEvent->buttons() & Qt::RightButton) && (mMouseButton == Qt::RightButton))
   {
      if (mObjectClicked)
      {
         mObjectClicked = false;
         if (!mObjectDragging)
         {
            if (mActiveScene->GetSceneType() == SceneType::cABT)
            {
               // See if we released the mouse on a node.
               bool          nodeFound = false;
               GraphicsNode* node      = nullptr;
               for (auto item : scene()->items(mapToScene(aEvent->pos())))
               {
                  node = dynamic_cast<GraphicsNode*>(item);
                  // If we released the mouse on a node, break out and handle it.
                  if (node != nullptr)
                  {
                     nodeFound = true;
                     break;
                  }
               }

               // If we found a node to process.
               if (nodeFound)
               {
                  // If there's a currently selected node
                  if (mSelectedNode != nullptr)
                  {
                     // If we right clicked on a different node
                     if (mSelectedNode != node)
                     {
                        // Deselect the current node.
                        mSelectedNode->ToggleSelected();
                        mSelectedNode->UpdateTextColor();
                        mSelectedNode->update();

                        // And set the newly clicked on one.
                        node->ToggleSelected();
                        node->UpdateTextColor();
                        node->update();
                        mSelectedNode = node;
                     }
                  }
                  // If there isn't a currently selected node, select the new one.
                  else
                  {
                     node->ToggleSelected();
                     node->UpdateTextColor();
                     node->update();
                     mSelectedNode = node;
                  }
               }
            }
            else
            {
               // See if we released the mouse on a state and if so set it for the
               // context menu options
               GraphicsState* state = nullptr;
               for (auto item : scene()->items(mapToScene(aEvent->pos())))
               {
                  state = dynamic_cast<GraphicsState*>(item);
                  // If we released the mouse on a node, break out and handle it.
                  if (state != nullptr)
                  {
                     mContextState = state;
                     break;
                  }
               }
            }
         }
         else
         {
            mObjectDragging = false;
         }
      }
      else
      {
         mContextState = nullptr;
      }
   }

   QGraphicsView::mouseReleaseEvent(aEvent);
}

void RvBAT::MovableView::wheelEvent(QWheelEvent* aEvent)
{
   QPoint scrollAmount = aEvent->angleDelta();

   // Apply zoom.
   scrollAmount.y() > 0 ? ZoomIn() : ZoomOut();
}

void RvBAT::MovableView::ToggleSelectedNodeChildrenVisible()
{
   if (mSelectedNode != nullptr)
   {
      mSelectedNode->ToggleChildrenVisible();

      // Reset the layout in case the tree can be compacted
      // or nodes need to be moved out of the way.
      ResetLayout();

      // Deselect the current node.
      mSelectedNode->ToggleSelected();
      mSelectedNode->UpdateTextColor();
      mSelectedNode->update();
      mSelectedNode = nullptr;
   }
}

void RvBAT::MovableView::ResizeWindow(const QSize aSize)
{
   // Resize the main window
   resize(aSize);
}

void RvBAT::MovableView::GoToComboBox(const QString& aStr)
{
   if (mDockWindowPtr != nullptr)
   {
      mDockWindowPtr->GoToComboBox(aStr);
   }
}

void RvBAT::MovableView::RedrawVisuals()
{
   // Redraw all scenes when the theme changes
   for (auto scene : mScenes)
   {
      scene->RedrawVisuals();
   }
   QGraphicsView* parentView = static_cast<QGraphicsView*>(parent());
   parentView->setStyleSheet(
      QIcon::themeName() == "Dark" ?
         "QGraphicsView { background-color: #222222; background-image: url(:/images/Dark/bg.png); background-repeat: "
         "repeat-xy; } QToolTip { background-color: #222222; color: #BBBBBB; border: 1px solid #777777 }" :
         "QGraphicsView { background-color: #EEEEEE; background-image: url(:/images/Light/bg.png); background-repeat: "
         "repeat-xy; } QToolTip { background-color: #EEEEEE; color: #222222; border: 1px solid #444444 }");
}

bool RvBAT::MovableView::BuildContextMenu(QMenu& aMenu, const QPoint& aPos)
{
   // Add the reset layout option
   auto action = new QAction(QIcon::fromTheme("undo"), QString("Reset Layout"), &aMenu);
   connect(action, &QAction::triggered, this, &MovableView::ResetLayout);
   aMenu.addAction(action);

   // Add the center (ABT/FSM) in view option
   QString centerSceneText = mActiveScene->GetSceneType() == SceneType::cABT ? "Tree" : "FSM";
   auto centerAction = new QAction(QIcon::fromTheme("aim"), QString("Center " + centerSceneText + " In View"), &aMenu);
   connect(centerAction, &QAction::triggered, this, &MovableView::CenterSceneInWindow);
   aMenu.addAction(centerAction);

   // Add hide/show children options for the ABT scenes
   if (mActiveScene->GetSceneType() == SceneType::cABT)
   {
      if (mSelectedNode != nullptr)
      {
         if (mSelectedNode->HasChildren())
         {
            if (mSelectedNode->AreChildrenVisible())
            {
               auto hideChildrenAction = new QAction(QIcon::fromTheme("hide"), QString("Hide Children"), &aMenu);
               connect(hideChildrenAction, &QAction::triggered, this, &MovableView::ToggleSelectedNodeChildrenVisible);
               aMenu.addAction(hideChildrenAction);
            }
            else
            {
               auto showChildrenAction = new QAction(QIcon::fromTheme("show"), QString("Show Children"), &aMenu);
               connect(showChildrenAction, &QAction::triggered, this, &MovableView::ToggleSelectedNodeChildrenVisible);
               aMenu.addAction(showChildrenAction);
            }
         }

         if (mSelectedNode->GetChildFSM_Id() != 0)
         {
            auto childFSM_Str   = QString("fsm_") + QString::number(mSelectedNode->GetChildFSM_Id());
            auto childFSMAction = new QAction(QIcon::fromTheme(QString::fromStdString(
                                                 ":/images/" + QIcon::themeName().toStdString() + "/60x60/fsm")),
                                              QString("Go to child FSM (") + childFSM_Str + QString(")"),
                                              &aMenu);
            connect(childFSMAction, &QAction::triggered, this, [this, childFSM_Str] { GoToComboBox(childFSM_Str); });
            aMenu.addAction(childFSMAction);
         }
      }
   }
   else // Add Go to child ABT/FSM options for FSM scenes
   {
      if (mContextState != nullptr)
      {
         auto& childABT_Str = mInterfacePtr->GetTreeNameFromId(mContextState->GetChildABT_Id());
         if (!childABT_Str.isEmpty())
         {
            auto childABTAction = new QAction(QIcon::fromTheme(QString::fromStdString(
                                                 ":/images/" + QIcon::themeName().toStdString() + "/60x60/root")),
                                              QString("Go to child ABT (") + childABT_Str + QString(")"),
                                              &aMenu);
            connect(childABTAction, &QAction::triggered, this, [this, childABT_Str] { GoToComboBox(childABT_Str); });
            aMenu.addAction(childABTAction);
         }

         if (mContextState->GetChildFSM_Id() != 0)
         {
            auto childFSM_Str   = QString("fsm_") + QString::number(mContextState->GetChildFSM_Id());
            auto childFSMAction = new QAction(QIcon::fromTheme(QString::fromStdString(
                                                 ":/images/" + QIcon::themeName().toStdString() + "/60x60/fsm")),
                                              QString("Go to child FSM (") + childFSM_Str + QString(")"),
                                              &aMenu);
            connect(childFSMAction, &QAction::triggered, this, [this, childFSM_Str] { GoToComboBox(childFSM_Str); });
            aMenu.addAction(childFSMAction);
         }
      }
   }

   return true;
}

void RvBAT::MovableView::CenterSceneInWindow()
{
   if (mActiveScene != nullptr)
   {
      // Center the view on the scene rect
      centerOn(mActiveScene->GetSceneRect().center());

      // Update the center point for the active scene
      auto sceneRect = mActiveScene->GetSceneRect();
      mActiveScene->SetCenterPoint(sceneRect.center());
   }
}

void RvBAT::MovableView::ResetLayout()
{
   mActiveScene->ResetLayout();
}

void RvBAT::MovableView::Reset()
{
   for (auto& scene : mScenes)
   {
      delete scene;
   }
   mScenes.clear();
   mSelectedNode = nullptr;
   mContextState = nullptr;
   mIsLoaded     = false;
}

void RvBAT::MovableView::comboBoxChangedIndex(const QString& aStr)
{
   // Reset some blackboard data vars
   mBlackboardPtr->ClearBlackboardData();
   mBlackboardPtr->SetBlackboardTitle("Shared");
   if (mSelectedNode != nullptr)
   {
      mSelectedNode->ToggleSelected();
      mSelectedNode->UpdateTextColor();
      mSelectedNode->update();
      mSelectedNode = nullptr;
   }

   mCurrentPlatform = aStr.toUtf8().toStdString();

   size_t platformIndex;
   if (rv::Environment::Exists())
   {
      rv::ResultData* currentData = rvEnv.GetData();
      if (currentData != nullptr)
      {
         platformIndex = currentData->FindPlatformIndex(mCurrentPlatform);
      }
   }

   if (!mCurrentPlatform.empty())
   {
      // If we don't have a default tree for this platform, set it.
      if (mCurrentScene.find(mCurrentPlatform) == mCurrentScene.end())
      {
         // If there isn't a current scene set for this platform, set a default.
         // Attempt to find the current tree or fsm from the combo box string
         RvBAT::BehaviorTree bt;
         bool                defaultSceneSet = false;
         QString             defaultSceneName;
         // If we have an abt, use the first one as the default tree
         if (mInterfacePtr->GetBehaviorTrees().find(platformIndex) != mInterfacePtr->GetBehaviorTrees().end())
         {
            CreateScene(mInterfacePtr->GetBehaviorTrees().value(platformIndex).firstKey(), SceneType::cABT, platformIndex);
            defaultSceneName =
               QString::fromStdString(mInterfacePtr->GetBehaviorTrees().value(platformIndex).first().mName);
            defaultSceneSet = true;
         }

         // If an ABT didn't exist, try with FSMs
         if (!defaultSceneSet)
         {
            if (mInterfacePtr->GetFSMs().find(platformIndex) != mInterfacePtr->GetFSMs().end())
            {
               CreateScene(mInterfacePtr->GetFSMs().value(platformIndex).firstKey(), SceneType::cFSM, platformIndex);
               defaultSceneName =
                  QString("fsm_") + QString::number(mInterfacePtr->GetFSMs().value(platformIndex).firstKey());
               defaultSceneSet = true;
            }
         }

         if (defaultSceneSet)
         {
            mCurrentScene[mCurrentPlatform] = defaultSceneName;
         }
      }
      else
      {
         // Load the combo box and current scene.  Using a single shot event because
         // otherwise the GoToComboBox call will resolve before the scene combo box
         // options are populated for the platform change.  This will result in it
         // not finding the mCurrentScene.  Need to look into event prioritization
         // of some sort.
         QTimer::singleShot(0, this, SLOT(LoadSceneComboBox()));
      }

      // Load the scenes config.  Use a single shot event because the
      // first time centerOn/scale gets called some sort of race condition
      // occurs and the QGraphicsView doesn't update properly.
      QTimer::singleShot(0, this, SLOT(LoadSceneConfig()));
   }
}

void RvBAT::MovableView::Zoom(const float aScaleFactor)
{
   scale(aScaleFactor, aScaleFactor);
   mViewScale *= aScaleFactor;
   mActiveScene->SetZoom(mViewScale);
}

void RvBAT::MovableView::GentleZoom(const float aScaleFactor)
{
   scale(aScaleFactor, aScaleFactor);
   mViewScale *= aScaleFactor;

   QPointF deltaViewportPos = mTargetViewportPos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
   QPointF viewportCenter   = mapFromScene(mTargetScenePos) - deltaViewportPos;
   centerOn(mapToScene(viewportCenter.toPoint()));

   mActiveScene->SetCenterPoint(mapToScene(viewportCenter.toPoint()));
   mActiveScene->SetZoom(mViewScale);
}

void RvBAT::MovableView::Update(const rv::ResultData& aData)
{
   // Get the platform index from the current platform
   size_t platformIndex;
   if (rv::Environment::Exists())
   {
      rv::ResultData* currentData = rvEnv.GetData();
      if (currentData != nullptr)
      {
         platformIndex = currentData->FindPlatformIndex(mCurrentPlatform);
      }
   }

   // If we have a current platform
   if (!mCurrentPlatform.empty())
   {
      // If we have an active scene, update it
      if (mActiveScene != nullptr)
      {
         // If we have an ABT scene
         if (mActiveScene->GetSceneType() == SceneType::cABT)
         {
            auto                      scene     = dynamic_cast<ABTScene*>(mActiveScene);
            rv::MsgBehaviorTreeState* treeState = mInterfacePtr->GetBehaviorTrees()[platformIndex][scene->Id()].mState;
            // If we don't have a state message, update the state with the nullptr (will clear and idle all nodes)
            // and clear the blackboard
            if (treeState == nullptr)
            {
               mPreviousStateMessageIndex = 0;
               scene->UpdateState(treeState);
               scene->update();

               mBlackboardPtr->ClearBlackboardData();
            }
            else
            {
               // Only update things when we have a different message
               if (mPreviousStateMessageIndex != treeState->GetMessageIndex())
               {
                  QVector<rv::BehaviorTreeBlackboardData> sharedBlackboardMsgs;
                  scene->UpdateState(treeState, sharedBlackboardMsgs);

                  // Handle the shared blackboard messages.
                  if (!sharedBlackboardMsgs.empty())
                  {
                     if (!mSelectedNode || mSelectedNode == scene->GetRootNode())
                     {
                        scene->GetRootNode()->SetBlackboardData(sharedBlackboardMsgs);
                        mBlackboardPtr->SetBlackboardData(sharedBlackboardMsgs);
                     }
                  }

                  if (mSelectedNode && mSelectedNode != scene->GetRootNode()) // If we have a selected node, push it's
                                                                              // data into the blackboard
                  {
                     mBlackboardPtr->SetBlackboardData(mSelectedNode->GetBlackboardData());
                  }

                  scene->update();
               }
               mPreviousStateMessageIndex = treeState->GetMessageIndex();
            }
         }
         else // mActiveScene->GetSceneType() == SceneType::cFSM
         {
            auto scene = dynamic_cast<FSMScene*>(mActiveScene);
            // Run the exec for the scene, so that the state colors match what they should be at the current time.
            rv::MsgStateMachineState* fsmState = mInterfacePtr->GetFSMs()[platformIndex][scene->Id()].mState;

            if (fsmState == nullptr)
            {
               mPreviousStateMessageIndex = 0;
               scene->UpdateState(fsmState);
               scene->update();
            }
            else
            {
               // Only update things when we have a different message
               if (mPreviousStateMessageIndex != fsmState->GetMessageIndex())
               {
                  scene->UpdateState(fsmState);
                  scene->update();
               }
               mPreviousStateMessageIndex = fsmState->GetMessageIndex();
            }
         }
      }
   }
}

void RvBAT::MovableView::CreateScene(const size_t aId, const SceneType aSceneType, const size_t aPlatformIndex)
{
   if (aSceneType == SceneType::cABT)
   {
      // Default tree is the first key
      auto      tree     = mInterfacePtr->GetBehaviorTrees().value(aPlatformIndex).value(aId);
      ABTScene* abtScene = new ABTScene(aId, aSceneType, tree, this);
      mScenes.push_back(abtScene);

      // Set the initial state for the scene
      // Run the exec for the scene, so that the node colors match what they should be at the current time.
      rv::MsgBehaviorTreeState* treeState = mInterfacePtr->GetBehaviorTrees()[aPlatformIndex][abtScene->Id()].mState;
      abtScene->UpdateState(treeState);

      mActiveScene = abtScene;
   }
   else // cFSM
   {
      // Default tree is the first key
      auto      fsm      = mInterfacePtr->GetFSMs().value(aPlatformIndex).value(aId);
      FSMScene* fsmScene = new FSMScene(aId, aSceneType, fsm, this);
      mScenes.push_back(fsmScene);

      // Set the initial state for the scene
      // Run the exec for the scene, so that the node colors match what they should be at the current time.
      rv::MsgStateMachineState* fsmState = mInterfacePtr->GetFSMs()[aPlatformIndex][fsmScene->Id()].mState;
      fsmScene->UpdateState(fsmState);

      mActiveScene = fsmScene;
   }
}

void RvBAT::MovableView::LoadScene(const size_t aId, const SceneType aSceneType)
{
   // If we can find the scene, set it as the active scene
   auto it = std::find_if(mScenes.begin(),
                          mScenes.end(),
                          [aId, aSceneType](const Scene* scene)
                          { return scene->Id() == aId && scene->GetSceneType() == aSceneType; });
   if (it != mScenes.end())
   {
      mActiveScene = *it;
   }
}

void RvBAT::MovableView::ZoomIn()
{
   GentleZoom(1 + mZoomDelta);
}

void RvBAT::MovableView::ZoomOut()
{
   GentleZoom(1 - mZoomDelta);
}

void RvBAT::MovableView::Pan(QPointF aDelta)
{
   // Scale the pan amount by the current zoom.
   aDelta *= mViewScale;
   aDelta *= mPanSpeed;

   // Have panning be anchored from the mouse.
   setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
   QPoint newCenter(viewport()->rect().width() / 2 - aDelta.x(), viewport()->rect().height() / 2 - aDelta.y());
   centerOn(mapToScene(newCenter));
   mActiveScene->SetCenterPoint(mapToScene(newCenter));

   // For zooming to anchor from the view center.
   setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}
