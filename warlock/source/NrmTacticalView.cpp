// AFSIM-driven two-dimensional operational and communication topology view.

#include "NrmTacticalView.hpp"

#include <algorithm>
#include <cmath>
#include <map>

#include <QPainter>
#include <QPaintEvent>

#include "nrm/NetworkTypeUtils.hpp"

namespace
{
QColor NetworkColor(nrm::NetworkType aType)
{
   switch (aType)
   {
   case nrm::NetworkType::cLINK11:
      return QColor(45, 135, 255);
   case nrm::NetworkType::cLINK16:
      return QColor(255, 70, 80);
   case nrm::NetworkType::cSATCOM:
      return QColor(185, 90, 255);
   case nrm::NetworkType::cCDL:
      return QColor(40, 210, 120);
   default:
      return QColor(160, 170, 185);
   }
}

QString NetworkLabel(nrm::NetworkType aType)
{
   return QString::fromStdString(nrm::ToString(aType));
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
   setMinimumSize(620, 480);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   connect(&mData, &DataContainer::SnapshotChanged, this, qOverload<>(&QWidget::update));
   connect(&mData, &DataContainer::AssessmentChanged, this, qOverload<>(&QWidget::update));
}

void WkNrm::TacticalView::paintEvent(QPaintEvent*)
{
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.fillRect(rect(), QColor(10, 20, 32));

   const QRectF plotRect = QRectF(rect()).adjusted(46.0, 62.0, -32.0, -54.0);
   painter.setPen(QPen(QColor(34, 61, 78), 1.0));
   for (int i = 0; i <= 10; ++i)
   {
      const qreal x = plotRect.left() + plotRect.width() * i / 10.0;
      const qreal y = plotRect.top() + plotRect.height() * i / 10.0;
      painter.drawLine(QPointF(x, plotRect.top()), QPointF(x, plotRect.bottom()));
      painter.drawLine(QPointF(plotRect.left(), y), QPointF(plotRect.right(), y));
   }

   const auto& snapshot = mData.GetSnapshot();
   painter.setPen(QColor(225, 235, 245));
   QFont titleFont = painter.font();
   titleFont.setPointSize(14);
   titleFont.setBold(true);
   painter.setFont(titleFont);
   painter.drawText(QPointF(22.0, 30.0), "AFSIM Operational Network View");

   QFont normalFont = painter.font();
   normalFont.setPointSize(9);
   normalFont.setBold(false);
   painter.setFont(normalFont);
   painter.setPen(QColor(145, 170, 190));
   painter.drawText(QPointF(22.0, 50.0),
                    QString("Live AFSIM snapshot  v%1  T=%2 s  •  aircraft / satellite / ground station / links")
                       .arg(snapshot.snapshotVersion)
                       .arg(snapshot.simTime, 0, 'f', 1));

   if (snapshot.endpoints.empty())
   {
      painter.setPen(QColor(210, 220, 230));
      painter.drawText(plotRect, Qt::AlignCenter, "Waiting for AFSIM platform state...");
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
   minLat -= rawLatSpan * 0.06;
   maxLat += rawLatSpan * 0.06;
   minLon -= rawLonSpan * 0.08;
   maxLon += rawLonSpan * 0.08;
   const double latSpan = maxLat - minLat;
   const double lonSpan = maxLon - minLon;

   std::map<std::string, QPointF> points;
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
      points[endpoint.endpointId] = QPointF(x, y);
      endpoints[endpoint.endpointId] = &endpoint;
   }

   for (const auto& link : snapshot.links)
   {
      const auto sourceIt = points.find(link.sourceEndpointId);
      const auto destinationIt = points.find(link.destinationEndpointId);
      if (sourceIt == points.end() || destinationIt == points.end())
      {
         continue;
      }
      QPen linkPen(NetworkColor(link.networkType), link.networkType == nrm::NetworkType::cCDL ? 3.0 : 2.0);
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
      const QPointF midpoint = (sourceIt->second + destinationIt->second) / 2.0;
      painter.drawText(midpoint + QPointF(5.0, -5.0), NetworkLabel(link.networkType));
   }

   std::map<std::string, QPointF> platformPoints;
   for (const auto& endpointEntry : endpoints)
   {
      platformPoints[endpointEntry.second->platformName] = points[endpointEntry.first];
   }
   if (mData.HasAssessment())
   {
      const nrm::AssessmentResult& assessment = mData.GetAssessment();
      auto drawRoute = [&painter, &platformPoints](const std::vector<std::string>& route,
                                                   const QColor& color,
                                                   bool candidate,
                                                   const QString& label)
      {
         if (route.size() < 2)
         {
            return;
         }
         QPen routePen(color, label == "PRIMARY" ? 7.0 : 4.0);
         routePen.setStyle(candidate ? Qt::DashLine : Qt::SolidLine);
         routePen.setCapStyle(Qt::RoundCap);
         painter.setPen(routePen);
         for (std::size_t index = 1; index < route.size(); ++index)
         {
            const auto sourceIt = platformPoints.find(route[index - 1]);
            const auto destinationIt = platformPoints.find(route[index]);
            if (sourceIt == platformPoints.end() || destinationIt == platformPoints.end())
            {
               continue;
            }
            painter.drawLine(sourceIt->second, destinationIt->second);
            if (index == 1)
            {
               const QPointF midpoint = (sourceIt->second + destinationIt->second) / 2.0;
               painter.drawText(midpoint + QPointF(8.0, 15.0), label);
            }
         }
      };
      drawRoute(assessment.backupRoute,
                QColor(0, 220, 255, 190),
                assessment.backupRouteUsesCandidate,
                "BACKUP");
      drawRoute(assessment.primaryRoute,
                QColor(255, 210, 35, 225),
                assessment.primaryRouteUsesCandidate,
                "PRIMARY");

      if (!assessment.primaryRoute.empty())
      {
         const auto sourceIt = platformPoints.find(assessment.primaryRoute.front());
         const auto destinationIt = platformPoints.find(assessment.primaryRoute.back());
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

   for (const auto& item : points)
   {
      const auto* endpointPtr = endpoints[item.first];
      const QPointF point = item.second;
      const QColor color = NetworkColor(endpointPtr->networkType);
      painter.setPen(QPen(color, 2.0));
      painter.setBrush(QColor(color.red(), color.green(), color.blue(), 85));

      const bool isSatellite = Contains(endpointPtr->platformName, "sat_");
      const bool isGround = Contains(endpointPtr->platformName, "control") ||
                            Contains(endpointPtr->platformName, "command") ||
                            Contains(endpointPtr->platformName, "station") ||
                            Contains(endpointPtr->platformName, "terminal");
      if (isSatellite)
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

      painter.setPen(QColor(235, 240, 245));
      painter.drawText(point + QPointF(14.0, -11.0),
                       QString::fromStdString(endpointPtr->platformName));
      painter.setPen(color);
      painter.drawText(point + QPointF(14.0, 4.0), NetworkLabel(endpointPtr->networkType));
   }

   qreal legendX = 22.0;
   const qreal legendY = height() - 20.0;
   for (const auto type : {nrm::NetworkType::cLINK11,
                           nrm::NetworkType::cLINK16,
                           nrm::NetworkType::cSATCOM,
                           nrm::NetworkType::cCDL})
   {
      painter.setPen(QPen(NetworkColor(type), 3.0));
      painter.drawLine(QPointF(legendX, legendY), QPointF(legendX + 26.0, legendY));
      painter.setPen(QColor(205, 215, 225));
      painter.drawText(QPointF(legendX + 32.0, legendY + 4.0), NetworkLabel(type));
      legendX += 115.0;
   }
}
