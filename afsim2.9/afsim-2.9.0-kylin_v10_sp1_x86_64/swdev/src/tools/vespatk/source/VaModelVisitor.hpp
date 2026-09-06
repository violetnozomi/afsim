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

#ifndef VAMODELVISITOR_HPP
#define VAMODELVISITOR_HPP

#include "vespatk_export.h"

#include <set>
#include <stack>

#include <osg/Geometry>
#include <osg/LOD>
#include <osg/Matrix>
#include <osg/NodeVisitor>

namespace vespa
{
// this class will visit a node and apply the desired rotation to all vertices and normals, it applies the
// internal rotations to the model as an optimization, and converts parameters to vertex arrays
// sets up uniforms for material colors
// makes sure the max ranged LOD shows since LODS won't work correctly in shader anyhow
class VESPATK_EXPORT ModelVisitor : public osg::NodeVisitor
{
public:
   // multiply vertices simplifies the model by pre multiplying the matrices into the vertices
   ModelVisitor(const osg::Matrix&                 aTransform,
                std::map<std::string, osg::Node*>& aDofMap,
                bool                               aMultiplyInternalMatrices = true);

private:
   void apply(osg::Geometry& aGeometry) override;
   void apply(osg::Transform& aNode) override;
   void apply(osg::Node& aNode) override;
   void apply(osg::LOD& aNode) override;
   void apply(osg::StateSet& aStateSet);
   void CommonApply(osg::Node& aNode);

   void SetVertexAttribute(osg::Geometry&                  aGeometry,
                           unsigned int                    aIndex,
                           const std::string&              aName,
                           osg::Array&                     aArray,
                           bool                            aNormalize,
                           osg::Geometry::AttributeBinding aBinding);

   osg::Matrix                        mAppliedTransform;
   std::stack<osg::Matrix>            mTransform;
   std::set<osg::Referenced*>         mRefSet;
   std::map<std::string, osg::Node*>& mDofMap;
   bool                               mMultiplyInternalMatrices;
};
} // namespace vespa
#endif
