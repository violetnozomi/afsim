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
#ifndef L16_MESSAGE11_0_HPP
#define L16_MESSAGE11_0_HPP
#include <cmath>

#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J11_0
{
// J11.0 Weapon Response/Status Message: I, E0, C1-C20

class WeaponDiscreteField : public NormalField<5, NoStatementZero>
{
public:
   enum WeaponDiscreteType
   {
      cNO_STATEMENT,
      cACKNOWLEDGE,
      cNEGATIVE_ACKNOWLEDGE,
      cPING_RESPONSE,
      cLOSS_OF_COMM_CHECK_IN,
      cCONTACT_REPORT,
      cHANDOFF_CHECK_IN,
      cBOMB_HIT_INIDICATION,
      cSELF_ABORT,
      cBASIC_WIFT,
      cWIFT_SUPPLEMENT
   };
   DEFINE_ACCESSORS(WeaponDiscreteField, WeaponDiscreteType);
};

class WeaponSelfAbortField : public NormalField<3, NoStatementZero>
{
public:
   enum WeaponSelfAbortType
   {
      cNO_STATEMENT,
      cNO_IMPACT_EXCLUSION_ZONE,
      cNO_FLY_EXCLUSION_ZONE,
      cPRIMARY_TARGET_NOT_DETECTED,
      cFAILURE,
      cUNDEFINED_1,
      cUNDEFINED_2,
      cUNDEFINED_3
   };
   DEFINE_ACCESSORS(WeaponSelfAbortField, WeaponSelfAbortType);
};

class WeaponDetailedStatusField : public NormalField<5, NoStatementZero>
{
public:
   enum WeaponDetailedStatusType
   {
      cNO_STATEMENT,
      cUNKNOWN,
      cAGL_LIMIT,
      cMSL_LIMIT,
      cMIN_SPEED,
      cMAX_TIME_OF_FLIGHT,
      cCOAST_TIME_EXCEEDED,
      cMAX_GIMBAL,
      cMAX_LOS_RATE,
      cMIN_MACH,
      cMIN_CLOSING_VELOCITY,
      cMIN_ACCELERATION,
      cILLUMINATION_LOST,
      cTARGET_NOT_AVAILABLE,
      cDATA_LINK_LOST,
      cNO_GUIDANCE,
      cTARGET_NOT_ACQUIRED,
      cTRACK_REQUIRED_AT_LAUNCH,
      cNOT_ARMED,
      cOUTSIDE_KILL_RADIUS,
      cDOPPLER_COAST_TIME_EXCEEDED,
      cDOPPLER_COAST_THEN_MAX_GIMBAL,
      cDOPPLER_COAST_THEN_MAX_LOS_RATE,
      cFUZE_VELOCITY,
      cINSUFFICIENT_MEMORY,
      cCAN_NOT_LAUNCH,
      cMAX_BORESIGHT_ERROR,
      cLAUNCHER_NOT_AVAILABLE
   };
   DEFINE_ACCESSORS(WeaponDetailedStatusField, WeaponDetailedStatusType);
};

class WeaponProfileField : public NormalField<3, NoStatementZero>
{
public:
   enum WeaponProfileType
   {
      cNO_STATEMENT,
      cPRELAUNCH,
      cLAUNCH,
      cMIDCOURSE,
      cLOITER,
      cSEARCH,
      cTERMINAL,
      cABORT
   };
   DEFINE_ACCESSORS(WeaponProfileField, WeaponProfileType);
};

class WeaponNEW_TypeField : public NormalField<6, NoStatementZero>
{
public:
   enum WeaponNEW_TypeType
   {
      cNO_STATEMENT,
      cJASSM_ER,
      cSDB,
      cJDAM,
      cMALD,
      cJSOW,
      cHARPOON_BLK_3,
      cUNDEFINED_1,
      cUNDEFINED_2,
      cUNDEFINED_3,
      cUNDEFINED_4,
      cUNDEFINED_5
   };
   DEFINE_ACCESSORS(WeaponNEW_TypeField, WeaponNEW_TypeType);
};

class SeekerAcquisitionConfidenceField : public NormalField<3, NoStatementZero>
{
public:
   enum SeekerAcquisitionConfidenceType
   {
      cNO_STATEMENT,
      cBELOW_50,
      c50_THROUGH_59,
      c60_THROUGH_69,
      c70_THROUGH_79,
      c80_THROUGH_89,
      c90_THROUGH_99,
      c100
   };
   DEFINE_ACCESSORS(SeekerAcquisitionConfidenceField, SeekerAcquisitionConfidenceType);
};

class SeekerAcquisitionStatusField : public NormalField<4, NoStatementZero>
{
public:
   enum SeekerAcquisitionStatusType
   {
      cNO_STATEMENT,
      cNOT_IN_SEARCH_OR_TRACK,
      cSEARCH_MODE_COURSE,
      cSEARCH_MODE_FINE,
      cMULTIPLE_POTENTIAL_TARGETS,
      cSINGLE_POTENTIAL_TARGETS,
      cTRACKING,
      cTRACKING_200_FT,
      cTRACKING_100_FT,
      cTRACKING_50_FT,
      cTRACKING_20_FT,
      cTRACKING_10_FT,
      cTRACKING_5_FT,
      cTRACKING_2_FT,
      cTRACKING_1_FT,
      cTRACKING_0_5_FT
   };
   DEFINE_ACCESSORS(SeekerAcquisitionStatusField, SeekerAcquisitionStatusType);
};

template<unsigned BIT_COUNT>
class SpeedField : public NormalField<BIT_COUNT, NoStatementMax>
{
   typedef typename NormalField<BIT_COUNT, NoStatementMax>::value_type value_type;

public:
   typedef double                          accessor_type;
   typedef AccessorUnitsMetersPerSecondTag accessor_units;
   SpeedField&                             operator=(double aMPS)
   {
      SetSpeedMetersPerSec(aMPS);
      return *this;
   }
          operator double() const { return GetSpeedMetersPerSec(); }
   void   SetSpeedKnots(double aSpeed) { this->mValue = (value_type)((aSpeed / 5 > 254) ? 254 : aSpeed / 5); }
   void   SetSpeedMetersPerSec(double aSpeed) { SetSpeedKnots(MetersPerSecToKnots(aSpeed)); }
   double GetSpeedKnots() const { return this->mValue * 5.0; }
   double GetSpeedMetersPerSec() const { return KnotsToMetersPerSec(GetSpeedKnots()); }
};

class TimeToGoField : public NormalField<13, NoStatementMax>
{
public:
   typedef double accessor_type;
   TimeToGoField& operator=(double aTimeToGo)
   {
      if (aTimeToGo > ((cMAX_VALUE - 2) * 2))
      {
         mValue = (value_type)(cMAX_VALUE - 1);
      }
      else
      {
         mValue = (value_type)(aTimeToGo / 2);
      }
      return *this;
   }
   operator double() const { return mValue * 2.0; }
};

class TimeToInputLockoutField : public NormalField<5, NoStatementMax>
{
public:
   typedef int              accessor_type;
   TimeToInputLockoutField& operator=(double aSeconds)
   {
      SetTimeToLockout(aSeconds);
      return *this;
   }
        operator int() const { return GetTimeToLockout(); }
   int  GetTimeToLockout() const { return mValue; }
   void SetTimeToLockout(double aSeconds)
   {
      // Convert from time in seconds to TTL range of 0 - 30
      if (aSeconds <= 2.0)
      {
         mValue = 0;
      }
      else
      {
         // How to calculate the TTL value based on the time in seconds remaining:
         // Excel formula: ROUND(ROUND((-1+SQRT(ABS(1-(4*(aSeconds * 2 - 2))))/2+1/(aSeconds*aSeconds)),2),0)
         // C++ compatible formula: FLOOR(FLOOR((-1+SQRT(ABS(1-(4*(aSeconds * 2 - 2))))/2+1/(aSeconds*Seconds))+0.5,1)+0.5,1)
         mValue =
            (int)floor((floor((-1 + sqrt(fabs(1 - (4 * (aSeconds * 2 - 2)))) / 2 + 1 / (pow(aSeconds, 2)))) + 0.5) + 0.5);
      }
   }
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

struct VelocityField11 : public NormalField<11, NoStatementMid>
{
public:
   typedef double                          accessor_type;
   typedef AccessorUnitsMetersPerSecondTag accessor_units;
   VelocityField11&                        operator=(double aVal)
   {
      mValue = (value_type)EncodeTCMod((int)MetersToFeet(aVal));
      return *this;
   }
   operator double() const { return FeetToMeters(DecodeTC(mValue)); }
};

class Continuation18 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 18)

   typedef SpareField<6>      FieldType1; //
   typedef LongitudeField<28> FieldType2; // AIMPOINT LONGITUDE, 0.00008 MINUTE
   typedef AltitudeField1<18> FieldType3; // HEIGHT ABOVE ELLIPSOID
   typedef SpareField<11>     FieldType4; //

   DEFINE_MEMBERS4(Spare1, AimpointLongitude, AimpointHeightAboveEllipsoid, Spare4);
};

