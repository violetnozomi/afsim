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
#ifndef L16_FIELDTYPES_HPP
#define L16_FIELDTYPES_HPP

#include "wsf_l16_export.h"

#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "MessagePacking.hpp"
#include "NumericConversion.hpp"
#include "UtCast.hpp"
#include "UtInput.hpp"

//! This file contains the base code to define fields. All of the common fields are defined in this file.
//! Some fields which are only on a single message type are defined along with those messages.

//! Each of these fields is parameterized for a specific Link16 representation, including a specific size
//! in bits.  When possible, the fields have operator=() and conversion operator defined to allow
//! conversion to and from standard WSF units (meters, seconds, etc...)

namespace WsfL16
{
namespace Messages
{
using namespace WsfL16::NumericConversion; // for brevity

class MessageBitInput
{
public:
   MessageBitInput(const char* aBuffer)
      : mBuffer(aBuffer)
      , mBitOffset(0)
   {
   }

   void Read(unsigned& aValue, int aBitCount)
   {
      Unpack(mBuffer, aValue, mBitOffset, mBitOffset + aBitCount - 1);
      mBitOffset += aBitCount;
   }
   void Read(unsigned short& aValue, int aBitCount)
   {
      unsigned int readValue;
      Unpack(mBuffer, readValue, mBitOffset, mBitOffset + aBitCount - 1);
      aValue = readValue;
      mBitOffset += aBitCount;
   }
   void Read(unsigned char& aValue, int aBitCount)
   {
      unsigned int readValue;
      Unpack(mBuffer, readValue, mBitOffset, mBitOffset + aBitCount - 1);
      aValue = readValue;
      mBitOffset += aBitCount;
   }
   void Skip(int aBitCount) { mBitOffset += aBitCount; }
   int  GetBitOffset() { return mBitOffset; }

private:
   const char* mBuffer;
   int         mBitOffset;
};

class MessageBitOutput
{
public:
   MessageBitOutput(char* aBuffer)
      : mBuffer(aBuffer)
      , mBitOffset(0)
   {
   }

   void Write(unsigned aValue, int aBitCount)
   {
      Pack(mBuffer, aValue, mBitOffset, mBitOffset + aBitCount - 1);
      mBitOffset += aBitCount;
   }
   void Skip(int aBitCount) { mBitOffset += aBitCount; }
   int  GetBitOffset() { return mBitOffset; }
   void Reset() { mBitOffset = 0; }

private:
   char* mBuffer;
   int   mBitOffset;
};

// struct AccessorUnitsNoneTag;
struct NoAccessorTypeTag;

struct NoEnumStringMapTag;
struct EnumStringMapTag;

struct NoAccessorUnitsTag;
struct AccessorUnitsMetersTag
{
   static const UtInput::ValueType cUNIT_TYPE = UtInput::cLENGTH;
   static const char*              GetName() { return "Meters"; }
};
struct AccessorUnitsMetersPerSecondTag
{
   static const UtInput::ValueType cUNIT_TYPE = UtInput::cSPEED;
   static const char*              GetName() { return "Meters Per Second"; }
};
struct AccessorUnitsCyclesPerSecondTag
{
   static const UtInput::ValueType cUNIT_TYPE = UtInput::cFREQUENCY;
   static const char*              GetName() { return "Hz"; }
};
struct AccessorUnitsRadiansTag
{
   static const UtInput::ValueType cUNIT_TYPE = UtInput::cANGLE;
   static const char*              GetName() { return "Radians"; }
};
struct AccessorUnitsKgTag
{
   static const UtInput::ValueType cUNIT_TYPE = UtInput::cMASS;
   static const char*              GetName() { return "Kg"; }
};
struct AccessorUnitsLatitudeTag
{
   static const UtInput::ValueType cUNIT_TYPE = UtInput::cLATITUDE;
   static const char*              GetName() { return "Deg"; }
};
struct AccessorUnitsLongitudeTag
{
   static const UtInput::ValueType cUNIT_TYPE = UtInput::cLONGITUDE;
   static const char*              GetName() { return "Deg"; }
};

struct accessor_unit_type;
struct FieldNoStatementInvalidTag;
struct FieldNoStatementValidTag;

//! Macro to assist in defining assignment operator and conversion operator to _TYPE_
#define DEFINE_ACCESSORS(_CLASS_, _TYPE_)  \
   typedef _TYPE_ accessor_type;           \
   _CLASS_&       operator=(_TYPE_ aValue) \
   {                                       \
      mValue = (value_type)aValue;         \
      return *this;                        \
   }                                       \
   operator _TYPE_() const { return (_TYPE_)mValue; }

#define DEFINE_ENUM_MAP()                                       \
   typedef EnumStringMapTag          enum_map_type;             \
   static const char*                GetEnumString(int aValue); \
   static void                       CreateEnumMap();           \
   static std::map<int, const char*> sEnumMap;

#ifdef USE_LARGE_INTEGERS
//! Defines integer_type to be a regular unsigned int
template<unsigned _BITS_>
struct SmallIntType
{
   typedef unsigned integer_type;
};
#else
template<unsigned _INDEX_>
struct SmallIntTypeHelper
{
};
template<>
struct SmallIntTypeHelper<0>
{
   typedef unsigned char integer_type;
};
template<>
struct SmallIntTypeHelper<1>
{
   typedef unsigned short integer_type;
};
template<>
struct SmallIntTypeHelper<2>
{
   typedef unsigned int integer_type;
};

//! Defines integer_type to be the smallest unsigned integer type possible
//! that fits _BITS_ number of bits
template<unsigned _BITS_>
struct SmallIntType
{
   typedef typename SmallIntTypeHelper<(_BITS_ > 8) + (_BITS_ > 16) + (_BITS_ > 32)>::integer_type integer_type;
};
#endif

void WSF_L16_EXPORT FormatEnumerationString(std::string& aEnumString);

//! A base class for all fields.
//! The current maximum of bits is 31
template<unsigned BIT_COUNT>
class FieldBase
{
private:
   //! constants for computing 2's compliment
   static const int HighBit      = 1 << (BIT_COUNT - 1);
   static const int LowMask      = HighBit - 1;
   static const int LowestValue  = -(LowMask + 1);
   static const int HighestValue = LowMask;

public:
   FieldBase()
      : mValue(0)
   {
   }
   typedef NoAccessorTypeTag          accessor_type;
   typedef NoAccessorUnitsTag         accessor_units;
   typedef FieldNoStatementInvalidTag no_statement_type;
   typedef NoEnumStringMapTag         enum_map_type;

   static const unsigned int cFIELD_SIZE = BIT_COUNT;
   static const unsigned int cMID_VALUE  = (1u << cFIELD_SIZE) / 2;
   static const unsigned int cMAX_VALUE  = (1u << cFIELD_SIZE) - 1;

   //! Use the smallest integer to hold the data
   typedef typename SmallIntType<BIT_COUNT>::integer_type value_type;

   friend MessageBitOutput& operator<<(MessageBitOutput& aAr, FieldBase<cFIELD_SIZE>& aField)
   {
      aAr.Write(aField.mValue, cFIELD_SIZE);
      return aAr;
   }

   friend MessageBitInput& operator<<(MessageBitInput& aAr, FieldBase<cFIELD_SIZE>& aField)
   {
      aAr.Read(aField.mValue, cFIELD_SIZE);
      return aAr;
   }

   value_type GetRawData() const { return mValue; }

   template<class T>
   value_type SafeCast(T aValue) const
   {
      return ut::safe_cast<value_type, T>(aValue);
   }

