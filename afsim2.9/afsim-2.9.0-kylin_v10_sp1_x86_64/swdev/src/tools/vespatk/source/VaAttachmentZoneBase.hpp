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

#ifndef VaAttachmetnZoneBase_hpp
#define VaAttachmetnZoneBase_hpp

#include "vespatk_export.h"

#include "VaAttachment.hpp"
#include "VaScenarioKey.hpp"

namespace vespa
{
class VaAttachmentZone;
class VaEntity;

class VESPATK_DEPRECATED_EXPORT VaAttachmentZoneBase : public VaAttachment
{
public:
   VaAttachmentZoneBase(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName = "zone_base");
   ~VaAttachmentZoneBase() override;

   virtual VaAttachmentZoneBase*       Ref() { return this; }
   virtual const VaAttachmentZoneBase* Ref() const { return this; }

   static VaAttachmentZone* AddAbstractZone(const VaScenarioKey& aKey, const std::string& aZoneType);

   static std::map<VaScenarioKey, VaEntity*>& GetAbstractAttachmentContainer() { return msAbstractAttachmentContainer; }

protected:
   static std::map<VaScenarioKey, VaEntity*> msAbstractAttachmentContainer; // this entity will hold prototype zones to be referenced
private:
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentZoneBase)
#endif
