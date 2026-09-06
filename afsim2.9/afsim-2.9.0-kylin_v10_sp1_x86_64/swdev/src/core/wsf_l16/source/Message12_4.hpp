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
#ifndef L16_MESSAGE12_4_HPP
#define L16_MESSAGE12_4_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J12_4
{

enum RadioType
{
   cNO_STATEMENT,
   cUHF_AM,
   cVHF_AM,
   cVHF_FM,
   cHF_SSB_UPPER,
   cHF,
   cUHF_FM,
   cHF_SSB_LOWER
};

class RadioTypeField : public FieldBase<3>
{
public:
   enum ControlChangeType
   {
      cCONTROL_CHANGE_ORDER,
      cCONTROL_REQUEST,
      cTERMINATE_CONTROL
   };
   DEFINE_ACCESSORS(RadioTypeField, RadioType);
};

class ControlChangeField : public FieldBase<2>
{
public:
   enum ControlChangeType
   {
      cCONTROL_CHANGE_ORDER,
      cCONTROL_REQUEST,
      cTERMINATE_CONTROL
   };
   DEFINE_ACCESSORS(ControlChangeField, ControlChangeType);
};

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(12, 4, 0);

   typedef SpareField<4>                       FieldType1;
   typedef BooleanField                        FieldType2;
   typedef CallsignField<4>                    FieldType3;
   typedef Frequency13Field                    FieldType4;
   typedef IntegerFieldNS<12, NoStatementZero> FieldType5;
   typedef BooleanField                        FieldType6;
   typedef Frequency13Field                    FieldType7;
   DEFINE_MEMBERS7(Spare, IsAlternateVoiceChannel, VoiceCallsign, Link4aFrequency, Mode3Code, SquawkIndicator, VoiceFrequency);
};


class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(12, 4);

   typedef TrackNumberAddresseeField         FieldType1;
   typedef TrackNumberField                  FieldType2;
   typedef ControlChangeField                FieldType3;
   typedef IntegerFieldNS<7, NoStatementMax> FieldType4;
   typedef RadioTypeField                    FieldType5;
   typedef BooleanField                      FieldType6;
   typedef SpareField<1>                     FieldType7;
   typedef ReceiptComplianceField            FieldType8;
   typedef RecurrenceRateField               FieldType9;
   DEFINE_MEMBERS9(TrackNumberAddressee,
                   TrackNumber,
                   ControlChange,
                   ControlChannel,
                   RadioType,
                   IsSecureRadio,
                   Spare,
                   ReceiptCompliance,
                   RecurrenceRate);

   DEFINE_CONTINUATIONS0()
   DEFINE_EXTENSIONS1()

protected:
};

} // namespace J12_4
} // namespace Messages
} // namespace WsfL16

#endif
