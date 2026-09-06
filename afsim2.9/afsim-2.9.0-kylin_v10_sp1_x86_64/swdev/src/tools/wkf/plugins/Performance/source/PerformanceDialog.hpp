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
#ifndef PERFORMANCEDIALOG_HPP
#define PERFORMANCEDIALOG_HPP

#include <chrono>

#include <QDialog>

#include "UtQtMemoryUsage.hpp"
#include "ui_PerformanceDialog.h"

namespace Performance
{
class Dialog : public QDialog
{
   Q_OBJECT

public:
   Dialog(QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());

   void Update();

private:
   void Build();

   Ui::PerformanceDialog mUi;

   int    mMemoryOverlayID;
   double mMemoryMin;
   double mMemoryMax;

   UtQtMemoryUsage mMemoryUsage;

   std::chrono::time_point<std::chrono::system_clock> mStartTime;
};
} // namespace Performance
#endif
