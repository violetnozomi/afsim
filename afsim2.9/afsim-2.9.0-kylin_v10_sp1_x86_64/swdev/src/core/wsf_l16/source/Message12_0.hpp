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
#ifndef L16_MESSAGE12_0_HPP
#define L16_MESSAGE12_0_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J12_0
{

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 0, 1)

   typedef TrackStrengthField                 FieldType1;
   typedef DisusedField<8>                    FieldType2;
   typedef EnvironmentField                   FieldType3;
   typedef BooleanField                       FieldType4;
   typedef IntegerFieldNS<6, NoStatementZero> FieldType5;
   typedef LatitudeField<20>                  FieldType6;
   typedef LongitudeField<21>                 FieldType7;

   DEFINE_MEMBERS7(Strength, DisusedField1, Environment, IsReceiversIndexNumber, IndexNumber, Latitude, Longitude);
};

class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 0, 2)
   typedef SpareField<12>    FieldType1;
   typedef FireMethodField   FieldType2;
   typedef AngleField        FieldType3;
   typedef SpeedField        FieldType4;
   typedef AltitudeField<11> FieldType5;
   typedef IdentityField     FieldType6;
   typedef BooleanField      FieldType7;
   typedef IntegerField<12>  FieldType8;
   typedef BooleanField      FieldType9;
   DEFINE_MEMBERS9(SpareField1, MethodOfFire, Heading, Speed, Altitude, Identity, IsExerciseTrackUnit, SpecificType, IsSpecialIntrest)
};

class Continuation3 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 0, 3)
   typedef LatitudeField<23>  FieldType1;
   typedef LongitudeField<24> FieldType2;
   typedef ElevationField<10> FieldType3;
   typedef PointTypeField     FieldType4;
   typedef PointNumberField   FieldType5;
   DEFINE_MEMBERS5(Latitude, Longitude, Elevation, PointType, PointNumber);
};

class Continuation4 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 0, 4)
   typedef TargetTypeField                   FieldType1;
   typedef TargetDefensesField               FieldType2;
   typedef AngleField                        FieldType3;
   typedef AngleField                        FieldType4;
   typedef IntegerFieldNS<6, NoStatementMax> FieldType5;
   typedef StoresTypeField                   FieldType6;
   typedef IntegerField<7>                   FieldType7;
   typedef TimeDiscreteField                 FieldType8;
   typedef SpareField<2>                     FieldType9;
   typedef MinuteField                       FieldType10;
   typedef HourField                         FieldType11;
   DEFINE_MEMBERS11(TargetType,
                    TargetDefenses,
                    RunInHeading,
                    EngressHeading,
                    NumberOfStores,
                    TypeOfStores,
                    ModeOfDelivery,
                    TimeDiscrete,
                    Spare,
                    Minute,
                    Hour);
};

class Continuation5 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 0, 5)
   typedef LengthField         FieldType1;
   typedef AngleField16        FieldType2;
   typedef DeltaElevationField FieldType3;
   typedef IntegerField<6>     FieldType4;
   typedef DisusedField<1>     FieldType5;
   typedef DisusedField<1>     FieldType6;
   typedef DisusedField<3>     FieldType7;
   typedef SpareField<6>       FieldType8;
   DEFINE_MEMBERS8(TargetRange, TargetBearing, DeltaElevation, BeaconCode, LatitudeLSB, LongitudeLSB, ElevationLSB, Spare);
};

class Continuation6 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 0, 6)
   typedef IntegerField<16> FieldType1;
   typedef IntegerField<19> FieldType2;
   typedef IntegerField<6>  FieldType3;
   typedef SpareField<22>   FieldType4;
   DEFINE_MEMBERS4(LaserIlluminatorCode, TrackNumber, IndexNumber, Spare);
};

class Continuation7 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 0, 7);
   typedef IntegerField<15>    FieldType1;
   typedef IntegerField<6>     FieldType2;
   typedef ElevationAngleField FieldType3;
   typedef SpareField<27>      FieldType4;
   DEFINE_MEMBERS4(TrackNumber, IndexNumber, ElevationAngle, SpareField1);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(12, 0);

   typedef TrackNumberAddresseeField FieldType1;
   typedef MissionTypeField          FieldType2;
   typedef DisusedField<3>           FieldType3;
   typedef DisusedField<2>           FieldType4;
   typedef TrackNumberField          FieldType5;
   typedef ThreatEnvironmentField    FieldType6;
   typedef ReceiptComplianceField    FieldType7;
   typedef RecurrenceRateField       FieldType8;

   DEFINE_MEMBERS8(TrackNumber,
                   MissionAssignment,
                   DissusedField1,
                   DissusedField2,
                   TrackNumberObjective,
                   ThreatWarningEnvironment,
                   ReceiptCompliance,
                   RecurrenceRate);

   DEFINE_CONTINUATIONS7(1, 2, 3, 4, 5, 6, 7)
   DEFINE_EXTENSIONS0()

protected:
};

} // namespace J12_0
} // namespace Messages
} // namespace WsfL16

#endif
