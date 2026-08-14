#include "NrmDockWidget.hpp"

#include <cmath>
#include <limits>
#include <map>
#include <vector>

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QProcess>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStyle>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "nrm/AssessmentEvaluator.hpp"
#include "nrm/NetworkTypeUtils.hpp"
#include "nrm/Version.hpp"
#include "NrmUiText.hpp"

namespace
{
QString ModernStyleSheet()
{
   return QString::fromUtf8(R"QSS(
      QWidget#NrmRoot {
         background: #0b1220;
         color: #dce7f5;
         font-family: "Noto Sans CJK SC", "Microsoft YaHei UI", sans-serif;
         font-size: 10pt;
      }
      QFrame#NrmHero {
         background: #111d30;
         border: 1px solid #24344e;
         border-radius: 10px;
      }
      QLabel#NrmHeroTitle {
         color: #f5f9ff;
         font-size: 17pt;
         font-weight: 700;
      }
      QLabel#NrmHeroSubtitle {
         color: #8fa8c5;
         font-size: 9pt;
      }
      QFrame#NrmMetricCard {
         background: #101a2b;
         border: 1px solid #22324a;
         border-radius: 8px;
      }
      QLabel#NrmMetricTitle {
         color: #7f96b2;
         font-size: 8pt;
      }
      QLabel#NrmMetricValue {
         color: #f2f7ff;
         font-size: 11pt;
         font-weight: 650;
      }
      QTabWidget::pane {
         background: #0f1929;
         border: 1px solid #22324a;
         border-radius: 8px;
         top: -1px;
      }
      QTabBar::tab {
         background: transparent;
         color: #8fa3bd;
         border: none;
         border-bottom: 2px solid transparent;
         padding: 9px 13px;
         margin-right: 2px;
      }
      QTabBar::tab:hover { color: #dbeafe; background: #142238; }
      QTabBar::tab:selected {
         color: #66c7ff;
         background: #132238;
         border-bottom: 2px solid #38bdf8;
         font-weight: 600;
      }
      QTableWidget {
         background: #0d1726;
         alternate-background-color: #111d2e;
         color: #d8e4f2;
         border: none;
         border-radius: 6px;
         gridline-color: transparent;
         selection-background-color: #164e73;
         selection-color: #ffffff;
      }
      QHeaderView::section {
         background: #16243a;
         color: #9fb4ce;
         border: none;
         border-right: 1px solid #243650;
         border-bottom: 1px solid #2a3d59;
         padding: 7px 8px;
         font-weight: 600;
      }
      QTableCornerButton::section { background: #16243a; border: none; }
      QLineEdit, QComboBox, QDoubleSpinBox, QTextEdit {
         background: #0b1524;
         color: #edf5ff;
         border: 1px solid #2a3d59;
         border-radius: 6px;
         padding: 6px 8px;
         selection-background-color: #1479b8;
      }
      QLineEdit:focus, QComboBox:focus, QDoubleSpinBox:focus, QTextEdit:focus {
         border: 1px solid #38bdf8;
      }
      QComboBox::drop-down { border: none; width: 24px; }
      QPushButton {
         background: #1479b8;
         color: #ffffff;
         border: 1px solid #2998d2;
         border-radius: 6px;
         min-height: 24px;
         padding: 6px 14px;
         font-weight: 600;
      }
      QPushButton:hover { background: #188aca; border-color: #5bc7f4; }
      QPushButton:pressed { background: #0f6398; }
      QPushButton:disabled { background: #253247; color: #71839a; border-color: #34445b; }
      QLabel { color: #d7e3f2; }
      QScrollBar:vertical {
         background: #0b1422;
         width: 10px;
         margin: 0;
      }
      QScrollBar::handle:vertical {
         background: #334a67;
         border-radius: 5px;
         min-height: 28px;
      }
      QScrollBar::handle:vertical:hover { background: #456381; }
      QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
   )QSS");
}

QFrame* CreateMetricCard(const QString& aTitle,
                         QLabel*       aValuePtr,
                         QWidget*      aParentPtr)
{
   QFrame* cardPtr = new QFrame(aParentPtr);
   cardPtr->setObjectName("NrmMetricCard");
   cardPtr->setMinimumHeight(54);
   QVBoxLayout* layoutPtr = new QVBoxLayout(cardPtr);
   layoutPtr->setContentsMargins(10, 7, 10, 7);
   layoutPtr->setSpacing(2);
   QLabel* titlePtr = new QLabel(aTitle, cardPtr);
   titlePtr->setObjectName("NrmMetricTitle");
   aValuePtr->setObjectName("NrmMetricValue");
   aValuePtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   layoutPtr->addWidget(titlePtr);
   layoutPtr->addWidget(aValuePtr);
   return cardPtr;
}

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

QString NavigationModeText(nrm::NavigationMode aMode)
{
   switch (aMode)
   {
   case nrm::NavigationMode::cPERFECT: return QString::fromUtf8("理想导航");
   case nrm::NavigationMode::cGPS_ACTIVE: return QString::fromUtf8("卫星导航");
   case nrm::NavigationMode::cGPS_DEGRADED: return QString::fromUtf8("卫导降级");
   case nrm::NavigationMode::cGPS_EXTERNAL: return QString::fromUtf8("外部导航输入");
   case nrm::NavigationMode::cINS: return QString::fromUtf8("惯性导航");
   case nrm::NavigationMode::cUNKNOWN: return QString::fromUtf8("未知");
   }
   return QString::fromUtf8("未知");
}

QString DisplayCode(const std::string& aCode)
{
   return QString::fromStdString(WkNrm::UiText::TranslateCodeWithRaw(aCode));
}

QString DisplayCode(const char* aCode)
{
   return DisplayCode(std::string(aCode == nullptr ? "" : aCode));
}

QString DisplayCodeList(const std::vector<std::string>& aCodes)
{
   QStringList output;
   for (const std::string& code : aCodes) output.push_back(DisplayCode(code));
   return output.join(QString::fromUtf8("、"));
}

QString CapabilityMetricText(const nrm::MetricValue<double>& aMetric, int aPrecision = 2)
{
   const QString metadata =
      QString::fromUtf8("来源=%1，置信度=%2，原因=%3")
         .arg(DisplayCode(nrm::ToString(aMetric.origin)),
              DisplayCode(nrm::ToString(aMetric.confidence)),
              DisplayCode(nrm::ToString(aMetric.reason)));
   if (!aMetric.valid)
   {
      return QString::fromUtf8("无效［") + metadata + QString::fromUtf8("］");
   }
   return QString::number(aMetric.value, 'f', aPrecision) + " " +
          QString::fromStdString(aMetric.unit) + QString::fromUtf8("［") + metadata + QString::fromUtf8("］");
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
   tablePtr->setSelectionMode(QAbstractItemView::SingleSelection);
   tablePtr->setAlternatingRowColors(true);
   tablePtr->setShowGrid(false);
   tablePtr->setWordWrap(false);
   tablePtr->setTextElideMode(Qt::ElideRight);
   tablePtr->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
   tablePtr->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
   tablePtr->setMinimumSize(0, 0);
   tablePtr->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
   tablePtr->verticalHeader()->setVisible(false);
   tablePtr->verticalHeader()->setDefaultSectionSize(30);
   tablePtr->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
   tablePtr->horizontalHeader()->setStretchLastSection(false);
   tablePtr->horizontalHeader()->setMinimumSectionSize(56);
   tablePtr->horizontalHeader()->setDefaultSectionSize(126);
   tablePtr->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
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

QString CurrentMissionPath()
{
   const QStringList arguments = QCoreApplication::arguments();
   for (int index = arguments.size() - 1; index > 0; --index)
   {
      const QFileInfo candidate(arguments[index]);
      if (candidate.exists() && candidate.isFile()) return candidate.canonicalFilePath();
   }
   return QString();
}

QString BoundScenarioPath(const QString& aPlanPath)
{
   QFile binding(aPlanPath + ".scenario");
   if (!binding.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
   const QString value = QString::fromUtf8(binding.readLine()).trimmed();
   if (value.isEmpty()) return QString();
   const QFileInfo candidate(QDir::isAbsolutePath(value)
                                ? value
                                : QDir(QString::fromLocal8Bit(qgetenv("NRM_SOURCE"))).filePath(value));
   return candidate.exists() ? candidate.canonicalFilePath() : QString();
}
} // namespace

WkNrm::DockWidget::DockWidget(DataContainer& aData, QWidget* aParentPtr)
   : QDockWidget(QString::fromUtf8("网络资源管理器"), aParentPtr)
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
   , mEnvironmentTablePtr(nullptr)
   , mNavigationTablePtr(nullptr)
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
   , mPlanRecommendationTablePtr(nullptr)
   , mPlanDetailTabsPtr(nullptr)
   , mMainTabsPtr(nullptr)
   , mPlanPagePtr(nullptr)
   , mDemandSummaryPtr(new QLabel(this))
   , mDemandOperationPtr(new QLabel(this))
   , mDemandTablePtr(nullptr)
   , mDemandMatchTablePtr(nullptr)
   , mDemandGapTablePtr(nullptr)
   , mDemandRecommendationTablePtr(nullptr)
   , mPreacceptanceStatusPtr(new QLabel(this))
   , mPreacceptanceTimePtr(new QLabel(this))
   , mPreacceptanceChecksPtr(new QLabel(this))
   , mPreacceptanceTestsPtr(new QLabel(this))
   , mPreacceptanceScenariosPtr(new QLabel(this))
   , mPreacceptanceSnapshotPtr(new QLabel(this))
   , mPreacceptanceRevisionPtr(new QLabel(this))
   , mPreacceptanceReportPtr(new QLabel(this))
   , mPreacceptanceNoticePtr(new QLabel(this))
   , mPreacceptanceMonitorPtr(new PreacceptanceStatusMonitor(this))
{
   QWidget* contentPtr = new QWidget(this);
   contentPtr->setObjectName("NrmRoot");
   contentPtr->setStyleSheet(ModernStyleSheet());
   QVBoxLayout* rootLayoutPtr = new QVBoxLayout(contentPtr);
   rootLayoutPtr->setContentsMargins(12, 10, 12, 12);
   rootLayoutPtr->setSpacing(10);

   QFrame* heroPtr = new QFrame(contentPtr);
   heroPtr->setObjectName("NrmHero");
   QHBoxLayout* heroLayoutPtr = new QHBoxLayout(heroPtr);
   heroLayoutPtr->setContentsMargins(14, 10, 14, 10);
   QVBoxLayout* heroTextPtr = new QVBoxLayout();
   heroTextPtr->setSpacing(1);
   QLabel* heroTitlePtr = new QLabel(QString::fromUtf8("网络资源态势中心"), heroPtr);
   heroTitlePtr->setObjectName("NrmHeroTitle");
   QLabel* heroSubtitlePtr = new QLabel(
      QString::fromUtf8("AFSIM 实时状态 · 四网资源 · 路由评估 · 导航与环境"), heroPtr);
   heroSubtitlePtr->setObjectName("NrmHeroSubtitle");
   heroTextPtr->addWidget(heroTitlePtr);
   heroTextPtr->addWidget(heroSubtitlePtr);
   heroLayoutPtr->addLayout(heroTextPtr);
   heroLayoutPtr->addStretch();
   QPushButton* statusTogglePtr =
      new QPushButton(QString::fromUtf8("展开运行概况"), heroPtr);
   statusTogglePtr->setCheckable(true);
   statusTogglePtr->setChecked(false);
   heroLayoutPtr->addWidget(statusTogglePtr);
   rootLayoutPtr->addWidget(heroPtr);

   QWidget* statusPanelPtr = new QWidget(contentPtr);
   QGridLayout* statusLayoutPtr = new QGridLayout(statusPanelPtr);
   statusLayoutPtr->setContentsMargins(0, 0, 0, 0);
   statusLayoutPtr->setHorizontalSpacing(8);
   statusLayoutPtr->setVerticalSpacing(8);
   statusLayoutPtr->addWidget(CreateMetricCard(QString::fromUtf8("运行状态"), mStateValuePtr, contentPtr), 0, 0);
   statusLayoutPtr->addWidget(CreateMetricCard(QString::fromUtf8("上报状态"), mReportingValuePtr, contentPtr), 0, 1);
   statusLayoutPtr->addWidget(CreateMetricCard(QString::fromUtf8("仿真时间"), mSimTimeValuePtr, contentPtr), 1, 0);
   statusLayoutPtr->addWidget(CreateMetricCard(QString::fromUtf8("网络 / 端点"), mNetworkCountValuePtr, contentPtr), 1, 1);
   statusLayoutPtr->addWidget(CreateMetricCard(QString::fromUtf8("已发送"), mTransmittedValuePtr, contentPtr), 2, 0);
   statusLayoutPtr->addWidget(CreateMetricCard(QString::fromUtf8("已接收"), mReceivedValuePtr, contentPtr), 2, 1);
   statusLayoutPtr->addWidget(CreateMetricCard(QString::fromUtf8("转发跳数"), mHopValuePtr, contentPtr), 3, 0);
   statusLayoutPtr->addWidget(CreateMetricCard(QString::fromUtf8("丢弃 / 路由失败"), mDiscardedValuePtr, contentPtr), 3, 1);
   rootLayoutPtr->addWidget(statusPanelPtr);
   statusPanelPtr->setVisible(false);
   connect(statusTogglePtr, &QPushButton::toggled, statusPanelPtr,
           &QWidget::setVisible);
   connect(statusTogglePtr, &QPushButton::toggled, this,
           [statusTogglePtr](bool aExpanded)
           {
              statusTogglePtr->setText(
                 aExpanded ? QString::fromUtf8("收起运行概况")
                           : QString::fromUtf8("展开运行概况"));
           });

   // 版本与端点总数仍保留在数据模型中，合并显示以减少顶部信息噪声。
   mVersionValuePtr->hide();
   mEndpointCountValuePtr->hide();

   QTabWidget* tabsPtr = new QTabWidget(contentPtr);
   mMainTabsPtr = tabsPtr;
   mNetworkTablePtr = CreateTable(
      {QString::fromUtf8("类型"), QString::fromUtf8("网络"), QString::fromUtf8("模型"),
       QString::fromUtf8("成员数"), QString::fromUtf8("在线数"), QString::fromUtf8("链路数"),
       QString::fromUtf8("发送"), QString::fromUtf8("接收"), QString::fromUtf8("丢弃")}, tabsPtr);
   mMetricsTablePtr = CreateTable(
      {QString::fromUtf8("类型"), QString::fromUtf8("网络"), QString::fromUtf8("统计窗口"),
       QString::fromUtf8("吞吐量"), "PDR", QString::fromUtf8("在网率"),
       QString::fromUtf8("排队时延"), QString::fromUtf8("传输时延"),
       QString::fromUtf8("队列占用率"), "ACK", "RTT"},
      tabsPtr);
   mEndpointTablePtr =
      CreateTable({QString::fromUtf8("类型"), QString::fromUtf8("平台"), QString::fromUtf8("通信设备"),
                   QString::fromUtf8("地址"), QString::fromUtf8("职责"), QString::fromUtf8("状态"), QString::fromUtf8("纬度"),
                   QString::fromUtf8("经度"), QString::fromUtf8("高度")}, tabsPtr);
   mLinkTablePtr = CreateTable(
      {QString::fromUtf8("类型"),
       QString::fromUtf8("源平台"),
       QString::fromUtf8("目的平台"),
       QString::fromUtf8("状态"),
       QString::fromUtf8("距离"),
       QString::fromUtf8("带宽"),
       QString::fromUtf8("10秒吞吐量"),
       QString::fromUtf8("占用率"),
       "RSSI",
       "SNR",
       "BER"},
      tabsPtr);
   mEnvironmentTablePtr = CreateTable(
      {QString::fromUtf8("环境域"), QString::fromUtf8("状态"),
       QString::fromUtf8("数据来源"), QString::fromUtf8("关键观测"),
       QString::fromUtf8("影响说明")}, tabsPtr);
   mNavigationTablePtr = CreateTable(
      {QString::fromUtf8("平台"), QString::fromUtf8("导航模式"),
       QString::fromUtf8("原生状态"), QString::fromUtf8("真实纬度"),
       QString::fromUtf8("真实经度"), QString::fromUtf8("真实高度"),
       QString::fromUtf8("感知纬度"), QString::fromUtf8("感知经度"),
       QString::fromUtf8("感知高度"), QString::fromUtf8("纵向误差"),
       QString::fromUtf8("横向误差"), QString::fromUtf8("垂直误差"),
       QString::fromUtf8("总位置误差"), QString::fromUtf8("更新时间")}, tabsPtr);
   QWidget* assessmentPagePtr = new QWidget(tabsPtr);
   QVBoxLayout* assessmentLayoutPtr = new QVBoxLayout(assessmentPagePtr);
   QFormLayout* taskFormPtr = new QFormLayout();
   mSourceSelectorPtr = new QComboBox(assessmentPagePtr);
   mDestinationSelectorPtr = new QComboBox(assessmentPagePtr);
   mSourceSelectorPtr->setMinimumContentsLength(24);
   mDestinationSelectorPtr->setMinimumContentsLength(24);
   mAllowedNetworkPtr = new QComboBox(assessmentPagePtr);
   mAllowedNetworkPtr->addItems({QString::fromUtf8("全部网络"), "LINK11", "LINK16", "SATCOM", "CDL"});
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
   taskFormPtr->addRow(QString::fromUtf8("源平台"), mSourceSelectorPtr);
   taskFormPtr->addRow(QString::fromUtf8("目的平台"), mDestinationSelectorPtr);
   taskFormPtr->addRow(QString::fromUtf8("允许使用的网络"), mAllowedNetworkPtr);
   taskFormPtr->addRow(QString::fromUtf8("所需带宽"), mBandwidthKbpsPtr);
   taskFormPtr->addRow(QString::fromUtf8("最大时延"), mMaximumDelayMsPtr);
   taskFormPtr->addRow(QString::fromUtf8("最低PDR"), mMinimumPdrPtr);
   assessmentLayoutPtr->addLayout(taskFormPtr);
   QPushButton* evaluateButtonPtr =
      new QPushButton(QString::fromUtf8("评估当前网络与候选网络"), assessmentPagePtr);
   assessmentLayoutPtr->addWidget(evaluateButtonPtr);
   mAssessmentResultPtr = new QTextEdit(assessmentPagePtr);
   mAssessmentResultPtr->setReadOnly(true);
   mAssessmentResultPtr->setPlainText(QString::fromUtf8("请选择通信任务的源平台、目的平台和约束条件，然后执行评估。"));
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
   mCapabilityAllowedNetworkPtr->addItems({QString::fromUtf8("全部网络"), "LINK11", "LINK16", "SATCOM", "CDL"});
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
   capabilityFormPtr->addRow(QString::fromUtf8("源平台"), mCapabilitySourceSelectorPtr);
   capabilityFormPtr->addRow(QString::fromUtf8("目的平台"), mCapabilityDestinationSelectorPtr);
   capabilityFormPtr->addRow(QString::fromUtf8("允许使用的网络"), mCapabilityAllowedNetworkPtr);
   capabilityFormPtr->addRow(QString::fromUtf8("所需带宽"), mCapabilityBandwidthKbpsPtr);
   capabilityFormPtr->addRow(QString::fromUtf8("最大时延（0表示不限制）"), mCapabilityMaximumDelayMsPtr);
   capabilityFormPtr->addRow(QString::fromUtf8("最低PDR"), mCapabilityMinimumPdrPtr);
   capabilityLayoutPtr->addLayout(capabilityFormPtr);
   QPushButton* capabilityButtonPtr = new QPushButton(QString::fromUtf8("查询通信能力"),
                                                      capabilityPagePtr);
   capabilityButtonPtr->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
   capabilityLayoutPtr->addWidget(capabilityButtonPtr);
   mCapabilityResultPtr = new QTextEdit(capabilityPagePtr);
   mCapabilityResultPtr->setReadOnly(true);
   mCapabilityResultPtr->setPlainText(QString::fromUtf8("请选择源平台和目的平台，然后查询当前快照中的通信能力。"));
   capabilityLayoutPtr->addWidget(mCapabilityResultPtr);
   connect(capabilityButtonPtr, &QPushButton::clicked, this, &DockWidget::QueryCapability);

   QWidget* planPagePtr = new QWidget(tabsPtr);
   mPlanPagePtr = planPagePtr;
   QVBoxLayout* planLayoutPtr = new QVBoxLayout(planPagePtr);
   mPlanSummaryPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   mPlanOperationPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   mPlanSummaryPtr->setWordWrap(true);
   mPlanOperationPtr->setWordWrap(true);
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
      {QString::fromUtf8("分配编号"), QString::fromUtf8("网络"), QString::fromUtf8("类型"),
       QString::fromUtf8("配置模板"), QString::fromUtf8("频率（Hz）"), QString::fromUtf8("信道"),
       QString::fromUtf8("子网"), QString::fromUtf8("时隙"), QString::fromUtf8("成员"),
       QString::fromUtf8("路由策略"), QString::fromUtf8("启用")}, planPagePtr);
   mPlanDemandTablePtr = CreateEditableTable(
      {QString::fromUtf8("需求编号"), QString::fromUtf8("业务类型"), QString::fromUtf8("源平台"),
       QString::fromUtf8("目的平台"), QString::fromUtf8("载荷（bit）"),
       QString::fromUtf8("带宽（bit/s）"), QString::fromUtf8("最大时延（ms）"),
       QString::fromUtf8("最低PDR（%）"), QString::fromUtf8("允许网络")},
      planPagePtr);

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
      {QString::fromUtf8("严重程度"), QString::fromUtf8("原因码"), QString::fromUtf8("字段"),
       QString::fromUtf8("记录"), QString::fromUtf8("说明")}, planPagePtr);
   mPlanEvaluationTablePtr = CreateTable(
      {QString::fromUtf8("需求编号"), QString::fromUtf8("状态"), QString::fromUtf8("并发结果"),
       QString::fromUtf8("冲突任务"), QString::fromUtf8("路径来源"),
       QString::fromUtf8("速率"), QString::fromUtf8("时延"), QString::fromUtf8("丢包率"),
       QString::fromUtf8("原因码")},
      planPagePtr);
   mPlanRecommendationTablePtr = CreateTable(
      {QString::fromUtf8("需求编号"), QString::fromUtf8("状态"),
       QString::fromUtf8("主要原因"), QString::fromUtf8("调整建议")},
      planPagePtr);
   mPlanRecommendationTablePtr->setColumnWidth(3, 620);
   mPlanDetailTabsPtr = new QTabWidget(planPagePtr);
   mPlanDetailTabsPtr->setDocumentMode(true);
   mPlanDetailTabsPtr->addTab(mPlanAllocationTablePtr, QString::fromUtf8("资源分配"));
   mPlanDetailTabsPtr->addTab(mPlanDemandTablePtr, QString::fromUtf8("业务需求"));
   mPlanDetailTabsPtr->addTab(mPlanIssueTablePtr, QString::fromUtf8("校验问题"));
   mPlanDetailTabsPtr->addTab(mPlanEvaluationTablePtr, QString::fromUtf8("推演结果"));
   mPlanDetailTabsPtr->addTab(mPlanRecommendationTablePtr, QString::fromUtf8("调整建议"));
   planLayoutPtr->addWidget(mPlanDetailTabsPtr, 1);

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
                 QString::fromUtf8("草案：表格编辑尚未写入规划仓库"));
           });
   connect(mPlanDemandTablePtr, &QTableWidget::itemChanged, this,
           [this](QTableWidgetItem*)
           {
              mPlanDirty = true;
              mPlanOperationPtr->setText(
                 QString::fromUtf8("草案：表格编辑尚未写入规划仓库"));
           });

   QWidget* demandPagePtr = new QWidget(tabsPtr);
   QVBoxLayout* demandLayoutPtr = new QVBoxLayout(demandPagePtr);
   mDemandSummaryPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   mDemandOperationPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   mDemandSummaryPtr->setWordWrap(true);
   mDemandOperationPtr->setWordWrap(true);
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
      {QString::fromUtf8("需求编号"), QString::fromUtf8("任务阶段"), QString::fromUtf8("业务类型"),
       QString::fromUtf8("源平台"), QString::fromUtf8("目的平台"), QString::fromUtf8("载荷（bit）"),
       QString::fromUtf8("业务流量（bit/s）"), QString::fromUtf8("带宽（bit/s）"),
       QString::fromUtf8("最大时延（ms）"), QString::fromUtf8("最低PDR（%）"),
       QString::fromUtf8("最大距离（m）"), QString::fromUtf8("最小网络规模"),
       QString::fromUtf8("允许网络")},
      demandPagePtr);
   demandLayoutPtr->addWidget(mDemandTablePtr);
   mDemandMatchTablePtr = CreateTable(
      {QString::fromUtf8("需求编号"), QString::fromUtf8("状态"), QString::fromUtf8("快照版本"),
       QString::fromUtf8("路径"), QString::fromUtf8("距离"), QString::fromUtf8("速率"),
       QString::fromUtf8("原因码")},
      demandPagePtr);
   demandLayoutPtr->addWidget(mDemandMatchTablePtr);
   mDemandGapTablePtr = CreateTable(
      {QString::fromUtf8("需求编号"), QString::fromUtf8("约束项"), QString::fromUtf8("要求值"),
       QString::fromUtf8("当前值"), QString::fromUtf8("裕量"), QString::fromUtf8("原因码")},
      demandPagePtr);
   demandLayoutPtr->addWidget(mDemandGapTablePtr);
   mDemandRecommendationTablePtr = CreateTable(
      {QString::fromUtf8("需求编号"), QString::fromUtf8("建议类型"), QString::fromUtf8("状态"),
       QString::fromUtf8("候选对象"), QString::fromUtf8("建议值"), QString::fromUtf8("排序"),
       QString::fromUtf8("来源 / 置信度"), QString::fromUtf8("原因码"), QString::fromUtf8("依据")},
      demandPagePtr);
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
                 QString::fromUtf8("草案：表格编辑尚未写入需求仓库，旧匹配结果已失效"));
              mDemandMatchTablePtr->setRowCount(0);
              mDemandGapTablePtr->setRowCount(0);
              mDemandRecommendationTablePtr->setRowCount(0);
           });

   QWidget* preacceptancePagePtr = new QWidget(tabsPtr);
   QVBoxLayout* preacceptanceLayoutPtr = new QVBoxLayout(preacceptancePagePtr);
   QLabel* preacceptanceIntroPtr = new QLabel(
      QString::fromUtf8(
         "本页只读显示最近一次命令行预验收结果；它不会从界面启动、停止或重启AFSIM。"
         "结果文件由 scripts/run_preacceptance.sh 原子更新，页面约每2秒自动刷新。"),
      preacceptancePagePtr);
   preacceptanceIntroPtr->setWordWrap(true);
   preacceptanceLayoutPtr->addWidget(preacceptanceIntroPtr);

   QFormLayout* preacceptanceFormPtr = new QFormLayout();
   preacceptanceFormPtr->addRow(QString::fromUtf8("总体结果"), mPreacceptanceStatusPtr);
   preacceptanceFormPtr->addRow(QString::fromUtf8("执行时间（UTC）"), mPreacceptanceTimePtr);
   preacceptanceFormPtr->addRow(QString::fromUtf8("检查项"), mPreacceptanceChecksPtr);
   preacceptanceFormPtr->addRow(QString::fromUtf8("固定测试"), mPreacceptanceTestsPtr);
   preacceptanceFormPtr->addRow(QString::fromUtf8("批准场景"), mPreacceptanceScenariosPtr);
   preacceptanceFormPtr->addRow(QString::fromUtf8("最终快照"), mPreacceptanceSnapshotPtr);
   preacceptanceFormPtr->addRow(QString::fromUtf8("Git修订"), mPreacceptanceRevisionPtr);
   preacceptanceFormPtr->addRow(QString::fromUtf8("报告路径"), mPreacceptanceReportPtr);
   preacceptanceLayoutPtr->addLayout(preacceptanceFormPtr);

   mPreacceptanceRevisionPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   mPreacceptanceSnapshotPtr->setWordWrap(true);
   mPreacceptanceReportPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   mPreacceptanceReportPtr->setWordWrap(true);
   mPreacceptanceNoticePtr->setWordWrap(true);
   mPreacceptanceNoticePtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   mPreacceptanceNoticePtr->setText(QString::fromUtf8(
      "Windows远程测试：在VNC中观察本页，同时在SSH终端执行\n"
      "cd /home/pyh/afsim/network_resource_manager && ./scripts/run_preacceptance.sh\n"
      "脚本会重启Warlock一次，重新连接后本页将显示“运行中（RUNNING）”，完成后更新为“通过（PASS）”或“失败（FAIL）”。"));
   preacceptanceLayoutPtr->addWidget(mPreacceptanceNoticePtr);
   preacceptanceLayoutPtr->addStretch();

   tabsPtr->addTab(mNetworkTablePtr, QString::fromUtf8("四网总览"));
   tabsPtr->addTab(mMetricsTablePtr, QString::fromUtf8("窗口指标"));
   tabsPtr->addTab(mEndpointTablePtr, QString::fromUtf8("网络成员"));
   tabsPtr->addTab(mLinkTablePtr, QString::fromUtf8("通信链路"));
   tabsPtr->addTab(mEnvironmentTablePtr, QString::fromUtf8("环境状态"));
   tabsPtr->addTab(mNavigationTablePtr, QString::fromUtf8("导航状态"));
   tabsPtr->addTab(assessmentPagePtr, QString::fromUtf8("任务评估"));
   tabsPtr->addTab(capabilityPagePtr, QString::fromUtf8("通信能力"));
   tabsPtr->addTab(planPagePtr, QString::fromUtf8("资源规划"));
   tabsPtr->addTab(demandPagePtr, QString::fromUtf8("需求匹配"));
   tabsPtr->addTab(preacceptancePagePtr, QString::fromUtf8("预验收状态"));
   tabsPtr->setCurrentWidget(assessmentPagePtr);
   rootLayoutPtr->addWidget(tabsPtr);

   setWidget(contentPtr);
   resize(720, 620);

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
   connect(mPreacceptanceMonitorPtr, &PreacceptanceStatusMonitor::StatusChanged,
           this, &DockWidget::RefreshPreacceptance);
   RefreshPreacceptance(PreacceptanceStatus());
   mPreacceptanceMonitorPtr->start();
   Refresh();
   RefreshNetworkPlan();
   RefreshResourceDemands();
}

