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
#ifndef L16_MESSAGE10_2_HPP
#define L16_MESSAGE10_2_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J10_2
{
class SecondTenthsField : public NormalField<10, NoStatementMax>
{
public:
   typedef double     accessor_type;
   SecondTenthsField& operator=(double aValue)
   {
      mValue = (value_type)(aValue * 10.0);
      return *this;
   }
   operator double() const { return (double)(mValue / 10.0); }

   bool IsIllegalValue() const { return mValue >= 600 && mValue <= 1022; }
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(10, 2, 1);
   typedef TrackNumberField            FieldType1;
   typedef WeaponEngagementStatusField FieldType2;
   typedef TrackNumberField            FieldType3;
   typedef WeaponEngagementStatusField FieldType4;
   typedef SpareField<17>              FieldType5;

   DEFINE_MEMBERS5(TrackNumber1, AirWeaponEngagement1, TrackNumber2, AirWeaponEngagement2, Spare);
};

class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(10, 2, 2);
   typedef TrackNumberField           FieldType1;
   typedef TimeFunctionInterceptField FieldType2;
   typedef SecondTenthsField          FieldType3;
   typedef MinuteField                FieldType4;
   typedef SpareField<25>             FieldType5;

   DEFINE_MEMBERS5(TrackNumber, TimeFunctionIntercept, Second, Minute, Spare);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(10, 2);
   typedef TrackNumberField            FieldType1;
   typedef TrackNumberField            FieldType2;
   typedef WeaponSystemField           FieldType3;
   typedef SpareField<1>               FieldType4;
   typedef WeaponEngagementStatusField FieldType5;
   typedef SpareField<9>               FieldType6;

   DEFINE_MEMBERS6(TrackNumber1, TrackNumber2, WeaponSystem, Spare1, WeaponEngagementStatus, Spare2);

   DEFINE_CONTINUATIONS2(1, 2)
   DEFINE_EXTENSIONS0()
};

} // namespace J10_2
} // namespace Messages
} // namespace WsfL16


#endif
