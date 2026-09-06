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

#include "WkfOverlayUpdater.hpp"

#include <QApplication>
#include <QColorDialog>
#include <QDesktopWidget>
#include <QDropEvent>
#include <QFontDialog>
#include <QFontMetrics>
#include <QGLWidget>
#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QMimeData>
#include <QPainter>
#include <QStyle>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>

#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfOverlayUpdaterPrefObject.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::OverlayUpdater::OverlayUpdater(vespa::VaViewer*               aViewerPtr,
                                    std::unique_ptr<wkf::Updater>& aUpdater,
                                    const QString&                 aPlatformName,
                                    int                            aX,
                                    int                            aY)
   : vespa::VaOverlayQt("UpdaterOverlay", vespa::VaOverlay::cOVERLAY_WINDOW, true)
   , mDragging(false)
   , mPlatformName(aPlatformName)
{
   SetType<wkf::OverlayUpdater>();
   mUpdaterList.emplace_back(std::move(aUpdater));
   aViewerPtr->AddOverlay(this);
   Build(aX, aY);

   Update();

   mConnection = QObject::connect(wkfEnv.GetPreferenceObject<OverlayUpdaterPrefObject>(),
                                  &OverlayUpdaterPrefObject::OverlayUpdatersChanged,
                                  [this]()
                                  {
                                     if (mNodePtr)
                                        mNodePtr->UpdateFonts();
                                  });
   mCallbacks.Add(
      vespa::VaObserver::ViewerResized.Connect(&OverlayUpdater::ViewerResizedCB, this, GetViewer()->GetUniqueId()));
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      wkf::Platform* p = scenarioPtr->FindPlatformByName(aPlatformName);
      if (p)
      {
         mCallbacks.Add(
            vespa::VaObserver::EntityDeleted.Connect(&OverlayUpdater::PlatformDeletedCB, this, p->GetUniqueId()));
      }
   }
}

void wkf::OverlayUpdater::Build(int aX, int aY)
{
   mNodePtr = new wkf::UpdaterNode(mPlatformName, aX, aY);

   UtoRawShape raw;
   raw.SetReferenceFrame(UtoShape::cFRAME_ENTITY);

   mShapePtr = VaOverlay::BindShape(GetName(), raw);
   mShapePtr->GetTree()->addChild(mNodePtr.get());
   mShapePtr->OverrideRenderBin(100, false);
}


void wkf::OverlayUpdater::ViewerResizedCB(vespa::VaViewer* aViewerPtr)
{
   int x = mNodePtr->GetX();
   int y = mNodePtr->GetY();
   if (x < 0)
   {
      x = 0;
   }
   if (y < 0)
   {
      y = 0;
   }
   if (x + mNodePtr->GetWidth() > GetViewer()->GetWidth())
   {
      x = GetViewer()->GetWidth() - mNodePtr->GetWidth();
   }
   if (y + mNodePtr->GetHeight() > GetViewer()->GetHeight())
   {
      y = GetViewer()->GetHeight() - mNodePtr->GetHeight();
   }

   mNodePtr->Reposition(x, y);
}

void wkf::OverlayUpdater::PlatformDeletedCB(vespa::VaEntity* aEntityPtr)
{
   if (aEntityPtr->GetName() == mPlatformName.toStdString())
   {
      GetViewer()->DeleteLater(this);
   }
}

wkf::OverlayUpdater::~OverlayUpdater()
{
   QObject::disconnect(mConnection);
}

void wkf::OverlayUpdater::Update()
{
   std::vector<std::pair<QString, std::pair<QString, QString>>> data;
   for (auto&& upd : mUpdaterList)
   {
      if (upd)
      {
         data.emplace_back(
            std::make_pair(upd->GetUpdaterName(), std::make_pair(upd->GetDisplayName(), upd->GetValueString())));
      }
   }
   mNodePtr->SetText(data);
}

void wkf::UpdaterNode::UpdateFonts()
{
   int                       hdpif = QApplication::desktop()->devicePixelRatio();
   OverlayUpdaterPrefObject* po    = wkfEnv.GetPreferenceObject<OverlayUpdaterPrefObject>();
   if (mTitleLabel)
   {
      QFont f = po->LookupFont("title");
      f.setPointSize(f.pointSize() * hdpif);
      QColor   c       = po->LookupColor("title");
      QPalette palette = mTitleLabel->palette();
      palette.setColor(QPalette::WindowText, c);
      mTitleLabel->setPalette(palette);
      mTitleLabel->setFont(f);
   }
   for (auto&& label : mUpdaterLabels)
   {
      QFont f = po->LookupFont(label->objectName());
      f.setPointSize(f.pointSize() * hdpif);
      QColor   c       = po->LookupColor(label->objectName());
      QPalette palette = label->palette();
      palette.setColor(QPalette::WindowText, c);
      label->setPalette(palette);
      label->setFont(f);
   }
}

