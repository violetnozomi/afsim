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
#ifndef L16_MESSAGE14_0_HPP
#define L16_MESSAGE14_0_HPP
#include "MessageDefines.hpp"


namespace WsfL16
{
namespace Messages
{
namespace J14_0
{

class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(14, 0, 2)

   typedef TgtLatitudeField  FieldType1;
   typedef TgtLongitudeField FieldType2;
   typedef IntegerField<6>   FieldType3;
   typedef IntegerField<6>   FieldType4;
   typedef IntegerField<8>   FieldType5;

   DEFINE_MEMBERS5(TargetLatitude, TargetLongitude, AreaMajorAxis, AreaMinorAxis, AxisOrientation);
};

class Continuation4 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(14, 0, 4)
   typedef PRF_PRI_IndicatorField FieldType1;
   typedef PRF_Field              FieldType2;
   typedef PulseWidthField        FieldType3;
   typedef JitterField            FieldType4;
   typedef ScanTypeField          FieldType5;
   typedef ScanRatePeriodField    FieldType6;
   typedef ScanRateField          FieldType7;
   typedef IntegerField<3>        FieldType8;
   typedef IntegerField<3>        FieldType9;
   DEFINE_MEMBERS9(PRF_PRI_Indicator,
                   PulseRepetitionFrequency,
                   PulseWidthMessageSizeCheck,
                   Jitter,
                   ScanType,
                   AntennaScanRatePeriod,
                   AntennaScanRate,
                   Polarization,
                   JammerReceivedSignalLevel);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(14, 0);

   typedef ThreatEvaluationField FieldType1;
   typedef BooleanField          FieldType2;
   typedef BooleanField          FieldType3;
   typedef BooleanField          FieldType4;
   typedef BooleanField          FieldType5;
   typedef TrackNumberField      FieldType6;
   typedef BooleanField          FieldType7;
   typedef SquareCircleField     FieldType8;
   typedef SpareField<3>         FieldType9;
   typedef SecondField           FieldType10;
   typedef MinuteField           FieldType11;
   typedef HourField             FieldType12;
   typedef IntegerField<3>       FieldType13;
   typedef EnvironmentField      FieldType14;
   typedef LockOnField           FieldType15;

   DEFINE_MEMBERS15(ThreatEvaluation,
                    ResponseIndicator,
                    SpecialProcessingIndicator,
                    AgileEmitter,
                    TrackNumberReference,
                    TrackNumber,
                    BearingOrigin,
                    SquareCircleSwitch,
                    Spare,
                    Second,
                    Minute,
                    Hour,
                    RequestNumber,
                    Environment,
                    LockOn);

   DEFINE_CONTINUATIONS2(2, 4)
   DEFINE_EXTENSIONS0()

protected:
};

} // namespace J14_0
} // namespace Messages
} // namespace WsfL16

#endif
