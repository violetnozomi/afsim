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
#ifndef WKFUTILS_HPP
#define WKFUTILS_HPP

#include "wkf_export.h"

#include <QColor>

#include "UtQt.hpp"
#include "VaPosition.hpp"

namespace wkf
{
namespace util
{
WKF_EXPORT QColor  GetColor(const QString& aString);
WKF_EXPORT QString GetHexColor(const QString& aString);
WKF_EXPORT QColor  GetHashColor(const QString& aString);
WKF_EXPORT UtColor GetHashUtColor(const std::string& aString);

WKF_EXPORT QString ParseLinks(const QString& aString, bool aClickable = true);

// WKF has its own URLs that serialize camera-views, platforms, and tracks
// these URLs have a scheme of wkf
// the host may be platform, track, or view
// a platform link has the name of the platform as the query and would look like
// wkf://platform?PLATFORM_NAME
// a track link similiarly has a track-name as the query and would look like
// wkf://track?TRACK_NAME
// a view link's query contains a comma separated list with
// latitude, longitude, distance from view point, elevation, azimuth
// a view link looks like
// wkf://view?LAT,LON,DIST,EL,AZ
WKF_EXPORT bool FollowLink(const QUrl& aSrc, QString& aErrorMsg);
WKF_EXPORT vespa::VaPosition GetPositionFromUrl(const QUrl& aSrc);
} // namespace util
} // namespace wkf
#endif
