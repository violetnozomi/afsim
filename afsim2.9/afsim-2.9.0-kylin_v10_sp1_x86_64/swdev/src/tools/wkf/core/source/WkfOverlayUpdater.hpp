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

#ifndef WKFOVERLAYUPDATER_HPP
#define WKFOVERLAYUPDATER_HPP

#include "wkf_export.h"

#include <memory>

#include <QColor>
#include <QFrame>
#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <osg/Group>
#include <osg/Texture2D>

#include "VaCallbackHolder.hpp"
#include "VaOverlayQt.hpp"
#include "WkfUpdater.hpp"


namespace wkf
{
class WKF_EXPORT UpdaterNode : public osg::Group
{
public:
   UpdaterNode(const QString& mTitle, float aOffsetX = 20.0f, float aOffsetY = 20.0f);

   void SetText(const std::vector<std::pair<QString, std::pair<QString, QString>>>& aData);

   bool                CursorInside(int aX, int aY) const;
   bool                HitsX(int aX, int aY) const;
   void                Reposition(int aX, int aY);
   std::pair<int, int> GetOffset() const;
   void                DrawX(bool aState);
   int                 GetX() const { return static_cast<int>(mOffset.x()); }
   int                 GetY() const { return static_cast<int>(mOffset.y()); }
   int                 GetWidth() const { return mWidth; }
   int                 GetHeight() const { return mHeight; }
   void                BuildContextMenu(QMenu& aMenu, const QPoint& aPoint);
   void                SetFontColor(const QString& aDatum);
   void                SetFontStyle(const QString& aDatum);
   void                UpdateFonts();

private:
   void SetTextureFromImage(const QImage& aImage, const osg::Texture2D::FilterMode& aFilter);
   void AdjustGeometry(int aWidth, int aHeight, float aUScale = 1.0f, float aVScale = 1.0f);
   void Update();

   osg::ref_ptr<osg::MatrixTransform>                           mMatrixNode;
   osg::ref_ptr<osg::Geode>                                     mGeode;
   std::vector<std::pair<QString, std::pair<QString, QString>>> mData;
   osg::Vec2                                                    mOffset;
   int                                                          mWidth;
   int                                                          mHeight;
   bool                                                         mDrawX;
   QFrame                                                       mFrame;
   QGridLayout*                                                 mGridLayoutPtr;
   std::vector<QLabel*>                                         mUpdaterLabels;
   QLabel*                                                      mTitleLabel{nullptr};
};

class WKF_EXPORT OverlayUpdater : public vespa::VaOverlayQt
{
public:
   OverlayUpdater(vespa::VaViewer* aViewerPtr, std::unique_ptr<Updater>& aUpdater, const QString& aPlatformName, int aX, int aY);

   OverlayUpdater* Clone() const override { return nullptr; }
   bool            Initialize() override { return true; }

   ~OverlayUpdater() override;
   void Update() override;

   bool            SelectableAt(int aMouseX, int aMouseY) const override;
   HandleEventMode GetHandleEventMode() const override { return cHANDLE_EVENTS_FOCUS; }

   bool Btn1Down(int aMouseX, int aMouseY, int aState) override;
   bool Btn1Drag(int aMouseX, int aMouseY, int aState) override;
   bool Btn1Up(int aMouseX, int aMouseY, int aState) override;
   bool MouseMove(int aMouseX, int aMouseY) override;
   bool BuildContextMenu(QMenu& aMenu, const QPoint& aPoint) const override;
   bool SupportsDropEvent() const override { return true; }
   bool DropEvent(QDropEvent* aEventPtr) override;
   void AddUpdater(std::unique_ptr<wkf::Updater> aNewUpdater);
   void ClearUpdaters();

private:
   OverlayUpdater(const OverlayUpdater& aSrc) = delete;
   OverlayUpdater& operator=(const OverlayUpdater& aSrc) = delete;

   void Build(int aX, int aY);

   void ViewerResizedCB(vespa::VaViewer* aViewerPtr);
   void PlatformDeletedCB(vespa::VaEntity* aEntityPtr);

   std::vector<std::unique_ptr<Updater>> mUpdaterList;
   UtoShape*                             mShapePtr;
   osg::ref_ptr<UpdaterNode>             mNodePtr;
   bool                                  mDragging;
   std::pair<int, int>                   mDragMousePosition;
   std::pair<int, int>                   mDragOverlayPosition;
   vespa::VaCallbackHolder               mCallbacks;
   QString                               mPlatformName;
   QMetaObject::Connection               mConnection;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::OverlayUpdater)
#endif