   void SetRawData(value_type aValue) { mValue = aValue; }

protected:
   //! Convert an unsigned integer to a signed integer
   //! using modified 2's compliment based on BIT_COUNT
   int DecodeTC(unsigned a2C) const
   {
      int r = a2C;
      if (a2C & HighBit)
      {
         unsigned int allBits = (unsigned int)-1;
         r                    = (int)((allBits << BIT_COUNT) | a2C);
      }
      return r;
   }
   //! Encode an integer into a N-bit two's compliment representation
   unsigned EncodeTC(int aInt) const
   {
      if (aInt < LowestValue)
      {
         aInt = LowestValue;
      }
      if (aInt > HighestValue)
      {
         aInt = HighestValue;
      }
      return static_cast<unsigned int>(aInt);
   }
   //! Encode an integer into a N-bit two's compliment representation,
   //! where cMID_VALUE is equal to No statement
   unsigned EncodeTCMod(int aInt) const
   {
      if (aInt < LowestValue)
      {
         aInt = LowestValue;
      }
      if (aInt > HighestValue - 1)
      {
         aInt = HighestValue - 1;
      }
      return static_cast<unsigned int>(aInt);
   }
   value_type mValue;
};

//! A field defined as disused
template<unsigned BIT_COUNT>
struct DisusedField : public FieldBase<BIT_COUNT>
{
};

//! A field defined as spare
template<unsigned BIT_COUNT>
struct SpareField : public FieldBase<BIT_COUNT>
{
};

//! { Most fields have a no-statement value either as 0, the midpoint value, or the max value
//!   this logic selects the no-statement value more easily
struct NoStatementMax
{
};
struct NoStatementMid
{
};
struct NoStatementZero
{
};
struct NoStatementNA
{
};

//! use a specific no-statement value
template<unsigned NS_VALUE>
struct NoStatementValue
{
   static const int value = NS_VALUE;
};

namespace Detail
{
//! compute the no-statement value based on bit-count
template<unsigned BIT_COUNT, typename NS_TAG>
struct NS_Valuator
{
   static const int value = NS_TAG::value;
};
template<unsigned BIT_COUNT>
struct NS_Valuator<BIT_COUNT, NoStatementMax>
{
   static const int value = (1u << BIT_COUNT) - 1;
};
template<unsigned BIT_COUNT>
struct NS_Valuator<BIT_COUNT, NoStatementMid>
{
   static const int value = (1u << BIT_COUNT) / 2;
};
template<unsigned BIT_COUNT>
struct NS_Valuator<BIT_COUNT, NoStatementZero>
{
   static const int value = 0;
};
} // namespace Detail

//! A field which supports a 'no-statement' value.  No-statement is the default value
//! NoStatementTag can be NoStatementMax, NoStatementMid, NoStatementZero, or NoStatementValue<VAL>
template<unsigned BIT_COUNT, typename NoStatementTag>
class NormalField : public FieldBase<BIT_COUNT>
{
   typedef typename Detail::NS_Valuator<BIT_COUNT, NoStatementTag> NS_ValuatorType;

public:
   typedef FieldNoStatementValidTag no_statement_type;
   static const unsigned int        cNO_STATEMENT_VALUE = NS_ValuatorType::value;
   NormalField() { SetNoStatement(); }
   void SetNoStatement() { FieldBase<BIT_COUNT>::mValue = (unsigned char)cNO_STATEMENT_VALUE; }
   bool IsNoStatement() const { return FieldBase<BIT_COUNT>::mValue == cNO_STATEMENT_VALUE; }
};

//! Integral field without a 'NoStatement' value.  Supports conversion to/from int.
template<unsigned BIT_COUNT>
class IntegerField : public FieldBase<BIT_COUNT>
{
public:
   typedef typename FieldBase<BIT_COUNT>::value_type value_type;
   IntegerField() { FieldBase<BIT_COUNT>::mValue = 0; }
   typedef unsigned int accessor_type;
   IntegerField&        operator=(unsigned int aValue)
   {
      FieldBase<BIT_COUNT>::mValue = (value_type)aValue;
      return *this;
   }
   operator unsigned int() const { return (unsigned int)FieldBase<BIT_COUNT>::mValue; }
};

//! Integral field with a 'NoStatement' value.  Supports conversion to/from int.
template<unsigned BIT_COUNT, class NO_STATEMENT_TAG>
class IntegerFieldNS : public NormalField<BIT_COUNT, NO_STATEMENT_TAG>
{
public:
   typedef typename NormalField<BIT_COUNT, NO_STATEMENT_TAG>::value_type value_type;
   typedef unsigned int                                                  accessor_type;
   IntegerFieldNS&                                                       operator=(unsigned int aValue)
   {
      FieldBase<BIT_COUNT>::mValue = (value_type)aValue;
      return *this;
   }
   operator unsigned int() const { return (unsigned int)FieldBase<BIT_COUNT>::mValue; }
};

class ActionField : public FieldBase<3>
{
public:
   enum ActionTypes
   {
      cDATA_UPDATE_REQUEST_BY_REQUEST_INDICATOR,
      cDATA_UPDATE_REQUEST_BY_TRACK_NUMBER,
      cBALLISTIC_MISSILE_UPDATE_REQUEST,
      cUNDEFINED
   };
   void        SetValue(ActionTypes aType) { mValue = std::min<value_type>(aType, cUNDEFINED); }
   ActionTypes GetValue() const
   {
      if (mValue > 3)
      {
         return cUNDEFINED;
      }
      else
      {
         return (ActionTypes)mValue;
      }
   }
   DEFINE_ENUM_MAP()
};

class ActionPointerField : public FieldBase<3>
{
public:
   enum ActionPointerTypes
   {
      cNO_STATEMENT,
      cWEAPONS,
      cTRACKING,
      cELECTRONIC_WARFARE,
      cSPECIAL_PROCESSING,
      cUNDEFINED
   };
   void               SetValue(ActionPointerTypes aType) { mValue = std::min<value_type>(aType, cUNDEFINED); }
   ActionPointerTypes GetValue() const
   {
      if (mValue >= 5)
      {
         return cUNDEFINED;
      }
      else
      {
         return (ActionPointerTypes)mValue;
      }
   }
   DEFINE_ENUM_MAP()
};

class ActionTrackMgmtField : public FieldBase<3>
{
public:
   enum ActionTrackMgmtTypes
   {
      cDROP_TRACK_REPORT,
      cDIFFERENT_TRACK_REPORT,
      cCHANGE_DATA_ORDER,
      cEMERGENCY_STATUS_CHANGE,
      cFORCE_TELL_STATUSC_CHANGE,
      cSTRENGTH_CHANGE,
      cEXERCISE_STATUS_ORDER,
      cUNDEFINED
   };
   void                 SetValue(ActionTrackMgmtTypes aType) { mValue = (value_type)aType; }
   ActionTrackMgmtTypes GetValue() const { return (ActionTrackMgmtTypes)mValue; }
   DEFINE_ENUM_MAP()
};

class AirPlatformActivityField : public NormalField<7, NoStatementZero>
{
public:
   enum AirPlatformActivityTypes
   {
      cNO_STATEMENT,
      cRECONNAISSANCE,
      cOVER_THE_HORIZON_TARGETING,
      cTRAINING,
      cLOGISTICS_SUPPORT,
      cANTISURFACE_WARFARE,
      cELECTRONIC_WARFARE,
      cSURVEILLANCE,
      cSEARCH_AND_RESCUE,
      cESCORTING,
      cMINELAYING,
      cTRANSITING,
      cSPECIAL_WEAPONS_ATTACK,
      cINTRUDING,
      cELECTRONIC_WARFARE_SUPPORT,
      cCOMMUNICATIONS_RELAY,
      cPATROL_OCEAN_SURVEILLANCE,
      cAIRLIFT_TRANSPORT,
      cANTISUBMARINE_WARFARE,
      cSHADOWING_INTERVENING,
      cTOWING,
      cAIR_ASSAULT,
      cINTERCEPTION,
      cELECTRONIC_ATTACK,
      cPOLICING,
      cCONVENTIONAL_ATTACK,
      cMEDICAL_EVACUATION,
      cMINE_COUNTERMEASURES,
      cSEARCH,
      cREFUELING_TANKING,
      cINTERDICTION,
      cCOMBAT_AIR_PATROL,
      cFORWARD_AIR_CONTROLLER,
      cVERY_IMPORTANT_PERSON_FLIGHT,
      cNONCOMBATANT_OPERATIONS,
      cCLOSE_AIR_SUPPORT,
      cAIRBORNE_EARLY_WARNING,
      cGROUND_ATTACK_TACTICS,
      cAIRBORNE_COMMAND_POST,
      cRESCUE_COMBAT_AIR_PATROL,
      cSURFACE_COMBAT_AIR_PATROL,
      cTANKING,
      cSPOTTING,
      cSTRIKE_WARFARE,
      cSPECIAL,
      cHIJACK,
      cJAMMER,
      cTROOPLIFT,
      cXRAY,
      cANTIAIR_WARFARE,
      cCOMMAND_AND_CONTROL,
      cCOUNTER_AIR_WARFARE,
      cRETURN_TO_BASE,
      cMINE_WARFARE,
      cCHAFF_LAYING,
      cVIDEO_DATA_LINKING,
      cDIPPING,
      cRESCUE_HELO,
      cORBITING,
      cUNDER_RECALL,
      cENGAGING,
      cENGAGING_PRIORITY_KILL,
      cINVESTIGATING,
      cCLEARED_TO_DROP,
      cINTERVENING,
      cDIVERTING,
      cAIR_TO_GROUND,
      cAIR_TO_AIR,
      cPRECISION_BOMBING,
      cLASER_DESIGNATION,
      cBEACON_BOMBING,
      cSHADOWING,
      cCOVERING,
      cVISUAL_IDENTIFICATION,
      cHIGH_ENERGY_LASING
   };
   void                     SetValue(AirPlatformActivityTypes aType) { mValue = (value_type)aType; }
   AirPlatformActivityTypes GetValue() const { return (AirPlatformActivityTypes)mValue; }
   DEFINE_ENUM_MAP()
};

class AirPlatformField : public NormalField<6, NoStatementZero>
{
public:
   enum AirPlatformTypes
   {
      cNO_STATEMENT,
      cFIGHTER,
      cFIGHTER_BOMBER,
      cATTACK,
      cBOMBER,
      cRECONNAISSANCE,
      cTANKER,
      cTANKER_BOOM_ONLY,
      cTANKER_DROGUE_ONLY,
      cINTERCEPTOR,
      cTRANSPORT,
      cAIRBORNE_COMMAND_POST,
      cMISSILE_CARRIER,
      cMISSILE,
      cELECTRONIC_WARFARE,
      cANTISUBMARINE_WARFARE,
      cAIRBORNE_EARLY_WARNING_AND_CONTROL,
      cMARITIME_PATROL_AIRCRAFT,
      cSEARCH_AND_RESCUE,
      cDRONE,
      cREMOTELY_PILOTED_VEHICLE,
      cFIXED_WING_GUNSHIP,
      cCIVIL_AIRLINER,
      cCIVIL_GENERAL,
      cLIGHTER_THAN_AIR,
      cGLIDER,
      cDECOY,
      cHELICOPTER,
      cATTACK_HELICOPTER,
      cHELICOPTER_GUNSHIP,
      cANTISUBMARINE_WARFARE_HELICOPTER,
      cMINE_WARFARE_HELICOPTER,
      cTRANSPORT_HELICOPTER,
      cTACTICAL_SUPPORT,
      cPATROL,
      cMISCELLANEOUS_FIXED_WING,
      cMISSILE_CONTROL_UNIT,
      cSURFACE_TO_AIR_MISSILE,
      cAIR_TO_SURFACE_MISSILE,
      cSURFACE_TO_SURFACE_MISSILE,
      cLOGISTIC,
      cAIR_TO_AIR_MISSILE,
      cSUBSURFACE_TO_SURFACE_MISSILE,
      cSURFACE_TO_SUBSURFACE_MISSILE,
      cCRUISE_MISSILE,
      cBALLISTIC_MISSILE,
      cAIRBORNE_LAND_SURVEILLANCE,
      cAIRBORNE_LASER
   };
   void             SetValue(AirPlatformTypes aType) { mValue = (value_type)aType; }
   AirPlatformTypes GetValue() const { return (AirPlatformTypes)mValue; }
   DEFINE_ENUM_MAP()
};

namespace Detail
{
template<unsigned BIT_COUNT>
struct AltitudeFieldConverter;
template<>
struct AltitudeFieldConverter<18>
{
   typedef FieldBase<18>::value_type value_type;
   static value_type                 Convert(double aAlt)
   {
      if (aAlt < -2000)
      {
         return (value_type)0;
      }
      else if (aAlt <= -0.5)
      {
         return (value_type)(-2000 / aAlt);
      }
      else if (aAlt <= 129070.5)
      {
         return (value_type)((int)(aAlt / 0.5) + 4001);
      }
      else
      {
         return (value_type)(262143);
      }
   }
   static double Convert(value_type aVal)
   {
      if (aVal <= 0)
      {
         return -99999.9; //! No statement; not valid
      }
      else if (aVal < 4001)
      {
         return -2000.0 / aVal;
      }
      else if (aVal < 262143)
      {
         return 0.5 * (aVal - 4001);
      }
      else
      {
         return 999999.9; //! Not valid; greater than 129,070.5 ft
      }
   }
};

template<>
struct AltitudeFieldConverter<13>
{
   typedef FieldBase<13>::value_type value_type;
   static value_type                 Convert(double aAlt)
   {
      int intAlt = (int)(aAlt / 25.0);
      if (intAlt == 63)
      {
         ++intAlt;
      }
      if (intAlt == 64)
      {
         --intAlt;
      }
      if (intAlt > (int)FieldBase<13>::cMAX_VALUE)
      {
         intAlt = (int)FieldBase<13>::cMAX_VALUE;
      }
      return (value_type)intAlt;
   }
   static double Convert(value_type aVal)
   {
      if (aVal == 63 || aVal == 64)
      {
         return -1.0;
      }
      return aVal * 25.0;
   }
};

template<>
struct AltitudeFieldConverter<12>
{
   typedef FieldBase<12>::value_type value_type;
   static value_type                 Convert(double aAlt)
   {
      if (aAlt < -2000)
      {
         return (value_type)0;
      }
      else if (aAlt <= -25)
      {
         return (value_type)(-2000 / aAlt);
      }
      else if (aAlt <= 100325)
      {
         return (value_type)((int)(aAlt / 25) + 81); //! 25 ft increments
      }
      else
      {
         return (value_type)(4095);
      }
   }
   static double Convert(value_type aVal)
   {
      if (aVal <= 0)
      {
         return -99999.9; //! No statement; not valid
      }
      else if (aVal < 81)
      {
         return -2000.0 / aVal;
      }
      else if (aVal < 4095)
      {
         return 25 * (aVal - 81);
      }
      else
      {
         return 999999.9; //! Not valid; greater than 100325 ft
      }
   }
};

template<>
struct AltitudeFieldConverter<11>
{
   typedef FieldBase<11>::value_type value_type;
   static value_type                 Convert(double aAlt) { return (value_type)(aAlt / 100.0); }
   static double                     Convert(value_type aVal) { return aVal * 100.0; }
};
} // namespace Detail

template<unsigned FIELD_SIZE>
class AltitudeField : public NormalField<FIELD_SIZE, NoStatementMax>
{
   typedef typename NormalField<FIELD_SIZE, NoStatementMax>::value_type value_type;

public:
   typedef double                 accessor_type;
   typedef AccessorUnitsMetersTag accessor_units;
   AltitudeField&                 operator=(double aVal)
   {
      SetAltitudeMeters(aVal);
      return *this;
   }
        operator double() const { return GetAltitudeMeters(); }
   void SetSkyObscured()
   {
      if (FIELD_SIZE == 13)
      {
         this->mValue = 63;
      }
   }
   bool IsSkyObscured() const
   {
      if (FIELD_SIZE == 13)
      {
         return this->mValue == 63;
      }
      return false;
   }
   void   SetUnknown() { this->mValue = this->cMAX_VALUE; }
   bool   IsUnknown() const { return this->mValue == this->cMAX_VALUE; }
   void   SetAltitudeFeet(double aAlt) { this->mValue = Detail::AltitudeFieldConverter<FIELD_SIZE>::Convert(aAlt); }
   void   SetAltitudeMeters(double aAlt) { SetAltitudeFeet(MetersToFeet(aAlt)); }
   double GetAltitudeFeet() const
   {
      if (IsUnknown())
      {
         return -1.0;
      }
      return Detail::AltitudeFieldConverter<FIELD_SIZE>::Convert(this->mValue);
   }
   double GetAltitudeMeters() const { return FeetToMeters(GetAltitudeFeet()); }
};

template<unsigned FIELD_SIZE>
class AltitudeField1 : public NormalField<FIELD_SIZE, NoStatementZero>
{
   typedef typename NormalField<FIELD_SIZE, NoStatementZero>::value_type value_type;

public:
   typedef double                 accessor_type;
   typedef AccessorUnitsMetersTag accessor_units;
   AltitudeField1&                operator=(double aVal)
   {
      SetAltitudeMeters(aVal);
      return *this;
   }
          operator double() const { return GetAltitudeMeters(); }
   void   SetAltitudeFeet(double aAlt) { this->mValue = Detail::AltitudeFieldConverter<FIELD_SIZE>::Convert(aAlt); }
   void   SetAltitudeMeters(double aAlt) { SetAltitudeFeet(MetersToFeet(aAlt)); }
   double GetAltitudeFeet() const { return Detail::AltitudeFieldConverter<FIELD_SIZE>::Convert(this->mValue); }
   double GetAltitudeMeters() const { return FeetToMeters(GetAltitudeFeet()); }
};

class AltitudeSourceField : public NormalField<2, NoStatementZero>
{
public:
   enum AltitudeSourceTypes
   {
      cNO_STATEMENT_OR_ESTIMATED,
      cSENSOR,
      cMANUAL_AIRCRAFT_REPORT,
      cAUTOMATIC_AIRCRAFT_REPORT
   };
   DEFINE_ACCESSORS(AltitudeSourceField, AltitudeSourceTypes);
   DEFINE_ENUM_MAP()
};

class WSF_L16_EXPORT AngleField : public NormalField<9, NoStatementMax>
{
public:
   bool                            IsIllegalValue() const { return (mValue < 511 && mValue >= 360); }
   void                            SetDegrees(double aAngle);
   void                            SetRadians(double aAngle) { SetDegrees(RadiansToDeg(aAngle)); }
   double                          GetDegrees() const { return (double)mValue; }
   double                          GetRadians() const { return DegToRadians(GetDegrees()); }
   typedef double                  accessor_type;
   typedef AccessorUnitsRadiansTag accessor_units;
   AngleField&                     operator=(double aVal)
   {
      SetRadians(aVal);
      return *this;
   }
   operator double() const { return GetRadians(); }
};

class WSF_L16_EXPORT AngleField16 : public NormalField<16, NoStatementMax>
{
public:
   bool                            IsIllegalValue() const { return mValue >= 35999 && mValue < cMAX_VALUE; }
   void                            SetDegrees(double aAngle);
   void                            SetRadians(double aAngle) { SetDegrees(RadiansToDeg(aAngle)); }
   double                          GetDegrees() const { return mValue / 100.0; }
   double                          GetRadians() const { return DegToRadians(GetDegrees()); }
   typedef double                  accessor_type;
   typedef AccessorUnitsRadiansTag accessor_units;
   AngleField16&                   operator=(double aVal)
   {
      SetRadians(aVal);
      return *this;
   }
   operator double() const { return GetRadians(); }
};

class AngleQualityField : public FieldBase<3>
{
public:
   void   SetQualityRadians(double aRadian);
   double GetQualityRadians() const;
};

class AreaAxisLengthField : public NormalField<6, NoStatementMax>
{
public:
   typedef double                 accessor_type;
   typedef AccessorUnitsMetersTag accessor_units;
   AreaAxisLengthField&           operator=(double aMeters)
   {
      mValue = (value_type)MetersToDataMiles(aMeters);
      return *this;
   }
   operator double() const { return DataMilesToMeters(mValue); }
};

class EllipseOrientationOrTiltField : public NormalField<11, NoStatementValue<1024>>
{
   // const XATypes::Float64Min UNCERTAINTY_ORIENTATION_LSB = 0.08789;
   // const XATypes::UInt32Min UNCERTAINTY_ORIENTATION_NEGATIVE_OFFSET = 2048;

public:
   typedef double accessor_type;

