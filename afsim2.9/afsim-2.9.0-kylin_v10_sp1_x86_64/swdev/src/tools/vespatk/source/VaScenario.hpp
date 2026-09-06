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

#ifndef VaScenario_hpp
#define VaScenario_hpp

#include "vespatk_export.h"

#include <map>
#include <string>
#include <vector>

#include "VaBoundingBox.hpp"
#include "VaCallbackHolder.hpp"
#include "VaDefs.hpp"
#include "VaEntity.hpp"
#include "VaObject.hpp"
#include "VaScenarioKey.hpp"

class DisEntityId;
class UtEntity;
class UtLLAPos;

namespace vespa
{
class VaAttachment;
class VaBoundingBox;
class VaPosition;
class VaSelectionBox;
class VaViewer;

class VESPATK_EXPORT VaScenario : public VaObject
{
public:
   //! A list of simulation platform types.
   typedef std::vector<std::string> PlatformTypeList;

public:
   VaScenario(const std::string& aFormat);
   VaScenario(const VaScenario& aSrc) = delete;
   ~VaScenario() override;

   //-------------------------------------------------------------------------------
   //! Scenario State Methods.

   const VaScenarioKey& GetKey() const { return mKey; }

   void SetName(const std::string& aName) override;

   virtual void SetFormat(const std::string& aFormat);
   std::string  GetFormat() const { return mFormat; }

   virtual void SetSource(const std::string& aSource);
   std::string  GetSource() const { return mSource; }
   std::string  GetSourceFile() const { return mSourceFile; }

   virtual void SetModified(bool aModified);
   const bool   IsModified() const { return mModified; }

   bool IsInitialized() const { return mInitialized; }

   bool IsLoaded() const { return mLoaded; }

   //-------------------------------------------------------------------------------
   //! Scenario Lifecycle Methods.

   virtual bool Initialize();
   virtual void Load(VaViewer* aViewerPtr);
   virtual bool Reset();
   virtual void Update(double aTime);
   virtual void UpdateFrame(double aTime);

   //-------------------------------------------------------------------------------
   //! Scenario Utility Methods.

   virtual void TranslateScenario(double aDeltaLat, double aDeltaLon);
   virtual void RotateScenario(const UtLLAPos& aCenterLLA, double aAngle);

   virtual double GetMaxTime() { return 0.0; }

   virtual void GetScenarioBound(VaBoundingBox& aBoundingBox) const;
   virtual void GetBound(const VaEntityMap& aEntities, VaBoundingBox& aBoundingBox) const;

   //-------------------------------------------------------------------------------
   //! Entity Management Methods.

   virtual bool      AddEntity(VaEntity* aEntityPtr);
   virtual bool      RemoveEntity(VaEntity* aEntityPtr);
   virtual void      DeleteEntity(VaEntity* aEntityPtr);
   virtual VaEntity* FindEntity(const std::string& aName) const;
   template<class T>
   VaEntity* FindEntityOfType(const std::string& aName) const;

   virtual VaEntity* FindEntity(unsigned int aUniqueId) const;
   virtual VaEntity* FindEntity(const DisEntityId& aId) { return nullptr; }
   virtual VaEntity* FindEntity(const int aSite, const int aApplication, const int aEntity) { return nullptr; }

   VaAttachment* FindAttachment(unsigned int aUniqueId) const;
   VaAttachment* FindAttachment(const std::string& aName) const;

   void GetEntityList(VaEntityList& aEntityList) const;
   template<class T>
   void GetEntityListOfType(VaEntityList& aEntityList) const;
   void GetSelectedEntityList(VaViewer* aViewerPtr, VaEntityList& aEntityList) const;

   virtual bool BoxSelect(VaViewer* aViewerPtr, const VaSelectionBox& aBox, bool aInside, bool aAddSelection = false);

