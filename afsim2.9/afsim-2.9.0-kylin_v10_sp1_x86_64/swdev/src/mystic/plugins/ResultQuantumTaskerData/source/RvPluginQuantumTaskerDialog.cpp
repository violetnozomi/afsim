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
#include "RvPluginQuantumTaskerDialog.hpp"

#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "WkfMainWindow.hpp"

RvQuantumTasker::Dialog::Dialog(QWidget* aParent)
   : QDialog(aParent)
   , mCurMatrixTime(0.0)
{
   connect(this, SIGNAL(rejected()), this, SLOT(DestroyTable()));
   setGeometry(wkfEnv.GetMainWindow()->width() * 0.5, wkfEnv.GetMainWindow()->height() * 0.5, 600, 400);
   this->setAttribute(Qt::WA_DeleteOnClose, true);
   InitTable();
}

RvQuantumTasker::Dialog::~Dialog()
{
   DestroyTable();
}

void RvQuantumTasker::Dialog::DestroyTable()
{
   if (mMatrix != nullptr)
   {
      delete mMatrix;
      mMatrix = nullptr;
   }
}

void RvQuantumTasker::Dialog::resizeEvent(QResizeEvent* aEvent)
{
   if (mMatrix)
   {
      mMatrix->setGeometry(2, 2, width() - 5, height() - 5);
   }
}

void RvQuantumTasker::Dialog::InitTable()
{
   mMatrix = new QTableWidget(this);
   mMatrix->setGeometry(2, 2, width() - 5, height() - 5);
}

void RvQuantumTasker::Dialog::UpdateTable(const rv::ResultData& aData)
{
   if (mMatrix == nullptr)
   {
      InitTable();
   }

   double        time  = aData.GetSimTime();
   rv::ResultDb* mData = aData.GetDb();

   std::multimap<float, rv::ResultQuantumTaskerMatrixObject*>           QTMatrix = mData->GetQuantumTaskerMatrix();
   std::multimap<float, rv::ResultQuantumTaskerMatrixObject*>::iterator itlow; // itup;

   itlow = QTMatrix.lower_bound((float)time);
   if (itlow == QTMatrix.end())
   {
      return;
   }
   float curLow = itlow->first;

   if (curLow > time)
   {
      if (itlow != QTMatrix.begin())
      {
         --itlow;
         curLow = itlow->first;
      }
      else
      {
         curLow = 0;
      }
   }

   // only update if things have changed
   if (curLow != mCurMatrixTime)
   {
      mCurMatrixTime      = curLow;
      QString windowTitle = "Quantum Tasker Matrix at T=";
      windowTitle.append(QString::number(mCurMatrixTime));
      setWindowTitle(windowTitle);
      // get all the data records with the same time stamp
      std::pair<std::multimap<float, rv::ResultQuantumTaskerMatrixObject*>::iterator,
                std::multimap<float, rv::ResultQuantumTaskerMatrixObject*>::iterator>
         ret;
      ret = QTMatrix.equal_range(curLow);
      mMatrix->clear();
      int numRows = 0;
      int numCols = 0;
      for (auto it = ret.first; it != ret.second; ++it)
      {
         rv::ResultQuantumTaskerMatrixObject* curObj = it->second;

         if (curObj->GetColumn() + 1 > numCols)
         {
            numCols = curObj->GetColumn() + 1;
            mMatrix->setColumnCount(numCols);
         }
         if (curObj->GetRow() + 1 > numRows)
         {
            numRows = curObj->GetRow() + 1;
            mMatrix->setRowCount(numRows);
         }

         QTableWidgetItem* newItem = new QTableWidgetItem();
         // TODO: the winner logic isn't working correctly at this time, revisit
         if (curObj->IsTaskWinner())
         {
            newItem->setBackgroundColor(Qt::green);
            newItem->setTextColor(Qt::black);
         }
         newItem->setText(QString::number(curObj->GetValue()));
         mMatrix->setItem(curObj->GetRow(), curObj->GetColumn(), newItem);
         QTableWidgetItem* curHeader = new QTableWidgetItem();
         curHeader->setText(QString(curObj->GetAssetName().c_str()));
         mMatrix->setHorizontalHeaderItem(curObj->GetColumn(), curHeader);
         QTableWidgetItem* curVHeader = new QTableWidgetItem();
         curVHeader->setText(QString(curObj->GetTaskName().c_str()));
         mMatrix->setVerticalHeaderItem(curObj->GetRow(), curVHeader);
      }
      mMatrix->show();
   }
}
