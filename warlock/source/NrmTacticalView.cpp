// AFSIM-driven two-dimensional operational and communication topology view.

#include "NrmTacticalView.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>

#include <QApplication>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QStringList>
#include <QWheelEvent>

#include "nrm/NetworkTypeUtils.hpp"
#include "NrmTacticalActivity.hpp"
#include "NrmUiScale.hpp"
#include "NrmUiText.hpp"

namespace
{
QColor NetworkColor(nrm::NetworkType aType)
{
   switch (aType)
   {
   case nrm::NetworkType::cLINK11:
      return QColor("#4c9aff");
   case nrm::NetworkType::cLINK16:
      return QColor("#ff6b6b");
   case nrm::NetworkType::cSATCOM:
      return QColor("#b983ff");
   case nrm::NetworkType::cCDL:
      return QColor("#39d98a");
   default:
      return QColor("#91a4ba");
   }
}

QString NetworkLabel(nrm::NetworkType aType)
{
   return QString::fromStdString(WkNrm::UiText::TranslateCode(nrm::ToString(aType)));
}

QString RouteNetworkLabel(nrm::NetworkType aType)
{
   return QString::fromStdString(WkNrm::UiText::TranslateCode(nrm::ToString(aType)));
}

void DrawRouteLabel(QPainter& aPainter,
                    const QPointF& aAnchor,
                    const QString& aText,
                    const QColor& aColor)
{
   const QFont originalFont = aPainter.font();
   QFont routeFont = originalFont;
   routeFont.setPointSize(8);
   routeFont.setBold(true);
   aPainter.setFont(routeFont);
   const QRectF textBounds = aPainter.fontMetrics().boundingRect(aText);
   const QRectF labelRect(aAnchor.x() - textBounds.width() / 2.0 - 5.0,
                          aAnchor.y() - textBounds.height() / 2.0 - 3.0,
                          textBounds.width() + 10.0,
                          textBounds.height() + 6.0);
   aPainter.setPen(QPen(QColor(7, 16, 27, 225), 1.0));
   aPainter.setBrush(QColor(7, 16, 27, 220));
   aPainter.drawRoundedRect(labelRect, 4.0, 4.0);
   aPainter.setPen(aColor);
   aPainter.drawText(labelRect, Qt::AlignCenter, aText);
   aPainter.setFont(originalFont);
}

void DrawDirectedRouteHop(QPainter& aPainter,
                          const QPointF& aSource,
                          const QPointF& aDestination,
                          const nrm::AssessmentRouteHop& aHop,
                          const QColor& aColor,
                          bool aPrimary)
{
   const qreal deltaX = aDestination.x() - aSource.x();
   const qreal deltaY = aDestination.y() - aSource.y();
   const qreal length = std::sqrt(deltaX * deltaX + deltaY * deltaY);
   if (length < 1.0) return;

   const qreal unitX = deltaX / length;
   const qreal unitY = deltaY / length;
   const qreal endpointInset = std::min<qreal>(18.0, length * 0.2);
   const QPointF lineStart = aSource + QPointF(unitX * endpointInset, unitY * endpointInset);
   const QPointF lineEnd = aDestination - QPointF(unitX * endpointInset, unitY * endpointInset);
   QPen routePen(aColor, aPrimary ? 6.0 : 3.5);
   routePen.setCapStyle(Qt::RoundCap);
   routePen.setStyle((aHop.kind == nrm::AssessmentRouteHopKind::cCANDIDATE_LINK ||
                      aHop.candidate)
                        ? Qt::DashLine
                        : Qt::SolidLine);
   aPainter.setPen(routePen);
   aPainter.setBrush(Qt::NoBrush);
   aPainter.drawLine(lineStart, lineEnd);

   const qreal arrowLength = aPrimary ? 12.0 : 10.0;
   const qreal arrowWidth = aPrimary ? 6.5 : 5.5;
   const QPointF arrowBase = lineEnd - QPointF(unitX * arrowLength, unitY * arrowLength);
   const QPointF normal(-unitY, unitX);
   QPolygonF arrow;
   arrow << lineEnd
         << arrowBase + normal * arrowWidth
         << arrowBase - normal * arrowWidth;
   aPainter.setPen(Qt::NoPen);
   aPainter.setBrush(aColor);
   aPainter.drawPolygon(arrow);

   nrm::NetworkType displayType = aHop.sourceNetworkType;
   if (displayType == nrm::NetworkType::cUNKNOWN)
   {
      displayType = aHop.destinationNetworkType;
   }
   QString label = QString::fromUtf8("第%1跳 %2")
                      .arg(aHop.hopIndex)
                      .arg(RouteNetworkLabel(displayType));
   if (aHop.kind == nrm::AssessmentRouteHopKind::cCANDIDATE_LINK || aHop.candidate)
   {
      label += QString::fromUtf8("·候选");
   }
   const QPointF midpoint = (lineStart + lineEnd) / 2.0;
   DrawRouteLabel(aPainter, midpoint + QPointF(-unitY * 13.0, unitX * 13.0), label, aColor);
}

void DrawGatewayTransition(QPainter& aPainter,
                           const QPointF& aGatewayPoint,
                           const nrm::AssessmentRouteHop& aHop,
                           const QColor& aColor,
                           bool aPrimary)
{
   const qreal diameter = aPrimary ? 38.0 : 32.0;
   const QRectF loopRect(aGatewayPoint.x() + 14.0,
                         aGatewayPoint.y() - diameter - 4.0,
                         diameter,
                         diameter);
   QPen routePen(aColor, aPrimary ? 5.0 : 3.0);
   routePen.setCapStyle(Qt::RoundCap);
   aPainter.setPen(routePen);
   aPainter.setBrush(Qt::NoBrush);
   aPainter.drawArc(loopRect, 35 * 16, 290 * 16);

   const QPointF arrowTip(loopRect.right() - 1.0, loopRect.center().y() + 5.0);
   QPolygonF arrow;
   arrow << arrowTip
         << arrowTip + QPointF(-9.0, -6.0)
         << arrowTip + QPointF(-8.0, 5.0);
   aPainter.setPen(Qt::NoPen);
   aPainter.setBrush(aColor);
   aPainter.drawPolygon(arrow);

   const QString label = QString::fromUtf8("第%1跳 %2→%3")
                            .arg(aHop.hopIndex)
                            .arg(RouteNetworkLabel(aHop.sourceNetworkType))
                            .arg(RouteNetworkLabel(aHop.destinationNetworkType));
   DrawRouteLabel(aPainter,
                  QPointF(loopRect.center().x(), loopRect.top() - 10.0),
                  label,
                  aColor);
}

bool Contains(const std::string& aText, const char* aToken)
{
   return aText.find(aToken) != std::string::npos;
}
} // namespace