   //! Return type as radians
   typedef AccessorUnitsRadiansTag accessor_units;

   //! Packing
   EllipseOrientationOrTiltField& operator=(double aAngleRadians)
   {
      double aAngleDeg = RadiansToDeg(aAngleRadians);
      if (aAngleDeg < 0)
      {
         mValue = (value_type)((aAngleDeg / 0.08789) + 0.5 + 2048);
      }
      else
      {
         mValue = (value_type)((aAngleDeg / 0.08789) + 0.5);
      }
      return *this;
   }

   //! Unpacking
   operator double() const
   {
      if (mValue > 1024)
      {
         return DegToRadians((mValue - 2048) * 0.08789);
      }
      else if (mValue < 1024)
      {
         return DegToRadians(mValue * 0.08789);
      }
      else
      {
         return 0;
      }
   }
};

class AxisOrientationField : public NormalField<8, NoStatementMax>
{
public:
   typedef double                  accessor_type;
   typedef AccessorUnitsRadiansTag accessor_units;
   AxisOrientationField&           operator=(double aAngle)
   {
      double angle = RadiansToDeg(aAngle);
      if (angle < 0)
      {
         angle += 720;
      }
      mValue = (value_type)angle % 180;
      return *this;
   }
   operator double() const { return DegToRadians(mValue); }
};

class BearingField : public FieldBase<12>
{
public:
   typedef double                  accessor_type;
   typedef AccessorUnitsRadiansTag accessor_units;
   BearingField&                   operator=(double aVal)
   {
      mValue = (value_type)(NormalizeAngle0_360(RadiansToDeg(aVal)) * 4096.0 / 360.0);
      return *this;
   }
   operator double() const { return DegToRadians(mValue * 360.0 / 4096.0); }
};

class AngleRateField : public NormalField<9, NoStatementMax>
{
public:
   typedef double                  accessor_type;
   typedef AccessorUnitsRadiansTag accessor_units;
   AngleRateField&                 operator=(double aVal)
   {
      mValue = (value_type)((RadiansToDeg(aVal) - (-90.0)) / 0.35225039);
      return *this;
   }
   operator double() const { return DegToRadians((mValue * 0.35225039) + -90.0); }
};

class BearingInterceptField : public NormalField<16, NoStatementMax>
{
public:
   typedef double                  accessor_type;
   typedef AccessorUnitsRadiansTag accessor_units;
   BearingInterceptField&          operator=(double aAngle)
   {
      mValue = (value_type)(NormalizeAngle0_360(RadiansToDeg(aAngle)) * (65535.0 / 360.0));
      return *this;
   }
   operator double() const { return DegToRadians(mValue * (360.0 / 65535.0)); }
};

//! for fields with 1 == true
class BooleanField : public FieldBase<1>
{
public:
   BooleanField() { operator=(false); }
   bool          IsNoStatement() const { return false; }
   typedef bool  accessor_type;
                 operator bool() const { return mValue != 0; }
   BooleanField& operator=(bool aValue)
   {
      mValue = aValue ? 1U : 0U;
      return *this;
   }
};

// four character callsign, 6 bits per character
// 0      = ' '
// 1-9    = '1'-'9'
// 10-36  = 'A'-'Z'
// 63     = '0'
//(others are invalid)
class WSF_L16_EXPORT CallsignFieldBase
{
protected:
   char GetCallsignChar(int aSixBit) const;
   int  GetCharEnumeration(char aCharacter) const;
};

template<unsigned CHAR_COUNT>
class CallsignField : public FieldBase<CHAR_COUNT * 6>, public CallsignFieldBase
{
public:
   static const int cCHARACTER_COUNT = CHAR_COUNT;

   bool IsInvalidValue() const
   {
      int callsign = this->mValue;
      int bitShift = 18;
      for (int i = 0; i < cCHARACTER_COUNT; bitShift -= 6, ++i)
      {
         if ('_' == GetCallsignChar((callsign >> bitShift) & 0x3f))
         {
            return false;
         }
      }
      return true;
   }

   typedef std::string accessor_type;

   operator std::string() const
   {
      std::string lReturn;
      int         callsign = this->mValue;
      int         bitShift = 18;
      for (int i = 0; i < 4; bitShift -= 6, ++i)
      {
         lReturn += GetCallsignChar((callsign >> bitShift) & 0x3f);
      }
      return lReturn;
   }

   void operator=(const std::string aCallsign)
   {
      int callsign = 0;
      int bitShift = 18;
      for (int i = 0; i < cCHARACTER_COUNT; bitShift -= 6, ++i)
      {
         if (aCallsign.length() <= unsigned(i))
         {
            continue;
         }
         int num = GetCharEnumeration(aCallsign[i]);
         callsign += num << bitShift;
      }
      this->mValue = callsign;
   }
};

class WSF_L16_EXPORT CommandField : public FieldBase<5>
{
public:
   enum CommandTypes
   {
      cWEAPONS_FREE,
      cWEAPONS_TIGHT,
      cENGAGE,
      cASSIGN,
      cCLEAR_ENGAGE,
      cHOLD_FIRE,
      cCEASE_FIRE,
      cCOVER,
      cSALVO_CLEAR_AIRCRAFT,
      cASSUME_CONTROL,
      cATTACK,
      cCEASE_ATTACK,
      cDISUSED,
      cPROCEED_TO_POINT = 14,
      cCEASE_PROCEEDING_TO_POINT,
      cCONDUCT_PROCEDURES_INDICATED,
      cCEASE_CONDUCTING_PROCEDURES_INDICATED,
      cASSUME_DUTIES_INDICATED,
      cCEASE_DUTIES_INDICATED,
      cTRANSFER_CONTROL,
      cRETURN_TO_BASE,
      cLAUNCH_ALERT_AIRCRAFT,
      cINVESTIGATE,
      cINTERVENE,
      cSHADOW,
      cUNDEFINED,
      cENGAGE_IN_OBJECTIVE = 27,
      cPRIORITY_KILL
   };
   DEFINE_ACCESSORS(CommandField, CommandTypes);
   DEFINE_ENUM_MAP()
};

class CommImpairmentField : public NormalField<3, NoStatementZero>
{
public:
   enum CommImpairment
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
   DEFINE_ACCESSORS(CommImpairmentField, CommImpairment);
   DEFINE_ENUM_MAP()
};

class ConfidenceField : public NormalField<5, NoStatementZero>
{
public:
   enum Confidence
   {
      cNO_STATEMENT,
      cUNCLASSIFIED,
      cPOSSIBLE_SUBMARINE_LOW_ONE,
      cPOSSIBLE_SUBMARINE_LOW_TWO,
      cPOSSIBLE_SUBMARINE_HIGH_THREE,
      cPOSSIBLE_SUBMARINE_HIGH_FOUR,
      cPROBABLE_SUBMARINE,
      cCERTAIN_SUBMARINE,
      cNON_SUBMARINE,
      cSURFACE_VESSEL
   };
   DEFINE_ACCESSORS(ConfidenceField, Confidence);
   DEFINE_ENUM_MAP()
};

typedef IntegerFieldNS<7, NoStatementMax> ControlChannelField;

class DeltaLatLonField : public FieldBase<14>
{
public:
   typedef double    accessor_type;
   DeltaLatLonField& operator=(double aDegrees)
   {
      EncodeTC((int)(aDegrees * 60.0 * .0313));
      return *this;
   }
   operator double() const { return DecodeTC(mValue) / 60.0 / .0313; }
};

class DeltaElevationField : public NormalField<15, NoStatementMid>
{
public:
   typedef AccessorUnitsMetersTag accessor_units;
   typedef double                 accessor_type;
   DeltaElevationField&           operator=(double aMeters)
   {
      SetFeet(MetersToFeet(aMeters));
      return *this;
   }
        operator double() const { return FeetToMeters(GetFeet()); }
   void SetFeet(double aFeet)
   {
      int ft = (int)aFeet;
      mValue = (ft >= 0) ? ft : (32768 + ft);
   }
   double GetFeet() const { return (double)((mValue < cMID_VALUE) ? mValue : -32768 + mValue); }
};

class DepthCategoryField : public NormalField<4, NoStatementZero>
{
public:
   enum DepthCategory
   {
      cNO_STATEMENT,
      cSURFACED,
      cESTIMATED_SHALLOW,
      cPERISCOPE_DEPTH,
      cABOVE_FIRST_LAYER,
      cBELOW_FIRST_LAYER,
      cBETWEEN_FIRST_AND_SECOND_LAYERS,
      cBETWEEN_SECOND_AND_THIRD_LAYERS,
      cBELOW_BOTTOM_LAYER,
      cESTIMATED_DEEP,
      cBOTTOMED,
      cUNKNOWN = 15
   };
   DEFINE_ACCESSORS(DepthCategoryField, DepthCategory);
   DEFINE_ENUM_MAP()
};

class DepthField : public NormalField<7, NoStatementMax>
{
public:
   typedef double                 accessor_type;
   typedef AccessorUnitsMetersTag accessor_units;
   DepthField&                    operator=(double aDepth)
   {
      mValue = std::min<value_type>((value_type)(cMAX_VALUE - 1), (value_type)(aDepth / 15));
      return *this;
   }
   operator double() const { return mValue * 15.0; }
};

template<int BIT_COUNT>
class ElevationField : public NormalField<BIT_COUNT, NoStatementMax>
{
public:
   typedef double                 accessor_type;
   typedef AccessorUnitsMetersTag accessor_units;
   ElevationField&                operator=(double aMeters)
   {
      SetMeters(aMeters);
      return *this;
   }
          operator double() const { return GetMeters(); }
   void   SetMeters(double aMeters) { SetFeet(MetersToFeet(aMeters)); }
   double GetMeters() const { return FeetToMeters(GetFeet()); }
   void   SetFeet(double aFt) { this->mValue = (int)(aFt / 25); }
   double GetFeet() const { return this->mValue * 25; }
};

int    ElevationAngleField_ConvertToValue(double aValue, unsigned aMax, unsigned aMid);
double ElevationAngleField_ExtractDegrees(unsigned aValue, unsigned aMid);

class ElevationAngle10Field : public NormalField<10, NoStatementMid>
{
public:
   typedef double                  accessor_type;
   typedef AccessorUnitsRadiansTag accessor_units;
                                   operator double() const { return DecodeTC(mValue) * 3.1415926 / 1024.0; }
   ElevationAngle10Field&          operator=(double aVal)
   {
      mValue = EncodeTC((int)(aVal / 3.1415926 * 1024.0));
      return *this;
   }
};

// template <unsigned BIT_COUNT>
class ElevationAngleField : public NormalField<15, NoStatementMid>
{
public:
   typedef double                  accessor_type;
   typedef AccessorUnitsRadiansTag accessor_units;
   ElevationAngleField&            operator=(double aRadians)
   {
      SetRadians(aRadians);
      return *this;
   }
   operator double() const { return GetRadians(); }

