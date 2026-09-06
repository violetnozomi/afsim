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
#ifndef L16_MESSAGE7_0_HPP
#define L16_MESSAGE7_0_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J7_0
{
class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(7, 0, 1);
   typedef IntegerFieldNS<12, NoStatementZero> FieldType1;
   typedef SpareField<20>                      FieldType2;
   typedef SpareField<31>                      FieldType3;

   DEFINE_MEMBERS3(PlatformSpecificType, Spare1, Spare2);
};

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(7, 0, 0);

   typedef TrackMgmtStrengthField             FieldType1;
   typedef IntegerFieldNS<4, NoStatementZero> FieldType2;
   typedef SpareField<30>                     FieldType3;
   typedef SpareField<30>                     FieldType4;

   DEFINE_MEMBERS4(Strength, StrenghtPercentOfTrackedVehicles, Spare1, Spare2);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(7, 0);

   typedef BooleanField                       FieldType1;
   typedef ActionTrackMgmtField               FieldType2;
   typedef SpareField<1>                      FieldType3;
   typedef BooleanField                       FieldType4;
   typedef TrackNumberField                   FieldType5;
   typedef TrackStrengthField                 FieldType6;
   typedef BooleanField                       FieldType7;
   typedef IntegerFieldNS<6, NoStatementZero> FieldType8;
   typedef IntegerFieldNS<7, NoStatementZero> FieldType9;
   typedef SpareField<3>                      FieldType10;
   typedef EnvironmentField                   FieldType11;
   typedef IntegerField<4>                    FieldType12;
   typedef IdentityField                      FieldType13;
   typedef BooleanField                       FieldType14;

   DEFINE_MEMBERS14(IsExerciseIndicator,
                    ActionTrackMgmt,
                    Spare1,
                    IsControllingUnitIndicator,
                    TrackNumber,
                    Strength,
                    IsAlertStatusChange,
                    PlatformType,
                    ActivityType,
                    Spare2,
                    Environment,
                    IdentityConfidence,
                    Identity,
                    IsSpecialInterestIndicator);

   DEFINE_CONTINUATIONS1(1)
   DEFINE_EXTENSIONS1()
};

} // namespace J7_0
} // namespace Messages
} // namespace WsfL16


#endif