WkNrm::TacticalView::TacticalView(DataContainer& aData, QWidget* aParentPtr)
   : QWidget(aParentPtr)
   , mData(aData)
{
   setMinimumSize(0, 0);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   connect(&mData, &DataContainer::SnapshotChanged, this, qOverload<>(&QWidget::update));
   connect(&mData, &DataContainer::AssessmentChanged, this, qOverload<>(&QWidget::update));
   mAnimationTimer.setInterval(80);
   connect(&mAnimationTimer, &QTimer::timeout, this, [this]()
   {
      ++mAnimationFrame;
      if (RecentLinkActivityCount(mData.GetSnapshot().links) > 0)
      {
         update();
      }
   });
   mAnimationTimer.start();
}

void WkNrm::TacticalView::BeginAssessmentSelection(int aTarget)
{
   TacticalSelectionTarget target = TacticalSelectionTarget::cNONE;
   if (aTarget == static_cast<int>(TacticalSelectionTarget::cSOURCE))
   {
      target = TacticalSelectionTarget::cSOURCE;
   }
   else if (aTarget == static_cast<int>(TacticalSelectionTarget::cDESTINATION))
   {
      target = TacticalSelectionTarget::cDESTINATION;
   }
   BeginTacticalSelection(mSelectionState, target);
   update();
}

void WkNrm::TacticalView::SetAssessmentPlatforms(const QString& aSourcePlatform,
                                                  const QString& aDestinationPlatform)
{
   mSelectionState.sourcePlatform = aSourcePlatform.toStdString();
   mSelectionState.destinationPlatform = aDestinationPlatform.toStdString();
   update();
}

void WkNrm::TacticalView::SetUiScalePercent(int aPercent)
{
   const int previousPercent = mViewport.ScalePercent();
   const QPointF anchor = mPlotRect.isValid() ? mPlotRect.center() : QPointF();
   mViewport.SetScalePercent(aPercent, anchor);
   if (mViewport.ScalePercent() != previousPercent)
   {
      update();
   }
}

void WkNrm::TacticalView::mousePressEvent(QMouseEvent* aEventPtr)
{
   const QPointF viewPosition = aEventPtr->localPos();
   if (aEventPtr->button() != Qt::LeftButton || !mPlotRect.contains(viewPosition) ||
       mDetailsRect.contains(viewPosition))
   {
      QWidget::mousePressEvent(aEventPtr);
      return;
   }

   mLeftButtonPressed = true;
   mDraggingView = false;
   mPressPosition = viewPosition;
   mLastDragPosition = viewPosition;
   aEventPtr->accept();
}

void WkNrm::TacticalView::mouseMoveEvent(QMouseEvent* aEventPtr)
{
   if (!mLeftButtonPressed || !(aEventPtr->buttons() & Qt::LeftButton))
   {
      QWidget::mouseMoveEvent(aEventPtr);
      return;
   }

   const QPointF viewPosition = aEventPtr->localPos();
   const QPoint dragDistance = viewPosition.toPoint() - mPressPosition.toPoint();
   if (!mDraggingView && dragDistance.manhattanLength() >= QApplication::startDragDistance())
   {
      mDraggingView = true;
      setCursor(Qt::ClosedHandCursor);
   }
   if (mDraggingView)
   {
      mViewport.PanBy(viewPosition - mLastDragPosition);
      mLastDragPosition = viewPosition;
      update();
   }
   aEventPtr->accept();
}

