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
#ifndef L16_MESSAGE12_2_HPP
#define L16_MESSAGE12_2_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J12_2
{


class VerticalControlField : public FieldBase<9>
{
public:
   double GetDegrees() { return DecodeTC((unsigned)mValue) * (22.0 / 512.0); }
   void   SetDegrees(double aDeg) { mValue = (value_type)EncodeTC((int)(aDeg / (22.0 / 512.0))); }
};

class VerticalErrorField : public FieldBase<6>
{
public:
   double GetFeet() { return DecodeTC((unsigned)mValue) * 20.0; }
   void   SetFeet(double aFt) { mValue = (value_type)EncodeTC((int)(aFt / 20.0)); }
};

class LateralErrorField : public FieldBase<7>
{
public:
   double GetFeet() { return DecodeTC((unsigned)mValue) * 15.625; }
   void   SetFeet(double aFt) { mValue = (value_type)EncodeTC((int)(aFt / 15.625)); }
};

class LateralControlField : public FieldBase<9>
{
public:
   double GetDegrees() { return DecodeTC((unsigned)mValue) * (60.0 / 512.0); }
   void   SetDegrees(double aFt) { mValue = (value_type)EncodeTC((int)(aFt / (60.0 / 512.0))); }
};

class PrecisionAircraftDirectionField : public NormalField<4, NoStatementZero>
{
public:
   enum DirectionType
   {
      cNO_STATEMENT,
      cPRECISION_RADAR_READY,
      cUNDER_PRECISION_CONTROL,
      c10_SECONDS_TO_GO,
      cNOT_UNDER_COMMAND,
      cREVERT_TO_VOICE,
      cWAVE_OFF,
      cBEACON_ON,
      cLANDING_CHECKOFF
   };
   DEFINE_ACCESSORS(PrecisionAircraftDirectionField, DirectionType);
};

typedef IntegerField<5> TimeSeconds;

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(12, 2);

   typedef TrackNumberAddresseeField       FieldType1;
   typedef VerticalControlField            FieldType2;
   typedef VerticalErrorField              FieldType3;
   typedef LateralErrorField               FieldType4;
   typedef LateralControlField             FieldType5;
   typedef PrecisionAircraftDirectionField FieldType6;
   typedef IntegerField<5>                 FieldType7;
   typedef BooleanField                    FieldType8;
   typedef BooleanField                    FieldType9;
   DEFINE_MEMBERS9(TrackNumber,
                   VerticalControl,
                   VerticalError,
                   LateralError,
                   LateralControl,
                   PrecisionDirection,
                   TimeToGoSeconds,
                   IsAutopilotControlled,
                   DoDropWeapon);

   DEFINE_CONTINUATIONS0();
   DEFINE_EXTENSIONS0();

protected:
};

} // namespace J12_2
} // namespace Messages
} // namespace WsfL16

#endif
