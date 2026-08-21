/**
 * @file NrmTacticalViewport.hpp
 * @brief Defines map-style zoom and pan transforms for the tactical topology layer.
 */

#ifndef NRM_TACTICAL_VIEWPORT_HPP
#define NRM_TACTICAL_VIEWPORT_HPP

#include <QPointF>
#include <QRectF>
#include <QTransform>

namespace WkNrm
{
class TacticalViewport
{
public:
   void SetPlotRect(const QRectF& aPlotRect);
   void SetScalePercent(int aPercent, const QPointF& aViewAnchor);
   void PanBy(const QPointF& aViewDelta);
   void FitAll();

   QPointF ContentToView(const QPointF& aContentPoint) const;
   QPointF ViewToContent(const QPointF& aViewPoint) const;
   QTransform Transform() const;
   int ScalePercent() const;

private:
   void ClampPan();

   QRectF  mPlotRect;
   QPointF mPanOffset;
   int     mScalePercent = 100;
};
} // namespace WkNrm

#endif
