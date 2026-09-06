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
#ifndef L16_MESSAGE11_2_HPP
#define L16_MESSAGE11_2_HPP
#include "MessageDefines.hpp"

namespace WsfL16
{
namespace Messages
{
namespace J11_2
{
// J11.2 Weapon Coordination Message: I, C1-C5

class WeaponCoordinationField : public NormalField<4, NoStatementZero>
{
public:
   enum WeaponCoordinationType
   {
      cNO_STATEMENT,
      cIMMEDIATE_CONTROL_REQUEST_TO_CC,
      cFUTURE_CONTROL_REQUEST_TO_CC,
      cIMMEDIATE_IFTU_3PS_REQUEST_TO_CC,
      cFUTURE_IFTU_3PS_REQUEST_TO_CC,
      cIMMEDIATE_CONTROL_REQUEST_FROM_CC,
      cFUTURE_CONTROL_REQUEST_FROM_CC,
      cIMMEDIATE_IFTU_3PS_REQUEST_FROM_CC,
      cFUTURE_IFTU_3PS_REQUEST_FROM_CC,
      cACCEPT_RESPONSE,
      cREJECT_RESPONSE,
      cHANDOFF_FAILURE,
      cCEASE_IFTU_3PS_REQUEST_FROM_CC
   };
   DEFINE_ACCESSORS(WeaponCoordinationField, WeaponCoordinationType);
};

class Initial : public InitialBase
{
public:
   DEFINE_INITIAL(11, 2);

   typedef WeaponCoordinationField FieldType1; // WEAPON COORDINATION DISCRETE
   typedef SpareField<18>          FieldType2;
   typedef SpareField<15>          FieldType3;
   typedef SpareField<20>          FieldType4;

   DEFINE_MEMBERS4(WeaponCoordination, Spare2, Spare3, Spare4);

   DEFINE_CONTINUATIONS0()
   DEFINE_EXTENSIONS0()

protected:
};

} // namespace J11_2
} // namespace Messages
} // namespace WsfL16

#endif
