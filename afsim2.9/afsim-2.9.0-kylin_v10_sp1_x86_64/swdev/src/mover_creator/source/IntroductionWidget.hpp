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

#ifndef INTRODUCTION_WIDGET_HPP
#define INTRODUCTION_WIDGET_HPP

#include <QWidget>

namespace Ui
{
   class IntroductionWidget;
}

namespace Designer
{
   class IntroductionWidget : public QWidget
   {
         Q_OBJECT
      public:
         explicit IntroductionWidget(QWidget*        aParent,
                                     Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~IntroductionWidget();

      private:
         Ui::IntroductionWidget* mUIPtr;
   };
}
#endif // !INTRODUCTION_WIDGET_HPP
