// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "GraphicsViewWidget.hpp"

#include "ui_GraphicsViewWidget.h"

namespace Designer
{

GraphicsViewWidget::GraphicsViewWidget(QWidget* aParent)
   : QGraphicsView(aParent)
   , mUIPtr(new Ui::GraphicsViewWidget)
{
   mUIPtr->setupUi(this);
}

GraphicsViewWidget::~GraphicsViewWidget()
{
   delete mUIPtr;
}

} // namespace Designer