   double GetRadians() const { return DegToRadians(GetDegrees()); }
   void   SetRadians(double aAngle) { SetDegrees(RadiansToDeg(aAngle)); }
   // double GetDegrees() const { return ElevationAngleField_ExtractDegrees(mValue, cMID_VALUE); }
   double GetDegrees() const { return DecodeTC((unsigned)mValue) * (0.0054931640625); }
   // void SetDegrees(double aAngle) { mValue = ElevationAngleField_ConvertToValue(aAngle, cMAX_VALUE, cMID_VALUE); }
   void SetDegrees(double aDeg) { mValue = (value_type)EncodeTC((int)(aDeg / (0.0054931640625))); }
};

class EmitterStatusField : public NormalField<3, NoStatementZero>
{
public:
   enum StatusTypes
   {
      cNO_STATEMENT,
      cINACTIVE,
      cTARGET_EMITTER_ACTIVE,
      cTARGET_EMITTER_IN_SEARCH,
      cTARGET_EMITTER_IN_TRACK,
      cMISSILE_LAUNCH
   };
   DEFINE_ACCESSORS(EmitterStatusField, StatusTypes);
   DEFINE_ENUM_MAP()
};

class WSF_L16_EXPORT EnvironmentField : public NormalField<3, NoStatementZero>
{
public:
   enum EnvironmentTypes
   {
      cNO_STATEMENT,
      cSPACE,
      cAIR,
      cSURFACE,
      cSUBSURFACE,
      cLAND,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(EnvironmentField, EnvironmentTypes);
   DEFINE_ENUM_MAP()
};

class EngagementActionField : public FieldBase<4>
{
public:
   enum WeaponTypes
   {
      cINTEND_TO_ENGAGE,
      cDO_NOT_INTEND_TO_ENGAGE,
      cAVAILABLE_FOR_SUPPORT,
      cNOT_AVAILABLE_FOR_SUPPORT,
      cSUPPORT_EXPECTED,
      cSUPPORT_NOT_EXPECTED,
      cUNDEFINED
   };

   void SetValue(WeaponTypes aType)
   {
      if (aType > cUNDEFINED)
      {
         mValue = cUNDEFINED;
      }
      else
      {
         mValue = (value_type)aType;
      }
   }
   WeaponTypes GetValue() const
   {
      if (mValue > 5)
      {
         return cUNDEFINED;
      }
      else
      {
         return (WeaponTypes)mValue;
      }
   }

   DEFINE_ENUM_MAP()
};

class EquipmentOperationalField : public NormalField<2, NoStatementZero>
{
public:
   enum EquipmentOperationalStates
   {
      cNO_STATEMENT,
      cOPERATIONAL,
      cNON_OPERATIONAL,
      cOFF
   };
   DEFINE_ACCESSORS(EquipmentOperationalField, EquipmentOperationalStates);
   DEFINE_ENUM_MAP()
};

class WSF_L16_EXPORT FireMethodField : public NormalField<3, NoStatementZero>
{
public:
   enum FireMethods
   {
      cNO_STATEMENT,
      cSINGLE_SHOT,
      cDUAL_SHOT,
      cMULTIPLE_SHOT,
      cSHOOT_LOOK_SHOOT
   };
   DEFINE_ACCESSORS(FireMethodField, FireMethods)

   bool IsUndefined() const { return mValue > cSHOOT_LOOK_SHOOT; }
   DEFINE_ENUM_MAP()
};

class FreqOrRangeField : public FieldBase<1>
{
public:
   bool IsFreq() const { return mValue == 0; }
   bool IsRange() const { return mValue == 1; }
   void SetFreqFlag() { mValue = 0; }
   void SetRangeFlag() { mValue = 1; }
};

class FixOrBearingField : public NormalField<3, NoStatementZero>
{
public:
   enum Type
   {
      cEW_FIX,
      cAREA_OF_PROBABILITY,
      cBEARING_TYPE_NOT_SPECIFIED,
      cBEARING_ES,
      cBEARING_EA,
      cBEARING_RADIO_DIRECTION_FINDING,
      cBEARING_UNKNOWN = 7
   };
   DEFINE_ACCESSORS(FixOrBearingField, Type);
   DEFINE_ENUM_MAP()
};

class FrequencyField : public NormalField<20, NoStatementZero>
{
public:
   bool                                    IsNoStatement() const { return mValue == 0; }
   typedef double                          accessor_type;
   typedef AccessorUnitsCyclesPerSecondTag accessor_units;
   FrequencyField&                         operator=(double aFreq)
   {
      double val = aFreq;
      while (val >= 1048575.0)
      {
         val = val / 10;
      }
      int oval = (int)val;
      mValue   = oval;
      return *this;
   }
   operator double() const { return (double)mValue; }
};

class Frequency13Field : public NormalField<13, NoStatementZero>
{
public:
   bool IsIllegal() const { return (mValue >= 7001 && mValue <= 7039) || (mValue >= 7296 && mValue <= 8198); }

   void SetUHF_Frequency(double aFreq) { mValue = (value_type)((aFreq - 225.0) / .025 + 1.0); }

   bool   IsUHF_Frequency() const { return mValue > 0 && mValue <= 7000; }
   double GetUHF_Frequency() const { return 225.0 + .025 * (mValue - 1); }

   void SetChannelA(int aChannelNo) { mValue = (value_type)(7040 + aChannelNo); }
   void SetChannelB(int aChannelNo) { mValue = (value_type)(7168 + aChannelNo); }

   bool IsChannelGroupA() const { return (mValue >= 7040) && (mValue <= 7040 + 127); }
   int  GetChannelA() const { return mValue - 7040; }

   bool IsChannelGroupB() const { return (mValue >= 7168) && (mValue <= 7168 + 127); }
   int  GetChannelB() const { return mValue - 7168; }
};

class FuelField : public NormalField<12, NoStatementMax>
{
public:
   typedef double accessor_type;
   FuelField&     operator=(double aKg)
   {
      SetFuelKilograms(aKg);
      return *this;
   }
                              operator double() const { return GetFuelKilograms(); }
   typedef AccessorUnitsKgTag accessor_units;
   void                       SetFuelPounds(double aFuelPounds)
   {
      mValue = (value_type)(aFuelPounds / 100);
      if (mValue >= cMAX_VALUE)
      {
         mValue = (value_type)(cMAX_VALUE - 1);
      }
   }
   double GetFuelPounds() const { return mValue * 100.0; }
   void   SetFuelKilograms(double aFuelKG) { SetFuelPounds(KilogramsToPounds(aFuelKG)); }
   double GetFuelKilograms() const { return PoundsToKilograms(GetFuelPounds()); }
};

class HourField : public NormalField<5, NoStatementMax>
{
public:
   typedef int accessor_type;
   HourField&  operator=(int aHour)
   {
      SetHour(aHour);
      return *this;
   }
        operator int() const { return GetHour(); }
   int  GetHour() const { return mValue; }
   void SetHour(int aHour) { mValue = SafeCast(aHour); }
   void SetHourToNow();
   bool IsIllegalValue() const { return (mValue < 31 && mValue > 23); }
};

class HundredthsField : public NormalField<7, NoStatementMax>
{
public:
   typedef int      accessor_type;
   HundredthsField& operator=(int aVal)
   {
      mValue = aVal;
      return *this;
   }
   operator int() const
   {
      if (mValue <= 99)
      {
         return mValue;
      }
      return 0;
   }
};

class IdentityField : public FieldBase<3>
{
public:
   enum IdentityTypes
   {
      cPENDING,
      cUNKNOWN,
      cASSUMED_FRIEND,
      cFRIEND,
      cNEUTRAL,
      cSUSPECT,
      cHOSTILE,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(IdentityField, IdentityTypes);
   DEFINE_ENUM_MAP()
};

class IdentityAmplifyField : public FieldBase<3>
{
public:
   enum IdentityTypes
   {
      cEXERCISE_PENDING,
      cEXERCISE_UNKNOWN,
      cEXERCISE_ASSUMED_FRIEND,
      cEXERCISE_FRIEND,
      cEXERCISE_NEUTRAL,
      cJOKER,
      cFAKER
   };
   DEFINE_ACCESSORS(IdentityAmplifyField, IdentityTypes);
   DEFINE_ENUM_MAP()
};

//! for fields with 0 == true
class InverseBooleanField : public FieldBase<1>
{
public:
   InverseBooleanField() { operator=(false); }
                        operator bool() const { return mValue == 0; }
   InverseBooleanField& operator=(bool aTf)
   {
      mValue = aTf;
      return *this;
   }
};

class JitterField : public NormalField<2, NoStatementZero>
{
public:
   enum JitterTypes
   {
      cNO_STATEMENT,
      cNO_JITTER,
      cJITTER_PRESENT,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(JitterField, JitterTypes)
   DEFINE_ENUM_MAP()
};

template<unsigned BIT_COUNT>
class LatitudeField : public NormalField<BIT_COUNT, NoStatementMid>
{
public:
   typedef double                   accessor_type;
   typedef AccessorUnitsLatitudeTag accessor_units;
   LatitudeField&                   operator=(double aLatitude)
   {
      int maxInt           = this->cMAX_VALUE;
      int numBitsMinusOne  = this->cFIELD_SIZE - 1;
      int mymidInt         = (1 << numBitsMinusOne);
      int mymidIntMinusOne = mymidInt - 1;
      if (aLatitude > 0.0f)
      {
         this->mValue = (int)(aLatitude / 90.0 * mymidIntMinusOne);
      }
      else if (aLatitude < 0.0f)
      {
         this->mValue = maxInt + (int)(aLatitude / 90.0 * mymidIntMinusOne);
      }
      else
      {
         this->mValue = 0;
      }
      return *this;
   }

