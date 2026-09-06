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
#ifndef L16_MESSAGE12_6_HPP
#define L16_MESSAGE12_6_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J12_6
{
enum EmitterStatusTypes
{
   cES_NO_STATEMENT,
   cES_INACTIVE,
   cES_TARGET_EMITTER_ACTIVE,
   cES_TARGET_EMITTER_IN_SEARCH,
   cES_TARGET_EMITTER_IN_TRACK,
   cES_MISSILE_LAUNCH
};
enum JammerTypes
{
   cJT_NO_STATEMENT,
   cJT_VELOCITY_DECEPTION,
   cJT_RANGE_DECEPTION,
   cJT_NOISE_JAMMING,
   cJT_ANGLE_DECEPTION
};
enum SensorTypes
{
   cST_NO_STATEMENT,
   cST_TELEVISION,
   cST_INFRARED,
   cST_ELECTRONIC_WARFARE_SUPPORT,
   cST_JAM_STROBE,
   cST_VISUAL,
   cST_RADAR,
   cST_ANTI_RADIATION_MISSILE,
   cST_LASER_DETECTOR_TRACKER,
   cST_RADAR_WARNING_RECEIVER,
   cST_SENSOR_FUSION_DATA,
   cST_PASSIVE_RADAR
};

class WSF_L16_EXPORT StatusField : public NormalField<4, NoStatementZero>
{
public:
   enum StatusTypes
   {
      cNO_STATEMENT,
      cENGAGING,
      cINVESTIGATING,
      cWEAPON_RELEASE,
      cNEW_SENSOR_TARGET_REPORT,
      cCANCEL_SENSOR_TARGET,
      cTRACK_TARGET_DESTROYED,
      cDISENGAGING,
      cHEADS_UP = 9,
      cLOCK_ON,
      cINVESTIGATE = 13,
      cENGAGE,
      cDISENGAGE
   };
   DEFINE_ACCESSORS(StatusField, StatusTypes);
   DEFINE_ENUM_MAP()
   // Some of these values are commands, others are status information.
   // If it is a command, the source track number should be set in the C1 word
   bool IsCommand() { return mValue == cINVESTIGATE || mValue == cENGAGE || mValue == cDISENGAGE; }
};

//! 12.6 E0   Target sorting extension word
class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(12, 6, 0)

   typedef TrackNumberField                    FieldType1;
   typedef BooleanField                        FieldType2;
   typedef AngleField                          FieldType3;
   typedef SpeedField                          FieldType4;
   typedef AltitudeField<11>                   FieldType5;
   typedef IdentityField                       FieldType6;
   typedef BooleanField                        FieldType7;
   typedef IntegerFieldNS<12, NoStatementZero> FieldType8;
   typedef BooleanField                        FieldType9;

   DEFINE_MEMBERS9(TrackNumberObjective,
                   CorrelationIndicator,
                   Course,
                   Speed,
                   Altitude,
                   Identity,
                   ExerciseIndicator,
                   SpecificType,
                   SpecialInterest);
};

//! 12.6 C1   Engagement status continuation word
class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 6, 1)

   typedef SpareField<6>               FieldType1;
   typedef TrackNumberAddresseeField   FieldType2;
   typedef BooleanField                FieldType3;
   typedef WeaponSystemField           FieldType4;
   typedef WeaponEngagementStatusField FieldType5;
   typedef TrackNumberField            FieldType6;
   typedef Mode3InterrogationTypeField FieldType7;
   typedef Mode4IndicatorField         FieldType8;
   typedef TrackStrengthField          FieldType9;
   typedef SpareField<5>               FieldType10;

   DEFINE_MEMBERS10(Spare,
                    TrackNumber,
                    Pointer,
                    WeaponSystem,
                    WeaponEngagementStatus,
                    TrackNumberReference,
                    Mode3_IFF,
                    Mode4_Indicator,
                    Strength,
                    Spare2);
};

//! 12.6 C2   Fusion type continuation word
class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 6, 2)

   typedef BooleanField                       FieldType1;
   typedef TimeIndicatorField                 FieldType2;
   typedef MinuteField                        FieldType3;
   typedef SecondField                        FieldType4;
   typedef IntegerFieldNS<7, NoStatementMax>  FieldType5;
   typedef PRF_TypeField                      FieldType6;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType7;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType8;
   typedef SpareField<5>                      FieldType9;
   typedef IntegerField<15>                   FieldType10;
   typedef IntegerField<6>                    FieldType11;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType12;
   typedef BooleanField                       FieldType13;
   DEFINE_MEMBERS13(IsEndOfTargetReporting,
                    TimeIndicator,
                    Minutes,
                    Seconds,
                    Hundredths,
                    PrfType,
                    EmitterStatus,
                    JammerType,
                    Spare,
                    TrackNumber,
                    TrackIndex,
                    SensorType,
                    IsDataFusion);
};

