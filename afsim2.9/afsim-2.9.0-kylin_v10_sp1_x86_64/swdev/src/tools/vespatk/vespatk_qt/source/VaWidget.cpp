// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaWidget.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QTime>
#include <QWheelEvent>
#include <osg/Depth>

#include "UtQt.hpp"
#include "UtoViewer.hpp"
#include "VaEnvironment.hpp"
#include "VaModelViewer.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

namespace
{
struct HoverFunctor
{
   const std::map<vespa::VaHitEntry, std::string>* mEntryMap;
   const std::function<void(vespa::VaHitEntry)>*   mFunctionPtr;
   void                                            operator()(QAction* aActionPtr)
   {
      if (mFunctionPtr && aActionPtr)
      {
         int idx = aActionPtr->data().toInt();
         if (idx >= 0 && idx < (int)mEntryMap->size())
         {
            std::map<vespa::VaHitEntry, std::string>::const_iterator iter = mEntryMap->begin();
            std::advance(iter, idx);
            (*mFunctionPtr)(iter->first);
         }
      }
   }
};

int GetModifierState(const Qt::KeyboardModifiers& aMods)
{
   return ((aMods & Qt::ShiftModifier) ? vespa::VaViewer::ShiftKey : 0) |
          ((aMods & Qt::ControlModifier) ? vespa::VaViewer::CtrlKey : 0) |
          ((aMods & Qt::AltModifier) ? vespa::VaViewer::AltKey : 0) |
          ((aMods & Qt::KeypadModifier) ? vespa::VaViewer::NumLockState : 0);
}
} // namespace

vespa::VaWidget::VaWidget(QWidget* aParentPtr, bool aPrimeContext)
   : QOpenGLWidget(aParentPtr)
   , mGraphicsWindow(new osgViewer::GraphicsWindowEmbedded(x(), y(), width(), height()))
   , mOsgViewer(new osgViewer::Viewer)
   , mChooser(this)
{
   mHighDPI_ScalingFactor = QApplication::desktop()->devicePixelRatio();

   setMinimumSize(256, 256);
   osg::Camera* camera = new osg::Camera;
   camera->setViewport(0, 0, width(), height());
   camera->setClearColor(osg::Vec4(0.9f, 0.9f, 1.0f, 1.0f));
   float aspectRatio = static_cast<float>(width()) / static_cast<float>(height());
   camera->setProjectionMatrixAsPerspective(30.0f, aspectRatio, 1.0f, 1000.0f);
   camera->setGraphicsContext(mGraphicsWindow);
   mOsgViewer->setCamera(camera);
   setMouseTracking(true);
   setFocusPolicy(Qt::StrongFocus);
   mOsgViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
   mOsgViewer->realize();
   camera->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth);
   osg::StateSet* ss = camera->getOrCreateStateSet();
   ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
   mGraphicsWindow->getState()->setUseModelViewAndProjectionUniforms(true);
   setAcceptDrops(true);
}

vespa::VaWidget::VaWidget(unsigned int aViewerType, unsigned int aViewerModes, QWidget* aParentPtr, bool aPrimeTheContext)
   : VaWidget(aParentPtr, aPrimeTheContext)
{
   if (aViewerType != VaViewer::NoViewer)
   {
      if (aViewerType != VaViewer::ModelViewer)
      {
         SetViewer(new VaViewer(aViewerType, aViewerModes));
      }
      else
      {
         SetViewer(new VaModelViewer());
      }
   }
   // Note: this is sort of a hack to get a valid context needed for initialization.
   // We show the widget, have the app process the events, initialize, and then hide
   if (aPrimeTheContext)
   {
      setParent(nullptr);
      show();
      qApp->processEvents();
      hide();
      setParent(aParentPtr);
   }
}

