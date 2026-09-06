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

#include "UtScriptFunction.hpp"

#include "UtScriptClass.hpp"

UtScriptFunction::UtScriptFunction()
   : mFlags(0)
   , mReturnType()
   , mReturnClassPtr()
   , mArgs()
{
}

bool UtScriptFunction::operator<(const UtScriptFunction& aRhs) const
{
   if (mArgs.size() < aRhs.mArgs.size())
      return true;
   if (mArgs.size() > aRhs.mArgs.size())
      return false;
   if (mReturnType < aRhs.mReturnType)
      return true;
   if (mReturnType > aRhs.mReturnType)
      return false;
   if (mFlags < aRhs.mFlags)
      return true;
   if (mFlags > aRhs.mFlags)
      return false;
   for (size_t i = 0; i < mArgs.size(); ++i)
   {
      UtStringId lhs = mArgs[i];
      UtStringId rhs = aRhs.mArgs[i];
      if (lhs < rhs)
         return true;
      if (lhs > rhs)
         return false;
   }
   return false;
}

UtStringId UtScriptFunction::SpecializeType(UtStringId                                aThisType,
                                            std::pair<UtScriptClass*, UtScriptClass*> aArgs,
                                            UtStringId                                aType)
{
   int id = UtStringId::UnsafeGetNumber(aType);
   if (id == -1)
      return aArgs.first ? aArgs.first->GetClassName() : UtStringId::UnsafeFindStringId(-1);
   if (id == -2)
      return aArgs.second ? aArgs.second->GetClassName() : UtStringId::UnsafeFindStringId(-1);
   if (id == -3)
      return aThisType;
   return aType;
}

void UtScriptFunction::Specialize(UtStringId aThisType, std::pair<UtScriptClass*, UtScriptClass*> aTempArgs)
{
   mReturnType = SpecializeType(aThisType, aTempArgs, mReturnType);
   for (size_t i = 0; i < mArgs.size(); ++i)
   {
      mArgs[i] = SpecializeType(aThisType, aTempArgs, mArgs[i]);
   }
}