class Continuation17 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 17)

   typedef LatitudeField<27> FieldType1; // AIMPOINT LATITUDE, 0.00008 MINUTE
   typedef SpareField<25>    FieldType2; //
   typedef SpareField<11>    FieldType3; //

   DEFINE_MEMBERS3(AimpointLatitude, Spare2, Spare3);
};

class Continuation16 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 16)

   typedef SpareField<1>      FieldType1; //
   typedef AltitudeField1<18> FieldType2; // HEIGHT ABOVE ELLIPSOID
   typedef VelocityField9     FieldType3; // TARGET VELOCITY NORTH
   typedef VelocityField9     FieldType4; // TARGET VELOCITY EAST
   typedef VelocityField8     FieldType5; // TARGET VELOCITY DOWN
   typedef SpareField<18>     FieldType6; //

   DEFINE_MEMBERS6(Spare1, TargetHeightAboveEllipsoid, TargetVelocityNorth, TargetVelocityEast, TargetVelocityDown, Spare6);
};

class Continuation15 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 15)

   typedef LatitudeField<27>  FieldType1; // TARGET LATITUDE, 0.00008 MINUTE
   typedef LongitudeField<28> FieldType2; // TARGET LONGITUDE, 0.00008 MINUTE
   typedef SpareField<8>      FieldType3; //

   DEFINE_MEMBERS3(TargetLatitude, TargetLongitude, Spare3);
};

