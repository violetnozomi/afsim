/**
 * @file NrmDemandDetailTabs.hpp
 * @brief Provides the full-height secondary tabs for demand matching details.
 */

#ifndef NRM_DEMAND_DETAIL_TABS_HPP
#define NRM_DEMAND_DETAIL_TABS_HPP

#include <QTabWidget>

namespace WkNrm
{
class DemandDetailTabs : public QTabWidget
{
public:
   explicit DemandDetailTabs(QWidget* aParentPtr = nullptr);

   void SetPages(QWidget* aDemandPagePtr,
                 QWidget* aMatchPagePtr,
                 QWidget* aGapPagePtr,
                 QWidget* aRecommendationPagePtr,
                 QWidget* aEvidencePagePtr);
   void ShowDemands();
   void ShowEvaluation(bool aHasFailures, bool aHasRecommendations);
   void ShowTechnicalEvidence();

private:
   QWidget* mDemandPagePtr = nullptr;
   QWidget* mMatchPagePtr = nullptr;
   QWidget* mGapPagePtr = nullptr;
   QWidget* mRecommendationPagePtr = nullptr;
   QWidget* mEvidencePagePtr = nullptr;
};
} // namespace WkNrm

#endif