vespa::VaWidget::VaWidget(VaViewer* aViewerPtr, QWidget* aParentPtr, bool aPrimeContext /*= false*/)
   : VaWidget(aParentPtr, aPrimeContext)
{
   SetViewer(aViewerPtr);
   // Note: this is sort of a hack to get a valid context needed for initialization.
   // We show the widget, have the app process the events, initialize, and then hide
   if (aPrimeContext)
   {
      setParent(nullptr);
      show();
      qApp->processEvents();
      hide();
      setParent(aParentPtr);
   }
}

vespa::VaWidget::~VaWidget()
{
   if (vespa::VaEnvironment::Exists())
   {
      vespa::VaEnvironment::Instance().GetViewerManager()->RemoveViewer(mVaViewer);
   }
   delete mVaViewer;
}

void vespa::VaWidget::paintGL()
{
   if (mVaViewer)
   {
      mVaViewer->PrepareToRedraw();
   }
   mOsgViewer->frame();
   //   mVaViewer->Redraw();
}

void vespa::VaWidget::resizeGL(int aWidth, int aHeight)
{
   mGraphicsWindow->resized(x() * mHighDPI_ScalingFactor,
                            y() * mHighDPI_ScalingFactor,
                            width() * mHighDPI_ScalingFactor,
                            height() * mHighDPI_ScalingFactor);
   osg::Camera* camera = mOsgViewer->getCamera();
   camera->setViewport(0, 0, width() * mHighDPI_ScalingFactor, height() * mHighDPI_ScalingFactor);
   if (mVaViewer)
   {
      mVaViewer->Resize(0, 0, aWidth * mHighDPI_ScalingFactor, aHeight * mHighDPI_ScalingFactor);
   }
}

void vespa::VaWidget::initializeGL()
{
   if (!mInitialized)
   {
      makeCurrent();
      bool shaderable     = false;
      bool geomShaderable = false;

      const char* extensions = (char*)glGetString(GL_EXTENSIONS);
      if (nullptr != extensions)
      {
         if ((nullptr != strstr(extensions, "GL_ARB_shader_objects")) &&
             (nullptr != strstr(extensions, "GL_ARB_vertex_shader")) &&
             (nullptr != strstr(extensions, "GL_ARB_fragment_shader")))
         {
            shaderable = true;
         }
         if (nullptr != extensions)
         {
            if (nullptr != strstr(extensions, "GL_ARB_geometry_shader4"))
            {
               geomShaderable = true;
            }
         }
      }
      const char* version   = (char*)glGetString(GL_VERSION);
      const char* slversion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
      if (version && slversion && extensions)
      {
         vespa::VaEnvironment::Instance().SetGL_Info(version, slversion, extensions);
      }

      mInitialized = true;
      if (mVaViewer)
      {
         mVaViewer->Initialize(mOsgViewer, shaderable, geomShaderable);

         // some of our GLSL environments don't support uniform defaulting in shader-code, so lets make sure these are
         // zero here I don't trust drivers to get it right.  Any viewer that needs a value of one should be setting it
         // up after this anyway.
         mVaViewer->SetUniform("VA_LogDepth", 0.0f);
         mVaViewer->GetViewer()->GetOSGViewer()->getCamera()->getOrCreateStateSet()->addUniform(
            new osg::Uniform("VA_LogDepth", 0.0f));
         mVaViewer->SetUniform("VA_ModelsSupportLighting", 0.0f);

         emit Initialized();
      }
      doneCurrent();
   }
}

void vespa::VaWidget::wheelEvent(QWheelEvent* aEvent)
{
   if (mVaViewer)
   {
      int amt = aEvent->delta();
      if (amt > 0)
      {
         mVaViewer->WheelUp(-amt);
      }
      else
      {
         mVaViewer->WheelDown(-amt);
      }
   }
}

