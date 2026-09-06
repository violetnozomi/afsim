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
#ifndef _UTOTERRAIN_COLOR_VISITOR_HPP_
#define _UTOTERRAIN_COLOR_VISITOR_HPP_

// osg includes
#include <osg/Material>
#include <osg/NodeVisitor>

// defines the behavior for marking nodes for blending and transparency
class UtoTerrainColorVisitor : public osg::NodeVisitor
{
public:
   // constructor
   UtoTerrainColorVisitor(const osg::Vec4& rBlendColor)
      : osg::NodeVisitor(osg::NodeVisitor::NODE_VISITOR, osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      , m_vBlendColor(rBlendColor)
      , m_pUserData(new osg::Referenced)
   {
      /* Do Nothing */
   }

   // each node will be marked as being visited
   void apply(osg::Node& rNode) override
   {
      // the node marking algorithm will just set user data
      // to an otherwise unused osg::Referenced instance.
      // there might be better choices, like the parent.
      // there is still a problem of if the dll has happen to already
      // set the user data that and that this would just be trashing it
      // so far no encounters or problems.
      rNode.setUserData(m_pUserData.get());
      // visit the base class
      NodeVisitor::apply(rNode);
   }

   // each geometry node will set a specific blending node
   void apply(osg::Geode& rNode) override
   {
      // obtain the number of drawables
      unsigned int nNumDrawables = rNode.getNumDrawables();

      for (unsigned int nIndex = 0; nIndex < nNumDrawables; nIndex++)
      {
         // obtain the geometry node from within the drawable
         if (osg::Geometry* pGeometry = dynamic_cast<osg::Geometry*>(rNode.getDrawable(nIndex)))
         {
            // create a new Vec4Array with only one availabe location
            osg::Vec4Array* pColorArray = new osg::Vec4Array(1);
            // copy the blend value to color array
            (*pColorArray)[0] = m_vBlendColor;
            // set the color array inside of the geometry
            pGeometry->setColorArray(pColorArray);
            pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
            // set the states for this node
            rNode.getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
            rNode.getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
            // determine if lighting is enabled
            if (pGeometry->getOrCreateStateSet()->getMode(GL_LIGHTING) || rNode.getOrCreateStateSet()->getMode(GL_LIGHTING))
            {
               // create a new material object
               osg::Material* pMaterial = new osg::Material;
               // set the material properties
               pMaterial->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
               pMaterial->setAmbient(osg::Material::FRONT_AND_BACK, m_vBlendColor);
               pMaterial->setDiffuse(osg::Material::FRONT_AND_BACK, m_vBlendColor);

               // if lighting is enabled, turn material traking on
               rNode.getOrCreateStateSet()->setAttribute(pMaterial, osg::StateAttribute::ON);
               pGeometry->getOrCreateStateSet()->setAttribute(pMaterial, osg::StateAttribute::ON);
            }
         }
      }

      // call the base class
      NodeVisitor::apply(rNode);
   }

   // public member variables
   const osg::Vec4&              m_vBlendColor;
   osg::ref_ptr<osg::Referenced> m_pUserData;
};

// defines the behavior of marking nodes as they are visited
class UtoTerrainNodeMarkVisitor : public osg::NodeVisitor
{
public:
   // constructor
   UtoTerrainNodeMarkVisitor(osg::Vec4& rBlendColor)
      : osg::NodeVisitor(osg::NodeVisitor::NODE_VISITOR, osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
      , m_vBlendColor(rBlendColor){
           /* Do Nothing */
        };

   // visits each node
   void apply(osg::Node& rNode) override
   {
      // each node will pass through this function
      // if the node is not marked by evaluating the user data,
      // the node will be traversed and color blending values set
      if (rNode.getUserData() == nullptr)
      {
         // create a color visitor
         // the color visitor will be responsible for setting nodes
         // that have been marked as visited and colored.
         UtoTerrainColorVisitor oColorVisitor(m_vBlendColor);
         // traverse all the nodes below this node
         rNode.accept(oColorVisitor);
      }

      // call the base class
      NodeVisitor::apply(rNode);
   }

   // public member variables
   const osg::Vec4& m_vBlendColor;
};

class UtoTerrainNodeClearVisitor : public osg::NodeVisitor
{
public:
   // constructor
   UtoTerrainNodeClearVisitor()
      : osg::NodeVisitor(osg::NodeVisitor::NODE_VISITOR, osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
   {
      /* Do Nothing */
   }

   // visits each node
   void apply(osg::Node& rNode) override
   {
      // clear the user data
      rNode.setUserData(nullptr);
      // call the base class
      NodeVisitor::apply(rNode);
   }
};

// defines the behavior of how the nodes are updated when traversed
class UtoTerrainUpdateMarkNodeVisitor : public osg::NodeCallback
{
public:
   // constructor
   UtoTerrainUpdateMarkNodeVisitor(float fRed, float fGreen, float fBlue, float fAlpha)
      : m_bClearMarkedNodes(false)
      , m_vBlendColor(fRed, fGreen, fBlue, fAlpha)
   {
      /* Do Nothing */
   }