   operator double() const
   {
      double rlat;
      int    midIntMinusOne = this->cMID_VALUE - 1;
      if (this->mValue < this->cMID_VALUE)
      {
         rlat = 90.0 * ((double)this->mValue) / midIntMinusOne;
         return rlat;
      }
      else if (this->mValue > this->cMID_VALUE)
      {
         rlat = -90.0 * ((double)(this->cMAX_VALUE - this->mValue)) / midIntMinusOne;
         return rlat;
      }
      return 90.0;
   }
};

class LengthField : public NormalField<15, NoStatementMax>
{
public:
   typedef double                 accessor_type;
   typedef AccessorUnitsMetersTag accessor_units;
   LengthField&                   operator=(double aMeters)
   {
      SetMeters(aMeters);
      return *this;
   }
          operator double() const { return GetMeters(); }
   void   SetMeters(double aMeters) { SetFeet(MetersToFeet(aMeters)); }
   double GetMeters() const { return FeetToMeters(GetFeet()); }
   void   SetFeet(double aFeet) { mValue = (aFeet > this->cMAX_VALUE - 1) ? this->cMAX_VALUE - 1 : (int)aFeet; }
   double GetFeet() const { return (double)this->mValue; }
};

class LetterField : public NormalField<5, NoStatementZero>
{
public:
   void SetLetter(char aChar);
   void SetLetter(int aInt);
   char GetLetter() const { return mValue + 'A' - 1; }
   bool IsUndefined() const { return mValue > 26; }
};

class LetterNumberField : public NormalField<6, NoStatementZero>
{
public:
   bool               IsUndefined() { return (mValue > 35); }
   LetterNumberField& operator=(char aChar);
                      operator char() const;
};

class LinkStatusField : public NormalField<3, NoStatementZero>
{
public:
   enum LinkStatusTypes
   {
      cNO_STATEMENT,
      cAVAILABLE_VIA_LOCAL_BUFFER,
      cAVAILABLE_VIA_REMOTE_BUFFER,
      cAVAILABLE_VIA_LOCAL_AND_REMOTE_BUFFER,
      cDEGRADED,
      cNOT_AVAILABLE,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(LinkStatusField, LinkStatusTypes);
   DEFINE_ENUM_MAP()
};

class LockOnField : public NormalField<2, NoStatementZero>
{
public:
   enum LockOnTypes
   {
      cNO_STATEMENT,
      cLOCK_ON,
      cSPY,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(LockOnField, LockOnTypes)
   DEFINE_ENUM_MAP()
};

template<unsigned BIT_COUNT>
class LongitudeField : public NormalField<BIT_COUNT, NoStatementMid>
{
public:
   typedef double                    accessor_type;
   typedef AccessorUnitsLongitudeTag accessor_units;
   LongitudeField&                   operator=(double aLongitude)
   {
      int maxInt           = this->cMAX_VALUE;
      int numBitsMinusOne  = this->cFIELD_SIZE - 1;
      int mymidInt         = (1 << numBitsMinusOne);
      int mymidIntMinusOne = mymidInt - 1;
      if (aLongitude >= 0.0f)
      {
         this->mValue = (int)(aLongitude / 180.0 * mymidIntMinusOne);
      }
      else if (aLongitude < 0.0f)
      {
         this->mValue = maxInt + (int)(aLongitude / 180.0 * mymidIntMinusOne);
      }
      return *this;
   }

   operator double() const
   {
      int range = this->cMID_VALUE - 1;
      if (this->mValue < this->cMID_VALUE)
      {
         return (double)this->mValue / range * 180.0;
      }
      else if (this->mValue > this->cMID_VALUE)
      {
         return -(double)(this->cMAX_VALUE - this->mValue) / range * 180.0;
      }
      return 0.0;
   }
};

class MinuteField : public NormalField<6, NoStatementMax>
{
public:
   typedef int  accessor_type;
                operator int() { return mValue; }
   MinuteField& operator=(int aMinute)
   {
      mValue = SafeCast(aMinute);
      return *this;
   }
   void SetMinuteToNow();
   bool IsIllegalValue() const;
};

class MissileTypeField : public NormalField<6, NoStatementZero>
{
public:
   enum MissileType
   {
      cNO_STATEMENT,
      cSHRT_RNG_SAM,
      cMED_RNG_SAM,
      cLONG_RNG_SAM,
      cSHRT_RNG_SSM,
      cMED_RNG_SSM,
      cLONG_RNG_SSM,
      cSM2_ER = 8,
      cSM2_MR,
      cSM1_ER,
      cSM1_MR,
      cEXT_RNG_SAM_SHRT,
      cEXT_RNG_SAM_MED,
      cEXT_RNG_SAM_LONG,
      cEXT_RNG_SSM_SHRT,
      cEXT_RNG_SSM_MED,
      cEXT_RNG_SSM_LONG,
      cSM2_BLOCK_II,
      cSM2_BLOCK_III,
      cSM2_BLOCK_IIIA,
      cSM2_BLOCK_IIIB,
      cSM2_BLOCK_IV,
      cSM2_BLOCK_IVA,
      cTOMAHAWK_TASM,
      cTOMAHAWK_TLAM_C,
      cHARPOON,
      cSM3_LEAP,
      cMISSILE_1 = 57,
      cMISSILE_2,
      cMISSILE_3,
      cMISSILE_4,
      cMISSILE_5,
      cMISSILE_6,
      cMISSILE_7
   };
   DEFINE_ACCESSORS(MissileTypeField, MissileType);
   DEFINE_ENUM_MAP()
};

class WSF_L16_EXPORT MissionTypeField : public NormalField<6, NoStatementMax>
{
public:
   enum MissionTypes
   {
      cNO_STATEMENT,
      cREFUEL,
      cORBIT,
      cRECALL,
      cRETURN_TO_BASE,
      cENGAGE,
      cPRIORITY_KILL,
      cBREAK_ENGAGEMENT,
      cINVESTIGATE_INTERROGATE,
      cCLEAR_TO_DROP,
      cCEASE_DO_NOT_DROP,
      cINTERVENE,
      cDIVERT,
      cAIR_TO_SURFACE,
      cAIR_TO_AIR,
      cSEARCH_AND_RESCUE,
      cCOMBAT_AIR_PATROL,
      cPRECISION_BOMBING,
      cLASER_DESIGNATION,
      cBEACON_BOMBING,
      cCLOSE_AIR_SUPPORT,
      cINTERDICTION,
      cAERIAL_RECONNAISSANCE,
      cESCORT,
      cSHADOW,
      cWEAPONS_FREE,
      cWEAPONS_TIGHT,
      cSALVO_CLEAR_AIRCRAFT,
      cALERT_CONDITION_WHITE,
      cALERT_CONDITION_YELLOW,
      cALERT_CONDITION_RED,
      cCOVER,
      cVISUAL_IDENTIFICATION,
      cDISUSED,
      cGO_TO_VOICE,
      cTRACK_DESIGNATION,
      cCANCEL_TRACK_DESIGNATION,
      cSENSOR_TARGET_REPORTS_ON,
      cSENSOR_TARGET_REPORTS_OFF,
      cSUPPRESSION_OF_ENEMY_AD,
      cARMED_RECONNAISSANCE,
      cATTACK,
      cCEASE_ATTACK,
      cRELATED_MISSION_DATA,
      cCOUNTER_AIR_ATTACK,
      cFIGHTER_SWEEP,
      cCEASE_FIRE,
      cHOLD_FIRE,
      cUNDEFINED,
      cCEASE_MISSION
   };
   DEFINE_ACCESSORS(MissionTypeField, MissionTypes);
   DEFINE_ENUM_MAP()
};

class Mode1CodeField : public NormalField<5, NoStatementZero>
{
public:
   typedef int     accessor_type;
   Mode1CodeField& operator=(int aValue);
                   operator int() const;
};

typedef NormalField<12, NoStatementZero> Mode2CodeField;

typedef NormalField<12, NoStatementZero> Mode3CodeField;

class WSF_L16_EXPORT Mode3InterrogationTypeField : public NormalField<2, NoStatementZero>
{
public:
   enum Mode3_InterrogationTypes
   {
      cNO_STATEMENT,
      cCORRECT,
      cINCORRECT,
      cNO_RESPONSE
   };
   DEFINE_ACCESSORS(Mode3InterrogationTypeField, Mode3_InterrogationTypes);
   DEFINE_ENUM_MAP()
};

class WSF_L16_EXPORT Mode4IndicatorField : public NormalField<2, NoStatementZero>
{
public:
   enum Mode4_IndicatorTypes
   {
      cNO_STATEMENT,
      cNO_RESPONSE,
      cINVALID_RESPONSE,
      cVALID_RESPONSE
   };
   DEFINE_ACCESSORS(Mode4IndicatorField, Mode4_IndicatorTypes);
   DEFINE_ENUM_MAP()
};

class MultiplierField : public NormalField<4, NoStatementMax>
{
public:
   void SetMultiplier(double aFreq);
   int  GetMultiplier() { return mValue; }
};

class NetNumberField : public NormalField<7, NoStatementMax>
{
public:
   DEFINE_ACCESSORS(NetNumberField, int)
};

class NetworkParticipationStatusField : public NormalField<4, NoStatementZero>
{
public:
   enum NetParticipationStatusState
   {
      cNO_STATEMENT,
      cACTIVE_NON_SPECIFIC,
      cINACTIVE_NON_SPECIFIC,
      cCONDITIONAL_RADIO_SILENCE,
      cHIGH_ERROR_RATE,
      cNO_J00_BEING_RECEIVED,
      cTACTICAL_SYSTEM_DATA_FAILURE,
      cPOLLING
   };
   bool IsInvalidValue() { return (mValue > cPOLLING); }
   DEFINE_ACCESSORS(NetworkParticipationStatusField, NetParticipationStatusState);
   DEFINE_ENUM_MAP()
};


class OperationalCapabilityField : public NormalField<2, NoStatementZero>
{
public:
   enum CapabilityStates
   {
      cNO_STATEMENT,
      cABLE_TO_PERFORM_MISSION,
      cLOW_FUEL_STATE,
      cUNABLE_TO_PERFORM_MISSION
   };
   DEFINE_ACCESSORS(OperationalCapabilityField, CapabilityStates);
   DEFINE_ENUM_MAP()
};

class OperationalImpairmentField : public NormalField<5, NoStatementZero>
{
public:
   enum ImpairmentTypes
   {
      cNO_STATEMENT,
      cSURVEILLANCE_RADAR,
      cACQUISITION_RADAR,
      cTRACKING_RADAR,
      cGUIDANCE_RADAR,
      cMULTIFUNCTION_RADAR,
      cHEIGHT_FINDER_RADAR,
      cATC_RADAR,
      cIFF_SIF_INTERROGATOR,
      cMOBILITY,
      cPRIMARY_POWER,
      cCOMMUNICATIONS,
      cES_PASSIVE_SENSOR,
      cMULTISTATIC_PASSIVE_SENSOR,
      cORDNANCE_DEPLETED,
      cLAUNCHER_FAILURE,
      cTDS_FAILURE,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(OperationalImpairmentField, ImpairmentTypes);
   DEFINE_ENUM_MAP()
};

class PeriodicReportField : public NormalField<2, NoStatementZero>
{
public:
   enum PeriodicReportTypes
   {
      cNO_STATEMENT,
      cORIGIAL_OR_CHANGED,
      cPERIODIC,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(PeriodicReportField, PeriodicReportTypes);
   DEFINE_ENUM_MAP()
};

class PointNumberField : public NormalField<2, NoStatementZero>
{
public:
   enum PointNumberTypes
   {
      cNO_STATEMENT,
      cSTART_POINT,
      cSTOP_POINT,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(PointNumberField, PointNumberTypes)
};

class PointTrackField : public FieldBase<1>
{
public:
   bool IsPoint() const { return mValue == 0; }
   bool IsTrack() const { return mValue == 1; }
   void SetPoint() { mValue = 0; }
   void SetTrack() { mValue = 1; }
};

class PointTypeField : public NormalField<4, NoStatementZero>
{
public:
   enum PointTypes
   {
      cNO_STATEMENT,
      cTARGET,
      cCLOSEST_FRIENDLY,
      cINITIAL_POINT,
      cBEACON,
      cCONTACT_POINT,
      cLASER_DESIGNATOR,
      cMARKER,
      cPOINT,
      cCENTER_POINT,
      cINTERNAL_ROUTE_POINT,
      cTARGET_DEFENSES,
      cENGAGEMENT_POINT,
      cUNDEFINED_1,
      cUNDEFINED_2,
      cORIGIN_OF_BEARING
   };
   DEFINE_ACCESSORS(PointTypeField, PointTypes)
};

class PRF_PRI_IndicatorField : public FieldBase<1>
{
public:
   bool IsPRF() const { return mValue == 0; }
   bool IsPRI() const { return mValue == 1; }
   void SetPRFflag() { mValue = 0; }
   void SetPRIflag() { mValue = 1; }
};

class PRF_TypeField : public NormalField<3, NoStatementZero>
{
public:
   enum PrfTypes
   {
      cNO_STATEMENT,
      cLOW,
      cMED,
      cHIGH,
      cMISSILE_COMPATIBLE
   };
   DEFINE_ACCESSORS(PRF_TypeField, PrfTypes);
};

class PRF_Field : public NormalField<23, NoStatementZero>
{
public:
   //! Can be either a frequency,
   typedef double                          accessor_type;
   typedef AccessorUnitsCyclesPerSecondTag accessor_units;
   PRF_Field&                              operator=(double aPrf)
   {
      SetPRF(aPrf);
      return *this;
   }
   operator double() const { return GetPRF(); }

