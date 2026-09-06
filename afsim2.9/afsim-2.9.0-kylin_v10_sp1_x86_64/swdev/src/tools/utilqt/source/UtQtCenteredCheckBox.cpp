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

#include "UtQtCenteredCheckBox.hpp"

#include <QHBoxLayout>

UtQtCenteredCheckBox::UtQtCenteredCheckBox(QWidget* parent /*= 0*/)
   : QWidget(parent)
{
   setContentsMargins(0, 0, 0, 0);
   setAutoFillBackground(true);

   mCheckBox = new QCheckBox();

   QHBoxLayout* layout = new QHBoxLayout();
   layout->addWidget(mCheckBox);
   layout->setAlignment(mCheckBox, Qt::AlignHCenter | Qt::AlignVCenter);
   layout->setContentsMargins(0, 0, 0, 0);
   layout->setSpacing(0);

   setLayout(layout);
   setFocusProxy(mCheckBox);
}

UtQtCenteredCheckBox::~UtQtCenteredCheckBox() {}
