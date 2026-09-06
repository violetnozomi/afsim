// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VAMODELDATABASE_HPP
#define VAMODELDATABASE_HPP

#include <map>
#include <mutex>

#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Vec3>
#include <osg/ref_ptr>
#include <vespatk_export.h>

#include "VaUtils.hpp"

class UtInput;
class UtPath;

namespace vespa
{
class VaOverlayModelInstantiations;
class VaViewer;

class VESPATK_EXPORT VaModelDefinition
{
public:
   VaModelDefinition()          = default;
   virtual ~VaModelDefinition() = default;
   VaModelDefinition(const std::string& aIconFile,
                     float              aScreenScale = 1.0f,
                     const osg::Vec3&   aWingTip     = osg::Vec3(0, 0, 0),
                     bool               aBillboarded = false,
                     bool               aTeamColored = true);
   void                      SetSharedIconNode(const std::string& aTag, osg::Node* aNodePtr);
   void                      SetDefinitionSourceFile(const std::string& aTag, const std::string& aPath);
   std::string               GetDefinitionSourceFile(const std::string& aTag) const;
   void                      SetDefinitionRange(const std::string& aTag, size_t aRangeLow, size_t aRangeHigh);
   std::pair<size_t, size_t> GetDefinitionRange(const std::string& aTag) const;
   void                      SetReadOnly(const std::string& aTag, bool aReadOnly);
   void ReadModel(UtInput& aInput, const UtPath& aRoot, const std::string& aName, std::set<std::string>& aSets);

   void ReadEngineData(UtInput& aInput, std::vector<osg::Vec4>& aEngineList);

   osg::Vec3              GetWingTip(const std::string& aTag) const;
   std::vector<osg::Vec4> GetEngines(const std::string& aTag) const;

   void        GetArticulations(const std::string& aTag, std::set<std::string>& aArticulationSet) const;
   bool        IsReadOnly(const std::string& aTag) const;
   std::string GetCategories() const { return mCategory; }
   osg::Node*  RequestSharedModel(const std::string& aTag) const;
   osg::Node*  RequestUniqueModel(const std::string& aTag) const;
   void        DereferenceSharedResource(const std::string& aTag);

   bool        Load(const std::string& aTag) const;
   bool        LoadUnique(const std::string& aTag) const;
   float       GetModelScale(const std::string& aTag) const;
   static void SetUnknownModel(VaModelDefinition& aModelDef);

   std::set<std::string> GetSets();

private:
   struct Model
   {
      std::string               mFileName;
      osg::Matrix               mPreTransform;
      float                     mScreenScale;
      bool                      mBillboard;
      bool                      mTeamColor;
      std::string               mDefinitionSource;
      std::pair<size_t, size_t> mDefinitionRange;
      bool                      mReadOnly;
      // vshader
      // fshader
      osg::Vec3                          mWingTip;
      std::vector<osg::Vec4>             mEngines;
      std::map<std::string, std::string> mDofMap;
      mutable osg::ref_ptr<osg::Node>    mSharedResource;
      // this is a prototype for any needed unique resources
      mutable osg::ref_ptr<osg::MatrixTransform> mUniqueResource;
      mutable bool                               mFailedToLoad = false;
   };
   const Model&                 GetBestModel(const std::string& aTag) const;
   std::map<std::string, Model> mModels;
   std::string                  mCategory;
   static Model*                msUnknownModelPtr;
};

// This class loads in a model definition file and is then used by VaAttachmentModel to look up a model by name
class VESPATK_EXPORT VaModelDatabase
{
public:
   VaModelDatabase()          = default;
   virtual ~VaModelDatabase() = default;

   // frees up the model when there are no more instances.  This wouldn't be necessary for a non-shared model, but
   // it is probably simpler just to keep a common interface
   void ReleaseModel(const std::string& aName, osg::ref_ptr<osg::Node>& aRef, vespa::VaViewer* aViewerPtr);

   void ReleaseModel(unsigned int aModelId);

   // request a model that does not need to be modified internally by the owning platform (no articulations or switch usages)
   osg::Node* RequestSharedModel(const std::string& aName, vespa::VaViewer* aViewerPtr);

