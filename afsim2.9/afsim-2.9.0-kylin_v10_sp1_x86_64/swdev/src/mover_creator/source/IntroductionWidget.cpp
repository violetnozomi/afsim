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

#include "IntroductionWidget.hpp"

#include "ui_IntroductionWidget.h"

namespace Designer
{

IntroductionWidget::IntroductionWidget(QWidget* aParent,
                                       Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::IntroductionWidget)
{
   mUIPtr->setupUi(this);
}

IntroductionWidget::~IntroductionWidget()
{
   delete mUIPtr;
}

} // namespace Designer
