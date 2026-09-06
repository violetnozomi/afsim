/**
 * @file NrmAcceptanceDemoPanel.cpp
 * @brief Fixed Chinese workflow for visual contract-acceptance recording.
 */

#include "NrmAcceptanceDemoPanel.hpp"

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QString>
#include <QVBoxLayout>

namespace
{
QLabel* AddSummary(QVBoxLayout* aLayoutPtr, const char* aObjectName, const QString& aInitialText)
{
   QLabel* labelPtr = new QLabel(aInitialText);
   labelPtr->setObjectName(aObjectName);
   labelPtr->setWordWrap(true);
   labelPtr->setTextInteractionFlags(Qt::TextSelectableByMouse);
   aLayoutPtr->addWidget(labelPtr);
   return labelPtr;
}

QPushButton* AddStep(QVBoxLayout*   aLayoutPtr,
                     int            aStep,
                     const QString& aTitle,
                     const QString& aDescription,
                     const char*    aObjectName)
{
   QFrame* framePtr = new QFrame;
   framePtr->setObjectName("AcceptanceStepCard");
   QVBoxLayout* cardLayoutPtr = new QVBoxLayout(framePtr);
   cardLayoutPtr->setContentsMargins(12, 10, 12, 10);
   cardLayoutPtr->setSpacing(5);

   QLabel* titlePtr = new QLabel(
      QString::fromUtf8("步骤%1　%2").arg(aStep).arg(aTitle), framePtr);
   titlePtr->setObjectName("AcceptanceStepTitle");
   titlePtr->setWordWrap(true);
   cardLayoutPtr->addWidget(titlePtr);

   QLabel* descriptionPtr = new QLabel(aDescription, framePtr);
   descriptionPtr->setObjectName("AcceptanceStepDescription");
   descriptionPtr->setWordWrap(true);
   cardLayoutPtr->addWidget(descriptionPtr);

   QPushButton* buttonPtr = new QPushButton(aTitle, framePtr);
   buttonPtr->setObjectName(aObjectName);
   buttonPtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
   cardLayoutPtr->addWidget(buttonPtr);

   aLayoutPtr->addWidget(framePtr);
   return buttonPtr;
}
}

