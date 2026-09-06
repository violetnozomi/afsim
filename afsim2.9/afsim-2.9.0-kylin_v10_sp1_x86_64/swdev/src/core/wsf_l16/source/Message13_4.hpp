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
#ifndef L16_MESSAGE13_4_HPP
#define L16_MESSAGE13_4_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J13_4
{
class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(13, 4, 1);
   typedef MissileTypeField                   FieldType1;
   typedef IntegerFieldNS<7, NoStatementMax>  FieldType2;
   typedef MissileTypeField                   FieldType3;
   typedef IntegerFieldNS<7, NoStatementMax>  FieldType4;
   typedef MissileTypeField                   FieldType5;
   typedef IntegerFieldNS<7, NoStatementMax>  FieldType6;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType7;
   typedef IntegerFieldNS<3, NoStatementMax>  FieldType8;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType9;
   typedef IntegerFieldNS<3, NoStatementMax>  FieldType10;
   typedef SystemStatusField                  FieldType11;
   typedef SystemStatusField                  FieldType12;
   typedef SystemStatusField                  FieldType13;
   typedef SpareField<4>                      FieldType14;

   DEFINE_MEMBERS14(MissileType1,
                    MissileCount1,
                    MissileType2,
                    MissileCount2,
                    MissileType3,
                    MissileCount3,
                    TorpedoType1,
                    TorpedoCount1,
                    TorpedoType2,
                    TorpedoCount2,
                    TorpedoTubeStatus,
                    VerticalLauncherStatus,
                    FireControlSystemStatus,
                    Spare);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(13, 4);

   typedef SpareField<4>    FieldType1;
   typedef TrackNumberField FieldType2;
   typedef IntegerField<12> FieldType3;
   typedef SpareField<22>   FieldType4;

   DEFINE_MEMBERS4(Spare, TrackNumberReference, SubsurfaceSpecificType, Spare2);

   DEFINE_CONTINUATIONS1(1);
   DEFINE_EXTENSIONS0();
};

} // namespace J13_4
} // namespace Messages
} // namespace WsfL16


#endif
