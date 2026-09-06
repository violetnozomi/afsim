// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "VaDefs.hpp"

#include <algorithm>

#include "VaAttachment.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"

namespace vespa
{
VaEntity* VaHitEntry::FindEntity() const
{
   if (mEntityId == 0)
      return nullptr;
   return VaEnvironment::Instance().FindEntity(mEntityId);
}
VaAttachment* VaHitEntry::FindAttachment() const
{
   if (mAttachmentId == 0)
      return nullptr;
   return VaEnvironment::Instance().FindAttachment(mAttachmentId);
}
VaObject* VaHitEntry::FindObject() const
{
   if (mAttachmentId == 0)
      return FindEntity();
   return FindAttachment();
}

vespa::VaHitEntry VaHitEntry::FromAttachment(VaAttachment&     aAttachment,
                                             VaAttachmentSubId aSubId /*= VaAttachmentSubId()*/)
{
   return FromAttachment(aAttachment.GetParent().GetUniqueId(), aAttachment.GetUniqueId(), aSubId);
}

vespa::VaHitEntry VaHitEntry::FromEntity(const VaEntity& aEntity)
{
   VaHitEntry entry;
   entry.mEntityId = aEntity.GetUniqueId();
   return entry;
}


void CollectEntities(const VaHitSet& aHits, VaEntityMap& aEntities, bool aExcludeAttachments)
{
   for (VaHitSet::const_iterator i = aHits.begin(); i != aHits.end(); ++i)
   {
      if ((!aExcludeAttachments) || (i->AttachmentId() == 0))
      {
         VaEntity* entityPtr = i->FindEntity();
         if (entityPtr)
         {
            aEntities[entityPtr->GetUniqueId()] = entityPtr;
         }
      }
   }
}

void CollectAttachments(const VaHitSet& aHits, VaAttachmentList& aAttachments)
{
   for (VaHitSet::const_iterator i = aHits.begin(); i != aHits.end(); ++i)
   {
      VaAttachment* attachmentPtr = i->FindAttachment();
      if (attachmentPtr)
      {
         aAttachments.push_back(attachmentPtr);
      }
   }
   std::sort(aAttachments.begin(), aAttachments.end());
   aAttachments.erase(std::unique(aAttachments.begin(), aAttachments.end()), aAttachments.end());
}

VaEntity* GetSingleEntityHit(const VaHitSet& aHits)
{
   VaEntity* hitEntity = nullptr;
   for (VaHitSet::const_iterator i = aHits.begin(); i != aHits.end(); ++i)
   {
      if (!i->IsEntity())
         continue;

      VaEntity* entityPtr = i->FindEntity();
      if (entityPtr)
      {
         if (hitEntity)
            return nullptr;
         hitEntity = entityPtr;
      }
   }
   return hitEntity;
}

VaEntity* GetFirstEntityHit(const VaHitSet& aHits)
{
   for (VaHitSet::const_iterator i = aHits.begin(); i != aHits.end(); ++i)
   {
      if (!i->IsEntity())
         continue;

      VaEntity* entityPtr = i->FindEntity();
      if (entityPtr)
      {
         return entityPtr;
      }
   }
   return nullptr;
}

VaEntity* GetFirstEntityHit(const VaHitList& aHits)
{
   for (VaHitList::const_iterator i = aHits.begin(); i != aHits.end(); ++i)
   {
      if (!i->IsEntity())
         continue;

      VaEntity* entityPtr = i->FindEntity();
      if (entityPtr)
      {
         return entityPtr;
      }
   }
   return nullptr;
}

VaAttachment* GetSingleAttachmentHit(const VaHitSet& aHits)
{
   VaAttachment* hitAttachment = nullptr;
   for (VaHitSet::const_iterator i = aHits.begin(); i != aHits.end(); ++i)
   {
      VaAttachment* attachmentPtr = i->FindAttachment();
      if (attachmentPtr)
      {
         if (hitAttachment && hitAttachment != attachmentPtr)
            return nullptr;
         hitAttachment = attachmentPtr;
      }
   }
   return hitAttachment;
}

VaAttachment* GetFirstAttachmentHit(const VaHitSet& aHits)
{
   for (VaHitSet::const_iterator i = aHits.begin(); i != aHits.end(); ++i)
   {
      VaAttachment* attachmentPtr = i->FindAttachment();
      if (attachmentPtr)
      {
         return attachmentPtr;
      }
   }
   return nullptr;
}

VaAttachment* GetFirstAttachmentHit(const VaHitList& aHits)
{
   for (VaHitList::const_iterator i = aHits.begin(); i != aHits.end(); ++i)
   {
      VaAttachment* attachmentPtr = i->FindAttachment();
      if (attachmentPtr)
      {
         return attachmentPtr;
      }
   }
   return nullptr;
}

VaHitSet::iterator LowerBoundAttachment(VaHitSet& aHitSet, VaAttachment* aAttachmentPtr)
{
   VaHitEntry entry = VaHitEntry::FromAttachment(*aAttachmentPtr, 0xffffffff);
   return aHitSet.lower_bound(entry);
}
VaHitSet::const_iterator VESPATK_EXPORT LowerBoundAttachment(const VaHitSet& aHitSet, VaAttachment* aAttachmentPtr)
{
   VaHitEntry entry = VaHitEntry::FromAttachment(*aAttachmentPtr, 0xffffffff);
   return aHitSet.lower_bound(entry);
}
VESPATK_EXPORT void RemoveAttachmentHits(VaHitSet& aHitSet, VaAttachment* aAttachmentPtr)
{
   VaHitSet::iterator i = LowerBoundAttachment(aHitSet, aAttachmentPtr);
   while (i != aHitSet.end() && i->AttachmentId() == aAttachmentPtr->GetUniqueId())
   {
      aHitSet.erase(i++);
   }
}

VESPATK_EXPORT void AttachmentHits(VaAttachment* aAttachmentPtr, const VaHitSet& aHitSet, VaHitSet& aAttachmentHits)
{
   VaHitSet::const_iterator i = LowerBoundAttachment(aHitSet, aAttachmentPtr);
   while (i != aHitSet.end() && i->AttachmentId() == aAttachmentPtr->GetUniqueId())
   {
      aAttachmentHits.insert(*i);
   }
}


} // end namespace vespa
