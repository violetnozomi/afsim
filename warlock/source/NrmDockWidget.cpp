#include "NrmDockWidget.hpp"

#include <cmath>
#include <vector>

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "nrm/AssessmentEvaluator.hpp"
#include "nrm/NetworkTypeUtils.hpp"
#include "nrm/Version.hpp"

namespace
{
QColor NetworkColor(nrm::NetworkType aType)
{
   switch (aType)
   {
   case nrm::NetworkType::cLINK11:
      return QColor(46, 134, 222);
   case nrm::NetworkType::cLINK16:
      return QColor(220, 68, 55);
   case nrm::NetworkType::cSATCOM:
      return QColor(145, 83, 184);
   case nrm::NetworkType::cCDL:
      return QColor(39, 174, 96);
   case nrm::NetworkType::cUNKNOWN:
   default:
      return QColor(127, 140, 141);
   }
}

QString MetricText(const nrm::MetricValue<double>& aMetric, int aPrecision = 2)
{
   return aMetric.valid ? QString::number(aMetric.value, 'f', aPrecision) + " " + QString::fromStdString(aMetric.unit)
                        : QString::fromUtf8("—");
}

const nrm::WindowMetrics* FindWindow(const std::vector<nrm::WindowMetrics>& aWindows, double aWindowS)
{
   for (const nrm::WindowMetrics& window : aWindows)
   {
      if (std::abs(window.windowS - aWindowS) < 0.01)
      {
         return &window;
      }
   }
   return nullptr;
}

QTableWidget* CreateTable(const QStringList& aHeaders, QWidget* aParentPtr)
{
   QTableWidget* tablePtr = new QTableWidget(aParentPtr);
   tablePtr->setColumnCount(aHeaders.size());
   tablePtr->setHorizontalHeaderLabels(aHeaders);
   tablePtr->setEditTriggers(QAbstractItemView::NoEditTriggers);
   tablePtr->setSelectionBehavior(QAbstractItemView::SelectRows);
   tablePtr->setAlternatingRowColors(true);
   tablePtr->verticalHeader()->setVisible(false);
   tablePtr->horizontalHeader()->setStretchLastSection(true);
   tablePtr->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
   return tablePtr;
}
} // namespace

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
   , mDiscardedValuePtr(new QLabel(this))
   , mNetworkTablePtr(nullptr)
   , mMetricsTablePtr(nullptr)
   , mEndpointTablePtr(nullptr)
   , mLinkTablePtr(nullptr)
   , mSourceEditPtr(nullptr)
   , mDestinationEditPtr(nullptr)
   , mAllowedNetworkPtr(nullptr)
   , mBandwidthKbpsPtr(nullptr)
   , mMaximumDelayMsPtr(nullptr)
   , mMinimumPdrPtr(nullptr)
   , mAssessmentResultPtr(nullptr)
{
   QWidget* contentPtr = new QWidget(this);
   QVBoxLayout* rootLayoutPtr = new QVBoxLayout(contentPtr);
   QFormLayout* statusLayoutPtr = new QFormLayout();
   statusLayoutPtr->addRow("Version", mVersionValuePtr);
   statusLayoutPtr->addRow("Runtime state", mStateValuePtr);
   statusLayoutPtr->addRow("Simulation time", mSimTimeValuePtr);
   statusLayoutPtr->addRow("Networks", mNetworkCountValuePtr);
   statusLayoutPtr->addRow("Endpoints", mEndpointCountValuePtr);
   statusLayoutPtr->addRow("Transmitted", mTransmittedValuePtr);
   statusLayoutPtr->addRow("Received", mReceivedValuePtr);
   statusLayoutPtr->addRow("Message hops", mHopValuePtr);
   statusLayoutPtr->addRow("Discarded / route failed", mDiscardedValuePtr);
   rootLayoutPtr->addLayout(statusLayoutPtr);

   QTabWidget* tabsPtr = new QTabWidget(contentPtr);
   mNetworkTablePtr = CreateTable(
      {"Type", "Network", "Model", "Members", "Online", "Links", "Tx", "Rx", "Dropped"}, tabsPtr);
   mMetricsTablePtr = CreateTable(
      {"Type", "Network", "Window", "Throughput", "PDR", "Online ratio", "Queue delay", "Transport delay"},
      tabsPtr);
   mEndpointTablePtr =
      CreateTable({"Type", "Platform", "Comm", "Address", "State", "Latitude", "Longitude", "Altitude"}, tabsPtr);
   mLinkTablePtr = CreateTable(
      {"Type",
       "Source",
       "Destination",
       "State",
       "Distance",
       "Bandwidth",
       "10 s throughput",
       "Utilization",
       "RSSI",
       "SNR",
       "BER"},
      tabsPtr);
   QWidget* assessmentPagePtr = new QWidget(tabsPtr);
   QVBoxLayout* assessmentLayoutPtr = new QVBoxLayout(assessmentPagePtr);
   QFormLayout* taskFormPtr = new QFormLayout();
   mSourceEditPtr = new QLineEdit("l11_control", assessmentPagePtr);
   mDestinationEditPtr = new QLineEdit("l11_member", assessmentPagePtr);
   mAllowedNetworkPtr = new QComboBox(assessmentPagePtr);
   mAllowedNetworkPtr->addItems({"ALL", "LINK11", "LINK16", "SATCOM", "CDL"});
   mBandwidthKbpsPtr = new QDoubleSpinBox(assessmentPagePtr);
   mBandwidthKbpsPtr->setRange(0.0, 100000000.0);
   mBandwidthKbpsPtr->setDecimals(3);
   mBandwidthKbpsPtr->setSuffix(" kbit/s");
   mMaximumDelayMsPtr = new QDoubleSpinBox(assessmentPagePtr);
   mMaximumDelayMsPtr->setRange(0.0, 10000000.0);
   mMaximumDelayMsPtr->setValue(1000.0);
   mMaximumDelayMsPtr->setSuffix(" ms");
   mMinimumPdrPtr = new QDoubleSpinBox(assessmentPagePtr);
   mMinimumPdrPtr->setRange(0.0, 100.0);
   mMinimumPdrPtr->setValue(90.0);
   mMinimumPdrPtr->setSuffix(" %");
   taskFormPtr->addRow("Source platform", mSourceEditPtr);
   taskFormPtr->addRow("Destination platform", mDestinationEditPtr);
   taskFormPtr->addRow("Allowed network", mAllowedNetworkPtr);
   taskFormPtr->addRow("Required bandwidth", mBandwidthKbpsPtr);
   taskFormPtr->addRow("Maximum delay", mMaximumDelayMsPtr);
   taskFormPtr->addRow("Minimum PDR", mMinimumPdrPtr);
   assessmentLayoutPtr->addLayout(taskFormPtr);
   QPushButton* evaluateButtonPtr = new QPushButton("Evaluate current graph", assessmentPagePtr);
   assessmentLayoutPtr->addWidget(evaluateButtonPtr);
   mAssessmentResultPtr = new QTextEdit(assessmentPagePtr);
   mAssessmentResultPtr->setReadOnly(true);
   mAssessmentResultPtr->setPlainText("Enter a task and evaluate the current enabled graph.");
   assessmentLayoutPtr->addWidget(mAssessmentResultPtr);
   connect(evaluateButtonPtr, &QPushButton::clicked, this, &DockWidget::EvaluateTask);

   tabsPtr->addTab(mNetworkTablePtr, "Four-network overview");
   tabsPtr->addTab(mMetricsTablePtr, "Window metrics");
   tabsPtr->addTab(mEndpointTablePtr, "Members");
   tabsPtr->addTab(mLinkTablePtr, "Links");
   tabsPtr->addTab(assessmentPagePtr, "Task assessment");
   rootLayoutPtr->addWidget(tabsPtr);

   setWidget(contentPtr);
   resize(920, 620);

   connect(&mData, &DataContainer::SnapshotChanged, this, &DockWidget::Refresh);
   Refresh();
}