class Continuation3 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 6, 3)

   typedef IntegerFieldNS<4, NoStatementZero>         FieldType1;
   typedef IntegerFieldNS<4, NoStatementZero>         FieldType2;
   typedef IntegerFieldNS<4, NoStatementZero>         FieldType3;
   typedef EllipseOrientationOrTiltField              FieldType4;
   typedef EllipseOrientationOrTiltField              FieldType5;
   typedef IntegerFieldNS<13, NoStatementValue<2048>> FieldType6;
   typedef IntegerFieldNS<4, NoStatementZero>         FieldType7;
   typedef IntegerFieldNS<4, NoStatementZero>         FieldType8;
   typedef IntegerFieldNS<4, NoStatementZero>         FieldType9;
   typedef SpareField<4>                              FieldType10;
   DEFINE_MEMBERS10(SlantRangeUncertainty,
                    CrossRangeUncertainty,
                    VerticalUncertainty,
                    OrientationUncertainty,
                    TiltUncertainty,
                    VelocityDown,
                    VelocityNUncertainty,
                    VelocityEUncertainty,
                    VelocityDUncertainty,
                    Spare);
};

class Continuation6 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 6, 6)

   typedef IntegerFieldNS<3, NoStatementZero>  FieldType1;
   typedef BooleanField                        FieldType2;
   typedef IntegerFieldNS<12, NoStatementZero> FieldType3;
   typedef IntegerFieldNS<5, NoStatementZero>  FieldType4;
   typedef IntegerFieldNS<2, NoStatementZero>  FieldType5;
   typedef BooleanField                        FieldType6;
   typedef IntegerFieldNS<9, NoStatementZero>  FieldType7;
   typedef IntegerFieldNS<5, NoStatementZero>  FieldType8;
   typedef IntegerFieldNS<5, NoStatementZero>  FieldType9;
   typedef IntegerFieldNS<3, NoStatementZero>  FieldType10;
   typedef IntegerFieldNS<8, NoStatementZero>  FieldType11;
   typedef IntegerFieldNS<4, NoStatementZero>  FieldType12;
   typedef IntegerFieldNS<4, NoStatementZero>  FieldType13;
   typedef SpareField<1>                       FieldType14;
   DEFINE_MEMBERS14(SensorTypeIndicator,
                    SensorStatusCommandIndicator,
                    SensorChannelCode,
                    SensorChannelSet,
                    FreqAgilityIndicator,
                    PDIIndicator,
                    PDIMissleDataLinkChannel,
                    AirGroundWeaponDataLinkChannel,
                    WeaponID,
                    RadarPRF,
                    ArmThreatNumber,
                    ArmType,
                    ArmThreatNumberMSBS,
                    Spare);
};

class Continuation8 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 6, 8)

   typedef LatitudeField<5>  FieldType1;
   typedef LongitudeField<5> FieldType2;
   // typedef AltitudeField<6>                      FieldType3;
   typedef IntegerFieldNS<6, NoStatementValue<131008>> FieldType3; // ERIN; we will have to figure out how to scale this
                                                                   // altitude precision

   typedef IntegerFieldNS<5, NoStatementZero> FieldType4;
   typedef IntegerFieldNS<5, NoStatementZero> FieldType5;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType6;

   typedef IntegerFieldNS<10, NoStatementZero> FieldType7;
   typedef IntegerFieldNS<5, NoStatementZero>  FieldType8;

   typedef IntegerFieldNS<2, NoStatementZero> FieldType9;
   typedef IntegerFieldNS<2, NoStatementZero> FieldType10;

   typedef IntegerFieldNS<3, NoStatementZero>  FieldType11;
   typedef BooleanField                        FieldType12;
   typedef IntegerFieldNS<11, NoStatementZero> FieldType13;
   DEFINE_MEMBERS13(Latitude,
                    Longitude,
                    Altitude,
                    XPositionUncertainty,
                    YPositionUncertainty,
                    ZPositionUncertainty,
                    PositionUncertaintyOrientation,
                    PositionUncertaintyTilt,
                    XVelocityUncertainty,
                    YVelocityUncertainty,
                    VelocityUncertaintyOrient,
                    VelocityUncertaintyIndicator,
                    HAEAdjustment);
};

//! 12.6 I   Target sorting Initial word
class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(12, 6);

   typedef StatusField        FieldType1;
   typedef IntegerField<2>    FieldType2;
   typedef EnvironmentField   FieldType3;
   typedef IntegerField<1>    FieldType4;
   typedef IntegerField<6>    FieldType5;
   typedef LatitudeField<20>  FieldType6;
   typedef LongitudeField<21> FieldType7;

   DEFINE_MEMBERS7(StatusInformation, TargetPositionQuality, Environment, MsgOriginator, IndexNumber, Latitude, Longitude);

   DEFINE_CONTINUATIONS5(1, 2, 3, 6, 8)
   DEFINE_EXTENSIONS1()

protected:
};


} // namespace J12_6
} // namespace Messages
} // namespace WsfL16

#endif
