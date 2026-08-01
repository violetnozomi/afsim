#include "NrmDockWidget.hpp"

#include <cmath>
#include <limits>
#include <map>
#include <vector>

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateTime>
#include <QFileDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QStyle>
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

QString CapabilityMetricText(const nrm::MetricValue<double>& aMetric, int aPrecision = 2)
{
   const QString metadata =
      QString("source=%1, confidence=%2, reason=%3")
         .arg(nrm::ToString(aMetric.origin),
              nrm::ToString(aMetric.confidence),
              nrm::ToString(aMetric.reason));
   if (!aMetric.valid)
   {
      return "INVALID [" + metadata + "]";
   }
   return QString::number(aMetric.value, 'f', aPrecision) + " " +
          QString::fromStdString(aMetric.unit) + " [" + metadata + "]";
}

void AddAllowedNetwork(const QString& aValue, std::vector<nrm::NetworkType>& aNetworks)
{
   if (aValue == "LINK11") aNetworks.push_back(nrm::NetworkType::cLINK11);
   else if (aValue == "LINK16") aNetworks.push_back(nrm::NetworkType::cLINK16);
   else if (aValue == "SATCOM") aNetworks.push_back(nrm::NetworkType::cSATCOM);
   else if (aValue == "CDL") aNetworks.push_back(nrm::NetworkType::cCDL);
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

QTableWidget* CreateEditableTable(const QStringList& aHeaders, QWidget* aParentPtr)
{
   QTableWidget* tablePtr = CreateTable(aHeaders, aParentPtr);
   tablePtr->setEditTriggers(QAbstractItemView::DoubleClicked |
                             QAbstractItemView::EditKeyPressed |
                             QAbstractItemView::SelectedClicked);
   return tablePtr;
}

nrm::NetworkType ParseNetworkType(const QString& aValue)
{
   if (aValue == "LINK11") return nrm::NetworkType::cLINK11;
   if (aValue == "LINK16") return nrm::NetworkType::cLINK16;
   if (aValue == "SATCOM") return nrm::NetworkType::cSATCOM;
   if (aValue == "CDL") return nrm::NetworkType::cCDL;
   return nrm::NetworkType::cUNKNOWN;
}

std::vector<std::string> SplitValues(const QString& aValue)
{
   std::vector<std::string> output;
   for (const QString& item : aValue.split(',', QString::SkipEmptyParts))
   {
      const QString trimmed = item.trimmed();
      if (!trimmed.isEmpty()) output.push_back(trimmed.toStdString());
   }
   return output;
}

QString JoinValues(const std::vector<std::string>& aValues)
{
   QStringList output;
   for (const std::string& value : aValues)
      output.push_back(QString::fromStdString(value));
   return output.join(", ");
}

QString JoinNetworks(const std::vector<nrm::NetworkType>& aValues)
{
   QStringList output;
   for (nrm::NetworkType value : aValues) output.push_back(nrm::ToString(value));
   return output.join(", ");
}

QString CellText(const QTableWidget* aTablePtr, int aRow, int aColumn)
{
   const QTableWidgetItem* itemPtr = aTablePtr->item(aRow, aColumn);
   return itemPtr == nullptr ? QString() : itemPtr->text().trimmed();
}
} // namespace

