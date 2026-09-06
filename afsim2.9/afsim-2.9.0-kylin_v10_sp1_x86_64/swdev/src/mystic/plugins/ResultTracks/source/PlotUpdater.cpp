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

#include "PlotUpdater.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvTrackDb.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "WkfUnitsObject.hpp"

RvTracks::PlotUpdater::PlotUpdater(const QString& aX,
                                   const QString& aY,
                                   const QString& aPlatformName,
                                   const int      aTrackIndex,
                                   bool           aLocal)
   : rv::PlotUpdater()
   , mInitialXAxis(aX)
   , mInitialYAxis(aY)
   , mLocal(aLocal)
   , mTrackId(aPlatformName.toStdString(), aTrackIndex)
   , mTargetIndex(0)
{
   CollectContributorData();
   mInitialSelection.push_back(aPlatformName);

   SetSampleRate(0.0);
}

QStringList RvTracks::PlotUpdater::GetPlatforms() const
{
   QStringList     list;
   rv::ResultData* dataPtr = rvEnv.GetData();

   // initialize the plot window's information
   if (dataPtr != nullptr)
   {
      rv::ResultDb* dbPtr = dataPtr->GetDb();
      if (dbPtr != nullptr)
      {
         for (unsigned int i = 0; i < dbPtr->GetPlatformCount(); i++)
         {
            list.push_back(QString::fromStdString(dbPtr->GetPlatformName(i)));
         }
      }
   }
   return list;
}

QStringList RvTracks::PlotUpdater::GetAxes() const
{
   QStringList axisVariableList;
   axisVariableList.push_back("Time");
   axisVariableList.push_back("Latitude");
   axisVariableList.push_back("Longitude");
   axisVariableList.push_back("Altitude");
   axisVariableList.push_back("Positional Error");
   axisVariableList.push_back("Speed");
   axisVariableList.push_back("Heading");
   axisVariableList.push_back("Quality");
   return axisVariableList;
}

QString RvTracks::PlotUpdater::GetUnits(const QString& aAxis) const
{
   if (aAxis == "Time")
   {
      return WKF_UNIT_NAME(Time);
   }
   else if (aAxis == "Latitude" || aAxis == "Longitude")
   {
      return "degrees";
   }
   else if (aAxis == "Altitude")
   {
      return WKF_UNIT_NAME(Length);
   }
   else if (aAxis == "Positional Error")
   {
      return "meters";
   }
   else if (aAxis == "Speed")
   {
      return WKF_UNIT_NAME(Speed);
   }
   else if (aAxis == "Heading")
   {
      return WKF_UNIT_NAME(Angle);
   }
   // Quality is unitless
   return {};
}

const QStringList& RvTracks::PlotUpdater::GetInitialSelection() const
{
   return mInitialSelection;
}

const QString& RvTracks::PlotUpdater::GetInitialXAxis() const
{
   return mInitialXAxis;
}

const QString& RvTracks::PlotUpdater::GetInitialYAxis() const
{
   return mInitialYAxis;
}

