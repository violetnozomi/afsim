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

#ifndef WSFSCRIPTPLATFORMLISTCLASS_HPP
#define WSFSCRIPTPLATFORMLISTCLASS_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"
#include "WsfCommandChain.hpp"

//! This script class represents a list of platforms from a command chain.
//! An object of this type is returned by WsfPlatform methods Peers("...") and
//! Subordinates("...").
class WSF_EXPORT WsfScriptPlatformListClass : public UtScriptClass
{
public:
   WsfScriptPlatformListClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void         Destroy(void* aObjectPtr) override;
   std::string  ToString(void* aObjectPtr) const override;
   int          EnumerateSize(void* aObjectPtr) override;
   UtScriptData Enumerate(void* aObjectPtr, int aIndex) override;
   UT_DECLARE_SCRIPT_METHOD(Count);
   UT_DECLARE_SCRIPT_METHOD(Empty);
   UT_DECLARE_SCRIPT_METHOD(Entry);
   UT_DECLARE_SCRIPT_METHOD(GetIterator);
};

class WSF_EXPORT WsfScriptPlatformListIteratorClass : public UtScriptClass
{
public:
   WsfScriptPlatformListIteratorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptPlatformListIteratorClass() override;

   void Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(HasNext);
   UT_DECLARE_SCRIPT_METHOD(Next);
   UT_DECLARE_SCRIPT_METHOD(HasPrev);
   UT_DECLARE_SCRIPT_METHOD(Prev);
   UT_DECLARE_SCRIPT_METHOD(Key);
   UT_DECLARE_SCRIPT_METHOD(Data);

   //! Iterator for a platform list.
   class Iterator
   {
   public:
      Iterator(const WsfCommandChain::PlatformList* aPlatformListPtr);
      virtual ~Iterator();

      virtual bool         HasNext();
      virtual WsfPlatform* Next();

      virtual bool         HasPrev();
      virtual WsfPlatform* Prev();

      virtual int          Key() const;
      virtual WsfPlatform* Data() const;

   private:
      const WsfCommandChain::PlatformList* mPlatformListPtr;
      int                                  mCurrentKey;
      WsfPlatform*                         mCurrentDataPtr;
   };
};

#endif
