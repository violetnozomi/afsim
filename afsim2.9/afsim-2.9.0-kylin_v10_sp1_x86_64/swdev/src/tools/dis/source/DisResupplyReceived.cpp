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

#include "DisResupplyReceived.hpp"

#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
const DisUint16 sDisResupplyReceivedSize = (48 + 48 + (8 + 24)) / 8;
}

DisResupplyReceived::DisResupplyReceived()
   : DisPdu()
   , mRequestingEntityId()
   , mServicingEntityId()
   , mSupplyRecordCount(0)
{
   SetPduType(DisEnum::Pdu::Type::ResupplyReceived);
   SetProtocolFamily(DisEnum::Pdu::Family::Logistics);
   GetLength();
}

DisResupplyReceived::DisResupplyReceived(const DisResupplyReceived& aSrc)
   : DisPdu(aSrc)
   , mRequestingEntityId(aSrc.mRequestingEntityId)
   , mServicingEntityId(aSrc.mServicingEntityId)
   , mSupplyRecordCount(aSrc.mSupplyRecordCount)
   , mSupplyRecordList(aSrc.mSupplyRecordList)
{
   GetLength(); // Ensure consistent length
}

DisResupplyReceived::DisResupplyReceived(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisResupplyReceived::DisResupplyReceived(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisResupplyReceived::~DisResupplyReceived()
{
   RemoveAllSupplyRecords();
}

// virtual
DisPdu* DisResupplyReceived::Clone() const
{
   return new DisResupplyReceived(*this);
}

// virtual
const DisEntityId& DisResupplyReceived::GetOriginatingEntity() const
{
   return mRequestingEntityId;
}

int DisResupplyReceived::GetClass() const
{
   return (int)DisEnum::Pdu::Type::ResupplyReceived;
}

// Virtual
void DisResupplyReceived::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

void DisResupplyReceived::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   DisUint8  pad8;
   DisUint16 pad16;

   RemoveAllSupplyRecords();

   aGenI >> mRequestingEntityId;
   aGenI >> mServicingEntityId;
   aGenI >> mSupplyRecordCount;

   aGenI >> pad16;
   aGenI >> pad8;

   if (mSupplyRecordCount != 0)
   {
      mSupplyRecordList.Reserve(mSupplyRecordCount);
      for (int i = 0; i < mSupplyRecordCount; ++i)
      {
         DisSupplyRecord* ptr = new DisSupplyRecord();
         aGenI >> *ptr;
         mSupplyRecordList.Add(ptr);
      }
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

DisUint16 DisResupplyReceived::GetLength()
{
   DisUint16 byteCount =
      DisPdu::GetBaseLength() + sDisResupplyReceivedSize + mSupplyRecordCount * DisSupplyRecord::GetLength();
   SetLength(byteCount);
   return byteCount;
}

void DisResupplyReceived::Put(GenO& aGenO)
{
   DisUint8  pad8  = 0;
   DisUint16 pad16 = 0;

   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mRequestingEntityId;
   aGenO << mServicingEntityId;
   aGenO << mSupplyRecordCount;
   aGenO << pad16;
   aGenO << pad8;

   std::vector<DisSupplyRecord*>::const_iterator iter(mSupplyRecordList.GetBegin());
   while (iter != mSupplyRecordList.GetEnd())
   {
      aGenO << **iter++;
   }
}

bool DisResupplyReceived::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mRequestingEntityId.IsValid())
      isValid = false;
   if (!mServicingEntityId.IsValid())
      isValid = false;

   std::vector<DisSupplyRecord*>::const_iterator iter(mSupplyRecordList.GetBegin());
   while (iter != mSupplyRecordList.GetEnd())
   {
      if (!(*iter++)->IsValid())
         isValid = false;
   }

   return isValid;
}

// virtual
void DisResupplyReceived::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);

   aStream << "      RequestEntityId = " << GetRequestingEntityId() << std::endl;
   aStream << "      ServicingEntity = " << GetServicingEntityId() << std::endl;

   aStream << "      => Record Count = " << static_cast<int>(mSupplyRecordCount) << std::endl;

   std::vector<DisSupplyRecord*>::const_iterator iter(mSupplyRecordList.GetBegin());
   while (iter != mSupplyRecordList.GetEnd())
   {
      (*iter++)->Stream(aStream);
      aStream << std::endl;
   }
   aStream << "      === End DisResupplyReceived ===" << std::endl << std::endl;
}
