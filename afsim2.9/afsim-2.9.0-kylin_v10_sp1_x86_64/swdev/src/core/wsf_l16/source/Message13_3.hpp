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
#ifndef L16_MESSAGE13_3_HPP
#define L16_MESSAGE13_3_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J13_3
{
class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(13, 3, 1);
   typedef MissileTypeField                   FieldType1;
   typedef SpareField<1>                      FieldType2;
   typedef IntegerFieldNS<7, NoStatementMax>  FieldType3;
   typedef SpareField<1>                      FieldType4;
   typedef IntegerFieldNS<4, NoStatementMax>  FieldType5;
   typedef MissileTypeField                   FieldType6;
   typedef SpareField<1>                      FieldType7;
   typedef IntegerFieldNS<7, NoStatementMax>  FieldType8;
   typedef SpareField<1>                      FieldType9;
   typedef IntegerFieldNS<4, NoStatementMax>  FieldType10;
   typedef MissileTypeField                   FieldType11;
   typedef SpareField<1>                      FieldType12;
   typedef IntegerFieldNS<7, NoStatementMax>  FieldType13;
   typedef SpareField<1>                      FieldType14;
   typedef IntegerFieldNS<4, NoStatementMax>  FieldType15;
   typedef BooleanField                       FieldType16;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType17;
   typedef SpareField<2>                      FieldType18;
   DEFINE_MEMBERS18(MissileType1,
                    Spare,
                    MissileCount1,
                    Spare2,
                    FireControlSystemCount1,
                    MissileType2,
                    Spare3,
                    MissileCount2,
                    Spare4,
                    FireControlSystemCount2,
                    MissileType3,
                    Spare5,
                    MissileCount3,
                    Spare6,
                    FireControlSystemCount3,
                    IsOffloadableFuel,
                    FuelAmmount,
                    Spare7);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(13, 3);

   typedef OperationalCapabilityField FieldType1;
   typedef SpareField<2>              FieldType2;
   typedef TrackNumberField           FieldType3;
   typedef IntegerField<3>            FieldType4;
   typedef IntegerField<3>            FieldType5;
   typedef TimeReportField            FieldType6;
   typedef MinuteField                FieldType7;
   typedef HourField                  FieldType8;
   typedef IntegerField<12>           FieldType9;
   typedef SpareField<2>              FieldType10;

   DEFINE_MEMBERS10(OperationalCapability,
                    Spare,
                    TrackNumber,
                    FlightDeckStatus,
                    LandingApproachCondition,
                    ReportTimeFunction,
                    Minute,
                    Hour,
                    SurfaceSpecificType,
                    Spare2);

   DEFINE_CONTINUATIONS1(1);
   DEFINE_EXTENSIONS0();
};

} // namespace J13_3
} // namespace Messages
} // namespace WsfL16


#endif
