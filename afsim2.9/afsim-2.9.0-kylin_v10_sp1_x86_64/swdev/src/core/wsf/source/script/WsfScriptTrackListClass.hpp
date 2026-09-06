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

#ifndef WSFSCRIPTTRACKLISTCLASS_HPP
#define WSFSCRIPTTRACKLISTCLASS_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"
class WsfTrack;
#include "WsfTrackListFwd.hpp"

//! WsfScriptTrackListClass is a UtScriptClass that defines the methods in WsfTrackList.
//! This makes the methods available for use in script.

class WSF_EXPORT WsfScriptTrackListClass : public UtScriptClass
{
public:
   WsfScriptTrackListClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~WsfScriptTrackListClass() override;

   // See the base class for a description of Create, Clone, and Destroy.

   void*        Create(const UtScriptContext& aInstance) override;
   void*        Clone(void* aObjectPtr) override;
   void         Destroy(void* aObjectPtr) override;
   int          EnumerateSize(void* aObjectPtr) override;
   UtScriptData Enumerate(void* aObjectPtr, int aIndex) override;

   // Each of the exported methods are defined as function objects.

   UT_DECLARE_SCRIPT_METHOD(Count);
   UT_DECLARE_SCRIPT_METHOD(Empty);
   UT_DECLARE_SCRIPT_METHOD(Entry);
   UT_DECLARE_SCRIPT_METHOD(Find);
   UT_DECLARE_SCRIPT_METHOD(GetIterator);
};

class WSF_EXPORT WsfScriptTrackListIteratorClass : public UtScriptClass
{
public:
   WsfScriptTrackListIteratorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptTrackListIteratorClass() override;

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
      Iterator(const WsfTrackList& aTrackList);
      virtual ~Iterator();

      virtual bool      HasNext();
      virtual WsfTrack* Next();

      virtual bool      HasPrev();
      virtual WsfTrack* Prev();

      virtual int       Key() const;
      virtual WsfTrack* Data() const;

      bool        IteratorValidForAccess() const;
      std::string IteratorErrorString() const;

   private:
      const WsfTrackList& mTrackListRef;
      int                 mCurrentKey;
      int                 mVectorSize;
   };
};

#endif
