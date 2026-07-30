#include "NrmDockWidget.hpp"

#include <QFormLayout>
#include <QLabel>
#include <QWidget>

#include "nrm/Version.hpp"

WkNrm::DockWidget::DockWidget(DataContainer& aData, QWidget* aParentPtr)
   : QDockWidget("Network Resource Manager", aParentPtr)
   , mData(aData)
   , mVersionValuePtr(new QLabel(this))
   , mStateValuePtr(new QLabel(this))
   , mSimTimeValuePtr(new QLabel(this))
   , mNetworkCountValuePtr(new QLabel(this))
   , mEndpointCountValuePtr(new QLabel(this))
   , mTransmittedValuePtr(new QLabel(this))
   , mReceivedValuePtr(new QLabel(this))
   , mHopValuePtr(new QLabel(this))
{
   QWidget*     contentPtr = new QWidget(this);
   QFormLayout* layoutPtr  = new QFormLayout(contentPtr);
   layoutPtr->addRow("Version", mVersionValuePtr);
   layoutPtr->addRow("Runtime state", mStateValuePtr);
   layoutPtr->addRow("Simulation time", mSimTimeValuePtr);
   layoutPtr->addRow("Networks", mNetworkCountValuePtr);
   layoutPtr->addRow("Endpoints", mEndpointCountValuePtr);
   layoutPtr->addRow("Transmitted", mTransmittedValuePtr);
   layoutPtr->addRow("Received", mReceivedValuePtr);
   layoutPtr->addRow("Message hops", mHopValuePtr);
   setWidget(contentPtr);

   connect(&mData, &DataContainer::SnapshotChanged, this, &DockWidget::Refresh);
   Refresh();
}

void WkNrm::DockWidget::Refresh()
{
   const nrm::FrameworkSnapshot& snapshot = mData.GetSnapshot();
   mVersionValuePtr->setText(nrm::cVERSION);
   mStateValuePtr->setText(RuntimeStateText(snapshot.runtimeState));
   mSimTimeValuePtr->setText(QString::number(snapshot.simTime, 'f', 2) + " s");
   mNetworkCountValuePtr->setText(QString::number(snapshot.networkCount));
   mEndpointCountValuePtr->setText(QString::number(snapshot.endpointCount));
   mTransmittedValuePtr->setText(QString::number(snapshot.transmitted));
   mReceivedValuePtr->setText(QString::number(snapshot.received));
   mHopValuePtr->setText(QString::number(snapshot.hops));
}

QString WkNrm::DockWidget::RuntimeStateText(nrm::RuntimeState aState)
{
   switch (aState)
   {
   case nrm::RuntimeState::cINITIALIZING:
      return "Initializing";
   case nrm::RuntimeState::cRUNNING:
      return "Running";
   case nrm::RuntimeState::cCOMPLETE:
      return "Complete";
   case nrm::RuntimeState::cIDLE:
   default:
      return "Idle";
   }
}