void vespa::VaWidget::MouseMove(int aX, int aY, int aMods, int aButtons)
{
   mClickPossible[0] = mClickPossible[1] = mClickPossible[2] = false;

   if (aButtons & Qt::LeftButton)
   {
      mVaViewer->Btn1Drag(aX, aY, aMods);
   }
   else if (aButtons & Qt::MidButton)
   {
      mVaViewer->Btn2Drag(aX, aY, aMods);
   }
   else if (aButtons & Qt::RightButton)
   {
      if (!mBlockRightClick)
      {
         mVaViewer->Btn3Drag(aX, aY, aMods);
      }
   }
   else
   {
      mVaViewer->MouseMove(aX, aY);
   }
}

void vespa::VaWidget::SetViewer(vespa::VaViewer* aViewerPtr)
{
   mVaViewer = aViewerPtr;

   vespa::VaEnvironment::Instance().GetViewerManager()->AddViewer(mVaViewer);
   mCallbacks.Add(vespa::VaObserver::ViewerDestroyed.Connect(&VaWidget::ViewerDestroyedCB, this));
}

void vespa::VaWidget::ViewerDestroyedCB(vespa::VaViewer* aViewerPtr)
{
   if (mVaViewer)
   {
      if (aViewerPtr->GetUniqueId() == mVaViewer->GetUniqueId())
      {
         // something else, probably the viewer manager is cleaning up our viewer
         mVaViewer = nullptr;
      }
   }
}

void vespa::VaWidget::mousePressEvent(QMouseEvent* aEvent)
{
   if (mVaViewer)
   {
      int   state       = GetModifierState(aEvent->modifiers());
      QTime currentTime = QTime::currentTime();
      if (aEvent->buttons() & Qt::LeftButton)
      {
         mVaViewer->Btn1Down(aEvent->x() * mHighDPI_ScalingFactor, (height() - aEvent->y()) * mHighDPI_ScalingFactor, state);
         mClickPossible[0]  = true;
         mLeftDown          = true;
         mButtonDownTime[0] = currentTime;
      }
      if (aEvent->buttons() & Qt::MidButton)
      {
         mClickPossible[1]  = true;
         mMiddleDown        = true;
         mButtonDownTime[1] = currentTime;
         mVaViewer->Btn2Down(aEvent->x() * mHighDPI_ScalingFactor, (height() - aEvent->y()) * mHighDPI_ScalingFactor, state);
      }
      if (aEvent->buttons() & Qt::RightButton)
      {
         mClickPossible[2]  = true;
         mRightDown         = true;
         mButtonDownTime[2] = currentTime;
         if (!mBlockRightClick)
         {
            mVaViewer->Btn3Down(aEvent->x() * mHighDPI_ScalingFactor,
                                (height() - aEvent->y()) * mHighDPI_ScalingFactor,
                                state);
         }
      }
   }
}

