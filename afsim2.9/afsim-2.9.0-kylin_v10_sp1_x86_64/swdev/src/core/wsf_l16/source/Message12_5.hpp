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
#ifndef L16_MESSAGE12_5_HPP
#define L16_MESSAGE12_5_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J12_5
{

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(12, 5, 0);
   typedef IntegerFieldNS<15, NoStatementZero> FieldType1;
   typedef IntegerFieldNS<6, NoStatementZero>  FieldType2;
   typedef BooleanField                        FieldType3;
   typedef IntegerFieldNS<15, NoStatementZero> FieldType4;
   typedef IntegerFieldNS<6, NoStatementZero>  FieldType5;
   typedef BooleanField                        FieldType6;
   typedef IntegerFieldNS<15, NoStatementZero> FieldType7;
   typedef IntegerFieldNS<6, NoStatementZero>  FieldType8;
   typedef BooleanField                        FieldType9;
   typedef SpareField<2>                       FieldType10;
   DEFINE_MEMBERS10(OriginatorTrackNumberB,
                    OriginatorIndexB,
                    IsCorrelatedB,
                    OriginatorTrackNumberC,
                    OriginatorIndexC,
                    IsCorrelatedC,
                    OriginatorTrackNumberD,
                    OriginatorIndexD,
                    IsCorrelatedD,
                    Spare);
};

class Extension1 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(12, 5, 1);
   typedef IntegerFieldNS<15, NoStatementZero> FieldType1;
   typedef IntegerFieldNS<6, NoStatementZero>  FieldType2;
   typedef BooleanField                        FieldType3;
   typedef IntegerFieldNS<15, NoStatementZero> FieldType4;
   typedef IntegerFieldNS<6, NoStatementZero>  FieldType5;
   typedef BooleanField                        FieldType6;
   typedef IntegerFieldNS<15, NoStatementZero> FieldType7;
   typedef IntegerFieldNS<6, NoStatementZero>  FieldType8;
   typedef BooleanField                        FieldType9;
   typedef SpareField<2>                       FieldType10;
   DEFINE_MEMBERS10(OriginatorTrackNumberE,
                    OriginatorIndexE,
                    IsCorrelatedE,
                    OriginatorTrackNumberF,
                    OriginatorIndexF,
                    IsCorrelatedF,
                    OriginatorTrackNumberG,
                    OriginatorIndexG,
                    IsCorrelatedG,
                    Spare);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(12, 5, 1)


   typedef SpareField<6>                       FieldType1;
   typedef IntegerFieldNS<15, NoStatementZero> FieldType2;
   typedef BooleanField                        FieldType3;
   typedef WeaponSystemField                   FieldType4;
   typedef WeaponEngagementStatusField         FieldType5;
   typedef TrackNumberField                    FieldType6;
   typedef Mode3InterrogationTypeField         FieldType7;
   typedef Mode4IndicatorField                 FieldType8;
   typedef TrackStrengthField                  FieldType9;
   typedef SpareField<5>                       FieldType10;

   DEFINE_MEMBERS10(Spare,
                    TrackNumber,
                    Pointer,
                    WeaponSystem,
                    WeaponEngagementStatus,
                    TrackNumberReference,
                    Mode3_IFF,
                    Mode4_Indicator,
                    Strength,
                    Spare2);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(12, 5);

   typedef TrackNumberField                   FieldType1;
   typedef TrackNumberAddresseeField          FieldType2;
   typedef IntegerFieldNS<6, NoStatementZero> FieldType3;
   typedef BooleanField                       FieldType4;
   typedef IntegerField<3>                    FieldType5;
   typedef TrackStrengthField                 FieldType6;
   typedef IntegerField<2>                    FieldType7;
   typedef SpareField<7>                      FieldType8;

   DEFINE_MEMBERS8(ObjectiveTrackNumber, OriginatorTrackNumber, IndexNumber, IsCorrelated, Identity, TrackStrength, IdIndicator, Spare);

   DEFINE_CONTINUATIONS0();
   DEFINE_EXTENSIONS2();

protected:
};


} // namespace J12_5
} // namespace Messages
} // namespace WsfL16

#endif