void WkNrm::TacticalView::mouseReleaseEvent(QMouseEvent* aEventPtr)
{
   if (aEventPtr->button() != Qt::LeftButton || !mLeftButtonPressed)
   {
      QWidget::mouseReleaseEvent(aEventPtr);
      return;
   }

   const bool wasDragging = mDraggingView;
   mLeftButtonPressed = false;
   mDraggingView = false;
   unsetCursor();
   if (!wasDragging)
   {
      HandlePlatformClick(aEventPtr->localPos());
   }
   aEventPtr->accept();
}

void WkNrm::TacticalView::wheelEvent(QWheelEvent* aEventPtr)
{
   const QPointF viewPosition = aEventPtr->posF();
   if (!mPlotRect.contains(viewPosition) || mDetailsRect.contains(viewPosition) ||
       aEventPtr->angleDelta().y() == 0)
   {
      QWidget::wheelEvent(aEventPtr);
      return;
   }

   const int currentPercent = mViewport.ScalePercent();
   const int nextPercent = aEventPtr->angleDelta().y() > 0
                              ? UiScale::IncreasePercent(currentPercent)
                              : UiScale::DecreasePercent(currentPercent);
   mViewport.SetScalePercent(nextPercent, viewPosition);
   if (mViewport.ScalePercent() != currentPercent)
   {
      emit UiScalePercentChanged(mViewport.ScalePercent());
      update();
   }
   aEventPtr->accept();
}

void WkNrm::TacticalView::HandlePlatformClick(const QPointF& aViewPosition)
{
   if (!mPlotRect.contains(aViewPosition) || mDetailsRect.contains(aViewPosition))
   {
      return;
   }

   const std::string platformName =
      HitTestPlatform(mHitRegions, aViewPosition.x(), aViewPosition.y());
   const TacticalSelectionAssignment assignment =
      ApplyPlatformClick(mSelectionState, platformName);
   if (!platformName.empty())
   {
      emit PlatformSelected(QString::fromStdString(platformName), static_cast<int>(assignment));
      emit SelectionTargetChanged(static_cast<int>(mSelectionState.target));
   }
   update();
}

