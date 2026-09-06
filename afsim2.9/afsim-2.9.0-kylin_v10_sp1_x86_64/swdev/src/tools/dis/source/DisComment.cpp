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

#include "DisComment.hpp"

#include <cstring>  // for memcpy
#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
const DisUint16 sDisCommentSize = (48 + 48 + 32 + 32) / 8;
}

DisComment::DisComment()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mNumFixedRecords(0)
   , mNumVariableRecords(0)
{
   SetPduType(DisEnum::Pdu::Type::Comment);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   GetLength();
}

DisComment::DisComment(const DisComment& aSrc)
   : DisPdu(aSrc)
{
   Copy(aSrc);
}

DisComment::DisComment(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisComment::DisComment(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisComment::~DisComment()
{
   DeleteClassData();
}

// virtual
DisPdu* DisComment::Clone() const
{
   return new DisComment(*this);
}

// virtual
int DisComment::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Comment;
}

// Copy the data portion from one DisComment to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisComment::Copy(const DisComment& aSrc)
{
   mOriginatingEntity  = aSrc.mOriginatingEntity;
   mReceivingEntity    = aSrc.mReceivingEntity;
   mNumFixedRecords    = aSrc.mNumFixedRecords;
   mNumVariableRecords = aSrc.mNumVariableRecords;

   unsigned int i;

   for (i = 0; i < mNumVariableRecords; ++i)
   {
      const VariableRecord*      vr = aSrc.GetVariableRecord(i);
      DisComment::VariableRecord pb;
      pb.mVariableDatumId       = vr->mVariableDatumId;
      pb.mVariableDatumLength   = vr->mVariableDatumLength;
      pb.mVariablePaddingLength = vr->mVariablePaddingLength;
      pb.mVariableDatumValue    = new char[vr->mVariableDatumLength / 8];
      memcpy(pb.mVariableDatumValue, vr->mVariableDatumValue, vr->mVariableDatumLength / 8);
      mVariableRecords.push_back(pb);
   }
}

const DisComment::VariableRecord* DisComment::GetVariableRecord(const unsigned int i) const
{
   if (i < mNumVariableRecords)
   {
      return &(mVariableRecords[i]);
   }
   else
   {
      return nullptr;
   }
}

// virtual
void DisComment::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisComment::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisCommentSize;

   std::vector<VariableRecord>::iterator it;

   for (it = mVariableRecords.begin(); it != mVariableRecords.end(); ++it)
   {
      byteCount += 8 + static_cast<DisUint16>((it->mVariableDatumLength + it->mVariablePaddingLength) / 8);
   }

   SetLength(byteCount);
   return byteCount;
}

// private
void DisComment::GetMemberData(GenI& aGenI)
{
   DeleteClassData(); // Release existing class-owned data

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mNumFixedRecords;
   aGenI >> mNumVariableRecords;

   unsigned int i;
   unsigned int j;

   for (i = 0; i < mNumVariableRecords; ++i)
   {
      unsigned char  dummy;
      char*          buffer;
      VariableRecord vr;
      aGenI >> vr.mVariableDatumId;
      aGenI >> vr.mVariableDatumLength;
      buffer = new char[vr.mVariableDatumLength / 8];
      aGenI.GetArray(buffer, vr.mVariableDatumLength / 8);
      vr.mVariableDatumValue    = buffer;
      vr.mVariablePaddingLength = 64 - (vr.mVariableDatumLength % 64);
      for (j = 0; j < vr.mVariablePaddingLength / 8; ++j)
      {
         aGenI >> dummy;
      }
      mVariableRecords.push_back(vr);
   }
}


const char* DisComment::GetData(const unsigned int aIndex, unsigned int& aDataLength) const
{
   if (aIndex < mNumVariableRecords)
   {
      aDataLength = mVariableRecords[aIndex].mVariableDatumLength;
      return mVariableRecords[aIndex].mVariableDatumValue;
   }
   else
   {
      aDataLength = 0;
      return nullptr;
   }
}

void DisComment::AddVariableRecord(unsigned char* aDataPtr, unsigned int aDataLength)
{
   VariableRecord vr;
   vr.mVariableDatumId     = mNumVariableRecords;
   vr.mVariableDatumLength = aDataLength;
   vr.mVariableDatumValue  = new char[vr.mVariableDatumLength / 8];
   memcpy(vr.mVariableDatumValue, aDataPtr, aDataLength / 8);
   vr.mVariablePaddingLength = 64 - (vr.mVariableDatumLength % 64);
   mVariableRecords.push_back(vr);
   mNumVariableRecords++;
}

// virtual
void DisComment::Put(GenO& aGenO)
{
   DisUint8     dummy = 0;
   unsigned int i;

   GetLength(); // Ensure length is up-to-date

   DisPdu::Put(aGenO);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mNumFixedRecords;
   aGenO << mNumVariableRecords;

   std::vector<VariableRecord>::iterator it;

   for (it = mVariableRecords.begin(); it != mVariableRecords.end(); ++it)
   {
      aGenO << it->mVariableDatumId;
      aGenO << it->mVariableDatumLength;
      aGenO.PutRaw(it->mVariableDatumValue, it->mVariableDatumLength / 8);
      for (i = 0; i < it->mVariablePaddingLength / 8; ++i)
      {
         aGenO << dummy;
      }
   }
}

DisComment& DisComment::operator=(const DisComment& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      DeleteClassData();
      Copy(aRhs);
   }
   return *this;
}

void DisComment::DeleteClassData()
{
   std::vector<VariableRecord>::iterator it;

   for (it = mVariableRecords.begin(); it != mVariableRecords.end(); ++it)
   {
      delete[] it->mVariableDatumValue;
   }
   mVariableRecords.clear();
   mNumVariableRecords = 0;
}

// virtual
const DisEntityId& DisComment::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisComment::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisComment::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisComment::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisComment::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => Fixed Records    = " << mNumFixedRecords << std::endl;
   aStream << "      => Variable Records = " << mNumVariableRecords << std::endl;
   for (unsigned int i = 0; i != mNumVariableRecords; ++i)
   {
      unsigned int byteLength = (mVariableRecords[i].mVariableDatumLength + 7) / 8;
      aStream << "         => ";
      for (unsigned int j = 0; j < byteLength; ++j)
      {
         aStream << mVariableRecords[i].mVariableDatumValue[j];
      }
      aStream << std::endl;
   }
   aStream << "      === End DisComment ===" << std::endl << std::endl;
}

bool DisComment::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mOriginatingEntity.IsValid())
      isValid = false;
   if (!mReceivingEntity.IsValid())
      isValid = false;
   return isValid;
}
