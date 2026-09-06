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
#ifndef L16_MESSAGE3_2_HPP
#define L16_MESSAGE3_2_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J3_2
{
class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 2, 0);
   typedef SpareField<2>      FieldType1;
   typedef LatitudeField<21>  FieldType2;
   typedef DisusedField<1>    FieldType3;
   typedef SpareField<1>      FieldType4;
   typedef LongitudeField<22> FieldType5;
   typedef BooleanField       FieldType6;
   typedef AngleField         FieldType7;
   typedef SpeedField         FieldType8;

   DEFINE_MEMBERS8(Spare, Latitude, Disused, Spare2, Longitude, IsSensorActive, Course, Speed);
};

class Extension1 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 2, 1);
   typedef DisSiteField        FieldType1;
   typedef DisApplicationField FieldType2;
   typedef DisEntityIdField    FieldType3;
   typedef SpareField<20>      FieldType4;

   DEFINE_MEMBERS4(DisSite, DisApplication, DisEntityId, Spare);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(3, 2, 1);
   typedef BooleanField     FieldType1;
   typedef Mode1CodeField   FieldType2;
   typedef IntegerField<12> FieldType3;
   typedef IntegerField<12> FieldType4;
   typedef IntegerField<2>  FieldType5;
   typedef IntegerField<2>  FieldType6;
   typedef IntegerField<12> FieldType7;
   typedef SpareField<1>    FieldType8;
   typedef SpareField<5>    FieldType9;
   typedef MinuteField      FieldType10;
   typedef HourField        FieldType11;

   DEFINE_MEMBERS11(IsAirSpecificType,
                    Mode1Code,
                    Mode2Code,
                    Mode3Code,
                    Mode4Code,
                    PPLI_IFF_Indicator,
                    AirSpecificType,
                    Spare,
                    Spare2,
                    Minute,
                    Hour);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(3, 2);

   typedef BooleanField        FieldType1;
   typedef BooleanField        FieldType2;
   typedef BooleanField        FieldType3;
   typedef BooleanField        FieldType4;
   typedef BooleanField        FieldType5;
   typedef BooleanField        FieldType6;
   typedef TrackNumberField    FieldType7;
   typedef TrackStrengthField  FieldType8;
   typedef AltitudeSourceField FieldType9;
   typedef AltitudeField<13>   FieldType10;
   typedef BooleanField        FieldType11;
   typedef TrackQualityField   FieldType12;
   typedef IntegerField<4>     FieldType13;
   typedef IdentityField       FieldType14;
   typedef BooleanField        FieldType15;

   DEFINE_MEMBERS15(IsExerciseTrackUnit,
                    IsSourcePPLI,
                    IsForceTell,
                    IsEmergencyStatus,
                    IsSpecialProcessingRequired,
                    IsSimulated,
                    TrackNumber,
                    TrackStrength,
                    AltitudeSource,
                    Altitude,
                    IsIdentityDifference,
                    TrackQuality,
                    IdentityConfidence,
                    Identity,
                    IsSpecialIntrest);

   DEFINE_CONTINUATIONS1(1);
   DEFINE_EXTENSIONS2();
};

} // namespace J3_2
} // namespace Messages
} // namespace WsfL16


#endif
