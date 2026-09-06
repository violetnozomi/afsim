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
#ifndef L16_MESSAGE9_0_HPP
#define L16_MESSAGE9_0_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J9_0
{
class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(9, 0, 1);
   typedef CallsignField<4>    FieldType1;
   typedef Frequency13Field    FieldType2;
   typedef ControlChannelField FieldType3;
   typedef BooleanField        FieldType4;
   typedef BooleanField        FieldType5;
   typedef FireMethodField     FieldType6;
   typedef SpareField<14>      FieldType7;

   DEFINE_MEMBERS7(Callsign,
                   Frequency,
                   ControlChannel,
                   IsSecureRadioIndicator,
                   IsAlternateVoiceFrequencyChannel,
                   MethodOfFire,
                   Spare);
};


class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(9, 0, 2);
   typedef HourField       FieldType1;
   typedef MinuteField     FieldType2;
   typedef SecondField     FieldType3;
   typedef IntegerField<3> FieldType4;
   typedef SpareField<30>  FieldType5; // SpareField max is 31, so splitting up
   typedef SpareField<13>  FieldType6; // spare<43>

   DEFINE_MEMBERS6(Hour, Minute, Second, NumberOfMissiles, Spare1, Spare2);
};

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(9, 0, 0);
   typedef IntegerFieldNS<6, NoStatementMax>  FieldType1;
   typedef IntegerFieldNS<5, NoStatementMax>  FieldType2;
   typedef IntegerFieldNS<6, NoStatementMax>  FieldType3;
   typedef SpareField<16>                     FieldType4;
   typedef SpareField<5>                      FieldType5;
   typedef SpareField<2>                      FieldType6;
   typedef ThreatEnvironmentField             FieldType7;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType8;
   typedef SpareField<2>                      FieldType9;
   typedef TrackNumberField                   FieldType10;

   DEFINE_MEMBERS10(DutyAssignment,
                    Mission,
                    NumberOfAircraft,
                    DisUsed1,
                    DisUsed2,
                    DisUsed3,
                    ThreatEnvironment,
                    DutyAssignmentArea,
                    Spare,
                    TrackNumber);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(9, 0);
   typedef TrackNumberAddresseeField FieldType1;
   typedef CommandField              FieldType2;
   typedef ThreatWarningField        FieldType3;
   typedef WeaponTypeField           FieldType4;
   typedef TrackNumberField          FieldType5;
   typedef SpareField<3>             FieldType6;
   typedef BooleanField              FieldType7;
   typedef SpareField<1>             FieldType8;
   typedef ReceiptComplianceField    FieldType9;
   typedef RecurrenceRateField       FieldType10;

   DEFINE_MEMBERS10(Addressee,
                    Command,
                    ThreatWarningCondition,
                    WeaponType,
                    TrackNumber,
                    DisUsed,
                    IsExerciseIndicator,
                    Spare,
                    ReceiptCompliance,
                    RecurrenceRate);

   DEFINE_CONTINUATIONS2(1, 2)
   DEFINE_EXTENSIONS1()
};

} // namespace J9_0
} // namespace Messages
} // namespace WsfL16


#endif