bool wkf::OverlayUpdater::SelectableAt(int aMouseX, int aMouseY) const
{
   return mNodePtr->CursorInside(aMouseX, aMouseY);
}

bool wkf::OverlayUpdater::Btn1Down(int aMouseX, int aMouseY, int aState)
{
   if (mNodePtr->CursorInside(aMouseX, aMouseY))
   {
      mDragging            = true;
      mDragOverlayPosition = mNodePtr->GetOffset();
      mDragMousePosition   = std::make_pair(aMouseX, aMouseY);
      if (mNodePtr->HitsX(aMouseX, aMouseY))
      {
         GetViewer()->DeleteLater(this);
      }
      return true;
   }
   return false;
}

bool wkf::OverlayUpdater::Btn1Drag(int aMouseX, int aMouseY, int aState)
{
   if (mDragging)
   {
      std::pair<int, int> movement =
         std::make_pair(aMouseX - mDragMousePosition.first, aMouseY - mDragMousePosition.second);

      int x = mDragOverlayPosition.first + movement.first;
      int y = mDragOverlayPosition.second + movement.second;
      if (x < 0)
      {
         x = 0;
      }
      if (y < 0)
      {
         y = 0;
      }
      if (x + mNodePtr->GetWidth() > GetViewer()->GetWidth())
      {
         x = GetViewer()->GetWidth() - mNodePtr->GetWidth();
      }
      if (y + mNodePtr->GetHeight() > GetViewer()->GetHeight())
      {
         y = GetViewer()->GetHeight() - mNodePtr->GetHeight();
      }

      mNodePtr->Reposition(x, y);
      return true;
   }
   return false;
}

bool wkf::OverlayUpdater::Btn1Up(int aMouseX, int aMouseY, int aState)
{
   mDragging = false;
   return false;
}

bool wkf::OverlayUpdater::MouseMove(int aMouseX, int aMouseY)
{
   if (mNodePtr->CursorInside(aMouseX, aMouseY))
   {
      mNodePtr->DrawX(true);
   }
   else
   {
      mNodePtr->DrawX(false);
   }
   return false;
}

bool wkf::OverlayUpdater::DropEvent(QDropEvent* aEventPtr)
{
   if (aEventPtr->mimeData()->hasFormat(wkf::Updater::cMIME_IDENTIFIER))
   {
      aEventPtr->acceptProposedAction();
      QByteArray   ba = aEventPtr->mimeData()->data(wkf::Updater::cMIME_IDENTIFIER);
      QDataStream  in(&ba, QIODevice::ReadOnly);
      unsigned int qi;
      size_t       pluginId;
      QString      platformName;
      QString      datum;
      unsigned int context;
      in >> qi;
      in >> platformName;
      in >> datum;
      in >> context;
      pluginId = static_cast<size_t>(qi);

      if (platformName == mPlatformName)
      {
         auto plugin = wkfEnv.GetPluginManager()->GetPlugin(pluginId);
         if (plugin)
         {
            auto updaterPtr = plugin->GetUpdater(platformName, datum, context);
            if (updaterPtr)
            {
               mUpdaterList.emplace_back(std::move(updaterPtr));
               return true;
            }
         }
      }
   }
   return false;
}

void wkf::OverlayUpdater::AddUpdater(std::unique_ptr<wkf::Updater> aNewUpdater)
{
   if (aNewUpdater)
   {
      mUpdaterList.emplace_back(std::move(aNewUpdater));
   }
}

void wkf::OverlayUpdater::ClearUpdaters()
{
   mUpdaterList.clear();
}