std::vector<std::pair<double, double>> RvTracks::PlotUpdater::GetSeries(const QString& aItem,
                                                                        const QString& aXAxis,
                                                                        const QString& aYAxis) const
{
   std::vector<std::pair<double, double>> series;

   rv::ResultData* results = rvEnv.GetData();

   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         std::string name = aItem.toStdString();

         rv::ResultPlatform* platformPtr = db->FindPlatformByName(name);
         if (platformPtr)
         {
            int idx = platformPtr->GetPlatformIndex();

            const rv::TrackDb* trackdb = db->LockTrackDb();
            if (trackdb)
            {
               const rv::TrackDb::SourceTracks* sourceTracks = trackdb->FindOwnerTracks(idx);
               if (sourceTracks)
               {
                  if (mLocal)
                  {
                     for (auto&& it : sourceTracks->mLocalTracks)
                     {
                        if ((it.first.owner() == mTrackId.GetPlatformName()) &&
                            (it.first.localTrackNumber() == mTrackId.GetLocalTrackId()))
                        {
                           double lastUpdate = -GetSampleRate() - 1.0;
                           for (auto&& jt : it.second.mUpdates)
                           {
                              rv::MsgLocalTrackUpdate* trackMessage = dynamic_cast<rv::MsgLocalTrackUpdate*>(jt);

                              if ((trackMessage->simTime() - lastUpdate) > GetSampleRate())
                              {
                                 double x = GetData(aXAxis, trackMessage);
                                 double y = GetData(aYAxis, trackMessage);
                                 series.emplace_back(std::pair<double, double>(x, y));

                                 lastUpdate = trackMessage->simTime();
                              }
                           }
                           break;
                        }
                     }
                  }
                  else
                  {
                     for (auto&& it : sourceTracks->mSensorTracks)
                     {
                        if ((it.first.owner() == mTrackId.GetPlatformName()) &&
                            (it.first.localTrackNumber() == mTrackId.GetLocalTrackId()))
                        {
                           double lastUpdate = -GetSampleRate() - 1.0;
                           for (auto&& jt : it.second.mUpdates)
                           {
                              rv::MsgSensorTrackUpdate* trackMessage = dynamic_cast<rv::MsgSensorTrackUpdate*>(jt);
                              if ((trackMessage->simTime() - lastUpdate) > GetSampleRate())
                              {
                                 double x = GetData(aXAxis, trackMessage);
                                 double y = GetData(aYAxis, trackMessage);
                                 series.emplace_back(std::pair<double, double>(x, y));

                                 lastUpdate = trackMessage->simTime();
                              }
                           }
                           break;
                        }
                     }
                  }
               }
            }

            db->UnlockTrackDb();
         }
      }
   }
   return series;
}

std::vector<std::pair<double, double>> RvTracks::PlotUpdater::GetTruthSeries(const QString& aXAxis, const QString& aYAxis) const
{
   std::vector<std::pair<double, double>> series;

   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         rv::ResultPlatform* platformPtr = db->FindPlatform(mTargetIndex);
         if (platformPtr)
         {
            auto stateArray = platformPtr->GetArray<rv::MsgEntityState>();
            if (stateArray != nullptr)
            {
               for (auto stateMsg : *stateArray)
               {
                  double x = GetData(aXAxis, dynamic_cast<rv::MsgEntityState*>(stateMsg));
                  double y = GetData(aYAxis, dynamic_cast<rv::MsgEntityState*>(stateMsg));
                  series.emplace_back(std::pair<double, double>(x, y));
               }
            }
         }
      }
   }
   return series;
}

std::vector<std::pair<double, double>> RvTracks::PlotUpdater::GetSensorTrackSeries(const QString& aOwner,
                                                                                   const int      aTrackId,
                                                                                   const QString& aXAxis,
                                                                                   const QString& aYAxis) const
{
   std::vector<std::pair<double, double>> series;

   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         rv::ResultPlatform* platformPtr = db->FindPlatformByName(aOwner);
         if (platformPtr)
         {
            int idx = platformPtr->GetPlatformIndex();

            const rv::TrackDb* trackdb = db->LockTrackDb();
            if (trackdb)
            {
               const rv::TrackDb::SourceTracks* sourceTracks = trackdb->FindOwnerTracks(idx);
               if (sourceTracks)
               {
                  for (auto&& it : sourceTracks->mSensorTracks)
                  {
                     if (it.first.localTrackNumber() == aTrackId)
                     {
                        for (auto&& jt : it.second.mUpdates)
                        {
                           double x = GetData(aXAxis, dynamic_cast<rv::MsgSensorTrackUpdate*>(jt));
                           double y = GetData(aYAxis, dynamic_cast<rv::MsgSensorTrackUpdate*>(jt));
                           series.emplace_back(std::pair<double, double>(x, y));
                        }
                     }
                  }
               }
            }

            db->UnlockTrackDb();
         }
      }
   }
   return series;
}

double RvTracks::PlotUpdater::GetData(const QString& aDesc, const rv::MsgLocalTrackUpdate* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else
   {
      return GetData(aState->simTime(), aDesc, aState->track());
   }
}