WkNrm::DockWidget::DockWidget(DataContainer& aData, QWidget* aParentPtr)
   : QDockWidget("Network Resource Manager", aParentPtr)
   , mData(aData)
   , mVersionValuePtr(new QLabel(this))
   , mStateValuePtr(new QLabel(this))
   , mReportingValuePtr(new QLabel(this))
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
   , mSourceSelectorPtr(nullptr)
   , mDestinationSelectorPtr(nullptr)
   , mAllowedNetworkPtr(nullptr)
   , mBandwidthKbpsPtr(nullptr)
   , mMaximumDelayMsPtr(nullptr)
   , mMinimumPdrPtr(nullptr)
   , mAssessmentResultPtr(nullptr)
   , mCapabilitySourceSelectorPtr(nullptr)
   , mCapabilityDestinationSelectorPtr(nullptr)
   , mCapabilityAllowedNetworkPtr(nullptr)
   , mCapabilityBandwidthKbpsPtr(nullptr)
   , mCapabilityMaximumDelayMsPtr(nullptr)
   , mCapabilityMinimumPdrPtr(nullptr)
   , mCapabilityResultPtr(nullptr)
   , mPlanSummaryPtr(new QLabel(this))
   , mPlanOperationPtr(new QLabel(this))
   , mPlanAllocationTablePtr(nullptr)
   , mPlanDemandTablePtr(nullptr)
   , mPlanIssueTablePtr(nullptr)
   , mPlanEvaluationTablePtr(nullptr)
   , mDemandSummaryPtr(new QLabel(this))
   , mDemandOperationPtr(new QLabel(this))
   , mDemandTablePtr(nullptr)
   , mDemandMatchTablePtr(nullptr)
   , mDemandGapTablePtr(nullptr)
   , mDemandRecommendationTablePtr(nullptr)
{
   QWidget* contentPtr = new QWidget(this);
   QVBoxLayout* rootLayoutPtr = new QVBoxLayout(contentPtr);
   QFormLayout* statusLayoutPtr = new QFormLayout();
   statusLayoutPtr->addRow("Version", mVersionValuePtr);
   statusLayoutPtr->addRow("Runtime state", mStateValuePtr);
   statusLayoutPtr->addRow("Reporting", mReportingValuePtr);
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
   mSourceSelectorPtr = new QComboBox(assessmentPagePtr);
   mDestinationSelectorPtr = new QComboBox(assessmentPagePtr);
   mSourceSelectorPtr->setMinimumContentsLength(24);
   mDestinationSelectorPtr->setMinimumContentsLength(24);
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
   taskFormPtr->addRow("Source platform", mSourceSelectorPtr);
   taskFormPtr->addRow("Destination platform", mDestinationSelectorPtr);
   taskFormPtr->addRow("Allowed network", mAllowedNetworkPtr);
   taskFormPtr->addRow("Required bandwidth", mBandwidthKbpsPtr);
   taskFormPtr->addRow("Maximum delay", mMaximumDelayMsPtr);
   taskFormPtr->addRow("Minimum PDR", mMinimumPdrPtr);
   assessmentLayoutPtr->addLayout(taskFormPtr);
   QPushButton* evaluateButtonPtr =
      new QPushButton("Evaluate current / candidate graph", assessmentPagePtr);
   assessmentLayoutPtr->addWidget(evaluateButtonPtr);
   mAssessmentResultPtr = new QTextEdit(assessmentPagePtr);
   mAssessmentResultPtr->setReadOnly(true);
   mAssessmentResultPtr->setPlainText("Enter a task and evaluate the current enabled graph.");
   assessmentLayoutPtr->addWidget(mAssessmentResultPtr);
   connect(evaluateButtonPtr, &QPushButton::clicked, this, &DockWidget::EvaluateTask);

   QWidget* capabilityPagePtr = new QWidget(tabsPtr);
   QVBoxLayout* capabilityLayoutPtr = new QVBoxLayout(capabilityPagePtr);
   QFormLayout* capabilityFormPtr = new QFormLayout();
   mCapabilitySourceSelectorPtr = new QComboBox(capabilityPagePtr);
   mCapabilityDestinationSelectorPtr = new QComboBox(capabilityPagePtr);
   mCapabilitySourceSelectorPtr->setMinimumContentsLength(24);
   mCapabilityDestinationSelectorPtr->setMinimumContentsLength(24);
   mCapabilityAllowedNetworkPtr = new QComboBox(capabilityPagePtr);
   mCapabilityAllowedNetworkPtr->addItems({"ALL", "LINK11", "LINK16", "SATCOM", "CDL"});
   mCapabilityBandwidthKbpsPtr = new QDoubleSpinBox(capabilityPagePtr);
   mCapabilityBandwidthKbpsPtr->setRange(0.0, 100000000.0);
   mCapabilityBandwidthKbpsPtr->setDecimals(3);
   mCapabilityBandwidthKbpsPtr->setSuffix(" kbit/s");
   mCapabilityMaximumDelayMsPtr = new QDoubleSpinBox(capabilityPagePtr);
   mCapabilityMaximumDelayMsPtr->setRange(0.0, 10000000.0);
   mCapabilityMaximumDelayMsPtr->setSuffix(" ms");
   mCapabilityMinimumPdrPtr = new QDoubleSpinBox(capabilityPagePtr);
   mCapabilityMinimumPdrPtr->setRange(0.0, 100.0);
   mCapabilityMinimumPdrPtr->setSuffix(" %");
   capabilityFormPtr->addRow("Source platform", mCapabilitySourceSelectorPtr);
   capabilityFormPtr->addRow("Destination platform", mCapabilityDestinationSelectorPtr);
   capabilityFormPtr->addRow("Allowed network", mCapabilityAllowedNetworkPtr);
   capabilityFormPtr->addRow("Required bandwidth", mCapabilityBandwidthKbpsPtr);
   capabilityFormPtr->addRow("Maximum delay (0 = none)", mCapabilityMaximumDelayMsPtr);
   capabilityFormPtr->addRow("Minimum PDR", mCapabilityMinimumPdrPtr);
   capabilityLayoutPtr->addLayout(capabilityFormPtr);
   QPushButton* capabilityButtonPtr = new QPushButton("Query communication capability",
                                                      capabilityPagePtr);
   capabilityButtonPtr->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
   capabilityLayoutPtr->addWidget(capabilityButtonPtr);
   mCapabilityResultPtr = new QTextEdit(capabilityPagePtr);
   mCapabilityResultPtr->setReadOnly(true);
   mCapabilityResultPtr->setPlainText("Select a task path and query the current snapshot.");
   capabilityLayoutPtr->addWidget(mCapabilityResultPtr);
   connect(capabilityButtonPtr, &QPushButton::clicked, this, &DockWidget::QueryCapability);

   QWidget* planPagePtr = new QWidget(tabsPtr);
   QVBoxLayout* planLayoutPtr = new QVBoxLayout(planPagePtr);
   mPlanSummaryPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   mPlanOperationPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   planLayoutPtr->addWidget(mPlanSummaryPtr);
   planLayoutPtr->addWidget(mPlanOperationPtr);

   QHBoxLayout* planFileActionsPtr = new QHBoxLayout();
   QPushButton* loadPlanButtonPtr = new QPushButton(QString::fromUtf8("加载"), planPagePtr);
   loadPlanButtonPtr->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
   QPushButton* unloadPlanButtonPtr = new QPushButton(QString::fromUtf8("卸载"), planPagePtr);
   unloadPlanButtonPtr->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
   QPushButton* savePlanButtonPtr =
      new QPushButton(QString::fromUtf8("保存新修订"), planPagePtr);
   savePlanButtonPtr->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
   planFileActionsPtr->addWidget(loadPlanButtonPtr);
   planFileActionsPtr->addWidget(unloadPlanButtonPtr);
   planFileActionsPtr->addWidget(savePlanButtonPtr);
   planFileActionsPtr->addStretch();
   planLayoutPtr->addLayout(planFileActionsPtr);

   mPlanAllocationTablePtr = CreateEditableTable(
      {"Allocation", "Network", "Type", "Profile", "Frequency Hz", "Channel",
       "Subnet", "Slots", "Members", "Route policy", "Enabled"}, planPagePtr);
   mPlanAllocationTablePtr->setMinimumHeight(135);
   planLayoutPtr->addWidget(mPlanAllocationTablePtr);
   mPlanDemandTablePtr = CreateEditableTable(
      {"Demand", "Business", "Source", "Destination", "Payload bits",
       "Bandwidth bit/s", "Max delay ms", "Min PDR %", "Allowed networks"},
      planPagePtr);
   mPlanDemandTablePtr->setMinimumHeight(120);
   planLayoutPtr->addWidget(mPlanDemandTablePtr);

   QHBoxLayout* planServiceActionsPtr = new QHBoxLayout();
   QPushButton* validatePlanButtonPtr =
      new QPushButton(QString::fromUtf8("校验"), planPagePtr);
   validatePlanButtonPtr->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
   QPushButton* evaluatePlanButtonPtr =
      new QPushButton(QString::fromUtf8("只读推演"), planPagePtr);
   evaluatePlanButtonPtr->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
   QPushButton* packagePlanButtonPtr =
      new QPushButton(QString::fromUtf8("生成分发包"), planPagePtr);
   packagePlanButtonPtr->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
   planServiceActionsPtr->addWidget(validatePlanButtonPtr);
   planServiceActionsPtr->addWidget(evaluatePlanButtonPtr);
   planServiceActionsPtr->addWidget(packagePlanButtonPtr);
   planServiceActionsPtr->addStretch();
   planLayoutPtr->addLayout(planServiceActionsPtr);

   mPlanIssueTablePtr = CreateTable(
      {"Severity", "Reason", "Field", "Record", "Description"}, planPagePtr);
   mPlanIssueTablePtr->setMinimumHeight(105);
   planLayoutPtr->addWidget(mPlanIssueTablePtr);
   mPlanEvaluationTablePtr = CreateTable(
      {"Demand", "Status", "Path source", "Rate", "Delay", "Packet loss", "Reasons"},
      planPagePtr);
   mPlanEvaluationTablePtr->setMinimumHeight(105);
   planLayoutPtr->addWidget(mPlanEvaluationTablePtr);

   connect(loadPlanButtonPtr, &QPushButton::clicked, this, &DockWidget::LoadNetworkPlan);
   connect(unloadPlanButtonPtr, &QPushButton::clicked, this, &DockWidget::UnloadNetworkPlan);
   connect(savePlanButtonPtr, &QPushButton::clicked,
           this, &DockWidget::SaveNetworkPlanRevision);
   connect(validatePlanButtonPtr, &QPushButton::clicked,
           this, &DockWidget::ValidateNetworkPlan);
   connect(evaluatePlanButtonPtr, &QPushButton::clicked,
           this, &DockWidget::EvaluateNetworkPlan);
   connect(packagePlanButtonPtr, &QPushButton::clicked,
           this, &DockWidget::GenerateNetworkPlanPackage);
   connect(mPlanAllocationTablePtr, &QTableWidget::itemChanged, this,
           [this](QTableWidgetItem*)
           {
              mPlanDirty = true;
              mPlanOperationPtr->setText(
                 QString::fromUtf8("DRAFT：表格编辑尚未写入规划仓库"));
           });
   connect(mPlanDemandTablePtr, &QTableWidget::itemChanged, this,
           [this](QTableWidgetItem*)
           {
              mPlanDirty = true;
              mPlanOperationPtr->setText(
                 QString::fromUtf8("DRAFT：表格编辑尚未写入规划仓库"));
           });

   QWidget* demandPagePtr = new QWidget(tabsPtr);
   QVBoxLayout* demandLayoutPtr = new QVBoxLayout(demandPagePtr);
   mDemandSummaryPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   mDemandOperationPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   demandLayoutPtr->addWidget(mDemandSummaryPtr);
   demandLayoutPtr->addWidget(mDemandOperationPtr);

   QHBoxLayout* demandActionsPtr = new QHBoxLayout();
   QPushButton* loadDemandButtonPtr =
      new QPushButton(QString::fromUtf8("加载"), demandPagePtr);
   loadDemandButtonPtr->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
   QPushButton* unloadDemandButtonPtr =
      new QPushButton(QString::fromUtf8("卸载"), demandPagePtr);
   unloadDemandButtonPtr->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
   QPushButton* saveDemandButtonPtr =
      new QPushButton(QString::fromUtf8("保存新修订"), demandPagePtr);
   saveDemandButtonPtr->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
   QPushButton* evaluateDemandButtonPtr =
      new QPushButton(QString::fromUtf8("执行匹配"), demandPagePtr);
   evaluateDemandButtonPtr->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
   demandActionsPtr->addWidget(loadDemandButtonPtr);
   demandActionsPtr->addWidget(unloadDemandButtonPtr);
   demandActionsPtr->addWidget(saveDemandButtonPtr);
   demandActionsPtr->addWidget(evaluateDemandButtonPtr);
   demandActionsPtr->addStretch();
   demandLayoutPtr->addLayout(demandActionsPtr);

   mDemandTablePtr = CreateEditableTable(
      {"Demand", "Mission stage", "Business", "Source", "Destination",
       "Payload bits", "Traffic bit/s", "Bandwidth bit/s", "Max delay ms",
       "Min PDR %", "Max distance m", "Min network size", "Allowed networks"},
      demandPagePtr);
   mDemandTablePtr->setMinimumHeight(145);
   demandLayoutPtr->addWidget(mDemandTablePtr);
   mDemandMatchTablePtr = CreateTable(
      {"Demand", "Status", "Snapshot", "Path", "Distance", "Rate", "Reasons"},
      demandPagePtr);
   mDemandMatchTablePtr->setMinimumHeight(105);
   demandLayoutPtr->addWidget(mDemandMatchTablePtr);
   mDemandGapTablePtr = CreateTable(
      {"Demand", "Requirement", "Required", "Current", "Margin", "Reason"},
      demandPagePtr);
   mDemandGapTablePtr->setMinimumHeight(105);
   demandLayoutPtr->addWidget(mDemandGapTablePtr);
   mDemandRecommendationTablePtr = CreateTable(
      {"Demand", "Type", "Status", "Candidate", "Value", "Rank",
       "Source / confidence", "Reason", "Evidence"},
      demandPagePtr);
   mDemandRecommendationTablePtr->setMinimumHeight(120);
   demandLayoutPtr->addWidget(mDemandRecommendationTablePtr);

   connect(loadDemandButtonPtr, &QPushButton::clicked,
           this, &DockWidget::LoadResourceDemands);
   connect(unloadDemandButtonPtr, &QPushButton::clicked,
           this, &DockWidget::UnloadResourceDemands);
   connect(saveDemandButtonPtr, &QPushButton::clicked,
           this, &DockWidget::SaveResourceDemandRevision);
   connect(evaluateDemandButtonPtr, &QPushButton::clicked,
           this, &DockWidget::EvaluateResourceDemands);
   connect(mDemandTablePtr, &QTableWidget::itemChanged, this,
           [this](QTableWidgetItem*)
           {
              mDemandDirty = true;
              mDemandOperationPtr->setText(
                 QString::fromUtf8("DRAFT：表格编辑尚未写入需求仓库，旧匹配结果已失效"));
              mDemandMatchTablePtr->setRowCount(0);
              mDemandGapTablePtr->setRowCount(0);
              mDemandRecommendationTablePtr->setRowCount(0);
           });

   tabsPtr->addTab(mNetworkTablePtr, "Four-network overview");
   tabsPtr->addTab(mMetricsTablePtr, "Window metrics");
   tabsPtr->addTab(mEndpointTablePtr, "Members");
   tabsPtr->addTab(mLinkTablePtr, "Links");
   tabsPtr->addTab(assessmentPagePtr, "Task assessment");
   tabsPtr->addTab(capabilityPagePtr, QString::fromUtf8("通信能力"));
   tabsPtr->addTab(planPagePtr, QString::fromUtf8("资源规划"));
   tabsPtr->addTab(demandPagePtr, QString::fromUtf8("需求匹配"));
   tabsPtr->setCurrentWidget(assessmentPagePtr);
   rootLayoutPtr->addWidget(tabsPtr);

   setWidget(contentPtr);
   resize(1120, 760);

   connect(&mData, &DataContainer::SnapshotChanged, this, &DockWidget::Refresh);
   connect(&mData, &DataContainer::NetworkPlanChanged, this,
           [this]()
           {
              if (!mPlanDirty) RefreshNetworkPlan();
              if (!mDemandDirty) RefreshResourceDemands();
           });
   connect(&mData, &DataContainer::ResourceDemandChanged, this,
           [this]()
           {
              if (!mDemandDirty) RefreshResourceDemands();
           });
   Refresh();
   RefreshNetworkPlan();
   RefreshResourceDemands();
}

