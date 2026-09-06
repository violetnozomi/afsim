// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef MAPRULERPROPERTIESWIDGET_HPP
#define MAPRULERPROPERTIESWIDGET_HPP

#include <QWidget>

#include "WkfAttachmentGreatCircleRuler.hpp"
#include "ui_RulerProperties.h"
using Ruler     = wkf::AttachmentGreatCircleRuler;
using LabelMode = Ruler::LabelMode;
using RulerMode = Ruler::RulerMode;

namespace Map
{
class RulerPropertiesWidget : public QWidget
{
public:
   RulerPropertiesWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

   void                                   SetRuler(wkf::AttachmentGreatCircleRuler* aRulerPtr);
   const wkf::AttachmentGreatCircleRuler* GetRuler() const { return mRulerPtr; }

   int       GetLabelMode() const { return mLabelMode; }
   RulerMode GetRulerMode() const { return mRulerMode; }

private:
   void UpdateWidget();
   void LabelModeToggled(int aLabelMode, bool aChecked);
   void RulerModeToggled(int aLabelMode, bool aChecked);

   void LoadSettings();
   void SaveSettings();

   int       mLabelMode;
   RulerMode mRulerMode;

   Ui::MapRulerProperties           mUi;
   wkf::AttachmentGreatCircleRuler* mRulerPtr;
};
} // namespace Map
#endif