void WkNrm::DockWidget::EvaluateTask()
{
   nrm::AssessmentTask task;
   task.taskId              = "GUI-" + std::to_string(mData.GetSnapshot().snapshotVersion);
   task.sourcePlatform      = mSourceEditPtr->text().trimmed().toStdString();
   task.destinationPlatform = mDestinationEditPtr->text().trimmed().toStdString();
   task.requiredBandwidthBps = mBandwidthKbpsPtr->value() * 1000.0;
   task.maximumDelayMs       = mMaximumDelayMsPtr->value();
   task.minimumPdrPercent    = mMinimumPdrPtr->value();
   const QString allowedNetwork = mAllowedNetworkPtr->currentText();
   if (allowedNetwork == "LINK11")
   {
      task.allowedNetworks.push_back(nrm::NetworkType::cLINK11);
   }
   else if (allowedNetwork == "LINK16")
   {
      task.allowedNetworks.push_back(nrm::NetworkType::cLINK16);
   }
   else if (allowedNetwork == "SATCOM")
   {
      task.allowedNetworks.push_back(nrm::NetworkType::cSATCOM);
   }
   else if (allowedNetwork == "CDL")
   {
      task.allowedNetworks.push_back(nrm::NetworkType::cCDL);
   }

   const nrm::AssessmentResult result =
      nrm::AssessmentEvaluator().Evaluate(mData.GetSnapshot(), task);
   mData.StoreAssessment(result);
   QStringList route;
   for (const std::string& platform : result.primaryRoute)
   {
      route.push_back(QString::fromStdString(platform));
   }
   QStringList reasons;
   for (nrm::AssessmentReason reason : result.reasons)
   {
      reasons.push_back(nrm::ToString(reason));
   }
   QStringList recommendations;
   for (const std::string& recommendation : result.recommendations)
   {
      recommendations.push_back(QString::fromStdString(recommendation));
   }

   QString text;
   text += QString("Snapshot: %1 @ %2 s\n")
              .arg(result.snapshotVersion)
              .arg(result.simTime, 0, 'f', 3);
   text += QString("Reachable: %1\nCan establish now: %2\nCan complete: %3\nStable: %4\n")
              .arg(result.reachable ? "YES" : "NO")
              .arg(result.canEstablish ? "YES" : "NO")
              .arg(result.canComplete ? "YES" : "NO")
              .arg(result.stable ? "YES" : "NO");
   text += "Primary route: " + (route.isEmpty() ? QString::fromUtf8("—") : route.join(" → ")) + "\n";
   text += "Predicted delay: " + MetricText(result.predictedDelayMs, 3) + "\n";
   text += "Estimated PDR: " + MetricText(result.estimatedPdrPercent, 2) + "\n";
   text += "Bottleneck bandwidth: " + MetricText(result.bottleneckBandwidthBps, 1) + "\n";
   text += "Bandwidth margin: " + MetricText(result.bandwidthMarginBps, 1) + "\n";
   text += "Delay margin: " + MetricText(result.delayMarginMs, 3) + "\n";
   text += "Reliability margin: " + MetricText(result.reliabilityMarginPercent, 2) + "\n";
   text += "Reason codes: " + (reasons.isEmpty() ? QString("NONE") : reasons.join(", ")) + "\n";
   text += "Recommendation: " +
           (recommendations.isEmpty() ? QString::fromUtf8("—") : recommendations.join("\n- "));
   mAssessmentResultPtr->setPlainText(text);
}

