/**
 * @file NrmOperationalDetailTabs.hpp
 * @brief Keeps concise operational summaries separate from raw technical evidence.
 */

#ifndef NRM_OPERATIONAL_DETAIL_TABS_HPP
#define NRM_OPERATIONAL_DETAIL_TABS_HPP

#include <QTabWidget>

namespace WkNrm
{
class OperationalDetailTabs : public QTabWidget
{
public:
   explicit OperationalDetailTabs(QWidget* aParentPtr = nullptr);

   void SetPages(QWidget* aSummaryPagePtr,
                 QWidget* aTechnicalPagePtr,
                 const QString& aSummaryTitle,
                 const QString& aTechnicalTitle);
   void ShowSummary();
   void ShowTechnicalDetails();

private:
   QWidget* mSummaryPagePtr = nullptr;
   QWidget* mTechnicalPagePtr = nullptr;
};
} // namespace WkNrm

#endif
