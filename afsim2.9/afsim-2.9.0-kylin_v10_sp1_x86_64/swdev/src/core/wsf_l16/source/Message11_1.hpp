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
#ifndef L16_MESSAGE11_1_HPP
#define L16_MESSAGE11_1_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J11_1
{
// J11.1 Weapon Directive Message: I, E0, C1-C9
// C10 Created to support fast moving targets

class WeaponDiscreteField : public NormalField<6, NoStatementZero>
{
public:
   enum WeaponDiscreteType
   {
      cNO_STATEMENT,
      cEXCLUSION_ZONE_ABORT,
      cTHIRD_PARTY_COMM_ON,
      cTHIRD_PARTY_COMM_OFF,
      cTHIRD_PARTY_COMM_PING_ONLY,
      cEMCON_STATE_CHANGE_TRANSMIT_ENABLE,
      cEMCON_STATE_CHANGE_RADIO_SILENT,
      cHANDOFF_CONTROL_CHANGE_ORDER,
      cHANDOFF_CHECK_IN_RESPONSE,
      cMISSION_SUPPLEMENT,
      cRETARGET_FIXED,
      cRETARGET_MOVING_RELOCATABLE,
      cIN_FLIGHT_TARGET_UPDATE,
      cRESUME,
      cLOITER_PREPLANNED,
      cLOITER_FIXED_COORDINATES,
      cLOITER_IMMEDIATE,
      cPING,
      cQUERY_BASIC_WIFT,
      cQUERY_WIFT_SUPPLEMENT,
      cQUERY_ALL_WIFT,
      cABORT_PREPLANNED,
      cABORT_FIXED_COORDINATES,
      cABORT_IMMEDIATE
   };
   DEFINE_ACCESSORS(WeaponDiscreteField, WeaponDiscreteType);
};

class ThirdPartySourceField : public NormalField<3, NoStatementZero>
{
public:
   enum ThirdPartySourceType
   {
      cNO_STATEMENT,
      cIN_FLIGHT_TARGET_UPDATE_3PS,
      cALTERNATE_CONTROLLER,
      cUNDEFINED_1,
      cUNDEFINED_2,
      cUNDEFINED_3,
      cUNDEFINED_4,
      cUNDEFINED_5
   };
   DEFINE_ACCESSORS(ThirdPartySourceField, ThirdPartySourceType);
};

struct VelocityField8 : public NormalField<8, NoStatementMid>
{
public:
   typedef double                          accessor_type;
   typedef AccessorUnitsMetersPerSecondTag accessor_units;
   VelocityField8&                         operator=(double aVal)
   {
      mValue = (value_type)EncodeTCMod((int)MetersToFeet(aVal));
      return *this;
   }
   operator double() const { return FeetToMeters(DecodeTC(mValue)); }
};

struct VelocityField9 : public NormalField<9, NoStatementMid>
{
public:
   typedef double                          accessor_type;
   typedef AccessorUnitsMetersPerSecondTag accessor_units;
   VelocityField9&                         operator=(double aVal)
   {
      mValue = (value_type)EncodeTCMod((int)MetersToFeet(aVal));
      return *this;
   }
   operator double() const { return FeetToMeters(DecodeTC(mValue)); }
};

struct VelocityField12 : public NormalField<12, NoStatementMid>
{
public:
   typedef double                          accessor_type;
   typedef AccessorUnitsMetersPerSecondTag accessor_units;
   VelocityField12&                        operator=(double aVal)
   {
      mValue = (value_type)EncodeTCMod((int)MetersToFeet(aVal));
      return *this;
   }
   operator double() const { return FeetToMeters(DecodeTC(mValue)); }
};

struct VelocityField16 : public NormalField<16, NoStatementMid>
{
public:
   typedef double                          accessor_type;
   typedef AccessorUnitsMetersPerSecondTag accessor_units;
   VelocityField16&                        operator=(double aVal)
   {
      mValue = (value_type)EncodeTCMod((int)MetersToFeet(aVal));
      return *this;
   }
   operator double() const { return FeetToMeters(DecodeTC(mValue)); }
};

class Continuation10 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 1, 10)

   typedef VelocityField16 FieldType1; // TARGET VELOCITY NORTH
   typedef VelocityField16 FieldType2; // TARGET VELOCITY EAST
   typedef VelocityField16 FieldType3; // TARGET VELOCITY DOWN
   typedef SpareField<15>  FieldType4; //

   DEFINE_MEMBERS4(TargetVelocityNorth, TargetVelocityEast, TargetVelocityDown, Spare4);
};

