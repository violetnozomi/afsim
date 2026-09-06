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

#ifndef WkfAttachmetnZoneBase_hpp
#define WkfAttachmetnZoneBase_hpp

#include "wkf_common_export.h"

#include "VaAttachmentQt.hpp"
#include "VaScenarioKey.hpp"

namespace vespa
{
class VaEntity;
}
namespace wkf
{
class AttachmentZone;

class WKF_COMMON_EXPORT AttachmentZoneBase : public vespa::VaAttachmentQt
{
public:
   AttachmentZoneBase(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr, const std::string& aName = "zone_base");
   ~AttachmentZoneBase() override;

   virtual AttachmentZoneBase*       Ref() { return this; }
   virtual const AttachmentZoneBase* Ref() const { return this; }

   static AttachmentZone* AddAbstractZone(const vespa::VaScenarioKey& aKey, const std::string& aZoneType);

   static std::map<vespa::VaScenarioKey, vespa::VaEntity*>& GetAbstractAttachmentContainer()
   {
      return msAbstractAttachmentContainer;
   }

protected:
   static std::map<vespa::VaScenarioKey, vespa::VaEntity*> msAbstractAttachmentContainer; // this entity will hold prototype
                                                                                          // zones to be referenced
private:
};
} // end namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentZoneBase)
#endif
