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
#ifndef VaDefs_hpp
#define VaDefs_hpp

#include <list>
#include <map>
#include <set>
#include <vector>

#include "UtVariant.hpp"
#include "VaFwd.hpp"
#include "VaScenarioKey.hpp"

#ifdef BadValue
#undef BadValue
#endif

namespace vespa
{
enum Keys
{
   Key_Escape     = 0x01000000,
   Key_Delete     = 0x01000007,
   Key_Home       = 0x01000010,
   Key_Left       = 0x01000012,
   Key_Up         = 0x01000013,
   Key_Right      = 0x01000014,
   Key_Down       = 0x01000015,
   Key_Shift      = 0x01000020,
   Key_Control    = 0x01000021,
   Key_Alt        = 0x01000023,
   Key_F1         = 0x01000030,
   Key_F2         = 0x01000031,
   Key_F3         = 0x01000032,
   Key_F4         = 0x01000033,
   Key_F5         = 0x01000034,
   Key_F6         = 0x01000035,
   Key_F7         = 0x01000036,
   Key_F8         = 0x01000037,
   Key_F9         = 0x01000038,
   Key_F10        = 0x01000039,
   Key_F11        = 0x0100003a,
   Key_F12        = 0x0100003b,
   Key_Space      = 0x20,
   Key_Exclam     = 0x21,
   Key_NumberSign = 0x23,
   Key_Dollar     = 0x24,
   Key_Percent    = 0x25,
   Key_Ampersand  = 0x26,
   Key_ParenLeft  = 0x28,
   Key_ParenRight = 0x29,
   Key_Asterisk   = 0x2a,
   Key_Plus       = 0x2b,
   Key_Comma      = 0x2c,
   Key_Minus      = 0x2d,
   Key_Period     = 0x2e,
   Key_Equal      = 0x3d,
   Key_0          = 0x30,
   Key_1          = 0x31,
   Key_2          = 0x32,
   Key_3          = 0x33,
   Key_4          = 0x34,
   Key_5          = 0x35,
   Key_6          = 0x36,
   Key_7          = 0x37,
   Key_8          = 0x38,
   Key_9          = 0x39,
   Key_At         = 0x40,
   Key_A          = 0x41,
   Key_B          = 0x42,
   Key_C          = 0x43,
   Key_D          = 0x44,
   Key_E          = 0x45,
   Key_F          = 0x46,
   Key_G          = 0x47,
   Key_H          = 0x48,
   Key_I          = 0x49,
   Key_J          = 0x4a,
   Key_K          = 0x4b,
   Key_L          = 0x4c,
   Key_M          = 0x4d,
   Key_N          = 0x4e,
   Key_O          = 0x4f,
   Key_P          = 0x50,
   Key_Q          = 0x51,
   Key_R          = 0x52,
   Key_S          = 0x53,
   Key_T          = 0x54,
   Key_U          = 0x55,
   Key_V          = 0x56,
   Key_W          = 0x57,
   Key_X          = 0x58,
   Key_Y          = 0x59,
   Key_Z          = 0x5a,
   Key_Caret      = 0x5e
};

enum SwipeDirection
{
   NoDirection,
   Left,
   Right,
   Up,
   Down
};

typedef std::list<VaEntity*>                 VaEntityList;
typedef std::map<int, VaEntity*>             VaEntityMap;
typedef std::map<int, VaAttachment*>         VaAttachmentMap;
typedef std::vector<VaAttachment*>           VaAttachmentList;
typedef std::map<VaScenarioKey, VaScenario*> VaScenarioList;

// A sub-VaAttachment identifier
// When mSubId is zero, this represents the entire attachment.
// Otherwise, this is a unique sub-part of an attachment
struct VaAttachmentSubId
{
   VaAttachmentSubId()
      : mSubId(0)
   {
   }
   VaAttachmentSubId(int aSubId)
      : mSubId(aSubId)
   {
   }
   bool operator==(const VaAttachmentSubId& aRhs) const { return mSubId == aRhs.mSubId && mAuxData == aRhs.mAuxData; }
   bool operator!=(const VaAttachmentSubId& aRhs) const { return !operator==(aRhs); }
   int  mSubId;
   std::vector<UtVariant> mAuxData;
};

// A reference to a VaEntity or VaAttachment / sub attachment
struct VESPATK_EXPORT VaHitEntry
{
   VaHitEntry()
      : mEntityId(0)
      , mAttachmentId(0)
      , mSubId(0)
   {
   }

   static VaHitEntry FromEntity(const VaEntity& aEntity);
   static VaHitEntry FromAttachment(VaAttachment& aAttachment, VaAttachmentSubId aSubId = VaAttachmentSubId());

   // Does this refer to a whole entity, not just an attachment
   bool IsEntity() const { return mEntityId != 0 && mAttachmentId == 0; }
   // Does this refer to an entity's attachment (FindEntity() still reports attachment's owner)
   bool IsAttachment() const { return mAttachmentId != 0; }

   // Returns the entity to which this hit refers, or the attachment's owner.  Returns null if the object does not exist.
   VaEntity* FindEntity() const;
   // Returns the attachment to which this hit refers.  Returns null if the object does not exist.
   VaAttachment* FindAttachment() const;
   // Returns a VaObject to the object this hit refers to.  May be a VaEntity or VaAttachment.  Returns null if the
   // object does not exist.
   VaObject* FindObject() const;

