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

#ifndef VaFactory_hpp
#define VaFactory_hpp

#include "vespatk_export.h"

#include <string>

#include "UtException.hpp"

namespace vespa
{
class VaAttachment;
class VaEntity;
class VaFileChooser;
class VaOverlay;
class VaScenario;
class VaViewer;

class UnknownAttachmentTypeError : public UtException
{
public:
   UnknownAttachmentTypeError(std::string aType)
      : UtException("Unknown attachment type: " + aType)
   {
   }
};

class VESPATK_EXPORT VaFactory
{
public:
   VaFactory();
   virtual ~VaFactory();

   template<class T>
   T*        CreateEntity(const std::string& aEntityType) const;
   VaEntity* CreateEntity(const std::string& aEntityType) const;

   template<class T>
   T* CreateAttachment(const std::string& aAttachmentType, VaEntity& aEntity, VaViewer* aViewerPtr, bool aLoad = true) const;
   VaAttachment* CreateAttachment(const std::string& aAttachmentType,
                                  VaEntity&          aEntity,
                                  VaViewer*          aViewerPtr,
                                  bool               aLoad = true) const;

   VaScenario* CreateScenario(const std::string& aType) const;

   VaOverlay* CreateOverlay(const std::string& aOverlayType, const std::string& aOverlayName = "") const;

   VaFileChooser* CreateFileChooser(const std::string& aFileChooserType,
                                    const std::string& aMessage,
                                    const std::string& aPattern,
                                    const std::string& aFileName,
                                    int                aRelative,
                                    const std::string& aNS,
                                    int                aType) const;

protected:
   virtual VaEntity* UserCreateEntity(const std::string& aEntityType) const { return nullptr; }

   virtual VaAttachment* UserCreateAttachment(const std::string& aAttachmentType, VaEntity& aEntity, VaViewer* aViewerPtr) const
   {
      return nullptr;
   }

   virtual VaScenario* UserCreateScenario(const std::string& aScenarioType) const { return nullptr; }

   virtual VaOverlay* UserCreateOverlay(const std::string& aOverlayType, const std::string& aOverlayName = "") const
   {
      return nullptr;
   }

   virtual VaFileChooser* UserCreateFileChooser(const std::string& aFileChooserType,
                                                const std::string& aMessage,
                                                const std::string& aPattern,
                                                const std::string& aFileName,
                                                int                aRelative,
                                                const std::string& aNS,
                                                int                aType) const
   {
      return nullptr;
   }
};

template<class T>
T* VaFactory::CreateEntity(const std::string& aEntityType) const
{
   return static_cast<T*>(CreateEntity(aEntityType));
}

template<class T>
T* VaFactory::CreateAttachment(const std::string& aAttachmentType, VaEntity& aEntity, VaViewer* aViewerPtr, bool aLoad) const
{
   return static_cast<T*>(CreateAttachment(aAttachmentType, aEntity, aViewerPtr, aLoad));
}
} // end namespace vespa
#endif
