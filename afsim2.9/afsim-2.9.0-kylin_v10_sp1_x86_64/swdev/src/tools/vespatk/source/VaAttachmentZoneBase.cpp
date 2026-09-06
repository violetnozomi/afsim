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

#include "VaAttachmentZoneBase.hpp"

#include "VaAttachmentZone.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaFactory.hpp"

namespace vespa
{
std::map<VaScenarioKey, VaEntity*> VaAttachmentZoneBase::msAbstractAttachmentContainer;

VaAttachmentZoneBase::VaAttachmentZoneBase(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName)
   : VaAttachment(aParent, aViewerPtr, aName)
{
   SetType<VaAttachmentZoneBase>();
}

VaAttachmentZoneBase::~VaAttachmentZoneBase() {}

// static
VaAttachmentZone* VaAttachmentZoneBase::AddAbstractZone(const VaScenarioKey& aKey, const std::string& aZoneType)
{
   if (msAbstractAttachmentContainer.find(aKey) == msAbstractAttachmentContainer.end())
   {
      msAbstractAttachmentContainer[aKey] = VaEnvironment::CreateEntity<VaEntity>("entity");
   }
   return VaEnvironment::CreateAttachment<VaAttachmentZone>(aZoneType, *(msAbstractAttachmentContainer[aKey]), nullptr);
}

} // namespace vespa
