#include "NrmPreacceptanceStatusMonitor.hpp"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace
{
qint64 JsonInteger(const QJsonObject& aObject, const QString& aName)
{
   return static_cast<qint64>(aObject.value(aName).toDouble());
}

WkNrm::PreacceptanceStatus ParseStatus(const QByteArray& aPayload)
{
   WkNrm::PreacceptanceStatus status;
   QJsonParseError parseError;
   const QJsonDocument document = QJsonDocument::fromJson(aPayload, &parseError);
   if (parseError.error != QJsonParseError::NoError || !document.isObject())
   {
      status.error = QString("Invalid status JSON: %1").arg(parseError.errorString());
      return status;
   }

   const QJsonObject root = document.object();
   if (root.value("schemaVersion").toString() != "nrm.preacceptance_status.v1")
   {
      status.error = "Unsupported pre-acceptance status schema.";
      return status;
   }

   status.available = true;
   status.overallStatus = root.value("overallStatus").toString("NOT_AVAILABLE");
   status.generatedAtUtc = root.value("generatedAtUtc").toString();
   status.branch = root.value("branch").toString();
   status.revision = root.value("revision").toString();
   status.reportPath = root.value("reportPath").toString();
   status.completedChecks = static_cast<int>(JsonInteger(root, "completedChecks"));
   status.passedChecks = static_cast<int>(JsonInteger(root, "passedChecks"));
   status.fixedTestCount = static_cast<int>(JsonInteger(root, "fixedTestCount"));
   status.passedTestCount = static_cast<int>(JsonInteger(root, "passedTestCount"));
   status.fixedScenarioCount = static_cast<int>(JsonInteger(root, "fixedScenarioCount"));
   status.passedScenarioCount = static_cast<int>(JsonInteger(root, "passedScenarioCount"));

   const QJsonObject snapshot = root.value("guiSnapshot").toObject();
   status.guiSnapshotValid = snapshot.value("valid").toBool(false);
   status.simTime = snapshot.value("simTime").toDouble();
   status.networkCount = static_cast<int>(JsonInteger(snapshot, "networkCount"));
   status.endpointCount = static_cast<int>(JsonInteger(snapshot, "endpointCount"));
   status.linkCount = static_cast<int>(JsonInteger(snapshot, "linkCount"));
   status.transmitted = JsonInteger(snapshot, "transmitted");
   status.received = JsonInteger(snapshot, "received");
   status.discarded = JsonInteger(snapshot, "discarded");
   status.routingFailed = JsonInteger(snapshot, "routingFailed");
   return status;
}
} // namespace

WkNrm::PreacceptanceStatusMonitor::PreacceptanceStatusMonitor(QObject* aParentPtr)
   : QThread(aParentPtr)
{
   QByteArray explicitStatusPath = qgetenv("NRM_PREACCEPTANCE_STATUS_FILE");
   if (!explicitStatusPath.isEmpty())
   {
      mStatusPath = QString::fromLocal8Bit(explicitStatusPath);
   }
   else
   {
      QByteArray outputDirectory = qgetenv("NRM_OUTPUT_DIR");
      if (outputDirectory.isEmpty()) outputDirectory = "/tmp/nrm-output";
      mStatusPath = QDir(QString::fromLocal8Bit(outputDirectory))
                       .filePath("preacceptance/latest_status.json");
   }
   qRegisterMetaType<WkNrm::PreacceptanceStatus>("WkNrm::PreacceptanceStatus");
}

WkNrm::PreacceptanceStatusMonitor::~PreacceptanceStatusMonitor()
{
   requestInterruption();
   wait();
}

void WkNrm::PreacceptanceStatusMonitor::run()
{
   QByteArray previousPayload;
   bool       emittedMissingStatus = false;
   while (!isInterruptionRequested())
   {
      QFile statusFile(mStatusPath);
      if (!statusFile.exists())
      {
         if (!emittedMissingStatus)
         {
            PreacceptanceStatus status;
            status.error = QString("No status file: %1").arg(mStatusPath);
            emit StatusChanged(status);
            emittedMissingStatus = true;
         }
      }
      else if (statusFile.open(QIODevice::ReadOnly))
      {
         const QByteArray payload = statusFile.readAll();
         if (payload != previousPayload)
         {
            emit StatusChanged(ParseStatus(payload));
            previousPayload = payload;
         }
         emittedMissingStatus = false;
      }

      for (int tick = 0; tick < 20 && !isInterruptionRequested(); ++tick)
      {
         msleep(100);
      }
   }
}
