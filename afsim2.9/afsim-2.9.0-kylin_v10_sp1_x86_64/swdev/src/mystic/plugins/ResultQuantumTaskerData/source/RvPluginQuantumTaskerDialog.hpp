// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVPLUGINQUANTUMTASKERDIALOG_HPP
#define RVPLUGINQUANTUMTASKERDIALOG_HPP

#include <QDialog>
#include <QTableWidget>

#include "RvPlugin.hpp"

namespace RvQuantumTasker
{
class Dialog : public QDialog
{
   Q_OBJECT
public:
   Dialog(QWidget* aParent);
   ~Dialog() override;

   void resizeEvent(QResizeEvent* aEvent) override;

   void UpdateTable(const rv::ResultData& aData);
   void InitTable();
public slots:
   void DestroyTable();

private:
   QTableWidget* mMatrix;
   float         mCurMatrixTime;
};
} // namespace RvQuantumTasker
#endif