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
#ifndef L16_MESSAGE7_1_HPP
#define L16_MESSAGE7_1_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J7_1
{

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(7, 1, 1)
   typedef TrackNumberAddresseeField FieldType1;
   typedef TrackNumberAddresseeField FieldType2;
   typedef TrackNumberAddresseeField FieldType3;
   typedef TrackNumberAddresseeField FieldType4;
   typedef SpareField<3>             FieldType5;

   DEFINE_MEMBERS5(Addressee1, Addressee2, Addressee3, Addressee4, Spare);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(7, 1)
   typedef SpareField<1>                      FieldType1;
   typedef ActionField                        FieldType2;
   typedef SpareField<2>                      FieldType3;
   typedef TrackNumberField                   FieldType4;
   typedef BooleanField                       FieldType5;
   typedef SpareField<1>                      FieldType6;
   typedef BooleanField                       FieldType7;
   typedef BooleanField                       FieldType8;
   typedef BooleanField                       FieldType9;
   typedef BooleanField                       FieldType10;
   typedef BooleanField                       FieldType11;
   typedef BooleanField                       FieldType12;
   typedef BooleanField                       FieldType13;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType14;
   typedef IntegerFieldNS<3, NoStatementZero> FieldType15;
   typedef BooleanField                       FieldType16;
   typedef TrackNumberAddresseeField          FieldType17;

   DEFINE_MEMBERS17(Spare1,
                    Action,
                    Spare2,
                    Spare3,
                    TrackNumber1,
                    IsReferencePt,
                    DisUsed,
                    IsElectronicAttack,
                    IsESData,
                    IsElectronicWarfareFixes,
                    IsWpnStatusData,
                    IsWeatherStatusData,
                    IsAmpData,
                    FilterNumber,
                    FreqOfUpdate,
                    IsCovarianceIndicator,
                    Addressee);

   DEFINE_CONTINUATIONS1(1)
   DEFINE_EXTENSIONS0()
};

} // namespace J7_1
} // namespace Messages
} // namespace WsfL16


#endif