   void   SetPRF(double aFreq) { mValue = (value_type)(aFreq * 10.0); }
   double GetPRF() const { return mValue / 10.0; }
   //! Or a 'high' value meaning anything over 131,071 hz
   void SetHigh() { mValue = cMAX_VALUE; }
   bool IsHigh() const { return mValue == (value_type)cMAX_VALUE; }
};

class PulseWidthField : public NormalField<14, NoStatementZero>
{
public:
   typedef double   accessor_type;
   PulseWidthField& operator=(double aPulseWidth)
   {
      mValue = (value_type)(aPulseWidth * 2.0);
      return *this;
   }
   operator double() const { return mValue / 2.0; }
};

typedef IntegerFieldNS<3, NoStatementZero> ProbabilityOfKillField;

class RadarSystemStatusField : public NormalField<2, NoStatementZero>
{
public:
   enum RadarStatusTypes
   {
      cNO_STATEMENT,
      cSYSTEM_OPERATIONAL,
      cSYSTEM_NOT_OPERATIONAL,
      cSYSTEM_OFF
   };
   DEFINE_ACCESSORS(RadarSystemStatusField, RadarStatusTypes);
   DEFINE_ENUM_MAP()
};

class SubsurfacePlatformActivityField : public NormalField<7, NoStatementZero>
{
public:
   enum ActivityType
   {
      cNO_STATEMENT,
      cRECONNAISSANCE,
      cOVER_THE_HORIZON_TARGETING_OTHT,
      cTRAINING,
      cDIVING,
      cANTISURFACE_WARFARE,
      cELECTRONIC_WARFARE_EW,
      cSURVEILLANCE,
      cSEARCH_AND_RESCUE_SAR,
      cESCORTING,
      cMINELAYING,
      cTRANSITING,
      cSPECIAL_WEAPONS_ATTACK,
      cSURFACING,
      cAMPHIBIOUS_WARFARE,
      cINTELLIGENCE_COLLECTING,
      cPATROL,
      cTRANSPORT,
      cANTISUBMARINE_WARFARE_ASW,
      cSHADOWING_INTERVENING,
      cBOTTOMING,
      cSPECIAL_WARFARE,
      cSTRIKE_WARFARE,
      cCLANDESTINE_OPERATIONS,
      cSNORKELING,
      cCONVENTIONAL_ATTACK,
      cMINE_COUNTERMEASURES,
      cMINE_WARFARE,
      cMARKING,
      cNONCOMBATANT_OPERATIONS,
      cDIRECT_SUPPORT,
      cVIDEO_DATA_LINKING_TARGETING,
      cRETURN_TO_BASE_RTB    = 50,
      cRESET_TO_NO_STATEMENT = 127
   };
   DEFINE_ACCESSORS(SubsurfacePlatformActivityField, ActivityType);
   DEFINE_ENUM_MAP()
};

class SubsurfacePlatformField : public NormalField<6, NoStatementZero>
{
public:
   enum PlatformType
   {
      cNO_STATEMENT,
      cSUBMARINE_PROPULSION_UNKNOWN,
      cDIESEL_ELECTRIC_SUBMARINE_GENERAL,
      cDIESEL_ELECTRIC_ATTACK_SUBMARINE,
      cDIESEL_ELECTRIC_MISSILE_SUBMARINE,
      cDIESEL_ELECTRIC_BALLISTIC_MISSILE_SUBMARINE,
      cTYPE_1_DIESEL,
      cTYPE_2_DIESEL,
      cTYPE_3_DIESEL,
      cNUCLEAR_SUBMARINE_GENERAL,
      cNUCLEAR_ATTACK_SUBMARINE,
      cNUCLEAR_MISSILE_SUBMARINE,
      cNUCLEAR_BALLISTIC_MISSILE_SUBMARINE,
      cTYPE_I_NUCLEAR,
      cTYPE_II_NUCLEAR,
      cTYPE_III_NUCLEAR,
      cTYPE_IV_NUCLEAR,
      cTYPE_V_NUCLEAR,
      cNON_SUBMARINE,
      cSURFACE_VESSEL,
      cTORPEDO,
      cMINE,
      cDECOY,
      cWRECK,
      cSEABED_PIPELINE,
      cFISH_MARINE_LIFE,
      cSWIMMER_FROGMAN,
      cKNUCKLE_WAKE,
      cATTACK_SUBMARINE,
      cCRUISE_MISSILE_LAUNCHER,
      cPINNACLE_SEAMOUNTAIN,
      cNON_MILITARY_SUBMERSIBLE,
      cTYPE_VI_NUCLEAR = 33,
      cTYPE_VII_NUCLEAR,
      cCONVENTIONAL_COMMAND_AND_CONTROL,
      cCONVENTIONAL_AUXILIARY,
      cNUCLEAR_COMMAND_AND_CONTROL,
      cMISSILE_CONTROL_UNIT  = 49,
      cRESET_TO_NO_STATEMENT = 63
   };
   DEFINE_ACCESSORS(SubsurfacePlatformField, PlatformType);
   DEFINE_ENUM_MAP()
};

class TargetDefensesField : public NormalField<3, NoStatementZero>
{
public:
   enum DefenseTypes
   {
      cNO_STATEMENT,
      cAIRCRAFT,
      cSURFACE_TO_AIR_MISSILE,
      cNO_KNOWN_DEFENSES,
      cANTIAIRCRAFT_ARTILLERY,
      cSMALL_ARMS
   };
   DEFINE_ACCESSORS(TargetDefensesField, DefenseTypes)
};

class TargetTypeField : public NormalField<5, NoStatementZero>
{
public:
   enum TargetTypes
   {
      cNO_STATEMENT,
      cBRIDGE,
      cPETROLEUM,
      _OIL,
      _AND_LUBRICANTS_POL,
      cCOMMUNICATIONS,
      cVEHICLES,
      cCONVOY,
      cSHIPPING,
      cDOCK_AREA,
      cTROOPS,
      cARMORED_VEHICLES,
      cRAIL,
      cSURFACE_TO_AIR_MISSILE_SAM,
      cANTIAIRCRAFT_ARTILLERY_AAA,
      cAIR_DEFENSES,
      _AIRCRAFT,
      cARTILLERY_MORTARS,
      cPILL_BOX_BUNKER,
      cGUN_EMPLACEMENT,
      cFORTIFICATION,
      cPOWER_PLANT,
      cINDUSTRIAL_SITE,
      cSUPPLY_DEPOT,
      cAIRPORT_FACILITIES,
      cROAD,
      cTUNNEL,
      cDAM,
      cWARSHIP_LARGE,
      cWARSHIP_MED,
      cWARSHIP_SMALL,
      cOTHER_SURFACE_MARITIME_VESSELS,
      cAIRBASE
   };
   DEFINE_ACCESSORS(TargetTypeField, TargetTypes)
};
class QualityField : public FieldBase<4>
{
public:
   QualityField& operator=(double aValue);
                 operator double() const;
};

class RadarChannelTypeField : public FieldBase<1>
{
public:
   bool IsSingleRF() { return mValue == 0; }
   void SetSingleRF() { mValue = 0; }
   bool IsAutomaticAgilityFrequency() { return mValue == 1; }
   void SetAutomaticAgilityFrequency() { mValue = 1; }
};

class RadioStatusField : public NormalField<3, NoStatementZero>
{
public:
   enum RadioStatuses
   {
      cNO_STATEMENT,
      cRADIO_ON,
      cRADIO_OFF,
      cRADIO_NON_OPERATIONAL,
      cRECEIVER_NON_OPERATIONAL,
      cTRANSMITTER_NON_OPERATIONAL
   };
   DEFINE_ACCESSORS(RadioStatusField, RadioStatuses)

   bool IsUndefined() { return mValue > cTRANSMITTER_NON_OPERATIONAL; }
};

class RadioTypeField : public NormalField<3, NoStatementZero>
{
public:
   enum RadioTypes
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
   DEFINE_ACCESSORS(RadioTypeField, RadioTypes)
};

class ReceiptComplianceField : public NormalField<5, NoStatementMax>
{
public:
   enum ReceiptComplianceTypes
   {
      cORDER_RESPONSE_REQUIRED, //! Response required
      cORDER_NO_RESPONSE,       //! Response NOT required
      cMACHINE_RECEIPT,         //! Order received without error
      cWILCO,                   //! (WILL COMPLY)
      cHAVCO,                   //! (HAVE COMPLIED)
      cORDER_MACHINE,           // Response required(MACHINE_RECEIPT or CANTPRO only)
      cCANTCO,                  //! (CANNOT COMPLY)
      cCANTPRO,                 //! (CANNOT PROCESS)
      cNO_STATEMENT = 31
   };