wkf::UpdaterNode::UpdaterNode(const QString& aTitle, float aOffsetX /*= 20.0f*/, float aOffsetY /*= 20.0f*/)
   : osg::Group()
   , mOffset(aOffsetX - (static_cast<int>(aOffsetX) % 10), aOffsetY - (static_cast<int>(aOffsetY) % 10))
   , mDrawX(false)
{
   OverlayUpdaterPrefObject* po = wkfEnv.GetPreferenceObject<OverlayUpdaterPrefObject>();

   mGeode    = new osg::Geode{};
   auto geom = osg::createTexturedQuadGeometry(osg::Vec3{0.0f, 0.0f, 0.0f},
                                               osg::Vec3{1.0f, 0.0f, 0.0f},
                                               osg::Vec3{0.0f, 1.0f, 0.0f},
                                               0.0f,
                                               0.0f,
                                               1.0f,
                                               1.0f);
   geom->setCullingActive(false);
   mGeode->addDrawable(geom);

   auto off = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE;

   auto stateSet = mGeode->getOrCreateStateSet();
   stateSet->setMode(GL_LIGHTING, off);

   stateSet->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 0.0, 1.0, false));
   mMatrixNode = new osg::MatrixTransform;
   mMatrixNode->setMatrix(osg::Matrix::scale(osg::Vec3(mWidth, mHeight, 1)) *
                          osg::Matrix::translate(osg::Vec3(mOffset.x(), mOffset.y(), 0)));
   mMatrixNode->addChild(mGeode);

   addChild(mMatrixNode);

   mFrame.setFrameShape(QFrame::StyledPanel);
   QVBoxLayout* vlo = new QVBoxLayout();
   vlo->setContentsMargins(3, 3, 3, 3);
   mGridLayoutPtr = new QGridLayout();
   mGridLayoutPtr->setContentsMargins(0, 0, 0, 0);
   mFrame.setLayout(vlo);
   int hdpif = QApplication::desktop()->devicePixelRatio();
   if (!aTitle.isEmpty())
   {
      mTitleLabel = new QLabel(aTitle, &mFrame);
      mTitleLabel->setObjectName("title");
      QFont f = po->LookupFont("title");
      f.setPointSize(f.pointSize() * hdpif);

      QColor   c       = po->LookupColor("title");
      QPalette palette = mTitleLabel->palette();
      palette.setColor(QPalette::WindowText, c);
      mTitleLabel->setPalette(palette);


      mTitleLabel->setFont(f);
      vlo->addWidget(mTitleLabel, 0, Qt::AlignCenter);
      QFrame* line = new QFrame(&mFrame);
      line->setFrameShape(QFrame::HLine);
      line->setFrameShadow(QFrame::Sunken);
      vlo->addWidget(line);
   }
   vlo->addLayout(mGridLayoutPtr);
}

void wkf::UpdaterNode::SetText(const std::vector<std::pair<QString, std::pair<QString, QString>>>& aData)
{
   mData = aData;
   Update();
}

void wkf::UpdaterNode::Update()
{
   OverlayUpdaterPrefObject* po = wkfEnv.GetPreferenceObject<OverlayUpdaterPrefObject>();

   int hdpif = QApplication::desktop()->devicePixelRatio();
   for (auto&& l : mUpdaterLabels)
   {
      delete l;
   }
   mUpdaterLabels.clear();
   int row = 0;
   for (auto&& datum : mData)
   {
      QLabel* n = new QLabel(datum.second.first, &mFrame);
      QLabel* v = new QLabel(datum.second.second, &mFrame);
      n->setObjectName(datum.first);
      v->setObjectName(datum.first);
      QFont f = po->LookupFont(datum.first);
      f.setPointSize(f.pointSize() * hdpif);
      n->setFont(f);
      v->setFont(f);

      const QColor& c       = po->LookupColor(datum.first);
      QPalette      palette = n->palette();
      palette.setColor(QPalette::WindowText, c);
      n->setPalette(palette);
      v->setPalette(palette);

      mUpdaterLabels.emplace_back(n);
      mUpdaterLabels.emplace_back(v);
      mGridLayoutPtr->addWidget(n, row, 0);
      mGridLayoutPtr->addWidget(v, row, 1);
      ++row;
   }
   mFrame.adjustSize();

   QRect r = mFrame.rect();

   int w = 16;
   while (w < r.width() + 6)
   {
      w *= 2;
   }
   int h = 16;
   while (h < r.height() + 6)
   {
      h *= 2;
   } // find a power of two size that fits... this will prevent osg from scaling "for us"
   QPixmap  pixmap(w, h);
   QPainter painter(&pixmap);
   mFrame.render(&painter, QPoint(0, h - r.height()));

   if (mDrawX)
   {
      QIcon closeIcon = QIcon::fromTheme("delete");
      QRect buttonR(r.width() - 19, h - r.height() + 3, 16, 16);
      painter.drawPixmap(buttonR, closeIcon.pixmap(16, 16));
   }

   QImage image = pixmap.toImage();
   SetTextureFromImage(image, osg::Texture2D::NEAREST);
   AdjustGeometry(r.width(), r.height(), (r.width()) / (float)w, (r.height()) / (float)h);
}

bool wkf::UpdaterNode::CursorInside(int aX, int aY) const
{
   if ((aX > mOffset.x()) && (aY > mOffset.y()) && (aX < mOffset.x() + mWidth) && (aY < mOffset.y() + mHeight))
   {
      return true;
   }
   return false;
}

bool wkf::UpdaterNode::HitsX(int aX, int aY) const
{
   if (mDrawX)
   {
      if ((aX > mOffset.x() + mWidth - 19) && (aY > mOffset.y() + mHeight - 19) && (aX < mOffset.x() + mWidth - 3) &&
          (aY < mOffset.y() + mHeight - 3))
      {
         return true;
      }
   }
   return false;
}

