// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptTrackClass.hpp"

#include <memory>
#include <sstream>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfCovariance.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptContext.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTypes.hpp"
#include "WsfUtil.hpp"
#include "WsfZone.hpp"

namespace
{
//! Initialize a UtEntity that represents the supplied track.
//! The heading is aligned with the velocity vector.
bool InitializeEntity(double aSimTime, WsfTrack* aTrackPtr, UtEntity& aEntity)
{
   double trackLocWCS[3];
   bool   ok = aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, trackLocWCS);
   if (ok)
   {
      double heading = 0.0;
      aEntity.SetLocationWCS(trackLocWCS);
      if (aTrackPtr->VelocityValid())
      {
         double trackVelWCS[3];
         aTrackPtr->GetVelocityWCS(trackVelWCS);
         aEntity.SetVelocityWCS(trackVelWCS);
         double trackVelNED[3];
         aEntity.GetVelocityNED(trackVelNED);
         heading = atan2(trackVelNED[1], trackVelNED[0]);
      }
      aEntity.SetOrientationNED(heading, 0.0, 0.0);
   }
   return ok;
}

double TrueBearing(double aSimTime, WsfTrack* aThisPtr, const double aOtherLocWCS[3])
{
   double   bearing = 0.0;
   UtEntity thisEntity;
   if (InitializeEntity(aSimTime, aThisPtr, thisEntity))
   {
      bearing = thisEntity.TrueBearing(aOtherLocWCS);
   }
   return bearing;
}

double RelativeBearing(double aSimTime, WsfTrack* aThisPtr, const double aOtherLocWCS[3])
{
   double   bearing = 0.0;
   UtEntity thisEntity;
   if (InitializeEntity(aSimTime, aThisPtr, thisEntity))
   {
      bearing = thisEntity.RelativeBearing(aOtherLocWCS);
   }
   return bearing;
}

//! Compute the approximate ground range between a platform and another point.
double GroundRange(double aSimTime, WsfTrack* aThisPtr, const double aOtherLocWCS[3])
{
   double groundRange = 0.0;
   double thisLocWCS[3];
   if (aThisPtr->GetExtrapolatedLocationWCS(aSimTime, thisLocWCS))
   {
      groundRange = UtMeasurementUtil::GroundRange(thisLocWCS, aOtherLocWCS);
   }
   return groundRange;
}

//! Compute the slant range between a track and another point.
double SlantRange(double aSimTime, WsfTrack* aThisPtr, const double aOtherLocWCS[3], const bool aUseSpherical = false)
{
   double slantRange = 0.0;
   double thisLocWCS[3];
   if (aThisPtr->GetExtrapolatedLocationWCS(aSimTime, thisLocWCS))
   {
      if (aUseSpherical)
      {
         slantRange = UtMeasurementUtil::SlantRange(thisLocWCS, aOtherLocWCS);
      }
      else
      {
         double thisToOtherLocWCS[3];
         UtVec3d::Subtract(thisToOtherLocWCS, thisLocWCS, aOtherLocWCS);
         slantRange = UtVec3d::Magnitude(thisToOtherLocWCS);
      }
   }
   return slantRange;
}

//! Compute the down range between a platform and another point.
//! Positive ahead, negative behind
double DownRange(double aSimTime, WsfTrack* aThisPtr, const double aOtherLocWCS[3])
{
   double   groundRange = GroundRange(aSimTime, aThisPtr, aOtherLocWCS);
   UtEntity thisEntity;
   InitializeEntity(aSimTime, aThisPtr, thisEntity);
   double bearing = thisEntity.RelativeBearing(aOtherLocWCS);
   return groundRange * cos(bearing);
}

//! Compute the cross range between a platform and another point.
//! Positive right, negative left
double CrossRange(double aSimTime, WsfTrack* aThisPtr, const double aOtherLocWCS[3])
{
   double   groundRange = GroundRange(aSimTime, aThisPtr, aOtherLocWCS);
   UtEntity thisEntity;
   InitializeEntity(aSimTime, aThisPtr, thisEntity);
   double bearing = thisEntity.RelativeBearing(aOtherLocWCS);
   return groundRange * sin(bearing);
}

void VelocityWCS(const double  aOriginLocationWCS[3],
                 const double& aSpeed,
                 const double& aBearing,
                 const double& aElevation,
                 double        aVelocityWCS[3])
{
   double sinBearing = sin(aBearing);
   double cosBearing = cos(aBearing);
   double lat, lon, alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aOriginLocationWCS, lat, lon, alt);
   double transECEF[3][3];
   double refLocWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, transECEF, refLocWCS);
   if (aElevation != 0.0)
   {
      double sinElevation = sin(aElevation);
      double cosElevation = cos(aElevation);
      double myVelNED[3] = {aSpeed * cosBearing * cosElevation, aSpeed * sinBearing * cosElevation, -aSpeed * sinElevation};
      UtMat3d::InverseTransform(aVelocityWCS, transECEF, myVelNED);
   }
   else
   {
      double myVelNED[3] = {aSpeed * cosBearing, aSpeed * sinBearing, 0.0};
      UtMat3d::InverseTransform(aVelocityWCS, transECEF, myVelNED);
   }
}

void VelocityWCS(const double aOriginLocationWCS[3], const double aVelocityNED[3], double aVelocityWCS[3])
{
   double lat, lon, alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aOriginLocationWCS, lat, lon, alt);
   double transECEF[3][3];
   double refLocWCS[3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, transECEF, refLocWCS);
   UtMat3d::InverseTransform(aVelocityWCS, transECEF, aVelocityNED);
}

//! Return the unextrapolated LLA location from a track.
void GetLocationLLA(const WsfTrack* aTrackPtr, double& aLat, double& aLon, double& aAlt)
{
   // Assume location is unknown.

   aLat = 0.0;
   aLon = 0.0;
   aAlt = 0.0;
   if (aTrackPtr != nullptr)
   {
      if (aTrackPtr->LocationValid())
      {
         // Location is directly known.
         aTrackPtr->GetLocationLLA(aLat, aLon, aAlt);
      }
      else
      {
         // Location is not directly known. Attempt to derive it from other sources.
         double locWCS[3];
         if (aTrackPtr->GetExtrapolatedLocationWCS(aTrackPtr->GetUpdateTime(), locWCS))
         {
            WsfPlatform::ConvertWCSToLLA(locWCS, aLat, aLon, aAlt);
         }
      }
   }
}

void GetLocationWCS(const WsfTrack* aTrackPtr, double aLocationWCS[3])
{
   UtVec3d::Set(aLocationWCS, 0.0); // Assume location is unknown.
   if (aTrackPtr != nullptr)
   {
      if (aTrackPtr->LocationValid())
      {
         // Location is directly known
         aTrackPtr->GetLocationWCS(aLocationWCS);
      }
      else
      {
         // Location is not directly known. Attempt to derive it from other sources.
         aTrackPtr->GetExtrapolatedLocationWCS(aTrackPtr->GetUpdateTime(), aLocationWCS);
      }
   }
}
} // end anonymous namespace

