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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisDirectedEnergyAimpoint.hpp"

#include "Dis.hpp"
#include "DisPrecisionBeamAimpoint.hpp"

DisDirectedEnergyAimpoint::DisDirectedEnergyAimpoint()
   : mAimpointType(1)
   , // only precision aimpoint type currently supported.
   mPrecisionBeamAimpointPtr(nullptr)
{
}

// virtual
DisDirectedEnergyAimpoint::~DisDirectedEnergyAimpoint()
{
   if (mAimpointType == 1)
   {
      delete mPrecisionBeamAimpointPtr;
   }
   else if (mAimpointType == 2)
   {
      // delete mDE_AreaAimpointPtr;  // (todo)
   }
}

DisUint16 DisDirectedEnergyAimpoint::GetLength()
{
   DisUint16 length = 0;
   if (mAimpointType == 1)
   {
      length = 4 + mPrecisionBeamAimpointPtr->GetLength();
   }
   else if (mAimpointType == 2)
   {
      // length = 4 + mDE_AreaAimpointPtr->GetLength();
   }
   return length;
}

void DisDirectedEnergyAimpoint::Get(GenI& aGenI)
{
   aGenI >> mAimpointType;
   DisUint32 padding;
   aGenI >> padding;
   if (mAimpointType == 1)
   {
      if (mPrecisionBeamAimpointPtr == nullptr)
      {
         mPrecisionBeamAimpointPtr = new DisPrecisionBeamAimpoint();
      }
      mPrecisionBeamAimpointPtr->Get(aGenI);
   }
   else if (mAimpointType == 2)
   {
      // mDE_AreaAimpointPtr->Get(aGenI); //(todo)
   }
}

void DisDirectedEnergyAimpoint::Put(GenO& aGenO)
{
   aGenO << mAimpointType;
   DisUint32 padding = 0;
   aGenO << padding;
   if (mAimpointType == 1)
   {
      mPrecisionBeamAimpointPtr->Put(aGenO);
   }
   else if (mAimpointType == 2)
   {
      // mDE_AreaAimpointPtr->Put(aGenO);
   }
}

void DisDirectedEnergyAimpoint::SetPrecisionAimpoint(DisPrecisionBeamAimpoint* aPrecisionBeamAimpointPtr)
{
   if ((mAimpointType == 1) && (mPrecisionBeamAimpointPtr != nullptr))
   {
      delete mPrecisionBeamAimpointPtr;
   }
   // else if ... todo
   mAimpointType             = 1;
   mPrecisionBeamAimpointPtr = aPrecisionBeamAimpointPtr;
}
