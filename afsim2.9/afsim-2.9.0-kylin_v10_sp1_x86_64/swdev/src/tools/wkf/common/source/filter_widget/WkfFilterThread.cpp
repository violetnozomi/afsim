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

#include "WkfFilterThread.hpp"

#include <algorithm>
#include <array>
#include <fstream>

#include <QCoreApplication>
#include <QMessageBox>
#include <QProgressDialog>

#include "UtException.hpp"

namespace WkfFilter
{
constexpr int cFILTER_PAGE_SIZE = 1000;

FilterThread::~FilterThread()
{
   Stop();
}

void FilterThread::RunFilter(FilterPtr aFilter, const QAbstractItemModel* aModel)
{
   Stop();
   mFilters = std::move(aFilter);
   mModel   = aModel;
   mCache.clear();
   mIndex = 0;
   start();
}

void FilterThread::ContinueFilter()
{
   Stop();
   start();
}

void FilterThread::RestartFilter()
{
   Stop();
   mCache.clear();
   mIndex = 0;
   start();
}

void FilterThread::Stop()
{
   requestInterruption();
   wait();
}

void FilterThread::WaitForFinish()
{
   wait();
}

std::vector<int> FilterThread::TakeCache()
{
   std::vector<int>            retval;
   std::lock_guard<std::mutex> lock(mMutex);
   retval.swap(mCache);
   return retval;
}

void FilterThread::run()
{
   if (mModel == nullptr)
   {
      return;
   }

   ut::SetupThreadErrorHandling();

   const int rowCount = mModel->rowCount();

   while (!isInterruptionRequested() && (mIndex < rowCount))
   {
      mIndex = FilterPage(mIndex, rowCount);
   }
}

int FilterThread::FilterPage(int aStart, int aRowCount)
{
   std::array<int, cFILTER_PAGE_SIZE> passed;
   int                                passedCount = 0;

   const int pageEnd = std::min(aStart + cFILTER_PAGE_SIZE, aRowCount);
   for (int i = aStart; i < pageEnd; i++)
   {
      if (mFilters == nullptr || mFilters->TestRow(mModel, i))
      {
         passed[passedCount] = i;
         passedCount++;
      }
   }

   if (passedCount > 0)
   {
      {
         std::lock_guard<std::mutex> lock(mMutex);
         mCache.insert(mCache.end(), passed.begin(), passed.begin() + passedCount);
      }
      emit NewDataReady();
   }
   return pageEnd;
}
} // namespace WkfFilter
