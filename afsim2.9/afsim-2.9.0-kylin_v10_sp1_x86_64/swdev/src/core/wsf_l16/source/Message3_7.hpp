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
#ifndef L16_MESSAGE3_7_HPP
#define L16_MESSAGE3_7_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J3_7
{
// Continuation2,3,4,5 not yet defined

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(3, 7, 1);

   typedef DisusedField<7>                    FieldType1;
   typedef BooleanField                       FieldType2;
   typedef BearingField                       FieldType3;
   typedef TrackNumberField                   FieldType4;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType5;
   typedef ElevationAngle10Field              FieldType6;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType7;
   typedef IntegerField<4>                    FieldType8;
   typedef EnvironmentField                   FieldType9;

   DEFINE_MEMBERS9(Disused,
                   UseLatLonAsOrigin,
                   Bearing,
                   TrackNumber,
                   BearingAccuracy,
                   ElevationAngle,
                   PlatformEvaluationConfidence,
                   IdentityConfidence,
                   Environment);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(3, 7);

   typedef BooleanField      FieldType1;
   typedef BooleanField      FieldType2;
   typedef BooleanField      FieldType3;
   typedef BooleanField      FieldType4;
   typedef BooleanField      FieldType5;
   typedef TrackNumberField  FieldType6;
   typedef BooleanField      FieldType7;
   typedef IntegerField<3>   FieldType8;
   typedef MinuteField       FieldType9;
   typedef SecondField       FieldType10;
   typedef HourField         FieldType11;
   typedef BooleanField      FieldType12;
   typedef DisusedField<4>   FieldType13;
   typedef SquareCircleField FieldType14;
   typedef IdentityField     FieldType15;
   typedef DisusedField<1>   FieldType16;
   typedef SpareField<1>     FieldType17;

   DEFINE_MEMBERS17(IsExerciseTrackUnit,
                    IsForceTell,
                    IsEmergency,
                    IsSpecialProcessingRequired,
                    IsSimulated,
                    TrackNumber,
                    IsResponse,
                    FixOrBearingDescriptor,
                    Minute,
                    Second,
                    Hour,
                    IsIdentityDifferent,
                    Disused,
                    CircleSquareSwitch,
                    Identity,
                    Disused2,
                    Spare);
   DEFINE_CONTINUATIONS0();
   DEFINE_EXTENSIONS0();
};

} // namespace J3_7
} // namespace Messages
} // namespace WsfL16


#endif
