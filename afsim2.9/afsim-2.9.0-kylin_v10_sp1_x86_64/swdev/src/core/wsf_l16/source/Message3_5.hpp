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
#ifndef L16_MESSAGE3_5_HPP
#define L16_MESSAGE3_5_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J3_5
{

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(3, 5, 1);

   // IsLandSpecificType determines which fields to send...
   //       template <class AR>
   //       void Transfer(AR& ar)
   //       {
   //          ar << IsLandSpecificType << Mode1Code << Mode2Code << Mode3Code
   //             << Mode4Code << PPLI_IFF_Indicator;
   //          if (IsLandSpecificType)
   //          {
   //             ar << LandSpecificType;
   //          }
   //          else
   //          {
   //             ar << LandPlatform << PlatformActivity;
   //          }
   //          ar << Spare1 << Spare2 << TimeFunction << Minute << Hour;
   //       }

   typedef BooleanField      FieldType1;
   typedef Mode1CodeField    FieldType2;
   typedef IntegerField<12>  FieldType3;
   typedef IntegerField<12>  FieldType4;
   typedef IntegerField<2>   FieldType5;
   typedef IntegerField<2>   FieldType6;
   typedef IntegerField<12>  FieldType7;
   typedef SpareField<1>     FieldType8;
   typedef SpareField<2>     FieldType9;
   typedef TimeFunctionField FieldType10;
   typedef MinuteField       FieldType11;
   typedef HourField         FieldType12;

   DEFINE_MEMBERS12(IsLandSpecificType,
                    Mode1Code,
                    Mode2Code,
                    Mode3Code,
                    Mode4Code,
                    PPLI_IFF_Indicator,
                    LandSpecificType,
                    Spare1,
                    Spare2,
                    TimeFunction,
                    Minute,
                    Hour)
private:
};

class Extension0 : public ExtensionBase
{
public:
   class TrackIdentifierField : public NormalField<2, NoStatementZero>
   {
   public:
      enum TrackIdentifierType
      {
         cNO_STATEMENT,
         cE_TRACK,
         cA_TRACK,
         cUNDEFINED
      };
      DEFINE_ACCESSORS(TrackIdentifierField, TrackIdentifierType);
   };

   DEFINE_EXTENSION(3, 5, 0);

   typedef TrackIdentifierField FieldType1;
   typedef LatitudeField<21>    FieldType2;
   typedef DisusedField<1>      FieldType3;
   typedef SpareField<1>        FieldType4;
   typedef LongitudeField<22>   FieldType5;
   typedef BooleanField         FieldType6;
   typedef AngleField           FieldType7;
   typedef SpeedField           FieldType8;
   DEFINE_MEMBERS8(TrackIdentifier, Latitude, Dissused, Spare, Longitude, ActiveSensor, Course, Speed);
};

class Extension1 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 5, 1);
   typedef DisSiteField        FieldType1;
   typedef DisApplicationField FieldType2;
   typedef DisEntityIdField    FieldType3;
   typedef SpareField<20>      FieldType4;

   DEFINE_MEMBERS4(DisSite, DisApplication, DisEntityId, Spare);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(3, 5);

   typedef BooleanField        FieldType1;
   typedef DisusedField<1>     FieldType2;
   typedef BooleanField        FieldType3;
   typedef BooleanField        FieldType4;
   typedef BooleanField        FieldType5;
   typedef BooleanField        FieldType6;
   typedef TrackNumberField    FieldType7;
   typedef TrackStrengthField  FieldType8;
   typedef ElevationField<11>  FieldType9;
   typedef PointTrackField     FieldType10;
   typedef PeriodicReportField FieldType11;
   typedef BooleanField        FieldType12;
   typedef BooleanField        FieldType13;
   typedef TrackQualityField   FieldType14;
   typedef IntegerField<4>     FieldType15;
   typedef IdentityField       FieldType16;
   typedef BooleanField        FieldType17;

   DEFINE_MEMBERS17(IsExerciseTrackUnit,
                    Dissused,
                    IsForceTell,
                    IsEmergencyStatus,
                    IsSpecialProcessingRequired,
                    IsSimulated,
                    TrackNumber,
                    TrackStrength,
                    Altitude,
                    PointOrTrack,
                    ReportType,
                    IsSourcePPLI,
                    IsIdentityDifference,
                    TrackQuality,
                    IdentityConfidence,
                    Identity,
                    IsSpecialIntrest);
   DEFINE_CONTINUATIONS1(1);
   DEFINE_EXTENSIONS2();
};

} // namespace J3_5
} // namespace Messages
} // namespace WsfL16


#endif