void wkf::UpdaterNode::Reposition(int aX, int aY)
{
   mOffset.set(aX - aX % 10, aY - aY % 10);
   mMatrixNode->setMatrix(osg::Matrix::scale(osg::Vec3(mWidth, mHeight, 1)) *
                          osg::Matrix::translate(osg::Vec3(mOffset.x(), mOffset.y(), 0)));
}

std::pair<int, int> wkf::UpdaterNode::GetOffset() const
{
   return std::make_pair(static_cast<int>(mOffset.x()), static_cast<int>(mOffset.y()));
}

void wkf::UpdaterNode::DrawX(bool aState)
{
   if (aState != mDrawX)
   {
      mDrawX = aState;
      Update();
   }
}

void wkf::UpdaterNode::SetTextureFromImage(const QImage& aImage, const osg::Texture2D::FilterMode& aFilter)
{
   QImage GL_formatted = QGLWidget::convertToGLFormat(aImage);

   osg::Image*    textureImagePtr = new osg::Image;
   size_t         tsize           = aImage.width() * aImage.height() * 4;
   unsigned char* newData         = new unsigned char[tsize];
   memcpy(newData, GL_formatted.bits(), tsize);

   textureImagePtr
      ->setImage(aImage.width(), aImage.height(), 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, newData, osg::Image::USE_NEW_DELETE);
   osg::Texture2D* tex = new osg::Texture2D(textureImagePtr);
   tex->setBorderColor(osg::Vec4d(1.0, 1.0, 1.0, 0));
   tex->setFilter(osg::Texture2D::MIN_FILTER, aFilter);
   tex->setFilter(osg::Texture2D::MAG_FILTER, aFilter);
   tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
   tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
   tex->setBorderColor(osg::Vec4(0.0, 0.0, 0.0, 0.0));
   mMatrixNode->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
}

void wkf::UpdaterNode::AdjustGeometry(int aWidth, int aHeight, float aUScale /*= 1.0f*/, float aVScale /*= 1.0f*/)
{
   mGeode->removeDrawables(0, mGeode->getNumDrawables());
   auto geom = osg::createTexturedQuadGeometry(osg::Vec3{0.0f, 0.0f, 0.0f},
                                               osg::Vec3{1.0f, 0.0f, 0.0f},
                                               osg::Vec3{0.0f, 1.0f, 0.0f},
                                               0.0f,
                                               0.0f,
                                               aUScale,
                                               aVScale);
   geom->setCullingActive(false);
   mGeode->addDrawable(geom);

   mWidth  = aWidth;
   mHeight = aHeight;
   mMatrixNode->setMatrix(osg::Matrix::scale(osg::Vec3(mWidth, mHeight, 1)) *
                          osg::Matrix::translate(osg::Vec3(mOffset.x(), mOffset.y(), 0)));
}

void wkf::UpdaterNode::BuildContextMenu(QMenu& aMenu, const QPoint& aPoint)
{
   QPoint   local(aPoint.x() - mOffset.x(), mHeight - aPoint.y() + mOffset.y());
   QWidget* child = mFrame.childAt(local);
   if (child)
   {
      QString id = child->objectName();
      QObject::connect(aMenu.addAction(QString("Set %1 font style").arg(id)),
                       &QAction::triggered,
                       [this, id]() { SetFontStyle(id); });
      QObject::connect(aMenu.addAction(QString("Set %1 font color").arg(id)),
                       &QAction::triggered,
                       [this, id]() { SetFontColor(id); });
   }
}

void wkf::UpdaterNode::SetFontColor(const QString& aDatum)
{
   OverlayUpdaterPrefObject* po           = wkfEnv.GetPreferenceObject<OverlayUpdaterPrefObject>();
   const QColor&             currentColor = po->LookupColor(aDatum);
   QColor                    newColor =
      QColorDialog::getColor(currentColor, wkfEnv.GetMainWindow(), QString("Choose Color for %1").arg(aDatum));
   if (newColor.isValid() && (newColor != currentColor))
   {
      po->AddColor(aDatum, newColor);
   }
}

void wkf::UpdaterNode::SetFontStyle(const QString& aDatum)
{
   OverlayUpdaterPrefObject* po          = wkfEnv.GetPreferenceObject<OverlayUpdaterPrefObject>();
   const QFont&              currentFont = po->LookupFont(aDatum);
   bool                      ok          = true;
   QFont                     newFont =
      QFontDialog::getFont(&ok, currentFont, wkfEnv.GetMainWindow(), QString("Choose Font for %1").arg(aDatum));
   if (ok)
   {
      po->AddFont(aDatum, newFont);
   }
}

bool wkf::OverlayUpdater::BuildContextMenu(QMenu& aMenu, const QPoint& aPoint) const
{
   mNodePtr->BuildContextMenu(aMenu, aPoint);
   return true;
}