void WkNrm::DockWidget::EvaluateTask()
{
   nrm::AssessmentTask task;
   task.taskId              = "GUI-" + std::to_string(mData.GetSnapshot().snapshotVersion);
   task.sourcePlatform      = mSourceSelectorPtr->currentData().toString().toStdString();
   task.destinationPlatform = mDestinationSelectorPtr->currentData().toString().toStdString();
   if (task.sourcePlatform.empty() || task.destinationPlatform.empty())
   {
      mAssessmentResultPtr->setPlainText("Select both a source platform and a destination platform.");
      return;
   }
   if (task.sourcePlatform == task.destinationPlatform)
   {
      mAssessmentResultPtr->setPlainText("Source and destination must be different platforms.");
      return;
   }
   task.requiredBandwidthBps = mBandwidthKbpsPtr->value() * 1000.0;
   task.maximumDelayMs       = mMaximumDelayMsPtr->value();
   task.minimumPdrPercent    = mMinimumPdrPtr->value();
   AddAllowedNetwork(mAllowedNetworkPtr->currentText(), task.allowedNetworks);

   nrm::NetworkProfileRepository profiles =
      nrm::NetworkProfileRepository::BuiltInDemo();
   const QByteArray profilePath = qgetenv("NRM_NETWORK_PROFILE_CONFIG");
   if (!profilePath.isEmpty())
   {
      nrm::NetworkProfileValidation validation;
      profiles.LoadFromFile(profilePath.constData(), validation);
   }
   const nrm::AssessmentResult result =
      nrm::AssessmentEvaluator(profiles).Evaluate(mData.GetSnapshot(), task);
   mData.StoreAssessment(result);
   QStringList route;
   for (const std::string& platform : result.primaryRoute)
   {
      route.push_back(QString::fromStdString(platform));
   }
   QStringList backupRoute;
   for (const std::string& platform : result.backupRoute)
   {
      backupRoute.push_back(QString::fromStdString(platform));
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
   text += "Primary route: " + (route.isEmpty() ? QString::fromUtf8("—") : route.join(" → "));
   if (!route.isEmpty() && result.primaryRouteUsesCandidate)
   {
      text += "  [CANDIDATE]";
   }
   text += "\n";
   text += "Backup route: " +
           (backupRoute.isEmpty() ? QString::fromUtf8("—") : backupRoute.join(" → "));
   if (!backupRoute.isEmpty() && result.backupRouteUsesCandidate)
   {
      text += "  [CANDIDATE]";
   }
   text += "\n";
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

void WkNrm::DockWidget::QueryCapability()
{
   nrm::CapabilityRequest request;
   request.requestId = "GUI-CAP-" + std::to_string(mData.GetSnapshot().snapshotVersion);
   request.sourcePlatform =
      mCapabilitySourceSelectorPtr->currentData().toString().toStdString();
   request.destinationPlatform =
      mCapabilityDestinationSelectorPtr->currentData().toString().toStdString();
   if (request.sourcePlatform.empty() || request.destinationPlatform.empty() ||
       request.sourcePlatform == request.destinationPlatform)
   {
      mCapabilityResultPtr->setPlainText(
         "Select different source and destination platforms.");
      return;
   }
   request.requiredBandwidthBps = mCapabilityBandwidthKbpsPtr->value() * 1000.0;
   request.maximumDelayMs = mCapabilityMaximumDelayMsPtr->value();
   request.minimumPdrPercent = mCapabilityMinimumPdrPtr->value();
   AddAllowedNetwork(mCapabilityAllowedNetworkPtr->currentText(), request.allowedNetworks);

   const nrm::CapabilityResult result = mData.QueryCapability(request);
   QStringList route;
   for (const std::string& platform : result.route)
   {
      route.push_back(QString::fromStdString(platform));
   }
   QStringList reasons;
   for (nrm::CapabilityReason reason : result.reasons)
   {
      reasons.push_back(nrm::ToString(reason));
   }

   QString text;
   text += QString("Snapshot: %1 @ %2 s\n")
              .arg(result.snapshotVersion)
              .arg(result.simTime, 0, 'f', 3);
   text += QString("Request valid: %1\nPath available: %2\nPath source: %3\n")
              .arg(result.requestValid ? "YES" : "NO")
              .arg(result.pathAvailable ? "YES" : "NO")
              .arg(result.usesCandidate ? "PARAMETERIZED CANDIDATE" : "CURRENT");
   text += "Route: " + (route.isEmpty() ? QString::fromUtf8("—") : route.join(" → ")) + "\n";
   text += "Communication distance: " + CapabilityMetricText(result.communicationDistanceM, 1) + "\n";
   text += "Maximum hop distance: " + CapabilityMetricText(result.maximumHopDistanceM, 1) + "\n";
   text += "Transmission rate: " + CapabilityMetricText(result.transmissionRateBps, 1) + "\n";
   text += "Packet loss: " + CapabilityMetricText(result.packetLossPercent, 3) + "\n";
   text += "Transmission delay: " + CapabilityMetricText(result.transmissionDelayMs, 3) + "\n";
   text += "Network throughput: " + CapabilityMetricText(result.networkThroughputBps, 1) + "\n";
   text += "Access ratio: " + CapabilityMetricText(result.accessRatioPercent, 2) + "\n";
   text += "Environment effects:\n";
   for (const nrm::EnvironmentEffect& effect : result.environmentEffects)
   {
      text += QString("- %1: %2 [source=%3, confidence=%4, reason=%5]\n")
                 .arg(nrm::ToString(effect.domain),
                      effect.valid ? "VALID" : "INVALID",
                      nrm::ToString(effect.origin),
                      nrm::ToString(effect.confidence),
                      nrm::ToString(effect.reason));
   }
   text += "Reason codes: " + (reasons.isEmpty() ? QString("NONE") : reasons.join(", "));
   mCapabilityResultPtr->setPlainText(text);
}

void WkNrm::DockWidget::LoadNetworkPlan()
{
   if (mPlanDirty &&
       QMessageBox::question(this, QString::fromUtf8("未保存草案"),
                             QString::fromUtf8("加载新规划将放弃当前表格编辑，继续吗？")) !=
          QMessageBox::Yes)
      return;
   const QString path = QFileDialog::getOpenFileName(
      this, QString::fromUtf8("加载内部规划文件"), QString(),
      QString::fromUtf8("NRM规划文件 (*.nrm);;所有文件 (*)"));
   if (path.isEmpty()) return;
   mPlanDirty = false;
   mData.LoadNetworkPlan(path.toStdString());
   RefreshNetworkPlan();
}

void WkNrm::DockWidget::UnloadNetworkPlan()
{
   if (!mData.HasNetworkPlan()) return;
   if (QMessageBox::question(this, QString::fromUtf8("卸载规划"),
                             QString::fromUtf8("卸载只清除规划状态，不影响实时快照。继续吗？")) !=
       QMessageBox::Yes)
      return;
   mPlanDirty = false;
   mData.UnloadNetworkPlan();
   RefreshNetworkPlan();
}

void WkNrm::DockWidget::SaveNetworkPlanRevision()
{
   if (!mData.HasNetworkPlan())
   {
      mPlanOperationPtr->setText("NO_CURRENT_PLAN");
      return;
   }
   const nrm::NetworkPlanDocument* planPtr = mData.GetNetworkPlan();
   const QString suggested = planPtr == nullptr
                                ? QString()
                                : QString("%1-r%2.nrm")
                                     .arg(QString::fromStdString(planPtr->planId))
                                     .arg(planPtr->revision + 1);
   const QString path = QFileDialog::getSaveFileName(
      this, QString::fromUtf8("保存内部规划新修订"), suggested,
      QString::fromUtf8("NRM规划文件 (*.nrm);;所有文件 (*)"));
   if (path.isEmpty()) return;
   // Saving is an explicit new-revision operation even when table values are unchanged.
   mPlanDirty = true;
   if (!ApplyNetworkPlanEdits()) return;
   mData.SaveNetworkPlanRevision(path.toStdString());
   RefreshNetworkPlan();
}

void WkNrm::DockWidget::ValidateNetworkPlan()
{
   if (!ApplyNetworkPlanEdits()) return;
   mData.ValidateNetworkPlan();
   RefreshNetworkPlan();
}

void WkNrm::DockWidget::EvaluateNetworkPlan()
{
   if (!ApplyNetworkPlanEdits()) return;
   mData.EvaluateNetworkPlan();
   RefreshNetworkPlan();
}

void WkNrm::DockWidget::GenerateNetworkPlanPackage()
{
   if (!ApplyNetworkPlanEdits()) return;
   if (!mData.HasPlanValidation() || !mData.HasPlanEvaluation())
   {
      mData.GenerateNetworkPlanPackage();
      RefreshNetworkPlan();
      return;
   }
   const QString directory = QFileDialog::getExistingDirectory(
      this, QString::fromUtf8("选择本地分发包输出根目录"));
   if (directory.isEmpty()) return;
   mData.GenerateNetworkPlanPackage(directory.toStdString());
   RefreshNetworkPlan();
}

bool WkNrm::DockWidget::ApplyNetworkPlanEdits()
{
   if (!mPlanDirty) return mData.HasNetworkPlan();
   const nrm::NetworkPlanDocument* currentPtr = mData.GetNetworkPlan();
   if (currentPtr == nullptr)
   {
      mPlanOperationPtr->setText("NO_CURRENT_PLAN");
      return false;
   }

   nrm::NetworkPlanDocument draft = *currentPtr;
   draft.previousPlanId = currentPtr->planId;
   draft.previousRevision = currentPtr->revision;
   draft.revision = currentPtr->revision + 1;
   draft.createdTime =
      QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
   draft.state = nrm::NetworkPlanState::cDRAFT;
   draft.valid = true;
   draft.allocations.clear();
   draft.demands.clear();

   for (int row = 0; row < mPlanAllocationTablePtr->rowCount(); ++row)
   {
      nrm::NetworkPlanAllocation allocation;
      allocation.allocationId = CellText(mPlanAllocationTablePtr, row, 0).toStdString();
      allocation.networkName = CellText(mPlanAllocationTablePtr, row, 1).toStdString();
      allocation.networkType = ParseNetworkType(CellText(mPlanAllocationTablePtr, row, 2));
      allocation.profileId = CellText(mPlanAllocationTablePtr, row, 3).toStdString();
      bool frequencyOk = false;
      allocation.frequencyHz =
         CellText(mPlanAllocationTablePtr, row, 4).toDouble(&frequencyOk);
      allocation.channelId = CellText(mPlanAllocationTablePtr, row, 5).toStdString();
      allocation.subnetId = CellText(mPlanAllocationTablePtr, row, 6).toStdString();
      allocation.slotIds = SplitValues(CellText(mPlanAllocationTablePtr, row, 7));
      allocation.memberPlatformIds =
         SplitValues(CellText(mPlanAllocationTablePtr, row, 8));
      allocation.routePolicyId =
         CellText(mPlanAllocationTablePtr, row, 9).toStdString();
      const QString enabled = CellText(mPlanAllocationTablePtr, row, 10);
      if (!frequencyOk || (enabled != "0" && enabled != "1"))
      {
         mPlanOperationPtr->setText("PARSE_ERROR [allocation numeric/enabled]");
         return false;
      }
      allocation.enabled = enabled == "1";
      draft.allocations.push_back(allocation);
   }

   for (int row = 0; row < mPlanDemandTablePtr->rowCount(); ++row)
   {
      nrm::NetworkPlanDemand demand;
      demand.demandId = CellText(mPlanDemandTablePtr, row, 0).toStdString();
      demand.businessType = CellText(mPlanDemandTablePtr, row, 1).toStdString();
      demand.sourcePlatform = CellText(mPlanDemandTablePtr, row, 2).toStdString();
      demand.destinationPlatform = CellText(mPlanDemandTablePtr, row, 3).toStdString();
      bool payloadOk = false;
      bool bandwidthOk = false;
      bool delayOk = false;
      bool pdrOk = false;
      demand.payloadBits =
         CellText(mPlanDemandTablePtr, row, 4).toULongLong(&payloadOk);
      demand.requiredBandwidthBps =
         CellText(mPlanDemandTablePtr, row, 5).toDouble(&bandwidthOk);
      demand.maximumDelayMs =
         CellText(mPlanDemandTablePtr, row, 6).toDouble(&delayOk);
      demand.minimumPdrPercent =
         CellText(mPlanDemandTablePtr, row, 7).toDouble(&pdrOk);
      for (const std::string& token :
           SplitValues(CellText(mPlanDemandTablePtr, row, 8)))
         demand.allowedNetworks.push_back(
            ParseNetworkType(QString::fromStdString(token)));
      if (!payloadOk || !bandwidthOk || !delayOk || !pdrOk)
      {
         mPlanOperationPtr->setText("PARSE_ERROR [demand numeric]");
         return false;
      }
      draft.demands.push_back(demand);
   }

   const bool replaced = mData.ReplaceNetworkPlanDraft(draft);
   if (!replaced)
   {
      mPlanOperationPtr->setText(
         QString::fromLatin1(nrm::ToString(mData.GetPlanOperation().reason)));
      return false;
   }
   mPlanDirty = false;
   RefreshNetworkPlan();
   return true;
}

void WkNrm::DockWidget::RefreshNetworkPlan()
{
   if (mPlanDirty) return;
   const nrm::NetworkPlanDocument* planPtr = mData.GetNetworkPlan();
   if (planPtr == nullptr)
   {
      mPlanSummaryPtr->setText(QString::fromUtf8("当前规划：未加载"));
      const nrm::PlanRepositoryResult& operation = mData.GetPlanOperation();
      mPlanOperationPtr->setText(
         operation.reason == nrm::PlanValidationReason::cNONE
            ? QString::fromUtf8("状态：无规划")
            : QString("reason=%1, field=%2")
                 .arg(nrm::ToString(operation.reason),
                      QString::fromStdString(operation.field)));
      mPlanAllocationTablePtr->setRowCount(0);
      mPlanDemandTablePtr->setRowCount(0);
      mPlanIssueTablePtr->setRowCount(0);
      mPlanEvaluationTablePtr->setRowCount(0);
      return;
   }

   mPlanSummaryPtr->setText(
      QString("planId=%1 | revision=%2 | state=%3 | source=%4 | confidence=%5 | config=%6")
         .arg(QString::fromStdString(planPtr->planId))
         .arg(planPtr->revision)
         .arg(nrm::ToString(mData.GetNetworkPlanState()))
         .arg(nrm::ToString(planPtr->source))
         .arg(nrm::ToString(planPtr->confidence))
         .arg(QString::fromStdString(planPtr->configVersion)));

   QString operationText;
   if (mData.HasDistributionPackage())
   {
      const nrm::DistributionPackageResult& package = mData.GetDistributionPackage();
      operationText = package.generated
                         ? QString("package=%1").arg(
                              QString::fromStdString(package.outputPath))
                         : QString("reason=%1").arg(nrm::ToString(package.reason));
   }
   else
   {
      const nrm::PlanRepositoryResult& operation = mData.GetPlanOperation();
      operationText = operation.success
                         ? QString("operation=OK, path=%1").arg(
                              QString::fromStdString(operation.path))
                         : QString("reason=%1").arg(nrm::ToString(operation.reason));
   }
   mPlanOperationPtr->setText(operationText);

   const QSignalBlocker allocationBlocker(mPlanAllocationTablePtr);
   const QSignalBlocker demandBlocker(mPlanDemandTablePtr);
   mPlanAllocationTablePtr->setRowCount(
      static_cast<int>(planPtr->allocations.size()));
   for (std::size_t index = 0; index < planPtr->allocations.size(); ++index)
   {
      const int row = static_cast<int>(index);
      const nrm::NetworkPlanAllocation& allocation = planPtr->allocations[index];
      SetTableText(mPlanAllocationTablePtr, row, 0,
                   QString::fromStdString(allocation.allocationId));
      SetTableText(mPlanAllocationTablePtr, row, 1,
                   QString::fromStdString(allocation.networkName));
      SetTableText(mPlanAllocationTablePtr, row, 2,
                   nrm::ToString(allocation.networkType));
      SetTableText(mPlanAllocationTablePtr, row, 3,
                   QString::fromStdString(allocation.profileId));
      SetTableText(mPlanAllocationTablePtr, row, 4,
                   QString::number(allocation.frequencyHz, 'g', 16));
      SetTableText(mPlanAllocationTablePtr, row, 5,
                   QString::fromStdString(allocation.channelId));
      SetTableText(mPlanAllocationTablePtr, row, 6,
                   QString::fromStdString(allocation.subnetId));
      SetTableText(mPlanAllocationTablePtr, row, 7, JoinValues(allocation.slotIds));
      SetTableText(mPlanAllocationTablePtr, row, 8,
                   JoinValues(allocation.memberPlatformIds));
      SetTableText(mPlanAllocationTablePtr, row, 9,
                   QString::fromStdString(allocation.routePolicyId));
      SetTableText(mPlanAllocationTablePtr, row, 10,
                   allocation.enabled ? "1" : "0");
   }

   mPlanDemandTablePtr->setRowCount(static_cast<int>(planPtr->demands.size()));
   for (std::size_t index = 0; index < planPtr->demands.size(); ++index)
   {
      const int row = static_cast<int>(index);
      const nrm::NetworkPlanDemand& demand = planPtr->demands[index];
      SetTableText(mPlanDemandTablePtr, row, 0,
                   QString::fromStdString(demand.demandId));
      SetTableText(mPlanDemandTablePtr, row, 1,
                   QString::fromStdString(demand.businessType));
      SetTableText(mPlanDemandTablePtr, row, 2,
                   QString::fromStdString(demand.sourcePlatform));
      SetTableText(mPlanDemandTablePtr, row, 3,
                   QString::fromStdString(demand.destinationPlatform));
      SetTableText(mPlanDemandTablePtr, row, 4,
                   QString::number(demand.payloadBits));
      SetTableText(mPlanDemandTablePtr, row, 5,
                   QString::number(demand.requiredBandwidthBps, 'g', 16));
      SetTableText(mPlanDemandTablePtr, row, 6,
                   QString::number(demand.maximumDelayMs, 'g', 16));
      SetTableText(mPlanDemandTablePtr, row, 7,
                   QString::number(demand.minimumPdrPercent, 'g', 16));
      SetTableText(mPlanDemandTablePtr, row, 8,
                   JoinNetworks(demand.allowedNetworks));
   }

   mPlanIssueTablePtr->setRowCount(
      mData.HasPlanValidation()
         ? static_cast<int>(mData.GetPlanValidation().issues.size())
         : 0);
   if (mData.HasPlanValidation())
   {
      const std::vector<nrm::PlanValidationIssue>& issues =
         mData.GetPlanValidation().issues;
      for (std::size_t index = 0; index < issues.size(); ++index)
      {
         const int row = static_cast<int>(index);
         SetTableText(mPlanIssueTablePtr, row, 0, nrm::ToString(issues[index].severity));
         SetTableText(mPlanIssueTablePtr, row, 1, nrm::ToString(issues[index].reason));
         SetTableText(mPlanIssueTablePtr, row, 2,
                      QString::fromStdString(issues[index].field));
         SetTableText(mPlanIssueTablePtr, row, 3,
                      QString::fromStdString(issues[index].recordId));
         SetTableText(mPlanIssueTablePtr, row, 4,
                      QString::fromStdString(issues[index].description));
      }
   }

   mPlanEvaluationTablePtr->setRowCount(
      mData.HasPlanEvaluation()
         ? static_cast<int>(mData.GetPlanEvaluation().demands.size())
         : 0);
   if (mData.HasPlanEvaluation())
   {
      const std::vector<nrm::PlanDemandEvaluation>& evaluations =
         mData.GetPlanEvaluation().demands;
      for (std::size_t index = 0; index < evaluations.size(); ++index)
      {
         const int row = static_cast<int>(index);
         const nrm::PlanDemandEvaluation& evaluation = evaluations[index];
         QStringList reasons;
         for (nrm::PlanValidationReason reason : evaluation.reasons)
            reasons.push_back(nrm::ToString(reason));
         SetTableText(mPlanEvaluationTablePtr, row, 0,
                      QString::fromStdString(evaluation.demandId));
         SetTableText(mPlanEvaluationTablePtr, row, 1,
                      nrm::ToString(evaluation.status));
         SetTableText(mPlanEvaluationTablePtr, row, 2,
                      evaluation.capability.pathAvailable
                         ? (evaluation.capability.usesCandidate
                               ? "PARAMETERIZED_MODEL/LOW"
                               : "CURRENT")
                         : "UNAVAILABLE");
         SetTableText(mPlanEvaluationTablePtr, row, 3,
                      MetricText(evaluation.capability.transmissionRateBps, 1));
         SetTableText(mPlanEvaluationTablePtr, row, 4,
                      MetricText(evaluation.capability.transmissionDelayMs, 3));
         SetTableText(mPlanEvaluationTablePtr, row, 5,
                      MetricText(evaluation.capability.packetLossPercent, 3));
         SetTableText(mPlanEvaluationTablePtr, row, 6,
                      reasons.isEmpty() ? "NONE" : reasons.join(", "));
      }
   }
}

void WkNrm::DockWidget::LoadResourceDemands()
{
   if (mDemandDirty &&
       QMessageBox::question(this, QString::fromUtf8("未保存需求草案"),
                             QString::fromUtf8("加载新需求集将放弃当前表格编辑，继续吗？")) !=
          QMessageBox::Yes)
      return;
   const QString path = QFileDialog::getOpenFileName(
      this, QString::fromUtf8("加载内部需求文件"), QString(),
      QString::fromUtf8("NRM需求文件 (*.nrm *.demand);;所有文件 (*)"));
   if (path.isEmpty()) return;
   mDemandDirty = false;
   mData.LoadResourceDemands(path.toStdString());
   RefreshResourceDemands();
}

void WkNrm::DockWidget::UnloadResourceDemands()
{
   if (!mData.HasResourceDemandSet()) return;
   if (QMessageBox::question(this, QString::fromUtf8("卸载需求集"),
                             QString::fromUtf8("卸载将清除当前需求及匹配结果，继续吗？")) !=
       QMessageBox::Yes)
      return;
   mDemandDirty = false;
   mData.UnloadResourceDemands();
   RefreshResourceDemands();
}

void WkNrm::DockWidget::SaveResourceDemandRevision()
{
   if (!mData.HasResourceDemandSet())
   {
      mDemandOperationPtr->setText("NO_CURRENT_DEMAND_SET");
      return;
   }
   const nrm::ResourceDemandSet* setPtr = mData.GetResourceDemandSet();
   const QString suggested = setPtr == nullptr
                                ? QString()
                                : QString("%1-r%2.demand")
                                     .arg(QString::fromStdString(setPtr->demandSetId))
                                     .arg(setPtr->revision + 1);
   const QString path = QFileDialog::getSaveFileName(
      this, QString::fromUtf8("保存内部需求新修订"), suggested,
      QString::fromUtf8("NRM需求文件 (*.demand);;所有文件 (*)"));
   if (path.isEmpty()) return;
   mDemandDirty = true;
   if (!ApplyResourceDemandEdits()) return;
   mData.SaveResourceDemandRevision(path.toStdString());
   RefreshResourceDemands();
}

void WkNrm::DockWidget::EvaluateResourceDemands()
{
   if (!ApplyResourceDemandEdits()) return;
   mData.EvaluateResourceDemands();
   RefreshResourceDemands();
}

bool WkNrm::DockWidget::ApplyResourceDemandEdits()
{
   if (!mDemandDirty) return mData.HasResourceDemandSet();
   const nrm::ResourceDemandSet* currentPtr = mData.GetResourceDemandSet();
   if (currentPtr == nullptr)
   {
      mDemandOperationPtr->setText("NO_CURRENT_DEMAND_SET");
      return false;
   }

   nrm::ResourceDemandSet draft = *currentPtr;
   draft.previousDemandSetId = currentPtr->demandSetId;
   draft.previousRevision = currentPtr->revision;
   draft.revision = currentPtr->revision + 1;
   draft.createdTime =
      QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
   draft.valid = true;
   draft.demands.clear();

   for (int row = 0; row < mDemandTablePtr->rowCount(); ++row)
   {
      nrm::ResourceDemand demand;
      demand.demandId = CellText(mDemandTablePtr, row, 0).toStdString();
      demand.demandSetId = draft.demandSetId;
      demand.revision = draft.revision;
      demand.missionStage = CellText(mDemandTablePtr, row, 1).toStdString();
      demand.businessType = CellText(mDemandTablePtr, row, 2).toStdString();
      demand.sourcePlatform = CellText(mDemandTablePtr, row, 3).toStdString();
      demand.destinationPlatform = CellText(mDemandTablePtr, row, 4).toStdString();
      bool payloadOk = false;
      bool trafficOk = false;
      bool bandwidthOk = false;
      bool delayOk = false;
      bool pdrOk = false;
      bool distanceOk = false;
      bool networkSizeOk = false;
      demand.payloadBits = static_cast<std::uint64_t>(
         CellText(mDemandTablePtr, row, 5).toULongLong(&payloadOk));
      demand.businessTrafficBps =
         CellText(mDemandTablePtr, row, 6).toDouble(&trafficOk);
      demand.requiredBandwidthBps =
         CellText(mDemandTablePtr, row, 7).toDouble(&bandwidthOk);
      demand.maximumDelayMs =
         CellText(mDemandTablePtr, row, 8).toDouble(&delayOk);
      demand.minimumPdrPercent =
         CellText(mDemandTablePtr, row, 9).toDouble(&pdrOk);
      demand.maximumDistanceM =
         CellText(mDemandTablePtr, row, 10).toDouble(&distanceOk);
      const qulonglong networkSize =
         CellText(mDemandTablePtr, row, 11).toULongLong(&networkSizeOk);
      if (networkSize > std::numeric_limits<std::size_t>::max())
         networkSizeOk = false;
      demand.minimumNetworkSize = static_cast<std::size_t>(networkSize);
      for (const std::string& token :
           SplitValues(CellText(mDemandTablePtr, row, 12)))
      {
         const nrm::NetworkType type =
            ParseNetworkType(QString::fromStdString(token));
         if (type == nrm::NetworkType::cUNKNOWN)
            networkSizeOk = false;
         demand.allowedNetworks.push_back(type);
      }
      if (!payloadOk || !trafficOk || !bandwidthOk || !delayOk || !pdrOk ||
          !distanceOk || !networkSizeOk)
      {
         mDemandOperationPtr->setText("PARSE_ERROR [demand numeric/network]");
         return false;
      }

      const auto existing = std::find_if(
         currentPtr->demands.begin(), currentPtr->demands.end(),
         [&demand](const nrm::ResourceDemand& aExisting)
         {
            return aExisting.demandId == demand.demandId;
         });
      if (existing != currentPtr->demands.end())
      {
         demand.source = existing->source;
         demand.confidence = existing->confidence;
      }
      else
      {
         demand.source = draft.source;
         demand.confidence = draft.confidence;
      }
      demand.valid = true;
      draft.demands.push_back(demand);
   }

   const bool replaced = mData.ReplaceResourceDemandDraft(draft);
   if (!replaced)
   {
      const nrm::ResourceDemandRepositoryResult& operation =
         mData.GetDemandOperation();
      mDemandOperationPtr->setText(
         QString("reason=%1, field=%2")
            .arg(nrm::ToString(operation.reason),
                 QString::fromStdString(operation.field)));
      return false;
   }
   mDemandDirty = false;
   RefreshResourceDemands();
   return true;
}

void WkNrm::DockWidget::RefreshResourceDemands()
{
   if (mDemandDirty) return;
   const nrm::ResourceDemandSet* setPtr = mData.GetResourceDemandSet();
   if (setPtr == nullptr)
   {
      mDemandSummaryPtr->setText(QString::fromUtf8("当前需求集：未加载"));
      const nrm::ResourceDemandRepositoryResult& operation =
         mData.GetDemandOperation();
      mDemandOperationPtr->setText(
         operation.reason == nrm::ResourceDemandReason::cNONE
            ? QString::fromUtf8("状态：无需求集")
            : QString("reason=%1, field=%2")
                 .arg(nrm::ToString(operation.reason),
                      QString::fromStdString(operation.field)));
      mDemandTablePtr->setRowCount(0);
      mDemandMatchTablePtr->setRowCount(0);
      mDemandGapTablePtr->setRowCount(0);
      mDemandRecommendationTablePtr->setRowCount(0);
      return;
   }

   mDemandSummaryPtr->setText(
      QString("demandSetId=%1 | revision=%2 | demands=%3 | source=%4 | confidence=%5 | config=%6")
         .arg(QString::fromStdString(setPtr->demandSetId))
         .arg(setPtr->revision)
         .arg(setPtr->demands.size())
         .arg(nrm::ToString(setPtr->source))
         .arg(nrm::ToString(setPtr->confidence))
         .arg(QString::fromStdString(setPtr->configVersion)));
   const nrm::ResourceDemandRepositoryResult& operation =
      mData.GetDemandOperation();
   mDemandOperationPtr->setText(
      operation.success
         ? QString("operation=OK, path=%1").arg(
              QString::fromStdString(operation.path))
         : QString("reason=%1, field=%2")
              .arg(nrm::ToString(operation.reason),
                   QString::fromStdString(operation.field)));

   const QSignalBlocker demandBlocker(mDemandTablePtr);
   mDemandTablePtr->setRowCount(static_cast<int>(setPtr->demands.size()));
   for (std::size_t index = 0; index < setPtr->demands.size(); ++index)
   {
      const int row = static_cast<int>(index);
      const nrm::ResourceDemand& demand = setPtr->demands[index];
      SetTableText(mDemandTablePtr, row, 0, QString::fromStdString(demand.demandId));
      SetTableText(mDemandTablePtr, row, 1, QString::fromStdString(demand.missionStage));
      SetTableText(mDemandTablePtr, row, 2, QString::fromStdString(demand.businessType));
      SetTableText(mDemandTablePtr, row, 3, QString::fromStdString(demand.sourcePlatform));
      SetTableText(mDemandTablePtr, row, 4,
                   QString::fromStdString(demand.destinationPlatform));
      SetTableText(mDemandTablePtr, row, 5, QString::number(demand.payloadBits));
      SetTableText(mDemandTablePtr, row, 6,
                   QString::number(demand.businessTrafficBps, 'g', 16));
      SetTableText(mDemandTablePtr, row, 7,
                   QString::number(demand.requiredBandwidthBps, 'g', 16));
      SetTableText(mDemandTablePtr, row, 8,
                   QString::number(demand.maximumDelayMs, 'g', 16));
      SetTableText(mDemandTablePtr, row, 9,
                   QString::number(demand.minimumPdrPercent, 'g', 16));
      SetTableText(mDemandTablePtr, row, 10,
                   QString::number(demand.maximumDistanceM, 'g', 16));
      SetTableText(mDemandTablePtr, row, 11,
                   QString::number(static_cast<qulonglong>(demand.minimumNetworkSize)));
      SetTableText(mDemandTablePtr, row, 12, JoinNetworks(demand.allowedNetworks));
   }

   if (!mData.HasDemandMatching())
   {
      mDemandMatchTablePtr->setRowCount(0);
      mDemandGapTablePtr->setRowCount(0);
      mDemandRecommendationTablePtr->setRowCount(0);
      return;
   }

   const nrm::ResourceDemandBatchResult& batch = mData.GetDemandMatching();
   mDemandOperationPtr->setText(
      QString("matching: total=%1, satisfied=%2, unsatisfied=%3, dataInvalid=%4, snapshot=%5")
         .arg(batch.totalCount)
         .arg(batch.satisfiedCount)
         .arg(batch.unsatisfiedCount)
         .arg(batch.dataInvalidCount)
         .arg(batch.snapshotVersion));
   mDemandMatchTablePtr->setRowCount(static_cast<int>(batch.results.size()));
   int gapRows = 0;
   int recommendationRows = 0;
   for (const nrm::ResourceDemandMatchResult& result : batch.results)
   {
      for (const nrm::RequirementCheck& check : result.checks)
         if (check.applicable && (!check.passed || !check.currentValue.valid)) ++gapRows;
      recommendationRows += static_cast<int>(result.recommendations.size());
   }
   mDemandGapTablePtr->setRowCount(gapRows);
   mDemandRecommendationTablePtr->setRowCount(recommendationRows);

   int gapRow = 0;
   int recommendationRow = 0;
   for (std::size_t index = 0; index < batch.results.size(); ++index)
   {
      const int row = static_cast<int>(index);
      const nrm::ResourceDemandMatchResult& result = batch.results[index];
      QStringList reasons;
      for (nrm::ResourceDemandReason reason : result.reasons)
         reasons.push_back(nrm::ToString(reason));
      SetTableText(mDemandMatchTablePtr, row, 0,
                   QString::fromStdString(result.demandId));
      SetTableText(mDemandMatchTablePtr, row, 1, nrm::ToString(result.status));
      SetTableText(mDemandMatchTablePtr, row, 2,
                   QString::number(result.snapshotVersion));
      SetTableText(mDemandMatchTablePtr, row, 3,
                   result.capability.pathAvailable ? "AVAILABLE" : "UNAVAILABLE");
      SetTableText(mDemandMatchTablePtr, row, 4,
                   MetricText(result.capability.communicationDistanceM, 1));
      SetTableText(mDemandMatchTablePtr, row, 5,
                   MetricText(result.capability.transmissionRateBps, 1));
      SetTableText(mDemandMatchTablePtr, row, 6,
                   reasons.isEmpty() ? "NONE" : reasons.join(", "));

      for (const nrm::RequirementCheck& check : result.checks)
      {
         if (!check.applicable || (check.passed && check.currentValue.valid)) continue;
         SetTableText(mDemandGapTablePtr, gapRow, 0,
                      QString::fromStdString(result.demandId));
         SetTableText(mDemandGapTablePtr, gapRow, 1, nrm::ToString(check.type));
         SetTableText(mDemandGapTablePtr, gapRow, 2, MetricText(check.requiredValue, 3));
         SetTableText(mDemandGapTablePtr, gapRow, 3, MetricText(check.currentValue, 3));
         SetTableText(mDemandGapTablePtr, gapRow, 4, MetricText(check.margin, 3));
         SetTableText(mDemandGapTablePtr, gapRow, 5, nrm::ToString(check.reason));
         ++gapRow;
      }

      for (const nrm::PlanningRecommendation& recommendation : result.recommendations)
      {
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 0,
                      QString::fromStdString(recommendation.demandId));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 1,
                      nrm::ToString(recommendation.type));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 2,
                      nrm::ToString(recommendation.status));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 3,
                      QString::fromStdString(recommendation.candidateId));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 4,
                      QString::fromStdString(recommendation.value));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 5,
                      recommendation.rank == 0 ? QString::fromUtf8("—")
                                               : QString::number(recommendation.rank));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 6,
                      QString("%1 / %2")
                         .arg(nrm::ToString(recommendation.source),
                              nrm::ToString(recommendation.confidence)));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 7,
                      nrm::ToString(recommendation.reason));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 8,
                      JoinValues(recommendation.evidence));
         ++recommendationRow;
      }
   }
}

