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

#ifndef WSFTASKRESOURCE_HPP
#define WSFTASKRESOURCE_HPP

#include "wsf_export.h"

#include <cassert>

#include "WsfStringId.hpp"

enum // Reserve 0-2 for wsf
{
   cTASK_RESOURCE_TYPE_NONE      = 0, //!< resource is not set
   cTASK_RESOURCE_TYPE_SENSOR    = 1, //!< resource is a sensor
   cTASK_RESOURCE_TYPE_PROCESSOR = 2, //!< resource is a processor
};

//! Attributes of a task-able resource.
class WSF_EXPORT WsfTaskResource
{
public:
   WsfTaskResource()
      : mType(cTASK_RESOURCE_TYPE_NONE)
   {
   }
   WsfTaskResource(int aResourceType)
      : mType(aResourceType)
   {
      // Ensure that mType identifies the correct c++ class.
      // These types have no specialized c++ type:
      assert(aResourceType == cTASK_RESOURCE_TYPE_NONE || aResourceType == cTASK_RESOURCE_TYPE_SENSOR ||
             aResourceType == cTASK_RESOURCE_TYPE_PROCESSOR);
   }
   WsfTaskResource(const WsfTaskResource& aSrc) = default;
   virtual ~WsfTaskResource()                   = default;

   virtual WsfTaskResource* Clone() const;

   virtual bool IsMatch(const WsfTaskResource& aRhs) const;

   virtual WsfStringId GetTypeName() const;

   //! For XIO (de)serialization.
   template<typename AR>
   void Serialize(AR& aAr)
   {
      aAr& mNameId& mType& mModeNameId;
   }

   //! The name of the assigned resource (sensor, processor, etc) on the assigned platform (string ID).
   WsfStringId mNameId;

   //! The name of the mode of the assigned resource.
   WsfStringId mModeNameId;

   int GetType() const { return mType; }

protected:
   // Constructor to bypass the resource type check
   WsfTaskResource(int aResourceType, bool /*aIsSpecialization*/)
      : mType(aResourceType)
   {
   }

   //! The type of resource.
   int mType;

private:
   // disable operator=
   void operator=(const WsfTaskResource& aRhs) = delete;
};

#endif