class Continuation14 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 14)

   typedef AltitudeField1<18> FieldType1; // HEIGHT ABOVE ELLIPSOID
   typedef VelocityField11    FieldType2; // WEAPON VELOCITY NORTH
   typedef VelocityField11    FieldType3; // WEAPON VELOCITY EAST
   typedef VelocityField11    FieldType4; // WEAPON VELOCITY DOWN
   typedef SpareField<12>     FieldType5; //

   DEFINE_MEMBERS5(WeaponHeightAboveEllipsoid, WeaponVelocityNorth, WeaponVelocityEast, WeaponVelocityDown, Spare5);
};

class Continuation13 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 13)

   typedef LatitudeField<27>  FieldType1; // LATITUDE, 0.00008 MINUTE
   typedef LongitudeField<28> FieldType2; // LONGITUDE, 0.00008 MINUTE
   typedef SpareField<8>      FieldType3; //

   DEFINE_MEMBERS3(WeaponLatitude, WeaponLongitude, Spare3);
};

class Continuation9 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 9)

   typedef WeaponDetailedStatusField FieldType1; //  DETAILED STATUS INFORMATION
   typedef SpareField<25>            FieldType2; //
   typedef SpareField<25>            FieldType3; //
   typedef SpareField<8>             FieldType4; //

   DEFINE_MEMBERS4(DetailedStatusInformation, Spare2, Spare3, Spare4);
};

