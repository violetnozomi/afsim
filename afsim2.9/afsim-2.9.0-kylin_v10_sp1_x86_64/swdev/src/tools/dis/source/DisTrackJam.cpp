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

#include "DisTrackJam.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "Dis.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisTrackJam::DisTrackJam()
   : mEntityId()
   , mEmitterNumber(0)
   , mBeamNumber(0)
{
}

DisTrackJam::DisTrackJam(const DisTrackJam& aSrc)
   : mEntityId(aSrc.mEntityId)
   , mEmitterNumber(aSrc.mEmitterNumber)
   , mBeamNumber(aSrc.mBeamNumber)
{
}

DisTrackJam::DisTrackJam(const DisEntityId& aEntity, DisUint8 aEmitter, DisUint8 aBeam)
   : mEntityId(aEntity)
   , mEmitterNumber(aEmitter)
   , mBeamNumber(aBeam)
{
}

void DisTrackJam::Get(GenI& aGenI)
{
   aGenI >> mEntityId;
   aGenI >> mEmitterNumber;
   aGenI >> mBeamNumber;
}

void DisTrackJam::Put(GenO& aGenO) const
{
   aGenO << mEntityId;
   aGenO << mEmitterNumber;
   aGenO << mBeamNumber;
}

DisTrackJam& DisTrackJam::operator=(const DisTrackJam& aRhs)
{
   if (this != &aRhs)
   {
      mEntityId      = aRhs.mEntityId;
      mEmitterNumber = aRhs.mEmitterNumber;
      mBeamNumber    = aRhs.mBeamNumber;
   }
   return *this;
}

bool DisTrackJam::operator<(const DisTrackJam& aRhs) const
{
   // DisEntityId only defines operator< and operator==.
   return (mEntityId < aRhs.mEntityId)            ? true :
          (mEntityId != aRhs.mEntityId)           ? false :
          (mEmitterNumber < aRhs.mEmitterNumber)  ? true :
          (mEmitterNumber != aRhs.mEmitterNumber) ? false :
                                                    mBeamNumber < aRhs.mBeamNumber;
}

bool DisTrackJam::IsValid() const
{
   // Must refer to a single DIS Entity ID.
   if (!mEntityId.IsSingle())
   {
      return false;
   }
   // If not referring to an emitter, referring to a beam number is invalid
   if (mEmitterNumber == Dis::cNO_EMITTER && mBeamNumber != Dis::cNO_BEAM)
   {
      return false;
   }

   return true;
}

void DisTrackJam::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisTrackJam::ToString() const
{
   using namespace std;
   std::ostringstream ss("");
   ss << "***** Track/Jam Target Data *****" << '\n'
      << "Entity ID:      " << GetEntityId().ToString() << '\n'
      << "Emitter Number: " << static_cast<DisUint16>(GetEmitterNumber()) << '\n'
      << "Beam Number:    " << static_cast<DisUint16>(GetBeamNumber());
   return ss.str();
}
