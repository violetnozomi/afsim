// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtScriptVector.hpp"

using ut::script::VectorWrapperBase;

VectorWrapperBase::VectorWrapperBase(UtScriptContext& aContext)
   : mVec(new ScriptVector())
   , mContext(&aContext)
   , mOwnsVec(true)
{
}


VectorWrapperBase::VectorWrapperBase(const VectorWrapperBase& aCopy)
   : mVec(new ScriptVector(*aCopy.mVec))
   , mContext(aCopy.mContext)
   , mElementClass(aCopy.mElementClass)
   , mOwnsVec(true)
{
}

VectorWrapperBase::VectorWrapperBase(VectorWrapperBase&& aMove) noexcept
   : mVec(aMove.mVec)
   , mContext(aMove.mContext)
   , mElementClass(aMove.mElementClass)
   , mOwnsVec(aMove.mOwnsVec)
{
   aMove.mOwnsVec = false;
}

VectorWrapperBase::~VectorWrapperBase()
{
   if (mOwnsVec)
   {
      delete mVec;
   }
}

VectorWrapperBase::allocator_type VectorWrapperBase::get_allocator()
{
   return mVec->get_allocator();
}

void VectorWrapperBase::pop_back()
{
   mVec->pop_back();
}

void VectorWrapperBase::reserve(size_type aCapacity)
{
   mVec->reserve(aCapacity);
}

void VectorWrapperBase::resize(size_type aSize)
{
   mVec->resize(aSize);
}

void VectorWrapperBase::shrink_to_fit()
{
   mVec->shrink_to_fit();
}

void VectorWrapperBase::clear() noexcept
{
   mVec->clear();
}

UtScriptData* VectorWrapperBase::data() const noexcept
{
   return mVec->data();
}

void VectorWrapperBase::swap(VectorWrapperBase& aOther) noexcept
{
   std::swap(mVec, aOther.mVec);
   std::swap(mContext, aOther.mContext);
   std::swap(mElementClass, aOther.mElementClass);
   std::swap(mOwnsVec, aOther.mOwnsVec);
}

UtScriptData& VectorWrapperBase::PushBack()
{
   mVec->emplace_back();
   return mVec->back();
}

UtScriptData& VectorWrapperBase::EmplaceAt(UtScriptData* aPos)
{
   auto index = std::distance(mVec->data(), &*aPos);
   mVec->emplace(mVec->begin() + index);
   return (*mVec)[index];
}

VectorWrapperBase& VectorWrapperBase::operator=(const VectorWrapperBase& aOther) noexcept
{
   *mVec         = *aOther.mVec; // deep copy.
   mContext      = aOther.mContext;
   mElementClass = aOther.mElementClass;
   // mOwnsVec = aOther.mOwnsVec; do not copy this over!
   return *this;
}

VectorWrapperBase& VectorWrapperBase::operator=(VectorWrapperBase&& aOther) noexcept
{
   if (mOwnsVec)
   {
      delete mVec;
   }
   mVec            = aOther.mVec;
   mContext        = aOther.mContext;
   mElementClass   = aOther.mElementClass;
   mOwnsVec        = aOther.mOwnsVec;
   aOther.mOwnsVec = false;
   return *this;
}