#include "NrmDataContainer.hpp"

#include <QByteArray>
#include <QDir>

#include "NrmSnapshotReporter.hpp"

WkNrm::DataContainer::DataContainer(QObject* aParentPtr)
   : QObject(aParentPtr)
{
   QByteArray outputDirectory = qgetenv("NRM_OUTPUT_DIR");
   if (outputDirectory.isEmpty())
   {
      outputDirectory = "/tmp/nrm-output";
   }
   QDir().mkpath(QString::fromLocal8Bit(outputDirectory));
   mReporterPtr.reset(new SnapshotReporter(outputDirectory.constData()));
}

WkNrm::DataContainer::~DataContainer() = default;

void WkNrm::DataContainer::SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot)
{
   mSnapshot = aSnapshot;
   mReporterPtr->Enqueue(mSnapshot);
   emit SnapshotChanged();
}