   // operation called by the node for updates
   void operator()(osg::Node* pNode, osg::NodeVisitor* pVisitor) override
   {
      // determine if marked nodes should be cleared
      if (m_bClearMarkedNodes)
      {
         // create a local stack node visitor
         UtoTerrainNodeClearVisitor oClearVisitor;
         // have the root node accept the visitor
         pNode->accept(oClearVisitor);
         // reset the clear marked nodes flag
         m_bClearMarkedNodes = false;
      }

      // create a local stack node visitor
      UtoTerrainNodeMarkVisitor oMarkVisitor(m_vBlendColor);
      // every update will check for nodes that are not marked as being colored
      pNode->accept(oMarkVisitor);

      // note, callback is responsible for scenegraph traversal so
      // should always include call traverse(node,nv) to ensure
      // that the rest of cullbacks and the scene graph are traversed.
      NodeCallback::traverse(pNode, pVisitor);
   }

   // sets the clear node state for the update visitor
   void ClearChildNodes()
   {
      // setting this value to true indicates that the
      // child nodes of the root node should be cleared.
      m_bClearMarkedNodes = true;
   }

   // public member variables
   bool      m_bClearMarkedNodes;
   osg::Vec4 m_vBlendColor;
};

// resets the terrain to a color of white
class UtoClearTerrainColorVisitor : public osg::NodeVisitor
{
public:
   // constructor
   UtoClearTerrainColorVisitor()
      : osg::NodeVisitor(osg::NodeVisitor::NODE_VISITOR, osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      ,

      m_vBlendColor(1.0f, 1.0f, 1.0f, 1.0f)
   {
      /* Do Nothing */
   }

   // each node will be marked as being visited
   void apply(osg::Node& rNode) override
   {
      // release the unused referenced data
      rNode.setUserData(nullptr);
      // visit the base class
      NodeVisitor::apply(rNode);
   }

   // each geometry node will set a specific blending node
   void apply(osg::Geode& rNode) override
   {
      // obtain the number of drawables
      unsigned int nNumDrawables = rNode.getNumDrawables();

      for (unsigned int nIndex = 0; nIndex < nNumDrawables; nIndex++)
      {
         // obtain the geometry node from within the drawable
         if (osg::Geometry* pGeometry = dynamic_cast<osg::Geometry*>(rNode.getDrawable(nIndex)))
         {
            // create a new Vec4Array with only one availabe location
            osg::Vec4Array* pColorArray = new osg::Vec4Array(1);
            // copy the blend value to color array
            (*pColorArray)[0] = m_vBlendColor;
            // set the color array inside of the geometry
            pGeometry->setColorArray(pColorArray);
            pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
            // set the states for this node
            rNode.getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
            rNode.getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
            // determine if lighting is enabled
            if (pGeometry->getOrCreateStateSet()->getMode(GL_LIGHTING) || rNode.getOrCreateStateSet()->getMode(GL_LIGHTING))
            {
               // create a new material object
               osg::Material* pMaterial = new osg::Material;
               // set the material properties
               pMaterial->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
               pMaterial->setAmbient(osg::Material::FRONT_AND_BACK, m_vBlendColor);
               pMaterial->setDiffuse(osg::Material::FRONT_AND_BACK, m_vBlendColor);

               // if lighting is enabled, turn material traking on
               rNode.getOrCreateStateSet()->setAttribute(pMaterial, osg::StateAttribute::ON);
               pGeometry->getOrCreateStateSet()->setAttribute(pMaterial, osg::StateAttribute::ON);
            }
         }
      }

      // call the base class
      NodeVisitor::apply(rNode);
   }

private:
   // private member variables
   osg::Vec4 m_vBlendColor;
};

#endif // _UTOTERRAIN_COLOR_VISITOR_HPP_