double RvTracks::PlotUpdater::GetData(const QString& aDesc, const rv::MsgSensorTrackUpdate* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else
   {
      return GetData(aState->simTime(), aDesc, aState->track());
   }
}

double RvTracks::PlotUpdater::GetData(double aSimTime, const QString& aDesc, const rv::Track& aTrack) const
{
   if (aDesc == "Latitude")
   {
      rv::Vec3d l      = aTrack.locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      double    lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      return lla[0]; // degrees, no other units available...
   }
   else if (aDesc == "Longitude")
   {
      rv::Vec3d l      = aTrack.locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      double    lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      return lla[1]; // degrees, no other units available
   }
   else if (aDesc == "Altitude")
   {
      rv::Vec3d l      = aTrack.locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      double    lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAltitudeUnit(0);
      return UtUnitLength::ConvertFromStandard(lla[2], u);
   }
   else if (aDesc == "Positional Error")
   {
      rv::Vec3d track = aTrack.locationWCS();
      rv::Vec3d truth;
      FindLocationTruthAtTime(aSimTime, truth);
      return sqrt((track.x() - truth.x()) * (track.x() - truth.x()) +
                  (track.y() - truth.y()) * (track.y() - truth.y()) + (track.z() - truth.z()) * (track.z() - truth.z()));
   }
   else if (aDesc == "Heading")
   {
      UtEntity entity;
      double   xyz[3]  = {aTrack.locationWCS().x(), aTrack.locationWCS().y(), aTrack.locationWCS().z()};
      double   vxyz[3] = {aTrack.velocityWCS().x(), aTrack.velocityWCS().y(), aTrack.velocityWCS().z()};
      entity.SetLocationWCS(xyz);
      entity.SetVelocityWCS(vxyz);
      double vel[3];
      entity.GetVelocityNED(vel);
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(atan2(vel[1], vel[0]), u);
   }
   else if (aDesc == "Speed")
   {
      rv::Vec3f             v = aTrack.velocityWCS();
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z()), u);
   }
   else if (aDesc == "Quality")
   {
      return aTrack.trackQuality();
   }
   return 0.0;
}

double RvTracks::PlotUpdater::GetData(const QString& aDesc, const rv::MsgEntityState* aState) const
{
   if (aDesc == "Time")
   {
      UtUnitTime::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeUnit();
      return UtUnitTime::ConvertFromStandard(aState->simTime(), u);
   }
   else if (aDesc == "Latitude")
   {
      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      double    lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      return lla[0]; // degrees, no other units available...
   }
   else if (aDesc == "Longitude")
   {
      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      double    lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      return lla[1]; // degrees, no other units available
   }
   else if (aDesc == "Altitude")
   {
      rv::Vec3d l      = aState->state().locationWCS();
      double    xyz[3] = {l.x(), l.y(), l.z()};
      double    lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(xyz, lla[0], lla[1], lla[2]);
      UtUnitLength::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAltitudeUnit(0);
      return UtUnitLength::ConvertFromStandard(lla[2], u);
   }
   else if (aDesc == "Positional Error")
   {
      return 0.0;
   }
   else if (aDesc == "Heading")
   {
      UtEntity entity;
      double   xyz[3]  = {aState->state().locationWCS().x(),
                       aState->state().locationWCS().y(),
                       aState->state().locationWCS().z()};
      double   vxyz[3] = {aState->state().velocityWCS().x(),
                        aState->state().velocityWCS().y(),
                        aState->state().velocityWCS().z()};
      entity.SetLocationWCS(xyz);
      entity.SetVelocityWCS(vxyz);
      double vel[3];
      entity.GetVelocityNED(vel);
      UtUnitAngle::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit();
      return UtUnitAngle::ConvertFromStandard(atan2(vel[1], vel[0]), u);
   }
   else if (aDesc == "Speed")
   {
      rv::Vec3f             v = aState->state().velocityWCS();
      UtUnitSpeed::BaseUnit u = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit();
      return UtUnitSpeed::ConvertFromStandard(sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z()), u);
   }
   else if (aDesc == "Quality")
   {
      return 1.0;
   }
   return 0.0;
}