void vespa::VaWidget::mouseReleaseEvent(QMouseEvent* aEvent)
{
   if (mVaViewer)
   {
      int   state       = GetModifierState(aEvent->modifiers());
      QTime currentTime = QTime::currentTime();

      if (!(aEvent->buttons() & Qt::LeftButton))
      {
         if (mLeftDown)
         {
            // aMState (state of keys, SHIFT, CTRL, etc., sent on through)
            mVaViewer->Btn1Up(aEvent->x() * mHighDPI_ScalingFactor,
                              (height() - aEvent->y()) * mHighDPI_ScalingFactor,
                              state); // Third argument was 0 BAR
            mLeftDown = false;
            if (mClickPossible[0] && mButtonDownTime[0].msecsTo(currentTime) < 500)
            {
               mClickPossible[0] = false;
               mVaViewer->Btn1Click(aEvent->x() * mHighDPI_ScalingFactor,
                                    (height() - aEvent->y()) * mHighDPI_ScalingFactor,
                                    state);
            }
         }
      }
      if (!(aEvent->buttons() & Qt::MidButton))
      {
         if (mMiddleDown)
         {
            mVaViewer->Btn2Up(aEvent->x() * mHighDPI_ScalingFactor, (height() - aEvent->y()) * mHighDPI_ScalingFactor, 0);
            mMiddleDown = false;
            if (mClickPossible[1] && mButtonDownTime[1].msecsTo(currentTime) < 500)
            {
               mClickPossible[1] = false;
               mVaViewer->Btn2Click(aEvent->x() * mHighDPI_ScalingFactor,
                                    (height() - aEvent->y()) * mHighDPI_ScalingFactor,
                                    state);
            }
         }
      }
      if (!(aEvent->buttons() & Qt::RightButton))
      {
         if (mRightDown)
         {
            if (!mBlockRightClick)
            {
               mVaViewer->Btn3Up(aEvent->x() * mHighDPI_ScalingFactor, (height() - aEvent->y()) * mHighDPI_ScalingFactor, 0);
            }
            mRightDown = false;
            if (mClickPossible[2] && mButtonDownTime[2].msecsTo(currentTime) < 500)
            {
               mClickPossible[2] = false;

               if (!mBlockRightClick)
               {
                  mVaViewer->Btn3Click(aEvent->x() * mHighDPI_ScalingFactor,
                                       (height() - aEvent->y()) * mHighDPI_ScalingFactor,
                                       state);
               }
            }
         }
      }
   }
}

void vespa::VaWidget::mouseMoveEvent(QMouseEvent* aEvent)
{
   // Ignore event if the mouse has not moved since the last mouseMoveEvent. It was discovered
   // with Qt 5.12 that a spurious move event may be delivered between the mouse press and mouse
   // release, when a QToolTip is shown, thereby blocking a click.

   if (mVaViewer && (mLastMouseMove != aEvent->pos()))
   {
      // If our we are checking for a drag threshold only block clicks if we past the threshold
      if (!mCheckDragThreshold || (aEvent->pos() - mLastMouseMove).manhattanLength() >= QApplication::startDragDistance())
      {
         mClickPossible[0] = mClickPossible[1] = mClickPossible[2] = false;
      }

      mLastMouseMove = aEvent->pos();
      int state      = GetModifierState(aEvent->modifiers());

      if (aEvent->buttons() & Qt::LeftButton)
      {
         mVaViewer->Btn1Drag(aEvent->x() * mHighDPI_ScalingFactor, (height() - aEvent->y()) * mHighDPI_ScalingFactor, state);
      }
      else if (aEvent->buttons() & Qt::MidButton)
      {
         mVaViewer->Btn2Drag(aEvent->x() * mHighDPI_ScalingFactor, (height() - aEvent->y()) * mHighDPI_ScalingFactor, state);
      }
      else if (aEvent->buttons() & Qt::RightButton)
      {
         if (!mBlockRightClick)
         {
            mVaViewer->Btn3Drag(aEvent->x() * mHighDPI_ScalingFactor,
                                (height() - aEvent->y()) * mHighDPI_ScalingFactor,
                                state);
         }
      }
      else
      {
         mVaViewer->MouseMove(aEvent->x() * mHighDPI_ScalingFactor, (height() - aEvent->y()) * mHighDPI_ScalingFactor);
      }
   }
}

void vespa::VaWidget::mouseDoubleClickEvent(QMouseEvent* aEvent)
{
   if (mVaViewer)
   {
      if (aEvent->buttons() & Qt::LeftButton)
      {
         mClickPossible[0] = false;
         mVaViewer->Btn1DblClick(aEvent->x() * mHighDPI_ScalingFactor,
                                 (height() - aEvent->y()) * mHighDPI_ScalingFactor,
                                 GetModifierState(aEvent->modifiers()));
      }
   }
}

void vespa::VaWidget::keyPressEvent(QKeyEvent* aEvent)
{
   if (mVaViewer)
   {
      if (!aEvent->isAutoRepeat())
      {
         QPoint pt = QCursor::pos();
         mVaViewer->KeyPress(pt.x(), pt.y(), aEvent->key(), GetModifierState(aEvent->modifiers()));
      }
   }
}

