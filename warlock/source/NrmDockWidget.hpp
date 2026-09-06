#ifndef NRM_DOCK_WIDGET_HPP
#define NRM_DOCK_WIDGET_HPP

#include <QDockWidget>

class QLabel;
class QComboBox;
class QDoubleSpinBox;
class QPushButton;
class QTableWidget;
class QTabWidget;
class QTextEdit;
class QWidget;

#include "NrmDataContainer.hpp"
#include "NrmPreacceptanceStatusMonitor.hpp"

namespace WkNrm
{
class AcceptanceDemoPanel;
class DemandDetailTabs;
class OperationalDetailTabs;

class DockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit DockWidget(DataContainer& aData, QWidget* aParentPtr = nullptr);
   void ShowAcceptanceDemo();
   void ShowNetworkPlan();

public slots:
   void ApplyTacticalPlatformSelection(const QString& aPlatformName, int aAssignment);
   void SetTacticalSelectionTarget(int aTarget);
   void SetUiScalePercent(int aPercent);

signals:
   void TacticalSelectionRequested(int aTarget);
   void AssessmentPlatformsChanged(const QString& aSourcePlatform,
                                   const QString& aDestinationPlatform);
   void UiScaleChanged(int aPercent);

private:
   void Refresh();
   void EvaluateTask();
   void QueryCapability();
   void LoadNetworkPlan();
   void UnloadNetworkPlan();
   void SaveNetworkPlanRevision();
   void ValidateNetworkPlan();
   void EvaluateNetworkPlan();
   void GenerateNetworkPlanPackage();
   bool ApplyNetworkPlanEdits();
   void RefreshNetworkPlan();
   void LoadResourceDemands();
   void UnloadResourceDemands();
   void SaveResourceDemandRevision();
   void EvaluateResourceDemands();
   bool ApplyResourceDemandEdits();
   void RefreshResourceDemands();
   void RefreshPreacceptance(const PreacceptanceStatus& aStatus);
   void RefreshNodeSelectors(const nrm::FrameworkSnapshot& aSnapshot);
   void PublishAssessmentPlatforms();
   void RefreshTacticalScaleControl();
   void StartAcceptanceScenario();
   void RunAcceptanceAssessment();
   void RunAcceptanceCapability();
   void RunAcceptancePlan();
   void ShowAcceptanceTab(QWidget* aPagePtr);
   static bool SetComboValue(QComboBox* aComboPtr, const QString& aValue);
   static QString RuntimeStateText(nrm::RuntimeState aState);
   static void SetTableText(QTableWidget* aTablePtr, int aRow, int aColumn, const QString& aText);

   DataContainer& mData;
   QWidget*        mContentPtr;
   QLabel*         mScaleValuePtr;
   int             mUiScalePercent = 100;
   QLabel*        mVersionValuePtr;
   QLabel*        mStateValuePtr;
   QLabel*        mReportingValuePtr;
   QLabel*        mSimTimeValuePtr;
   QLabel*        mNetworkCountValuePtr;
   QLabel*        mEndpointCountValuePtr;
   QLabel*        mTransmittedValuePtr;
   QLabel*        mReceivedValuePtr;
   QLabel*        mHopValuePtr;
   QLabel*        mDiscardedValuePtr;
   QTableWidget*  mNetworkTablePtr;
   QTableWidget*  mMetricsSummaryTablePtr;
   QTableWidget*  mMetricsTablePtr;
   OperationalDetailTabs* mMetricsDetailTabsPtr;
   QTableWidget*  mEndpointTablePtr;
   QTableWidget*  mActiveLinkTablePtr;
   QTableWidget*  mLinkTablePtr;
   OperationalDetailTabs* mLinkDetailTabsPtr;
   QTableWidget*  mEnvironmentTablePtr;
   QTableWidget*  mNavigationTablePtr;
   QComboBox*      mSourceSelectorPtr;
   QComboBox*      mDestinationSelectorPtr;
   QComboBox*      mAllowedNetworkPtr;
   QDoubleSpinBox* mBandwidthKbpsPtr;
   QDoubleSpinBox* mMaximumDelayMsPtr;
   QDoubleSpinBox* mMinimumPdrPtr;
   QTextEdit*      mAssessmentResultPtr;
   QPushButton*    mSelectSourceOnMapPtr;
   QPushButton*    mSelectDestinationOnMapPtr;
   QLabel*         mTacticalSelectionStatusPtr;
   QComboBox*      mCapabilitySourceSelectorPtr;
   QComboBox*      mCapabilityDestinationSelectorPtr;
   QComboBox*      mCapabilityAllowedNetworkPtr;
   QDoubleSpinBox* mCapabilityBandwidthKbpsPtr;
   QDoubleSpinBox* mCapabilityMaximumDelayMsPtr;
   QDoubleSpinBox* mCapabilityMinimumPdrPtr;
   QTextEdit*      mCapabilityResultPtr;
   QLabel*         mPlanSummaryPtr;
   QLabel*         mPlanOperationPtr;
   QTableWidget*   mPlanOverviewTablePtr;
   QTableWidget*   mPlanAllocationTablePtr;
   QTableWidget*   mPlanDemandTablePtr;
   QTableWidget*   mPlanIssueTablePtr;
   QTableWidget*   mPlanEvaluationTablePtr;
   QTableWidget*   mPlanRecommendationTablePtr;
   QTabWidget*     mPlanDetailTabsPtr;
   QTabWidget*     mMainTabsPtr;
   AcceptanceDemoPanel* mAcceptanceDemoPanelPtr;
   QWidget*        mAssessmentPagePtr;
   QWidget*        mCapabilityPagePtr;
   QWidget*        mPlanPagePtr;
   QWidget*        mPreacceptancePagePtr;
   bool            mPlanDirty = false;
   QLabel*         mDemandSummaryPtr;
   QLabel*         mDemandOperationPtr;
   QTableWidget*   mDemandTablePtr;
   QTableWidget*   mDemandMatchTablePtr;
   QTableWidget*   mDemandGapTablePtr;
   QTableWidget*   mDemandRecommendationSummaryTablePtr;
   QTableWidget*   mDemandRecommendationTablePtr;
   DemandDetailTabs* mDemandDetailTabsPtr;
   bool            mDemandDirty = false;
   QLabel*         mPreacceptanceStatusPtr;
   QLabel*         mPreacceptanceTimePtr;
   QLabel*         mPreacceptanceChecksPtr;
   QLabel*         mPreacceptanceTestsPtr;
   QLabel*         mPreacceptanceScenariosPtr;
   QLabel*         mPreacceptanceSnapshotPtr;
   QLabel*         mPreacceptanceRevisionPtr;
   QLabel*         mPreacceptanceReportPtr;
   QLabel*         mPreacceptanceNoticePtr;
   PreacceptanceStatusMonitor* mPreacceptanceMonitorPtr;
};
} // namespace WkNrm

#endif