WsfScriptTrackClass::WsfScriptTrackClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfTrack");

   mConstructible = true;
   mCloneable     = true;

   // Add each of the method objects to the class.
   AddMethod(ut::make_unique<Initialize>());
   AddMethod(ut::make_unique<Update>());          // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<KinematicUpdate>()); // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<TrackId>());
   AddMethod(ut::make_unique<StartTime>());
   AddMethod(ut::make_unique<TimeSinceStarted>());
   AddMethod(ut::make_unique<UpdateTime>());
   AddMethod(ut::make_unique<SetUpdateTime>());
   AddMethod(ut::make_unique<TimeSinceUpdated>());
   AddMethod(ut::make_unique<UpdateCount>());
   AddMethod(ut::make_unique<IsStale>());
   AddMethod(ut::make_unique<IsPredefined>());
   AddMethod(ut::make_unique<IsFalseTarget>());
   AddMethod(ut::make_unique<SetStale>());
   AddMethod(ut::make_unique<Originator>());
   AddMethod(ut::make_unique<OriginatorName>());
   AddMethod(ut::make_unique<OriginatorType>());
   AddMethod(ut::make_unique<ProcessorName>());
   AddMethod(ut::make_unique<ProcessorType>());
   AddMethod(ut::make_unique<SensorName>());
   AddMethod(ut::make_unique<SensorType>());
   AddMethod(ut::make_unique<SensorMode>());
   AddMethod(ut::make_unique<RangeValid>());
   AddMethod(ut::make_unique<Range>());
   AddMethod(ut::make_unique<RangeRate>());
   AddMethod(ut::make_unique<SetRange>());
   AddMethod(ut::make_unique<RangeErrorSigma>());
   AddMethod(ut::make_unique<RangeRateErrorSigma>());
   AddMethod(ut::make_unique<BearingValid>());
   AddMethod(ut::make_unique<Bearing>());
   AddMethod(ut::make_unique<SetBearing>());
   AddMethod(ut::make_unique<BearingErrorSigma>());
   AddMethod(ut::make_unique<ElevationValid>());
   AddMethod(ut::make_unique<Elevation>());
   AddMethod(ut::make_unique<SetElevation>());
   AddMethod(ut::make_unique<ElevationErrorSigma>());
   AddMethod(ut::make_unique<TypeValid>());
   AddMethod(ut::make_unique<TypesValid>());
   AddMethod(ut::make_unique<Type>());
   AddMethod(ut::make_unique<Types>());
   AddMethod(ut::make_unique<SetType>());
   AddMethod(ut::make_unique<SetTypes>());
   AddMethod(ut::make_unique<SideValid>());
   AddMethod(ut::make_unique<Side>());
   AddMethod(ut::make_unique<SetSide>());
   AddMethod(ut::make_unique<IFF_Unknown>());
   AddMethod(ut::make_unique<IFF_Ambiguous>());
   AddMethod(ut::make_unique<IFF_Friend>());
   AddMethod(ut::make_unique<IFF_Foe>());
   AddMethod(ut::make_unique<IFF_Neutral>());
   AddMethod(ut::make_unique<SetIFF_Unknown>());
   AddMethod(ut::make_unique<SetIFF_Ambiguous>());
   AddMethod(ut::make_unique<SetIFF_Friend>());
   AddMethod(ut::make_unique<SetIFF_Foe>());
   AddMethod(ut::make_unique<SetIFF_Neutral>());
   AddMethod(ut::make_unique<LandDomain>());
   AddMethod(ut::make_unique<AirDomain>());
   AddMethod(ut::make_unique<SurfaceDomain>());
   AddMethod(ut::make_unique<SubsurfaceDomain>());
   AddMethod(ut::make_unique<SpaceDomain>());
   AddMethod(ut::make_unique<SetLandDomain>());
   AddMethod(ut::make_unique<SetAirDomain>());
   AddMethod(ut::make_unique<SetSurfaceDomain>());
   AddMethod(ut::make_unique<SetSubsurfaceDomain>());
   AddMethod(ut::make_unique<SetSpaceDomain>());
   AddMethod(ut::make_unique<IsA_Candidate>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<SignalToNoiseValid>());
   AddMethod(ut::make_unique<SignalToNoise>());
   AddMethod(ut::make_unique<SetSignalToNoise>());
   AddMethod(ut::make_unique<LocationValid>());
   AddMethod(ut::make_unique<LocationValid2D>());
   AddMethod(ut::make_unique<AltitudeKnown>());
   AddMethod(ut::make_unique<ReportedLocation>());
   AddMethod(ut::make_unique<CurrentLocation>());
   AddMethod(ut::make_unique<LocationAtTime>());
   AddMethod(ut::make_unique<Latitude>());
   AddMethod(ut::make_unique<Longitude>());
   AddMethod(ut::make_unique<Altitude>());
   AddMethod(ut::make_unique<X>());
   AddMethod(ut::make_unique<Y>());
   AddMethod(ut::make_unique<Z>());
   AddMethod(ut::make_unique<SetLocation>());
   AddMethod(ut::make_unique<SetLocationLLA>());
   AddMethod(ut::make_unique<SetLocationWCS>());
   AddMethod(ut::make_unique<SetLocation>("SetCurrentLocation")); // NO_DOC | DEPRECATED - For backward compatibility
   AddMethod(ut::make_unique<Speed>());
   AddMethod(ut::make_unique<Heading>());
   AddMethod(ut::make_unique<HeadingValid>());
   AddMethod(ut::make_unique<VelocityValid>());
   AddMethod(ut::make_unique<Vx>());
   AddMethod(ut::make_unique<Vy>());
   AddMethod(ut::make_unique<Vz>());
   AddMethod(ut::make_unique<VelocityWCS>());
   AddMethod(ut::make_unique<VelocityNED>());
   AddMethod(ut::make_unique<SetVelocityWCS>());
   AddMethod(ut::make_unique<SetVelocityNED>());
   AddMethod(ut::make_unique<SetVelocity_1>("SetVelocity"));
   AddMethod(ut::make_unique<SetVelocity_2>("SetVelocity"));
   AddMethod(ut::make_unique<TrackQuality_1>("TrackQuality"));
   AddMethod(ut::make_unique<TrackQuality_2>("TrackQuality"));
   AddMethod(ut::make_unique<SetTrackQuality>());
   AddMethod(ut::make_unique<Quality_1>("Quality"));
   AddMethod(ut::make_unique<Quality_2>("Quality"));
   AddMethod(ut::make_unique<PixelCount>());
   AddMethod(ut::make_unique<SetPixelCount>());
   AddMethod(ut::make_unique<StateCovariance>());
   AddMethod(ut::make_unique<StateCovarianceValid>());
   AddMethod(ut::make_unique<StateCovarianceAtTime>());
   AddMethod(ut::make_unique<MeasurementCovariance>());
   AddMethod(ut::make_unique<MeasurementCovarianceValid>());
   AddMethod(ut::make_unique<Target>());
   AddMethod(ut::make_unique<TargetIndex>());
   AddMethod(ut::make_unique<TargetName>());
   AddMethod(ut::make_unique<TargetType>());
   AddMethod(ut::make_unique<SetTarget>());
   AddMethod(ut::make_unique<ClearTarget>());
   AddMethod(ut::make_unique<TargetDamaged>());

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);

   AddMethod(ut::make_unique<SetGlobalTrackNumber>());
   AddMethod(ut::make_unique<GlobalTrackNumber>());

   AddMethod(ut::make_unique<HasHistory>());
   AddMethod(ut::make_unique<History>());

   AddMethod(ut::make_unique<OriginatorLocation>());
   AddMethod(ut::make_unique<SetOriginator>()); // NO_DOC | DEPRECATED

   AddMethod(ut::make_unique<FrequencyValid>());
   AddMethod(ut::make_unique<Frequency>());
   AddMethod(ut::make_unique<SetFrequency>());
   AddMethod(ut::make_unique<SignalCount>());
   AddMethod(ut::make_unique<SignalFrequency>());
   AddMethod(ut::make_unique<SignalPRI>());
   AddMethod(ut::make_unique<SignalPW>());
   AddMethod(ut::make_unique<SignalEmitterType>());

   AddMethod(ut::make_unique<EmitterCount>());
   AddMethod(ut::make_unique<EmitterType>());
   AddMethod(ut::make_unique<EmitterSignalIndexCount>());
   AddMethod(ut::make_unique<EmitterSignalIndexEntry>());

   AddMethod(ut::make_unique<TrueBearingTo_1>("TrueBearingTo"));         // TrueBearingTo(WsfTrack)
   AddMethod(ut::make_unique<TrueBearingTo_2>("TrueBearingTo"));         // TrueBearingTo(WsfPlatform)
   AddMethod(ut::make_unique<TrueBearingTo_3>("TrueBearingTo"));         // TrueBearingTo(lat, lon, alt)
   AddMethod(ut::make_unique<TrueBearingTo_4>("TrueBearingTo"));         // TrueBearingTo(WsfGeoPoint)
   AddMethod(ut::make_unique<RelativeBearingTo_1>("RelativeBearingTo")); // RelativeBearingTo(WsfTrack)
   AddMethod(ut::make_unique<RelativeBearingTo_2>("RelativeBearingTo")); // RelativeBearingTo(WsfPlatform)
   AddMethod(ut::make_unique<RelativeBearingTo_3>("RelativeBearingTo")); // RelativeBearingTo(lat, lon, alt)
   AddMethod(ut::make_unique<RelativeBearingTo_4>("RelativeBearingTo")); // RelativeBearingTo(lat, lon, alt)
   AddMethod(ut::make_unique<SlantRangeTo_1>("SlantRangeTo"));           // SlantRangeTo(WsfTrack)
   AddMethod(ut::make_unique<SlantRangeTo_1s>("SlantRangeTo"));          // SlantRangeTo(WsfTrack, bool)
   AddMethod(ut::make_unique<SlantRangeTo_2>("SlantRangeTo"));           // SlantRangeTo(WsfPlatform)
   AddMethod(ut::make_unique<SlantRangeTo_2s>("SlantRangeTo"));          // SlantRangeTo(WsfPlatform, bool)
   AddMethod(ut::make_unique<SlantRangeTo_3>("SlantRangeTo"));           // SlantRangeTo(lat, lon, alt)
   AddMethod(ut::make_unique<SlantRangeTo_3s>("SlantRangeTo"));          // SlantRangeTo(lat, lon, alt, bool)
   AddMethod(ut::make_unique<SlantRangeTo_4>("SlantRangeTo"));           // SlantRangeTo(WsfGeoPoint)
   AddMethod(ut::make_unique<SlantRangeTo_4s>("SlantRangeTo"));          // SlantRangeTo(WsfGeoPoint, bool)
   AddMethod(ut::make_unique<GroundRangeTo_1>("GroundRangeTo"));         // GroundRangeTo(WsfTrack)
   AddMethod(ut::make_unique<GroundRangeTo_2>("GroundRangeTo"));         // GroundRangeTo(WsfPlatform)
   AddMethod(ut::make_unique<GroundRangeTo_3>("GroundRangeTo"));         // GroundRangeTo(lat, lon, alt)
   AddMethod(ut::make_unique<GroundRangeTo_4>("GroundRangeTo"));         // GroundRangeTo(WsfGeoPoint)
   AddMethod(ut::make_unique<DownRangeTo_1>("DownRangeTo"));             // DownRangeTo(WsfTrack)
   AddMethod(ut::make_unique<DownRangeTo_2>("DownRangeTo"));             // DownRangeTo(WsfPlatform)
   AddMethod(ut::make_unique<DownRangeTo_3>("DownRangeTo"));             // DownRangeTo(lat, lon, alt)
   AddMethod(ut::make_unique<DownRangeTo_4>("DownRangeTo"));             // DownRangeTo(WsfGeoPoint)
   AddMethod(ut::make_unique<CrossRangeTo_1>("CrossRangeTo"));           // CrossRangeTo(WsfTrack)
   AddMethod(ut::make_unique<CrossRangeTo_2>("CrossRangeTo"));           // CrossRangeTo(WsfPlatform)
   AddMethod(ut::make_unique<CrossRangeTo_3>("CrossRangeTo"));           // CrossRangeTo(lat, lon, alt)
   AddMethod(ut::make_unique<CrossRangeTo_4>("CrossRangeTo"));           // CrossRangeTo(WsfGeoPoint)

   AddMethod(ut::make_unique<AzimuthLOSR_From>());   // AzimuthLOSR_From(WsfPlatform a Observer)
   AddMethod(ut::make_unique<ElevationLOSR_From>()); // ElevationLOSR_From(WsfPlatform a Observer)

   AddMethod(ut::make_unique<WithinZoneOf_1>("WithinZoneOf")); // WithinZoneOf(WsfPlatform, zone name)
   AddMethod(ut::make_unique<WithinZoneOf_2>("WithinZoneOf")); // WithinZoneOf(WsfPlatform, zone name, sensor name)

   AddMethod(ut::make_unique<BallisticPropagate>()); // // NO_DOC | PENDING
}

