// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofGraphicalSupport.hpp"

#include "UtLog.hpp"

P6DofGraphicalSupport::P6DofGraphicalSupport()
   : mObjectGraphicalType(0)
   , mObjectGraphicalAltType1(0)
   , mObjectGraphicalAltType2(0)
   , mObjectGraphicalAltType3(0)
   , mCurrentObjectGraphicalType(0)
{
}

P6DofGraphicalSupport* P6DofGraphicalSupport::Clone() const
{
   return new P6DofGraphicalSupport(*this);
}

void P6DofGraphicalSupport::SetObjectGraphicalType(int aTypeId)
{
   if (aTypeId < 0)
   {
      ut::log::warning() << "Negative typeId in P6DofGraphicalSupport::SetObjectGraphicalType().";
      return;
   }

   mObjectGraphicalType = (unsigned short)aTypeId;
}

void P6DofGraphicalSupport::SetObjectGraphicalAltType1(int aTypeId)
{
   if (aTypeId < 0)
   {
      ut::log::warning() << "Negtive typeId in P6DofGraphicalSupport::SetObjectGraphicalAltType1().";
      return;
   }

   mObjectGraphicalAltType1 = (unsigned short)aTypeId;
}

void P6DofGraphicalSupport::SetObjectGraphicalAltType2(int aTypeId)
{
   if (aTypeId < 0)
   {
      ut::log::warning() << "Negtive typeId in P6DofGraphicalSupport::SetObjectGraphicalAltType2().";
      return;
   }

   mObjectGraphicalAltType2 = (unsigned short)aTypeId;
}

void P6DofGraphicalSupport::SetObjectGraphicalAltType3(int aTypeId)
{
   if (aTypeId < 0)
   {
      ut::log::warning() << "Negtive typeId in P6DofGraphicalSupport::SetObjectGraphicalAltType3().";
      return;
   }

   mObjectGraphicalAltType3 = (unsigned short)aTypeId;
}

void P6DofGraphicalSupport::SetCurrentObjectGraphicalType(int aAltType)
{
   switch (aAltType)
   {
   case 0:
      mCurrentObjectGraphicalType = mObjectGraphicalType;
      break;
   case 1:
      mCurrentObjectGraphicalType = mObjectGraphicalAltType1;
      break;
   case 2:
      mCurrentObjectGraphicalType = mObjectGraphicalAltType2;
      break;
   case 3:
      mCurrentObjectGraphicalType = mObjectGraphicalAltType3;
      break;
   default:
      break;
   }
}
