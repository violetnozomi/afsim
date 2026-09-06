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

#include "WkfAttachmentZoneBase.hpp"

#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaFactory.hpp"
#include "zone/WkfAttachmentZone.hpp"

namespace wkf
{
std::map<vespa::VaScenarioKey, vespa::VaEntity*> AttachmentZoneBase::msAbstractAttachmentContainer;

AttachmentZoneBase::AttachmentZoneBase(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr, const std::string& aName)
   : VaAttachmentQt(aParent, aViewerPtr, aName)
{
   SetType<AttachmentZoneBase>();
}

AttachmentZoneBase::~AttachmentZoneBase() {}

// static
AttachmentZone* AttachmentZoneBase::AddAbstractZone(const vespa::VaScenarioKey& aKey, const std::string& aZoneType)
{
   if (msAbstractAttachmentContainer.find(aKey) == msAbstractAttachmentContainer.end())
   {
      msAbstractAttachmentContainer[aKey] = vespa::VaEnvironment::CreateEntity<vespa::VaEntity>("entity");
   }
   return vespa::VaEnvironment::CreateAttachment<AttachmentZone>(aZoneType, *(msAbstractAttachmentContainer[aKey]), nullptr);
}

} // namespace wkf