void WkNrm::DockWidget::Refresh()
{
   const nrm::FrameworkSnapshot& snapshot = mData.GetSnapshot();
   mVersionValuePtr->setText(nrm::cVERSION);
   mStateValuePtr->setText(RuntimeStateText(snapshot.runtimeState));
   mSimTimeValuePtr->setText(QString::number(snapshot.simTime, 'f', 2) + " s");
   mNetworkCountValuePtr->setText(QString::number(snapshot.networks.size()));
   mEndpointCountValuePtr->setText(QString::number(snapshot.endpoints.size()));
   mTransmittedValuePtr->setText(QString::number(snapshot.messages.transmitted));
   mReceivedValuePtr->setText(QString::number(snapshot.messages.received));
   mHopValuePtr->setText(QString::number(snapshot.messages.hops));
   mDiscardedValuePtr->setText(QString("%1 / %2")
                                 .arg(snapshot.messages.discarded)
                                 .arg(snapshot.messages.routingFailed));

   mNetworkTablePtr->setRowCount(static_cast<int>(snapshot.networks.size()));
   for (std::size_t index = 0; index < snapshot.networks.size(); ++index)
   {
      const nrm::NetworkSnapshot& network = snapshot.networks[index];
      const int row = static_cast<int>(index);
      SetTableText(mNetworkTablePtr, row, 0, nrm::ToString(network.networkType));
      mNetworkTablePtr->item(row, 0)->setForeground(QBrush(NetworkColor(network.networkType)));
      SetTableText(mNetworkTablePtr, row, 1, QString::fromStdString(network.networkName));
      SetTableText(mNetworkTablePtr, row, 2, QString::fromStdString(network.modelType));
      SetTableText(mNetworkTablePtr, row, 3, QString::number(network.endpointCount));
      SetTableText(mNetworkTablePtr, row, 4, QString::number(network.onlineCount));
      SetTableText(mNetworkTablePtr, row, 5, QString::number(network.activeLinks));
      SetTableText(mNetworkTablePtr, row, 6, QString::number(network.messages.transmitted));
      SetTableText(mNetworkTablePtr, row, 7, QString::number(network.messages.received));
      SetTableText(mNetworkTablePtr,
                   row,
                   8,
                   QString::number(network.messages.discarded + network.messages.routingFailed));
   }

   std::size_t metricsRowCount = 0;
   for (const nrm::NetworkSnapshot& network : snapshot.networks)
   {
      metricsRowCount += network.windows.size();
   }
   mMetricsTablePtr->setRowCount(static_cast<int>(metricsRowCount));
   int metricsRow = 0;
   for (const nrm::NetworkSnapshot& network : snapshot.networks)
   {
      for (const nrm::WindowMetrics& window : network.windows)
      {
         SetTableText(mMetricsTablePtr, metricsRow, 0, nrm::ToString(network.networkType));
         mMetricsTablePtr->item(metricsRow, 0)->setForeground(QBrush(NetworkColor(network.networkType)));
         SetTableText(mMetricsTablePtr, metricsRow, 1, QString::fromStdString(network.networkName));
         SetTableText(mMetricsTablePtr, metricsRow, 2, QString::number(window.windowS, 'f', 0) + " s");
         SetTableText(mMetricsTablePtr, metricsRow, 3, MetricText(window.throughputBps, 1));
         SetTableText(mMetricsTablePtr, metricsRow, 4, MetricText(window.pdrPercent, 1));
         SetTableText(mMetricsTablePtr, metricsRow, 5, MetricText(window.onlineRatioPercent, 1));
         SetTableText(mMetricsTablePtr, metricsRow, 6, MetricText(window.averageQueueDelayMs, 3));
         SetTableText(mMetricsTablePtr, metricsRow, 7, MetricText(window.averageTransportDelayMs, 3));
         ++metricsRow;
      }
   }

   mEndpointTablePtr->setRowCount(static_cast<int>(snapshot.endpoints.size()));
   for (std::size_t index = 0; index < snapshot.endpoints.size(); ++index)
   {
      const nrm::EndpointSnapshot& endpoint = snapshot.endpoints[index];
      const int row = static_cast<int>(index);
      SetTableText(mEndpointTablePtr, row, 0, nrm::ToString(endpoint.networkType));
      mEndpointTablePtr->item(row, 0)->setForeground(QBrush(NetworkColor(endpoint.networkType)));
      SetTableText(mEndpointTablePtr, row, 1, QString::fromStdString(endpoint.platformName));
      SetTableText(mEndpointTablePtr, row, 2, QString::fromStdString(endpoint.commName));
      SetTableText(mEndpointTablePtr, row, 3, QString::fromStdString(endpoint.address));
      SetTableText(mEndpointTablePtr, row, 4, nrm::ToString(endpoint.state));
      SetTableText(mEndpointTablePtr, row, 5, MetricText(endpoint.latitudeDeg, 5));
      SetTableText(mEndpointTablePtr, row, 6, MetricText(endpoint.longitudeDeg, 5));
      SetTableText(mEndpointTablePtr, row, 7, MetricText(endpoint.altitudeM, 1));
   }

   mLinkTablePtr->setRowCount(static_cast<int>(snapshot.links.size()));
   for (std::size_t index = 0; index < snapshot.links.size(); ++index)
   {
      const nrm::LinkSnapshot& link = snapshot.links[index];
      const int row = static_cast<int>(index);
      SetTableText(mLinkTablePtr, row, 0, nrm::ToString(link.networkType));
      mLinkTablePtr->item(row, 0)->setForeground(QBrush(NetworkColor(link.networkType)));
      SetTableText(mLinkTablePtr, row, 1, QString::fromStdString(link.sourcePlatform));
      SetTableText(mLinkTablePtr, row, 2, QString::fromStdString(link.destinationPlatform));
      SetTableText(mLinkTablePtr, row, 3, nrm::ToString(link.state));
      SetTableText(mLinkTablePtr, row, 4, MetricText(link.distanceM, 1));
      SetTableText(mLinkTablePtr, row, 5, MetricText(link.bandwidthBps, 1));
      const nrm::WindowMetrics* window10s = FindWindow(link.windows, 10.0);
      SetTableText(mLinkTablePtr,
                   row,
                   6,
                   window10s == nullptr ? QString::fromUtf8("—") : MetricText(window10s->throughputBps, 1));
      SetTableText(mLinkTablePtr,
                   row,
                   7,
                   window10s == nullptr ? QString::fromUtf8("—") : MetricText(window10s->utilizationPercent, 1));
      SetTableText(mLinkTablePtr, row, 8, MetricText(link.rssiDbm));
      SetTableText(mLinkTablePtr, row, 9, MetricText(link.snrDb));
      SetTableText(mLinkTablePtr, row, 10, MetricText(link.ber, 6));
   }
}

void WkNrm::DockWidget::SetTableText(QTableWidget* aTablePtr, int aRow, int aColumn, const QString& aText)
{
   QTableWidgetItem* itemPtr = aTablePtr->item(aRow, aColumn);
   if (itemPtr == nullptr)
   {
      itemPtr = new QTableWidgetItem();
      aTablePtr->setItem(aRow, aColumn, itemPtr);
   }
   itemPtr->setText(aText);
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
