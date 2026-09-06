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

#include "DisEventId.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisUint16 DisEventId::mNextNumber = 0;

DisEventId::DisEventId(const DisEventId& aSrc)
   : mSite(aSrc.mSite)
   , mApplication(aSrc.mApplication)
   , mNumber(aSrc.mNumber)
{
}

DisEventId::DisEventId(DisUint16 aSite, DisUint16 aApplication, DisUint16 aNumber)
   : mSite(aSite)
   , mApplication(aApplication)
   , mNumber(aNumber)
{
}

DisEventId* DisEventId::Clone() const
{
   return new DisEventId(*this);
}

// Set the next event number to be assigned by 'AssignNewNumber()'.
// static
void DisEventId::SetNextNumber(DisUint16 aNextNumber)
{
   mNextNumber = aNextNumber;
}

// Assigns the next sequential Event number.  Site and Application
// are not modified.
void DisEventId::AssignNewNumber()
{
   while ((mNextNumber == 0x0000) || (mNextNumber == 0xFFFF))
   {
      ++mNextNumber;
   }
   mNumber = mNextNumber;
   ++mNextNumber;
}

void DisEventId::Get(GenI& aGenI)
{
   aGenI >> mSite;
   aGenI >> mApplication;
   aGenI >> mNumber;
}

void DisEventId::Put(GenO& aGenO) const
{
   aGenO << mSite;
   aGenO << mApplication;
   aGenO << mNumber;
}

bool DisEventId::operator<(const DisEventId& aRhs) const
{
   bool isLess = true;

   if (mSite > aRhs.mSite)
   {
      isLess = false;
   }
   else if (mSite == aRhs.mSite)
   {
      if (mApplication > aRhs.mApplication)
      {
         isLess = false;
      }
      else if (mApplication == aRhs.mApplication)
      {
         if (mNumber >= aRhs.mNumber)
         {
            isLess = false;
         }
      }
   }
   return isLess;
}

void DisEventId::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisEventId::ToString() const
{
   return std::to_string(mSite) + ':' + std::to_string(mApplication) + ':' + std::to_string(mNumber);
}

bool DisEventId::IsValid() const
{
   return true;
}
