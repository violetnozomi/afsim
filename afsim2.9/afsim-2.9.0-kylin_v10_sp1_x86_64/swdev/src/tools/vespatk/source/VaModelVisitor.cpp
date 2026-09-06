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

#include "VaModelVisitor.hpp"

#include <iostream>
#include <limits>

#include <osg/Geometry>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include "UtoMemory.hpp"

vespa::ModelVisitor::ModelVisitor(const osg::Matrix&                 aTransform,
                                  std::map<std::string, osg::Node*>& aDofMap,
                                  bool                               aMultiplyInternalMatrices)
   : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
   , mAppliedTransform(aTransform)
   , mDofMap(aDofMap)
   , mMultiplyInternalMatrices(aMultiplyInternalMatrices)
{
   mTransform.push(osg::Matrix::identity());
}

void vespa::ModelVisitor::apply(osg::Geometry& aGeometry)
{
   if (aGeometry.getStateSet())
   {
      apply(*aGeometry.getStateSet());
   }
   // if the array is unique or the first instance
   // int vsize = 0;
   osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(aGeometry.getVertexArray());
   if (nullptr != verts)
   {
      if (mRefSet.find(verts) == mRefSet.end())
      {
         mRefSet.insert(verts);
         osg::Matrix inverse = osg::Matrix::inverse(mTransform.top());
         // vsize = sizeof(verts);
         for (size_t i = 0; i < verts->size(); ++i)
         {
            (*verts)[i] = (*verts)[i] * mTransform.top() * mAppliedTransform; // * inverse;
         }
      }

      SetVertexAttribute(aGeometry, 0, "model_Vertex", *aGeometry.getVertexArray(), false, osg::Geometry::BIND_PER_VERTEX);
      aGeometry.setVertexArray(nullptr);
   }
   osg::Vec3Array* norms = dynamic_cast<osg::Vec3Array*>(aGeometry.getNormalArray());
   if (nullptr != norms)
   {
      if (mRefSet.find(norms) == mRefSet.end())
      {
         mRefSet.insert(norms);
         osg::Matrix inverse = osg::Matrix::inverse(mTransform.top());
         for (size_t i = 0; i < norms->size(); ++i)
         {
            osg::Vec4 n4((*norms)[i], 0.0f);
            n4 = n4 * mTransform.top() * mAppliedTransform * inverse;
            n4.normalize();
            (*norms)[i].x() = n4.x();
            (*norms)[i].y() = n4.y();
            (*norms)[i].z() = n4.z();
         }
      }
      SetVertexAttribute(aGeometry, 9, "model_Normal", *aGeometry.getNormalArray(), true, aGeometry.getNormalBinding());
      aGeometry.setNormalArray(nullptr);
   }
   else
   {
      osg::Vec3Array* norms = new osg::Vec3Array();
      norms->resize(1);
      osg::Matrix inverse = osg::Matrix::inverse(mTransform.top());
      for (auto&& norm : *norms)
      {
         norm = osg::Vec3(0, 0, 1) * mTransform.top() * mAppliedTransform * inverse;
      }
      SetVertexAttribute(aGeometry, 9, "model_Normal", *norms, true, osg::Geometry::BIND_OVERALL);
   }
   if (nullptr != aGeometry.getTexCoordArray(0))
   {
      SetVertexAttribute(aGeometry, 10, "model_MultiTexCoord0", *aGeometry.getTexCoordArray(0), true, osg::Geometry::BIND_PER_VERTEX);
      aGeometry.setTexCoordArray(0, nullptr);
   }
   else
   {
      if (aGeometry.getVertexAttribArray(10) == nullptr)
      {
         osg::Vec4Array* texcos = new osg::Vec4Array();
         texcos->resize(1);
         for (auto&& it : *texcos)
         {
            it = osg::Vec4(0, 0, 0, 0);
         }
         SetVertexAttribute(aGeometry, 10, "model_MultiTexCoord0", *texcos, false, osg::Geometry::BIND_OVERALL);
      }
   }
   // hey we should also get color array, secondary color array, fog coord array, and texcoord 1 - 7
}

void vespa::ModelVisitor::CommonApply(osg::Node& aNode)
{
   // if the name is in our list of nodes to add a DOF to...
   auto dofEntry = mDofMap.find(aNode.getName());
   if (dofEntry != mDofMap.end())
   {
      dofEntry->second = &aNode;
   }
   if (aNode.getStateSet())
   {
      apply(*aNode.getStateSet());
   }
}

void vespa::ModelVisitor::apply(osg::LOD& aNode)
{
   CommonApply(aNode);

   unsigned int maxIndex = 0;
   float        maxRange = 0.0F;
   for (unsigned int i = 0; i < aNode.getNumRanges(); ++i)
   {
      float range = aNode.getMaxRange(i);
      if (range > maxRange)
      {
         maxRange = range;
         maxIndex = i;
      }
   }
   for (unsigned int i = 0; i < aNode.getNumRanges(); ++i)
   {
      if (i == maxIndex)
      {
         aNode.setRange(i, 0, std::numeric_limits<float>::max());
      }
      else
      {
         aNode.setRange(i, std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
      }
   }
   traverse(aNode);
}

void vespa::ModelVisitor::apply(osg::Transform& aNode)
{
   CommonApply(aNode);
   if (mMultiplyInternalMatrices)
   {
      osg::Matrix active = mTransform.top();
      osg::Matrix nodeMat;
      aNode.computeLocalToWorldMatrix(nodeMat, nullptr);

      auto matrixTransform           = aNode.asMatrixTransform();
      auto positionAttitudeTransform = aNode.asPositionAttitudeTransform();
      if (matrixTransform)
      {
         matrixTransform->setMatrix(osg::Matrix::identity());
      }
      else if (positionAttitudeTransform)
      {
         positionAttitudeTransform->setPosition(osg::Vec3d());
         positionAttitudeTransform->setAttitude(osg::Quat());
         positionAttitudeTransform->setScale(osg::Vec3d());
      }
      active.postMult(nodeMat);
      mTransform.push(active);
      traverse(aNode);

      mTransform.pop();
   }
   else
   {
      traverse(aNode);
   }
}

void vespa::ModelVisitor::apply(osg::Node& aNode)
{
   CommonApply(aNode);
   traverse(aNode);
}

void vespa::ModelVisitor::apply(osg::StateSet& aStateSet)
{
   osg::Material* material = dynamic_cast<osg::Material*>(aStateSet.getAttribute(osg::StateAttribute::MATERIAL));
   if (material)
   {
      aStateSet.getOrCreateUniform("material_ambient", osg::Uniform::FLOAT_VEC4)->set(material->getAmbient(osg::Material::FRONT));
      aStateSet.getOrCreateUniform("material_diffuse", osg::Uniform::FLOAT_VEC4)->set(material->getDiffuse(osg::Material::FRONT));
      aStateSet.getOrCreateUniform("material_emission", osg::Uniform::FLOAT_VEC4)
         ->set(material->getEmission(osg::Material::FRONT));
      aStateSet.removeAttribute(material);
   }
}

void vespa::ModelVisitor::SetVertexAttribute(osg::Geometry&                  aGeometry,
                                             unsigned int                    aIndex,
                                             const std::string&              aName,
                                             osg::Array&                     aArray,
                                             bool                            aNormalize,
                                             osg::Geometry::AttributeBinding aBinding)
{
   aArray.setName(aName);
   aGeometry.setVertexAttribArray(aIndex, &aArray);
   aGeometry.setVertexAttribNormalize(aIndex, aNormalize);
   aGeometry.setVertexAttribBinding(aIndex, aBinding);
}
