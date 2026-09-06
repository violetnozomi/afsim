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
#ifndef MAPBULLSEYEWIDGET_HPP
#define MAPBULLSEYEWIDGET_HPP

#include <QDialog>

#include "ui_WkfBullseye.h"

namespace Annotation
{
class AnnotationDisplayInterface;

class BullseyeWidget : public QWidget
{
public:
   BullseyeWidget(AnnotationDisplayInterface& aDisplayInterface,
                  QWidget*                    parent = nullptr,
                  Qt::WindowFlags             f      = Qt::WindowFlags());
   ~BullseyeWidget() override = default;

   void Show();

private:
   void Apply();

   AnnotationDisplayInterface& mDisplayInterface;

   Ui_BullseyeWidget mUI;
};
} // namespace Annotation
#endif
