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
#ifndef L16_MESSAGE2_3_HPP
#define L16_MESSAGE2_3_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J2_3
{
class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(2, 3, 0);
   typedef LatitudeField<23>  FieldType1;
   typedef LongitudeField<24> FieldType2;
   typedef SpareField<1>      FieldType3;
   typedef AngleField         FieldType4;
   typedef SpeedField         FieldType5;
   DEFINE_MEMBERS5(Latitude, Longitude, Spare, Course, Speed);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 3, 1);
   typedef SpareField<1>    FieldType1;
   typedef Mode1CodeField   FieldType2;
   typedef IntegerField<12> FieldType3;
   typedef IntegerField<12> FieldType4;
   typedef IntegerField<6>  FieldType5;
   typedef IntegerField<7>  FieldType6;
   typedef SpareField<20>   FieldType7;

   DEFINE_MEMBERS7(Spare, Mode1Code, Mode2Code, Mode3Code, SurfacePlatform, Activity, Spare2);
};
class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 3, 2);
   typedef BooleanField     FieldType1;
   typedef BooleanField     FieldType2;
   typedef CallsignField<4> FieldType3;
   typedef IntegerField<13> FieldType4;
   typedef IntegerField<13> FieldType5;
   typedef NetNumberField   FieldType6;
   typedef BooleanField     FieldType7;
   typedef BooleanField     FieldType8;
   typedef BooleanField     FieldType9;
   typedef SpareField<1>    FieldType10;
   DEFINE_MEMBERS10(InterpretCallsign,
                    InterpretLink4Addr,
                    Callsign,
                    Link4Addr,
                    VoiceFreqChannel,
                    ControlChannel,
                    IsRelayActiveVoice,
                    IsRelayActiveControl,
                    IsAlternateVoiceChannel,
                    Spare);
};

class Continuation3 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 3, 3);
   typedef SpareField<1>     FieldType1;
   typedef IntegerField<20>  FieldType2;
   typedef IntegerField<20>  FieldType3;
   typedef IntegerField<15>  FieldType4;
   typedef QualityField      FieldType5;
   typedef AngleQualityField FieldType6;

   DEFINE_MEMBERS6(Spare, UCoordinate, VCoordinate, BetaAngle, PositionQuality, AzimuthQuality)
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(2, 3);

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
   typedef BooleanField                       FieldType14;
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
                    Spare2,
                    IsActiveRelay,
                    IsReplyStatusNetwork,
                    NetParticipation,
                    TimeQuality,
                    PositionQualityFt,
                    TrackStrength,
                    Spare3,
                    Altitude,
                    Spare4,
                    MissionCorrelator,
                    AltitudeQualityFt,
                    Spare5);

   DEFINE_CONTINUATIONS3(1, 2, 3);
   DEFINE_EXTENSIONS1();
};

} // namespace J2_3
} // namespace Messages
} // namespace WsfL16


#endif