void vespa::VaWidget::keyReleaseEvent(QKeyEvent* aEvent)
{
   if (mVaViewer)
   {
      if (!aEvent->isAutoRepeat())
      {
         QPoint pt = QCursor::pos();
         mVaViewer->KeyRelease(pt.x(), pt.y(), aEvent->key(), GetModifierState(aEvent->modifiers()));
      }
   }
}

void vespa::VaWidget::enterEvent(QEvent* aEvent)
{
   if (mVaViewer)
   {
      mVaViewer->MouseEnter();
   }
}

void vespa::VaWidget::leaveEvent(QEvent* aEvent)
{
   if (mVaViewer)
   {
      mVaViewer->MouseLeave();
   }
}

void vespa::VaWidget::dragEnterEvent(QDragEnterEvent* aEvent)
{
   emit DragEnter(aEvent);
}

void vespa::VaWidget::dropEvent(QDropEvent* aEvent)
{
   emit DragDrop(aEvent);
}

void vespa::VaWidget::dragMoveEvent(QDragMoveEvent* aEvent)
{
#ifdef _WIN32
   if (mVaViewer)
   {
      int state = GetModifierState(aEvent->keyboardModifiers());
      MouseMove(aEvent->pos().x(),
                height() - aEvent->pos().y(),
                GetModifierState(aEvent->keyboardModifiers()),
                aEvent->mouseButtons());
   }
   emit DragMove(aEvent);
#endif
}

void vespa::VaWidget::dragLeaveEvent(QDragLeaveEvent* aEvent)
{
   emit DragLeave(aEvent);
}

vespa::VaViewer* vespa::VaWidget::GetViewer()
{
   return mVaViewer;
}

vespa::VaChooser* vespa::VaWidget::GetChooser()
{
   return &mChooser;
}

bool vespa::VaWidget::event(QEvent* aEvent)
{
   bool handled = QOpenGLWidget::event(aEvent);
   update();
   return handled;
}

vespa::VaHitEntry vespa::VaWidgetChooser::Choose(const std::map<VaHitEntry, std::string>& aChoiceList,
                                                 int                                      aX,
                                                 int                                      aY,
                                                 const std::function<void(VaHitEntry)>* aHoverFunction /*= nullptr*/) const
{
   int    highDPI_ScalingFactor = QApplication::desktop()->devicePixelRatio();
   QPoint pt =
      mWidgetPtr->mapToGlobal(QPoint(aX / highDPI_ScalingFactor, mWidgetPtr->height() - aY / highDPI_ScalingFactor));

   if (aChoiceList.empty())
      return vespa::VaHitEntry();
   if (aChoiceList.size() == 1)
      return aChoiceList.begin()->first;
   QMenu popup;
   UtQtTranslucentMenu(popup);
   int idx = 0;
   for (const auto& i : aChoiceList)
   {
      QString  choiceStr = QString::fromStdString(i.second);
      QAction* action    = popup.addAction(choiceStr);
      choiceStr =
         QFontMetrics(action->font()).elidedText(choiceStr, Qt::ElideMiddle, 300); // width is in pixels, not characters
      action->setText(choiceStr);
      action->setData(idx);
      ++idx;
   }
   int          choice = 0;
   HoverFunctor hf;
   hf.mFunctionPtr = aHoverFunction;
   hf.mEntryMap    = &aChoiceList;
   QObject::connect(&popup, &QMenu::hovered, hf);

   QAction* selection = popup.exec(pt);
   if (selection)
   {
      choice = selection->data().toInt();

      std::map<vespa::VaHitEntry, std::string>::const_iterator iter = aChoiceList.begin();
      std::advance(iter, choice);
      return iter->first;
   }
   return vespa::VaHitEntry();
}
