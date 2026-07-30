#ifndef NRM_DOCK_WIDGET_HPP
#define NRM_DOCK_WIDGET_HPP

#include <QDockWidget>

class QLabel;
class QTableWidget;

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
   static QString RuntimeStateText(nrm::RuntimeState aState);
   static void SetTableText(QTableWidget* aTablePtr, int aRow, int aColumn, const QString& aText);

   DataContainer& mData;
   QLabel*        mVersionValuePtr;
   QLabel*        mStateValuePtr;
   QLabel*        mSimTimeValuePtr;
   QLabel*        mNetworkCountValuePtr;
   QLabel*        mEndpointCountValuePtr;
   QLabel*        mTransmittedValuePtr;
   QLabel*        mReceivedValuePtr;
   QLabel*        mHopValuePtr;
   QLabel*        mDiscardedValuePtr;
   QTableWidget*  mNetworkTablePtr;
   QTableWidget*  mEndpointTablePtr;
   QTableWidget*  mLinkTablePtr;
};
} // namespace WkNrm

#endif
