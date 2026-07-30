#ifndef NRM_TACTICAL_VIEW_HPP
#define NRM_TACTICAL_VIEW_HPP

#include <QWidget>

class QLabel;

#include "NrmDataContainer.hpp"

namespace WkNrm
{
class TacticalView : public QWidget
{
   Q_OBJECT

public:
   explicit TacticalView(DataContainer& aData, QWidget* aParentPtr = nullptr);

protected:
   void paintEvent(QPaintEvent* aEventPtr) override;

private:
   DataContainer& mData;
};
} // namespace WkNrm

#endif