#include "script/WsfScriptDefs.hpp"

// virtual
void* WsfScriptTrackClass::Create(const UtScriptContext& aInstance)
{
   WsfTrack* trackPtr = new WsfTrack();
   double    simTime  = WsfScriptContext::GetTIME_NOW(aInstance);

   // Define the track originator as the platform that called this method and set location of the originator.
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aInstance);
   if (platformPtr != nullptr)
   {
      platformPtr->Update(simTime); // Ensure the position is current
      double locationWCS[3];
      platformPtr->GetLocationWCS(locationWCS);
      trackPtr->SetOriginatorLocationWCS(locationWCS);
      trackPtr->SetPlatformOriginator(platformPtr);
   }

   // Mark the track as 'processed' and 'fusible'. This allows it participate in to be fused with
   // other tracks if passed to a track manager.
   trackPtr->SetTrackType(WsfTrack::cPROCESSED);
   trackPtr->SetFusible(true);

   // A track ID is not assigned at this time. In almost all cases when a track is created from script, it is created
   // only for the purpose of intercept or geometry calculations (They are rarely, if ever, passed off to the track
   // manager). If a new track ID was assigned for every such case, we would probably run out of ID's.
   trackPtr->Initialize(simTime, WsfTrackId(), *WsfScriptContext::GetSIMULATION(aInstance));
   trackPtr->Update(simTime);
   return trackPtr;
}

// virtual
void* WsfScriptTrackClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfTrack*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptTrackClass::Destroy(void* aObjectPtr)
{
   WsfTrack* objPtr = static_cast<WsfTrack*>(aObjectPtr);
   delete objPtr;
}

// virtual
std::string WsfScriptTrackClass::ToString(void* aObjectPtr) const
{
   std::stringstream ss;
   ss << "WsfTrack(";
   if (aObjectPtr != nullptr)
   {
      WsfTrack* trackPtr = (WsfTrack*)aObjectPtr;
      ss << "ID: " << trackPtr->GetTrackId();
   }
   else
   {
      ss << "null";
   }
   ss << ')';
   return ss.str();
}

