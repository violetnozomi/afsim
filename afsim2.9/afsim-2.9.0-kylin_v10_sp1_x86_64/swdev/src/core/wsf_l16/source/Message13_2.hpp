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
#ifndef L16_MESSAGE13_2_HPP
#define L16_MESSAGE13_2_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J13_2
{
class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(13, 2, 1);
   typedef EquipmentOperationalField FieldType1;
   typedef EquipmentOperationalField FieldType2;
   typedef IntegerField<6>           FieldType3;
   typedef EquipmentOperationalField FieldType4;
   typedef EquipmentOperationalField FieldType5;
   typedef EquipmentOperationalField FieldType6;
   typedef EquipmentOperationalField FieldType7;
   typedef EquipmentOperationalField FieldType8;
   typedef RadioTypeField            FieldType9;
   typedef RadioStatusField          FieldType10;
   typedef SecureRadioField          FieldType11;
   typedef RadioTypeField            FieldType12;
   typedef RadioStatusField          FieldType13;
   typedef SecureRadioField          FieldType14;
   typedef RadioTypeField            FieldType15;
   typedef RadioStatusField          FieldType16;
   typedef SecureRadioField          FieldType17;
   typedef BooleanField              FieldType18;
   typedef InverseBooleanField       FieldType19;
   typedef LetterField               FieldType20;
   typedef IntegerField<12>          FieldType21;

   DEFINE_MEMBERS21(RadarStatus,
                    InfaredStatus,
                    MissileChannel,
                    LaserStatus,
                    TelevisionStatus,
                    RadarWarningAndReceivingStatus,
                    AirToAirWeaponsStatus,
                    AirToGroundWeaponsStatus,
                    RadioType1,
                    RadioStatus1,
                    RadioSecurity1,
                    RadioType2,
                    RadioStatus2,
                    RadioSecurity2,
                    RadioType3,
                    RadioStatus3,
                    RadioSecurity3,
                    IsAutopilot,
                    IsSensorReportingAll,
                    AircraftModel,
                    AirSpecificType);
};

class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(13, 2, 2);

   typedef BooleanField              FieldType1;
   typedef IntegerField<6>           FieldType2;
   typedef StoresTypeField           FieldType3;
   typedef IntegerField<6>           FieldType4;
   typedef StoresTypeField           FieldType5;
   typedef IntegerField<6>           FieldType6;
   typedef StoresTypeField           FieldType7;
   typedef IntegerField<6>           FieldType8;
   typedef StoresTypeField           FieldType9;
   typedef BooleanField              FieldType10;
   typedef EquipmentOperationalField FieldType11;
   typedef EquipmentOperationalField FieldType12;
   typedef SpareField<1>             FieldType13;
   DEFINE_MEMBERS13(IsSummation,
                    NumberOfStores1,
                    StoreType1,
                    NumberOfStores2,
                    StoreType2,
                    NumberOfStores3,
                    StoreType3,
                    NumberOfStores4,
                    StoreType4,
                    IsGunCapable,
                    TransponderStatus,
                    InterrogatorStatus,
                    Spare);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(13, 2);

   typedef OperationalCapabilityField FieldType1;
   typedef SpareField<2>              FieldType2;
   typedef TrackNumberField           FieldType3;
   typedef BooleanField               FieldType4;
   typedef FuelField                  FieldType5;
   typedef RadarChannelTypeField      FieldType6;
   typedef IntegerField<6>            FieldType7;
   typedef TimeReportField            FieldType8;
   typedef MinuteField                FieldType9;
   typedef HourField                  FieldType10;

   DEFINE_MEMBERS10(OperationalCapability,
                    Spare,
                    TrackNumber,
                    IsOffLoadableFuel,
                    Fuel,
                    RadarChannelType,
                    RadarChannel,
                    TimeReportFunction,
                    Minute,
                    Hour);

   DEFINE_CONTINUATIONS2(1, 2)
   DEFINE_EXTENSIONS0()
};

} // namespace J13_2
} // namespace Messages
} // namespace WsfL16


#endif
