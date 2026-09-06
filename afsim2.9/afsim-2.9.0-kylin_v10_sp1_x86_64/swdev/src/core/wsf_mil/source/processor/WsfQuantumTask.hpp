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

#ifndef WSFQUANTUMTASK_HPP
#define WSFQUANTUMTASK_HPP


class UtScriptClass;
class UtScriptTypes;

#include "wsf_mil_export.h"

#include <memory>

#include "WsfTask.hpp"
#include "WsfTaskResource.hpp"
class WsfTrack;


////not in use yet, need to figure out inheritance issues between WsfTask and WsfQuantumTask
// class WsfQuantumTaskId
//{
//    public:
//
//       WsfQuantumTaskId(WsfStringId aTaskType,
//                        WsfStringId aTargetName,
//                        int         aResourceType)
//          : mTaskType(aTaskType),
//            mTargetName(aTargetName),
//            mResourceType(aResourceType) {}
//
//       bool operator==(const WsfQuantumTaskId& aRhs) const
//       {
//          return ((mTaskType     == aRhs.mTaskType) &&
//                  (mTargetName   == aRhs.mTargetName) &&
//                  (mResourceType == aRhs.mResourceType));
//       }
//
//       bool operator!=(const WsfQuantumTaskId& aRhs) const
//       {
//          return (! operator==(aRhs));
//       }
//
//       bool operator<(const WsfQuantumTaskId& aRhs) const
//       {
//          if (mTaskType < aRhs.mTaskType)
//          {
//             return true;
//          }
//          else if (mTaskType > aRhs.mTaskType)
//          {
//             return false;
//          }
//          else if (mTargetName < aRhs.mTargetName)
//          {
//             return true;
//          }
//          else if (mTargetName > aRhs.mTargetName)
//          {
//             return false;
//          }
//          return (mResourceType < aRhs.mResourceType);
//       }
//
//    private:
//
//       WsfStringId mTaskType;
//       WsfStringId mTargetName;
//       int         mResourceType;
// };


//! WsfQuantumTask adds two data fields to a WsfTask: priority and unique id
//! both are able to be set by the user, so technically the unique id might not be unique
//! by default the unique id is a combination of the target track id and the task resource type
class WSF_MIL_EXPORT WsfQuantumTask : public WsfTask
{
public:
   WsfQuantumTask(double                 aPriority = 1.0,
                  const WsfTaskResource& aResource = std::move(WsfTaskResource()),
                  WsfTrack*              aTrackPtr = nullptr);
   WsfQuantumTask(const WsfQuantumTask& aSrc) = default;
   WsfQuantumTask& operator=(const WsfQuantumTask&) = delete;
   ~WsfQuantumTask() override                       = default;
   virtual WsfQuantumTask* Clone() const;

   const char*                           GetScriptClassName() const override { return "WsfQuantumTask"; }
   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   double GetPriority() { return mPriority; }
   void   SetPriority(double aVal) { mPriority = aVal; }

   bool operator==(const WsfQuantumTask& aRhs) const
   {
      return (this->GetTargetName() == aRhs.GetTargetName() && this->GetResourceType() == aRhs.GetResourceType());
   }

   void SetUniqueId(unsigned int aUniqueId)
   {
      mUniqueId = aUniqueId;
      mTaskId   = mUniqueId;
   }

   // unsigned int UniqueId()
   //{
   //    if (mUniqueId == 0)
   //    {
   //       mUniqueId = UniqueId(*(WsfTask*)this);
   //       mTaskId = mUniqueId;
   //    }
   //    return mUniqueId;
   // }

   unsigned int GetUniqueId() const
   {
      if (mUniqueId > 0)
      {
         return mUniqueId;
      }
      else
      {
         return GetUniqueId(*(WsfTask*)this);
      }
   }

   static unsigned int GetUniqueId(WsfTask& aTask)
   {
      unsigned int id = 0;
      // TODO: Rework this code.  it depends on WsfStringId::UnsafeGetNumber, and assumes <=13 bit dictionary ID's
      id |= ReverseBits((unsigned int)WsfStringId::UnsafeGetNumber(aTask.GetTaskType())); // pack "left" bits first
      id |= (unsigned int)(WsfStringId::UnsafeGetNumber(aTask.GetTargetName()) *
                           8);                     // pack "right" bits, save for last 3 bits
      id |= (unsigned int)aTask.GetResourceType(); // pack final 3 bits
      return id;
   }

   operator unsigned int() const { return this->GetUniqueId(); }

   // Taken from:  http://graphics.stanford.edu/~seander/bithacks.html#ReverseParallel
   static unsigned int ReverseBits(unsigned int x)
   {
      x = ((x >> 1) & 0x55555555U) | ((x & 0x55555555U) << 1);
      x = ((x >> 2) & 0x33333333U) | ((x & 0x33333333U) << 2);
      x = ((x >> 4) & 0x0f0f0f0fU) | ((x & 0x0f0f0f0fU) << 4);
      x = ((x >> 8) & 0x00ff00ffU) | ((x & 0x00ff00ffU) << 8);
      x = ((x >> 16) & 0x0000ffffU) | ((x & 0x0000ffffU) << 16);
      return x;
   }

private:
   double       mPriority;
   unsigned int mUniqueId;
   // WsfQuantumTaskId mId;
};

#endif