   DEFINE_ACCESSORS(ReceiptComplianceField, ReceiptComplianceTypes)
   bool IsCANTPRO() const { return mValue >= cCANTPRO && mValue <= 22; }
   bool IsOriginalOrder() const { return mValue <= cORDER_NO_RESPONSE || mValue == cORDER_MACHINE; }
   bool IsResponse() const { return mValue <= 22 && !IsOriginalOrder(); }
};

typedef IntegerFieldNS<4, NoStatementZero> RecurrenceRateField;

class SAM_ModeStateField : public NormalField<3, NoStatementZero>
{
public:
   enum StateType
   {
      cNO_STATEMENT,
      cSTANDBY,
      cTRANSITION_TO_OFF_LINE,
      cTRANSITION_TO_STANDBY,
      cTRANSITION_OUT_OF_STANDBY,
      cTRANSITION_TO_ROAD_MARCH,
      cREORIENTATION,
      cINITIALIZATION
   };
   DEFINE_ACCESSORS(SAM_ModeStateField, StateType);
   DEFINE_ENUM_MAP()
};

//!    Antenna
class ScanRateField : public FieldBase<10>
{
public:
   typedef double accessor_type;
   ScanRateField& operator=(double aRate)
   {
      mValue = (aRate > 1023.0) ? 1023 : (value_type)aRate;
      return *this;
   }
   operator double() const { return (double)mValue; }
};

//!    Antenna
class ScanRatePeriodField : public NormalField<2, NoStatementZero>
{
public:
   enum ScanRatePeriodFieldTypes
   {
      cNO_STATEMENT,
      cPRIMARY,
      cSCAN_PERIOD,
      cSECONDARY_SCAN_RATE
   };
   DEFINE_ACCESSORS(ScanRatePeriodField, ScanRatePeriodFieldTypes)
};

class ScanTypeField : public NormalField<5, NoStatementZero>
{
public:
   enum ScanTypeFieldTypes
   {
      cNO_STATEMENT,
      cFIXED_SCAN,
      cCIRCULAR_SCAN,
      cLOBE_SWITCHING,
      cDISUSED,
      cSECTOR_SCAN,
      cCONICAL_SCAN,
      cCOMPLEX_SCAN,
      cSPIRAL,
      cRASTER,
      cVARIABLE_RANDOM,
      cSTEADY,
      cMANUAL,
      cUNIDIRECTIONAL,
      cUNDETERMINED,
      cHORIZONTAL_SECTOR_B,
      cHORIZONTAL_SECTOR_T,
      cVERTICAL_SECTOR_C,
      cVERTICAL_SECTOR_S,
      cSECTOR_AND_CONICAL,
      cBI_DIRECTIONAL,
      cCIRCULAR_AND_CONICAL,
      cHELICAL,
      cCIRCULAR_AND_VERTICAL_SECTOR,
      cAGILE_BEAM,
      cUNI_DIRECTIONAL_SECTOR,
      cBI_DIRECTIONAL_SECTOR,
      cOTHER,
      cMONOPULSE,
      cPASSIVE_SCAN,
      cUNDEFINED,
      cALSO_UNDEFINED
   };
   DEFINE_ACCESSORS(ScanTypeField, ScanTypeFieldTypes)
};

class SecondField : public NormalField<6, NoStatementMax>
{
public:
   DEFINE_ACCESSORS(SecondField, int);
   void SetSecondToNow();
   bool IsIllegalValue() const { return mValue != cMAX_VALUE && mValue > 59; }
};

class SecureRadioField : public NormalField<2, NoStatementZero>
{
public:
   enum RadioSecurityTypes
   {
      cNO_STATEMENT,
      cSECURE,
      cNOT_SECURE,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(SecureRadioField, RadioSecurityTypes)
};

class SpeedASW_Field : public NormalField<9, NoStatementMax>
{
public:
   typedef double                          accessor_type;
   typedef AccessorUnitsMetersPerSecondTag accessor_units;
   SpeedASW_Field&                         operator=(double aMPS)
   {
      SetSpeedDataMiles(MetersToDataMiles(aMPS));
      return *this;
   }
          operator double() const { return DataMilesToMeters(GetSpeedDataMiles()); }
   void   SetSpeedDataMiles(double aSpeed) { mValue = (value_type)(aSpeed / 2.0); }
   double GetSpeedDataMiles() const { return mValue * 2.0; }
};

class SpeedField : public NormalField<11, NoStatementMax>
{
public:
   typedef double                          accessor_type;
   typedef AccessorUnitsMetersPerSecondTag accessor_units;
   SpeedField&                             operator=(double aMPS)
   {
      SetSpeedMetersPerSec(aMPS);
      return *this;
   }
          operator double() const { return GetSpeedMetersPerSec(); }
   void   SetSpeedDMPH(double aSpeed) { mValue = (value_type)((aSpeed / 2 > 2046) ? 2046 : aSpeed / 2); }
   void   SetSpeedMetersPerSec(double aSpeed) { SetSpeedDMPH(MetersPerSecToDMPH(aSpeed)); }
   double GetSpeedDMPH() const { return mValue * 2.0; }
   double GetSpeedMetersPerSec() const { return DMPHToMetersPerSec(GetSpeedDMPH()); }
};

class SiteTypeField : public NormalField<6, NoStatementZero>
{
public:
   enum SiteType
   {
      cNO_STATEMENT,
      cADOC,
      cSOC,
      cSOC_CRC,
      cCRC,
      cCRP,
      cAEGIS,
      cTACC_USAF,
      cCRC_MCE,
      cSSSB,
      cHIMAD_BDE,
      cHIMAD_BN,
      cPATRIOT_BN_ICC,
      cPATRIOT_ECS,
      cHAWK_AFP,
      cSHORAD_BDE,
      cSHORAD_BN,
      cSWC,
      cTACC_USMC,
      cTADC,
      cTAOC,
      cROCC,
      cRADIL,
      cASIT,
      cGCI,
      cMPC,
      cTOC,
      cDIVARTY,
      cNIKE,
      cGACC,
      cPLSS,
      cENSCE,
      cTGIF,
      cFACP,
      cTOC_THAAD,
      cLCS_THAAD,
      cSSI_THAAD,
      cJTAGS,
      cADTOC_BN,
      cADTOC_BDE,
      cADCP,
      cBM_DEFENSE_SITE
   };
   DEFINE_ACCESSORS(SiteTypeField, SiteType);
   DEFINE_ENUM_MAP()
};

class DisSiteField : public IntegerField<16>
{
public:
   DEFINE_ACCESSORS(DisSiteField, unsigned short int);
};

class DisApplicationField : public IntegerField<16>
{
public:
   DEFINE_ACCESSORS(DisApplicationField, unsigned short int);
};

class DisEntityIdField : public IntegerField<16>
{
public:
   DEFINE_ACCESSORS(DisEntityIdField, unsigned short int);
};

class StoresTypeField : public NormalField<8, NoStatementZero>
{
public:
   enum StoresTypes
   {
      cNO_STATEMENT,
      cAIM_7_SPARROW,
      cAIM_9_SIDEWINDER,
      cAIM_54_PHOENIX,
      cAGM_45_SHRIKE,
      cAGM_65A_MAVERICK_TV,
      cAGM_65B_MAVERICK_TV,
      cAGM_65D_MAVERICK_IR,
      cAGM_65E_MAVERICK_LASER,
      cAGM_78,
      cAGM_88_HARM,
      cAIM_120_AMRAAM,
      cBLU_27_NAPALM = 15,
      cMK_82_AIR     = 17,
      cBSU_50,
      cCBU_24,
      cCBU_49,
      cCBU_59_B_ROCKEYE_II = 22,
      cCBU_87_CEM          = 24,
      cCBU_89_GATOR,
      cCBU_90,
      cCBU_92,
      cCBU_93,
      cCBU_421A,
      cCBU_81B,
      cGBU_10_PAVEWAY_MK_84,
      cGBU_12_PAVEWAY_MK_82,
      cGBU_15V_4B,
      cGBU_22B,
      cMK_24 = 41,
      cMK_33,
      cMK_36_SEA_MINE,
      cMK_45,
      cMK_77,
      cMK_81SE,
      cMK_82_LD,
      cMK_82SE_SNAKE_EYE,
      cMK_83_GP_1000LB,
      cMK_84_LD_2000LB,
      cMK_117_750_LB,
      cROCKETS,
      cAGM_62_WALLEYE = 55,
      cAGM_62_WALLEYE_I_EO_1000LB,
      cAGM_62_WALLEYE_II_ER_DL_EO_2400LB,
      cSKY_FLASH,
      cAIM_132_ASRAAM,
      cASLAM = 62,
      cR530,
      cS530_SUPER_530F = 65,
      cASPIDE,
      cSHAFRIR        = 68,
      cAGM_12_BULLPUP = 72,
      cAGM_84_HARPOON = 74,
      cAGM_86C_D_CALCM,
      cMK_86_BOMB,
      cCBU_BL_755,
      cMW1_KB_44_TOR,
      cMW1_MKSA_TOR,
      cKORMORAN,
      cBLU_1,
      cCBU_2_C_A,
      cM_116_A1_A2,
      cFLARE_TP_400 = 85,
      cFLARE_ALE_40,
      cCHF_ALE_40_4,
      cSEA_EAGLE,
      cR550_MAGIC_I,
      cR550_MAGIC_II,
      cMICA_EM,
      cSUPER_530D_S530D,
      cBDU_33_PRACTICE_BOMB = 94,
      cBDU_48_PRACTICE_BOMB,
      cBDU_48_PRACTICE_MINE,
      cCRV_7_UNGUIDED_ROCKET,
      cMK_76,
      cMK_106_PRACTICE_BOMB,
      cBL_755,
      cCBU_78_GATOR_MINE,
      cGBU_16,
      cGBU_24,
      cGBU_27_LG_BLU_109,
      cGBU_28_LG_BLU_113,
      cLGTR,
      cJSOW_TRAINING,
      cAGM_154_JSOW_BLU_97,
      cAGM_154_JSOW_BLU_108,
      cAGM_154_JSOW_UNITARY,
      cJDAM_TRAINING,
      cGBU_31_JDAM_BLU_109,
      cGBU_31_JDAM_MK_84,
      cAGM_65F_MAVERICK_IR_MAW,
      cAGM_65G_MAVERICK_IR_MAW,
      cAGM_84E_SLAM,
      cAGM_84H_SLAM_ER,
      cATM_84_HARPOON_TRAINING,
      cTALD,
      cLUU_2,
      cMJU_27,
      cMJU_37,
      cMK_58,
      cMK_40_SEA_MINE_1000LB,
      cMK_41_SEA_MINE_2000LB,
      cMK_52_SEA_MINE_1200LB,
      cMK_55_SEA_MINE_2100LB,
      cMK_56_SEA_MINE_2200LB,
      cMK_63 = 131,
      cMK_64_SEA_MINE,
      cMK_65_SEA_MINE,
      cM129_2000LB_LEAFLET_CHAFF = 136,
      cAIM_9X_HIGH_OFF_BORESIGHT,
      cIRIS_T_AIM_SHORT_RANGE,
      cBDU_50_PRACTICE_BOMB,
      cBLU_109_2000LB_PENETRATOR,
      cMATRA_250_AG,
      cMC_1_AG,
      cAGM_130_POWERED_STANDOFF_WEAPON,
      cAGM_142_HAVE_NAP,
      cAGM_158_JASSM,
      cCBU_103_WCMD_CEM,
      cCBU_104_WCMD_GATOR,
      cCBU_105_WCMD_SFW,
      cGBU_37_GAM_5000LB,
      cMICA_IR,
      cAS30_LASER,
      cGBU_XX_PAVEWAY_II_MK_81,
      cKORMORAN_II,
      cCBU_97_SFW,
      cEGBU_27,
      cEGBU_28,
      cFIRST_UNDEFINED
   };
   DEFINE_ACCESSORS(StoresTypeField, StoresTypes);
   StoresTypeField& operator=(int aStoreType) { return operator=((StoresTypes)aStoreType); }
};

class SquareCircleField : public NormalField<2, NoStatementZero>
{
public:
   enum SquareCircleTypes
   {
      cNO_STATEMENT,
      cSQUARE_RECTANGLE,
      cCIRCLE_ELLIPTICAL,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(SquareCircleField, SquareCircleTypes)
};

class SystemStatusField : public NormalField<2, NoStatementZero>
{
public:
   enum StatusTypes
   {
      cNO_STATEMENT,
      cDEGRADED,
      cNOT_OPERATIONAL,
      cOPERATIONAL
   };
   DEFINE_ACCESSORS(SystemStatusField, StatusTypes);
   DEFINE_ENUM_MAP()
};

class TextIndicatorField : public FieldBase<2>
{
public:
   enum TextTypes
   {
      cNO_ASSOCIATED_TEXT,
      cASSOCIATED_TEXT_C2,
      cASSOCIATED_TEXT_C2_C3,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(TextIndicatorField, TextTypes)
   DEFINE_ENUM_MAP()
};

typedef LatitudeField<21> TgtLatitudeField;

typedef LongitudeField<22> TgtLongitudeField;

class ThreatEnvironmentField : public NormalField<3, NoStatementZero>
{
public:
   enum ThreatEnvironmentTypes
   {
      cNO_STATEMENT,
      cAIR,
      cSURFACE_MARITIME,
      cMINES,
      cSUBSURFACE_MARITIME,
      cLAND_GROUND,
      cSPACE,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(ThreatEnvironmentField, ThreatEnvironmentTypes)
};

class ThreatEvaluationField : public FieldBase<3>
{
public:
   enum ThreatEvaluationTypes
   {
      cPENDING,
      cUNKNOWN,
      cNON_THREAT,
      cTHREAT,
      cFRIEND,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(ThreatEvaluationField, ThreatEvaluationTypes)
};

class ThreatPostureField : public NormalField<3, NoStatementValue<4>>
{
public:
   enum PostureType
   {
      cSEARCH_SURVEILLANCE = 1,
      cREADY_TO_LAUNCH_FIRE,
      cLAUNCHED_FIRED,
      cUNKNOWN,
      cCANCEL,
      cDIRECTED_TO_ENGAGE
   };
   DEFINE_ACCESSORS(ThreatPostureField, PostureType);
};

class WSF_L16_EXPORT ThreatWarningField : public NormalField<2, NoStatementZero>
{
public:
   enum ThreatWarningTypes
   {
      cNO_STATEMENT,
      cWHITE,
      cYELLOW,
      cRED
   };
   DEFINE_ACCESSORS(ThreatWarningField, ThreatWarningTypes);
   DEFINE_ENUM_MAP()
};

class ThreatWeaponField : public NormalField<5, NoStatementZero>
{
public:
   enum WeaponTypes
   {
      cNO_STATEMENT,
      cHEAT_AAM,
      cRADAR_AAM,
      cACTIVE_AAM,
      cHARM
   };
   DEFINE_ACCESSORS(ThreatWeaponField, WeaponTypes);
};
class TimeDiscreteField : public NormalField<3, NoStatementZero>
{
public:
   enum TimeTypes
   {
      cNO_STATEMENT,
      cTIME_ON_TARGET,
      cILLUMINATION_ON,
      cINITIAL_POINT_TIME,
      cCHANGED_TIME_ON_TARGET
   };
   DEFINE_ACCESSORS(TimeDiscreteField, TimeTypes)
};

class TimeFunctionField : public NormalField<3, NoStatementZero>
{
public:
   enum TimeFunctionTypes
   {
      cNO_STATEMENT,
      cACTIVATION_TIME,
      cDEACTIVATION_TIME,
      cARRIVAL_TIME,
      cDEPARTURE_TIME,
      cTIME_POINT_ESTABLISHED,
      cNON_OP_TIME,
      cOP_TIME
   };
   DEFINE_ACCESSORS(TimeFunctionField, TimeFunctionTypes)
};

class TimeFunctionInterceptField : public NormalField<3, NoStatementZero>
{
public:
   enum TimeFunctionInterceptTypes
   {
      cNO_STATEMENT,
      cTIME_OF_PREDICTED_INTERCEPT,
      cTIME_OF_ACTUAL_INTERCEPT,
      cUNDEFINED3,
      cUNDEFINED4,
      cUNDEFINED5,
      cUNDEFINED6,
      cUNDEFINED7
   };
   DEFINE_ACCESSORS(TimeFunctionInterceptField, TimeFunctionInterceptTypes);
};

class TimeIndicatorField : public NormalField<3, NoStatementZero>
{
public:
   enum TimeIndicatorTypes
   {
      cNO_STATEMENT,
      cTIME_OF_OBSERVATION,
      cTIME_OF_EXTRAPOLATION,
      cTRACK_AGE,
      cGREATER_THAN_1_HOUR = 7
   };
   DEFINE_ACCESSORS(TimeIndicatorField, TimeIndicatorTypes);
};

class TimeSecondsField : public NormalField<8, NoStatementMax>
{
public:
   DEFINE_ACCESSORS(TimeSecondsField, int)
};

class TimeQualityField : public FieldBase<4>
{
public:
   void   SetTimeUncertainty(double aTimeNS);
   double GetTimeUncertainty() const;
   void   SetNetTimeReference() { mValue = 15; }
};

class TimeReportField : public NormalField<3, NoStatementZero>
{
public:
   enum TimeReportTypes
   {
      cNO_STATEMENT,
      cTIME_OF_REPORT,
      cARRIVE_ON_STATION = 3,
      cTIME_REMAINING_ON_STATION,
      cDEPART_STATION,
      cTIME_ON_TARGET,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(TimeReportField, TimeReportTypes);
   DEFINE_ENUM_MAP()
};

class TrackingStatusField : public NormalField<2, NoStatementZero>
{
public:
   enum TrackingStatusTypes
   {
      cNO_STATEMENT,
      cNOT_UNDER_TRACK,
      cTN_OBJECTIVE_UNDER_TRACK,
      cUNDEFINED
   };
   DEFINE_ACCESSORS(TrackingStatusField, TrackingStatusTypes);
   DEFINE_ENUM_MAP()
};

class WSF_L16_EXPORT TrackNumberField : public FieldBase<19>
{
public:
   TrackNumberField() { mValue = 0; }
   typedef std::string accessor_type;

