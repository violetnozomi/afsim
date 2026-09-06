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
#ifndef L16_MESSAGE12_7_HPP
#define L16_MESSAGE12_7_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J12_7
{
// Continuation2 and Continuation3 not yet defined.

class RangeField : public NormalField<16, NoStatementMax>
{
public:
   typedef double                 accessor_type;
   typedef AccessorUnitsMetersTag accessor_units;
   RangeField&                    operator=(double aVal)
   {
      mValue = (value_type)(MetersToFeet(aVal) / 15);
      return *this;
   }
   operator double() const { return FeetToMeters(mValue * 15); }
};

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(12, 7, 0);
   typedef AltitudeField<11>                   FieldType1;
   typedef ElevationAngleField                 FieldType2;
   typedef BearingInterceptField               FieldType3;
   typedef IntegerFieldNS<5, NoStatementZero>  FieldType4;
   typedef IntegerFieldNS<5, NoStatementZero>  FieldType5;
   typedef IntegerFieldNS<10, NoStatementZero> FieldType6;
   typedef IntegerField<4>                     FieldType7;
   typedef SpareField<2>                       FieldType8;

   DEFINE_MEMBERS8(Altitude,
                   ElevationAngle,
                   BearingIntercept,
                   ElevationUncertainty,
                   BearingUncertainty,
                   FrequencyIntercept,
                   FrequencyExponent,
                   Spare);
};

class Extension1 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(12, 7, 1);

   typedef TimeIndicatorField FieldType1;
   typedef MinuteField        FieldType2;
   typedef SecondField        FieldType3;
   typedef HundredthsField    FieldType4;

   // BearingAngleRate
   typedef AngleRateField FieldType5;
   // typedef IntegerFieldNS<9, NoStatementMax>    FieldType5;

   typedef IntegerFieldNS<5, NoStatementZero> FieldType6;

   // ElevationAngleRate
   typedef AngleRateField FieldType7;
   // typedef IntegerFieldNS<9, NoStatementMax>    FieldType7;

   typedef IntegerFieldNS<5, NoStatementZero> FieldType8;
   typedef PRF_TypeField                      FieldType9;
   typedef EmitterStatusField                 FieldType10;
   // typedef SpareField<12>                       FieldType11;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType11;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType12;
   typedef BooleanField                       FieldType13;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType14;

   DEFINE_MEMBERS14(TimeIndicator,
                    Minute,
                    Second,
                    Hundredths,
                    BearingAngleRate,
                    BearingAngleRateUncertainty,
                    ElevationAngleRate,
                    ElevationAngleRateUncertainty,
                    EmitterPRF,
                    EmitterStatus,
                    SID,
                    ID,
                    Exercise,
                    Spare);
   // Spare);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 7, 1)

   typedef RangeField                                 FieldType1;
   typedef IntegerFieldNS<4, NoStatementZero>         FieldType2;
   typedef SpareField<1>                              FieldType3;
   typedef IntegerFieldNS<13, NoStatementValue<4096>> FieldType4;
   typedef IntegerFieldNS<5, NoStatementZero>         FieldType5;
   typedef IntegerFieldNS<12, NoStatementValue<2048>> FieldType6;
   typedef IntegerFieldNS<12, NoStatementValue<2048>> FieldType7;

   DEFINE_MEMBERS7(Range, RangeUncertainty, Spare, RangeRate, RangeRateUncertainty, ObserverVelocityNorth, ObserverVelocityEast);
};

class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 7, 2)
   /* ERIN...
      typedef IntegerFieldNS<3,NoStatementZero>     FieldType1;
      typedef IntegerFieldNS<3,NoStatementZero>     FieldType2;
      typedef IntegerFieldNS<12, NoStatementZero>   FieldType3;
      typedef IntegerFieldNS<12, NoStatementZero>   FieldType4;
      typedef IntegerFieldNS<3,NoStatementZero>     FieldType5;
      typedef IntegerFieldNS<3,NoStatementZero>     FieldType6;
      typedef EnvironmentField                      FieldType7;  // 3 bits
      typedef SpareField<24>                        FieldType8;  // ERIN
      // ERIN
      DEFINE_MEMBERS8(ProbabilityOwnForces, ProbabilityEnemyForces, SpaceAmbiguousType, LandAmbiguousType,
      Type1Probability, Type2Probability, Environment, Spare_12_7_2);
   */

   typedef IntegerFieldNS<9, NoStatementZero>  FieldType1;
   typedef IntegerFieldNS<9, NoStatementZero>  FieldType2;
   typedef IntegerFieldNS<12, NoStatementZero> FieldType3;
   typedef IntegerFieldNS<12, NoStatementZero> FieldType4;
   typedef IntegerFieldNS<9, NoStatementZero>  FieldType5;
   typedef IntegerFieldNS<9, NoStatementZero>  FieldType6;
   typedef EnvironmentField                    FieldType7;
   DEFINE_MEMBERS7(ProbabilityOwnForces,
                   ProbabilityEnemyForces,
                   AmbiguousTypeI,
                   AmbiguousTypeII,
                   Type1Probability,
                   Type2Probability,
                   Environment);
};

class Continuation3 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 7, 3)

   typedef IntegerFieldNS<12, NoStatementValue<2048>> FieldType1;
   typedef TrackNumberField                           FieldType2;
   typedef SpareField<16>                             FieldType3;
   typedef SpareField<16>                             FieldType4;
   DEFINE_MEMBERS4(ObserverVelocityDown, TrackNumberObjective, Spare1, Spare2);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(12, 7);

   typedef IntegerFieldNS<6, NoStatementZero> FieldType1;
   typedef BooleanField                       FieldType2;
   typedef BooleanField                       FieldType3;
   typedef BooleanField                       FieldType4;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType5;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType6;
   typedef LatitudeField<20>                  FieldType7;
   typedef LongitudeField<21>                 FieldType8;

   DEFINE_MEMBERS8(IndexNumber, IsOffboard, IsMultipleTargets, IsLastInSequence, SensorType, JammerType, Latitude, Longitude);

   // ERIN
   DEFINE_CONTINUATIONS3(1, 2, 3)
   DEFINE_EXTENSIONS2()

protected:
};


} // namespace J12_7
} // namespace Messages
} // namespace WsfL16

#endif
