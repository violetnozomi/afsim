// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfUtils.hpp"

#include <QCryptographicHash>
#include <QRegularExpression>
#include <QUrl>

#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "VaCamera.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

namespace wkf
{
namespace util
{

// this will return a color based on the defined strings in UtColor, failing that it will
// hash the string to make a color
QColor GetColor(const QString& aString)
{
   if (UtColor::ColorExists(aString.toStdString()))
   {
      UtColor c = UtColor::GetColorMapping(aString.toStdString());
      float   rgba[4];
      c.Get(rgba[0], rgba[1], rgba[2], rgba[3]);
      return QColor(rgba[0] * 255, rgba[1] * 255, rgba[2] * 255, rgba[3] * 255);
   }
   else
   {
      return GetHashColor(aString);
   }
}

QString GetHexColor(const QString& aString)
{
   QColor        c     = wkf::util::GetColor(aString);
   unsigned char red   = c.redF() * 255;
   unsigned char green = c.greenF() * 255;
   unsigned char blue  = c.blueF() * 255;
   QString       str;
   if (red < 16)
   {
      str += "0";
   }
   str += QString::number(red, 16);
   if (green < 16)
   {
      str += "0";
   }
   str += QString::number(green, 16);
   if (blue < 16)
   {
      str += "0";
   }
   str += QString::number(blue, 16);
   return str;
}

QColor GetHashColor(const QString& aString)
{
   // If a color isn't in our list, go ahead and pick a random color.
   // To get the same color every time, use a hash based on the side string to pick a seed.
   // Then use the seed to draw for uniform random values
   QByteArray hashResult = QCryptographicHash::hash(aString.toLatin1(), QCryptographicHash::Sha1);

   // We're just using the first 32 bits of the hash
   int seed = ((int)hashResult[0]) << 24;
   seed |= ((int)hashResult[1]) << 16;
   seed |= ((int)hashResult[2]) << 8;
   seed |= ((int)hashResult[3]);

   ut::Random r;
   r.SetSeed(seed);
   // Luminance is shifted to the middle to support contrast on a variety of backgrounds
   auto   lum = r.Uniform<qreal>(0.25, 0.75);
   QColor c;
   c.setHslF(r.Uniform<qreal>(), r.Uniform<qreal>(), lum);
   return c;
}

UtColor GetHashUtColor(const std::string& aString)
{
   return UtQtColor(GetHashColor(QString::fromStdString(aString)));
}

QString ParseLinks(const QString& aString, bool aClickable)
{
   QString                         newString = aString;
   QRegularExpression              regexLink("{([ptvc]):([^}]+)}");
   QRegularExpressionMatchIterator regexMatchIterator = regexLink.globalMatch(aString);
   int                             offset             = 0;
   while (regexMatchIterator.hasNext())
   {
      QRegularExpressionMatch regexMatch = regexMatchIterator.next();
      QString                 type;
      QString                 target = regexMatch.captured(2);
      QString                 text   = target;
      switch (regexMatch.captured(1).at(0).toLatin1())
      {
      case ('p'):
         type = "platform";
         break;
      case ('t'):
         type = "track";
         break;
      case ('v'):
      {
         type           = "view";
         QStringList sl = target.split(",");
         if (sl.size() > 5)
         {
            text = sl[5];
         }
         else
         {
            text = "View";
         }
         break;
      }
      case ('c'):
         type = "time";
         break;
      }
      QString linkStr;
      if (aClickable)
      {
         linkStr = "<a href=\"wkf://" + type + "?" + target + "\">" + text + "</a>";
      }
      else
      {
         linkStr = "wkf://" + type + "?" + target;
      }
      newString.replace(regexMatch.capturedStart(0) + offset, regexMatch.capturedLength(0), linkStr);
      offset += linkStr.length() - regexMatch.capturedLength(0);
   }
   return newString;
}

bool FollowLink(const QUrl& aSrc, QString& aErrorMsg)
{
   QString scheme = aSrc.scheme();
   QString host   = aSrc.host();
   QString query  = aSrc.query();
   if (scheme == "wkf")
   {
      if (host == "platform")
      {
         if (!query.isEmpty())
         {
            auto* scenarioPtr = vaEnv.GetStandardScenario();
            if (scenarioPtr)
            {
               wkf::Platform* plat = scenarioPtr->FindPlatformByName(query);
               if (plat && plat->IsVisible(vaEnv.GetStandardViewer()))
               {
                  // Note: We may desire to clear selection on other entities such as routes, waypoints, and tracks.
                  //       Currently we do not have a method to do that, without clearing everything, which we can not
                  //       do. Clearing everything would clear all selected platforms on the viewer directly, this
                  //       causes the viewer to be out of sync from the environment for what is selected.
                  wkfEnv.SetPlatformsSelected({plat});
               }
               else
               {
                  aErrorMsg = "Could not find platform " + query;
                  return false;
               }
            }
            else
            {
               aErrorMsg = "Could not find platform " + query;
               return false;
            }
         }
         else
         {
            aErrorMsg = "Could not follow link, no query name provided";
            return false;
         }
      }
      else if (host == "track")
      {
         if (!query.isEmpty())
         {
            auto* scenarioPtr = vaEnv.GetStandardScenario();
            if (scenarioPtr)
            {
               vespa::VaEntity* ent = scenarioPtr->FindEntityOfType<wkf::Track>(query.toStdString());
               wkf::Track*      t   = dynamic_cast<wkf::Track*>(ent);
               if (t && t->IsVisible(vaEnv.GetStandardViewer()))
               {
                  // Clear the platform selections
                  wkfEnv.SetPlatformsSelected({});
                  // Explicitly call the viewer to clear selection because
                  //  tracks will not be cleared when the environment clears all platforms.
                  // Note: this only works because we are not trying to select a Platform after the Clear.
                  //       See comment above in the host == "platform" section
                  vaEnv.GetStandardViewer()->ClearSelection();
                  t->SetSelected(vaEnv.GetStandardViewer(), true);
               }
               else
               {
                  aErrorMsg = "Could not find track " + query;
                  return false;
               }
            }
            else
            {
               aErrorMsg = "Could not find track " + query;
               return false;
            }
         }
         else
         {
            aErrorMsg = "Could not follow link, no query name provided";
            return false;
         }
      }
      else if (host == "time")
      {
         aErrorMsg = "Time links are not implemented";
         return false;
      }
      else if (host == "view")
      {
         if (!query.isEmpty())
         {
            bool        good = true;
            QStringList sl   = query.split(',');
            if (sl.size() >= 5)
            {
               double lat  = UtMath::Limit(sl[0].toDouble(), 89.9);
               double lon  = UtMath::Limit(sl[1].toDouble(), 180.0);
               double dist = UtMath::Limit(sl[2].toDouble(), 1000.0, 1000000000.0);
               double tilt = UtMath::Limit(sl[3].toDouble(), 0.0, 89.9);
               double rot  = UtMath::Limit(sl[4].toDouble(), 360.0);

               vespa::VaViewer* viewer = vaEnv.GetStandardViewer();
               if (viewer)
               {
                  viewer->GetCamera()->GetCameraMotion()->AnimateToPosition(vespa::VaPosition(lat, lon, dist),
                                                                            rot,
                                                                            90 - tilt,
                                                                            1.0);
               }
            }
            else
            {
               good = false;
            }
            if (!good)
            {
               aErrorMsg = "View link was badly formatted";
               return false;
            }
         }
         else
         {
            aErrorMsg = "Could not follow link, no query name provided";
            return false;
         }
      }
      else
      {
         aErrorMsg = "Cannot process url due to unknown host (" + host + "): <pre>" + aSrc.toString() + "</pre>";
         return false;
      }
   }
   else
   {
      aErrorMsg = "Cannot process url due to unknown scheme (" + scheme + "): <pre>" + aSrc.toString() + "</pre>";
      return false;
   }
   return true;
}

vespa::VaPosition GetPositionFromUrl(const QUrl& aSrc)
{
   QString scheme = aSrc.scheme();
   QString host   = aSrc.host();
   QString query  = aSrc.query();
   if (scheme == "wkf")
   {
      if (host == "view")
      {
         QStringList sl = query.split(',');
         if (sl.size() >= 5)
         {
            double lat = UtMath::Limit(sl[0].toDouble(), 89.9);
            double lon = UtMath::Limit(sl[1].toDouble(), 180.0);
            return vespa::VaPosition(lat, lon, 0.0);
         }
      }
   }
   return vespa::VaPosition();
}

} // namespace util
} // namespace wkf
