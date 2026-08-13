#ifndef NRM_PREACCEPTANCE_STATUS_MONITOR_HPP
#define NRM_PREACCEPTANCE_STATUS_MONITOR_HPP

#include <QMetaType>
#include <QString>
#include <QThread>

namespace WkNrm
{
struct PreacceptanceStatus
{
   bool    available = false;
   QString overallStatus = "NOT_AVAILABLE";
   QString generatedAtUtc;
   QString branch;
   QString revision;
   QString reportPath;
   QString error;
   int     completedChecks = 0;
   int     passedChecks = 0;
   int     fixedTestCount = 0;
   int     passedTestCount = 0;
   int     fixedScenarioCount = 0;
   int     passedScenarioCount = 0;
   bool    guiSnapshotValid = false;
   double  simTime = 0.0;
   int     networkCount = 0;
   int     endpointCount = 0;
   int     linkCount = 0;
   qint64  transmitted = 0;
   qint64  received = 0;
   qint64  discarded = 0;
   qint64  routingFailed = 0;
};

// Reads the small pre-acceptance status file on a worker thread.  This keeps
// file I/O and JSON parsing out of both the AFSIM simulation callbacks and the
// Warlock GUI thread.
class PreacceptanceStatusMonitor : public QThread
{
   Q_OBJECT

public:
   explicit PreacceptanceStatusMonitor(QObject* aParentPtr = nullptr);
   ~PreacceptanceStatusMonitor() override;

signals:
   void StatusChanged(const WkNrm::PreacceptanceStatus& aStatus);

protected:
   void run() override;

private:
   QString mStatusPath;
};
} // namespace WkNrm

Q_DECLARE_METATYPE(WkNrm::PreacceptanceStatus)

#endif
