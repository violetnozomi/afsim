/**
 * @file AcceptanceDemoPanelTest.cpp
 * @brief Verifies the fixed, Chinese Warlock contract-acceptance workflow.
 */

#include "NrmAcceptanceDemoPanel.hpp"

#include <cassert>

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace
{
QPushButton* RequiredButton(WkNrm::AcceptanceDemoPanel& aPanel, const char* aObjectName)
{
   QPushButton* buttonPtr = aPanel.findChild<QPushButton*>(aObjectName);
   assert(buttonPtr != nullptr);
   assert(!buttonPtr->text().isEmpty());
   return buttonPtr;
}

QLabel* RequiredLabel(WkNrm::AcceptanceDemoPanel& aPanel, const char* aObjectName)
{
   QLabel* labelPtr = aPanel.findChild<QLabel*>(aObjectName);
   assert(labelPtr != nullptr);
   return labelPtr;
}
}

int main(int argc, char** argv)
{
   qputenv("QT_QPA_PLATFORM", "offscreen");
   QApplication application(argc, argv);

   WkNrm::AcceptanceDemoPanel panel;
   panel.ensurePolished();

   assert(WkNrm::AcceptanceDemoPanel::FixedScenarioPath() ==
          QString::fromLatin1("test_mission/operational_strike_demo/interactive.txt"));
   assert(WkNrm::AcceptanceDemoPanel::FixedPlanPath() ==
          QString::fromLatin1("data/network_plans/contract_acceptance_37node-r1.nrm"));
   assert(WkNrm::AcceptanceDemoPanel::FixedPlanId() ==
          QString::fromLatin1("contract-acceptance-37node"));
   assert(WkNrm::AcceptanceDemoPanel::FixedPlanRevision() == 1);
   assert(WkNrm::AcceptanceDemoPanel::FixedSwitchScriptPath() ==
          QString::fromLatin1("scripts/remote/switch-warlock-plan.sh"));
   assert(WkNrm::AcceptanceDemoPanel::FixedSourcePlatform() ==
          QString::fromLatin1("airborne_relay"));
   assert(WkNrm::AcceptanceDemoPanel::FixedDestinationPlatform() ==
          QString::fromLatin1("gw_l11_l16_reverse"));
   assert(WkNrm::AcceptanceDemoPanel::FixedNetwork() ==
          QString::fromLatin1("LINK16"));
   assert(WkNrm::AcceptanceDemoPanel::FixedBandwidthKbps() == 0.0);
   assert(WkNrm::AcceptanceDemoPanel::FixedMaximumDelayMs() == 1000.0);
   assert(WkNrm::AcceptanceDemoPanel::FixedMinimumPdrPercent() == 0.0);

   assert(panel.minimumWidth() == 0);
   assert(panel.minimumHeight() == 0);
   assert(panel.findChildren<QLineEdit*>().isEmpty());
   assert(RequiredLabel(panel, "acceptanceTitle")->text() ==
          QString::fromUtf8("合同验收演示"));
   const QList<QLabel*> stepDescriptions =
      panel.findChildren<QLabel*>("AcceptanceStepDescription");
   assert(!stepDescriptions.isEmpty());
   assert(stepDescriptions.front()->text().contains(QString::fromUtf8("37个协作平台")));

   int startCount         = 0;
   int overviewCount      = 0;
   int assessmentCount    = 0;
   int capabilityCount    = 0;
   int planCount          = 0;
   int gatewayCount       = 0;
   int navigationCount    = 0;
   int environmentCount   = 0;
   int preacceptanceCount = 0;

   QObject::connect(&panel,
                    &WkNrm::AcceptanceDemoPanel::StartScenarioRequested,
                    [&startCount]() { ++startCount; });
   QObject::connect(&panel,
                    &WkNrm::AcceptanceDemoPanel::ShowOverviewRequested,
                    [&overviewCount]() { ++overviewCount; });
   QObject::connect(&panel,
                    &WkNrm::AcceptanceDemoPanel::RunAssessmentRequested,
                    [&assessmentCount]() { ++assessmentCount; });
   QObject::connect(&panel,
                    &WkNrm::AcceptanceDemoPanel::RunCapabilityRequested,
                    [&capabilityCount]() { ++capabilityCount; });
   QObject::connect(&panel,
                    &WkNrm::AcceptanceDemoPanel::ShowPlanRequested,
                    [&planCount]() { ++planCount; });
   QObject::connect(&panel,
                    &WkNrm::AcceptanceDemoPanel::ShowGatewayRequested,
                    [&gatewayCount]() { ++gatewayCount; });
   QObject::connect(&panel,
                    &WkNrm::AcceptanceDemoPanel::ShowNavigationRequested,
                    [&navigationCount]() { ++navigationCount; });
   QObject::connect(&panel,
                    &WkNrm::AcceptanceDemoPanel::ShowEnvironmentRequested,
                    [&environmentCount]() { ++environmentCount; });
   QObject::connect(&panel,
                    &WkNrm::AcceptanceDemoPanel::ShowPreacceptanceRequested,
                    [&preacceptanceCount]() { ++preacceptanceCount; });

   RequiredButton(panel, "acceptanceStartScenario")->click();
   RequiredButton(panel, "acceptanceShowOverview")->click();
   RequiredButton(panel, "acceptanceRunAssessment")->click();
   RequiredButton(panel, "acceptanceRunCapability")->click();
   RequiredButton(panel, "acceptanceShowPlan")->click();
   RequiredButton(panel, "acceptanceShowGateway")->click();
   RequiredButton(panel, "acceptanceShowNavigation")->click();
   RequiredButton(panel, "acceptanceShowEnvironment")->click();
   RequiredButton(panel, "acceptanceShowPreacceptance")->click();

   assert(startCount == 1);
   assert(overviewCount == 1);
   assert(assessmentCount == 1);
   assert(capabilityCount == 1);
   assert(planCount == 1);
   assert(gatewayCount == 1);
   assert(navigationCount == 1);
   assert(environmentCount == 1);
   assert(preacceptanceCount == 1);

   panel.SetResourceSummary(4, 37, 108, 12);
   panel.SetNavigationSummary(11);
   panel.SetEnvironmentSummary(true, true, true);
   panel.SetPreacceptanceSummary(QString::fromUtf8("通过"), 43, 43, 10, 10);
   panel.SetLaunchResult(false, QString::fromUtf8("未找到固定场景"));

   assert(RequiredLabel(panel, "acceptanceResourceSummary")->text() ==
          QString::fromUtf8("4个网络 · 37个平台 · 108条链路 · 12个跨域网关"));
   assert(RequiredLabel(panel, "acceptanceNavigationSummary")->text() ==
          QString::fromUtf8("11个平台具有导航数据"));
   assert(RequiredLabel(panel, "acceptanceEnvironmentSummary")->text() ==
          QString::fromUtf8("地形可用 · 气象可用 · 电磁环境可用"));
   assert(RequiredLabel(panel, "acceptancePreacceptanceSummary")->text() ==
          QString::fromUtf8("通过 · 固定测试43/43 · 场景10/10"));
   assert(RequiredLabel(panel, "acceptanceLaunchResult")->text().contains(
      QString::fromUtf8("启动失败")));
   assert(RequiredLabel(panel, "acceptanceLaunchResult")->text().contains(
      QString::fromUtf8("未找到固定场景")));

   return 0;
}