namespace WkNrm
{
QString AcceptanceDemoPanel::FixedScenarioPath()
{
   return QString::fromLatin1("test_mission/operational_strike_demo/interactive.txt");
}

QString AcceptanceDemoPanel::FixedPlanPath()
{
   return QString::fromLatin1("data/network_plans/contract_acceptance_37node-r1.nrm");
}

QString AcceptanceDemoPanel::FixedPlanId()
{
   return QString::fromLatin1("contract-acceptance-37node");
}

int AcceptanceDemoPanel::FixedPlanRevision()
{
   return 1;
}

QString AcceptanceDemoPanel::FixedSwitchScriptPath()
{
   return QString::fromLatin1("scripts/remote/switch-warlock-plan.sh");
}

QString AcceptanceDemoPanel::FixedSourcePlatform()
{
   return QString::fromLatin1("airborne_relay");
}

QString AcceptanceDemoPanel::FixedDestinationPlatform()
{
   return QString::fromLatin1("gw_l11_l16_reverse");
}

QString AcceptanceDemoPanel::FixedNetwork()
{
   return QString::fromLatin1("LINK16");
}

double AcceptanceDemoPanel::FixedBandwidthKbps()
{
   return 0.0;
}

double AcceptanceDemoPanel::FixedMaximumDelayMs()
{
   return 1000.0;
}

double AcceptanceDemoPanel::FixedMinimumPdrPercent()
{
   return 0.0;
}

AcceptanceDemoPanel::AcceptanceDemoPanel(QWidget* aParentPtr)
   : QWidget(aParentPtr)
   , mResourceSummaryPtr(nullptr)
   , mNavigationSummaryPtr(nullptr)
   , mEnvironmentSummaryPtr(nullptr)
   , mPreacceptanceSummaryPtr(nullptr)
   , mLaunchResultPtr(nullptr)
{
   setObjectName("NrmAcceptanceDemoPanel");
   setMinimumSize(0, 0);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

   QVBoxLayout* outerLayoutPtr = new QVBoxLayout(this);
   outerLayoutPtr->setContentsMargins(0, 0, 0, 0);

   QScrollArea* scrollAreaPtr = new QScrollArea(this);
   scrollAreaPtr->setObjectName("acceptanceScrollArea");
   scrollAreaPtr->setWidgetResizable(true);
   scrollAreaPtr->setFrameShape(QFrame::NoFrame);
   scrollAreaPtr->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   scrollAreaPtr->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   outerLayoutPtr->addWidget(scrollAreaPtr);

   QWidget* pagePtr = new QWidget(scrollAreaPtr);
   pagePtr->setObjectName("acceptanceScrollPage");
   pagePtr->setMinimumSize(0, 0);
   QVBoxLayout* pageLayoutPtr = new QVBoxLayout(pagePtr);
   pageLayoutPtr->setContentsMargins(12, 12, 12, 16);
   pageLayoutPtr->setSpacing(9);

   QLabel* titlePtr = new QLabel(QString::fromUtf8("合同验收演示"), pagePtr);
   titlePtr->setObjectName("acceptanceTitle");
   pageLayoutPtr->addWidget(titlePtr);

   QLabel* subtitlePtr = new QLabel(
      QString::fromUtf8("按步骤完成四网资源、评估规划、跨域网关、导航环境和预验收证据展示。"),
      pagePtr);
   subtitlePtr->setObjectName("acceptanceSubtitle");
   subtitlePtr->setWordWrap(true);
   pageLayoutPtr->addWidget(subtitlePtr);

   QFrame* summaryFramePtr = new QFrame(pagePtr);
   summaryFramePtr->setObjectName("AcceptanceSummaryCard");
   QVBoxLayout* summaryLayoutPtr = new QVBoxLayout(summaryFramePtr);
   summaryLayoutPtr->setContentsMargins(12, 10, 12, 10);
   summaryLayoutPtr->setSpacing(4);
   mResourceSummaryPtr = AddSummary(summaryLayoutPtr,
                                    "acceptanceResourceSummary",
                                    QString::fromUtf8("等待综合场景资源快照"));
   mNavigationSummaryPtr = AddSummary(summaryLayoutPtr,
                                      "acceptanceNavigationSummary",
                                      QString::fromUtf8("等待导航数据"));
   mEnvironmentSummaryPtr = AddSummary(summaryLayoutPtr,
                                       "acceptanceEnvironmentSummary",
                                       QString::fromUtf8("等待自然与电磁环境数据"));
   mPreacceptanceSummaryPtr = AddSummary(summaryLayoutPtr,
                                         "acceptancePreacceptanceSummary",
                                         QString::fromUtf8("等待预验收结果"));
   mLaunchResultPtr = AddSummary(summaryLayoutPtr,
                                 "acceptanceLaunchResult",
                                 QString::fromUtf8("尚未启动固定综合场景"));
   pageLayoutPtr->addWidget(summaryFramePtr);

   QPushButton* startPtr = AddStep(
      pageLayoutPtr,
      1,
      QString::fromUtf8("启动固定综合场景"),
      QString::fromUtf8("自动加载37个协作平台（含12个跨域网关）和四类网络的固定验收场景。"),
      "acceptanceStartScenario");
   QPushButton* overviewPtr = AddStep(
      pageLayoutPtr,
      2,
      QString::fromUtf8("查看四网资源总览"),
      QString::fromUtf8("核对Link-11、Link-16、卫通、CDL的成员、链路和实时窗口指标。"),
      "acceptanceShowOverview");
   QPushButton* assessmentPtr = AddStep(
      pageLayoutPtr,
      3,
      QString::fromUtf8("执行固定任务评估"),
      QString::fromUtf8("自动选择固定源、目的和Link-16约束，展示可达性、差距与建议。"),
      "acceptanceRunAssessment");
   QPushButton* capabilityPtr = AddStep(
      pageLayoutPtr,
      4,
      QString::fromUtf8("查询固定通信能力"),
      QString::fromUtf8("复用同一组平台条件，展示当前链路能力与满足情况。"),
      "acceptanceRunCapability");
   QPushButton* planPtr = AddStep(
      pageLayoutPtr,
      5,
      QString::fromUtf8("查看资源规划推演"),
      QString::fromUtf8("查看并发需求、冲突、数值缺口和中文规划建议；被拒绝也可证明约束识别。"),
      "acceptanceShowPlan");
   QPushButton* gatewayPtr = AddStep(
      pageLayoutPtr,
      6,
      QString::fromUtf8("查看跨域网关链路"),
      QString::fromUtf8("核对多网平台、网关能力及跨网转发链路，不触发自动控制。"),
      "acceptanceShowGateway");
   QPushButton* navigationPtr = AddStep(
      pageLayoutPtr,
      7,
      QString::fromUtf8("查看导航状态"),
      QString::fromUtf8("展示AFSIM导航结果包解析后的真实位置、感知位置和误差字段。"),
      "acceptanceShowNavigation");
   QPushButton* environmentPtr = AddStep(
      pageLayoutPtr,
      8,
      QString::fromUtf8("查看环境状态"),
      QString::fromUtf8("展示地形、气象和电磁干扰输入及其有效性。"),
      "acceptanceShowEnvironment");
   QPushButton* preacceptancePtr = AddStep(
      pageLayoutPtr,
      9,
      QString::fromUtf8("查看预验收证据"),
      QString::fromUtf8("查看固定测试、场景验证、快照与报告状态。"),
      "acceptanceShowPreacceptance");

   connect(startPtr, &QPushButton::clicked, this, &AcceptanceDemoPanel::StartScenarioRequested);
   connect(overviewPtr, &QPushButton::clicked, this, &AcceptanceDemoPanel::ShowOverviewRequested);
   connect(assessmentPtr, &QPushButton::clicked, this, &AcceptanceDemoPanel::RunAssessmentRequested);
   connect(capabilityPtr, &QPushButton::clicked, this, &AcceptanceDemoPanel::RunCapabilityRequested);
   connect(planPtr, &QPushButton::clicked, this, &AcceptanceDemoPanel::ShowPlanRequested);
   connect(gatewayPtr, &QPushButton::clicked, this, &AcceptanceDemoPanel::ShowGatewayRequested);
   connect(navigationPtr, &QPushButton::clicked, this, &AcceptanceDemoPanel::ShowNavigationRequested);
   connect(environmentPtr, &QPushButton::clicked, this, &AcceptanceDemoPanel::ShowEnvironmentRequested);
   connect(preacceptancePtr,
           &QPushButton::clicked,
           this,
           &AcceptanceDemoPanel::ShowPreacceptanceRequested);

   pageLayoutPtr->addStretch(1);
   scrollAreaPtr->setWidget(pagePtr);
}

void AcceptanceDemoPanel::SetResourceSummary(int aNetworks,
                                             int aPlatforms,
                                             int aLinks,
                                             int aGateways)
{
   mResourceSummaryPtr->setText(
      QString::fromUtf8("%1个网络 · %2个平台 · %3条链路 · %4个跨域网关")
         .arg(aNetworks)
         .arg(aPlatforms)
         .arg(aLinks)
         .arg(aGateways));
}

void AcceptanceDemoPanel::SetNavigationSummary(int aPlatforms)
{
   mNavigationSummaryPtr->setText(
      QString::fromUtf8("%1个平台具有导航数据").arg(aPlatforms));
}

void AcceptanceDemoPanel::SetEnvironmentSummary(bool aTerrainAvailable,
                                                 bool aWeatherAvailable,
                                                 bool aElectromagneticAvailable)
{
   const QString terrain = aTerrainAvailable ? QString::fromUtf8("地形可用")
                                             : QString::fromUtf8("地形无有效数据");
   const QString weather = aWeatherAvailable ? QString::fromUtf8("气象可用")
                                             : QString::fromUtf8("气象无有效数据");
   const QString electromagnetic = aElectromagneticAvailable
                                      ? QString::fromUtf8("电磁环境可用")
                                      : QString::fromUtf8("电磁环境无有效数据");
   mEnvironmentSummaryPtr->setText(
      QString::fromUtf8("%1 · %2 · %3").arg(terrain, weather, electromagnetic));
}

void AcceptanceDemoPanel::SetPreacceptanceSummary(const QString& aStatus,
                                                   int            aPassedTests,
                                                   int            aTotalTests,
                                                   int            aPassedScenarios,
                                                   int            aTotalScenarios)
{
   mPreacceptanceSummaryPtr->setText(
      QString::fromUtf8("%1 · 固定测试%2/%3 · 场景%4/%5")
         .arg(aStatus)
         .arg(aPassedTests)
         .arg(aTotalTests)
         .arg(aPassedScenarios)
         .arg(aTotalScenarios));
}

void AcceptanceDemoPanel::SetLaunchResult(bool aSuccess, const QString& aDetail)
{
   const QString state = aSuccess ? QString::fromUtf8("操作成功")
                                  : QString::fromUtf8("启动失败");
   mLaunchResultPtr->setText(
      aDetail.isEmpty() ? state : QString::fromUtf8("%1：%2").arg(state, aDetail));
}
} // namespace WkNrm
