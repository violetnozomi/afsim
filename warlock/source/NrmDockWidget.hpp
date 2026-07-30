#ifndef NRM_DOCK_WIDGET_HPP
#define NRM_DOCK_WIDGET_HPP

#include <QDockWidget>

class QLabel;

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

   DataContainer& mData;
   QLabel*        mVersionValuePtr;
   QLabel*        mStateValuePtr;
   QLabel*        mSimTimeValuePtr;
   QLabel*        mNetworkCountValuePtr;
   QLabel*        mEndpointCountValuePtr;
   QLabel*        mTransmittedValuePtr;
   QLabel*        mReceivedValuePtr;
   QLabel*        mHopValuePtr;
};
} // namespace WkNrm

#endif

