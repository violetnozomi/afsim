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

#ifndef MAPOVERLAYSCALE_HPP
#define MAPOVERLAYSCALE_HPP

#include "vespatk_export.h"

#include "UtoRawShape.hpp"
#include "VaDecoratorNode.hpp"
#include "VaOverlay.hpp"

namespace Map
{
class OverlayScale : public vespa::VaOverlay
{
public:
   OverlayScale();
   explicit OverlayScale(const std::string& aName);
   ~OverlayScale() override;

   OverlayScale* Clone() const override;

   bool Initialize() override;
   void Update() override;

private:
   void BuildDrawings();

   OverlayScale(const OverlayScale& aSrc);

   UtoRawShape*                         mShape;
   osg::ref_ptr<osg::MatrixTransform>   mTransform;
   osg::ref_ptr<osg::Geode>             mDrawings;
   osg::ref_ptr<vespa::VaDecoratorNode> mTextNode;
};
} // end namespace Map

VA_DECLARE_OBJECT_TYPE(Map::OverlayScale)
#endif
