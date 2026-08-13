// AFSIM-driven two-dimensional operational and communication topology view.

#include "NrmTacticalView.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>

#include <QLinearGradient>
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
   setMinimumSize(0, 0);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   connect(&mData, &DataContainer::SnapshotChanged, this, qOverload<>(&QWidget::update));
   connect(&mData, &DataContainer::AssessmentChanged, this, qOverload<>(&QWidget::update));
}

void WkNrm::TacticalView::paintEvent(QPaintEvent*)
{
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setRenderHint(QPainter::TextAntialiasing, true);
   QLinearGradient background(0.0, 0.0, 0.0, height());
   background.setColorAt(0.0, QColor("#091321"));
   background.setColorAt(1.0, QColor("#060d18"));
   painter.fillRect(rect(), background);

   const auto& snapshot = mData.GetSnapshot();
   std::set<std::string> platformNames;
   for (const auto& endpoint : snapshot.endpoints)
   {
      platformNames.insert(endpoint.platformName);
   }

   const QRectF plotRect = QRectF(rect()).adjusted(24.0, 72.0, -24.0, -58.0);
   painter.setPen(QPen(QColor("#21344b"), 1.0));
   painter.setBrush(QColor("#0a1625"));
   painter.drawRoundedRect(plotRect, 12.0, 12.0);

   painter.save();
   painter.setClipRect(plotRect.adjusted(1.0, 1.0, -1.0, -1.0));
   painter.setPen(QPen(QColor(42, 66, 91, 115), 1.0));
   for (int i = 0; i <= 10; ++i)
   {
      const qreal x = plotRect.left() + plotRect.width() * i / 10.0;
      const qreal y = plotRect.top() + plotRect.height() * i / 10.0;
      painter.drawLine(QPointF(x, plotRect.top()), QPointF(x, plotRect.bottom()));
      painter.drawLine(QPointF(plotRect.left(), y), QPointF(plotRect.right(), y));
   }
   painter.restore();

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
                    QString::fromUtf8("AFSIM 实时快照  ·  v%1  ·  T=%2秒  ·  协作平台与四网链路")
                       .arg(snapshot.snapshotVersion)
                       .arg(snapshot.simTime, 0, 'f', 1));

   const QString summary = QString::fromUtf8("%1 网络   %2 平台   %3 链路")
                              .arg(snapshot.networks.size())
                              .arg(platformNames.size())
                              .arg(snapshot.links.size());
   const qreal summaryWidth = painter.fontMetrics().horizontalAdvance(summary) + 24.0;
   const QRectF summaryRect(width() - summaryWidth - 24.0, 16.0, summaryWidth, 30.0);
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
      QColor linkColor = NetworkColor(link.networkType);
      linkColor.setAlpha(155);
      QPen linkPen(linkColor, link.networkType == nrm::NetworkType::cCDL ? 2.8 : 1.8);
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
   }

   std::map<std::string, QPointF> platformPoints;
   std::map<std::string, std::vector<nrm::NetworkType>> platformNetworks;
   for (const auto& endpointEntry : endpoints)
   {
      platformPoints[endpointEntry.second->platformName] = points[endpointEntry.first];
      auto& networkTypes = platformNetworks[endpointEntry.second->platformName];
      if (std::find(networkTypes.begin(), networkTypes.end(), endpointEntry.second->networkType) ==
          networkTypes.end())
      {
         networkTypes.push_back(endpointEntry.second->networkType);
      }
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
         QPen routePen(color, label == QString::fromUtf8("主路由") ? 7.0 : 4.0);
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
                QString::fromUtf8("备选路由"));
      drawRoute(assessment.primaryRoute,
                QColor(255, 210, 35, 225),
                assessment.primaryRouteUsesCandidate,
                QString::fromUtf8("主路由"));

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

   std::vector<QRectF> occupiedLabels;
   for (const auto& platformEntry : platformPoints)
   {
      const std::string& platformName = platformEntry.first;
      const QPointF point = platformEntry.second;
      const std::vector<nrm::NetworkType>& networks = platformNetworks[platformName];
      const nrm::NetworkType primaryType =
         networks.empty() ? nrm::NetworkType::cUNKNOWN : networks.front();
      const QColor color = NetworkColor(primaryType);

      QColor glowColor = color;
      glowColor.setAlpha(34);
      painter.setPen(Qt::NoPen);
      painter.setBrush(glowColor);
      painter.drawEllipse(point, 17.0, 17.0);
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

   qreal legendX = 24.0;
   const qreal legendY = height() - 27.0;
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
}
