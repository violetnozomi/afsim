#include "NrmDataContainer.hpp"

WkNrm::DataContainer::DataContainer(QObject* aParentPtr)
   : QObject(aParentPtr)
{
}

void WkNrm::DataContainer::SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot)
{
   mSnapshot = aSnapshot;
   emit SnapshotChanged();
}