void WkNrm::DockWidget::ShowNetworkPlan()
{
   if (mMainTabsPtr != nullptr && mPlanPagePtr != nullptr)
      mMainTabsPtr->setCurrentWidget(mPlanPagePtr);
   RefreshNetworkPlan();
}

void WkNrm::DockWidget::RefreshPreacceptance(const PreacceptanceStatus& aStatus)
{
   const QString statusCode = aStatus.overallStatus;
   QString statusText = statusCode;
   QString statusColor = "#7f8c8d";
   if (statusCode == "PASS")
   {
      statusColor = "#27ae60";
      statusText = QString::fromUtf8("通过（PASS）");
   }
   else if (statusCode == "FAIL")
   {
      statusColor = "#c0392b";
      statusText = QString::fromUtf8("失败（FAIL）");
   }
   else if (statusCode == "RUNNING")
   {
      statusColor = "#d68910";
      statusText = QString::fromUtf8("运行中（RUNNING）");
   }

   if (!aStatus.available)
   {
      statusText = QString::fromUtf8("尚无结果");
      mPreacceptanceTimePtr->setText(QString::fromUtf8("—"));
      mPreacceptanceChecksPtr->setText(QString::fromUtf8("—"));
      mPreacceptanceTestsPtr->setText(QString::fromUtf8("—"));
      mPreacceptanceScenariosPtr->setText(QString::fromUtf8("—"));
      mPreacceptanceSnapshotPtr->setText(QString::fromUtf8("—"));
      mPreacceptanceRevisionPtr->setText(QString::fromUtf8("—"));
      mPreacceptanceReportPtr->setText(aStatus.error.isEmpty() ? QString::fromUtf8("—")
                                                               : aStatus.error);
   }
   else
   {
      mPreacceptanceTimePtr->setText(aStatus.generatedAtUtc);
      mPreacceptanceChecksPtr->setText(
         QString("%1 / %2").arg(aStatus.passedChecks).arg(aStatus.completedChecks));
      mPreacceptanceTestsPtr->setText(
         QString("%1 / %2").arg(aStatus.passedTestCount).arg(aStatus.fixedTestCount));
      mPreacceptanceScenariosPtr->setText(
         QString("%1 / %2").arg(aStatus.passedScenarioCount).arg(aStatus.fixedScenarioCount));
      if (aStatus.guiSnapshotValid)
      {
         mPreacceptanceSnapshotPtr->setText(
            QString::fromUtf8("T=%1秒 | %2个网络 / %3个端点 / %4条链路 | 发送 %5 / 接收 %6 / 丢弃 %7 / 路由失败 %8")
               .arg(aStatus.simTime, 0, 'f', 1)
               .arg(aStatus.networkCount)
               .arg(aStatus.endpointCount)
               .arg(aStatus.linkCount)
               .arg(aStatus.transmitted)
               .arg(aStatus.received)
               .arg(aStatus.discarded)
               .arg(aStatus.routingFailed));
      }
      else
      {
         mPreacceptanceSnapshotPtr->setText(
            statusCode == "RUNNING" ? QString::fromUtf8("等待最终120秒快照")
                                     : QString::fromUtf8("最终快照无效或未生成"));
      }
      const QString revision = aStatus.branch.isEmpty()
                                  ? aStatus.revision
                                  : aStatus.branch + " @ " + aStatus.revision;
      mPreacceptanceRevisionPtr->setText(revision);
      mPreacceptanceReportPtr->setText(aStatus.reportPath);
   }
   mPreacceptanceStatusPtr->setText(statusText);
   mPreacceptanceStatusPtr->setStyleSheet(
      QString("font-weight: bold; color: %1;").arg(statusColor));
}

