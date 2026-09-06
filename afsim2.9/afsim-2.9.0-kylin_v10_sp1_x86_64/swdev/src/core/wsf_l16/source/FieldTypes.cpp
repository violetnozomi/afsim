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
#include "FieldTypes.hpp"

#include <cmath>
#include <cstdio>
#include <ctime>
#include <stdlib.h>

#include "Exception.hpp"
#include "FieldEnumDefines.hpp"
#include "UtMath.hpp"

namespace WsfL16
{
namespace Messages
{

void FormatEnumerationString(std::string& aEnum)
{
   if (aEnum.length() > 0)
   {
      if (aEnum[0] == 'c')
      {
         aEnum = aEnum.substr(1);
      }

      for (unsigned i = 0; i < aEnum.length(); ++i)
      {
         if (aEnum[i] == '_')
         {
            aEnum[i] = ' ';
         }
      }
   }
}

} // namespace Messages
} // namespace WsfL16

namespace // unnamed
{
const double SquareMilesQualityEnumeration[15] =
   {2755., 686., 439., 247, 110, 27, 4.4, 1.1, .0281, .007, .0018, .0004, .0001, .00003};
const double SquareFeetTrackQualityEnumeration1[16] = {FLT_MAX,
                                                       FLT_MAX,
                                                       2.166332e10,
                                                       6.665636e9,
                                                       2.050965e9,
                                                       631066095.0,
                                                       194174183.0,
                                                       59745902.0,
                                                       18383355.0,
                                                       5656417.0,
                                                       1740436.0,
                                                       535519.0,
                                                       164775.0,
                                                       50700.0,
                                                       15600.0,
                                                       4800.0};
const int FourBitQualityEnumeration[15] = {18080, 9040, 4520, 2260, 1600, 1130, 800, 565, 400, 282, 200, 141, 100, 71, 50};
} // namespace