   // request a model that does not need to be modified internally by the owning platform (no articulations or switch
   // usages) note that it is preferred that you use the form of this that passes in the viewer, so that we get the
   // right model set automatically
   osg::Node* RequestSharedModel(const std::string& aName, const std::string& aSet);

   // request a model that needs to be modified internally by the owning platform (articulations or switch usages)
   // there is a performance cost using these over the shared models
   osg::Node* RequestUniqueModel(const std::string& aName, vespa::VaViewer* aViewerPtr);

   // request a model that needs to be modified internally by the owning platform (articulations or switch usages)
   // there is a performance cost using these over the shared models
   osg::Node* RequestUniqueModel(const std::string& aName, const std::string& aSet);

   unsigned int RequestModel(const std::string& aName,
                             VaViewer*          aViewerPtr,
                             const VaHitEntry&  aHitEntry,
                             const VaHitEntry&  aHoverEntry);

   // loads a set of models from a formatted file
   void LoadModelSet(const std::string& aFilename, bool aReadOnly, const std::string& aLocationPath = "");
   // initialize (builds the default model)
   void Initialize();

   void UpdateModel(unsigned int aId, const osg::Matrix& aModelMatrix, const osg::Matrix& aViewMatrix, const osg::Vec4f& aColor);

   void ArticulateModel(unsigned int aId, const std::string& aPartName, const osg::Matrix& aPartMatrix);

   void ResetModelColor(unsigned int aId, bool aHide);

   void ResetModelLighting(unsigned int aId, bool aShow);

   void ResetModelBillboard(unsigned int aId, bool aShow);

   std::set<std::string> GetNameList();

   //! Adds a custom model to the data base.
   //! The given name will be converted to lowercase.
   //! \param aName The name of the model. Passed by value.
   //! \param aDefinition A copy of the definition to add.
   void AddCustomModelDefinition(const std::string& aName, const VaModelDefinition& aDefinition);

   // returns nullptr if no model definition exists for aName
   const VaModelDefinition* GetModelDefinition(const std::string& aName) const;

   // it is the callers responsibility to clean up the returned definition
   std::pair<std::string, VaModelDefinition*> LoadTemporaryDefinition(const std::string& aDefinition, const UtPath& aRoot);

   static void
   TreatNode(osg::Node* aNodePtr, const osg::Matrix& aPreTransform, float aScreenScale, bool aBillboard, bool aTeamColor);
   static void TreatUniqueNode(osg::Node*                                aNodePtr,
                               const osg::Matrix&                        aPreTransform,
                               float                                     aScreenScale,
                               bool                                      aBillboard,
                               bool                                      aTeamColor,
                               const std::map<std::string, std::string>& aDofMap);
   void        Reset();
   void        ReloadModels();

   void        SetViewerModelSet(VaViewer* aViewerPtr, const std::string& aModelSet);
   std::string GetViewerModelSet(const VaViewer& aViewer);

   const std::set<std::string>& GetSetList() { return mSetList; }

private:
   static void BeginTreatNode(osg::Node* aNodePtr, osg::StateSet* aStateSetPtr);
   static void
   EndTreatNode(osg::Node* aNodePtr, osg::StateSet* aStateSetPtr, float aScreenScale, bool aBillboard, bool aTeamColor);

   std::string FindAlias(const std::string& aInput) const;

   using ModelDefinitionMap   = std::map<std::string, VaModelDefinition>;
   using ModelDefinitionMapIt = ModelDefinitionMap::iterator;

   ModelDefinitionMap                 mModelDefinitionMap;
   VaModelDefinition*                 mUnknownModelPtr{nullptr};
   std::map<std::string, std::string> mCategoryMap;
   std::map<std::string, std::string> mAliasMap;

   std::map<unsigned int, VaOverlayModelInstantiations*> mIdToOverlay;
   unsigned int                                          mIdAssigner{1};

   std::map<unsigned int, std::string> mViewerModelSetMap;
   std::set<std::string>               mSetList;
   std::mutex                          mReloadLock;
};

} // namespace vespa

#endif