void WkNrm::DockWidget::EvaluateTask()
{
   nrm::AssessmentTask task;
   task.taskId              = "GUI-" + std::to_string(mData.GetSnapshot().snapshotVersion);
   task.sourcePlatform      = mSourceSelectorPtr->currentData().toString().toStdString();
   task.destinationPlatform = mDestinationSelectorPtr->currentData().toString().toStdString();
   if (task.sourcePlatform.empty() || task.destinationPlatform.empty())
   {
      mAssessmentResultPtr->setPlainText(QString::fromUtf8("请选择源平台和目的平台。"));
      return;
   }
   if (task.sourcePlatform == task.destinationPlatform)
   {
      mAssessmentResultPtr->setPlainText(QString::fromUtf8("源平台和目的平台不能相同。"));
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
      reasons.push_back(DisplayCode(nrm::ToString(reason)));
   }
   QStringList recommendations;
   for (const std::string& recommendation : result.recommendations)
   {
      recommendations.push_back(QString::fromStdString(recommendation));
   }

   QString text;
   text += QString::fromUtf8("快照：%1 @ %2秒\n")
              .arg(result.snapshotVersion)
              .arg(result.simTime, 0, 'f', 3);
   text += QString::fromUtf8("当前可达：%1\n当前可建链：%2\n业务可完成：%3\n通信稳定：%4\n")
              .arg(result.reachable ? QString::fromUtf8("是") : QString::fromUtf8("否"))
              .arg(result.canEstablish ? QString::fromUtf8("是") : QString::fromUtf8("否"))
              .arg(result.canComplete ? QString::fromUtf8("是") : QString::fromUtf8("否"))
              .arg(result.stable ? QString::fromUtf8("是") : QString::fromUtf8("否"));
   text += QString::fromUtf8("主路由：") + (route.isEmpty() ? QString::fromUtf8("—") : route.join(" → "));
   if (!route.isEmpty() && result.primaryRouteUsesCandidate)
   {
      text += QString::fromUtf8("  ［候选链路］");
   }
   text += "\n";
   text += QString::fromUtf8("备选路由：") +
           (backupRoute.isEmpty() ? QString::fromUtf8("—") : backupRoute.join(" → "));
   if (!backupRoute.isEmpty() && result.backupRouteUsesCandidate)
   {
      text += QString::fromUtf8("  ［候选链路］");
   }
   text += "\n";
   text += QString::fromUtf8("预测时延：") + MetricText(result.predictedDelayMs, 3) + "\n";
   text += QString::fromUtf8("估算PDR：") + MetricText(result.estimatedPdrPercent, 2) + "\n";
   text += QString::fromUtf8("瓶颈带宽：") + MetricText(result.bottleneckBandwidthBps, 1) + "\n";
   text += QString::fromUtf8("带宽裕量：") + MetricText(result.bandwidthMarginBps, 1) + "\n";
   text += QString::fromUtf8("时延裕量：") + MetricText(result.delayMarginMs, 3) + "\n";
   text += QString::fromUtf8("可靠性裕量：") + MetricText(result.reliabilityMarginPercent, 2) + "\n";
   text += QString::fromUtf8("原因码：") + (reasons.isEmpty() ? QString::fromUtf8("无") : reasons.join(", ")) + "\n";
   text += QString::fromUtf8("建议：") +
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
         QString::fromUtf8("请选择不同的源平台和目的平台。"));
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
      reasons.push_back(DisplayCode(nrm::ToString(reason)));
   }

   QString text;
   text += QString::fromUtf8("快照：%1 @ %2秒\n")
              .arg(result.snapshotVersion)
              .arg(result.simTime, 0, 'f', 3);
   text += QString::fromUtf8("请求有效：%1\n存在可用路径：%2\n路径来源：%3\n")
              .arg(result.requestValid ? QString::fromUtf8("是") : QString::fromUtf8("否"))
              .arg(result.pathAvailable ? QString::fromUtf8("是") : QString::fromUtf8("否"))
              .arg(result.usesCandidate ? QString::fromUtf8("参数化候选模型") : QString::fromUtf8("当前网络"));
   text += QString::fromUtf8("路由：") + (route.isEmpty() ? QString::fromUtf8("—") : route.join(" → ")) + "\n";
   text += QString::fromUtf8("通信距离：") + CapabilityMetricText(result.communicationDistanceM, 1) + "\n";
   text += QString::fromUtf8("最大单跳距离：") + CapabilityMetricText(result.maximumHopDistanceM, 1) + "\n";
   text += QString::fromUtf8("传输速率：") + CapabilityMetricText(result.transmissionRateBps, 1) + "\n";
   text += QString::fromUtf8("丢包率：") + CapabilityMetricText(result.packetLossPercent, 3) + "\n";
   text += QString::fromUtf8("传输时延：") + CapabilityMetricText(result.transmissionDelayMs, 3) + "\n";
   text += QString::fromUtf8("网络吞吐量：") + CapabilityMetricText(result.networkThroughputBps, 1) + "\n";
   text += QString::fromUtf8("接入率：") + CapabilityMetricText(result.accessRatioPercent, 2) + "\n";
   text += QString::fromUtf8("环境影响：\n");
   for (const nrm::EnvironmentEffect& effect : result.environmentEffects)
   {
      text += QString::fromUtf8("- %1：%2［来源=%3，置信度=%4，原因=%5］\n")
                 .arg(DisplayCode(nrm::ToString(effect.domain)),
                      effect.valid ? QString::fromUtf8("有效") : QString::fromUtf8("无效"),
                      DisplayCode(nrm::ToString(effect.origin)),
                      DisplayCode(nrm::ToString(effect.confidence)),
                      DisplayCode(nrm::ToString(effect.reason)));
   }
   text += QString::fromUtf8("原因码：") + (reasons.isEmpty() ? QString::fromUtf8("无") : reasons.join(", "));
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
      this, QString::fromUtf8("加载资源规划文件"), QString(),
      QString::fromUtf8("资源规划文件 (*.json *.nrm);;甲方JSON (*.json);;内部NRM规划 (*.nrm)"));
   if (path.isEmpty()) return;
   mPlanDirty = false;
   bool planLoaded = false;
   if (path.endsWith(".json", Qt::CaseInsensitive))
   {
      mData.LoadCustomerJson(path.toStdString());
      const CustomerJsonDecodeResult& result = mData.LastCustomerJsonResult();
      planLoaded = result.valid &&
                   result.envelope.schema == "nrm.customer.network_plan.v1" &&
                   mData.HasNetworkPlan();
      if (!result.valid && !result.errors.empty())
         mPlanOperationPtr->setText(QString::fromUtf8("甲方JSON加载失败：%1 %2")
            .arg(QString::fromStdString(result.errors.front().code),
                 QString::fromStdString(result.errors.front().path)));
   }
   else
   {
      planLoaded = mData.LoadNetworkPlan(path.toStdString());
   }

   if (planLoaded)
   {
      const QString scenario = BoundScenarioPath(path);
      const QString current = CurrentMissionPath();
      if (!scenario.isEmpty() && scenario != current)
      {
         const QString switchScript =
            QString::fromLocal8Bit(qgetenv("NRM_SOURCE")) +
            "/scripts/remote/switch-warlock-plan.sh";
         const bool started = QProcess::startDetached(
            "systemd-run",
            {"--quiet", "--user", "--collect",
             QString("--unit=nrm-warlock-plan-switch-%1")
                .arg(QCoreApplication::applicationPid()),
             switchScript, scenario, path});
         if (started)
         {
            mPlanOperationPtr->setText(
               QString::fromUtf8("规划绑定场景与当前场景不同，正在重启并自动恢复规划……"));
            return;
         }
         mPlanOperationPtr->setText(
            QString::fromUtf8("场景自动切换失败，请检查systemd临时单元和切换脚本权限。"));
         return;
      }
      RefreshNetworkPlan();
      if (scenario.isEmpty())
      {
         mPlanOperationPtr->setText(
            QString::fromUtf8("规划加载成功；该文件未绑定演示场景，继续使用当前AFSIM场景。"));
      }
      else if (scenario == current)
      {
         mPlanOperationPtr->setText(
            QString::fromUtf8("规划加载成功；当前已经是绑定场景，无需再次重启。"));
      }
      return;
   }
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
      mPlanOperationPtr->setText(QString::fromUtf8("当前没有可保存的规划［NO_CURRENT_PLAN］"));
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
   mPlanDetailTabsPtr->setCurrentIndex(2);
}

