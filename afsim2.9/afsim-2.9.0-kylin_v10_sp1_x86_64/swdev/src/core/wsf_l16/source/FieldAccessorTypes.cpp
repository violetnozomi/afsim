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
#include "FieldAccessorTypes.hpp"

#include "Messages.hpp"

namespace WsfL16
{
namespace Messages
{

void MessageAccessorFiller::ProcessMessage(Messages::Base* aMessage)
{
   mFieldIndex = 0;
   mMessagePtr = aMessage;
   MessageAccessorType accessorType;
   mMessageAccessor              = &accessorType;
   mMessageAccessor->mWordType   = aMessage->GetWordType();
   int uid                       = aMessage->GetUID();
   mMessageAccessor->mLabel      = uid / 100000;
   mMessageAccessor->mSubLabel   = (uid / 1000) % 100;
   mMessageAccessor->mWordNumber = 0;
   if (mMessageAccessor->mWordType == Base::cCONTINUATION)
   {
      mMessageAccessor->mWordNumber = (uid / 10) % 100;
   }
   else if (mMessageAccessor->mWordType == Base::cEXTENSION)
   {
      mMessageAccessor->mWordNumber = (uid % 10) - 1;
   }
   mMessageAccessor->mMessageName = aMessage->GetClassName();
   // Will result in calls to AddField():
   aMessage->CreateAccessor(*this);

   MessageAccessorType::sMessageAccessorTypes[aMessage->GetUID()] = accessorType;
}

void MessageAccessorFiller::AddField(int aOffset, FieldAccessorType* aAccessor)
{
   MessageAccessorType::FieldData data;
   data.mByteOffset   = aOffset;
   data.mName         = mMessagePtr->GetFieldName(mFieldIndex++);
   data.mAccessorType = aAccessor;
   mMessageAccessor->mFields.push_back(data);
};

} // namespace Messages
} // namespace WsfL16