void RvTracks::PlotUpdater::CollectContributorData()
{
   // first, I want to know truth
   rv::ResultData* results = rvEnv.GetData();
   if (results)
   {
      rv::ResultDb* db = results->GetDb();
      if (db)
      {
         const rv::TrackDb* tdb = db->LockTrackDb();
         if (tdb)
         {
            rv::ResultPlatform* owner = db->FindPlatformByName(mTrackId.GetPlatformName());
            if (owner)
            {
               int ownerIndex = db->FindPlatformByName(mTrackId.GetPlatformName())->GetPlatformIndex();
               const rv::TrackDb::SourceTracks* srcTracks = tdb->FindOwnerTracks(ownerIndex);
               if (srcTracks)
               {
                  rv::Track_Id rvtrackid;
                  rvtrackid.owner(mTrackId.GetPlatformName());
                  rvtrackid.localTrackNumber(mTrackId.GetLocalTrackId());
                  if (mLocal)
                  {
                     std::map<rv::Track_Id, rv::TrackDb::LocalTrackList>::const_iterator trackList =
                        srcTracks->mLocalTracks.find(rvtrackid);
                     if (trackList != srcTracks->mLocalTracks.end())
                     {
                        mTargetIndex               = trackList->second.mMsgCreate->targetIndex();
                        rv::ResultPlatform* target = db->FindPlatform(mTargetIndex);
                        if (target)
                        {
                           mTargetName = QString::fromStdString(target->GetName());
                        }
                        double endTime = trackList->second.mUpdates.GetMaxTime();
                        std::vector<std::pair<rv::MsgBase*, unsigned int>> eventList;
                        db->TraceTrackId(rvtrackid, 0, eventList, endTime);
                        for (auto&& it : eventList)
                        {
                           if (it.first->GetMessageId() == rv::MsgSensorTrackCreated::cMESSAGE_ID)
                           {
                              rv::MsgSensorTrackCreated* stc = dynamic_cast<rv::MsgSensorTrackCreated*>(it.first);
                              mContributors.emplace(stc->trackId().owner(), stc->trackId().localTrackNumber());
                           }
                        }
                     }
                  }
                  else
                  {
                     std::map<rv::Track_Id, rv::TrackDb::SensorTrackList>::const_iterator trackList =
                        srcTracks->mSensorTracks.find(rvtrackid);
                     if (trackList != srcTracks->mSensorTracks.end())
                     {
                        mTargetIndex               = trackList->second.mMsgCreate->targetIndex();
                        rv::ResultPlatform* target = db->FindPlatform(mTargetIndex);
                        if (target)
                        {
                           mTargetName = QString::fromStdString(target->GetName());
                        }
                     }
                  }
               }
            }
         }
         db->UnlockTrackDb();
      }
   }
}

void RvTracks::PlotUpdater::FindLocationTruthAtTime(double aTime, rv::Vec3d& aValue) const
{
   rv::ResultData* results = rvEnv.GetData();
   if (nullptr != results)
   {
      rv::ResultDb* db = results->GetDb();
      if (nullptr != db)
      {
         rv::ResultPlatform* platformPtr = db->FindPlatform(mTargetIndex);
         if (platformPtr)
         {
            auto before = platformPtr->FindFirstBefore<rv::MsgEntityState>(aTime);
            auto after  = platformPtr->FindFirstAfter<rv::MsgEntityState>(aTime);
            if ((before != nullptr) && (after != nullptr))
            {
               rv::Vec3d b  = before->state().locationWCS();
               double    bt = (aTime - before->simTime()) / (after->simTime() - before->simTime());
               rv::Vec3d a  = after->state().locationWCS();
               double    at = (after->simTime() - aTime) / (after->simTime() - before->simTime());
               aValue.x(b.x() * at + a.x() * bt);
               aValue.y(b.y() * at + a.y() * bt);
               aValue.z(b.z() * at + a.z() * bt);
            }
            else if (before != nullptr)
            {
               aValue = before->state().locationWCS();
            }
            else if (after != nullptr)
            {
               aValue = after->state().locationWCS();
            }
         }
      }
   }
}
