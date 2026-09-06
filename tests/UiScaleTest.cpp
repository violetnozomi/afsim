/**
 * @file UiScaleTest.cpp
 * @brief Verifies bounded, deterministic Warlock UI scale calculations.
 */

#include "NrmUiScale.hpp"
#include "NrmUiStyle.hpp"
#include "NrmTacticalActivity.hpp"
#include "NrmTacticalViewport.hpp"

#include <cassert>
#include <cmath>

#include <QApplication>
#include <QPointF>
#include <QPushButton>
#include <QRectF>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
double Distance(const QPointF& aLeft, const QPointF& aRight)
{
   const QPointF delta = aLeft - aRight;
   return std::hypot(delta.x(), delta.y());
}
}

int main(int argc, char** argv)
{
   qputenv("QT_QPA_PLATFORM", "offscreen");
   QApplication application(argc, argv);

   using WkNrm::UiScale::ClampPercent;
   using WkNrm::UiScale::DecreasePercent;
   using WkNrm::UiScale::Factor;
   using WkNrm::UiScale::IncreasePercent;
   using WkNrm::UiScale::ScalePixels;

   assert(ClampPercent(10) == 50);
   assert(ClampPercent(100) == 100);
   assert(ClampPercent(600) == 500);
   assert(IncreasePercent(100) == 125);
   assert(IncreasePercent(499) == 500);
   assert(IncreasePercent(500) == 500);
   assert(DecreasePercent(100) == 75);
   assert(DecreasePercent(51) == 50);
   assert(DecreasePercent(50) == 50);
   assert(std::abs(Factor(125) - 1.25) < 0.0001);
   assert(ScalePixels(30, 100) == 30);
   assert(ScalePixels(30, 150) == 45);
   assert(ScalePixels(7, 125) == 9);

   nrm::LinkSnapshot quietLink;
   quietLink.state = nrm::ResourceState::cONLINE;
   nrm::WindowMetrics quietWindow;
   quietWindow.windowS = 10.0;
   quietLink.windows.push_back(quietWindow);
   assert(!WkNrm::HasRecentLinkActivity(quietLink));

   nrm::LinkSnapshot activeLink = quietLink;
   activeLink.windows.front().deliveredBits = 4096;
   activeLink.windows.front().messages.received = 1;
   assert(WkNrm::HasRecentLinkActivity(activeLink));
   assert(WkNrm::RecentLinkActivityCount({quietLink, activeLink}) == 1);

   activeLink.state = nrm::ResourceState::cOFFLINE;
   assert(!WkNrm::HasRecentLinkActivity(activeLink));

   assert(std::abs(WkNrm::LinkActivityPhase(0, 0) - 0.0) < 0.0001);
   assert(std::abs(WkNrm::LinkActivityPhase(20, 0) - 0.5) < 0.0001);
   assert(std::abs(WkNrm::LinkActivityPhase(40, 0) - 0.0) < 0.0001);
   assert(WkNrm::LinkActivityPhase(10, 1) != WkNrm::LinkActivityPhase(10, 0));

   WkNrm::TacticalViewport viewport;
   const QRectF plotRect(100.0, 80.0, 1000.0, 600.0);
   viewport.SetPlotRect(plotRect);
   const QPointF anchor(750.0, 350.0);
   const QPointF contentAtAnchor = viewport.ViewToContent(anchor);
   viewport.SetScalePercent(200, anchor);
   assert(viewport.ScalePercent() == 200);
   assert(Distance(viewport.ContentToView(contentAtAnchor), anchor) < 0.001);

   const QPointF contentPoint(800.0, 400.0);
   assert(Distance(viewport.Transform().map(contentPoint),
                   viewport.ContentToView(contentPoint)) < 0.001);
   assert(Distance(viewport.ViewToContent(viewport.ContentToView(contentPoint)),
                   contentPoint) < 0.001);

   // Topology zoom expands the distance between geographic positions, while
   // node glyphs and labels retain their requested screen-space dimensions.
   viewport.FitAll();
   const QSizeF fixedElementSize(28.0, 20.0);
   const QRectF elementAt100 = viewport.FixedElementRect(contentPoint, fixedElementSize);
   const QPointF secondContentPoint(900.0, 450.0);
   const qreal distanceAt100 =
      Distance(viewport.ContentToView(contentPoint),
               viewport.ContentToView(secondContentPoint));
   viewport.SetScalePercent(200, anchor);
   const QRectF elementAt200 = viewport.FixedElementRect(contentPoint, fixedElementSize);
   const qreal distanceAt200 =
      Distance(viewport.ContentToView(contentPoint),
               viewport.ContentToView(secondContentPoint));
   assert(std::abs(elementAt100.width() - 28.0) < 0.001);
   assert(std::abs(elementAt100.height() - 20.0) < 0.001);
   assert(std::abs(elementAt200.width() - 28.0) < 0.001);
   assert(std::abs(elementAt200.height() - 20.0) < 0.001);
   assert(Distance(elementAt200.center(), viewport.ContentToView(contentPoint)) < 0.001);
   assert(std::abs(distanceAt200 - distanceAt100 * 2.0) < 0.001);

   const QPointF beforePan = viewport.ContentToView(contentPoint);
   const QPointF panDelta(-100.0, 50.0);
   viewport.PanBy(panDelta);
   assert(Distance(viewport.ContentToView(contentPoint), beforePan + panDelta) < 0.001);

   viewport.PanBy(QPointF(10000.0, 10000.0));
   assert(Distance(viewport.ContentToView(plotRect.center()), plotRect.bottomRight()) < 0.001);
   viewport.FitAll();
   assert(viewport.ScalePercent() == 100);
   assert(Distance(viewport.ContentToView(contentPoint), contentPoint) < 0.001);

   QWidget root;
   root.setObjectName("NrmRoot");
   QVBoxLayout layout(&root);
   layout.setContentsMargins(12, 10, 12, 12);
   layout.setSpacing(10);
   QPushButton button(QString::fromUtf8("放大"), &root);
   layout.addWidget(&button);

   WkNrm::UiStyle::Apply(&root);
   root.ensurePolished();
   button.ensurePolished();
   const qreal basePointSize = button.font().pointSizeF();
   const int baseButtonHeight = button.sizeHint().height();
   const QMargins baseMargins = layout.contentsMargins();
   const int baseSpacing = layout.spacing();

   // Reapplying the fixed panel style must not resize any resource-panel widget.
   WkNrm::UiStyle::Apply(&root);
   application.processEvents();
   const QMargins unchangedMargins = layout.contentsMargins();

   assert(basePointSize >= 9.9 && basePointSize <= 10.1);
   assert(std::abs(button.font().pointSizeF() - basePointSize) < 0.0001);
   assert(button.sizeHint().height() == baseButtonHeight);
   assert(unchangedMargins == baseMargins);
   assert(layout.spacing() == baseSpacing);
   return 0;
}
