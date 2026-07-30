#ifndef NRM_DATA_CONTAINER_HPP
#define NRM_DATA_CONTAINER_HPP

#include <memory>

#include <QObject>

#include "nrm/NetworkResourceTypes.hpp"

namespace WkNrm
{
class SnapshotReporter;

class DataContainer : public QObject
{
   Q_OBJECT

public:
   explicit DataContainer(QObject* aParentPtr = nullptr);
   ~DataContainer() override;

   const nrm::FrameworkSnapshot& GetSnapshot() const { return mSnapshot; }
   void SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot);

signals:
   void SnapshotChanged();

private:
   nrm::FrameworkSnapshot            mSnapshot;
   std::unique_ptr<SnapshotReporter> mReporterPtr;
};
} // namespace WkNrm

#endif
