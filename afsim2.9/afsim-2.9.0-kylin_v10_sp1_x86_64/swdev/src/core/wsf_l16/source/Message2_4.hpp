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
#ifndef L16_MESSAGE2_4_HPP
#define L16_MESSAGE2_4_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J2_4
{
class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(2, 4, 0);
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
   DEFINE_CONTINUATION(2, 4, 1);
   typedef SpareField<1>    FieldType1;
   typedef Mode1CodeField   FieldType2;
   typedef IntegerField<12> FieldType3;
   typedef IntegerField<12> FieldType4;
   typedef IntegerField<6>  FieldType5;
   typedef IntegerField<7>  FieldType6;
   typedef SpareField<20>   FieldType7;

   DEFINE_MEMBERS7(Spare, Mode1Code, Mode2Code, Mode3Code, SubSurfacePlatform, Activity, Spare2);
};
class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 4, 2);
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
   DEFINE_CONTINUATION(2, 4, 3);
   typedef SpareField<1>     FieldType1;
   typedef IntegerField<20>  FieldType2;
   typedef IntegerField<20>  FieldType3;
   typedef IntegerField<15>  FieldType4;
   typedef QualityField      FieldType5;
   typedef AngleQualityField FieldType6;

   DEFINE_MEMBERS6(Spare, UCoordinate, VCoordinate, BetaAngle, PositionQuality, AzimuthQuality)
};

class Continuation4 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(2, 4, 4);
   typedef SpareField<3>      FieldType1;
   typedef LatitudeField<20>  FieldType2;
   typedef SpareField<3>      FieldType3;
   typedef LongitudeField<21> FieldType4;
   typedef SpareField<16>     FieldType5;

   DEFINE_MEMBERS5(Spare, Latitude, Spare2, Longitude, Spare3);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(2, 4);

   typedef BooleanField                       FieldType1;
   typedef BooleanField                       FieldType2;
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
   typedef SpareField<7>                      FieldType13;
   typedef DepthField                         FieldType14;
   typedef DepthCategoryField                 FieldType15;
   typedef IntegerFieldNS<8, NoStatementZero> FieldType16;
   typedef IntegerField<4>                    FieldType17;
   typedef SpareField<5>                      FieldType18;

   DEFINE_MEMBERS18(IsExerciseTrackUnit,
                    IsPositionInContinuation,
                    IsForceTell,
                    IsEmergencyStatus,
                    IsC2Unit,
                    IsSimulated,
                    Spare,
                    IsActiveRelay,
                    IsReplyStatusNetwork,
                    NetParticipation,
                    TimeQuality,
                    PositionQualityFt,
                    Spare2,
                    Depth,
                    DepthCategory,
                    MissionCorrelator,
                    DepthQuality,
                    Spare3);

   DEFINE_CONTINUATIONS4(1, 2, 3, 4);
   DEFINE_EXTENSIONS1();
};
} // namespace J2_4
} // namespace Messages
} // namespace WsfL16


#endif
