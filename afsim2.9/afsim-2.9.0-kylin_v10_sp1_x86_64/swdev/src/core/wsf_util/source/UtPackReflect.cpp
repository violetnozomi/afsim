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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "UtPackReflect.hpp"

#include <cstddef>

void UtPackReflector::AddMember(const char*    aTypeName,
                                const char*    aMemberName,
                                char*          aMemberPtr,
                                unsigned short aMemberSize,
                                int            aOptionIndex)
{
   UtPackStructLayout::Member m;
   m.mMemberSize    = aMemberSize;
   m.mOffset        = ut::safe_cast<int, ptrdiff_t>(aMemberPtr - mCurrentInstancePtr);
   m.mName          = aMemberName;
   m.mTypeName      = aTypeName;
   m.mOptionalIndex = aOptionIndex;
   mCurrentLayoutPtr->mMembers.push_back(m);
}

void UtPackReflector::End()
{
   assert(mCurrentInstancePtr);
   mCurrentInstancePtr = nullptr;
   // at this point mCurrentLayoutPtr is passed to mSerializerPtr for management
   mSerializerPtr->AddStructLayout(mCurrentLayoutPtr);
}

UtPackReflector::UtPackReflector(UtPackSerializer& aSerializer)
   : mCurrentLayoutPtr(nullptr)
   , mCurrentInstancePtr(nullptr)
   , mSerializerPtr(&aSerializer)
   , mNextOptionIndex(0)
{
}
