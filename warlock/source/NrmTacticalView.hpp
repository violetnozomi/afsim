#ifndef NRM_TACTICAL_VIEW_HPP
#define NRM_TACTICAL_VIEW_HPP

#include <cstdint>

#include <QTimer>
#include <QWidget>

#include "NrmDataContainer.hpp"
#include "NrmTacticalSelection.hpp"
#include "NrmTacticalViewport.hpp"

namespace WkNrm
{
class TacticalView : public QWidget
{
   Q_OBJECT

public:
   explicit TacticalView(DataContainer& aData, QWidget* aParentPtr = nullptr);

public slots:
   void BeginAssessmentSelection(int aTarget);
   void SetAssessmentPlatforms(const QString& aSourcePlatform,
                               const QString& aDestinationPlatform);
   void SetUiScalePercent(int aPercent);

signals:
   void PlatformSelected(const QString& aPlatformName, int aAssignment);
   void SelectionTargetChanged(int aTarget);
   void UiScalePercentChanged(int aPercent);

protected:
   void paintEvent(QPaintEvent* aEventPtr) override;
   void mousePressEvent(QMouseEvent* aEventPtr) override;
   void mouseMoveEvent(QMouseEvent* aEventPtr) override;
   void mouseReleaseEvent(QMouseEvent* aEventPtr) override;
   void wheelEvent(QWheelEvent* aEventPtr) override;

private:
   void HandlePlatformClick(const QPointF& aViewPosition);

   DataContainer&                mData;
   TacticalSelectionState        mSelectionState;
   TacticalViewport              mViewport;
   std::vector<PlatformHitRegion> mHitRegions;
   QRectF                         mPlotRect;
   QRectF                         mDetailsRect;
   QPointF                        mPressPosition;
   QPointF                        mLastDragPosition;
   QTimer                         mAnimationTimer;
   std::uint64_t                  mAnimationFrame = 0;
   bool                           mLeftButtonPressed = false;
   bool                           mDraggingView = false;
};
} // namespace WkNrm

#endif