void WkNrm::TacticalView::paintEvent(QPaintEvent*)
{
   mHitRegions.clear();
   mDetailsRect = QRectF();
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setRenderHint(QPainter::TextAntialiasing, true);
   const QRectF viewRect(rect());
   QLinearGradient background(0.0, 0.0, 0.0, viewRect.height());
   background.setColorAt(0.0, QColor("#091321"));
   background.setColorAt(1.0, QColor("#060d18"));
   painter.fillRect(viewRect, background);

   const auto& snapshot = mData.GetSnapshot();
   std::set<std::string> platformNames;
   for (const auto& endpoint : snapshot.endpoints)
   {
      platformNames.insert(endpoint.platformName);
   }
   PruneTacticalSelection(mSelectionState, platformNames);

   mPlotRect = viewRect.adjusted(24.0, 72.0, -24.0, -84.0);
   mViewport.SetPlotRect(mPlotRect);
   const QRectF plotRect = mPlotRect;
   painter.setPen(QPen(QColor("#21344b"), 1.0));
   painter.setBrush(QColor("#0a1625"));
   painter.drawRoundedRect(plotRect, 12.0, 12.0);

   painter.setPen(QColor("#f3f8ff"));
   QFont titleFont = painter.font();
   titleFont.setPointSize(16);
   titleFont.setBold(true);
   painter.setFont(titleFont);
   painter.drawText(QPointF(24.0, 29.0), QString::fromUtf8("综合通信资源态势"));

   QFont normalFont = painter.font();
   normalFont.setPointSize(9);
   normalFont.setBold(false);
   painter.setFont(normalFont);
   painter.setPen(QColor("#88a0bc"));
   painter.drawText(QPointF(24.0, 51.0),
                    QString::fromUtf8("AFSIM 实时快照  ·  版本%1  ·  仿真时间%2秒  ·  协作平台与四网链路")
                       .arg(snapshot.snapshotVersion)
                       .arg(snapshot.simTime, 0, 'f', 1));

   const std::size_t activeLinkCount = RecentLinkActivityCount(snapshot.links);
   const QString summary = QString::fromUtf8("%1 网络   %2 平台   %3 链路   %4 活动   %5 网关能力")
                              .arg(snapshot.networks.size())
                              .arg(platformNames.size())
                              .arg(snapshot.links.size())
                              .arg(activeLinkCount)
                              .arg(snapshot.gateways.size());
   const qreal summaryWidth = painter.fontMetrics().horizontalAdvance(summary) + 24.0;
   const QRectF summaryRect(viewRect.width() - summaryWidth - 24.0, 16.0, summaryWidth, 30.0);
   painter.setPen(QPen(QColor("#29435f"), 1.0));
   painter.setBrush(QColor("#102238"));
   painter.drawRoundedRect(summaryRect, 15.0, 15.0);
   painter.setPen(QColor("#9edcff"));
   painter.drawText(summaryRect, Qt::AlignCenter, summary);

   if (snapshot.endpoints.empty())
   {
      painter.setPen(QColor("#a8b9cc"));
      painter.drawText(plotRect, Qt::AlignCenter, QString::fromUtf8("正在等待AFSIM平台状态……"));
      return;
   }

   double minLat = 90.0;
   double maxLat = -90.0;
   double minLon = 180.0;
   double maxLon = -180.0;
   for (const auto& endpoint : snapshot.endpoints)
   {
      if (endpoint.latitudeDeg.valid && endpoint.longitudeDeg.valid)
      {
         minLat = std::min(minLat, endpoint.latitudeDeg.value);
         maxLat = std::max(maxLat, endpoint.latitudeDeg.value);
         minLon = std::min(minLon, endpoint.longitudeDeg.value);
         maxLon = std::max(maxLon, endpoint.longitudeDeg.value);
      }
   }
   const double rawLatSpan = std::max(0.05, maxLat - minLat);
   const double rawLonSpan = std::max(0.05, maxLon - minLon);
   // Leave additional vertical breathing room so north/south outliers and their
   // labels do not stretch the operational view to the full widget height.
   // This changes presentation only; the reported geographic coordinates stay
   // untouched.
   minLat -= rawLatSpan * 0.22;
   maxLat += rawLatSpan * 0.22;
   minLon -= rawLonSpan * 0.10;
   maxLon += rawLonSpan * 0.10;
   const double latSpan = maxLat - minLat;
   const double lonSpan = maxLon - minLon;

   std::map<std::string, QPointF> contentPoints;
   std::map<std::string, QPointF> viewPoints;
   std::map<std::string, const nrm::EndpointSnapshot*> endpoints;
   for (const auto& endpoint : snapshot.endpoints)
   {
      if (!endpoint.latitudeDeg.valid || !endpoint.longitudeDeg.valid)
      {
         continue;
      }
      const qreal x = plotRect.left() +
                      (endpoint.longitudeDeg.value - minLon) / lonSpan * plotRect.width();
      const qreal y = plotRect.bottom() -
                      (endpoint.latitudeDeg.value - minLat) / latSpan * plotRect.height();
      const QPointF contentPoint(x, y);
      contentPoints[endpoint.endpointId] = contentPoint;
      viewPoints[endpoint.endpointId] = mViewport.ContentToView(contentPoint);
      endpoints[endpoint.endpointId] = &endpoint;
   }

   painter.save();
   painter.setClipRect(plotRect.adjusted(1.0, 1.0, -1.0, -1.0));
   painter.setPen(QPen(QColor(42, 66, 91, 115), 1.0));
   for (int i = 0; i <= 10; ++i)
   {
      const qreal x = plotRect.left() + plotRect.width() * i / 10.0;
      const qreal y = plotRect.top() + plotRect.height() * i / 10.0;
      painter.drawLine(mViewport.ContentToView(QPointF(x, plotRect.top())),
                       mViewport.ContentToView(QPointF(x, plotRect.bottom())));
      painter.drawLine(mViewport.ContentToView(QPointF(plotRect.left(), y)),
                       mViewport.ContentToView(QPointF(plotRect.right(), y)));
   }

   std::size_t activeLinkOrdinal = 0;
   for (const auto& link : snapshot.links)
   {
      const auto sourceIt = viewPoints.find(link.sourceEndpointId);
      const auto destinationIt = viewPoints.find(link.destinationEndpointId);
      if (sourceIt == viewPoints.end() || destinationIt == viewPoints.end())
      {
         continue;
      }
      const bool active = HasRecentLinkActivity(link);
      QColor linkColor = NetworkColor(link.networkType);
      linkColor.setAlpha(active ? 235 : 125);
      const qreal baseWidth = link.networkType == nrm::NetworkType::cCDL ? 2.8 : 1.8;
      QPen linkPen(linkColor, active ? baseWidth + 1.3 : baseWidth);
      linkPen.setCapStyle(Qt::RoundCap);
      if (link.networkType == nrm::NetworkType::cLINK11)
      {
         linkPen.setStyle(Qt::DashLine);
      }
      else if (link.networkType == nrm::NetworkType::cSATCOM)
      {
         linkPen.setStyle(Qt::DotLine);
      }
      painter.setPen(linkPen);
      painter.drawLine(sourceIt->second, destinationIt->second);

      if (active)
      {
         const QPointF direction = destinationIt->second - sourceIt->second;
         for (std::size_t particle = 0; particle < 3; ++particle)
         {
            const double phase = LinkActivityPhase(
               mAnimationFrame, activeLinkOrdinal * 3U + particle);
            const QPointF particlePoint = sourceIt->second + direction * phase;
            QColor glow = NetworkColor(link.networkType);
            glow.setAlpha(70);
            painter.setPen(Qt::NoPen);
            painter.setBrush(glow);
            painter.drawEllipse(particlePoint, 7.0, 7.0);
            painter.setBrush(NetworkColor(link.networkType));
            painter.drawEllipse(particlePoint, 3.0, 3.0);
         }
         ++activeLinkOrdinal;
      }
   }

   std::map<std::string, QPointF> platformPoints;
   std::map<std::string, QPointF> platformContentPoints;
   std::map<std::string, std::vector<nrm::NetworkType>> platformNetworks;
   std::map<std::string, std::vector<const nrm::GatewayResourceState*>>
      platformGateways;
   for (const auto& endpointEntry : endpoints)
   {
      platformPoints[endpointEntry.second->platformName] = viewPoints[endpointEntry.first];
      platformContentPoints[endpointEntry.second->platformName] =
         contentPoints[endpointEntry.first];
      auto& networkTypes = platformNetworks[endpointEntry.second->platformName];
      if (std::find(networkTypes.begin(), networkTypes.end(), endpointEntry.second->networkType) ==
          networkTypes.end())
      {
         networkTypes.push_back(endpointEntry.second->networkType);
      }
   }
   for (const nrm::GatewayResourceState& gateway : snapshot.gateways)
   {
      platformGateways[gateway.platformId].push_back(&gateway);
   }
   if (mData.HasAssessment())
   {
      const nrm::AssessmentResult& assessment = mData.GetAssessment();
      auto drawRouteHops = [&painter, &platformPoints](
                              const std::vector<nrm::AssessmentRouteHop>& aHops,
                              const QColor& aColor,
                              bool aPrimary)
      {
         for (const nrm::AssessmentRouteHop& hop : aHops)
         {
            const auto sourceIt = platformPoints.find(hop.sourcePlatform);
            const auto destinationIt = platformPoints.find(hop.destinationPlatform);
            if (sourceIt == platformPoints.end() || destinationIt == platformPoints.end())
            {
               continue;
            }
            if (hop.kind == nrm::AssessmentRouteHopKind::cGATEWAY_TRANSITION || hop.gateway)
            {
               DrawGatewayTransition(painter, sourceIt->second, hop, aColor, aPrimary);
            }
            else
            {
               DrawDirectedRouteHop(painter,
                                    sourceIt->second,
                                    destinationIt->second,
                                    hop,
                                    aColor,
                                    aPrimary);
            }
         }
      };
      drawRouteHops(assessment.backupRouteHops, QColor(0, 220, 255, 190), false);
      drawRouteHops(assessment.primaryRouteHops, QColor(255, 210, 35, 225), true);

      if (!assessment.primaryRouteHops.empty())
      {
         const auto sourceIt = platformPoints.find(assessment.primaryRouteHops.front().sourcePlatform);
         const auto destinationIt =
            platformPoints.find(assessment.primaryRouteHops.back().destinationPlatform);
         painter.setBrush(Qt::NoBrush);
         if (sourceIt != platformPoints.end())
         {
            painter.setPen(QPen(QColor(60, 255, 120), 4.0));
            painter.drawEllipse(sourceIt->second, 18.0, 18.0);
         }
         if (destinationIt != platformPoints.end())
         {
            painter.setPen(QPen(QColor(255, 150, 30), 4.0));
            painter.drawEllipse(destinationIt->second, 18.0, 18.0);
         }
      }
   }

   std::vector<QRectF> occupiedLabels;
   for (const auto& platformEntry : platformPoints)
   {
      const std::string& platformName = platformEntry.first;
      const QPointF point = platformEntry.second;
      const QPointF contentPoint = platformContentPoints[platformName];
      mHitRegions.push_back({platformName, point.x(), point.y(), 20.0});
      const std::vector<nrm::NetworkType>& networks = platformNetworks[platformName];
      const nrm::NetworkType primaryType =
         networks.empty() ? nrm::NetworkType::cUNKNOWN : networks.front();
      const QColor color = NetworkColor(primaryType);
      const bool isGateway = platformGateways.find(platformName) != platformGateways.end();

      painter.setBrush(Qt::NoBrush);
      if (platformName == mSelectionState.sourcePlatform)
      {
         painter.setPen(QPen(QColor("#39d98a"), 3.0, Qt::DashLine));
         painter.drawEllipse(mViewport.FixedElementRect(contentPoint, QSizeF(44.0, 44.0)));
      }
      if (platformName == mSelectionState.destinationPlatform)
      {
         painter.setPen(QPen(QColor("#ff9f43"), 3.0, Qt::DashLine));
         painter.drawEllipse(mViewport.FixedElementRect(contentPoint, QSizeF(52.0, 52.0)));
      }
      if (platformName == mSelectionState.selectedPlatform)
      {
         painter.setPen(QPen(QColor("#f8e16c"), 2.0));
         painter.drawEllipse(mViewport.FixedElementRect(contentPoint, QSizeF(60.0, 60.0)));
      }

      QColor glowColor = color;
      glowColor.setAlpha(34);
      painter.setPen(Qt::NoPen);
      painter.setBrush(glowColor);
      painter.drawEllipse(mViewport.FixedElementRect(contentPoint, QSizeF(34.0, 34.0)));
      painter.setPen(QPen(color, 2.0));
      painter.setBrush(QColor(color.red(), color.green(), color.blue(), 105));

      const bool isSatellite = Contains(platformName, "sat_") || Contains(platformName, "satellite");
      const bool isGround = Contains(platformName, "control") ||
                            Contains(platformName, "command") ||
                            Contains(platformName, "station") ||
                            Contains(platformName, "terminal") ||
                            Contains(platformName, "center") ||
                            Contains(platformName, "service") ||
                            Contains(platformName, "sensor");
      if (isGateway)
      {
         QPolygonF gatewayShape;
         for (int vertex = 0; vertex < 6; ++vertex)
         {
            const qreal angle = 3.14159265358979323846 / 3.0 * vertex;
            gatewayShape << point + QPointF(std::cos(angle) * 13.0,
                                            std::sin(angle) * 13.0);
         }
         painter.drawPolygon(gatewayShape);
         QFont gatewayFont = painter.font();
         gatewayFont.setPointSize(6);
         gatewayFont.setBold(true);
         painter.setFont(gatewayFont);
         painter.setPen(QColor("#f8e16c"));
         painter.drawText(QRectF(point - QPointF(10.0, 6.0), QSizeF(20.0, 12.0)),
                          Qt::AlignCenter, QString::fromUtf8("关"));
         painter.setFont(normalFont);
      }
      else if (isSatellite)
      {
         QPolygonF diamond;
         diamond << point + QPointF(0.0, -12.0) << point + QPointF(12.0, 0.0)
                 << point + QPointF(0.0, 12.0) << point + QPointF(-12.0, 0.0);
         painter.drawPolygon(diamond);
      }
      else if (isGround)
      {
         painter.drawRect(QRectF(point - QPointF(10.0, 10.0), QSizeF(20.0, 20.0)));
      }
      else
      {
         QPolygonF aircraft;
         aircraft << point + QPointF(14.0, 0.0) << point + QPointF(-9.0, -8.0)
                  << point + QPointF(-5.0, 0.0) << point + QPointF(-9.0, 8.0);
         painter.drawPolygon(aircraft);
      }

      const QString displayName = QString::fromStdString(platformName);
      QStringList networkLabels;
      for (nrm::NetworkType network : networks)
      {
         networkLabels.push_back(NetworkLabel(network));
      }
      const QString networkText = networkLabels.join(" · ");
      const int labelWidth = std::max(painter.fontMetrics().horizontalAdvance(displayName),
                                      painter.fontMetrics().horizontalAdvance(networkText)) + 14;
      const qreal labelHeight = 32.0;
      const std::vector<QPointF> labelOffsets = {
         QPointF(14.0, -19.0),
         QPointF(14.0, 5.0),
         QPointF(-labelWidth - 14.0, -19.0),
         QPointF(-labelWidth - 14.0, 5.0),
         QPointF(-labelWidth / 2.0, -48.0),
         QPointF(-labelWidth / 2.0, 19.0)};
      QRectF labelRect;
      bool labelVisible = false;
      for (const QPointF& offset : labelOffsets)
      {
         const QRectF candidate(point + offset, QSizeF(labelWidth, labelHeight));
         if (!plotRect.adjusted(4.0, 4.0, -4.0, -4.0).contains(candidate))
         {
            continue;
         }
         bool overlaps = false;
         for (const QRectF& occupied : occupiedLabels)
         {
            if (occupied.adjusted(-3.0, -3.0, 3.0, 3.0).intersects(candidate))
            {
               overlaps = true;
               break;
            }
         }
         if (!overlaps)
         {
            labelRect = candidate;
            labelVisible = true;
            occupiedLabels.push_back(candidate);
            break;
         }
      }

      qreal badgeX = point.x() - 5.0 * static_cast<qreal>(networks.size() - 1);
      for (nrm::NetworkType network : networks)
      {
         painter.setPen(QPen(QColor("#07111d"), 1.0));
         painter.setBrush(NetworkColor(network));
         painter.drawEllipse(QPointF(badgeX, point.y() + 14.0), 3.5, 3.5);
         badgeX += 10.0;
      }

      if (!labelVisible)
      {
         continue;
      }
      painter.setPen(QPen(QColor(42, 59, 80, 210), 1.0));
      painter.setBrush(QColor(6, 14, 25, 220));
      painter.drawRoundedRect(labelRect, 5.0, 5.0);
      painter.setPen(QColor("#edf5ff"));
      painter.drawText(labelRect.adjusted(7.0, 2.0, -4.0, -14.0),
                       Qt::AlignLeft | Qt::AlignVCenter,
                       displayName);
      painter.setPen(color);
      painter.drawText(labelRect.adjusted(7.0, 15.0, -4.0, -2.0),
                       Qt::AlignLeft | Qt::AlignVCenter,
                       networkText);
   }
   painter.restore();

   if (!mSelectionState.selectedPlatform.empty())
   {
      std::vector<const nrm::EndpointSnapshot*> selectedEndpoints;
      for (const nrm::EndpointSnapshot& endpoint : snapshot.endpoints)
      {
         if (endpoint.platformName == mSelectionState.selectedPlatform)
         {
            selectedEndpoints.push_back(&endpoint);
         }
      }
      if (!selectedEndpoints.empty())
      {
         const qreal detailsWidth = std::min<qreal>(360.0, plotRect.width() * 0.46);
         const qreal detailsHeight = std::min<qreal>(340.0, plotRect.height() - 24.0);
         mDetailsRect = QRectF(plotRect.right() - detailsWidth - 12.0,
                              plotRect.top() + 12.0,
                              detailsWidth,
                              detailsHeight);
         painter.setPen(QPen(QColor("#486787"), 1.0));
         painter.setBrush(QColor(7, 18, 31, 238));
         painter.drawRoundedRect(mDetailsRect, 8.0, 8.0);

         const nrm::EndpointSnapshot& first = *selectedEndpoints.front();
         QString details = QString::fromUtf8("节点详情 · %1\n")
                              .arg(QString::fromStdString(first.platformName));
         details += QString::fromUtf8("平台标识：%1\n")
                       .arg(QString::fromStdString(first.platformId.empty()
                                                     ? first.platformName
                                                     : first.platformId));
         if (first.latitudeDeg.valid && first.longitudeDeg.valid)
         {
            details += QString::fromUtf8("位置：%1°, %2°")
                          .arg(first.latitudeDeg.value, 0, 'f', 5)
                          .arg(first.longitudeDeg.value, 0, 'f', 5);
            if (first.altitudeM.valid)
            {
               details += QString::fromUtf8("  高度 %1 米").arg(first.altitudeM.value, 0, 'f', 1);
            }
            details += "\n";
         }
         std::size_t onlineCount = 0;
         for (const nrm::EndpointSnapshot* endpointPtr : selectedEndpoints)
         {
            if (endpointPtr->state == nrm::ResourceState::cONLINE)
            {
               ++onlineCount;
            }
         }
         details += QString::fromUtf8("通信端点：%1  在线：%2\n")
                       .arg(selectedEndpoints.size())
                       .arg(onlineCount);
         const std::size_t visibleCount = std::min<std::size_t>(selectedEndpoints.size(), 5);
         for (std::size_t index = 0; index < visibleCount; ++index)
         {
            const nrm::EndpointSnapshot& endpoint = *selectedEndpoints[index];
            const QString direction = endpoint.canSend && endpoint.canReceive
                                         ? QString::fromUtf8("收发")
                                         : endpoint.canSend ? QString::fromUtf8("仅发")
                                                            : endpoint.canReceive ? QString::fromUtf8("仅收")
                                                                                  : QString::fromUtf8("禁用");
            const QString role = endpoint.memberRole.empty()
                                    ? QString::fromUtf8("未标注职责")
                                    : QString::fromStdString(
                                         UiText::TranslateCode(endpoint.memberRole));
            const QString state = QString::fromStdString(
               UiText::TranslateCode(nrm::ToString(endpoint.state)));
            details += QString::fromUtf8("• %1 | %2 | %3 | %4 | %5 | %6\n")
                          .arg(NetworkLabel(endpoint.networkType),
                               QString::fromStdString(endpoint.commName.empty()
                                                         ? endpoint.endpointId
                                                         : endpoint.commName),
                               QString::fromStdString(endpoint.address),
                               role,
                               state,
                               direction);
         }
         if (selectedEndpoints.size() > visibleCount)
         {
            details += QString::fromUtf8("…另有 %1 个端点")
                          .arg(selectedEndpoints.size() - visibleCount);
         }

         const auto selectedGateways =
            platformGateways.find(mSelectionState.selectedPlatform);
         if (selectedGateways != platformGateways.end())
         {
            details += QString::fromUtf8("\n跨域网关能力：%1\n")
                          .arg(selectedGateways->second.size());
            const std::size_t gatewayVisibleCount =
               std::min<std::size_t>(selectedGateways->second.size(), 3);
            for (std::size_t index = 0; index < gatewayVisibleCount; ++index)
            {
               const nrm::GatewayResourceState& gateway =
                  *selectedGateways->second[index];
               const QString status = gateway.enabled && gateway.valid
                                         ? QString::fromUtf8("可用")
                                         : QString::fromUtf8("不可用");
               details += QString::fromUtf8("◆ %1 | %2 → %3 | %4\n")
                             .arg(QString::fromStdString(gateway.gatewayId),
                                  QString::fromStdString(gateway.ingressNetworkId),
                                  QString::fromStdString(gateway.egressNetworkId),
                                  status);
               details += QString::fromUtf8("  队列 %1/%2，转发 %3，拒绝 %4，丢弃 %5\n")
                             .arg(gateway.queuedMessages)
                             .arg(gateway.maxQueueMessages)
                             .arg(gateway.forwardedCount)
                             .arg(gateway.rejectedCount)
                             .arg(gateway.droppedCount);
               if (!gateway.recentEvents.empty())
               {
                  const nrm::GatewayForwardingEvent& event =
                     gateway.recentEvents.back();
                  details += QString::fromUtf8("  最近：结果=%1 · 消息=%2 · 原因=%3\n")
                                .arg(QString::fromStdString(
                                        UiText::TranslateCode(event.result)),
                                     QString::fromStdString(event.messageType),
                                     QString::fromStdString(
                                        UiText::TranslateCode(event.reasonCode)));
               }
            }
         }

         QFont detailsFont = painter.font();
         detailsFont.setPointSize(9);
         painter.setFont(detailsFont);
         painter.setPen(QColor("#e8f2ff"));
         painter.drawText(mDetailsRect.adjusted(12.0, 10.0, -12.0, -10.0),
                          Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                          details);
      }
   }

   if (mSelectionState.target != TacticalSelectionTarget::cNONE)
   {
      const QString prompt = mSelectionState.target == TacticalSelectionTarget::cSOURCE
                                ? QString::fromUtf8("请在态势图中点击源节点")
                                : QString::fromUtf8("请在态势图中点击目的节点");
      const QRectF promptRect(plotRect.left() + 12.0, plotRect.top() + 12.0, 220.0, 30.0);
      painter.setPen(QPen(QColor("#5f86ad"), 1.0));
      painter.setBrush(QColor(13, 32, 52, 230));
      painter.drawRoundedRect(promptRect, 6.0, 6.0);
      painter.setPen(QColor("#f8e16c"));
      painter.drawText(promptRect, Qt::AlignCenter, prompt);
   }

   qreal legendX = 24.0;
   const qreal legendY = viewRect.height() - 56.0;
   for (const auto type : {nrm::NetworkType::cLINK11,
                           nrm::NetworkType::cLINK16,
                           nrm::NetworkType::cSATCOM,
                           nrm::NetworkType::cCDL})
   {
      const QRectF legendRect(legendX, legendY - 12.0, 98.0, 25.0);
      painter.setPen(QPen(QColor("#263a53"), 1.0));
      painter.setBrush(QColor("#0d1929"));
      painter.drawRoundedRect(legendRect, 12.0, 12.0);
      painter.setPen(Qt::NoPen);
      painter.setBrush(NetworkColor(type));
      painter.drawEllipse(QPointF(legendX + 14.0, legendY), 4.0, 4.0);
      painter.setPen(QColor("#cad8e8"));
      painter.drawText(QRectF(legendX + 24.0, legendY - 10.0, 68.0, 20.0),
                       Qt::AlignLeft | Qt::AlignVCenter,
                       NetworkLabel(type));
      legendX += 106.0;
   }

   struct RouteLegendEntry
   {
      QString label;
      QColor color;
      bool dashed = false;
      bool gateway = false;
      qreal width = 100.0;
   };
   const std::vector<RouteLegendEntry> routeLegend = {
      {QString::fromUtf8("当前链路"), QColor("#91a4ba"), false, false, 92.0},
      {QString::fromUtf8("算法主路由"), QColor(255, 210, 35), false, false, 108.0},
      {QString::fromUtf8("算法备选路由"), QColor(0, 220, 255), false, false, 116.0},
      {QString::fromUtf8("候选跳"), QColor(255, 210, 35), true, false, 84.0},
      {QString::fromUtf8("网关转换"), QColor("#f8e16c"), false, true, 100.0}};
   legendX = 24.0;
   const qreal routeLegendY = viewRect.height() - 24.0;
   for (const RouteLegendEntry& entry : routeLegend)
   {
      const QRectF legendRect(legendX, routeLegendY - 12.0, entry.width, 25.0);
      painter.setPen(QPen(QColor("#263a53"), 1.0));
      painter.setBrush(QColor("#0d1929"));
      painter.drawRoundedRect(legendRect, 12.0, 12.0);
      if (entry.gateway)
      {
         painter.setPen(QPen(entry.color, 2.0));
         painter.setBrush(Qt::NoBrush);
         painter.drawArc(QRectF(legendX + 8.0, routeLegendY - 7.0, 14.0, 14.0),
                         35 * 16,
                         290 * 16);
      }
      else
      {
         QPen samplePen(entry.color, 2.5);
         samplePen.setStyle(entry.dashed ? Qt::DashLine : Qt::SolidLine);
         painter.setPen(samplePen);
         painter.drawLine(QPointF(legendX + 8.0, routeLegendY),
                          QPointF(legendX + 24.0, routeLegendY));
      }
      painter.setPen(QColor("#cad8e8"));
      painter.drawText(QRectF(legendX + 29.0,
                              routeLegendY - 10.0,
                              entry.width - 34.0,
                              20.0),
                       Qt::AlignLeft | Qt::AlignVCenter,
                       entry.label);
      legendX += entry.width + 8.0;
   }
}
