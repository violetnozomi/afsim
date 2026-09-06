/**
 * @file NrmDemandDetailTabs.cpp
 * @brief Implements the demand matching full-height secondary tabs.
 */

#include "NrmDemandDetailTabs.hpp"

#include <QSizePolicy>

WkNrm::DemandDetailTabs::DemandDetailTabs(QWidget* aParentPtr)
   : QTabWidget(aParentPtr)
{
   setObjectName("NrmDemandDetailTabs");
   setDocumentMode(true);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   setMinimumSize(0, 0);
}

void WkNrm::DemandDetailTabs::SetPages(
   QWidget* aDemandPagePtr,
   QWidget* aMatchPagePtr,
   QWidget* aGapPagePtr,
   QWidget* aRecommendationPagePtr,
   QWidget* aEvidencePagePtr)
{
   mDemandPagePtr = aDemandPagePtr;
   mMatchPagePtr = aMatchPagePtr;
   mGapPagePtr = aGapPagePtr;
   mRecommendationPagePtr = aRecommendationPagePtr;
   mEvidencePagePtr = aEvidencePagePtr;

   addTab(mDemandPagePtr, QString::fromUtf8("业务需求"));
   addTab(mMatchPagePtr, QString::fromUtf8("匹配结果"));
   addTab(mGapPagePtr, QString::fromUtf8("约束差距"));
   addTab(mRecommendationPagePtr, QString::fromUtf8("调整建议"));
   addTab(mEvidencePagePtr, QString::fromUtf8("技术依据"));
   ShowDemands();
}

void WkNrm::DemandDetailTabs::ShowTechnicalEvidence()
{
   if (mEvidencePagePtr != nullptr) setCurrentWidget(mEvidencePagePtr);
}

void WkNrm::DemandDetailTabs::ShowDemands()
{
   if (mDemandPagePtr != nullptr) setCurrentWidget(mDemandPagePtr);
}

void WkNrm::DemandDetailTabs::ShowEvaluation(bool aHasFailures,
                                              bool aHasRecommendations)
{
   if (aHasFailures && aHasRecommendations &&
       mRecommendationPagePtr != nullptr)
   {
      setCurrentWidget(mRecommendationPagePtr);
   }
   else if (aHasFailures && mGapPagePtr != nullptr)
   {
      setCurrentWidget(mGapPagePtr);
   }
   else if (mMatchPagePtr != nullptr)
   {
      setCurrentWidget(mMatchPagePtr);
   }
}