// virtual
void WsfScriptTrackClass::OnNewScriptRef(UtScriptRef& aReference)
{
   if (aReference.GetMemManagement() != UtScriptRef::cMANAGE)
   {
      WsfTrack* trackPtr = static_cast<WsfTrack*>(aReference.GetAppObject());
      if (trackPtr != nullptr)
      {
         aReference.SetExternalReference(trackPtr->GetReferenceCount());
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Initialize, 0, "void", "")
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aContext);
   if (platformPtr != nullptr)
   {
      aObjectPtr->SetPlatformOriginator(platformPtr);
      aObjectPtr->Initialize(TIME_NOW, platformPtr->GetNextTrackId(), *SIMULATION);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Update, 0, "void", "") // NO_DOC | DEPRECATED
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);
   WsfPlatform* platformPtr = PLATFORM;
   if (platformPtr != nullptr)
   {
      platformPtr->Update(simTime); // Ensure the position is current
      double locationWCS[3];
      platformPtr->GetLocationWCS(locationWCS);
      aObjectPtr->SetOriginatorLocationWCS(locationWCS);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, KinematicUpdate, 0, "void", "") // NO_DOC | FOR_TEST_ONLY
{
   aObjectPtr->KinematicUpdate(TIME_NOW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, StartTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetStartTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TimeSinceStarted, 0, "double", "")
{
   aReturnVal.SetDouble(TIME_NOW - aObjectPtr->GetStartTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, UpdateTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetUpdateTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetUpdateTime, 1, "void", "double")
{
   aObjectPtr->SetUpdateTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TimeSinceUpdated, 0, "double", "")
{
   aReturnVal.SetDouble(TIME_NOW - aObjectPtr->GetUpdateTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, UpdateCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetUpdateCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, IsStale, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsStale());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, IsPredefined, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsPredefinedTrack());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, IsFalseTarget, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsFalseTarget());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetStale, 1, "void", "bool")
{
   aObjectPtr->SetStale(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Originator, 0, "WsfPlatform", "")
{
   WsfPlatform* originatorPtr = SIMULATION->GetPlatformByName(aObjectPtr->GetOriginatorNameId());
   aReturnVal.SetPointer(new UtScriptRef(originatorPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, OriginatorName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetOriginatorNameId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, OriginatorType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetOriginatorTypeId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, ProcessorName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetProcessorNameId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, ProcessorType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetProcessorTypeId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SensorName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSensorNameId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SensorType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSensorTypeId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SensorMode, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSensorModeId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TrackId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetTrackId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, RangeValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->RangeValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Range, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRange());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, RangeRate, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRangeRate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, RangeErrorSigma, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRangeError());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, RangeRateErrorSigma, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRangeRateError());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetRange, 1, "void", "double")
{
   aObjectPtr->SetRange(aVarArgs[0].GetDouble());
   aObjectPtr->SetRangeValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, BearingValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->BearingValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Bearing, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBearing() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetBearing, 1, "void", "double")
{
   aObjectPtr->SetBearing(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aObjectPtr->SetBearingValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, BearingErrorSigma, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBearingError());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, ElevationValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->ElevationValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Elevation, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetElevation() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetElevation, 1, "void", "double")
{
   aObjectPtr->SetElevation(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   aObjectPtr->SetElevationValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, ElevationErrorSigma, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetElevationError());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TypeValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->TypeIdValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TypesValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->TypeIdsValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Type, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTypeId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Types, 0, "Map<string, double>", "")
{
   const WsfScenario&                 scenario        = aObjectPtr->GetSimulation()->GetScenario();
   UtScriptTypes*                     scriptTypesPtr  = scenario.GetScriptTypes();
   static UtScriptMap&                sScriptMapClass = (UtScriptMap&)(*scriptTypesPtr->GetClass("Map<string,double>"));
   UtScriptMap::Map*                  mapPtr          = new UtScriptMap::Map;
   UtScriptMap::Map&                  map             = *mapPtr;
   const UtStringIdInformationVector& types           = aObjectPtr->GetTypeIds();
   for (const auto& type : types)
   {
      UtScriptData stringData(type.Get());
      UtScriptData probData(type.GetQuality());
      map[stringData] = probData;
   }
   UtScriptRef* mapRefPtr = new UtScriptRef(mapPtr, &sScriptMapClass, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(mapRefPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetType, 1, "void", "string")
{
   aObjectPtr->SetTypeId(WsfStringId(aVarArgs[0].GetString()));
   aObjectPtr->SetTypeIdValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetTypes, 1, "void", "Map<string, double>")
{
   UtScriptMap::Map*           mapPtr = (UtScriptMap::Map*)aVarArgs[0].GetPointer()->GetAppObject();
   UtScriptMap::Map::iterator  iter   = mapPtr->begin();
   UtStringIdInformationVector types;
   while (iter != mapPtr->end())
   {
      // std::pair<UtScriptData, UtScriptData> data = *iter;
      UtStringIdInformation type;
      type.Set(iter->first.GetString());
      type.SetQuality(iter->second.GetDouble());
      type.SetValid(true);
      types.push_back(type);
      ++iter;
   }

   aObjectPtr->SetTypeIds(types);
   aObjectPtr->SetTypeIdsValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SideValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->SideIdValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Side, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSideId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetSide, 1, "void", "string")
{
   aObjectPtr->SetSideId(WsfStringId(aVarArgs[0].GetString()));
   aObjectPtr->SetSideIdValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, IFF_Unknown, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIFF_Status() == WsfTrack::cIFF_UNKNOWN);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, IFF_Ambiguous, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIFF_Status() == WsfTrack::cIFF_AMBIGUOUS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, IFF_Foe, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIFF_Status() == WsfTrack::cIFF_FOE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, IFF_Friend, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIFF_Status() == WsfTrack::cIFF_FRIEND);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, IFF_Neutral, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetIFF_Status() == WsfTrack::cIFF_NEUTRAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetIFF_Unknown, 0, "void", "")
{
   aObjectPtr->SetIFF_Status(WsfTrack::cIFF_UNKNOWN);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetIFF_Ambiguous, 0, "void", "")
{
   aObjectPtr->SetIFF_Status(WsfTrack::cIFF_AMBIGUOUS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetIFF_Foe, 0, "void", "")
{
   aObjectPtr->SetIFF_Status(WsfTrack::cIFF_FOE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetIFF_Friend, 0, "void", "")
{
   aObjectPtr->SetIFF_Status(WsfTrack::cIFF_FRIEND);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetIFF_Neutral, 0, "void", "")
{
   aObjectPtr->SetIFF_Status(WsfTrack::cIFF_NEUTRAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, LandDomain, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, AirDomain, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_AIR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SurfaceDomain, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SURFACE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SubsurfaceDomain, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SUBSURFACE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SpaceDomain, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SPACE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetLandDomain, 0, "void", "")
{
   aObjectPtr->SetSpatialDomain(WSF_SPATIAL_DOMAIN_LAND);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetAirDomain, 0, "void", "")
{
   aObjectPtr->SetSpatialDomain(WSF_SPATIAL_DOMAIN_AIR);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetSurfaceDomain, 0, "void", "")
{
   aObjectPtr->SetSpatialDomain(WSF_SPATIAL_DOMAIN_SURFACE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetSubsurfaceDomain, 0, "void", "")
{
   aObjectPtr->SetSpatialDomain(WSF_SPATIAL_DOMAIN_SUBSURFACE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetSpaceDomain, 0, "void", "")
{
   aObjectPtr->SetSpatialDomain(WSF_SPATIAL_DOMAIN_SPACE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, IsA_Candidate, 0, "bool", "") // NO_DOC | DEPRECATED
{
   aReturnVal.SetBool(aObjectPtr->IsCandidate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SignalToNoiseValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->SignalToNoiseValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SignalToNoise, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSignalToNoise());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetSignalToNoise, 1, "void", "double")
{
   aObjectPtr->SetSignalToNoise(aVarArgs[0].GetDouble());
   aObjectPtr->SetSignalToNoiseValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, LocationValid, 0, "bool", "")
{
   // Although this LocationValid script method has the same name, it is not the same
   // as the WsfTrack::LocationValid method. This script method will return 'true' only
   // when we know the full 3D position as either an LLA (e.g. from a GPS) or an RBE
   // (e.g. Range/Bearing/Elevation from a sensor whose location is known).  The
   // WsfTrack::LocationValid method may return 'true' if the altitude has been 'guessed'.
   // This LocationValid script method returns 'true' only when we KNOW the altitude.

   aReturnVal.SetBool(aObjectPtr->Is3D());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, LocationValid2D, 0, "bool", "")
{
   // As with the previous LocationValid script method, LocationValid2D will return 'true'
   // when the track has the full 3D position.  In addition, LocationValid2D will return 'true'
   // if the altitude has been 'guessed'.
   aReturnVal.SetBool(aObjectPtr->LocationValid() || aObjectPtr->Is3D() ||
                      (aObjectPtr->RangeValid() && aObjectPtr->BearingValid()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, AltitudeKnown, 0, "bool", "")
{
   // For now we will require knowledge of horizontal AND vertical components (i.e. the
   // AltitudeKnown script method is the same as the LocationValid script method above).
   aReturnVal.SetBool(aObjectPtr->Is3D());
}

// WsfGeoPoint point = <track>.ReportedLocation();
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, ReportedLocation, 0, "WsfGeoPoint", "")
{
   // Get the last reported target location.
   double targetLocWCS[3] = {0.0, 0.0, 0.0}; // In case location cannot be determined.
   double simTime         = aObjectPtr->GetUpdateTime();
   aObjectPtr->GetExtrapolatedLocationWCS(simTime, targetLocWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(targetLocWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// WsfGeoPoint point = <track>.CurrentLocation();
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, CurrentLocation, 0, "WsfGeoPoint", "")
{
   // Get the target location.
   double targetLocWCS[3] = {0.0, 0.0, 0.0}; // In case location cannot be determined.
   double simTime         = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->GetExtrapolatedLocationWCS(simTime, targetLocWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(targetLocWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// WsfGeoPoint point = <track>.LocationAtTime(double aTime);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, LocationAtTime, 1, "WsfGeoPoint", "double")
{
   // Get the target location.
   double targetLocWCS[3] = {0.0, 0.0, 0.0}; // In case location cannot be determined.
   double simTime         = aVarArgs[0].GetDouble();
   aObjectPtr->GetExtrapolatedLocationWCS(simTime, targetLocWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(targetLocWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Latitude, 0, "double", "")
{
   double lla[3];
   GetLocationLLA(aObjectPtr, lla[0], lla[1], lla[2]);
   aReturnVal.SetDouble(lla[0]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Longitude, 0, "double", "")
{
   double lla[3];
   GetLocationLLA(aObjectPtr, lla[0], lla[1], lla[2]);
   aReturnVal.SetDouble(lla[1]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Altitude, 0, "double", "")
{
   double lla[3];
   GetLocationLLA(aObjectPtr, lla[0], lla[1], lla[2]);
   aReturnVal.SetDouble(lla[2]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, X, 0, "double", "")
{
   double xyz[3];
   GetLocationWCS(aObjectPtr, xyz);
   aReturnVal.SetDouble(xyz[0]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Y, 0, "double", "")
{
   double xyz[3];
   GetLocationWCS(aObjectPtr, xyz);
   aReturnVal.SetDouble(xyz[1]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Z, 0, "double", "")
{
   double xyz[3];
   GetLocationWCS(aObjectPtr, xyz);
   aReturnVal.SetDouble(xyz[2]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetLocation, 1, "void", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       xyz[3];
   pointPtr->GetLocationWCS(xyz);
   aObjectPtr->SetLocationWCS(xyz);
   // Note: to fuse correctly, the track must be 3d or have range/bearing.
   aObjectPtr->Set3D(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetLocationLLA, 3, "void", "double, double, double")
{
   aObjectPtr->SetLocationLLA(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
   // Note: to fuse correctly, the track must be 3d or have range/bearing.
   aObjectPtr->Set3D(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetLocationWCS, 3, "void", "double, double, double")
{
   double xyz[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   aObjectPtr->SetLocationWCS(xyz);
   // Note: to fuse correctly, the track must be 3d or have range/bearing.
   aObjectPtr->Set3D(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Speed, 0, "double", "")
{
   double speed = 0.0;
   if (aObjectPtr->VelocityValid())
   {
      double velWCS[3];
      aObjectPtr->GetVelocityWCS(velWCS);
      speed = UtVec3d::Magnitude(velWCS);
   }
   aReturnVal.SetDouble(speed);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Heading, 0, "double", "")
{
   double heading = 0.0;
   if (aObjectPtr->LocationValid() && aObjectPtr->VelocityValid())
   {
      double trackLocWCS[3];
      aObjectPtr->GetLocationWCS(trackLocWCS);
      double trackVelWCS[3];
      aObjectPtr->GetVelocityWCS(trackVelWCS);
      UtEntity entity;
      entity.SetLocationWCS(trackLocWCS);
      entity.SetVelocityWCS(trackVelWCS);
      double trackVelNED[3];
      entity.GetVelocityNED(trackVelNED);
      heading = atan2(trackVelNED[1], trackVelNED[0]);
      heading = UtMath::NormalizeAngle0_360(heading * UtMath::cDEG_PER_RAD);
   }
   aReturnVal.SetDouble(heading);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, HeadingValid, 0, "bool", "")
{
   bool headingValid = false;
   if (aObjectPtr->LocationValid() && aObjectPtr->VelocityValid())
   {
      headingValid = true;
   }
   aReturnVal.SetBool(headingValid);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, VelocityValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->VelocityValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Vx, 0, "double", "")
{
   double xyz[3];
   aObjectPtr->GetVelocityWCS(xyz);
   aReturnVal.SetDouble(xyz[0]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Vy, 0, "double", "")
{
   double xyz[3];
   aObjectPtr->GetVelocityWCS(xyz);
   aReturnVal.SetDouble(xyz[1]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Vz, 0, "double", "")
{
   double xyz[3];
   aObjectPtr->GetVelocityWCS(xyz);
   aReturnVal.SetDouble(xyz[2]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, VelocityWCS, 0, "Vec3", "")
{
   UtVec3d vel;
   aObjectPtr->GetVelocityWCS(vel.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(vel));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, VelocityNED, 0, "Vec3", "")
{
   UtVec3d vel;
   if (aObjectPtr->VelocityValid() && aObjectPtr->LocationValid())
   {
      UtVec3d locWCS, velWCS;
      aObjectPtr->GetVelocityWCS(velWCS.GetData());
      aObjectPtr->GetLocationWCS(locWCS.GetData());
      UtMeasurementUtil::LocationNED(locWCS.GetData(), (locWCS + velWCS).GetData(), vel.GetData());
   }
   aReturnVal.SetPointer(UtScriptVec3::Create(vel));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetVelocityWCS, 3, "void", "double, double, double")
{
   double vxyz[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   aObjectPtr->SetVelocityWCS(vxyz);
   aObjectPtr->SetVelocityValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetVelocityNED, 3, "void", "double, double, double")
{
   double vned[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   double xyz[3];
   aObjectPtr->GetLocationWCS(xyz);
   double vxyz[3];
   VelocityWCS(xyz, vned, vxyz);
   aObjectPtr->SetVelocityWCS(vxyz);
   aObjectPtr->SetVelocityValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetVelocity_1, 2, "void", "double, double")
{
   double xyz[3];
   aObjectPtr->GetLocationWCS(xyz);

   double vxyz[3];
   VelocityWCS(xyz, aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG, 0.0, vxyz);

   aObjectPtr->SetVelocityWCS(vxyz);
   aObjectPtr->SetVelocityValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetVelocity_2, 3, "void", "double, double, double")
{
   double xyz[3];
   aObjectPtr->GetLocationWCS(xyz);

   double vxyz[3];
   VelocityWCS(xyz,
               aVarArgs[0].GetDouble(),
               aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG,
               aVarArgs[2].GetDouble() * UtMath::cRAD_PER_DEG,
               vxyz);

   aObjectPtr->SetVelocityWCS(vxyz);
   aObjectPtr->SetVelocityValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, FrequencyValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->FrequencyValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Frequency, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetFrequency());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetFrequency, 1, "void", "double")
{
   aObjectPtr->SetFrequency(aVarArgs[0].GetDouble());
   aObjectPtr->SetFrequencyValid(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SignalCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetSignalCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SignalFrequency, 1, "double", "int")
{
   int              signalIndex = aVarArgs[0].GetInt();
   WsfTrack::Signal signal;
   aObjectPtr->GetSignalEntry(signalIndex, signal);
   aReturnVal.SetDouble(signal.mLowerFrequency + ((signal.mUpperFrequency - signal.mLowerFrequency) / 2));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SignalPRI, 1, "double", "int")
{
   int              signalIndex = aVarArgs[0].GetInt();
   WsfTrack::Signal signal;
   aObjectPtr->GetSignalEntry(signalIndex, signal);
   aReturnVal.SetDouble(signal.mPulseRepetitionInterval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SignalPW, 1, "double", "int")
{
   int              signalIndex = aVarArgs[0].GetInt();
   WsfTrack::Signal signal;
   aObjectPtr->GetSignalEntry(signalIndex, signal);
   aReturnVal.SetDouble(signal.mPulseWidth);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SignalEmitterType, 1, "string", "int")
{
   int              signalIndex = aVarArgs[0].GetInt();
   WsfTrack::Signal signal;
   aObjectPtr->GetSignalEntry(signalIndex, signal);
   aReturnVal.SetString(signal.mEmitterDerivedId);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, EmitterCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetEmitterTypeIdCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, EmitterType, 1, "string", "int")
{
   int                       emitterTypeIdIndex = aVarArgs[0].GetInt();
   WsfTrack::EmitterTypeData typeData;
   aObjectPtr->GetEmitterTypeIdEntry(emitterTypeIdIndex, typeData);
   aReturnVal.SetString(typeData.mEmitterDerivedId);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, EmitterSignalIndexCount, 1, "double", "int")
{
   int                       emitterTypeIdIndex = aVarArgs[0].GetInt();
   WsfTrack::EmitterTypeData typeData;
   aObjectPtr->GetEmitterTypeIdEntry(emitterTypeIdIndex, typeData);
   aReturnVal.SetDouble((double)typeData.mSignalIndices.size());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, EmitterSignalIndexEntry, 2, "int", "int, int")
{
   int                       emitterTypeIdIndex = aVarArgs[0].GetInt();
   int                       signalIndexEntry   = aVarArgs[1].GetInt();
   WsfTrack::EmitterTypeData typeData;
   aObjectPtr->GetEmitterTypeIdEntry(emitterTypeIdIndex, typeData);
   aReturnVal.SetInt(typeData.GetSignalIndexEntry(signalIndexEntry));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TrackQuality_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTrackQuality());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TrackQuality_2, 1, "double", "double")
{
   double updateTime = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(aObjectPtr->GetTrackQuality(updateTime));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetTrackQuality, 1, "void", "double")
{
   aObjectPtr->SetTrackQuality(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Quality_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetQuality());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Quality_2, 1, "double", "double")
{
   double updateTime = aVarArgs[0].GetDouble();
   aReturnVal.SetDouble(aObjectPtr->GetQuality(updateTime));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, PixelCount, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPixelCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetPixelCount, 1, "void", "double")
{
   aObjectPtr->SetPixelCount(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, StateCovariance, 0, "WsfCovariance", "")
{
   WsfCovariance* covariancePtr = nullptr;
   if (aObjectPtr->GetStateCovariance() != nullptr)
   {
      covariancePtr = const_cast<WsfCovariance*>(aObjectPtr->GetStateCovariance());
   }
   aReturnVal.SetPointer(new UtScriptRef(covariancePtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, StateCovarianceValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->StateCovarianceValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, StateCovarianceAtTime, 1, "WsfCovariance", "double")
{
   WsfCovariance* covariancePtr = nullptr;
   if (aObjectPtr->GetStateCovariance() != nullptr)
   {
      double simTime = aVarArgs[0].GetDouble();
      covariancePtr  = new WsfCovariance();
      if (!covariancePtr->ComputePredictedState(simTime, *aObjectPtr))
      {
         delete covariancePtr;
         covariancePtr = nullptr;
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(covariancePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, MeasurementCovariance, 0, "WsfCovariance", "")
{
   WsfCovariance* covariancePtr = nullptr;
   if (aObjectPtr->GetMeasurementCovariance() != nullptr)
   {
      covariancePtr = new WsfCovariance(*(aObjectPtr->GetMeasurementCovariance()));
   }
   aReturnVal.SetPointer(new UtScriptRef(covariancePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, MeasurementCovarianceValid, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->MeasurementCovarianceValid());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Target, 0, "WsfPlatform", "")
{
   WsfPlatform* targetPtr = SIMULATION->GetPlatformByIndex(aObjectPtr->GetTargetIndex());
   aReturnVal.SetPointer(new UtScriptRef(targetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TargetIndex, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetTargetIndex()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TargetName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTargetName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TargetType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTargetType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetTarget, 1, "void", "string")
{
   std::string  targetName        = aVarArgs[0].GetString();
   WsfPlatform* targetPlatformPtr = SIMULATION->GetPlatformByName(targetName);
   if (targetPlatformPtr != nullptr)
   {
      aObjectPtr->SetTargetName(targetPlatformPtr->GetNameId());
      aObjectPtr->SetTargetIndex(targetPlatformPtr->GetIndex());
      aObjectPtr->SetTargetType(targetPlatformPtr->GetTypeId());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, ClearTarget, 0, "void", "")
{
   aObjectPtr->SetTargetName(nullptr);
   aObjectPtr->SetTargetIndex(0);
   aObjectPtr->SetTargetType(nullptr);
}

//! Uses truth knowledge to report whether or not the target of the track is damaged or deleted
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TargetDamaged, 0, "double", "")
{
   WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(aObjectPtr->GetTargetIndex());

   double damFact = 0;

   if (platformPtr == nullptr)
   {
      damFact = 1;
   }
   else
   {
      damFact = platformPtr->GetDamageFactor();
   }

   aReturnVal.SetDouble(damFact);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetGlobalTrackNumber, 1, "void", "int")
{
   aObjectPtr->SetGlobalTrackNumber(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, GlobalTrackNumber, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetGlobalTrackNumber());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, HasHistory, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasHistory());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, History, 0, "Array<WsfTrack>", "")
{
   UtScriptClass*             classPtr = aContext.GetTypes()->GetClass(aReturnClassPtr->GetContainerDataTypeId());
   std::vector<UtScriptData>* arrayPtr = new std::vector<UtScriptData>();
   for (const auto& dataPtr : aObjectPtr->GetHistory())
   {
      if (dataPtr->IsTrack())
      {
         arrayPtr->push_back(UtScriptData(new UtScriptRef(static_cast<WsfTrack*>(dataPtr.get()), classPtr)));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(arrayPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, OriginatorLocation, 0, "WsfGeoPoint", "")
{
   // Get the target location.
   double locWCS[3];
   aObjectPtr->GetOriginatorLocationWCS(locWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SetOriginator, 1, "void", "WsfProcessor") // NO_DOC | DEPRECATED
{
   WsfProcessor* procPtr = (WsfProcessor*)aVarArgs[0].GetPointer()->GetAppObject();
   if (procPtr != nullptr)
   {
      aObjectPtr->SetProcessorOriginator(procPtr, procPtr->GetPlatform());
   }
}

// ----------------------------------------------------------------------------
//                         Relative geometry methods
// ----------------------------------------------------------------------------

//! double bearing = TrueBearingTo(WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TrueBearingTo_1, 1, "double", "WsfTrack")
{
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double bearing = TrueBearing(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = TrueBearingTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TrueBearingTo_2, 1, "double", "WsfPlatform")
{
   double       simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* otherPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   otherPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   otherPtr->GetLocationWCS(otherLocWCS);
   double bearing = TrueBearing(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = TrueBearingTo(double aLat, double aLon, double aAlt);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TrueBearingTo_3, 3, "double", "double, double, double")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double bearing = TrueBearing(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = TrueBearingTo(WsfGeoPoint aPoint);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TrueBearingTo_4, 1, "double", "WsfGeoPoint")
{
   double       simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double bearing = TrueBearing(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = RelativeBearingTo(WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, RelativeBearingTo_1, 1, "double", "WsfTrack")
{
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double bearing = RelativeBearing(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = RelativeBearingTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, RelativeBearingTo_2, 1, "double", "WsfPlatform")
{
   double       simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* otherPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   otherPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   otherPtr->GetLocationWCS(otherLocWCS);
   double bearing = RelativeBearing(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = RelativeBearingTo(double aLat, double aLon, double aAlt);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, RelativeBearingTo_3, 3, "double", "double, double, double")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double bearing = RelativeBearing(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = RelativeBearingTo(WsfGeoPoint aPoint);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, RelativeBearingTo_4, 1, "double", "WsfGeoPoint")
{
   double       simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double bearing = RelativeBearing(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double distance = SlantRangeTo(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SlantRangeTo_1, 1, "double", "WsfTrack")
{
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = SlantRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfTrack aTrack, bool aUseSpherical)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SlantRangeTo_1s, 2, "double", "WsfTrack, bool")
{
   double    simTime      = WsfScriptContext::GetTIME_NOW(aContext);
   WsfTrack* trackPtr     = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool      useSpherical = aVarArgs[1].GetBool();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = SlantRange(simTime, aObjectPtr, otherLocWCS, useSpherical);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SlantRangeTo_2, 1, "double", "WsfPlatform")
{
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = SlantRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfPlatform aPlatform, bool aUseSpherical);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SlantRangeTo_2s, 2, "double", "WsfPlatform, bool")
{
   double       simTime      = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* platformPtr  = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool         useSpherical = aVarArgs[1].GetBool();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = SlantRange(simTime, aObjectPtr, otherLocWCS, useSpherical);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(double aLat, double aLon, double aAlt);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SlantRangeTo_3, 3, "double", "double, double, double")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double distance = SlantRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(double aLat, double aLon, double aAlt, bool aUseSpherical);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SlantRangeTo_3s, 4, "double", "double, double, double, bool")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   bool   useSpherical = aVarArgs[3].GetBool();
   double distance     = SlantRange(simTime, aObjectPtr, otherLocWCS, useSpherical);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfGeoPoint aPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SlantRangeTo_4, 1, "double", "WsfGeoPoint")
{
   double       simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = SlantRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfGeoPoint aPoint, bool aUseSpherical)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, SlantRangeTo_4s, 2, "double", "WsfGeoPoint, bool")
{
   double       simTime      = WsfScriptContext::GetTIME_NOW(aContext);
   WsfGeoPoint* pointPtr     = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool         useSpherical = aVarArgs[1].GetBool();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = SlantRange(simTime, aObjectPtr, otherLocWCS, useSpherical);
   aReturnVal.SetDouble(distance);
}

//! double distance = GroundRangeTo(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, GroundRangeTo_1, 1, "double", "WsfTrack")
{
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = GroundRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = GroundRangeTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, GroundRangeTo_2, 1, "double", "WsfPlatform")
{
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = GroundRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = GroundRangeTo(double aLat, double aLon);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, GroundRangeTo_3, 3, "double", "double, double, double")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double distance = GroundRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = GroundRangeTo(WsfGeoPoint aPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, GroundRangeTo_4, 1, "double", "WsfGeoPoint")
{
   double       simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = GroundRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = DownRangeTo(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, DownRangeTo_1, 1, "double", "WsfTrack")
{
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = DownRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = DownRangeTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, DownRangeTo_2, 1, "double", "WsfPlatform")
{
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = DownRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = DownRangeTo(double aLat, double aLon);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, DownRangeTo_3, 3, "double", "double, double, double")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double distance = DownRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = DownRangeTo(WsfGeoPoint aPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, DownRangeTo_4, 1, "double", "WsfGeoPoint")
{
   double       simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = DownRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = CrossRangeTo(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, CrossRangeTo_1, 1, "double", "WsfTrack")
{
   double    simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = CrossRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = CrossRangeTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, CrossRangeTo_2, 1, "double", "WsfPlatform")
{
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = CrossRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = CrossRangeTo(double aLat, double aLon);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, CrossRangeTo_3, 3, "double", "double, double, double")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double distance = CrossRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = CrossRangeTo(WsfGeoPoint aPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, CrossRangeTo_4, 1, "double", "WsfGeoPoint")
{
   double       simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = CrossRange(simTime, aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double azimuthLOSR = AzimuthLOSR_From(WsfPlatform aObservingPlatform)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, AzimuthLOSR_From, 1, "double", "WsfPlatform")
{
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* observerPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   double       temp;
   double       azLOSR;
   bool         ok = aObjectPtr->LineOfSightRatesFrom(simTime, observerPtr, azLOSR, temp);
   if (ok)
   {
      azLOSR *= UtMath::cDEG_PER_RAD;
   }
   else
   {
      azLOSR = 0.0;
   }
   aReturnVal.SetDouble(azLOSR);
}

//! double = ElevationLOSR_From(WsfPlatform aObserver)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, ElevationLOSR_From, 1, "double", "WsfPlatform")
{
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* observerPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   double       temp;
   double       elLOSR;
   bool         ok = aObjectPtr->LineOfSightRatesFrom(simTime, observerPtr, temp, elLOSR);
   if (ok)
   {
      elLOSR *= UtMath::cDEG_PER_RAD;
   }
   else
   {
      elLOSR = 0.0;
   }
   aReturnVal.SetDouble(elLOSR);
}

//! double = ElevationLOSR_From(WsfPlatform aObserver)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, TotalAngularRate, 1, "double", "WsfPlatform")
{
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   WsfPlatform* observerPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   double       totalLOSR   = 0;
   double       azLOSR;
   double       elLOSR;
   bool         ok = aObjectPtr->LineOfSightRatesFrom(simTime, observerPtr, azLOSR, elLOSR);
   if (ok)
   {
      totalLOSR = sqrt(elLOSR * elLOSR + azLOSR * azLOSR);
      totalLOSR *= UtMath::cDEG_PER_RAD;
   }
   aReturnVal.SetDouble(totalLOSR);
}

// ----------------------------------------------------------------------------
//                         Zone containment methods
// ----------------------------------------------------------------------------

//! Is the track within the specified zone of a specified platform
// bool inside = WithinZoneOf(WsfPlatform aPlatform, string aZoneName)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, WithinZoneOf_1, 2, "bool", "WsfPlatform, string")
{
   bool         withinZone  = false;
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfZone*     zonePtr     = platformPtr->GetComponent<WsfZone>(aVarArgs[1].GetString());
   if (zonePtr != nullptr)
   {
      double simTime = WsfScriptContext::GetTIME_NOW(aContext);

      // Get the target location.
      double targetLocWCS[3];
      aObjectPtr->GetExtrapolatedLocationWCS(simTime, targetLocWCS);

      // Get the location and heading of this platform that contains the zone
      double refLocWCS[3];
      double refHeading;
      double refPitch;
      double refRoll;
      platformPtr->Update(simTime); // Ensure the reference platform position is current
      platformPtr->GetLocationWCS(refLocWCS);
      platformPtr->GetOrientationNED(refHeading, refPitch, refRoll);
      withinZone = zonePtr->PointIsInside(SIMULATION, targetLocWCS, refLocWCS, refHeading);
   }
   aReturnVal.SetBool(withinZone);
}

//! Is the track within the specified zone of a specified platform using the sensor orientation
// bool inside = WithinZoneOf(WsfPlatform aPlatform, string aZoneName, string aSensorName)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, WithinZoneOf_2, 3, "bool", "WsfPlatform, string, string")
{
   bool         withinZone  = false;
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfStringId  zoneName(aVarArgs[1].GetString());
   WsfZone*     zonePtr = platformPtr->GetComponent<WsfZone>(aVarArgs[1].GetString());
   if (zonePtr != nullptr)
   {
      WsfStringId sensorName(aVarArgs[2].GetString());
      WsfSensor*  sensorPtr = platformPtr->GetComponent<WsfSensor>(aVarArgs[2].GetString());
      if (sensorPtr != nullptr)
      {
         double simTime = WsfScriptContext::GetTIME_NOW(aContext);

         // Get the target location.
         double targetLocWCS[3];
         aObjectPtr->GetExtrapolatedLocationWCS(simTime, targetLocWCS);

         // Get the location and heading of this platform that contains the zone
         double refLocWCS[3];
         double refHeading;
         double refPitch;
         double refRoll;
         platformPtr->Update(simTime); // Ensure the reference platform position is current
         platformPtr->GetLocationWCS(refLocWCS);
         platformPtr->GetOrientationNED(refHeading, refPitch, refRoll);

         // Get the orientation of the sensor
         double sensorYaw(0.0);
         double sensorPitch(0.0);
         double sensorRoll(0.0);
         sensorPtr->GetOrientation(sensorYaw, sensorPitch, sensorRoll);

         // Calculate the new heading
         refHeading += sensorYaw;
         refHeading = UtMath::NormalizeAngle0_TwoPi(refHeading);

         withinZone = zonePtr->PointIsInside(SIMULATION, targetLocWCS, refLocWCS, refHeading);
      }
   }
   aReturnVal.SetBool(withinZone);
}

//!
// double duration = BallisticPropagate(WsfGeoPoint aPropagatedPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, BallisticPropagate, 1, "double", "WsfGeoPoint") // NO_DOC | PENDING
{
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   UtVec3d      pos, vel;
   aObjectPtr->GetExtrapolatedLocationWCS(simTime, pos.GetData());
   aObjectPtr->GetVelocityWCS(vel.GetData());

   UtVec3d finalPos, finalVel;
   double  deltaTime = WsfUtil::BallisticPropagate(pos, vel, 0.0, finalPos, finalVel);
   pointPtr->SetLocationWCS(finalPos.GetData());
   aReturnVal.SetDouble(deltaTime);
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(WsfScriptTrackClass, WsfTrack)

// ***************************************************************************
//! \relates WsfLocalTrack::CreateScriptClass
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfLocalTrack::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptLocalTrackClass>(aClassName, aScriptTypesPtr);
}

WsfScriptLocalTrackClass::WsfScriptLocalTrackClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptTrackClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfLocalTrack");

   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<RawTrackCount>());
   AddMethod(ut::make_unique<RawTrackCount>("NumRawTrackIds")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<RawTrackId>());
   AddMethod(ut::make_unique<RawTrack>());

   AddMethod(ut::make_unique<SetAuxData_1>("SetAuxData")); // SetAuxData(string, Object, bool)

   AddMethod(ut::make_unique<ContributorOf_1>("ContributorOf"));
   AddMethod(ut::make_unique<ContributorOf_2>("ContributorOf"));
   AddMethod(ut::make_unique<RawTracksFrom>());
   AddMethod(ut::make_unique<RawTrackFromSensorName>());
   AddMethod(ut::make_unique<RawTrackFromSensorType>());
   AddMethod(ut::make_unique<SensorTypeContributor>("SensorTypeContributor"));
   AddMethod(ut::make_unique<NumContributors>());
}

// virtual
void* WsfScriptLocalTrackClass::Create(const UtScriptContext& /*aContext*/)
{
   return nullptr; // Not default constructible
}

// virtual
void* WsfScriptLocalTrackClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfLocalTrack*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptLocalTrackClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfLocalTrack*>(aObjectPtr);
}
UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, RawTrackCount, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetRawTrackIds().GetCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, RawTrackId, 1, "WsfTrackId", "int")
{
   WsfTrackId*                          trackIdPtr  = nullptr;
   const WsfLocalTrack::RawTrackIdList& trackIdList = aObjectPtr->GetRawTrackIds();
   unsigned int                         trackIndex  = static_cast<int>(aVarArgs[0].GetInt());
   if (trackIndex < trackIdList.GetCount())
   {
      trackIdPtr = new WsfTrackId(*(trackIdList.GetEntry(trackIndex)));
   }
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, RawTrack, 1, "WsfTrack", "int")
{
   WsfTrack*        trackPtr        = nullptr;
   WsfTrackManager* trackManagerPtr = (WsfTrackManager*)aObjectPtr->GetTrackManager();
   if (trackManagerPtr != nullptr)
   {
      const WsfLocalTrack::RawTrackIdList& trackIdList = aObjectPtr->GetRawTrackIds();
      unsigned int                         trackIndex  = static_cast<unsigned int>(aVarArgs[0].GetInt());
      if (trackIndex < trackIdList.GetCount())
      {
         trackPtr = trackManagerPtr->FindRawTrack(*(trackIdList.GetEntry(trackIndex)));
         if (trackPtr != nullptr)
         {
            trackPtr = new WsfTrack(*trackPtr);
         }
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(trackPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! \<x\>.SetAuxData(string aAttributeName, Object aAttributeValue, bool aPropagateToRawTracks);
UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, SetAuxData_1, 3, "void", "string, Object, bool")
{
   WsfScriptAuxDataUtil::SetAuxData<WsfTrack>(aObjectPtr, aVarArgs, aReturnVal, aReturnClassPtr, aInterfaceMethodPtr);

   // Propagate the attribute to any correlated raw tracks
   if (aVarArgs[2].GetBool() && aObjectPtr->IsCorrelated() && (aObjectPtr->GetTrackManager() != nullptr))
   {
      const WsfLocalTrack::RawTrackIdList& idList = aObjectPtr->GetRawTrackIds();
      unsigned int                         trackListCount(idList.GetCount());
      for (unsigned int trackNum = 0; trackNum < trackListCount; ++trackNum)
      {
         const WsfTrackId* idPtr       = idList.GetEntry(trackNum);
         WsfTrack*         rawTrackPtr = aObjectPtr->GetTrackManager()->FindRawTrack(*idPtr);
         if (rawTrackPtr != nullptr)
         {
            WsfScriptAuxDataUtil::SetAuxData<WsfTrack>(rawTrackPtr,
                                                       aVarArgs,
                                                       aReturnVal,
                                                       aReturnClassPtr,
                                                       aInterfaceMethodPtr,
                                                       true);
         }
      }
   }
}

//! bool isContributorOf = TRACK.ContributorOf(WsfPlatform aPlatform);
//! Returns true if raw tracks from a sensor on the given platform is contributing to this local track.
UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, ContributorOf_1, 1, "bool", "WsfPlatform")
{
   bool bReturnValue(false);

   if (aObjectPtr->GetTrackManager() != nullptr)
   {
      // Argument 1: WsfPlatform
      WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();

      // Get the list of raw track that contributed in the forming of this local track
      const WsfLocalTrack::RawTrackIdList& idList = aObjectPtr->GetRawTrackIds();
      unsigned int                         trackListCount(idList.GetCount());
      for (unsigned int trackNum = 0; trackNum < trackListCount; ++trackNum)
      {
         const WsfTrackId* idPtr = idList.GetEntry(trackNum);
         // Find the raw track
         WsfTrack* rawTrackPtr = aObjectPtr->GetTrackManager()->FindRawTrack(*idPtr);
         if (rawTrackPtr != nullptr)
         {
            // Check the raw track platform ID against the given platform's ID
            if (rawTrackPtr->GetTrackId().GetOwningPlatformId() == platformPtr->GetNameId())
            {
               // Only need to find one raw track from the given platform
               bReturnValue = true;
               break;
            }
         }
      }
   }

   aReturnVal.SetBool(bReturnValue);
}

//! bool isContributorOf = TRACK.ContributorOf(WsfPlatform aPlatform, WsfSensor aSensor);
//! Returns true if raw tracks from a sensor on the given platform is contributing to this local track.
UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, ContributorOf_2, 2, "bool", "WsfPlatform, WsfSensor")
{
   bool bReturnValue(false);

   if (aObjectPtr->GetTrackManager() != nullptr)
   {
      // Argument 1: WsfPlatform
      WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();

      // Argument 2: WsfSensor
      WsfSensor* sensorPtr = (WsfSensor*)aVarArgs[1].GetPointer()->GetAppObject();

      // Get the list of raw track that contributed in the forming of this local track
      const WsfLocalTrack::RawTrackIdList& idList = aObjectPtr->GetRawTrackIds();
      unsigned int                         trackListCount(idList.GetCount());
      for (unsigned int trackNum = 0; trackNum < trackListCount; ++trackNum)
      {
         const WsfTrackId* idPtr = idList.GetEntry(trackNum);
         // Find the raw track
         WsfTrack* rawTrackPtr = aObjectPtr->GetTrackManager()->FindRawTrack(*idPtr);
         if (rawTrackPtr != nullptr)
         {
            // Check the raw track platform ID against the given platform's ID
            if ((rawTrackPtr->GetTrackId().GetOwningPlatformId() == platformPtr->GetNameId()) &&
                (rawTrackPtr->GetSensorNameId() == sensorPtr->GetNameId()))
            {
               // Only need to find one raw track from the given platform and sensor
               bReturnValue = true;
               break;
            }
         }
      }
   }

   aReturnVal.SetBool(bReturnValue);
}

//! "Array<WsfTrack> tracks = TRACK.RawTracksFrom(WsfPlatform aPlatform);
//! Returns a list of raw track owned by the specified platform.  If ContributorOf(WsfPlatform) is true, this should
//! return at least one track.
UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, RawTracksFrom, 1, "Array<WsfTrack>", "WsfPlatform")
{
   UtScriptDataList rVal;
   if (aObjectPtr->GetTrackManager() != nullptr)
   {
      // Argument 1: WsfPlatform
      WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();

      static UtScriptClass* trackClassPtr = aContext.GetTypes()->GetClass("WsfTrack");

      // Get the list of raw track that contributed in the forming of this local track
      const WsfLocalTrack::RawTrackIdList& idList = aObjectPtr->GetRawTrackIds();
      unsigned int                         trackListCount(idList.GetCount());
      for (unsigned int trackNum = 0; trackNum < trackListCount; ++trackNum)
      {
         const WsfTrackId* idPtr = idList.GetEntry(trackNum);
         // Find the raw track
         WsfTrack* rawTrackPtr = aObjectPtr->GetTrackManager()->FindRawTrack(*idPtr);

         if (rawTrackPtr != nullptr)
         {
            // Check the raw track platform ID against the given platform's ID
            if (rawTrackPtr->GetTrackId().GetOwningPlatformId() == platformPtr->GetNameId())
            {
               rVal.push_back(UtScriptData(new UtScriptRef(rawTrackPtr, trackClassPtr, UtScriptRef::cDONT_MANAGE)));
            }
         }
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(new UtScriptDataList(rVal), aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! "WsfTrack track = TRACK.RawTrackFromSensorName(WsfPlatform aPlatform, string aSensorName);
//! Returns a track contributing to this local track detected by aPlatform's sensor with the given name.
UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, RawTrackFromSensorName, 2, "WsfTrack", "WsfPlatform, string")
{
   WsfTrack* trackPtr = nullptr;
   if (aObjectPtr->GetTrackManager() != nullptr)
   {
      // Argument 1: WsfPlatform
      WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();

      WsfStringId sensorNameId = aVarArgs[1].GetString();

      // Get the list of raw track that contributed in the forming of this local track
      const WsfLocalTrack::RawTrackIdList& idList = aObjectPtr->GetRawTrackIds();
      unsigned int                         trackListCount(idList.GetCount());
      for (unsigned int trackNum = 0; trackNum < trackListCount; ++trackNum)
      {
         const WsfTrackId* idPtr = idList.GetEntry(trackNum);
         // Find the raw track
         WsfTrack* rawTrackPtr = aObjectPtr->GetTrackManager()->FindRawTrack(*idPtr);

         if (rawTrackPtr != nullptr)
         {
            // Check the raw track platform ID against the given platform's ID
            if (rawTrackPtr->GetTrackId().GetOwningPlatformId() == platformPtr->GetNameId())
            {
               if (rawTrackPtr->GetSensorNameId() == sensorNameId)
               {
                  trackPtr = rawTrackPtr;
                  // only a single track is needed.
                  break;
               }
            }
         }
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(trackPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

//! "WsfTrack track = TRACK.RawTrackFromSensorType(WsfPlatform aPlatform, string aSensorName);
//! Returns the most recent track contributing to this local track detected by a sensor of the specified type owned by aPlatform.
UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, RawTrackFromSensorType, 2, "WsfTrack", "WsfPlatform, string")
{
   WsfTrack* trackPtr = nullptr;

   if (aObjectPtr->GetTrackManager() != nullptr)
   {
      // Argument 1: WsfPlatform
      WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();

      WsfStringId sensorTypeId = WsfStringId(aVarArgs[1].GetString());

      // Get the list of raw track that contributed in the forming of this local track
      const WsfLocalTrack::RawTrackIdList& idList = aObjectPtr->GetRawTrackIds();
      unsigned int                         trackListCount(idList.GetCount());
      for (unsigned int trackNum = 0; trackNum < trackListCount; ++trackNum)
      {
         const WsfTrackId* idPtr = idList.GetEntry(trackNum);
         // Find the raw track
         WsfTrack* rawTrackPtr = aObjectPtr->GetTrackManager()->FindRawTrack(*idPtr);

         if (rawTrackPtr != nullptr)
         {
            // Check the raw track platform ID against the given platform's ID
            if (rawTrackPtr->GetTrackId().GetOwningPlatformId() == platformPtr->GetNameId())
            {
               if (rawTrackPtr->GetSensorTypeId() == sensorTypeId)
               {
                  // Keep the most recent raw track.
                  if (trackPtr == nullptr || trackPtr->GetUpdateTime() < rawTrackPtr->GetUpdateTime())
                  {
                     trackPtr = rawTrackPtr;
                  }
               }
            }
         }
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(trackPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

//! bool isContributorOf = TRACK.SensorTypeContributor(const string& aSensorType);
//! Returns true if raw tracks from a given sensor type is contributing to this local track.
UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, SensorTypeContributor, 1, "bool", "string")
{
   bool bReturnValue(false);
   if (aObjectPtr->GetTrackManager() != nullptr)
   {
      // Argument 1: aSensorType
      std::string aSensorType = aVarArgs[0].GetString();

      // Get the list of raw track that contributed in the forming of this local track
      const WsfLocalTrack::RawTrackIdList& idList = aObjectPtr->GetRawTrackIds();
      unsigned int                         trackListCount(idList.GetCount());
      for (unsigned int trackNum = 0; trackNum < trackListCount; ++trackNum)
      {
         const WsfTrackId* idPtr = idList.GetEntry(trackNum);

         // Find the raw track
         WsfTrack* rawTrackPtr = aObjectPtr->GetTrackManager()->FindRawTrack(*idPtr);
         // Check the raw track's sensor type id vs. the given sensor type.
         if ((rawTrackPtr != nullptr) && (rawTrackPtr->GetSensorTypeId() == aSensorType))
         {
            // Only need to find one raw track from the given sensor type
            bReturnValue = true;
            break;
         }
      }
   }
   aReturnVal.SetBool(bReturnValue);
}

//! int numContributors = TRACK.NumContributors();
//! Returns the number of platforms contributing to this local track.
UT_DEFINE_SCRIPT_METHOD(WsfScriptLocalTrackClass, WsfLocalTrack, NumContributors, 0, "int", "")
{
   // Container for the contributing platform name ids
   std::set<WsfStringId> contributorContainer;

   // Get the list of raw track that contributed in the forming of this local track
   const WsfLocalTrack::RawTrackIdList& idList = aObjectPtr->GetRawTrackIds();
   unsigned int                         trackListCount(idList.GetCount());
   for (unsigned int trackNum = 0; trackNum < trackListCount; ++trackNum)
   {
      const WsfTrackId* idPtr = idList.GetEntry(trackNum);
      // Find the raw track
      WsfTrack* rawTrackPtr = aObjectPtr->GetTrackManager()->FindRawTrack(*idPtr);
      if (rawTrackPtr != nullptr)
      {
         // Insert the raw track platform ID
         WsfStringId owningPlatformId((rawTrackPtr->GetTrackId()).GetOwningPlatformId());
         contributorContainer.insert(owningPlatformId);
      }
   }
   aReturnVal.SetInt(static_cast<int>(contributorContainer.size()));
}
