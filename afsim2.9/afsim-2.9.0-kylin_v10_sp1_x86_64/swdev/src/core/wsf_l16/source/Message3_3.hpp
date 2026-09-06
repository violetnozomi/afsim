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
#ifndef L16_MESSAGE3_3_HPP
#define L16_MESSAGE3_3_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J3_3
{

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 3, 0);

   typedef SpareField<2>      FieldType1;
   typedef LatitudeField<21>  FieldType2;
   typedef DisusedField<1>    FieldType3;
   typedef SpareField<1>      FieldType4;
   typedef LongitudeField<22> FieldType5;
   typedef BooleanField       FieldType6;
   typedef AngleField         FieldType7;
   typedef SpeedField         FieldType8;
   DEFINE_MEMBERS8(TrackIdentifier, Latitude, Dissused, Spare, Longitude, ActiveSensor, Course, Speed);
};

class Extension1 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 3, 1);
   typedef DisSiteField        FieldType1;
   typedef DisApplicationField FieldType2;
   typedef DisEntityIdField    FieldType3;
   typedef SpareField<20>      FieldType4;

   DEFINE_MEMBERS4(DisSite, DisApplication, DisEntityId, Spare);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(3, 3, 1);
   typedef BooleanField     FieldType1;
   typedef Mode1CodeField   FieldType2;
   typedef IntegerField<12> FieldType3;
   typedef IntegerField<12> FieldType4;
   typedef IntegerField<2>  FieldType5;
   typedef IntegerField<2>  FieldType6;
   typedef IntegerField<6>  FieldType7;
   typedef IntegerField<7>  FieldType8;
   typedef SpareField<5>    FieldType9;
   typedef MinuteField      FieldType10;
   typedef HourField        FieldType11;

   DEFINE_MEMBERS11(IsAirSpecificType,
                    Mode1Code,
                    Mode2Code,
                    Mode3Code,
                    Mode4Code,
                    PPLI_IFF_Indicator,
                    SurfacePlatform,
                    SurfacePlatformActivity,
                    Spare,
                    Minute,
                    Hour);
};

class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(3, 3, 2);
   typedef IntegerFieldNS<9, NoStatementZero> FieldType1;
   typedef IntegerFieldNS<9, NoStatementZero> FieldType2;
   typedef IntegerFieldNS<9, NoStatementZero> FieldType3;
   typedef SpareField<20>                     FieldType4;
   typedef SpareField<16>                     FieldType5;
   DEFINE_MEMBERS5(SAM_Type1, SAM_Type2, SAM_Type3, Spare, Spare2);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(3, 3);

   typedef BooleanField                        FieldType1;
   typedef BooleanField                        FieldType2;
   typedef BooleanField                        FieldType3;
   typedef BooleanField                        FieldType4;
   typedef BooleanField                        FieldType5;
   typedef BooleanField                        FieldType6;
   typedef TrackNumberField                    FieldType7;
   typedef TrackStrengthField                  FieldType8;
   typedef IntegerFieldNS<12, NoStatementZero> FieldType9;
   typedef SpareField<3>                       FieldType10;
   typedef BooleanField                        FieldType11;
   typedef TrackQualityField                   FieldType12;
   typedef IdentityField                       FieldType13;
   typedef BooleanField                        FieldType14; // Dummy field (1 bit) to pass message size check
   typedef IdentityAmplifyField                FieldType15;
   typedef BooleanField                        FieldType16;

   DEFINE_MEMBERS16(IsExerciseTrackUnit,
                    IsSourcePPLI,
                    IsForceTell,
                    IsEmergencyStatus,
                    IsSpecialProcessingRequired,
                    IsSimulated,
                    TrackNumber,
                    TrackStrength,
                    SurfaceSpecificType,
                    Spare,
                    IsIdentityDifference,
                    TrackQuality,
                    Identity,
                    Dummy,
                    IdentityAmplify,
                    IsSpecialIntrest);

   DEFINE_CONTINUATIONS2(1, 2);
   DEFINE_EXTENSIONS2();
};

} // namespace J3_3
} // namespace Messages
} // namespace WsfL16


#endif
