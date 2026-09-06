#ifndef NRM_ACCEPTANCE_DEMO_PANEL_HPP
#define NRM_ACCEPTANCE_DEMO_PANEL_HPP

#include <QWidget>

class QLabel;
class QString;

namespace WkNrm
{
/**
 * Presents a fixed, front-end-only workflow for recording contract acceptance.
 *
 * The panel does not accept paths, commands or protocol data.  It emits only
 * high-level user intents; DockWidget keeps ownership of the existing NRM
 * evaluation, planning and scenario-switch operations.
 */
class AcceptanceDemoPanel : public QWidget
{
   Q_OBJECT

public:
   explicit AcceptanceDemoPanel(QWidget* aParentPtr = nullptr);

   static QString FixedScenarioPath();
   static QString FixedPlanPath();
   static QString FixedPlanId();
   static int FixedPlanRevision();
   static QString FixedSwitchScriptPath();
   static QString FixedSourcePlatform();
   static QString FixedDestinationPlatform();
   static QString FixedNetwork();
   static double FixedBandwidthKbps();
   static double FixedMaximumDelayMs();
   static double FixedMinimumPdrPercent();

   void SetResourceSummary(int aNetworks, int aPlatforms, int aLinks, int aGateways);
   void SetNavigationSummary(int aPlatforms);
   void SetEnvironmentSummary(bool aTerrainAvailable,
                              bool aWeatherAvailable,
                              bool aElectromagneticAvailable);
   void SetPreacceptanceSummary(const QString& aStatus,
                                int            aPassedTests,
                                int            aTotalTests,
                                int            aPassedScenarios,
                                int            aTotalScenarios);
   void SetLaunchResult(bool aSuccess, const QString& aDetail);

signals:
   void StartScenarioRequested();
   void ShowOverviewRequested();
   void RunAssessmentRequested();
   void RunCapabilityRequested();
   void ShowPlanRequested();
   void ShowGatewayRequested();
   void ShowNavigationRequested();
   void ShowEnvironmentRequested();
   void ShowPreacceptanceRequested();

private:
   QLabel* mResourceSummaryPtr;
   QLabel* mNavigationSummaryPtr;
   QLabel* mEnvironmentSummaryPtr;
   QLabel* mPreacceptanceSummaryPtr;
   QLabel* mLaunchResultPtr;
};
} // namespace WkNrm

#endif
