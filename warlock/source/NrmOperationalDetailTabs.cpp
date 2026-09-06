/**
 * @file NrmOperationalDetailTabs.cpp
 * @brief Implements the operational summary/detail tab container.
 */

#include "NrmOperationalDetailTabs.hpp"

#include <QSizePolicy>

WkNrm::OperationalDetailTabs::OperationalDetailTabs(QWidget* aParentPtr)
   : QTabWidget(aParentPtr)
{
   setObjectName("NrmOperationalDetailTabs");
   setDocumentMode(true);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   setMinimumSize(0, 0);
}

void WkNrm::OperationalDetailTabs::SetPages(
   QWidget* aSummaryPagePtr,
   QWidget* aTechnicalPagePtr,
   const QString& aSummaryTitle,
   const QString& aTechnicalTitle)
{
   mSummaryPagePtr = aSummaryPagePtr;
   mTechnicalPagePtr = aTechnicalPagePtr;
   addTab(mSummaryPagePtr, aSummaryTitle);
   addTab(mTechnicalPagePtr, aTechnicalTitle);
   ShowSummary();
}

void WkNrm::OperationalDetailTabs::ShowSummary()
{
   if (mSummaryPagePtr != nullptr) setCurrentWidget(mSummaryPagePtr);
}

void WkNrm::OperationalDetailTabs::ShowTechnicalDetails()
{
   if (mTechnicalPagePtr != nullptr) setCurrentWidget(mTechnicalPagePtr);
}
