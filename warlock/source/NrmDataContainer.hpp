#ifndef NRM_DATA_CONTAINER_HPP
#define NRM_DATA_CONTAINER_HPP

#include <QObject>

#include "nrm/NetworkResourceTypes.hpp"

namespace WkNrm
{
class DataContainer : public QObject
{
   Q_OBJECT

public:
   explicit DataContainer(QObject* aParentPtr = nullptr);

   const nrm::FrameworkSnapshot& GetSnapshot() const { return mSnapshot; }
   void SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot);

signals:
   void SnapshotChanged();

private:
   nrm::FrameworkSnapshot mSnapshot;
};
} // namespace WkNrm

#endif

