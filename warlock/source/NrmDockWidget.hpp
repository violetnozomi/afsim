#ifndef NRM_DOCK_WIDGET_HPP
#define NRM_DOCK_WIDGET_HPP

#include <QDockWidget>

class QLabel;
class QComboBox;
class QDoubleSpinBox;
class QTableWidget;
class QTextEdit;

#include "NrmDataContainer.hpp"

namespace WkNrm
{
class DockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit DockWidget(DataContainer& aData, QWidget* aParentPtr = nullptr);

private:
   void Refresh();
   void EvaluateTask();
   void QueryCapability();
   void RefreshNodeSelectors(const nrm::FrameworkSnapshot& aSnapshot);
   static QString RuntimeStateText(nrm::RuntimeState aState);
   static void SetTableText(QTableWidget* aTablePtr, int aRow, int aColumn, const QString& aText);

   DataContainer& mData;
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
   QTableWidget*  mMetricsTablePtr;
   QTableWidget*  mEndpointTablePtr;
   QTableWidget*  mLinkTablePtr;
   QComboBox*      mSourceSelectorPtr;
   QComboBox*      mDestinationSelectorPtr;
   QComboBox*      mAllowedNetworkPtr;
   QDoubleSpinBox* mBandwidthKbpsPtr;
   QDoubleSpinBox* mMaximumDelayMsPtr;
   QDoubleSpinBox* mMinimumPdrPtr;
   QTextEdit*      mAssessmentResultPtr;
   QComboBox*      mCapabilitySourceSelectorPtr;
   QComboBox*      mCapabilityDestinationSelectorPtr;
   QComboBox*      mCapabilityAllowedNetworkPtr;
   QDoubleSpinBox* mCapabilityBandwidthKbpsPtr;
   QDoubleSpinBox* mCapabilityMaximumDelayMsPtr;
   QDoubleSpinBox* mCapabilityMinimumPdrPtr;
   QTextEdit*      mCapabilityResultPtr;
};
} // namespace WkNrm

#endif
