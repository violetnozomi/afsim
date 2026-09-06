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

#ifndef WSFSCRIPTTRACKCLASS_HPP
#define WSFSCRIPTTRACKCLASS_HPP

#include "wsf_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfScriptAuxDataUtil.hpp"

//! WsfScriptTrackClass is a UtScriptClass that defines the methods in
//! WsfTrack.  This makes the methods available for use in script.

class WSF_EXPORT WsfScriptTrackClass : public UtScriptClass
{
public:
   WsfScriptTrackClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~WsfScriptTrackClass() override = default;

   // See the base class for a description of Create, Clone, and Destroy.

   void*       Create(const UtScriptContext& aInstance) override;
   void*       Clone(void* aObjectPtr) override;
   void        Destroy(void* aObjectPtr) override;
   std::string ToString(void* aObjectPtr) const override;
   void        OnNewScriptRef(UtScriptRef& aReference) override;
   // Each of the exported methods are defined as function objects.

   UT_DECLARE_SCRIPT_METHOD(Initialize);
   UT_DECLARE_SCRIPT_METHOD(Update);          // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(KinematicUpdate); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(TrackId);
   UT_DECLARE_SCRIPT_METHOD(StartTime);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceStarted);
   UT_DECLARE_SCRIPT_METHOD(UpdateTime);
   UT_DECLARE_SCRIPT_METHOD(SetUpdateTime);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceUpdated);
   UT_DECLARE_SCRIPT_METHOD(UpdateCount);
   UT_DECLARE_SCRIPT_METHOD(IsStale);
   UT_DECLARE_SCRIPT_METHOD(IsPredefined);
   UT_DECLARE_SCRIPT_METHOD(IsFalseTarget);
   UT_DECLARE_SCRIPT_METHOD(SetStale);
   UT_DECLARE_SCRIPT_METHOD(OriginatorName);
   UT_DECLARE_SCRIPT_METHOD(OriginatorType);
   UT_DECLARE_SCRIPT_METHOD(ProcessorName);
   UT_DECLARE_SCRIPT_METHOD(ProcessorType);
   UT_DECLARE_SCRIPT_METHOD(SensorName);
   UT_DECLARE_SCRIPT_METHOD(SensorType);
   UT_DECLARE_SCRIPT_METHOD(SensorMode);
   UT_DECLARE_SCRIPT_METHOD(RangeValid);
   UT_DECLARE_SCRIPT_METHOD(Range);
   UT_DECLARE_SCRIPT_METHOD(RangeRate);
   UT_DECLARE_SCRIPT_METHOD(SetRange);
   UT_DECLARE_SCRIPT_METHOD(RangeErrorSigma);
   UT_DECLARE_SCRIPT_METHOD(RangeRateErrorSigma);
   UT_DECLARE_SCRIPT_METHOD(BearingValid);
   UT_DECLARE_SCRIPT_METHOD(Bearing);
   UT_DECLARE_SCRIPT_METHOD(SetBearing);
   UT_DECLARE_SCRIPT_METHOD(BearingErrorSigma);
   UT_DECLARE_SCRIPT_METHOD(ElevationValid);
   UT_DECLARE_SCRIPT_METHOD(Elevation);
   UT_DECLARE_SCRIPT_METHOD(SetElevation);
   UT_DECLARE_SCRIPT_METHOD(ElevationErrorSigma);
   UT_DECLARE_SCRIPT_METHOD(TypeValid);
   UT_DECLARE_SCRIPT_METHOD(TypesValid);
   UT_DECLARE_SCRIPT_METHOD(Type);
   UT_DECLARE_SCRIPT_METHOD(Types);
   UT_DECLARE_SCRIPT_METHOD(SetType);
   UT_DECLARE_SCRIPT_METHOD(SetTypes);
   UT_DECLARE_SCRIPT_METHOD(SideValid);
   UT_DECLARE_SCRIPT_METHOD(Side);
   UT_DECLARE_SCRIPT_METHOD(SetSide);
   UT_DECLARE_SCRIPT_METHOD(IFF_Unknown);
   UT_DECLARE_SCRIPT_METHOD(IFF_Ambiguous);
   UT_DECLARE_SCRIPT_METHOD(IFF_Foe);
   UT_DECLARE_SCRIPT_METHOD(IFF_Friend);
   UT_DECLARE_SCRIPT_METHOD(IFF_Neutral);
   UT_DECLARE_SCRIPT_METHOD(SetIFF_Unknown);
   UT_DECLARE_SCRIPT_METHOD(SetIFF_Ambiguous);
   UT_DECLARE_SCRIPT_METHOD(SetIFF_Foe);
   UT_DECLARE_SCRIPT_METHOD(SetIFF_Friend);
   UT_DECLARE_SCRIPT_METHOD(SetIFF_Neutral);
   UT_DECLARE_SCRIPT_METHOD(LandDomain);
   UT_DECLARE_SCRIPT_METHOD(AirDomain);
   UT_DECLARE_SCRIPT_METHOD(SurfaceDomain);
   UT_DECLARE_SCRIPT_METHOD(SubsurfaceDomain);
   UT_DECLARE_SCRIPT_METHOD(SpaceDomain);
   UT_DECLARE_SCRIPT_METHOD(SetLandDomain);
   UT_DECLARE_SCRIPT_METHOD(SetAirDomain);
   UT_DECLARE_SCRIPT_METHOD(SetSurfaceDomain);
   UT_DECLARE_SCRIPT_METHOD(SetSubsurfaceDomain);
   UT_DECLARE_SCRIPT_METHOD(SetSpaceDomain);
   UT_DECLARE_SCRIPT_METHOD(IsA_Candidate); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(SignalToNoiseValid);
   UT_DECLARE_SCRIPT_METHOD(SignalToNoise);
   UT_DECLARE_SCRIPT_METHOD(SetSignalToNoise);
   UT_DECLARE_SCRIPT_METHOD(LocationValid);
   UT_DECLARE_SCRIPT_METHOD(LocationValid2D);
   UT_DECLARE_SCRIPT_METHOD(AltitudeKnown);
   UT_DECLARE_SCRIPT_METHOD(ReportedLocation);
   UT_DECLARE_SCRIPT_METHOD(CurrentLocation);
   UT_DECLARE_SCRIPT_METHOD(LocationAtTime);
   UT_DECLARE_SCRIPT_METHOD(Latitude);
   UT_DECLARE_SCRIPT_METHOD(Longitude);
   UT_DECLARE_SCRIPT_METHOD(Altitude);
   UT_DECLARE_SCRIPT_METHOD(Speed);
   UT_DECLARE_SCRIPT_METHOD(Heading);
   UT_DECLARE_SCRIPT_METHOD(HeadingValid);
   UT_DECLARE_SCRIPT_METHOD(X);
   UT_DECLARE_SCRIPT_METHOD(Y);
   UT_DECLARE_SCRIPT_METHOD(Z);
   UT_DECLARE_SCRIPT_METHOD(SetLocationLLA);
   UT_DECLARE_SCRIPT_METHOD(SetLocationWCS);
   UT_DECLARE_SCRIPT_METHOD(SetLocation);
   UT_DECLARE_SCRIPT_METHOD(VelocityValid);
   UT_DECLARE_SCRIPT_METHOD(Vx);
   UT_DECLARE_SCRIPT_METHOD(Vy);
   UT_DECLARE_SCRIPT_METHOD(Vz);
   UT_DECLARE_SCRIPT_METHOD(VelocityWCS);
   UT_DECLARE_SCRIPT_METHOD(VelocityNED);
   UT_DECLARE_SCRIPT_METHOD(SetVelocityWCS);
   UT_DECLARE_SCRIPT_METHOD(SetVelocityNED);
   UT_DECLARE_SCRIPT_METHOD(SetVelocity_1);
   UT_DECLARE_SCRIPT_METHOD(SetVelocity_2);
   UT_DECLARE_SCRIPT_METHOD(TrackQuality_1);
   UT_DECLARE_SCRIPT_METHOD(TrackQuality_2);
   UT_DECLARE_SCRIPT_METHOD(Quality_1);
   UT_DECLARE_SCRIPT_METHOD(Quality_2);
   UT_DECLARE_SCRIPT_METHOD(SetTrackQuality);
   UT_DECLARE_SCRIPT_METHOD(PixelCount);
   UT_DECLARE_SCRIPT_METHOD(SetPixelCount);
   UT_DECLARE_SCRIPT_METHOD(StateCovariance);
   UT_DECLARE_SCRIPT_METHOD(StateCovarianceValid);
   UT_DECLARE_SCRIPT_METHOD(StateCovarianceAtTime);
   UT_DECLARE_SCRIPT_METHOD(MeasurementCovariance);
   UT_DECLARE_SCRIPT_METHOD(MeasurementCovarianceValid);
   UT_DECLARE_SCRIPT_METHOD(Target);
   UT_DECLARE_SCRIPT_METHOD(TargetIndex);
   UT_DECLARE_SCRIPT_METHOD(TargetName);
   UT_DECLARE_SCRIPT_METHOD(TargetType);
   UT_DECLARE_SCRIPT_METHOD(ClearTarget);
   UT_DECLARE_SCRIPT_METHOD(TargetDamaged);
   UT_DECLARE_SCRIPT_METHOD(Originator);
   UT_DECLARE_SCRIPT_METHOD(SetTarget);

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()

   UT_DECLARE_SCRIPT_METHOD(SetGlobalTrackNumber);
   UT_DECLARE_SCRIPT_METHOD(GlobalTrackNumber);

   UT_DECLARE_SCRIPT_METHOD(HasHistory);
   UT_DECLARE_SCRIPT_METHOD(History);

   UT_DECLARE_SCRIPT_METHOD(OriginatorLocation);
   UT_DECLARE_SCRIPT_METHOD(SetOriginator);      // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(BallisticPropagate); // NO_DOC | PENDING

   // Signal methods

   UT_DECLARE_SCRIPT_METHOD(FrequencyValid);
   UT_DECLARE_SCRIPT_METHOD(Frequency);
   UT_DECLARE_SCRIPT_METHOD(SetFrequency);
   UT_DECLARE_SCRIPT_METHOD(SignalCount);
   UT_DECLARE_SCRIPT_METHOD(SignalFrequency);
   UT_DECLARE_SCRIPT_METHOD(SignalPRI);
   UT_DECLARE_SCRIPT_METHOD(SignalPW);
   UT_DECLARE_SCRIPT_METHOD(SignalEmitterType);

   UT_DECLARE_SCRIPT_METHOD(EmitterCount);
   UT_DECLARE_SCRIPT_METHOD(EmitterType);
   UT_DECLARE_SCRIPT_METHOD(EmitterSignalIndexCount);
   UT_DECLARE_SCRIPT_METHOD(EmitterSignalIndexEntry);

   // Relative geometry methods

   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo_1);     // TrueBearingTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo_2);     // TrueBearingTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo_3);     // TrueBearingTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo_4);     // TrueBearingTo(WsfGeoPoint)
   UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo_1); // RelativeBearingTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo_2); // RelativeBearingTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo_3); // RelativeBearingTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo_4); // RelativeBearingTo(GeoPoint)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_1);      // SlantRangeTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_1s);     // SlantRangeTo(WsfTrack, bool)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_2);      // SlantRangeTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_2s);     // SlantRangeTo(WsfPlatform, bool)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_3);      // SlantRangeTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_3s);     // SlantRangeTo(lat, lon, alt, bool)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_4);      // SlantRangeTo(WsfGeoPoint)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_4s);     // SlantRangeTo(WsfGeoPoint, bool)
   UT_DECLARE_SCRIPT_METHOD(GroundRangeTo_1);     // GroundRangeTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(GroundRangeTo_2);     // GroundRangeTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(GroundRangeTo_3);     // GroundRangeTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(GroundRangeTo_4);     // GroundRangeTo(WsfGeoPoint)
   UT_DECLARE_SCRIPT_METHOD(DownRangeTo_1);       // DownRangeTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(DownRangeTo_2);       // DownRangeTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(DownRangeTo_3);       // DownRangeTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(DownRangeTo_4);       // DownRangeTo(WsfGeoPoint)
   UT_DECLARE_SCRIPT_METHOD(CrossRangeTo_1);      // CrossRangeTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(CrossRangeTo_2);      // CrossRangeTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(CrossRangeTo_3);      // CrossRangeTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(CrossRangeTo_4);      // CrossRangeTo(WsfGeoPoint)

   UT_DECLARE_SCRIPT_METHOD(AzimuthLOSR_From);   // Az_LOSR_From(WsfPlatform aObserver)
   UT_DECLARE_SCRIPT_METHOD(ElevationLOSR_From); // El_LOSR_From(WsfPlatform aObserver)
   UT_DECLARE_SCRIPT_METHOD(TotalAngularRate);   // TotalAngularRate(WsfPlatform aObserver)

   // Zone containment methods

   UT_DECLARE_SCRIPT_METHOD(WithinZoneOf_1); // WithinZoneOf(WsfPlatform, zone name)
   UT_DECLARE_SCRIPT_METHOD(WithinZoneOf_2); // WithinZoneOf(WsfPlatform, zone name, sensor name)
};


//! The script interface 'class'
class WSF_EXPORT WsfScriptLocalTrackClass : public WsfScriptTrackClass
{
public:
   WsfScriptLocalTrackClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(NumRawTrackIds);
   UT_DECLARE_SCRIPT_METHOD(RawTrackCount);
   UT_DECLARE_SCRIPT_METHOD(RawTrackId);
   UT_DECLARE_SCRIPT_METHOD(RawTrack);

   UT_DECLARE_SCRIPT_METHOD(SetAuxData_1); // SetAuxData(string, Object, bool)

   UT_DECLARE_SCRIPT_METHOD(ContributorOf_1);
   UT_DECLARE_SCRIPT_METHOD(ContributorOf_2);
   UT_DECLARE_SCRIPT_METHOD(RawTracksFrom);
   UT_DECLARE_SCRIPT_METHOD(RawTrackFromSensorName);
   UT_DECLARE_SCRIPT_METHOD(RawTrackFromSensorType);
   UT_DECLARE_SCRIPT_METHOD(SensorTypeContributor);
   UT_DECLARE_SCRIPT_METHOD(NumContributors);
};

#endif
