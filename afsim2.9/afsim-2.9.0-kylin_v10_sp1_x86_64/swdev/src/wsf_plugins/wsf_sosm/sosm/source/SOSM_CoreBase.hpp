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

#ifndef SOSM_COREBASE_HPP
#define SOSM_COREBASE_HPP

#include "sosm_export.h"

#include <string>

class UtInput;
#include "SOSM_SpectralObject.hpp"

//! The base class for the 'core' objects (sensor, target, atmosphere).
//! It defines the interface for processing input and initializing the core objects.

class SOSM_EXPORT SOSM_CoreBase : public SOSM_SpectralObject
{
public:
   SOSM_CoreBase();
   SOSM_CoreBase(const SOSM_CoreBase& aSrc);
   ~SOSM_CoreBase() override;
   SOSM_CoreBase& operator=(const SOSM_CoreBase&) = delete;

   //! Return the user-specified type name of the object.
   std::string GetTypeName() const { return mTypeName; }

   //! Set the user-specified type name of the object.
   void SetTypeName(const std::string& aTypeName) { mTypeName = aTypeName; }

   //! Return when the source file from which the object was recreated was last modified.
   double GetModificationTime() const { return mModificationTime; }

   //! Set the last modification time of the object.
   void SetModificationTime(double aModificationTime) { mModificationTime = aModificationTime; }

   void UpdateModificationTime(double aModificationTime);

   std::string GetCachePrefix() const;

   //! Set the cache prefix (subdirectory) to be used for creating cache files of this object.
   void SetCachePrefix(const std::string& aCachePrefix) { mCachePrefix = aCachePrefix; }

   virtual bool Initialize();

   virtual void InputComplete();

   virtual bool ProcessInput(UtInput& aInput);

protected:
   //! When the file was last modified.
   double mModificationTime;

   //! The type name of the object.
   std::string mTypeName;

   //! The 'cache prefix' to be used for cache files created for this object.
   //! This is basically the name of the first level subdirectory name within the cache.
   std::string mCachePrefix;
};

#endif