class Continuation9 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 1, 9)

   typedef LatitudeField<21>  FieldType1; // LATITUDE, 0.00008 MINUTE
   typedef LongitudeField<22> FieldType2; // LONGITUDE, 0.00008 MINUTE
   typedef VelocityField12    FieldType3; // TARGET VELOCITY DOWN
   typedef SpareField<8>      FieldType4; //

   DEFINE_MEMBERS4(Latitude, Longitude, TargetVelocityDown, Spare4);
};

class Continuation5 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 1, 5)

   typedef TrackNumberField                    FieldType1; // TRACK NUMBER, TARGET
   typedef IntegerFieldNS<12, NoStatementZero> FieldType2; // ENVIRONMENT (SPECIFIC TYPE)
   typedef AngleField                          FieldType3; // IMPACT COURSE
   typedef IntegerFieldNS<7, NoStatementMax>   FieldType4; // IMPACT ELEVATION ANGLE
   typedef TrackNumberAddresseeField           FieldType5; // TRACK NUMBER, INDEX ORIGINATOR 2
   typedef SpareField<1>                       FieldType6; //

   DEFINE_MEMBERS6(TrackNumber, SpecificType, ImpactCourse, ImpactElevationAngle, TrackNumberOrig2, Spare6);
};

class Continuation3 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 1, 3)

   typedef AltitudeField1<18> FieldType1; // HEIGHT ABOVE ELLIPSOID
   typedef VelocityField9     FieldType2; // TARGET VELOCITY NORTH
   typedef VelocityField9     FieldType3; // TARGET VELOCITY EAST
   typedef IntegerField<21>   FieldType4; // TIME OF TARGET MEASUREMENT
   typedef BooleanField       FieldType5; // REPORTED TGT VEL MEASURED BY OPERATOR OR SENSOR (TRUE=SENSOR)
   typedef BooleanField       FieldType6; // IFTU SUPPLEMENT INDICATOR
   typedef SpareField<4>      FieldType7; // SPARE

   DEFINE_MEMBERS7(HeightAboveEllipsoid,
                   TargetVelocityNorth,
                   TargetVelocityEast,
                   TimeOfTgtMeasurement,
                   EstimatedMeasureIndicator,
                   IFTUSuppIndicator,
                   Spare4);
};

class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 1, 2)

   typedef LatitudeField<27>  FieldType1; // LATITUDE, 0.00008 MINUTE
   typedef LongitudeField<28> FieldType2; // LONGITUDE, 0.00008 MINUTE
   typedef VelocityField8     FieldType3; // TARGET VELOCITY DOWN

   DEFINE_MEMBERS3(Latitude, Longitude, TargetVelocityDown);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 1, 1)

   typedef ThirdPartySourceField              FieldType1; // THIRD PARTY SOURCE TYPE
   typedef IntegerFieldNS<15, NoStatementMax> FieldType2; // TRACK NUMBER, THIRD PARTY or NEW CONTROLLER
   typedef SpareField<25>                     FieldType3;
   typedef SpareField<20>                     FieldType4;

   DEFINE_MEMBERS4(ThirdPartySource, TrackNumberHandOff, Spare3, Spare4);
};
typedef IntegerFieldNS<15, NoStatementMax> TrackNumberAddresseeField;

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(11, 1, 0)

   typedef SpareField<25> FieldType1;
   typedef SpareField<25> FieldType2;
   typedef SpareField<18> FieldType3;

   DEFINE_MEMBERS3(Spare1, Spare2, Spare3);
};

typedef IntegerFieldNS<6, NoStatementMax> IndexNumberField;

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(11, 1);

   typedef TrackNumberAddresseeField FieldType1; // TRACK NUMBER, ADDRESSEE
   typedef SpareField<10>            FieldType2; //
   typedef WeaponDiscreteField       FieldType3; // WEAPON DIRECTIVE DISCRETE
   typedef BooleanField              FieldType4; // RESPONSE REQUEST
   typedef IndexNumberField          FieldType5; // INDEX NUMBER, TARGET
   typedef BooleanField              FieldType6; // FUSE MODE COMMAND
   typedef EnvironmentField          FieldType7; // ENVIRONMENT
   typedef SpareField<15>            FieldType8; //

   DEFINE_MEMBERS8(TrackNumberAddressee, Spare2, WeaponDirective, ResponseRequest, IndexNumber, FuseModeCommand, Environment, Spare8);

   DEFINE_CONTINUATIONS6(1, 2, 3, 5, 9, 10)
   DEFINE_EXTENSIONS1()

protected:
};

} // namespace J11_1
} // namespace Messages
} // namespace WsfL16

#endif
