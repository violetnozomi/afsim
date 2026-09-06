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
#ifndef L16_MESSAGE15_0_HPP
#define L16_MESSAGE15_0_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J15_0
{

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(15, 0, 0);
   typedef SpareField<2>      FieldType1;
   typedef LatitudeField<21>  FieldType2;
   typedef SpareField<2>      FieldType3;
   typedef LongitudeField<22> FieldType4;
   typedef SpareField<2>      FieldType5;
   typedef TrackNumberField   FieldType6;
   DEFINE_MEMBERS6(Spare, Latitude, Spare2, Longitude, Spare3, TrackNumber);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(15, 0, 1)

   typedef SpareField<4>                       FieldType1;
   typedef AngleField                          FieldType2;
   typedef SpeedField                          FieldType3;
   typedef AltitudeField<13>                   FieldType4;
   typedef DisusedField<6>                     FieldType5;
   typedef IntegerFieldNS<5, NoStatementZero>  FieldType6;
   typedef IntegerFieldNS<12, NoStatementZero> FieldType7;
   typedef EnvironmentField                    FieldType8;
   DEFINE_MEMBERS8(Spare, Course, Speed, Altitude, Dissused, ThreatFuel, SpecificType, Environment);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(15, 0);

   typedef TrackNumberAddresseeField          FieldType1;
   typedef ThreatPostureField                 FieldType2;
   typedef SpareField<2>                      FieldType3;
   typedef TrackNumberField                   FieldType4;
   typedef TrackStrengthField                 FieldType5;
   typedef BooleanField                       FieldType6;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType7;
   typedef ThreatWeaponField                  FieldType8;
   typedef SpareField<4>                      FieldType9;

   DEFINE_MEMBERS9(Addressee, ThreatPosture, Spare, TrackNumber, TrackStrength, IsExerciseTrack, ThreatType, WeaponType, Spare2);

   DEFINE_CONTINUATIONS1(1)
   DEFINE_EXTENSIONS1()

protected:
};

} // namespace J15_0
} // namespace Messages
} // namespace WsfL16

#endif
