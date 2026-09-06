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
#ifndef L16_MESSAGE3_4_HPP
#define L16_MESSAGE3_4_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J3_4
{
// Continuation2 not yet defined

class DataReportTypeField : public NormalField<3, NoStatementZero>
{
public:
   enum ReportType
   {
      cSUBSURFACE_TRACK,
      cSURFACED_SUBMARINE,
      cSNORKELING_SUBMARINE,
      cDATUM = 4
   };
   DEFINE_ACCESSORS(DataReportTypeField, ReportType);
   DEFINE_ENUM_MAP()
};

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 4, 0);

   typedef SpareField<3>      FieldType1;
   typedef LatitudeField<21>  FieldType2;
   typedef LongitudeField<22> FieldType3;
   typedef SpareField<2>      FieldType4;
   typedef AngleField         FieldType5;
   typedef SpeedASW_Field     FieldType6;
   typedef SpareField<2>      FieldType7;

   DEFINE_MEMBERS7(Spare, Latitude, Longitude, Spare2, Course, Speed, Spare3);
};

class Extension1 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(3, 4, 1);
   typedef DisSiteField        FieldType1;
   typedef DisApplicationField FieldType2;
   typedef DisEntityIdField    FieldType3;
   typedef SpareField<20>      FieldType4;

   DEFINE_MEMBERS4(DisSite, DisApplication, DisEntityId, Spare);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(3, 4, 1);

   typedef MinuteField                         FieldType1;
   typedef HourField                           FieldType2;
   typedef DepthField                          FieldType3;
   typedef IntegerFieldNS<4, NoStatementZero>  FieldType4;
   typedef IntegerFieldNS<5, NoStatementZero>  FieldType5;
   typedef Mode1CodeField                      FieldType6;
   typedef IntegerFieldNS<12, NoStatementZero> FieldType7;
   typedef IntegerFieldNS<12, NoStatementZero> FieldType8;
   typedef IntegerFieldNS<2, NoStatementZero>  FieldType9;
   typedef IntegerFieldNS<2, NoStatementZero>  FieldType10;
   typedef SpareField<3>                       FieldType11;

   DEFINE_MEMBERS11(Minute, Hour, Depth, DepthContact, Sensor, Mode1Code, Mode2Code, Mode3Code, Mode4Code, TimeFunction, Spare);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(3, 4);

   typedef BooleanField                       FieldType1;
   typedef BooleanField                       FieldType2;
   typedef BooleanField                       FieldType3;
   typedef BooleanField                       FieldType4;
   typedef BooleanField                       FieldType5;
   typedef BooleanField                       FieldType6;
   typedef TrackNumberField                   FieldType7;
   typedef DataReportTypeField                FieldType8;
   typedef IdentityField                      FieldType9;
   typedef ConfidenceField                    FieldType10;
   typedef SubsurfacePlatformField            FieldType11;
   typedef SubsurfacePlatformActivityField    FieldType12;
   typedef BooleanField                       FieldType13;
   typedef IntegerFieldNS<2, NoStatementZero> FieldType14;
   typedef SpareField<5>                      FieldType15;

   DEFINE_MEMBERS15(IsExerciseTrackUnit,
                    IsSourcePPLI,
                    IsForceTell,
                    IsEmergencyStatus,
                    IsSpecialProcessingRequired,
                    IsSimulated,
                    TrackNumber,
                    DataReportType,
                    Identity,
                    ConfidenceLevel,
                    SubsurfacePlatform,
                    SubsurfacePlatformActivity,
                    IsIdentityDifference,
                    LaunchCapability,
                    Spare);

   DEFINE_CONTINUATIONS1(1);
   DEFINE_EXTENSIONS2();
};

} // namespace J3_4
} // namespace Messages
} // namespace WsfL16


#endif
