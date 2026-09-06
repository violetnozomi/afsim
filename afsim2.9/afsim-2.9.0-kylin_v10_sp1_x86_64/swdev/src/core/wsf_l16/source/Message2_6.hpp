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
#ifndef L16_MESSAGE2_6_HPP
#define L16_MESSAGE2_6_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J2_6
{
class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(2, 6, 0);
   typedef LatitudeField<23>  FieldType1;
   typedef LongitudeField<24> FieldType2;
   typedef SpareField<1>      FieldType3;
   typedef AngleField         FieldType4;
   typedef SpeedField         FieldType5;
   DEFINE_MEMBERS5(Latitude, Longitude, Spare, Course, Speed)
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 6, 1);
   typedef BooleanField     FieldType1;
   typedef SpareField<1>    FieldType2;
   typedef CallsignField<4> FieldType3;
   typedef IntegerField<6>  FieldType4;
   typedef IntegerField<7>  FieldType5;
   typedef IntegerField<13> FieldType6;
   typedef IntegerField<7>  FieldType7;
   typedef BooleanField     FieldType8;
   typedef BooleanField     FieldType9;
   typedef BooleanField     FieldType10;
   typedef SpareField<1>    FieldType11;

   DEFINE_MEMBERS11(InterpretVoiceCallsign,
                    Spare,
                    Callsign,
                    LandPlatform,
                    LandPlatformActivity,
                    VoiceFrequencyChannel,
                    ControlChannel,
                    VoiceActiveRelayIndicator,
                    ControlActiveRelayIndicator,
                    IsAlternateVoiceFrequen,
                    Spare2);
};

class Continuation3 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 6, 3);
   typedef SpareField<1>     FieldType1;
   typedef IntegerField<20>  FieldType2;
   typedef IntegerField<20>  FieldType3;
   typedef IntegerField<15>  FieldType4;
   typedef QualityField      FieldType5;
   typedef AngleQualityField FieldType6;

   DEFINE_MEMBERS6(Spare, UCoordinate, VCoordinate, BetaAngle, PositionQuality, AzimuthQuality)
};

class Continuation6 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 6, 6);
   typedef IntegerField<9>                       FieldType1;
   typedef TransverseMercatorGridCoordinateField FieldType2;
   typedef TransverseMercatorGridCoordinateField FieldType3;
   typedef TransverseMercatorGridCoordinateField FieldType4;
   typedef SpareField<1>                         FieldType5;
   typedef TransverseMercatorCoordinateField     FieldType6;
   typedef SpareField<2>                         FieldType7;
   typedef TransverseMercatorCoordinateField     FieldType8;
   typedef SpareField<2>                         FieldType9;
   typedef IntegerFieldNS<6, NoStatementZero>    FieldType10;

   DEFINE_MEMBERS10(TimeTag, SquareRow, SquareColumn, GridZoneRow, Spare, Northing, Spare2, Easting, Spare3, GridZoneColumn);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(2, 6);

   typedef BooleanField                       FieldType1;
   typedef SpareField<1>                      FieldType2;
   typedef BooleanField                       FieldType3;
   typedef BooleanField                       FieldType4;
   typedef BooleanField                       FieldType5;
   typedef BooleanField                       FieldType6;
   typedef SpareField<2>                      FieldType7;
   typedef BooleanField                       FieldType8;
   typedef InverseBooleanField                FieldType9;
   typedef NetworkParticipationStatusField    FieldType10;
   typedef TimeQualityField                   FieldType11;
   typedef QualityField                       FieldType12;
   typedef TrackStrengthField                 FieldType13;
   typedef SpareField<1>                      FieldType14;
   typedef AltitudeField<11>                  FieldType15;
   typedef SpareField<2>                      FieldType16;
   typedef IntegerFieldNS<8, NoStatementZero> FieldType17;
   typedef QualityField                       FieldType18;
   typedef SpareField<5>                      FieldType19;

   DEFINE_MEMBERS19(IsExerciseTrackUnit,
                    Spare,
                    IsForceTell,
                    IsEmergencyStatus,
                    IsC2Unit,
                    IsSimulated,
                    Spare1,
                    IsActiveRelay,
                    IsReplyStatusNetwork,
                    NetParticipation,
                    TimeQuality,
                    PositionQualityFt,
                    TrackStrength,
                    Spare2,
                    Altitude,
                    Spare3,
                    MissionCorrelator,
                    AltitudeQualityFt,
                    Spare4);

   DEFINE_CONTINUATIONS3(1, 3, 6);
   DEFINE_EXTENSIONS1();
};
} // namespace J2_6
} // namespace Messages
} // namespace WsfL16


#endif