class Continuation5 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 5)

   typedef SpareField<4>           FieldType1; // GEODETIC POSITION UNCERTAINTY
   typedef SpareField<4>           FieldType2; // HEIGHT UNCERTAINTY
   typedef TimeToInputLockoutField FieldType3; // TIME TO INPUT LOCKOUT, 0 - 30
   typedef SpareField<10>          FieldType4; // FLIGHT TIME REMAINING
   typedef SpareField<19>          FieldType5; // TARGET TRACK NUMBER
   typedef SpareField<15>          FieldType6; // TRACK NUMBER INDEX ORIGINATOR
   typedef SpareField<6>           FieldType7; // TARGET INDEX NUMBER

   DEFINE_MEMBERS7(Spare1, Spare2, TimeToLockout, Spare4, Spare5, Spare6, Spare7);
};

class Continuation4 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 4)

   typedef SpareField<25>            FieldType1; //
   typedef SpareField<23>            FieldType2; //
   typedef TrackNumberAddresseeField FieldType3; // TRACK NUMBER, CONTROLLER

   DEFINE_MEMBERS3(Spare1, Spare2, TrackNumberController);
};

class Continuation3 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 3)

   typedef SpareField<25> FieldType1;
   typedef SpareField<25> FieldType2;
   typedef SpareField<13> FieldType3;

   DEFINE_MEMBERS3(Spare1, Spare2, Spare3);
};

class Continuation2 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 2)

   typedef SpeedField<8>        FieldType1; // WEAPON SPEED
   typedef AngleField           FieldType2; // COURSE
   typedef WeaponSelfAbortField FieldType3; // WEAPON SELF ABORT REASON
   typedef TimeToGoField        FieldType4; // WEAPON TIME TO GO
   typedef SpareField<25>       FieldType5; //
   typedef SpareField<5>        FieldType6; //

   DEFINE_MEMBERS6(Speed, Course, SelfAbortReason, TimeToGoSeconds, Spare5, Spare6);
};

class Continuation1 : public ContinuationBase
{
public:
   DEFINE_CONTINUATION(11, 0, 1)

   typedef LatitudeField<21>  FieldType1; // WEAPON LATITUDE, 0.0051 MINUTE
   typedef LongitudeField<22> FieldType2; // WEAPON LONGITUDE, 0.0051 MINUTE
   typedef AltitudeField1<12> FieldType3; // WEAPON HEIGHT ABOVE ELLIPSOID
   typedef SpareField<8>      FieldType4; //

   DEFINE_MEMBERS4(Latitude, Longitude, HeightAboveEllipsoid, Spare4);
};

class Extension0 : public ExtensionBase
{
public:
   DEFINE_EXTENSION(11, 0, 0)

   typedef SpareField<16>               FieldType1;
   typedef SeekerAcquisitionStatusField FieldType2; // SEEKER ACQUISITION STATUS/QUALITY
   typedef SpareField<1>                FieldType3;
   typedef TrackNumberAddresseeField    FieldType4; // TRACK NUMBER, INDEX ORIGINATOR 2
   typedef SpareField<25>               FieldType5;
   typedef SpareField<7>                FieldType6;

   DEFINE_MEMBERS6(Spare1, SeekerAcquisitionStatus, Spare3, TrackNumberIndexOrig2, Spare5, Spare6);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(11, 0);

   typedef WeaponDiscreteField              FieldType1; // WEAPON STATUS DISCRETE
   typedef WeaponNEW_TypeField              FieldType2; // TYPE OF NEW
   typedef WeaponProfileField               FieldType3; // WEAPON PROFILE
   typedef SpareField<27>                   FieldType4; //
   typedef SeekerAcquisitionConfidenceField FieldType5; // SEEKER AQUISITION CONFIDENCE
   typedef SpareField<13>                   FieldType6; //

   DEFINE_MEMBERS6(WeaponStatus, WeaponType, WeaponProfile, Spare4, SeekerAquisitionConfidence, Spare6);

   DEFINE_CONTINUATIONS12(1, 2, 3, 4, 5, 9, 13, 14, 15, 16, 17, 18)
   DEFINE_EXTENSIONS1()

protected:
};

} // namespace J11_0
} // namespace Messages
} // namespace WsfL16

#endif
