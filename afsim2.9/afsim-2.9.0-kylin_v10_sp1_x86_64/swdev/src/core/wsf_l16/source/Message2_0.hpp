// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef L16_MESSAGE2_0_HPP
#define L16_MESSAGE2_0_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J2_0
{
typedef NormalField<4, NoStatementZero> GenericUnitTypeField;
typedef NormalField<3, NoStatementZero> SiteField;
typedef NormalField<4, NoStatementZero> UnitTypeField;
typedef NormalField<2, NoStatementZero> OriginatorEnvironmentField;

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(2, 0, 0);
   typedef LatitudeField<23>  FieldType1;
   typedef LongitudeField<24> FieldType2;
   typedef SpareField<1>      FieldType3;
   typedef AngleField         FieldType4;
   typedef SpeedField         FieldType5;
   DEFINE_MEMBERS5(Latitude, Longitude, IsAirborne, Course, Speed);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 0, 1);
   typedef SpareField<1>                   FieldType1;
   typedef Mode1CodeField                  FieldType2;
   typedef Mode2CodeField                  FieldType3;
   typedef Mode3CodeField                  FieldType4;
   typedef ElevationField<11>              FieldType5;
   typedef NormalField<6, NoStatementZero> FieldType6;
   typedef NormalField<7, NoStatementZero> FieldType7;
   typedef NormalField<8, NoStatementZero> FieldType8;
   typedef SpareField<1>                   FieldType9;

   DEFINE_MEMBERS9(Spare, Mode1Code, Mode2Code, Mode3Code, Elevation, AirPlatform, AirActivity, MissionCorrelator, Spare2);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(2, 0);

   typedef BooleanField               FieldType1;
   typedef BooleanField               FieldType2;
   typedef BooleanField               FieldType3;
   typedef BooleanField               FieldType4;
   typedef BooleanField               FieldType5;
   typedef BooleanField               FieldType6;
   typedef TrackNumberAddresseeField  FieldType7;
   typedef BooleanField               FieldType8;
   typedef BooleanField               FieldType9;
   typedef GenericUnitTypeField       FieldType10;
   typedef AltitudeField<13>          FieldType11;
   typedef QualityField               FieldType12;
   typedef QualityField               FieldType13;
   typedef SiteField                  FieldType14;
   typedef UnitTypeField              FieldType15;
   typedef OriginatorEnvironmentField FieldType16;

   DEFINE_MEMBERS16(IsExerciseTrackUnit,
                    IsBailOut,
                    IsForceTell,
                    IsEmergencyStatus,
                    IsC2Unit,
                    IsSimulated,
                    SourceTrackNumber,
                    IsFlightLeader,
                    IsMissionCommander,
                    GenericUnitType,
                    Altitude,
                    AltitudeQuality,
                    PositionQuality,
                    Site,
                    UnitType,
                    OriginatorEnvironment);

   DEFINE_CONTINUATIONS1(1)
   DEFINE_EXTENSIONS1()
};

} // namespace J2_0
} // namespace Messages
} // namespace WsfL16

#endif
