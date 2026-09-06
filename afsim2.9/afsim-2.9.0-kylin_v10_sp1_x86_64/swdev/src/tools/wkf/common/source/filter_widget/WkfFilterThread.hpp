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

#ifndef WKF_FILTER_THREAD_HPP
#define WKF_FILTER_THREAD_HPP

#include "wkf_common_export.h"

#include <mutex>

#include <QSortFilterProxyModel>
#include <QThread>

#include "WkfFilterObject.hpp"

namespace WkfFilter
{
//! The thread that performs the filtering process.
//! Designed to be interruptible to maintain a responsive user interface.
class WKF_COMMON_EXPORT FilterThread : public QThread
{
   Q_OBJECT

public:
   FilterThread() = default;
   ~FilterThread() override;

   //! Runs the filter from the beginning.
   void RunFilter(FilterPtr aFilter, const QAbstractItemModel* aModel);
   //! Runs the filter where it last left off.
   //! Can be useful if rows are appended to the source model.
   void ContinueFilter();
   //! Resets the cache and mIndex, then runs the previously-set filter.
   void RestartFilter();

   //! Requests that the thread stops.
   //! Waits until the thread exits.
   void Stop();
   //! Waits until the thread exits.
   void WaitForFinish();

   //! Retrieves the list of rows that have passed the filter since the last time the cache was retrieved.
   //! Afterwards, the cache is cleared.
   //! Safe to call from the GUI thread.
   std::vector<int> TakeCache();

signals:
   void NewDataReady();

private:
   //! The function called when the thread is started.
   void run() override;

   //! Used by run() to filter a page.
   //! Afterwards, the data is added to mCache.
   //! Returns the index of the next row to filter.
   //! @param aStart is the index of the first row to filter.
   //! @param aRowCount is the number of rows in the model.
   int FilterPage(int aStart, int aRowCount);

   //! Data cache and access control
   //! @{
   std::mutex       mMutex;
   std::vector<int> mCache;
   //! @}

   //! What and how to filter
   //! @{
   const QAbstractItemModel* mModel = nullptr;
   FilterPtr                 mFilters;
   //! @}

   //! The index of the next row to filter.
   int mIndex = 0;
};
} // namespace WkfFilter

#endif
