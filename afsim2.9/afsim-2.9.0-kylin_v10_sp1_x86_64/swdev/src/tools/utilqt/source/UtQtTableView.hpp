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
#ifndef UTQTTABLEVIEW_HPP
#define UTQTTABLEVIEW_HPP

#include "utilqt_export.h"

#include <QTableView>

//! A table view with extra features:
//! * Provides Ctrl-C copy functionality
class UTILQT_EXPORT UtQtTableView : public QTableView
{
public:
   explicit UtQtTableView(QWidget* parent = nullptr)
      : QTableView(parent)
   {
   }
   void keyPressEvent(QKeyEvent* e) override;
};
#endif
