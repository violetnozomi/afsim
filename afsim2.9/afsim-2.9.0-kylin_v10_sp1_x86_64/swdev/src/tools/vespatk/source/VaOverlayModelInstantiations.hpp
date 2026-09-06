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

#ifndef VAOVERLAYMODELINSTANTIATIONS_HPP
#define VAOVERLAYMODELINSTANTIATIONS_HPP

#include "vespatk_export.h"

#include <osg/Image>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Vec4>
#include <osg/ref_ptr>

#include "VaOverlay.hpp"

class UtoRawShape;

namespace osg
{
class Geometry;
class Node;
class Uniform;
} // namespace osg

namespace vespa
{
class VaModelDatabase;

class VESPATK_EXPORT VaOverlayModelInstantiations : public VaOverlay
{
public:
   VaOverlayModelInstantiations();

   VaOverlayModelInstantiations(const std::string& aName);

   VaOverlayModelInstantiations* Clone() const override;

   ~VaOverlayModelInstantiations() override;

   bool Initialize() override;

   bool HasModel(const std::string& aName) const;
   void AddModel(const std::string& aName, osg::Node* aModelNode, const std::set<std::string>& aDofList);
   void CreateInstance(unsigned int             aId,
                       const std::string&       aName,
                       const vespa::VaHitEntry& aHitEntry,
                       const vespa::VaHitEntry& aHoverEntry);
   void ReleaseInstance(unsigned int aId);
   void UpdateInstance(unsigned int aId, const osg::Matrix& aModelMat, const osg::Matrix& aViewMat, const osg::Vec4& aColor);
   void ArticulateModel(unsigned int aId, const std::string& aPartName, const osg::Matrix& aPartMatrix);
   void SetTeamColorUniform(unsigned int aId, bool aHide);
   void SetLightSpecularUniform(unsigned int aId, const osg::Vec4& aUniform);
   void SetBillboard(unsigned int aId, bool aUse);
   void Intersect(VaHitList&         aHitList,
                  int                aMouseX,
                  int                aMouseY,
                  const osg::Matrix& aView,
                  const osg::Matrix& aProjection,
                  int                aVP_Width,
                  int                aVP_Height,
                  bool               aScaled,
                  float              aIconScalar,
                  bool               aHover);
   void Reload(vespa::VaModelDatabase* aDatabase);

private:
   struct ModelBatch
   {
      osg::ref_ptr<osg::Node>                      mModel;
      osg::ref_ptr<osg::Uniform>                   mModelMatrixUniformArray;
      osg::ref_ptr<osg::Uniform>                   mColorUniformArray;
      std::vector<osg::Geometry*>                  mGeometryList;
      std::vector<vespa::VaHitEntry>               mHits;
      std::vector<vespa::VaHitEntry>               mHovers;
      std::vector<unsigned int>                    mIds;
      std::map<std::string, osg::MatrixTransform*> mDofList;
      unsigned int                                 mInstances;
      bool                                         mIsUnique{false};
   };
   ModelBatch* AddBatch(const std::string& aName);
   struct InstanceHolder
   {
      unsigned int mInstanceId;
      ModelBatch*  mBatchPtr;
   };

   UtoRawShape*                                    mShapePtr;
   std::string                                     mShapeName;
   std::map<std::string, std::vector<ModelBatch*>> mModelMap;
   std::map<unsigned int, InstanceHolder>          mIdToInstance;
   osg::ref_ptr<osg::Image>                        mImage;
};

} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayModelInstantiations)
#endif
