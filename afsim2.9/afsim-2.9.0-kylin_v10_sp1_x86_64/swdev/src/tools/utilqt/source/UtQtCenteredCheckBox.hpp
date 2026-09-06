// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTQTCENTEREDCHECKBOX_HPP
#define UTQTCENTEREDCHECKBOX_HPP

#include "utilqt_export.h"

#include <QCheckBox>
#include <QWidget>

class UTILQT_EXPORT UtQtCenteredCheckBox : public QWidget
{
public:
   UtQtCenteredCheckBox(QWidget* parent = nullptr);
   ~UtQtCenteredCheckBox() override;

   QCheckBox* checkBox() const { return mCheckBox; }

   bool isChecked() { return mCheckBox->isChecked(); }
   void setChecked(bool aChecked) { mCheckBox->setChecked(aChecked); }

private:
   QCheckBox* mCheckBox;
};

#endif // UTQTCENTEREDCHECKBOX_HPP