   virtual bool DeleteSelection(VaViewer* aViewerPtr, VaEntityMap& aEntityMap, VaAttachmentList& aAttachmentList);
   virtual void MoveSelection(VaViewer*          aViewerPtr,
                              const VaEntityMap& aEntityMap,
                              VaAttachmentList&  aAttachmentList,
                              UtEntity&          aRefBefore,
                              UtEntity&          aRefAfter,
                              int                aFeedbackFormat,
                              double             aFeedbackPrecision,
                              bool               aOverrideModifiable = false);
   virtual void MoveSelectionComplete(VaViewer* aViewerPtr, VaEntityMap& aEntityMap, VaAttachmentList& aAttachmentList);
   virtual void RotateSelection(VaViewer*         aViewerPtr,
                                VaEntityMap&      aEntityMap,
                                VaAttachmentList& aAttachmentList,
                                const UtLLAPos&   aCenterLLA,
                                double            aAngle);

   void GetPlatformTypes(PlatformTypeList& aPlatformTypes) const;
   bool AddPlatformType(const std::string& aPlatformType);

   virtual VaEntity* GetTopEntity() { return nullptr; }

   virtual bool AffectsTimeRange() { return true; }
   virtual void DirtyTimeRange();
   virtual void GetTimeRange(double& aStartTime, double& aEndTime);

   void AttachmentAdded(VaAttachment& aAttachment);
   void AttachmentRemoved(VaAttachment& aAttachment);

   virtual bool IsModifiable() { return mModifiable; }
   virtual void SetModifiable(bool aState) { mModifiable = aState; }

   virtual void SetAutocenter(bool aState) { mAutocenter = aState; }
   virtual bool GetAutocenter() { return mAutocenter; }

protected:
   using VaEntityNameMap = std::multimap<std::string, VaEntity*>;
   VaCallbackHolder        mCallbacks;
   static VaCallbackHolder mGlobalCallbacks;
   bool                    mModified;
   bool                    mModifiable;
   VaEntityMap             mEntityMap;
   VaEntityNameMap         mEntityNameMap;
   double                  mStartTime;
   double                  mEndTime;
   bool                    mTimeRangeDirty;

   void         SetPlatformTypes(const PlatformTypeList& aPlatformTypes);
   virtual void PrivateDeleteEntity(VaEntity* aEntityPtr) {}
   virtual void PrivateRemoveEntity(VaEntity* aEntityPtr) {}

private:
   VaScenario(); // disallow

   void AddAttachments(VaEntity* aEntityPtr);
   void AddAttachment(VaAttachment* aAttachmentPtr);
   void RemoveAttachments(VaEntity* aEntityPtr);
   void RemoveAttachment(VaAttachment* aAttachmentPtr, int aSubpartId);

   void EntityDeletedCB(VaEntity* aEntityPtr);
   void EntityNameChangedCB(const std::string& aOldName, const std::string& aNewName, VaEntity* aEntityPtr);

   VaScenarioKey   mKey;
   std::string     mFormat;
   std::string     mSource;
   std::string     mSourceFile;
   bool            mInitialized;
   bool            mLoaded;
   VaAttachmentMap mAttachmentMap;

   PlatformTypeList mPlatformTypes;

   bool mLayerShow;

   unsigned int mXmlHidden;

   bool mAutocenter;
};
} // end namespace vespa

template<class T>
vespa::VaEntity* vespa::VaScenario::FindEntityOfType(const std::string& aName) const
{
   VaEntity* entityPtr = nullptr;

   for (auto&& entity : mEntityMap)
   {
      VaEntity* tempEntityPtr = entity.second;

      if (tempEntityPtr->IsA_TypeOf<T>())
      {
         if (tempEntityPtr->GetName() == aName)
         {
            entityPtr = tempEntityPtr;
            break;
         }
      }
   }
   return entityPtr;
}

template<class T>
void vespa::VaScenario::GetEntityListOfType(VaEntityList& aEntityList) const
{
   for (auto&& entity : mEntityMap)
   {
      if (entity.second->IsA_TypeOf<T>())
      {
         aEntityList.push_back(entity.second);
      }
   }
}

#endif
