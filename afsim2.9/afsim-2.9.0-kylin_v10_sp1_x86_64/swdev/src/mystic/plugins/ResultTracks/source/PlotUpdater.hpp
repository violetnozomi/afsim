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
#ifndef RVTRACKSPLOTUPDATER_HPP
#define RVTRACKSPLOTUPDATER_HPP

#include <vector>

#include <QStringList>

#include "RvPlotUpdater.hpp"
#include "WkfTrack.hpp"

namespace rv
{
class MsgEntityState;
class MsgLocalTrackUpdate;
class MsgSensorTrackUpdate;
class Track;
class Vec3d;
} // namespace rv
namespace wkf
{
class PlatformGrouping;
}

namespace RvTracks
{
class PlotUpdater : public rv::PlotUpdater
{
public:
   PlotUpdater(const QString& aX, const QString& aY, const QString& aPlatformName, const int aTrackIndex, bool aLocal);
   ~PlotUpdater() override = default;

   QStringList GetPlatforms() const override;
   QStringList GetAxesX() const override { return GetAxes(); }
   QStringList GetAxesY() const override { return GetAxes(); }
   QString     GetUnitsX(const QString& aAxis) const override { return GetUnits(aAxis); }
   QString     GetUnitsY(const QString& aAxis) const override { return GetUnits(aAxis); }

   const QStringList& GetInitialSelection() const override;
   const QString&     GetInitialXAxis() const override;
   const QString&     GetInitialYAxis() const override;

   std::vector<std::pair<double, double>> GetSeries(const QString& aItem,
                                                    const QString& aXAxis,
                                                    const QString& aYAxis) const override;

   std::vector<std::pair<double, double>> GetTruthSeries(const QString& aXAxis, const QString& aYAxis) const;

   std::vector<std::pair<double, double>>
   GetSensorTrackSeries(const QString& aOwner, const int aTrackId, const QString& aXAxis, const QString& aYAxis) const;

   bool SupportsMultiplatform() const override { return false; }

   const QString&               GetTargetName() const { return mTargetName; }
   int                          GetTargetIndex() const { return mTargetIndex; }
   const std::set<wkf::TrackId> GetContributors() const { return mContributors; }
   const wkf::TrackId&          GetTrackId() const { return mTrackId; }

private:
   QStringList GetAxes() const;
   QString     GetUnits(const QString& aAxis) const;
   double      GetData(const QString& aDesc, const rv::MsgLocalTrackUpdate* aState) const;
   double      GetData(const QString& aDesc, const rv::MsgSensorTrackUpdate* aState) const;
   double      GetData(double aSimTime, const QString& aDesc, const rv::Track& aTrack) const;
   double      GetData(const QString& aDesc, const rv::MsgEntityState* aState) const;
   void        CollectContributorData();
   void        FindLocationTruthAtTime(double aTime, rv::Vec3d& aValue) const;

   QStringList  mInitialSelection;
   QString      mInitialXAxis;
   QString      mInitialYAxis;
   bool         mLocal;
   wkf::TrackId mTrackId;

   QString mTargetName;
   int     mTargetIndex;

   std::set<wkf::TrackId> mContributors;
};
} // namespace RvTracks

#endif
