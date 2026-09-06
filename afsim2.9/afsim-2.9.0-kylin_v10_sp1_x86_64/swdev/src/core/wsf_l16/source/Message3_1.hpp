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
#ifndef L16_MESSAGE3_1_HPP
#define L16_MESSAGE3_1_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J3_1
{

class Extension0 : public ExtensionBase
{
public:
   enum NuclearBurstType
   {
      cNO_STATEMENT,
      cUNKNOWN,
      cAIR,
      cSURFACE_GROUND,
      cUNDERGROUND,
      cSURFACE_WATER,
      cUNDERWATER,
      cSPACE
   };
   class NuclearBurstTypeField : public NormalField<3, NoStatementZero>
   {
   public:
      DEFINE_ACCESSORS(NuclearBurstTypeField, NuclearBurstType);
   };
   DEFINE_EXTENSION(3, 1, 0);

   typedef SpareField<2>                      FieldType1;
   typedef LatitudeField<21>                  FieldType2;
   typedef SpareField<2>                      FieldType3;
   typedef LongitudeField<22>                 FieldType4;
   typedef SpareField<4>                      FieldType5;
   typedef TimeFunctionField                  FieldType6;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType7;
   typedef MinuteField                        FieldType8;
   typedef HourField                          FieldType9;

   DEFINE_MEMBERS9(Spare, Latitude, Spare2, Longitude, Spare3, TimeFunction, PositionAccuracy, Minute, Hour);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(3, 1, 1);
   typedef SpareField<1>    FieldType1;
   typedef Mode1CodeField   FieldType2;
   typedef IntegerField<12> FieldType3;
   typedef IntegerField<12> FieldType4;
   typedef IntegerField<2>  FieldType5;
   typedef SpareField<31>   FieldType6;

   DEFINE_MEMBERS6(Spare, Mode1Code, Mode2Code, Mode3Code, Mode4Code, Spare2);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(3, 1);

   typedef BooleanField                       FieldType1;
   typedef SpareField<3>                      FieldType2;
   typedef BooleanField                       FieldType3;
   typedef BooleanField                       FieldType4;
   typedef TrackNumberField                   FieldType5;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType6;
   typedef SpareField<5>                      FieldType7;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType8;
   typedef TrackNumberField                   FieldType9;

   DEFINE_MEMBERS9(IsExerciseTrackUnit,
                   Spare,
                   IsSpecialProcessingRequired,
                   IsSimulated,
                   TrackNumber,
                   PersonnelCount,
                   Spare2,
                   EmergencyType,
                   TrackNumberPrevious);

   DEFINE_CONTINUATIONS1(1);
   DEFINE_EXTENSIONS1();
};

} // namespace J3_1
} // namespace Messages
} // namespace WsfL16


#endif