void WkNrm::DockWidget::EvaluateNetworkPlan()
{
   if (!ApplyNetworkPlanEdits()) return;
   mData.EvaluateNetworkPlan();
   RefreshNetworkPlan();
   mPlanDetailTabsPtr->setCurrentIndex(
      mData.GetPlanEvaluation().overallStatus == nrm::PlanEvaluationStatus::cPASS ? 3 : 4);
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
      mPlanOperationPtr->setText(QString::fromUtf8("当前没有可编辑的规划［NO_CURRENT_PLAN］"));
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
         mPlanOperationPtr->setText(QString::fromUtf8("解析错误：分配表的数值或启用字段无效［PARSE_ERROR］"));
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
         mPlanOperationPtr->setText(QString::fromUtf8("解析错误：需求表数值字段无效［PARSE_ERROR］"));
         return false;
      }
      draft.demands.push_back(demand);
   }

   const bool replaced = mData.ReplaceNetworkPlanDraft(draft);
   if (!replaced)
   {
      mPlanOperationPtr->setText(
         DisplayCode(nrm::ToString(mData.GetPlanOperation().reason)));
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
            : QString::fromUtf8("原因=%1，字段=%2")
                 .arg(DisplayCode(nrm::ToString(operation.reason)),
                      DisplayCode(operation.field)));
      mPlanAllocationTablePtr->setRowCount(0);
      mPlanDemandTablePtr->setRowCount(0);
      mPlanIssueTablePtr->setRowCount(0);
      mPlanEvaluationTablePtr->setRowCount(0);
      mPlanRecommendationTablePtr->setRowCount(0);
      return;
   }

   mPlanSummaryPtr->setText(
      QString::fromUtf8("规划编号=%1 | 修订=%2 | 状态=%3 | 来源=%4 | 置信度=%5 | 配置=%6")
         .arg(QString::fromStdString(planPtr->planId))
         .arg(planPtr->revision)
         .arg(DisplayCode(nrm::ToString(mData.GetNetworkPlanState())))
         .arg(DisplayCode(nrm::ToString(planPtr->source)))
         .arg(DisplayCode(nrm::ToString(planPtr->confidence)))
         .arg(QString::fromStdString(planPtr->configVersion)));

   QString operationText;
   if (mData.HasDistributionPackage())
   {
      const nrm::DistributionPackageResult& package = mData.GetDistributionPackage();
      operationText = package.generated
                         ? QString::fromUtf8("分发包=%1").arg(
                              QString::fromStdString(package.outputPath))
                         : QString::fromUtf8("原因=%1").arg(DisplayCode(nrm::ToString(package.reason)));
   }
   else
   {
      const nrm::PlanRepositoryResult& operation = mData.GetPlanOperation();
      operationText = operation.success
                         ? QString::fromUtf8("操作=成功，路径=%1").arg(
                              QString::fromStdString(operation.path))
                         : QString::fromUtf8("原因=%1").arg(DisplayCode(nrm::ToString(operation.reason)));
   }
   if (mData.HasPlanEvaluation())
   {
      const nrm::DegradationStatus& degradation =
         mData.GetConcurrentAssessment().degradation;
      const QString defaults = DisplayCodeList(degradation.usedDefaults);
      const QString missing = DisplayCodeList(degradation.missingFields);
      operationText += QString::fromUtf8(" | 降级等级=%1，数据覆盖=%2%，默认值=%3，缺失=%4")
         .arg(DisplayCode(nrm::ToString(degradation.level)))
         .arg(degradation.dataCoveragePercent, 0, 'f', 1)
         .arg(defaults.isEmpty() ? QString::fromUtf8("无") : defaults)
         .arg(missing.isEmpty() ? QString::fromUtf8("无") : missing);
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
         SetTableText(mPlanIssueTablePtr, row, 0, DisplayCode(nrm::ToString(issues[index].severity)));
         SetTableText(mPlanIssueTablePtr, row, 1, DisplayCode(nrm::ToString(issues[index].reason)));
         SetTableText(mPlanIssueTablePtr, row, 2,
                      DisplayCode(issues[index].field));
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
            reasons.push_back(DisplayCode(nrm::ToString(reason)));
         SetTableText(mPlanEvaluationTablePtr, row, 0,
                      QString::fromStdString(evaluation.demandId));
         SetTableText(mPlanEvaluationTablePtr, row, 1,
                      DisplayCode(nrm::ToString(evaluation.status)));
         const nrm::ConcurrentAssessmentResult& concurrent =
            mData.GetConcurrentAssessment();
         const nrm::ConcurrentTaskResult* concurrentPtr =
            index < concurrent.tasks.size() ? &concurrent.tasks[index] : nullptr;
         SetTableText(mPlanEvaluationTablePtr, row, 2,
                      concurrentPtr == nullptr ? QString::fromUtf8("未评估")
                      : concurrentPtr->allocated ? QString::fromUtf8("可并发")
                      : concurrentPtr->independent.canComplete
                           ? QString::fromUtf8("资源冲突")
                           : QString::fromUtf8("独立评估未通过"));
         QStringList conflicts;
         if (concurrentPtr != nullptr)
         {
            if (concurrentPtr->resourceReason != nrm::ConcurrentResourceReason::cNONE)
               conflicts.push_back(DisplayCode(nrm::ToString(concurrentPtr->resourceReason)));
            for (const std::string& taskId : concurrentPtr->conflictingTaskIds)
               conflicts.push_back(QString::fromStdString(taskId));
         }
         SetTableText(mPlanEvaluationTablePtr, row, 3,
                      conflicts.isEmpty() ? QString::fromUtf8("—") : conflicts.join(", "));
         SetTableText(mPlanEvaluationTablePtr, row, 4,
                      evaluation.capability.pathAvailable
                         ? (evaluation.capability.usesCandidate
                               ? QString::fromUtf8("参数化模型 / 低置信度")
                               : QString::fromUtf8("当前网络"))
                         : QString::fromUtf8("不可用"));
         SetTableText(mPlanEvaluationTablePtr, row, 5,
                      MetricText(evaluation.capability.transmissionRateBps, 1));
         SetTableText(mPlanEvaluationTablePtr, row, 6,
                      MetricText(evaluation.capability.transmissionDelayMs, 3));
         SetTableText(mPlanEvaluationTablePtr, row, 7,
                      MetricText(evaluation.capability.packetLossPercent, 3));
         SetTableText(mPlanEvaluationTablePtr, row, 8,
                      reasons.isEmpty() ? QString::fromUtf8("无") : reasons.join(", "));
      }
   }

   std::vector<const nrm::PlanDemandEvaluation*> recommendations;
   if (mData.HasPlanEvaluation())
   {
      for (const nrm::PlanDemandEvaluation& evaluation :
           mData.GetPlanEvaluation().demands)
      {
         if (!evaluation.recommendations.empty())
            recommendations.push_back(&evaluation);
      }
   }
   mPlanRecommendationTablePtr->setRowCount(
      static_cast<int>(recommendations.size()));
   for (std::size_t index = 0; index < recommendations.size(); ++index)
   {
      const int row = static_cast<int>(index);
      const nrm::PlanDemandEvaluation& evaluation = *recommendations[index];
      QStringList reasons;
      for (nrm::PlanValidationReason reason : evaluation.reasons)
         reasons.push_back(DisplayCode(nrm::ToString(reason)));
      QStringList advice;
      for (const std::string& recommendation : evaluation.recommendations)
         advice.push_back(QString::fromStdString(recommendation));
      SetTableText(mPlanRecommendationTablePtr, row, 0,
                   QString::fromStdString(evaluation.demandId));
      SetTableText(mPlanRecommendationTablePtr, row, 1,
                   DisplayCode(nrm::ToString(evaluation.status)));
      SetTableText(mPlanRecommendationTablePtr, row, 2,
                   reasons.isEmpty() ? QString::fromUtf8("无") : reasons.join(", "));
      SetTableText(mPlanRecommendationTablePtr, row, 3, advice.join("；"));
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
      mDemandOperationPtr->setText(QString::fromUtf8("当前没有可保存的需求集［NO_CURRENT_DEMAND_SET］"));
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
      mDemandOperationPtr->setText(QString::fromUtf8("当前没有可编辑的需求集［NO_CURRENT_DEMAND_SET］"));
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
         mDemandOperationPtr->setText(QString::fromUtf8("解析错误：需求数值或网络字段无效［PARSE_ERROR］"));
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
         QString::fromUtf8("原因=%1，字段=%2")
            .arg(DisplayCode(nrm::ToString(operation.reason)),
                 DisplayCode(operation.field)));
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
            : QString::fromUtf8("原因=%1，字段=%2")
                 .arg(DisplayCode(nrm::ToString(operation.reason)),
                      DisplayCode(operation.field)));
      mDemandTablePtr->setRowCount(0);
      mDemandMatchTablePtr->setRowCount(0);
      mDemandGapTablePtr->setRowCount(0);
      mDemandRecommendationTablePtr->setRowCount(0);
      return;
   }

   mDemandSummaryPtr->setText(
      QString::fromUtf8("需求集编号=%1 | 修订=%2 | 需求数=%3 | 来源=%4 | 置信度=%5 | 配置=%6")
         .arg(QString::fromStdString(setPtr->demandSetId))
         .arg(setPtr->revision)
         .arg(setPtr->demands.size())
         .arg(DisplayCode(nrm::ToString(setPtr->source)))
         .arg(DisplayCode(nrm::ToString(setPtr->confidence)))
         .arg(QString::fromStdString(setPtr->configVersion)));
   const nrm::ResourceDemandRepositoryResult& operation =
      mData.GetDemandOperation();
   mDemandOperationPtr->setText(
      operation.success
         ? QString::fromUtf8("操作=成功，路径=%1").arg(
              QString::fromStdString(operation.path))
         : QString::fromUtf8("原因=%1，字段=%2")
              .arg(DisplayCode(nrm::ToString(operation.reason)),
                   DisplayCode(operation.field)));

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
      QString::fromUtf8("匹配结果：总数=%1，满足=%2，不满足=%3，数据无效=%4，快照=%5")
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
         reasons.push_back(DisplayCode(nrm::ToString(reason)));
      SetTableText(mDemandMatchTablePtr, row, 0,
                   QString::fromStdString(result.demandId));
      SetTableText(mDemandMatchTablePtr, row, 1, DisplayCode(nrm::ToString(result.status)));
      SetTableText(mDemandMatchTablePtr, row, 2,
                   QString::number(result.snapshotVersion));
      SetTableText(mDemandMatchTablePtr, row, 3,
                   result.capability.pathAvailable ? QString::fromUtf8("可用") : QString::fromUtf8("不可用"));
      SetTableText(mDemandMatchTablePtr, row, 4,
                   MetricText(result.capability.communicationDistanceM, 1));
      SetTableText(mDemandMatchTablePtr, row, 5,
                   MetricText(result.capability.transmissionRateBps, 1));
      SetTableText(mDemandMatchTablePtr, row, 6,
                   reasons.isEmpty() ? QString::fromUtf8("无") : reasons.join(", "));

      for (const nrm::RequirementCheck& check : result.checks)
      {
         if (!check.applicable || (check.passed && check.currentValue.valid)) continue;
         SetTableText(mDemandGapTablePtr, gapRow, 0,
                      QString::fromStdString(result.demandId));
         SetTableText(mDemandGapTablePtr, gapRow, 1, DisplayCode(nrm::ToString(check.type)));
         SetTableText(mDemandGapTablePtr, gapRow, 2, MetricText(check.requiredValue, 3));
         SetTableText(mDemandGapTablePtr, gapRow, 3, MetricText(check.currentValue, 3));
         SetTableText(mDemandGapTablePtr, gapRow, 4, MetricText(check.margin, 3));
         SetTableText(mDemandGapTablePtr, gapRow, 5, DisplayCode(nrm::ToString(check.reason)));
         ++gapRow;
      }

      for (const nrm::PlanningRecommendation& recommendation : result.recommendations)
      {
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 0,
                      QString::fromStdString(recommendation.demandId));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 1,
                      DisplayCode(nrm::ToString(recommendation.type)));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 2,
                      DisplayCode(nrm::ToString(recommendation.status)));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 3,
                      QString::fromStdString(recommendation.candidateId));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 4,
                      QString::fromStdString(recommendation.value));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 5,
                      recommendation.rank == 0 ? QString::fromUtf8("—")
                                               : QString::number(recommendation.rank));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 6,
                      QString("%1 / %2")
                         .arg(DisplayCode(nrm::ToString(recommendation.source)),
                              DisplayCode(nrm::ToString(recommendation.confidence))));
         SetTableText(mDemandRecommendationTablePtr, recommendationRow, 7,
                      DisplayCode(nrm::ToString(recommendation.reason)));
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
   mStateValuePtr->setStyleSheet(
      snapshot.runtimeState == nrm::RuntimeState::cRUNNING
         ? "color: #39d98a;"
         : "color: #f2f7ff;");
   const QString reportingStatus = QString::fromStdString(mData.GetReportingStatus());
   mReportingValuePtr->setText(reportingStatus == "OK"
                                  ? QString::fromUtf8("正常（OK）")
                                  : reportingStatus);
   mReportingValuePtr->setStyleSheet(
      reportingStatus == "OK" ? "color: #39d98a;" : "color: #ffb454;");
   mSimTimeValuePtr->setText(QString::number(snapshot.simTime, 'f', 2) + " s");
   mNetworkCountValuePtr->setText(
      QString("%1 / %2").arg(snapshot.networks.size()).arg(snapshot.endpoints.size()));
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
      SetTableText(mNetworkTablePtr, row, 0, DisplayCode(nrm::ToString(network.networkType)));
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
         SetTableText(mMetricsTablePtr, metricsRow, 0, DisplayCode(nrm::ToString(network.networkType)));
         mMetricsTablePtr->item(metricsRow, 0)->setForeground(QBrush(NetworkColor(network.networkType)));
         SetTableText(mMetricsTablePtr, metricsRow, 1, QString::fromStdString(network.networkName));
         SetTableText(mMetricsTablePtr, metricsRow, 2, QString::number(window.windowS, 'f', 0) + " s");
         SetTableText(mMetricsTablePtr, metricsRow, 3, MetricText(window.throughputBps, 1));
         SetTableText(mMetricsTablePtr, metricsRow, 4, MetricText(window.pdrPercent, 1));
         SetTableText(mMetricsTablePtr, metricsRow, 5, MetricText(window.onlineRatioPercent, 1));
         SetTableText(mMetricsTablePtr, metricsRow, 6, MetricText(window.averageQueueDelayMs, 3));
         SetTableText(mMetricsTablePtr, metricsRow, 7, MetricText(window.averageTransportDelayMs, 3));
         SetTableText(mMetricsTablePtr, metricsRow, 8, MetricText(window.queueUtilizationPercent, 1));
         SetTableText(mMetricsTablePtr, metricsRow, 9, MetricText(window.ackDelayMs, 3));
         SetTableText(mMetricsTablePtr, metricsRow, 10, MetricText(window.rttMs, 3));
         ++metricsRow;
      }
   }

   mEndpointTablePtr->setRowCount(static_cast<int>(snapshot.endpoints.size()));
   for (std::size_t index = 0; index < snapshot.endpoints.size(); ++index)
   {
      const nrm::EndpointSnapshot& endpoint = snapshot.endpoints[index];
      const int row = static_cast<int>(index);
      SetTableText(mEndpointTablePtr, row, 0, DisplayCode(nrm::ToString(endpoint.networkType)));
      mEndpointTablePtr->item(row, 0)->setForeground(QBrush(NetworkColor(endpoint.networkType)));
      SetTableText(mEndpointTablePtr, row, 1, QString::fromStdString(endpoint.platformName));
      SetTableText(mEndpointTablePtr, row, 2, QString::fromStdString(endpoint.commName));
      SetTableText(mEndpointTablePtr, row, 3, QString::fromStdString(endpoint.address));
      SetTableText(mEndpointTablePtr, row, 4, QString::fromStdString(endpoint.memberRole));
      SetTableText(mEndpointTablePtr, row, 5, DisplayCode(nrm::ToString(endpoint.state)));
      SetTableText(mEndpointTablePtr, row, 6, MetricText(endpoint.latitudeDeg, 5));
      SetTableText(mEndpointTablePtr, row, 7, MetricText(endpoint.longitudeDeg, 5));
      SetTableText(mEndpointTablePtr, row, 8, MetricText(endpoint.altitudeM, 1));
   }

   mLinkTablePtr->setRowCount(static_cast<int>(snapshot.links.size()));
   for (std::size_t index = 0; index < snapshot.links.size(); ++index)
   {
      const nrm::LinkSnapshot& link = snapshot.links[index];
      const int row = static_cast<int>(index);
      SetTableText(mLinkTablePtr, row, 0, DisplayCode(nrm::ToString(link.networkType)));
      mLinkTablePtr->item(row, 0)->setForeground(QBrush(NetworkColor(link.networkType)));
      SetTableText(mLinkTablePtr, row, 1, QString::fromStdString(link.sourcePlatform));
      SetTableText(mLinkTablePtr, row, 2, QString::fromStdString(link.destinationPlatform));
      SetTableText(mLinkTablePtr, row, 3, DisplayCode(nrm::ToString(link.state)));
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

   const nrm::EnvironmentSnapshot& environment = snapshot.environment;
   mEnvironmentTablePtr->setRowCount(4);
   const QString provider = QString::fromStdString(environment.providerId);
   SetTableText(mEnvironmentTablePtr, 0, 0, QString::fromUtf8("地形"));
   SetTableText(mEnvironmentTablePtr, 0, 1,
                environment.terrain.available
                   ? (environment.terrain.enabled ? QString::fromUtf8("已启用")
                                                  : QString::fromUtf8("可用但未启用"))
                   : QString::fromUtf8("不可用"));
   SetTableText(mEnvironmentTablePtr, 0, 2, provider);
   SetTableText(mEnvironmentTablePtr, 0, 3,
                QString::fromUtf8("已检查 %1 条，遮挡 %2 条")
                   .arg(environment.terrain.evaluatedLinkCount)
                   .arg(environment.terrain.blockedLinkCount));
   SetTableText(mEnvironmentTablePtr, 0, 4,
                QString::fromUtf8("遮挡为硬约束；当前RF结果不重复修正"));

   SetTableText(mEnvironmentTablePtr, 1, 0, QString::fromUtf8("气象"));
   SetTableText(mEnvironmentTablePtr, 1, 1,
                environment.weather.available ? QString::fromUtf8("正常")
                                              : QString::fromUtf8("不可用"));
   SetTableText(mEnvironmentTablePtr, 1, 2, provider);
   SetTableText(mEnvironmentTablePtr, 1, 3,
                QString::fromUtf8("降雨 %1；风速 %2；云水密度 %3")
                   .arg(MetricText(environment.weather.rainRateMmPerHour, 2))
                   .arg(MetricText(environment.weather.windSpeedMps, 2))
                   .arg(MetricText(environment.weather.cloudWaterDensityKgPerM3, 6)));
   SetTableText(mEnvironmentTablePtr, 1, 4,
                QString::fromUtf8("候选链路采用低置信度参数化衰减"));

   SetTableText(mEnvironmentTablePtr, 2, 0, QString::fromUtf8("天象/时间"));
   SetTableText(mEnvironmentTablePtr, 2, 1,
                environment.celestial.available ? QString::fromUtf8("正常")
                                                : QString::fromUtf8("不可用"));
   SetTableText(mEnvironmentTablePtr, 2, 2, provider);
   SetTableText(mEnvironmentTablePtr, 2, 3,
                QString::fromUtf8("儒略日 %1；时间基准 %2")
                   .arg(MetricText(environment.celestial.julianDate, 6))
                   .arg(environment.celestial.usesSystemTime
                           ? QString::fromUtf8("系统时间")
                           : QString::fromUtf8("场景时间")));
   SetTableText(mEnvironmentTablePtr, 2, 4,
                QString::fromUtf8("为导航和后续日照计算提供统一时标"));

   SetTableText(mEnvironmentTablePtr, 3, 0, QString::fromUtf8("电磁干扰"));
   SetTableText(mEnvironmentTablePtr, 3, 1,
                environment.interference.available ? QString::fromUtf8("正常")
                                                   : QString::fromUtf8("不可用"));
   SetTableText(mEnvironmentTablePtr, 3, 2, provider);
   SetTableText(mEnvironmentTablePtr, 3, 3,
                QString::fromUtf8("观测链路 %1；最大功率 %2；最大影响 %3")
                   .arg(environment.interference.observedLinkCount)
                   .arg(MetricText(environment.interference.maximumPowerDbm, 2))
                   .arg(MetricText(environment.interference.maximumFactorPercent, 2)));
   SetTableText(mEnvironmentTablePtr, 3, 4,
                QString::fromUtf8("实测结果只作为证据；候选链路使用参数化影响"));

   const nrm::NavigationSnapshot& navigation = snapshot.navigation;
   mNavigationTablePtr->setRowCount(static_cast<int>(navigation.platforms.size()));
   for (std::size_t index = 0; index < navigation.platforms.size(); ++index)
   {
      const nrm::NavigationSample& sample = navigation.platforms[index];
      const int row = static_cast<int>(index);
      SetTableText(mNavigationTablePtr, row, 0, QString::fromStdString(sample.platformName));
      SetTableText(mNavigationTablePtr, row, 1, NavigationModeText(sample.mode));
      SetTableText(mNavigationTablePtr, row, 2, QString::fromStdString(sample.rawStatus));
      SetTableText(mNavigationTablePtr, row, 3, MetricText(sample.truthLatitudeDeg, 6));
      SetTableText(mNavigationTablePtr, row, 4, MetricText(sample.truthLongitudeDeg, 6));
      SetTableText(mNavigationTablePtr, row, 5, MetricText(sample.truthAltitudeM, 2));
      SetTableText(mNavigationTablePtr, row, 6, MetricText(sample.perceivedLatitudeDeg, 6));
      SetTableText(mNavigationTablePtr, row, 7, MetricText(sample.perceivedLongitudeDeg, 6));
      SetTableText(mNavigationTablePtr, row, 8, MetricText(sample.perceivedAltitudeM, 2));
      SetTableText(mNavigationTablePtr, row, 9, MetricText(sample.inTrackErrorM, 3));
      SetTableText(mNavigationTablePtr, row, 10, MetricText(sample.crossTrackErrorM, 3));
      SetTableText(mNavigationTablePtr, row, 11, MetricText(sample.verticalErrorM, 3));
      SetTableText(mNavigationTablePtr, row, 12, MetricText(sample.totalPositionErrorM, 3));
      SetTableText(mNavigationTablePtr, row, 13,
                   QString::number(sample.sampleTime, 'f', 3) + " s");
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
   itemPtr->setToolTip(aText);
}

QString WkNrm::DockWidget::RuntimeStateText(nrm::RuntimeState aState)
{
   switch (aState)
   {
   case nrm::RuntimeState::cINITIALIZING:
      return QString::fromUtf8("初始化中");
   case nrm::RuntimeState::cRUNNING:
      return QString::fromUtf8("运行中");
   case nrm::RuntimeState::cCOMPLETE:
      return QString::fromUtf8("已完成");
   case nrm::RuntimeState::cIDLE:
   default:
      return QString::fromUtf8("空闲");
   }
}
