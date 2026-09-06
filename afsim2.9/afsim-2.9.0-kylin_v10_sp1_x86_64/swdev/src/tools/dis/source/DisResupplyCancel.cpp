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

#include "DisResupplyCancel.hpp"

#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisResupplyCancel::DisResupplyCancel()
   : DisPdu()
   , mRequestingEntityId()
   , mServicingEntityId()
{
   SetPduType(DisEnum::Pdu::Type::ResupplyCancel);
   SetProtocolFamily(DisEnum::Pdu::Family::Logistics);
   SetLength(24); // 192 bits
}

DisResupplyCancel::DisResupplyCancel(const DisResupplyCancel& aSrc)
   : DisPdu(aSrc)
   , mRequestingEntityId(aSrc.mRequestingEntityId)
   , mServicingEntityId(aSrc.mServicingEntityId)
{
}

DisResupplyCancel::DisResupplyCancel(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisResupplyCancel::DisResupplyCancel(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisResupplyCancel::~DisResupplyCancel() {}

// virtual
DisPdu* DisResupplyCancel::Clone() const
{
   return new DisResupplyCancel(*this);
}

int DisResupplyCancel::GetClass() const
{
   return (int)DisEnum::Pdu::Type::ResupplyCancel;
}

// Virtual
void DisResupplyCancel::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

void DisResupplyCancel::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mRequestingEntityId;
   aGenI >> mServicingEntityId;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisResupplyCancel::Put(GenO& aGenO)
{
   DisPdu::Put(aGenO);
   aGenO << mRequestingEntityId;
   aGenO << mServicingEntityId;
}

// virtual
void DisResupplyCancel::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);

   aStream << "      RequestEntityId = " << GetRequestingEntityId() << std::endl;
   aStream << "      ServicingEntity = " << GetServicingEntityId() << std::endl;

   aStream << "      === End DisResupplyCancel ===" << std::endl << std::endl;
}

bool DisResupplyCancel::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mRequestingEntityId.IsValid())
      isValid = false;
   if (!mServicingEntityId.IsValid())
      isValid = false;
   return isValid;
}