namespace WsfL16
{
namespace Messages
{

IMPLEMENT_ENUM_MAP(ActionField)
{
   ADD_ENUM(cDATA_UPDATE_REQUEST_BY_REQUEST_INDICATOR);
   ADD_ENUM(cDATA_UPDATE_REQUEST_BY_TRACK_NUMBER);
   ADD_ENUM(cBALLISTIC_MISSILE_UPDATE_REQUEST);
   ADD_ENUM(cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(ActionPointerField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cWEAPONS);
   ADD_ENUM(cTRACKING);
   ADD_ENUM(cELECTRONIC_WARFARE);
   ADD_ENUM(cSPECIAL_PROCESSING);
   ADD_ENUM(cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(ActionTrackMgmtField)
{
   ADD_ENUM(cDROP_TRACK_REPORT);
   ADD_ENUM(cDIFFERENT_TRACK_REPORT);
   ADD_ENUM(cCHANGE_DATA_ORDER);
   ADD_ENUM(cEMERGENCY_STATUS_CHANGE);
   ADD_ENUM(cFORCE_TELL_STATUSC_CHANGE);
   ADD_ENUM(cSTRENGTH_CHANGE);
   ADD_ENUM(cEXERCISE_STATUS_ORDER);
   ADD_ENUM(cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(AirPlatformActivityField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cRECONNAISSANCE);
   ADD_ENUM(cOVER_THE_HORIZON_TARGETING);
   ADD_ENUM(cTRAINING);
   ADD_ENUM(cLOGISTICS_SUPPORT);
   ADD_ENUM(cANTISURFACE_WARFARE);
   ADD_ENUM(cELECTRONIC_WARFARE);
   ADD_ENUM(cSURVEILLANCE);
   ADD_ENUM(cSEARCH_AND_RESCUE);
   ADD_ENUM(cESCORTING);
   ADD_ENUM(cMINELAYING);
   ADD_ENUM(cTRANSITING);
   ADD_ENUM(cSPECIAL_WEAPONS_ATTACK);
   ADD_ENUM(cINTRUDING);
   ADD_ENUM(cELECTRONIC_WARFARE_SUPPORT);
   ADD_ENUM(cCOMMUNICATIONS_RELAY);
   ADD_ENUM(cPATROL_OCEAN_SURVEILLANCE);
   ADD_ENUM(cAIRLIFT_TRANSPORT);
   ADD_ENUM(cANTISUBMARINE_WARFARE);
   ADD_ENUM(cSHADOWING_INTERVENING);
   ADD_ENUM(cTOWING);
   ADD_ENUM(cAIR_ASSAULT);
   ADD_ENUM(cINTERCEPTION);
   ADD_ENUM(cELECTRONIC_ATTACK);
   ADD_ENUM(cPOLICING);
   ADD_ENUM(cCONVENTIONAL_ATTACK);
   ADD_ENUM(cMEDICAL_EVACUATION);
   ADD_ENUM(cMINE_COUNTERMEASURES);
   ADD_ENUM(cSEARCH);
   ADD_ENUM(cREFUELING_TANKING);
   ADD_ENUM(cINTERDICTION);
   ADD_ENUM(cCOMBAT_AIR_PATROL);
   ADD_ENUM(cFORWARD_AIR_CONTROLLER);
   ADD_ENUM(cVERY_IMPORTANT_PERSON_FLIGHT);
   ADD_ENUM(cNONCOMBATANT_OPERATIONS);
   ADD_ENUM(cCLOSE_AIR_SUPPORT);
   ADD_ENUM(cAIRBORNE_EARLY_WARNING);
   ADD_ENUM(cGROUND_ATTACK_TACTICS);
   ADD_ENUM(cAIRBORNE_COMMAND_POST);
   ADD_ENUM(cRESCUE_COMBAT_AIR_PATROL);
   ADD_ENUM(cSURFACE_COMBAT_AIR_PATROL);
   ADD_ENUM(cTANKING);
   ADD_ENUM(cSPOTTING);
   ADD_ENUM(cSTRIKE_WARFARE);
   ADD_ENUM(cSPECIAL);
   ADD_ENUM(cHIJACK);
   ADD_ENUM(cJAMMER);
   ADD_ENUM(cTROOPLIFT);
   ADD_ENUM(cXRAY);
   ADD_ENUM(cANTIAIR_WARFARE);
   ADD_ENUM(cCOMMAND_AND_CONTROL);
   ADD_ENUM(cCOUNTER_AIR_WARFARE);
   ADD_ENUM(cRETURN_TO_BASE);
   ADD_ENUM(cMINE_WARFARE);
   ADD_ENUM(cCHAFF_LAYING);
   ADD_ENUM(cVIDEO_DATA_LINKING);
   ADD_ENUM(cDIPPING);
   ADD_ENUM(cRESCUE_HELO);
   ADD_ENUM(cORBITING);
   ADD_ENUM(cUNDER_RECALL);
   ADD_ENUM(cENGAGING);
   ADD_ENUM(cENGAGING_PRIORITY_KILL);
   ADD_ENUM(cINVESTIGATING);
   ADD_ENUM(cCLEARED_TO_DROP);
   ADD_ENUM(cINTERVENING);
   ADD_ENUM(cDIVERTING);
   ADD_ENUM(cAIR_TO_GROUND);
   ADD_ENUM(cAIR_TO_AIR);
   ADD_ENUM(cPRECISION_BOMBING);
   ADD_ENUM(cLASER_DESIGNATION);
   ADD_ENUM(cBEACON_BOMBING);
   ADD_ENUM(cSHADOWING);
   ADD_ENUM(cCOVERING);
   ADD_ENUM(cVISUAL_IDENTIFICATION);
   ADD_ENUM(cHIGH_ENERGY_LASING);
}

IMPLEMENT_ENUM_MAP(AirPlatformField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cFIGHTER);
   ADD_ENUM(cFIGHTER_BOMBER);
   ADD_ENUM(cATTACK);
   ADD_ENUM(cBOMBER);
   ADD_ENUM(cRECONNAISSANCE);
   ADD_ENUM(cTANKER);
   ADD_ENUM(cTANKER_BOOM_ONLY);
   ADD_ENUM(cTANKER_DROGUE_ONLY);
   ADD_ENUM(cINTERCEPTOR);
   ADD_ENUM(cTRANSPORT);
   ADD_ENUM(cAIRBORNE_COMMAND_POST);
   ADD_ENUM(cMISSILE_CARRIER);
   ADD_ENUM(cMISSILE);
   ADD_ENUM(cELECTRONIC_WARFARE);
   ADD_ENUM(cANTISUBMARINE_WARFARE);
   ADD_ENUM(cAIRBORNE_EARLY_WARNING_AND_CONTROL);
   ADD_ENUM(cMARITIME_PATROL_AIRCRAFT);
   ADD_ENUM(cSEARCH_AND_RESCUE);
   ADD_ENUM(cDRONE);
   ADD_ENUM(cREMOTELY_PILOTED_VEHICLE);
   ADD_ENUM(cFIXED_WING_GUNSHIP);
   ADD_ENUM(cCIVIL_AIRLINER);
   ADD_ENUM(cCIVIL_GENERAL);
   ADD_ENUM(cLIGHTER_THAN_AIR);
   ADD_ENUM(cGLIDER);
   ADD_ENUM(cDECOY);
   ADD_ENUM(cHELICOPTER);
   ADD_ENUM(cATTACK_HELICOPTER);
   ADD_ENUM(cHELICOPTER_GUNSHIP);
   ADD_ENUM(cANTISUBMARINE_WARFARE_HELICOPTER);
   ADD_ENUM(cMINE_WARFARE_HELICOPTER);
   ADD_ENUM(cTRANSPORT_HELICOPTER);
   ADD_ENUM(cTACTICAL_SUPPORT);
   ADD_ENUM(cPATROL);
   ADD_ENUM(cMISCELLANEOUS_FIXED_WING);
   ADD_ENUM(cMISSILE_CONTROL_UNIT);
   ADD_ENUM(cSURFACE_TO_AIR_MISSILE);
   ADD_ENUM(cAIR_TO_SURFACE_MISSILE);
   ADD_ENUM(cSURFACE_TO_SURFACE_MISSILE);
   ADD_ENUM(cLOGISTIC);
   ADD_ENUM(cAIR_TO_AIR_MISSILE);
   ADD_ENUM(cSUBSURFACE_TO_SURFACE_MISSILE);
   ADD_ENUM(cSURFACE_TO_SUBSURFACE_MISSILE);
   ADD_ENUM(cCRUISE_MISSILE);
   ADD_ENUM(cBALLISTIC_MISSILE);
   ADD_ENUM(cAIRBORNE_LAND_SURVEILLANCE);
   ADD_ENUM(cAIRBORNE_LASER);
}

void AngleField::SetDegrees(double aAngle)
{
   float remainder = (float)fmod(aAngle, 360.0);
   if (remainder < 0.0f)
   {
      remainder += 360.0f;
   }
   mValue = (value_type)remainder;
}

void AngleField16::SetDegrees(double aAngle)
{
   float remainder = (float)fmod(aAngle, 360.0);
   if (remainder < 0.0f)
   {
      remainder += 360.0f;
   }
   mValue = (value_type)(remainder * 100);
}

IMPLEMENT_ENUM_MAP(AltitudeSourceField)
{
   ADD_ENUM4(cNO_STATEMENT_OR_ESTIMATED, cSENSOR, cMANUAL_AIRCRAFT_REPORT, cAUTOMATIC_AIRCRAFT_REPORT);
}

void AngleQualityField::SetQualityRadians(double aRadian)
{
   int quality = (int)(aRadian * 1000.0);
   for (mValue = 0; mValue < 8 && (quality < (64 >> mValue)); ++mValue)
   {
   }
}

double AngleQualityField::GetQualityRadians() const
{
   double miliRadians = (1 << (7 - mValue));
   return miliRadians / 1000.0;
}

char CallsignFieldBase::GetCallsignChar(int aSixBit) const
{
   char lReturn = '_';
   if (aSixBit == 0)
   {
      lReturn = ' ';
   }
   else if (aSixBit >= 1 && aSixBit <= 9)
   {
      lReturn = aSixBit + '0';
   }
   else if (aSixBit >= 10 && aSixBit <= 35)
   {
      lReturn = aSixBit - 10 + 'A';
   }
   else if (aSixBit == 63)
   {
      lReturn = '0';
   }
   return lReturn;
}

int CallsignFieldBase::GetCharEnumeration(char aCharacter) const
{
   int lReturn = 0;
   aCharacter  = toupper(aCharacter);
   // if (aCharacter==' ') lReturn = 0;
   if (aCharacter >= '1' && aCharacter <= '9')
   {
      lReturn = aCharacter - '0';
   }
   if (aCharacter >= 'A' && aCharacter <= 'Z')
   {
      lReturn = aCharacter - 'A' + 10;
   }
   if (aCharacter == '0')
   {
      lReturn = 63;
   }
   return lReturn;
}

IMPLEMENT_ENUM_MAP(CommandField)
{
   ADD_ENUM4(cWEAPONS_FREE, cWEAPONS_TIGHT, cENGAGE, cASSIGN);
   ADD_ENUM4(cCLEAR_ENGAGE, cHOLD_FIRE, cCEASE_FIRE, cCOVER);
   ADD_ENUM4(cCEASE_FIRE, cCOVER, cSALVO_CLEAR_AIRCRAFT, cASSUME_CONTROL);
   ADD_ENUM4(cATTACK, cCEASE_ATTACK, cDISUSED, cPROCEED_TO_POINT);
   ADD_ENUM2(cCEASE_PROCEEDING_TO_POINT, cCONDUCT_PROCEDURES_INDICATED);
   ADD_ENUM2(cCEASE_CONDUCTING_PROCEDURES_INDICATED, cASSUME_DUTIES_INDICATED);
   ADD_ENUM2(cCEASE_DUTIES_INDICATED, cTRANSFER_CONTROL);
   ADD_ENUM2(cTRANSFER_CONTROL, cRETURN_TO_BASE);
   ADD_ENUM2(cLAUNCH_ALERT_AIRCRAFT, cINVESTIGATE);
   ADD_ENUM4(cINTERVENE, cSHADOW, cUNDEFINED, cENGAGE_IN_OBJECTIVE);
   ADD_ENUM(cPRIORITY_KILL);
}

IMPLEMENT_ENUM_MAP(CommImpairmentField)
{
   ADD_ENUM8(cNO_STATEMENT, cUHF_AM, cVHF_AM, cVHF_FM, cHF_SSB_UPPER, cHF, cUHF_FM, cHF_SSB_LOWER);
}


IMPLEMENT_ENUM_MAP(ConfidenceField)
{
   ADD_ENUM8(cNO_STATEMENT,
             cUNCLASSIFIED,
             cPOSSIBLE_SUBMARINE_LOW_ONE,
             cPOSSIBLE_SUBMARINE_LOW_TWO,
             cPOSSIBLE_SUBMARINE_HIGH_THREE,
             cPOSSIBLE_SUBMARINE_HIGH_FOUR,
             cPROBABLE_SUBMARINE,
             cCERTAIN_SUBMARINE);
   ADD_ENUM2(cNON_SUBMARINE, cSURFACE_VESSEL);
}

IMPLEMENT_ENUM_MAP(DepthCategoryField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cSURFACED);
   ADD_ENUM(cESTIMATED_SHALLOW);
   ADD_ENUM(cPERISCOPE_DEPTH);
   ADD_ENUM(cABOVE_FIRST_LAYER);
   ADD_ENUM(cBELOW_FIRST_LAYER);
   ADD_ENUM(cBETWEEN_FIRST_AND_SECOND_LAYERS);
   ADD_ENUM(cBETWEEN_SECOND_AND_THIRD_LAYERS);
   ADD_ENUM(cBELOW_BOTTOM_LAYER);
   ADD_ENUM(cESTIMATED_DEEP);
   ADD_ENUM(cBOTTOMED);
   ADD_ENUM(cUNKNOWN);
}

void HourField::SetHourToNow()
{
   time_t     t;
   struct tm* tmout;
   t     = time(nullptr);
   tmout = gmtime(&t);
   SetHour(tmout->tm_hour);
}

int ElevationAngleField_ConvertToValue(double aAngle, unsigned aMax, unsigned aMid)
{
   int answer;
   aAngle = fmod(aAngle, 360.0);
   if (aAngle > 90.0f || aAngle < -90.0f)
   {
      throw InterpretException("Cannot set elevation angle greater than +-90 degrees");
   }
   int intValue = (int)(aAngle / 90.0 * (aMid - 1));
   if (intValue < 0)
   {
      answer = aMid - intValue + 1;
   }
   else
   {
      answer = intValue;
   }
   return answer;
}

double ElevationAngleField_ExtractDegrees(unsigned aValue, unsigned aMid)
{
   if (aValue < aMid)
   {
      return ((double)aValue) / (aMid - 1);
   }
   else if (aValue > aMid)
   {
      return ((double)(aMid - aValue)) / (aMid - 1);
   }
   return 0;
}

IMPLEMENT_ENUM_MAP(EmitterStatusField)
{
   ADD_ENUM4(cNO_STATEMENT, cINACTIVE, cTARGET_EMITTER_ACTIVE, cTARGET_EMITTER_IN_SEARCH);
   ADD_ENUM2(cTARGET_EMITTER_IN_TRACK, cMISSILE_LAUNCH);
}

IMPLEMENT_ENUM_MAP(EnvironmentField)
{
   ADD_ENUM4(cNO_STATEMENT, cSPACE, cAIR, cSURFACE);
   ADD_ENUM2(cSUBSURFACE, cLAND);
   ADD_ENUM(cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(EngagementActionField)
{
   ADD_ENUM4(cINTEND_TO_ENGAGE, cDO_NOT_INTEND_TO_ENGAGE, cAVAILABLE_FOR_SUPPORT, cNOT_AVAILABLE_FOR_SUPPORT);
   ADD_ENUM2(cSUPPORT_EXPECTED, cSUPPORT_NOT_EXPECTED);
   ADD_ENUM(cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(EquipmentOperationalField)
{
   ADD_ENUM4(cNO_STATEMENT, cOPERATIONAL, cNON_OPERATIONAL, cOFF);
}

IMPLEMENT_ENUM_MAP(FireMethodField)
{
   ADD_ENUM4(cNO_STATEMENT, cSINGLE_SHOT, cDUAL_SHOT, cMULTIPLE_SHOT);
   ADD_ENUM(cSHOOT_LOOK_SHOOT);
}

IMPLEMENT_ENUM_MAP(FixOrBearingField)
{
   ADD_ENUM4(cEW_FIX, cAREA_OF_PROBABILITY, cBEARING_TYPE_NOT_SPECIFIED, cBEARING_ES);
   ADD_ENUM2(cBEARING_EA, cBEARING_RADIO_DIRECTION_FINDING);
   ADD_ENUM(cBEARING_UNKNOWN);
}

IMPLEMENT_ENUM_MAP(IdentityField)
{
   ADD_ENUM8(cPENDING, cUNKNOWN, cASSUMED_FRIEND, cFRIEND, cNEUTRAL, cSUSPECT, cHOSTILE, cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(IdentityAmplifyField)
{
   ADD_ENUM4(cEXERCISE_PENDING, cEXERCISE_UNKNOWN, cEXERCISE_ASSUMED_FRIEND, cEXERCISE_FRIEND);
   ADD_ENUM2(cEXERCISE_NEUTRAL, cJOKER);
   ADD_ENUM(cFAKER);
}

IMPLEMENT_ENUM_MAP(JitterField)
{
   ADD_ENUM4(cNO_STATEMENT, cNO_JITTER, cJITTER_PRESENT, cUNDEFINED);
}

LetterNumberField& LetterNumberField::operator=(char aChar)
{
   if (aChar >= '1' && aChar <= '9')
   {
      mValue = aChar - '1' + 27;
   }
   else if (aChar >= 'A' && aChar <= 'Z')
   {
      mValue = aChar - 'A' + 1;
   }
   else
   {
      char str[2] = {aChar, 0};
      throw InterpretException(std::string("Character ") + str + " not valid in LetterNumberField");
   }
   return *this;
}

LetterNumberField::operator char() const
{
   if (mValue == 0)
   {
      return 0;
   }
   if (mValue <= 26)
   {
      return mValue + 'A' - 1;
   }
   if (mValue <= 35)
   {
      return mValue + '1';
   }
   return 0;
}

IMPLEMENT_ENUM_MAP(LinkStatusField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cAVAILABLE_VIA_LOCAL_BUFFER);
   ADD_ENUM(cAVAILABLE_VIA_REMOTE_BUFFER);
   ADD_ENUM(cAVAILABLE_VIA_LOCAL_AND_REMOTE_BUFFER);
   ADD_ENUM(cDEGRADED);
   ADD_ENUM(cNOT_AVAILABLE);
   ADD_ENUM(cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(LockOnField)
{
   ADD_ENUM4(cNO_STATEMENT, cLOCK_ON, cSPY, cUNDEFINED);
}


void MinuteField::SetMinuteToNow()
{
   time_t     t;
   struct tm* tmout;
   t       = time(nullptr);
   tmout   = gmtime(&t);
   operator=(tmout->tm_min);
}

bool MinuteField::IsIllegalValue() const
{
   return (mValue < 63 && mValue > 59);
}

IMPLEMENT_ENUM_MAP(MissileTypeField)
{
   ADD_ENUM8(cNO_STATEMENT, cSHRT_RNG_SAM, cMED_RNG_SAM, cLONG_RNG_SAM, cSHRT_RNG_SSM, cMED_RNG_SSM, cLONG_RNG_SSM, cSM2_ER);
   ADD_ENUM(cSM2_MR);
   ADD_ENUM8(cSM1_ER,
             cSM1_MR,
             cEXT_RNG_SAM_SHRT,
             cEXT_RNG_SAM_MED,
             cEXT_RNG_SAM_LONG,
             cEXT_RNG_SSM_SHRT,
             cEXT_RNG_SSM_MED,
             cEXT_RNG_SSM_LONG);
   ADD_ENUM8(cSM2_BLOCK_II,
             cSM2_BLOCK_III,
             cSM2_BLOCK_IIIA,
             cSM2_BLOCK_IIIB,
             cSM2_BLOCK_IV,
             cSM2_BLOCK_IVA,
             cTOMAHAWK_TASM,
             cTOMAHAWK_TLAM_C);
   ADD_ENUM8(cHARPOON, cSM3_LEAP, cMISSILE_1, cMISSILE_2, cMISSILE_3, cMISSILE_4, cMISSILE_5, cMISSILE_6);
   ADD_ENUM(cMISSILE_7);
}

IMPLEMENT_ENUM_MAP(MissionTypeField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cREFUEL);
   ADD_ENUM(cORBIT);
   ADD_ENUM(cRECALL);
   ADD_ENUM(cRETURN_TO_BASE);
   ADD_ENUM(cENGAGE);
   ADD_ENUM(cPRIORITY_KILL);
   ADD_ENUM(cBREAK_ENGAGEMENT);
   ADD_ENUM(cINVESTIGATE_INTERROGATE);
   ADD_ENUM(cCLEAR_TO_DROP);
   ADD_ENUM(cCEASE_DO_NOT_DROP);
   ADD_ENUM(cINTERVENE);
   ADD_ENUM(cDIVERT);
   ADD_ENUM(cAIR_TO_SURFACE);
   ADD_ENUM(cAIR_TO_AIR);
   ADD_ENUM(cSEARCH_AND_RESCUE);
   ADD_ENUM(cCOMBAT_AIR_PATROL);
   ADD_ENUM(cPRECISION_BOMBING);
   ADD_ENUM(cLASER_DESIGNATION);
   ADD_ENUM(cBEACON_BOMBING);
   ADD_ENUM(cCLOSE_AIR_SUPPORT);
   ADD_ENUM(cINTERDICTION);
   ADD_ENUM(cAERIAL_RECONNAISSANCE);
   ADD_ENUM(cESCORT);
   ADD_ENUM(cSHADOW);
   ADD_ENUM(cWEAPONS_FREE);
   ADD_ENUM(cWEAPONS_TIGHT);
   ADD_ENUM(cSALVO_CLEAR_AIRCRAFT);
   ADD_ENUM(cALERT_CONDITION_WHITE);
   ADD_ENUM(cALERT_CONDITION_YELLOW);
   ADD_ENUM(cALERT_CONDITION_RED);
   ADD_ENUM(cCOVER);
   ADD_ENUM(cVISUAL_IDENTIFICATION);
   ADD_ENUM(cDISUSED);
   ADD_ENUM(cGO_TO_VOICE);
   ADD_ENUM(cTRACK_DESIGNATION);
   ADD_ENUM(cCANCEL_TRACK_DESIGNATION);
   ADD_ENUM(cSENSOR_TARGET_REPORTS_ON);
   ADD_ENUM(cSENSOR_TARGET_REPORTS_OFF);
   ADD_ENUM(cSUPPRESSION_OF_ENEMY_AD);
   ADD_ENUM(cARMED_RECONNAISSANCE);
   ADD_ENUM(cATTACK);
   ADD_ENUM(cCEASE_ATTACK);
   ADD_ENUM(cRELATED_MISSION_DATA);
   ADD_ENUM(cCOUNTER_AIR_ATTACK);
   ADD_ENUM(cFIGHTER_SWEEP);
   ADD_ENUM(cCEASE_FIRE);
   ADD_ENUM(cHOLD_FIRE);
   ADD_ENUM(cUNDEFINED);
   ADD_ENUM(cCEASE_MISSION);
}

Mode1CodeField& Mode1CodeField::operator=(int aValue)
{
   int twoBits       = aValue & 0x03;
   int threeBits     = aValue & 0x70;
   int newBitPattern = (threeBits >> 1) | twoBits;
   mValue            = SafeCast(newBitPattern);
   return *this;
}

Mode1CodeField::operator int() const
{
   int thisBitPattern = mValue;
   int twoBits        = thisBitPattern & 0x03;
   int threeBits      = thisBitPattern & 0x34;
   int decimalNumber  = (threeBits << 1) | twoBits;
   return decimalNumber;
}

IMPLEMENT_ENUM_MAP(Mode3InterrogationTypeField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cCORRECT);
   ADD_ENUM(cINCORRECT);
   ADD_ENUM(cNO_RESPONSE);
}

IMPLEMENT_ENUM_MAP(Mode4IndicatorField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cNO_RESPONSE);
   ADD_ENUM(cINVALID_RESPONSE);
   ADD_ENUM(cVALID_RESPONSE);
}

void MultiplierField::SetMultiplier(double aFreq)
{
   double val        = aFreq;
   int    multiplier = 0;
   while (val >= 1048575.0)
   {
      val = val / 10;
      ++multiplier;
   }
   mValue = SafeCast(multiplier);
}

void LetterField::SetLetter(char aChar)
{
   int value = toupper(aChar) - 'A' + 1;

   if (value < 1 || value > 26)
   {
      char str[2] = {aChar, 0};
      throw InterpretException(std::string("Character ") + str + " not valid in LetterField");
   }
   mValue = SafeCast(value);
}
void LetterField::SetLetter(int aInt)
{
   if (aInt < 1 || aInt > 26)
   {
      char buff[100];
      sprintf(buff, "Character value %i is not valid in letter field", aInt);
      throw InterpretException(buff);
   }
   mValue = SafeCast(aInt);
}

IMPLEMENT_ENUM_MAP(NetworkParticipationStatusField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cACTIVE_NON_SPECIFIC);
   ADD_ENUM(cINACTIVE_NON_SPECIFIC);
   ADD_ENUM(cCONDITIONAL_RADIO_SILENCE);
   ADD_ENUM(cHIGH_ERROR_RATE);
   ADD_ENUM(cNO_J00_BEING_RECEIVED);
   ADD_ENUM(cTACTICAL_SYSTEM_DATA_FAILURE);
   ADD_ENUM(cPOLLING);
}

QualityField& QualityField::operator=(double aValue)
{
   mValue = 15;
   for (int i = 0; i < 15; ++i)
   {
      if (aValue > FourBitQualityEnumeration[i])
      {
         mValue = SafeCast(i);
         break;
      }
   }
   return *this;
}

QualityField::operator double() const
{
   return FourBitQualityEnumeration[mValue];
}

IMPLEMENT_ENUM_MAP(RadarSystemStatusField)
{
   ADD_ENUM4(cNO_STATEMENT, cSYSTEM_OPERATIONAL, cSYSTEM_NOT_OPERATIONAL, cSYSTEM_OFF);
}

IMPLEMENT_ENUM_MAP(SAM_ModeStateField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cSTANDBY);
   ADD_ENUM(cTRANSITION_TO_OFF_LINE);
   ADD_ENUM(cTRANSITION_TO_STANDBY);
   ADD_ENUM(cTRANSITION_OUT_OF_STANDBY);
   ADD_ENUM(cTRANSITION_TO_ROAD_MARCH);
   ADD_ENUM(cREORIENTATION);
   ADD_ENUM(cINITIALIZATION);
}

void SecondField::SetSecondToNow()
{
   time_t     t;
   struct tm* tmout;
   t      = time(nullptr);
   tmout  = gmtime(&t);
   mValue = SafeCast(tmout->tm_sec);
}

IMPLEMENT_ENUM_MAP(SiteTypeField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cADOC);
   ADD_ENUM(cSOC);
   ADD_ENUM(cSOC_CRC);
   ADD_ENUM(cCRC);
   ADD_ENUM(cCRP);
   ADD_ENUM(cAEGIS);
   ADD_ENUM(cTACC_USAF);
   ADD_ENUM(cCRC_MCE);
   ADD_ENUM(cSSSB);
   ADD_ENUM(cHIMAD_BDE);
   ADD_ENUM(cHIMAD_BN);
   ADD_ENUM(cPATRIOT_BN_ICC);
   ADD_ENUM(cPATRIOT_ECS);
   ADD_ENUM(cHAWK_AFP);
   ADD_ENUM(cSHORAD_BDE);
   ADD_ENUM(cSHORAD_BN);
   ADD_ENUM(cSWC);
   ADD_ENUM(cTACC_USMC);
   ADD_ENUM(cTADC);
   ADD_ENUM(cTAOC);
   ADD_ENUM(cROCC);
   ADD_ENUM(cRADIL);
   ADD_ENUM(cASIT);
   ADD_ENUM(cGCI);
   ADD_ENUM(cMPC);
   ADD_ENUM(cTOC);
   ADD_ENUM(cDIVARTY);
   ADD_ENUM(cNIKE);
   ADD_ENUM(cGACC);
   ADD_ENUM(cPLSS);
   ADD_ENUM(cENSCE);
   ADD_ENUM(cTGIF);
   ADD_ENUM(cFACP);
   ADD_ENUM(cTOC_THAAD);
   ADD_ENUM(cLCS_THAAD);
   ADD_ENUM(cSSI_THAAD);
   ADD_ENUM(cJTAGS);
   ADD_ENUM(cADTOC_BN);
   ADD_ENUM(cADTOC_BDE);
   ADD_ENUM(cADCP);
   ADD_ENUM(cBM_DEFENSE_SITE);
}

IMPLEMENT_ENUM_MAP(SubsurfacePlatformActivityField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cRECONNAISSANCE);
   ADD_ENUM(cOVER_THE_HORIZON_TARGETING_OTHT);
   ADD_ENUM(cTRAINING);
   ADD_ENUM(cDIVING);
   ADD_ENUM(cANTISURFACE_WARFARE);
   ADD_ENUM(cELECTRONIC_WARFARE_EW);
   ADD_ENUM(cSURVEILLANCE);
   ADD_ENUM(cSEARCH_AND_RESCUE_SAR);
   ADD_ENUM(cESCORTING);
   ADD_ENUM(cMINELAYING);
   ADD_ENUM(cTRANSITING);
   ADD_ENUM(cSPECIAL_WEAPONS_ATTACK);
   ADD_ENUM(cSURFACING);
   ADD_ENUM(cAMPHIBIOUS_WARFARE);
   ADD_ENUM(cINTELLIGENCE_COLLECTING);
   ADD_ENUM(cPATROL);
   ADD_ENUM(cTRANSPORT);
   ADD_ENUM(cANTISUBMARINE_WARFARE_ASW);
   ADD_ENUM(cSHADOWING_INTERVENING);
   ADD_ENUM(cBOTTOMING);
   ADD_ENUM(cSPECIAL_WARFARE);
   ADD_ENUM(cSTRIKE_WARFARE);
   ADD_ENUM(cCLANDESTINE_OPERATIONS);
   ADD_ENUM(cSNORKELING);
   ADD_ENUM(cCONVENTIONAL_ATTACK);
   ADD_ENUM(cMINE_COUNTERMEASURES);
   ADD_ENUM(cMINE_WARFARE);
   ADD_ENUM(cMARKING);
   ADD_ENUM(cNONCOMBATANT_OPERATIONS);
   ADD_ENUM(cDIRECT_SUPPORT);
   ADD_ENUM(cVIDEO_DATA_LINKING_TARGETING);
   ADD_ENUM(cRETURN_TO_BASE_RTB);
   ADD_ENUM(cRESET_TO_NO_STATEMENT);
}

IMPLEMENT_ENUM_MAP(SubsurfacePlatformField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cSUBMARINE_PROPULSION_UNKNOWN);
   ADD_ENUM(cDIESEL_ELECTRIC_SUBMARINE_GENERAL);
   ADD_ENUM(cDIESEL_ELECTRIC_ATTACK_SUBMARINE);
   ADD_ENUM(cDIESEL_ELECTRIC_MISSILE_SUBMARINE);
   ADD_ENUM(cDIESEL_ELECTRIC_BALLISTIC_MISSILE_SUBMARINE);
   ADD_ENUM(cTYPE_1_DIESEL);
   ADD_ENUM(cTYPE_2_DIESEL);
   ADD_ENUM(cTYPE_3_DIESEL);
   ADD_ENUM(cNUCLEAR_SUBMARINE_GENERAL);
   ADD_ENUM(cNUCLEAR_ATTACK_SUBMARINE);
   ADD_ENUM(cNUCLEAR_MISSILE_SUBMARINE);
   ADD_ENUM(cNUCLEAR_BALLISTIC_MISSILE_SUBMARINE);
   ADD_ENUM(cTYPE_I_NUCLEAR);
   ADD_ENUM(cTYPE_II_NUCLEAR);
   ADD_ENUM(cTYPE_III_NUCLEAR);
   ADD_ENUM(cTYPE_IV_NUCLEAR);
   ADD_ENUM(cTYPE_V_NUCLEAR);
   ADD_ENUM(cNON_SUBMARINE);
   ADD_ENUM(cSURFACE_VESSEL);
   ADD_ENUM(cTORPEDO);
   ADD_ENUM(cMINE);
   ADD_ENUM(cDECOY);
   ADD_ENUM(cWRECK);
   ADD_ENUM(cSEABED_PIPELINE);
   ADD_ENUM(cFISH_MARINE_LIFE);
   ADD_ENUM(cSWIMMER_FROGMAN);
   ADD_ENUM(cKNUCKLE_WAKE);
   ADD_ENUM(cATTACK_SUBMARINE);
   ADD_ENUM(cCRUISE_MISSILE_LAUNCHER);
   ADD_ENUM(cPINNACLE_SEAMOUNTAIN);
   ADD_ENUM(cNON_MILITARY_SUBMERSIBLE);
   ADD_ENUM(cTYPE_VI_NUCLEAR);
   ADD_ENUM(cTYPE_VII_NUCLEAR);
   ADD_ENUM(cCONVENTIONAL_COMMAND_AND_CONTROL);
   ADD_ENUM(cCONVENTIONAL_AUXILIARY);
   ADD_ENUM(cNUCLEAR_COMMAND_AND_CONTROL);
   ADD_ENUM(cMISSILE_CONTROL_UNIT);
   ADD_ENUM(cRESET_TO_NO_STATEMENT);
}

IMPLEMENT_ENUM_MAP(SystemStatusField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cDEGRADED);
   ADD_ENUM(cNOT_OPERATIONAL);
   ADD_ENUM(cOPERATIONAL);
}

IMPLEMENT_ENUM_MAP(TimeReportField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cTIME_OF_REPORT);
   ADD_ENUM(cARRIVE_ON_STATION);
   ADD_ENUM(cTIME_REMAINING_ON_STATION);
   ADD_ENUM(cDEPART_STATION);
   ADD_ENUM(cTIME_ON_TARGET);
   ADD_ENUM(cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(OperationalCapabilityField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cABLE_TO_PERFORM_MISSION);
   ADD_ENUM(cLOW_FUEL_STATE);
   ADD_ENUM(cUNABLE_TO_PERFORM_MISSION);
}

IMPLEMENT_ENUM_MAP(OperationalImpairmentField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cSURVEILLANCE_RADAR);
   ADD_ENUM(cACQUISITION_RADAR);
   ADD_ENUM(cTRACKING_RADAR);
   ADD_ENUM(cGUIDANCE_RADAR);
   ADD_ENUM(cMULTIFUNCTION_RADAR);
   ADD_ENUM(cHEIGHT_FINDER_RADAR);
   ADD_ENUM(cATC_RADAR);
   ADD_ENUM(cIFF_SIF_INTERROGATOR);
   ADD_ENUM(cMOBILITY);
   ADD_ENUM(cPRIMARY_POWER);
   ADD_ENUM(cCOMMUNICATIONS);
   ADD_ENUM(cES_PASSIVE_SENSOR);
   ADD_ENUM(cMULTISTATIC_PASSIVE_SENSOR);
   ADD_ENUM(cORDNANCE_DEPLETED);
   ADD_ENUM(cLAUNCHER_FAILURE);
   ADD_ENUM(cTDS_FAILURE);
   ADD_ENUM(cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(PeriodicReportField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cORIGIAL_OR_CHANGED);
   ADD_ENUM(cPERIODIC);
   ADD_ENUM(cUNDEFINED);
}

IMPLEMENT_ENUM_MAP(TextIndicatorField)
{
   ADD_ENUM4(cNO_ASSOCIATED_TEXT, cASSOCIATED_TEXT_C2, cASSOCIATED_TEXT_C2_C3, cUNDEFINED);
}

void TimeQualityField::SetTimeUncertainty(double aTimeNS)
{
   mValue = 14;
   for (int i = 0; i < 14; ++i)
   {
      if (aTimeNS > FourBitQualityEnumeration[i])
      {
         mValue = SafeCast(i);
         break;
      }
   }
}

IMPLEMENT_ENUM_MAP(TrackingStatusField)
{
   ADD_ENUM4(cNO_STATEMENT, cNOT_UNDER_TRACK, cTN_OBJECTIVE_UNDER_TRACK, cUNDEFINED);
}

double TimeQualityField::GetTimeUncertainty() const
{
   return FourBitQualityEnumeration[mValue];
}

IMPLEMENT_ENUM_MAP(ThreatWarningField)
{
   ADD_ENUM4(cNO_STATEMENT, cWHITE, cYELLOW, cRED);
}

void TrackNumberField::SetTrackNumber(const std::string aTNO)
{
   int         tNo = 0, bitShift = 0;
   std::string tno = aTNO;
   // first trim the string to 5 digits
   if (tno.length() > 5)
   {
      tno = tno.substr(0, 5);
   }
   while (tno.length() < 5)
   {
      tno = '0' + tno;
   }
   int i;
   for (i = 4; i >= 2; --i)
   {
      int val = tno[i] - '0';
      if (val < 0 || val > 7)
      {
         throw InterpretException("Invalid track number: " + tno);
      }
      tNo += val << bitShift;
      bitShift += 3;
   }
   for (i = 1; i >= 0; --i)
   {
      int val = CharToFiveBit(tno[i]);
      tNo += val << bitShift;
      bitShift += 5;
   }

   mValue = SafeCast(tNo);
}

void TrackNumberField::SetTrackNumber(int aTNO)
{
   int itemp   = aTNO % 4096;
   int itemp2  = aTNO / 4096;
   int overTmp = itemp2 << 14;
   itemp       = itemp | overTmp;
   mValue      = SafeCast(itemp);
}

int TrackNumberField::GetTrackNumberInteger() const
{
   return GetTrackNumberInteger(mValue);
}

// static
int TrackNumberField::GetTrackNumberInteger(int aTrackNumber)
{
   int itmp      = aTrackNumber & 07777;
   int itemp     = aTrackNumber >> 14;
   int itemp2    = itemp * 4096;
   int returnInt = itemp2 + itmp;
   return returnInt;
}

// static
int TrackNumberField::GetTrackNumberInteger(const std::string& aTNO_String)
{
   int l16_TrackNumber = strtol(aTNO_String.substr(2).c_str(), nullptr, 8);
   l16_TrackNumber += CharToFiveBit(aTNO_String[1]) << 9;
   l16_TrackNumber += CharToFiveBit(aTNO_String[0]) << 14;
   return l16_TrackNumber;
}

// static
void TrackNumberField::GetTrackNumberString(int aNumber, std::string& aTNO_String)
{
   aTNO_String = "     ";
   int tNo     = aNumber;
   int i;
   for (i = 4; i >= 2; --i)
   {
      aTNO_String[i] = (tNo & 7) + '0';
      tNo            = tNo >> 3;
   }
   for (i = 1; i >= 0; --i)
   {
      aTNO_String[i] = FiveBitToChar(tNo & 0x1f);
      tNo            = tNo >> 5;
   }
}

std::string TrackNumberField::GetTrackNumberString() const
{
   std::string lReturn = "     ";
   GetTrackNumberString(mValue, lReturn);
   return lReturn;
}

int TrackNumberField::CharToFiveBit(char aChar)
{
   if (aChar >= '0' && aChar <= '7')
   {
      return aChar - '0';
   }
   else if (aChar >= 'A' && aChar <= 'Z')
   {
      if (aChar == 'I' || aChar == 'O')
      {
         throw InterpretException("Letters I and O are invalid in track numbers.");
      }
      if (aChar > 'I')
      {
         --aChar;
      }
      if (aChar >= 'O')
      {
         --aChar;
      }
      return aChar - 'A' + 8;
   }
   char str[2] = {aChar, 0};
   throw InterpretException("Character " + std::string(str) + " is not valid in the track number.");
}

char TrackNumberField::FiveBitToChar(int aBits)
{
   if (aBits < 8)
   {
      return '0' + aBits;
   }
   else if (aBits < 32)
   {
      char c = aBits - 8 + 'A';
      if (c >= 'I')
      {
         ++c;
      }
      if (c >= 'O')
      {
         ++c;
      }
      return c;
   }
   throw InterpretException("Invalid bit sequence in track number.");
}

void TrackQualityField::SetUncertaintySquareMiles(double aValue)
{
   mValue = SafeCast(15);
   for (int i = 0; i < 15; ++i)
   {
      if (aValue > SquareMilesQualityEnumeration[i])
      {
         mValue = SafeCast(i);
         break;
      }
   }
}

double TrackQualityField::GetUncertaintySquareMiles() const
{
   return SquareMilesQualityEnumeration[mValue];
}

double TrackQualityField::GetUncertaintySquareMeters() const
{
   const double cM_PER_DM   = UtMath::cM_PER_FT * 6000.0;
   double       linearError = sqrt(GetUncertaintySquareMiles()) * cM_PER_DM;
   return linearError * linearError;
}

double TrackQualityField::GetDistance95CEP() const
{
   if (mValue == 0)
   {
      return 0;
   }
   const double errorTable95CEP[15] =
      {177697.0, 88662.0, 70927.0, 53202.0, 35504.0, 17590.0, 7101.0, 3550.0, 567.0, 283.0, 144.0, 68.0, 34.0, 19.0};
   return errorTable95CEP[mValue - 1];
}


void TrackQualityField1::SetUncertaintySquareFeet(double aValue)
{
   mValue = 0;
   for (int i = 15; i > 0; --i)
   {
      if (aValue <= SquareFeetTrackQualityEnumeration1[i])
      {
         mValue = SafeCast(i);
         break;
      }
   }
}

double TrackQualityField1::GetUncertaintySquareFeet() const
{
   return SquareFeetTrackQualityEnumeration1[mValue];
}

double TrackQualityField1::GetDistance95CEP() const
{
   if (mValue == 0)
   {
      return 0;
   }
   const double errorTable95CEP[15] =
      {39.0, 70.0, 127.0, 229.0, 413.0, 744.0, 1342.0, 2419.0, 4361.0, 7862.0, 14173.0, 25551.0, 46062.0, 83040.0, 83040.0};
   return errorTable95CEP[15 - mValue];
}

double TrackQualityField1::GetUncertaintySquareMeters() const
{
   double linearError = sqrt(GetUncertaintySquareFeet()) * UtMath::cM_PER_FT;
   return linearError * linearError;
}


TrackStrengthField::operator int() const
{
   int lUnits = 14;
   if (mValue <= 12)
   {
      lUnits = mValue;
   }
   if (mValue == 13)
   {
      lUnits = 4;
   }
   if (mValue == 14)
   {
      lUnits = 10;
   }
   return lUnits;
}

TrackStrengthField& TrackStrengthField::operator=(int numUnits)
{
   if (numUnits > 12)
   {
      mValue = SafeCast(15);
   }
   else
   {
      mValue = SafeCast(numUnits);
   }
   return *this;
}

IMPLEMENT_ENUM_MAP(TrackMgmtStrengthField)
{
   ADD_ENUM4(cNO_STATEMENT, c1_VEHICLE, c2_VEHICLES, c3_VEHICLES);
   ADD_ENUM4(c4_THRU_5_VEHICLES, c6_THRU_7_VEHICLES, c8_THRU_10_VEHICLES, c11_THRU_15_VEHICLES);
   ADD_ENUM4(c16_THRU_20_VEHICLES, c21_THRU_30_VEHICLES, c31_THRU_40_VEHICLES, c41_THRU_50_VEHICLES);
   ADD_ENUM4(c51_TRUE_70_VEHICLES, c71_THRU_100_VEHICLES, c101_THRU_200_VEHICLES, cGREATER_THAN_200_VEHICLES);
}
/*
IMPLEMENT_ENUM_MAP(TrackMgmtStrengthPercentField)
{
   ADD_ENUM2(cNO_STATEMENT,               cLT_OR_EQUAL_2_PERCENT);
   ADD_ENUM2(cGT_2_OR_EQUAL_4_PERCENT,    cGT_4_OR_EQUAL_6_PERCENT);
   ADD_ENUM2(cGT_6_OR_EQUAL_8_PERCENT,    cGT_8_OR_EQUAL_10_PERCENT);
   ADD_ENUM2(cGT_10_OR_EQUAL_15_PERCENT,  cGT_15_OR_EQUAL_20_PERCENT);
   ADD_ENUM2(cGT_20_OR_EQUAL_25_PERCENT,  cGT_25_OR_EQUAL_30_PERCENT);
   ADD_ENUM2(cGT_30_OR_EQUAL_40_PERCENT,  cGT_40_OR_EQUAL_50_PERCENT);
   ADD_ENUM2(cGT_50_OR_EQUAL_60_PERCENT,  cGT_60_OR_EQUAL_70_PERCENT);
   ADD_ENUM2(cGT_70_OR_EQUAL_80_PERCENT,  cGT_80_PERCENT);
}
*/

IMPLEMENT_ENUM_MAP(VectorField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cTO_WAYPOINT);
   ADD_ENUM(cALTITUDE_CHANGE_WARNING);
   ADD_ENUM(cSPEED_CHANGE_WARNING);
   ADD_ENUM(cCOURSE_CHANGE_WARNING);
   ADD_ENUM(cCOMMAND_CONTROL);
   ADD_ENUM(cREVERT_TO_VOICE);
   ADD_ENUM(cLANDING_CHECKLIST);
   ADD_ENUM(cCOMMAND_COURSE_CHANGE);
   ADD_ENUM(cCOMMAND_ALTITUDE_CHANGE);
   ADD_ENUM(cCOMMAND_SPEED_CHANGE);
   ADD_ENUM(cBUG_OUT_HEADING);
   ADD_ENUM(cCV_RECOVERY_CASE_1);
   ADD_ENUM(cCV_RECOVERY_CASE_2);
   ADD_ENUM(cCV_RECOVERY_CASE_3);
   ADD_ENUM(cFIELD_CONDITION_VMC);
   ADD_ENUM(cFIELD_CONDITION_IMC);
   ADD_ENUM(cJOIN_UP_RENDEZVOUS);
   ADD_ENUM(cNOT_UNDER_COMMAND);
   ADD_ENUM(cVISUAL_ID_TARGET_VECTOR);
   ADD_ENUM(cCOLLISION_CUT_OFF_VECTOR);
   ADD_ENUM(cPURSUIT_VECTOR);
   ADD_ENUM(cSTERN_VECTOR);
   ADD_ENUM(cORBIT_VECTOR);
   ADD_ENUM(cREFUEL_VECTOR);
}

IMPLEMENT_ENUM_MAP(WeaponEngagementStatusField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cRECOMMEND_REATTACK);
   ADD_ENUM(cWEAPON_ASSIGNED);
   ADD_ENUM(cTRACKING_LOCKED_ON_READY_TO_FIRE_BIRD_AFFIRM);
   ADD_ENUM(cFIRING_MISSILE_IN_FLIGHT_ENGAGING_TO_DESTROY);
   ADD_ENUM(cEFFECTIVE_TARGET_DESTROYED_GRAND_SLAM);
   ADD_ENUM(cPARTIALLY_EFFECTIVE);
   ADD_ENUM(cNOT_EFFECTIVE);
   ADD_ENUM(cENGAGEMENT_BROKEN);
   ADD_ENUM(cHEADS_UP);
   ADD_ENUM(cENGAGEMENT_INTERRUPTED);
   ADD_ENUM(cINVESTIGATING_INTERROGATING);
   ADD_ENUM(cSHADOWING);
   ADD_ENUM(cINTERVENING);
   ADD_ENUM(cCOVERING);
   ADD_ENUM(cBATTLE_DAMAGE_ASSESSMENT_UNKNOWN);
}

