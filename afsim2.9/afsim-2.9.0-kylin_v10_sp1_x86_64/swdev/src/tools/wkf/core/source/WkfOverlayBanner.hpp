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
#ifndef WKFOVERLAYBANNER_HPP
#define WKFOVERLAYBANNER_HPP

#include <QColor>
#include <QFont>
#include <QString>
#include <osg/MatrixTransform>

#include "VaCallbackHolder.hpp"
#include "VaDecoratorNode.hpp"
#include "VaOverlay.hpp"

namespace wkf
{
class OverlayBanner : public vespa::VaOverlay
{
public:
   OverlayBanner();
   explicit OverlayBanner(const std::string& aName);
   ~OverlayBanner() override = default;

   OverlayBanner* Clone() const override { return nullptr; }
   bool           Initialize() override { return true; }
   void           Setup(const QString& aText, const QColor& aColor, const QFont& aFont);

private:
   void ViewerResizedCB(vespa::VaViewer* aViewerPtr);

   vespa::VaCallbackHolder              mCallbacks;
   UtoShape*                            mShapePtr;
   osg::ref_ptr<osg::MatrixTransform>   mTransform;
   osg::ref_ptr<vespa::VaDecoratorNode> mTextNode;
};
} // namespace wkf
#endif
