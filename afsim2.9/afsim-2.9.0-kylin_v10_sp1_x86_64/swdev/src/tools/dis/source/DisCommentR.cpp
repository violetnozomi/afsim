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

#include "DisCommentR.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
const DisUint16 sMySizeMinusData = (48 + 48) / 8;
}

DisCommentR::DisCommentR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mData()
{
   SetPduType(DisEnum::Pdu::Type::CommentR);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   GetLength();
}

DisCommentR::DisCommentR(const DisCommentR& aSrc)
   : DisPdu(aSrc)
{
   Copy(aSrc);
}

DisCommentR::DisCommentR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisCommentR::DisCommentR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisCommentR::~DisCommentR() {}

// virtual
DisPdu* DisCommentR::Clone() const
{
   return new DisCommentR(*this);
}

// virtual
int DisCommentR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::CommentR;
}

// Copy the data portion from one DisCommentR to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisCommentR::Copy(const DisCommentR& aSrc)
{
   mOriginatingEntity = aSrc.mOriginatingEntity;
   mReceivingEntity   = aSrc.mReceivingEntity;
   mData              = aSrc.mData;
}

// virtual
void DisCommentR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisCommentR::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sMySizeMinusData + mData.GetLength();
   SetLength(byteCount);
   return byteCount;
}

// private
void DisCommentR::GetMemberData(GenI& aGenI)
{
   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   mData.Get(aGenI);
}

// virtual
void DisCommentR::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   mData.Put(aGenO);
}

DisCommentR& DisCommentR::operator=(const DisCommentR& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      Copy(aRhs);
   }
   return *this;
}

// virtual
const DisEntityId& DisCommentR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisCommentR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisCommentR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisCommentR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisCommentR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   mData.Stream(aStream);
   aStream << "      === End DisCommentR ===" << std::endl << std::endl;
}
