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
#ifndef L16_MESSAGE13_5_HPP
#define L16_MESSAGE13_5_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J13_5
{

// Continuation1, Continuation2:  Not yet defined.
class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(13, 5, 1);

   typedef IntegerFieldNS<7, NoStatementMax> FieldType1;
   typedef OperationalImpairmentField        FieldType2;
   typedef SystemStatusField                 FieldType3;
   typedef SystemStatusField                 FieldType4;
   typedef LinkStatusField                   FieldType5;
   typedef SystemStatusField                 FieldType6;
   typedef LinkStatusField                   FieldType7;
   typedef SystemStatusField                 FieldType8;
   typedef SystemStatusField                 FieldType9;
   typedef CommImpairmentField               FieldType10;
   typedef IntegerFieldNS<5, NoStatementMax> FieldType11;
   typedef TimeFunctionField                 FieldType12;
   typedef MinuteField                       FieldType13;
   typedef HourField                         FieldType14;
   typedef RadarSystemStatusField            FieldType15;
   typedef RadarSystemStatusField            FieldType16;
   typedef RadarSystemStatusField            FieldType17;
   typedef RadarSystemStatusField            FieldType18;
   typedef BooleanField                      FieldType19;
   typedef BooleanField                      FieldType20;
   typedef RadarSystemStatusField            FieldType21;
   typedef SpareField<1>                     FieldType22;

   DEFINE_MEMBERS22(ColdInventory,
                    OperationalImpairment,
                    NATOLink1Status,
                    Link14Status,
                    Link11Status,
                    Link11BStatus,
                    Link16Status,
                    ATDL_1Status,
                    IJMSStatus,
                    CommunicationsImpairment,
                    ControlPositions,
                    TimeFunction,
                    Minute,
                    Hour,
                    PrimarySurvRadarStatus,
                    SecondarySurvRadarStatus,
                    TertiarySurvRadarStatus,
                    AcquistionRadarStatus,
                    IlluminatingRadarStatus,
                    Mode4InterrogatorStatus,
                    IFFInterrogatorStatus,
                    Spare);
};

/*
// Continuation1, Continuation2:  Not yet defined.
class Continuation2: public ContinuationBase
{
   public:
      DEFINE_CONTINUATION(13,5,2);
      typedef EquipmentStatusField       FieldType1;
      typedef EquipmentStatusField       FieldType2;
      typedef EquipmentStatusField       FieldType3;
      typedef EquipmentStatusField       FieldType4;
      typedef EquipmentStatusField       FieldType5;
      typedef EquipmentStatusField       FieldType6;
      typedef EquipmentStatusField       FieldType7;
      typedef EquipmentStatusField       FieldType8;
      typedef EquipmentStatusField       FieldType9;
      typedef EquipmentStatusField       FieldType10;
      typedef EquipmentStatusField       FieldType11;
      typedef EquipmentStatusField       FieldType12;
      typedef EquipmentStatusField       FieldType13;
      typedef EquipmentStatusField       FieldType14;
      typedef EquipmentStatusField       FieldType15;
      typedef EquipmentStatusField       FieldType16;
      typedef EquipmentStatusField       FieldType17;
      typedef EquipmentStatusField       FieldType18;
      typedef EquipmentStatusField       FieldType19;
      typedef EquipmentStatusField       FieldType20;
      typedef EquipmentStatusField       FieldType21;
      typedef EquipmentStatusField       FieldType22;
      typedef EquipmentStatusField       FieldType23;
      typedef EquipmentStatusField       FieldType24;
      typedef EquipmentStatusField       FieldType25;
      typedef EquipmentStatusField       FieldType26;
      typedef EquipmentStatusField       FieldType27;
      typedef EquipmentStatusField       FieldType28;
      typedef EquipmentStatusField       FieldType29;
      typedef EquipmentStatusField       FieldType30;
      typedef EquipmentStatusField       FieldType31;
      typedef SpareField<1>              FieldType32;

      DEFINE_MEMBERS32(ESFreqBandA, ESFreqBandB, ESFreqBandC, ESFreqBandD, ESFreqBandE, ESFreqBandF,
                       ESFreqBandG, ESFreqBandH, ESFreqBandI, ESFreqBandJ, ESFreqBandK, ESFreqBandL,
                       ESFreqBandM, HFDirectionFinder, VHFDirectionFinder, UHFDirectionFinder,
                       EAFreqBandA, EAFreqBandB, EAFreqBandC, EAFreqBandD, EAFreqBandE, EAFreqBandF,
                       EAFreqBandG, EAFreqBandH, EAFreqBandI, EAFreqBandJ, EAFreqBandK, EAFreqBandL,
                       EAFreqBandM, DecoyChaff,  DecoyIR,     Spare2);
};
*/

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(13, 5);

   typedef OperationalCapabilityField        FieldType1;
   typedef TrackNumberField                  FieldType2;
   typedef SiteTypeField                     FieldType3;
   typedef IntegerFieldNS<7, NoStatementMax> FieldType4;
   typedef SAM_ModeStateField                FieldType5;
   typedef TimeFunctionField                 FieldType6;
   typedef MinuteField                       FieldType7;
   typedef HourField                         FieldType8;
   typedef BooleanField                      FieldType9;
   typedef SpareField<5>                     FieldType10;

   DEFINE_MEMBERS10(OperationalCapability,
                    TrackNumberReference,
                    SiteType,
                    HotMissileCount,
                    SamModeState,
                    TimeFunction,
                    Minute,
                    Hour,
                    PerimeterEngagementStatus,
                    Spare);

   DEFINE_CONTINUATIONS1(1);
   DEFINE_EXTENSIONS0();
};

} // namespace J13_5
} // namespace Messages
} // namespace WsfL16


#endif