   TrackNumberField& operator=(const std::string& aTNO)
   {
      SetTrackNumber(aTNO);
      return *this;
   }
   operator std::string() { return GetTrackNumberString(); }

   void        SetTrackNumber(const std::string aTNO);
   void        SetTrackNumber(int aTNO);
   int         GetTrackNumberInteger() const;
   static int  GetTrackNumberInteger(int aTrackNumber);
   static void GetTrackNumberString(int aTNO, std::string& aTNO_String);
   std::string GetTrackNumberString() const;
   static int  GetTrackNumberInteger(const std::string& aTNO_String);

protected:
   static int  CharToFiveBit(char aChar);
   static char FiveBitToChar(int aBits);
};

typedef IntegerFieldNS<15, NoStatementMax> TrackNumberAddresseeField;

class TrackQualityField : public FieldBase<4>
{
public:
   //! 'Data miles' = 6000 ft
   void   SetUncertaintySquareMiles(double aValue);
   double GetUncertaintySquareMiles() const;
   double GetUncertaintySquareMeters() const;
   double GetDistance95CEP() const;
};

class TrackQualityField1 : public FieldBase<4>
{
public:
   void   SetUncertaintySquareFeet(double aValue);
   double GetUncertaintySquareFeet() const;
   double GetUncertaintySquareMeters() const;
   double GetDistance95CEP() const;
};

class TrackMgmtStrengthField : public NormalField<4, NoStatementZero>
{
public:
   enum TrackMgmtStrengthTypes
   {
      cNO_STATEMENT,
      c1_VEHICLE,
      c2_VEHICLES,
      c3_VEHICLES,
      c4_THRU_5_VEHICLES,
      c6_THRU_7_VEHICLES,
      c8_THRU_10_VEHICLES,
      c11_THRU_15_VEHICLES,
      c16_THRU_20_VEHICLES,
      c21_THRU_30_VEHICLES,
      c31_THRU_40_VEHICLES,
      c41_THRU_50_VEHICLES,
      c51_TRUE_70_VEHICLES,
      c71_THRU_100_VEHICLES,
      c101_THRU_200_VEHICLES,
      cGREATER_THAN_200_VEHICLES
   };
   DEFINE_ACCESSORS(TrackMgmtStrengthField, TrackMgmtStrengthTypes);
   DEFINE_ENUM_MAP()
};
/*
class TrackMgmtStrengthPercentField : public NormalField<4, NoStatementZero>
{
   public:
      enum TrackMgmtStrengthPercentTypes
      {
         cNO_STATEMENT,               cLT_OR_EQUAL_2_PERCENT,
         cGT_2_OR_EQUAL_4_PERCENT,    cGT_4_OR_EQUAL_6_PERCENT,
         cGT_6_OR_EQUAL_8_PERCENT,    cGT_8_OR_EQUAL_10_PERCENT,
         cGT_10_OR_EQUAL_15_PERCENT,  cGT_15_OR_EQUAL_20_PERCENT,
         cGT_20_OR_EQUAL_25_PERCENT,  cGT_25_OR_EQUAL_30_PERCENT,
         cGT_30_OR_EQUAL_40_PERCENT,  cGT_40_OR_EQUAL_50_PERCENT,
         cGT_50_OR_EQUAL_60_PERCENT,  cGT_60_OR_EQUAL_70_PERCENT,
         cGT_70_OR_EQUAL_80_PERCENT,  cGT_80_PERCENT
      );
      DEFINE_ACCESSORS(TrackMgmtStrengthPercentField, TrackMgmtStrengthPercentTypes);
      DEFINE_ENUM_MAP()
};
*/
class WSF_L16_EXPORT TrackStrengthField : public NormalField<4, NoStatementZero>
{
public:
   typedef int         accessor_type;
   TrackStrengthField& operator=(int aNumUnits);
                       operator int() const;
   int                 GetStrength() const;
   bool                IsFewUnits() const { return mValue == 13; }
   bool                IsManyUnits() const { return mValue == 14; }
   bool                IsGreaterThan12() const { return mValue == 15; }
};

class TransverseMercatorGridCoordinateField : public NormalField<5, NoStatementZero>
{
public:
   typedef char                           accessor_type;
   TransverseMercatorGridCoordinateField& operator=(char aGridCoord)
   {
      mValue = aGridCoord - 'A' + 1;
      return *this;
   }
        operator char() const { return 'A' - 1 + mValue; }
   bool IsInvalid() const { return mValue == 9 || mValue == 15 || mValue > 26; }
};

class TransverseMercatorCoordinateField : public IntegerField<14>
{
public:
   typedef double                     accessor_type;
   TransverseMercatorCoordinateField& operator=(double aMeters)
   {
      mValue = (value_type)(aMeters / 10);
      return *this;
   }
        operator double() const { return mValue * 10.0; }
   bool IsInvalid() const { return mValue > 9999; }
};

class WSF_L16_EXPORT VectorField : public NormalField<6, NoStatementZero>
{
public:
   enum VectorType
   {
      cNO_STATEMENT,
      cTO_WAYPOINT,
      cALTITUDE_CHANGE_WARNING,
      cSPEED_CHANGE_WARNING,
      cCOURSE_CHANGE_WARNING,
      cCOMMAND_CONTROL,
      cREVERT_TO_VOICE = 7,
      cLANDING_CHECKLIST,
      cCOMMAND_COURSE_CHANGE,
      cCOMMAND_ALTITUDE_CHANGE,
      cCOMMAND_SPEED_CHANGE,
      cBUG_OUT_HEADING,
      cCV_RECOVERY_CASE_1,
      cCV_RECOVERY_CASE_2,
      cCV_RECOVERY_CASE_3,
      cFIELD_CONDITION_VMC,
      cFIELD_CONDITION_IMC,
      cJOIN_UP_RENDEZVOUS,
      cNOT_UNDER_COMMAND,
      cVISUAL_ID_TARGET_VECTOR = 21,
      cCOLLISION_CUT_OFF_VECTOR,
      cPURSUIT_VECTOR,
      cSTERN_VECTOR,
      cORBIT_VECTOR,
      cREFUEL_VECTOR
   };
   DEFINE_ACCESSORS(VectorField, VectorType);
   DEFINE_ENUM_MAP()
};

class WSF_L16_EXPORT WeaponEngagementStatusField : public NormalField<4, NoStatementZero>
{
public:
   enum EngagementStatusTypes
   {
      cNO_STATEMENT,
      cRECOMMEND_REATTACK,
      cWEAPON_ASSIGNED,
      cTRACKING_LOCKED_ON_READY_TO_FIRE_BIRD_AFFIRM,
      cFIRING_MISSILE_IN_FLIGHT_ENGAGING_TO_DESTROY,
      cEFFECTIVE_TARGET_DESTROYED_GRAND_SLAM,
      cPARTIALLY_EFFECTIVE,
      cNOT_EFFECTIVE,
      cENGAGEMENT_BROKEN,
      cHEADS_UP,
      cENGAGEMENT_INTERRUPTED,
      cINVESTIGATING_INTERROGATING,
      cSHADOWING,
      cINTERVENING,
      cCOVERING,
      cBATTLE_DAMAGE_ASSESSMENT_UNKNOWN
   };
   DEFINE_ACCESSORS(WeaponEngagementStatusField, EngagementStatusTypes);
   DEFINE_ENUM_MAP()
};

class WSF_L16_EXPORT WeaponSystemField : public NormalField<5, NoStatementZero>
{
public:
   enum WeaponTypes
   {
      cNO_STATEMENT,
      cSURFACE_TO_AIR_MISSILE_SAM,
      cSURFACE_TO_SURFACE_MISSILE_SSM,
      cAIRCRAFT,
      cCONVENTIONAL_SHRT_RNG_MISSILE_OR_GUNS,
      cANTISUBMARINE_WARFARE_HELICOPTER,
      cANTISUBMARINE_WARFARE_MISSILE,
      cDEPTH_CHARGE_DEPTH_BOMB,
      cTORPEDO,
      cFIXED_WING_ANTISUBMARINE_WARFARE_AIRCRAFT,
      cLASER,
      cBEAM_WEAPON,
      cINFRARED_DECOY,
      cCHAFF,
      cSPOT_JAMMER,
      cBARRAGE_JAMMER,
      cDECEPTION_JAMMER,
      cINTERCEPTOR,
      cAIR_TO_SURFACE_MISSILE_ASM
   };
   DEFINE_ACCESSORS(WeaponSystemField, WeaponTypes);
   DEFINE_ENUM_MAP()
};

class WSF_L16_EXPORT WeaponTypeField : public FieldBase<2>
{
public:
   enum WeaponTypes
   {
      cALL_WEAPON_SYSTEMS,
      cAIRCRAFT,
      cMISSILE,
      cCONVENTIONAL
   };
   DEFINE_ACCESSORS(WeaponTypeField, WeaponTypes)
   DEFINE_ENUM_MAP()
};
} // namespace Messages
} // namespace WsfL16

#endif
