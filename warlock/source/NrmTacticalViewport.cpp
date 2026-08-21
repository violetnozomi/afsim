/**
 * @file NrmTacticalViewport.cpp
 * @brief Implements cursor-anchored tactical topology zoom and bounded panning.
 */

#include "NrmTacticalViewport.hpp"

#include <algorithm>

#include "NrmUiScale.hpp"

void WkNrm::TacticalViewport::SetPlotRect(const QRectF& aPlotRect)
{
   if (mPlotRect == aPlotRect)
   {
      return;
   }
   mPlotRect = aPlotRect;
   mPanOffset = QPointF();
   ClampPan();
}

void WkNrm::TacticalViewport::SetScalePercent(int aPercent,
                                               const QPointF& aViewAnchor)
{
   const int clampedPercent = UiScale::ClampPercent(aPercent);
   if (mScalePercent == clampedPercent)
   {
      return;
   }

   if (!mPlotRect.isValid())
   {
      mScalePercent = clampedPercent;
      mPanOffset = QPointF();
      return;
   }

   const QPointF contentAtAnchor = ViewToContent(aViewAnchor);
   mScalePercent = clampedPercent;
   const qreal scaleFactor = UiScale::Factor(mScalePercent);
   const QPointF center = mPlotRect.center();
   mPanOffset = aViewAnchor - center - (contentAtAnchor - center) * scaleFactor;
   ClampPan();
}

void WkNrm::TacticalViewport::PanBy(const QPointF& aViewDelta)
{
   mPanOffset += aViewDelta;
   ClampPan();
}

void WkNrm::TacticalViewport::FitAll()
{
   mScalePercent = UiScale::cDEFAULT_PERCENT;
   mPanOffset = QPointF();
}

QPointF WkNrm::TacticalViewport::ContentToView(const QPointF& aContentPoint) const
{
   if (!mPlotRect.isValid())
   {
      return aContentPoint;
   }
   const QPointF center = mPlotRect.center();
   return center + mPanOffset +
          (aContentPoint - center) * UiScale::Factor(mScalePercent);
}

QPointF WkNrm::TacticalViewport::ViewToContent(const QPointF& aViewPoint) const
{
   if (!mPlotRect.isValid())
   {
      return aViewPoint;
   }
   const QPointF center = mPlotRect.center();
   return center + (aViewPoint - center - mPanOffset) /
                      UiScale::Factor(mScalePercent);
}

QTransform WkNrm::TacticalViewport::Transform() const
{
   const qreal scaleFactor = UiScale::Factor(mScalePercent);
   if (!mPlotRect.isValid())
   {
      return QTransform::fromScale(scaleFactor, scaleFactor);
   }
   const QPointF center = mPlotRect.center();
   const qreal translateX = center.x() + mPanOffset.x() - scaleFactor * center.x();
   const qreal translateY = center.y() + mPanOffset.y() - scaleFactor * center.y();
   return QTransform(scaleFactor, 0.0, 0.0, scaleFactor, translateX, translateY);
}

int WkNrm::TacticalViewport::ScalePercent() const
{
   return mScalePercent;
}

void WkNrm::TacticalViewport::ClampPan()
{
   if (!mPlotRect.isValid())
   {
      mPanOffset = QPointF();
      return;
   }

   const qreal scaleFactor = UiScale::Factor(mScalePercent);
   if (scaleFactor <= 1.0)
   {
      mPanOffset = QPointF();
      return;
   }

   const qreal maximumX = (scaleFactor - 1.0) * mPlotRect.width() / 2.0;
   const qreal maximumY = (scaleFactor - 1.0) * mPlotRect.height() / 2.0;
   mPanOffset.setX(std::max(-maximumX, std::min(maximumX, mPanOffset.x())));
   mPanOffset.setY(std::max(-maximumY, std::min(maximumY, mPanOffset.y())));
}
