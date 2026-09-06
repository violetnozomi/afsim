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
#ifndef L16_MESSAGE2_2_HPP
#define L16_MESSAGE2_2_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J2_2
{

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(2, 2, 0);
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
   DEFINE_CONTINUATION(2, 2, 1);
   typedef SpareField<1>            FieldType1;
   typedef Mode1CodeField           FieldType2;
   typedef Mode2CodeField           FieldType3;
   typedef Mode3CodeField           FieldType4;
   typedef AirPlatformField         FieldType5;
   typedef AirPlatformActivityField FieldType6;
   typedef SpareField<20>           FieldType7;

   DEFINE_MEMBERS7(Spare, Mode1Code, Mode2Code, Mode3Code, AirPlatform, Activity, Spare2);
};
class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 2, 2);
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

class Continuation5 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 2, 5);
   typedef NetNumberField    FieldType1;
   typedef BooleanField      FieldType2;
   typedef IntegerField<8>   FieldType3;
   typedef LetterNumberField FieldType4;
   typedef SpareField<2>     FieldType5;
   typedef IntegerField<15>  FieldType6;
   typedef SpareField<1>     FieldType7;
   typedef IntegerField<15>  FieldType8;
   typedef SpareField<8>     FieldType9;

   DEFINE_MEMBERS9(NetNumber_NonC2_to_NonC2,
                   NonC2_to_NonC2_NPG_status,
                   MissionCorrelator,
                   WingmanIDLetter,
                   Spare,
                   TrackNumberCommander,
                   Spare2,
                   TrackNumberLeader,
                   Spare3)
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(2, 2);

   typedef BooleanField                    FieldType1;
   typedef BooleanField                    FieldType2;
   typedef BooleanField                    FieldType3;
   typedef BooleanField                    FieldType4;
   typedef BooleanField                    FieldType5;
   typedef BooleanField                    FieldType6;
   typedef BooleanField                    FieldType7;
   typedef BooleanField                    FieldType8;
   typedef BooleanField                    FieldType9;
   typedef InverseBooleanField             FieldType10;
   typedef NetworkParticipationStatusField FieldType11;
   typedef TimeQualityField                FieldType12;
   typedef QualityField                    FieldType13;
   typedef TrackStrengthField              FieldType14;
   typedef BooleanField                    FieldType15;
   typedef AltitudeField<13>               FieldType16;
   typedef NetNumberField                  FieldType17;
   typedef InverseBooleanField             FieldType18;
   typedef QualityField                    FieldType19;
   typedef SpareField<5>                   FieldType20;

   DEFINE_MEMBERS20(IsExerciseTrackUnit,
                    IsMissionCommander,
                    IsForceTell,
                    IsEmergencyStatus,
                    IsC2Unit,
                    IsSimulated,
                    IsAirborne,
                    IsFlightLeader,
                    IsActiveRelay,
                    IsRTTRelayOperational,
                    NetParticipation,
                    TimeQuality,
                    PositionQualityFt,
                    TrackStrength,
                    IsBailOut,
                    Altitude,
                    NetNumber,
                    IsNPGActive,
                    AltitudeQualityFt,
                    Spare);

   DEFINE_CONTINUATIONS3(1, 2, 5)
   DEFINE_EXTENSIONS1()
};

} // namespace J2_2
} // namespace Messages
} // namespace WsfL16

#endif