   VaAttachmentSubId GetSubId() const
   {
      VaAttachmentSubId sub(mSubId);
      sub.mAuxData = mAuxData;
      return sub;
   }

   bool IsEmpty() const { return mEntityId == 0 && mAttachmentId == 0; }

   bool operator<(const VaHitEntry& aRhs) const
   {
      if (mEntityId < aRhs.mEntityId)
         return true;
      if (mEntityId > aRhs.mEntityId)
         return false;
      if (mAttachmentId < aRhs.mAttachmentId)
         return true;
      if (mAttachmentId > aRhs.mAttachmentId)
         return false;
      if (mSubId < aRhs.mSubId)
         return true;
      if (mSubId > aRhs.mSubId)
         return false;
      return mAuxData < aRhs.mAuxData;
   }

   bool operator==(const VaHitEntry& aRhs) const
   {
      return mEntityId == aRhs.mEntityId && mAttachmentId == aRhs.mAttachmentId && mSubId == aRhs.mSubId &&
             mAuxData == aRhs.mAuxData;
   }

   bool operator!=(const VaHitEntry& aRhs) const { return !operator==(aRhs); }

   unsigned int EntityId() const { return mEntityId; }
   unsigned int AttachmentId() const { return mAttachmentId; }
   int          SubId() const { return mSubId; }

   // Auxiliary data that identifies the part of the attachment being hit
   std::vector<UtVariant> mAuxData;


   static VaHitEntry FromEntity(unsigned int aEntityId)
   {
      VaHitEntry entry;
      entry.mEntityId = aEntityId;
      return entry;
   }

   static VaHitEntry FromAttachment(unsigned int      aEntityId,
                                    unsigned int      aAttachmentId,
                                    VaAttachmentSubId aSubId = VaAttachmentSubId())
   {
      VaHitEntry entry;
      entry.mEntityId     = aEntityId;
      entry.mAttachmentId = aAttachmentId;
      entry.mSubId        = aSubId.mSubId;
      entry.mAuxData      = aSubId.mAuxData;
      return entry;
   }

private:
   // Entity ID if this is an entity, or attachment's owning entity ID if this is an attachment
   unsigned int mEntityId;
   // ID of attachment if this is an attachment
   unsigned int mAttachmentId;
   // Sub-ID of attachment.  Zero indicates the whole attachment.
   int mSubId;
};

typedef std::list<VaHitEntry> VaHitList;
typedef std::set<VaHitEntry>  VaHitSet;

// VaHitSet helper functions
// {

VESPATK_EXPORT void CollectEntities(const VaHitSet& aHits, VaEntityMap& aEntities, bool aExcludeAttachments = false);
VESPATK_EXPORT void CollectAttachments(const VaHitSet& aHits, VaAttachmentList& aAttachments);

// If a hit set contains a single entity, return that entity
// otherwise return null
VESPATK_EXPORT VaEntity* GetSingleEntityHit(const VaHitSet& aHits);
// If any entities are in the hit set, return the first one.
// Note: the 'first' has no significance; the set is ordered by ID
VESPATK_EXPORT VaEntity* GetFirstEntityHit(const VaHitSet& aHits);
VESPATK_EXPORT VaEntity* GetFirstEntityHit(const VaHitList& aHits);
// If a hit set contains a single attachment, return that attachment
// otherwise return null
VESPATK_EXPORT VaAttachment* GetSingleAttachmentHit(const VaHitSet& aHits);
// If any attachments are in the hit set, return the first one.
// Note: the 'first' has no significance; the set is ordered by ID
VESPATK_EXPORT VaAttachment* GetFirstAttachmentHit(const VaHitSet& aHits);
VESPATK_EXPORT VaAttachment* GetFirstAttachmentHit(const VaHitList& aHits);


VaHitSet::iterator VESPATK_EXPORT       LowerBoundAttachment(VaHitSet& aHitSet, VaAttachment* aAttachmentPtr);
VaHitSet::const_iterator VESPATK_EXPORT LowerBoundAttachment(const VaHitSet& aHitSet, VaAttachment* aAttachmentPtr);
VESPATK_EXPORT void                     RemoveAttachmentHits(VaHitSet& aHitSet, VaAttachment* aAttachmentPtr);
VESPATK_EXPORT void AttachmentHits(VaAttachment* aAttachmentPtr, const VaHitSet& aHitSet, VaHitSet& aAttachmentHits);

// }

enum MoveRelativeMask
{
   cMR_ENTITY                      = 0x0001, // include entity
   cMR_ATTACHMENTS                 = 0x0002, // include entity's attachments
   cMR_SELECTED                    = 0x0004, // must be selected
   cMR_ENTITY_MOVED                = 0x0008, // entity was moved
   cMR_ENTITY_ATTACHMENTS          = cMR_ENTITY | cMR_ATTACHMENTS,
   cMR_ENTITY_ATTACHMENTS_SELECTED = cMR_ENTITY | cMR_ATTACHMENTS | cMR_SELECTED,
};

enum SceneMask
{
   cVESPA_SELECTION_MASK        = 0x00000001,
   cVESPA_ENTITY_SELECTION_MASK = 0x00000002
};

} // end namespace vespa

#endif