void WkNrm::DockWidget::Refresh()
{
   const nrm::FrameworkSnapshot& snapshot = mData.GetSnapshot();
   RefreshNodeSelectors(snapshot);
   mVersionValuePtr->setText(nrm::cVERSION);
   mStateValuePtr->setText(RuntimeStateText(snapshot.runtimeState));
   mReportingValuePtr->setText(QString::fromStdString(mData.GetReportingStatus()));
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
   RefreshResourceDemands();
}

void WkNrm::DockWidget::RefreshNodeSelectors(const nrm::FrameworkSnapshot& aSnapshot)
{
   struct PlatformChoice
   {
      std::string platformName;
      std::string networkType;
      std::string address;
   };

   std::map<std::string, PlatformChoice> choicesByPlatform;
   for (const nrm::EndpointSnapshot& endpoint : aSnapshot.endpoints)
   {
      PlatformChoice& choice = choicesByPlatform[endpoint.platformName];
      choice.platformName = endpoint.platformName;
      if (choice.networkType.empty())
      {
         choice.networkType = nrm::ToString(endpoint.networkType);
         choice.address = endpoint.address;
      }
      else if (choice.networkType.find(nrm::ToString(endpoint.networkType)) == std::string::npos)
      {
         choice.networkType += "/";
         choice.networkType += nrm::ToString(endpoint.networkType);
      }
   }

   auto refreshSelector = [&choicesByPlatform](QComboBox* aSelectorPtr, const QString& aPreferredPlatform)
   {
      QString currentPlatform = aSelectorPtr->currentData().toString();
      if (currentPlatform.isEmpty())
      {
         currentPlatform = aPreferredPlatform;
      }

      bool unchanged = aSelectorPtr->count() == static_cast<int>(choicesByPlatform.size());
      int index = 0;
      for (const auto& entry : choicesByPlatform)
      {
         if (!unchanged ||
             aSelectorPtr->itemData(index).toString() != QString::fromStdString(entry.first))
         {
            unchanged = false;
            break;
         }
         ++index;
      }
      if (unchanged)
      {
         return;
      }

      const QSignalBlocker blocker(aSelectorPtr);
      aSelectorPtr->clear();
      for (const auto& entry : choicesByPlatform)
      {
         const PlatformChoice& choice = entry.second;
         const QString label =
            QString("%1 [%2 | %3]")
               .arg(QString::fromStdString(choice.platformName),
                    QString::fromStdString(choice.networkType),
                    QString::fromStdString(choice.address));
         aSelectorPtr->addItem(label, QString::fromStdString(choice.platformName));
      }
      const int restoredIndex = aSelectorPtr->findData(currentPlatform);
      if (restoredIndex >= 0)
      {
         aSelectorPtr->setCurrentIndex(restoredIndex);
      }
   };

   refreshSelector(mSourceSelectorPtr, "l16_fighter");
   refreshSelector(mDestinationSelectorPtr, "l16_command");
   refreshSelector(mCapabilitySourceSelectorPtr, "l16_fighter");
   refreshSelector(mCapabilityDestinationSelectorPtr, "l16_command");
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