IMPLEMENT_ENUM_MAP(WeaponSystemField)
{
   ADD_ENUM(cNO_STATEMENT);
   ADD_ENUM(cSURFACE_TO_AIR_MISSILE_SAM);
   ADD_ENUM(cSURFACE_TO_SURFACE_MISSILE_SSM);
   ADD_ENUM(cAIRCRAFT);
   ADD_ENUM(cCONVENTIONAL_SHRT_RNG_MISSILE_OR_GUNS);
   ADD_ENUM(cANTISUBMARINE_WARFARE_HELICOPTER);
   ADD_ENUM(cANTISUBMARINE_WARFARE_MISSILE);
   ADD_ENUM(cDEPTH_CHARGE_DEPTH_BOMB);
   ADD_ENUM(cTORPEDO);
   ADD_ENUM(cFIXED_WING_ANTISUBMARINE_WARFARE_AIRCRAFT);
   ADD_ENUM(cLASER);
   ADD_ENUM(cBEAM_WEAPON);
   ADD_ENUM(cINFRARED_DECOY);
   ADD_ENUM(cCHAFF);
   ADD_ENUM(cSPOT_JAMMER);
   ADD_ENUM(cBARRAGE_JAMMER);
   ADD_ENUM(cDECEPTION_JAMMER);
   ADD_ENUM(cINTERCEPTOR);
   ADD_ENUM(cAIR_TO_SURFACE_MISSILE_ASM);
}

IMPLEMENT_ENUM_MAP(WeaponTypeField)
{
   ADD_ENUM4(cALL_WEAPON_SYSTEMS, cAIRCRAFT, cMISSILE, cCONVENTIONAL);
}

} // namespace Messages
} // namespace WsfL16
