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

/////////////////////////////////////////////////////////////////////////////
// UtoModelShapeImp.cpp : implementation file

#include "UtoModelShapeImp.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>

#include <osg/Material>
#include <osg/TexEnv>
#include <osg/Texture>
#include <osgDB/ReadFile>
#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>
#include <osgUtil/Optimizer>

#include "UtoModelPartImp.hpp"
#include "UtoModelShape.hpp"
#include "UtoModelSwitchImp.hpp"
#include "UtoOptions.hpp"
#include "UtoPriv.hpp"
#include "UtoResourceDB.hpp"
#include "UtoShapeFactory.hpp"
#include "UtoShapeFrame.hpp"
#include "UtoTexturedPolygonImp.hpp"

#define SCALE_VEC3(v1, v2) (v1.x() *= v2.x()), (v1.y() *= v2.y()), (v1.z() *= v2.z());

#ifdef _WIN32
#define STRICASECMP stricmp
#else
#include <strings.h>
#define STRICASECMP strcasecmp
#endif


namespace
{
const bool model_registered = UtoShapeFactory::Instance()->RegisterShape("model", UtoModelShapeImp::CreateModel);
const bool image_registered = UtoShapeFactory::Instance()->RegisterShape("image", UtoModelShapeImp::CreateImage);
}; // namespace

UtoModelShapeImp::AttributeMap UtoModelShapeImp::m_ModelAttributes;
UtoModelShapeImp::AttributeMap UtoModelShapeImp::m_ImageAttributes;
UtoModelPartImp*               m_DummyPart   = new UtoModelPartImp(new osgSim::DOFTransform());
UtoModelSwitchImp*             m_DummySwitch = new UtoModelSwitchImp(new osgSim::MultiSwitch);

UtoModelShapeImp::Data::Data() {}

UtoModelShapeImp::Data::~Data() {}

UtoModelShapeImp::UtoModelShapeImp()
   : m_Data(new Data)
   , m_LoadData(nullptr)
   , m_ModelNode(nullptr)
   , m_TexturedPolygon(false)
   , mIsShallowCopy(true)
   , m_CopyFlags(osg::CopyOp::DEEP_COPY_OBJECTS | osg::CopyOp::DEEP_COPY_NODES)
   , m_ColorArray(nullptr)
{
}

UtoModelShapeImp::UtoModelShapeImp(const UtoModelShapeImp& rhs)
   : UtoShapeImp(rhs)
   , m_Data(rhs.m_Data)
   , m_LoadData(nullptr)
   , m_ModelNode(rhs.m_ModelNode)
   , m_TexturedPolygon(rhs.m_TexturedPolygon)
   , mIsShallowCopy(false)
   , m_CopyFlags(rhs.m_CopyFlags)
   , m_ColorArray(rhs.m_ColorArray)
{
   if (!m_TexturedPolygon)
   {
      typedef osg::CopyOp CO;
      CO                  copyOp = m_CopyFlags;
      if (m_CopyFlags == osg::CopyOp::SHALLOW_COPY)
      {
         mIsShallowCopy = true;
      }
      if (rhs.m_ModelNode != nullptr)
      {
         m_ModelNode = dynamic_cast<osg::Node*>(rhs.m_ModelNode->clone(copyOp));
      }
      if (m_ModelNode.get())
      {
         AddShapeChildNode(m_ModelNode.get());
      }
      else
      {
         // Always assert failure
         assert(!"Failed to clone a model");
      }
   }
   else
   {
      AddShapeChildNode(m_ModelNode.get());
   }

   SetupStateSet();
}

UtoModelShapeImp::~UtoModelShapeImp()
{
   // cleanup the model parts.
   ModelPartMap::iterator itPart, endPart = m_ModelPart.end();
   for (itPart = m_ModelPart.begin(); itPart != endPart; ++itPart)
   {
      delete itPart->second;
   }

   // cleanup the model switches.
   ModelSwitchMap::iterator itSwitch, endSwitch = m_ModelSwitch.end();
   for (itSwitch = m_ModelSwitch.begin(); itSwitch != endSwitch; ++itSwitch)
   {
      delete itSwitch->second;
   }
}

void UtoModelShapeImp::SetMetric(const std::string& name, double value)
{
   m_Data->m_Metrics[name] = value;
}

bool UtoModelShapeImp::Metric(const std::string& name, double& value) const
{
   MetricMap::const_iterator it = m_Data->m_Metrics.find(name);
   if (it != m_Data->m_Metrics.end())
   {
      value = it->second;
      return true;
   }
   return false;
}

bool UtoModelShapeImp::InGroup(const std::string& group) const
{
   GroupVec::const_iterator it = std::find(m_Data->m_Groups.begin(), m_Data->m_Groups.end(), group);
   return it != m_Data->m_Groups.end();
}

// Finds the first node by the given name of the given type.
class NodeFinder : public osg::NodeVisitor
{
public:
   friend class osg::NodeVisitor;

   enum NodeType
   {
      MULTISWITCH,
      DOFTRANSFORM
   };
   NodeFinder(const std::string name, NodeType type)
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      , m_Name(name)
      , m_Type(type)
   {
      reset();
   }

   // null if none was found
   osgSim::MultiSwitch* MultiSwitchNode() { return m_MultiSwitch.get(); }
   // null if none was found
   osgSim::DOFTransform* DOFTransformNode() { return m_DOFTransform.get(); }

   void apply(osg::Node& node) override
   {
      // Stop looking as soon as we find one node that matches.
      if (!m_Found)
         NodeVisitor::apply(node);
   }

   void apply(osg::Transform& trans) override
   {
      if (osgSim::DOFTransform* dof = dynamic_cast<osgSim::DOFTransform*>(&trans))
      {
         if (dof->getName() == m_Name)
         {
            m_DOFTransform = dof;
            m_Found        = true;
            return;
         }
      }
      NodeVisitor::apply(trans);
   }

   void apply(osg::Group& group) override
   {
      if (osgSim::MultiSwitch* multiSwitch = dynamic_cast<osgSim::MultiSwitch*>(&group))
      {
         if (multiSwitch->getName() == m_Name)
         {
            m_MultiSwitch = multiSwitch;
            m_Found       = true;
            return;
         }
      }
      NodeVisitor::apply(static_cast<osg::Group&>(group));
   }

   void reset() override
   {
      m_Found        = false;
      m_MultiSwitch  = nullptr;
      m_DOFTransform = nullptr;
      NodeVisitor::reset();
   }

private:
   ~NodeFinder() override {}
   std::string                        m_Name;
   NodeType                           m_Type;
   osg::ref_ptr<osgSim::MultiSwitch>  m_MultiSwitch;
   osg::ref_ptr<osgSim::DOFTransform> m_DOFTransform;
   bool                               m_Found;
};

// Collects the names of any switch or DOFTransform (articulation) nodes
class NodeNameCollector : public osg::NodeVisitor
{
public:
   friend class osg::NodeVisitor;

   enum NodeType
   {
      MULTISWITCH,
      DOFTRANSFORM
   };
   NodeNameCollector(NodeType type)
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      , m_Type(type)
   {
      reset();
   }

   std::vector<std::string> DOFTransformNodeNames() { return m_NodeDOFTNames; }
   std::vector<std::string> MultiSwitchNodeNames() { return m_NodeMultiNames; }
   std::vector<int>         MultiSwitchNodeSizes() { return m_SwitchSetSizes; }

   void apply(osg::Node& node) override { traverse(node); }

   void apply(osg::Transform& trans) override
   {
      if (osgSim::DOFTransform* dof = dynamic_cast<osgSim::DOFTransform*>(&trans))
      {
         m_NodeDOFTNames.push_back(dof->getName());
      }
      traverse(trans);
   }

   void apply(osg::Group& group) override
   {
      if (osgSim::MultiSwitch* multiSwitch = dynamic_cast<osgSim::MultiSwitch*>(&group))
      {
         m_NodeMultiNames.push_back(multiSwitch->getName());
         osgSim::MultiSwitch::SwitchSetList test = multiSwitch->getSwitchSetList();
         m_SwitchSetSizes.push_back(test.size());
      }
      traverse(group);
   }

   void reset() override
   {
      m_NodeDOFTNames.clear();
      m_NodeMultiNames.clear();
      NodeVisitor::reset();
   }

private:
   ~NodeNameCollector() override {}
   NodeType                 m_Type;
   std::vector<std::string> m_NodeDOFTNames;
   std::vector<std::string> m_NodeMultiNames;
   std::vector<int>         m_SwitchSetSizes;
};

std::vector<std::string> UtoModelShapeImp::GetParts()
{
   osg::ref_ptr<NodeNameCollector> nf = new NodeNameCollector(NodeNameCollector::DOFTRANSFORM);
   m_ModelNode->accept(*nf.get());
   return nf->DOFTransformNodeNames();
}

std::vector<std::string> UtoModelShapeImp::GetParts(osg::ref_ptr<osg::Node> aModelNode)
{
   osg::ref_ptr<NodeNameCollector> nf = new NodeNameCollector(NodeNameCollector::DOFTRANSFORM);
   aModelNode->accept(*nf.get());
   return nf->DOFTransformNodeNames();
}

bool UtoModelShapeImp::HasPart(const std::string& name)
{
   if (m_ModelPart.find(name) != m_ModelPart.end())
      return true;

   osg::ref_ptr<NodeFinder> nf = new NodeFinder(name, NodeFinder::DOFTRANSFORM);
   m_ModelNode->accept(*nf.get());
   if (nf->DOFTransformNode())
   {
      return true;
   }
   return false;
}

UtoModelPartImp& UtoModelShapeImp::FindPart(const std::string& name)
{
   ModelPartMap::iterator it = m_ModelPart.find(name);
   if (it != m_ModelPart.end())
   {
      return *it->second;
   }

   osg::ref_ptr<NodeFinder> nf = new NodeFinder(name, NodeFinder::DOFTRANSFORM);
   m_ModelNode->accept(*nf.get());
   if (osgSim::DOFTransform* dof = nf->DOFTransformNode())
   {
      // If this is a shallow copy, we need to copy the model and re-find the switch
      if (mIsShallowCopy)
      {
         ForceFullCopy();
         return FindPart(name);
      }
      UtoModelPartImp* modelPart = new UtoModelPartImp(dof);
      m_ModelPart.insert(ModelPartMap::value_type(name, modelPart));
      return *modelPart;
   }
   return *m_DummyPart;
}

bool UtoModelShapeImp::HasSwitch(const std::string& name)
{
   if (m_ModelSwitch.find(name) != m_ModelSwitch.end())
      return true;

   osg::ref_ptr<NodeFinder> nf = new NodeFinder(name, NodeFinder::MULTISWITCH);
   m_ModelNode->accept(*nf.get());
   if (nf->MultiSwitchNode())
   {
      return true;
   }
   return false;
}

UtoModelSwitchImp& UtoModelShapeImp::FindSwitch(const std::string& name)
{
   ModelSwitchMap::iterator it = m_ModelSwitch.find(name);
   if (it != m_ModelSwitch.end())
   {
      return *it->second;
   }

   osg::ref_ptr<NodeFinder> nf = new NodeFinder(name, NodeFinder::MULTISWITCH);
   m_ModelNode->accept(*nf.get());
   if (osgSim::MultiSwitch* MultiSwitch = nf->MultiSwitchNode())
   {
      // If this is a shallow copy, we need to copy the model and re-find the switch
      if (mIsShallowCopy)
      {
         ForceFullCopy();
         return FindSwitch(name);
      }
      UtoModelSwitchImp* modelSwitch = new UtoModelSwitchImp(MultiSwitch);
      m_ModelSwitch.insert(ModelSwitchMap::value_type(name, modelSwitch));
      return *modelSwitch;
   }
   return *m_DummySwitch;
}

void UtoModelShapeImp::RebuildSwitchMap()
{
   std::vector<std::string>                            switchNames;
   std::map<std::string, UtoModelSwitchImp*>::iterator it;
   for (it = m_ModelSwitch.begin(); it != m_ModelSwitch.end(); ++it)
   {
      switchNames.push_back(it->first);
   }
   m_ModelSwitch.clear();
   std::vector<std::string>::iterator jt;
   for (jt = switchNames.begin(); jt != switchNames.end(); ++jt)
   {
      FindSwitch(*jt);
   }
}

std::vector<std::string> UtoModelShapeImp::GetSwitches()
{
   osg::ref_ptr<NodeNameCollector> nf = new NodeNameCollector(NodeNameCollector::MULTISWITCH);
   m_ModelNode->accept(*nf.get());
   return nf->MultiSwitchNodeNames();
}

std::vector<int> UtoModelShapeImp::GetSwitchSizes(osg::ref_ptr<osg::Node> aModelNode)
{
   osg::ref_ptr<NodeNameCollector> nf = new NodeNameCollector(NodeNameCollector::MULTISWITCH);
   aModelNode->accept(*nf.get());
   return nf->MultiSwitchNodeSizes();
}

std::vector<std::string> UtoModelShapeImp::GetSwitches(osg::ref_ptr<osg::Node> aModelNode)
{
   osg::ref_ptr<NodeNameCollector> nf = new NodeNameCollector(NodeNameCollector::MULTISWITCH);
   aModelNode->accept(*nf.get());
   return nf->MultiSwitchNodeNames();
}

void UtoModelShapeImp::SetCopyOptions(int copyFlags)
{
   m_CopyFlags = copyFlags;
}

UtoShape* UtoModelShapeImp::CreateModel()
{
   UtoModelShape*    shape = new UtoModelShape;
   UtoModelShapeImp* obj   = static_cast<UtoModelShapeImp*>(shape->Impl());

   obj->m_Data->m_sType = "model";
   if (m_ModelAttributes.empty())
   {
      m_ModelAttributes.insert(AttributeMap::value_type("file", &UtoModelShapeImp::ReadFile));
      m_ModelAttributes.insert(AttributeMap::value_type("rotate_x", &UtoModelShapeImp::ReadRotateX));
      m_ModelAttributes.insert(AttributeMap::value_type("rotate_y", &UtoModelShapeImp::ReadRotateY));
      m_ModelAttributes.insert(AttributeMap::value_type("rotate_z", &UtoModelShapeImp::ReadRotateZ));
      m_ModelAttributes.insert(AttributeMap::value_type("rotate3", &UtoModelShapeImp::ReadRotate3));
      m_ModelAttributes.insert(AttributeMap::value_type("scale3", &UtoModelShapeImp::ReadScale3));
      m_ModelAttributes.insert(AttributeMap::value_type("scale", &UtoModelShapeImp::ReadScale));
      m_ModelAttributes.insert(AttributeMap::value_type("LODscale", &UtoModelShapeImp::ReadLODScale));
      m_ModelAttributes.insert(AttributeMap::value_type("translate", &UtoModelShapeImp::ReadTranslate));
      m_ModelAttributes.insert(AttributeMap::value_type("group", &UtoModelShapeImp::ReadGroup));
      m_ModelAttributes.insert(AttributeMap::value_type("wing_tip", &UtoModelShapeImp::ReadWingTip));
      m_ModelAttributes.insert(AttributeMap::value_type("left_tip", &UtoModelShapeImp::ReadLeftTip));
      m_ModelAttributes.insert(AttributeMap::value_type("right_tip", &UtoModelShapeImp::ReadRightTip));
      m_ModelAttributes.insert(AttributeMap::value_type("pilot_eye", &UtoModelShapeImp::ReadPilotEye));
   }
   obj->m_Attributes = &m_ModelAttributes;

   return shape;
}

UtoShape* UtoModelShapeImp::CreateImage()
{
   UtoModelShape*    shape = new UtoModelShape;
   UtoModelShapeImp* obj   = static_cast<UtoModelShapeImp*>(shape->Impl());

   obj->m_Data->m_sType = "image";
   if (m_ImageAttributes.empty())
   {
      m_ImageAttributes.insert(AttributeMap::value_type("file", &UtoModelShapeImp::ReadFile));
      m_ImageAttributes.insert(AttributeMap::value_type("rotate_x", &UtoModelShapeImp::ReadRotateX));
      m_ImageAttributes.insert(AttributeMap::value_type("rotate_y", &UtoModelShapeImp::ReadRotateY));
      m_ImageAttributes.insert(AttributeMap::value_type("rotate_z", &UtoModelShapeImp::ReadRotateZ));
      m_ImageAttributes.insert(AttributeMap::value_type("rotate3", &UtoModelShapeImp::ReadRotate3));
      m_ImageAttributes.insert(AttributeMap::value_type("scale3", &UtoModelShapeImp::ReadScale3));
      m_ImageAttributes.insert(AttributeMap::value_type("scale", &UtoModelShapeImp::ReadScale));
      m_ImageAttributes.insert(AttributeMap::value_type("translate", &UtoModelShapeImp::ReadTranslate));
      m_ImageAttributes.insert(AttributeMap::value_type("group", &UtoModelShapeImp::ReadGroup));
      m_ImageAttributes.insert(AttributeMap::value_type("width", &UtoModelShapeImp::ReadWidth));
      m_ImageAttributes.insert(AttributeMap::value_type("height", &UtoModelShapeImp::ReadHeight));
      m_ImageAttributes.insert(AttributeMap::value_type("no_antialias", &UtoModelShapeImp::ReadNoAntiAlias));
      m_ImageAttributes.insert(AttributeMap::value_type("no_mipmapping", &UtoModelShapeImp::ReadNoMipMapping));
      m_ImageAttributes.insert(AttributeMap::value_type("antialias", &UtoModelShapeImp::ReadAntiAlias));
      m_ImageAttributes.insert(AttributeMap::value_type("mipmapping", &UtoModelShapeImp::ReadMipMapping));
      m_ImageAttributes.insert(AttributeMap::value_type("flags", &UtoModelShapeImp::ReadShapeFlags));
      m_ImageAttributes.insert(AttributeMap::value_type("rect", &UtoModelShapeImp::ReadRect));
      m_ImageAttributes.insert(AttributeMap::value_type("invert_y", &UtoModelShapeImp::ReadInvertY));
   }
   obj->m_Attributes = &m_ImageAttributes;

   return shape;
}

bool UtoModelShapeImp::PreLoad()
{
   m_LoadData               = new LoadData;
   m_LoadData->is_model     = m_Data->m_sType.compare(0, 5, "model") == 0;
   m_LoadData->is_image     = m_Data->m_sType.compare(0, 5, "image") == 0;
   m_LoadData->is_subimage  = false;
   m_LoadData->is_loaded    = false;
   m_LoadData->width        = 0;
   m_LoadData->height       = 0;
   m_LoadData->SImgRect.x0  = 0;
   m_LoadData->SImgRect.y0  = 0;
   m_LoadData->SImgRect.x1  = 0;
   m_LoadData->SImgRect.y1  = 0;
   m_LoadData->bInvertRectY = false;
   m_LoadData->nShapeFlags  = 0;
   m_LoadData->anti_alias   = UtoOptions::Instance()->UseAntiAliasDefault();
   m_LoadData->mip_mapping  = UtoOptions::Instance()->UseMipMappingDefault();
   // The default scale is 1.
   m_LoadData->scale = osg::Vec3(1, 1, 1);
   return true;
}

// OpenFlight animations are not supported by Uto.  The only sane thing to
// do at this point is to turn off the animations so an application can
// move the parts if desired.
// If the resource database scales the shape then the normals must be
// scaled independently to keep them of unit length.
class ModelUpdateVisitor : public osg::NodeVisitor
{
public:
   friend class osg::NodeVisitor;

   ModelUpdateVisitor(bool UpdateNormals, const osg::Vec3 scaler)
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      , m_UpdateNormals(UpdateNormals)
      , m_X(scaler.x())
      , m_Y(scaler.y())
      , m_Z(scaler.z())
   {
   }

   void apply(osg::Geode& geode) override
   {
      if (m_UpdateNormals)
      {
         osg::Geometry*           geometry;
         osg::Vec3Array*          normal;
         osg::Vec3Array::iterator v, vend;
         int                      stop = geode.getNumDrawables();
         for (int i = 0; i < stop; ++i)
         {
            geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
            if (geometry)
            {
               normal = static_cast<osg::Vec3Array*>(geometry->getNormalArray());
               if (normal)
               {
                  for (v = normal->begin(), vend = normal->end(); v != vend; ++v)
                  {
                     // counter the scaling in the matrix
                     v->x() *= m_X;
                     v->y() *= m_Y;
                     v->z() *= m_Z;
                  }
               }
            }
         }
      }
      NodeVisitor::apply(geode);
   }

   void apply(osg::Transform& trans) override
   {
      osgSim::DOFTransform* dof = dynamic_cast<osgSim::DOFTransform*>(&trans);
      if (dof)
      {
         dof->setAnimationOn(false);
      }
      NodeVisitor::apply(trans);
   }

private:
   ~ModelUpdateVisitor() override {}
   bool   m_UpdateNormals;
   double m_X, m_Y, m_Z;
};

bool UtoModelShapeImp::PostLoad()
{
   bool model_loaded = m_LoadData->is_loaded;

   // test for image.
   if (m_LoadData->is_image && !m_LoadData->file_name.empty())
   {
      bool           translucent;
      osg::Geometry* geometry = UtoTexturedPolygonImp::CreateModel(m_LoadData->file_name,
                                                                   translucent,
                                                                   m_LoadData->width,
                                                                   m_LoadData->height,
                                                                   m_LoadData->anti_alias,
                                                                   m_LoadData->mip_mapping,
                                                                   UtoTexturedPolygonImp::WRAP_REPEAT);
      m_TexturedPolygon       = true;

      if (geometry)
      {
         osg::Geode* geode = new osg::Geode;
         geode->addDrawable(geometry);
         m_ModelNode = geode;
         m_ModelNode->setName(m_Data->m_sType + ": " + m_LoadData->file_name.c_str());

         m_ColorArray = dynamic_cast<osg::Vec4Array*>(geometry->getColorArray());
         assert(m_ColorArray.get());

         /*osg::Vec3Array *tex_coords = dynamic_cast<osg::Vec3Array*>(
            geometry->getVertexArray());
          ASSERT(tex_coords);
          SCALE_VEC3(tex_coords->at(0),m_LoadData->scale);
          SCALE_VEC3(tex_coords->at(1),m_LoadData->scale);
          SCALE_VEC3(tex_coords->at(2),m_LoadData->scale);
          SCALE_VEC3(tex_coords->at(3),m_LoadData->scale);*/

         AddShapeChildNode(m_ModelNode.get());
         SetShapeFlags(SHARED_STATESET, SHARED_STATESET);
         model_loaded = true;
      }
   }
   else if (m_LoadData->is_subimage && !m_LoadData->file_name.empty())
   {
      // local(s)
      bool bTransparent = false;
      // create a new sub texture object
      osg::Geometry* pGeometry = UtoTexturedPolygonImp::CreateSubImage(m_LoadData->file_name,
                                                                       m_LoadData->SImgRect.x0,
                                                                       m_LoadData->SImgRect.y0,
                                                                       m_LoadData->SImgRect.x1,
                                                                       m_LoadData->SImgRect.y1,
                                                                       m_LoadData->width,
                                                                       m_LoadData->height,
                                                                       m_LoadData->bInvertRectY,
                                                                       bTransparent);

      // validate that the geometry was created
      if (pGeometry)
      {
         // set the color index pointer
         m_ColorArray = dynamic_cast<osg::Vec4Array*>(pGeometry->getColorArray());

         // create a new geode for the geometry
         osg::Geode* pGeode = new osg::Geode;
         // bind the geometry to the geode
         pGeode->addDrawable(pGeometry);
         // attach to the main node for the class
         m_ModelNode = pGeode;
         // set the attributes for the node
         m_ModelNode->setName(m_Data->m_sType + ": " + m_LoadData->file_name.c_str());
         // add to the top level node
         AddShapeChildNode(m_ModelNode.get());
         // set the flags for this shape
         SetShapeFlags(SHARED_STATESET, SHARED_STATESET);

         // ladies and gents we have a textured quad
         m_TexturedPolygon = true;

         model_loaded = true;
      }
   }
   else
   {
      if (model_loaded && m_ModelNode.get())
      {
         osg::Vec3 scale = m_LoadData->scale;
         // If the model is scaled the normals must be scaled as well, or
         // the lighting doesn't come out right.
         bool DoScaling = false;
         if (scale.x() != 1.0 || (scale.y() != 1.0 && scale.z() != 1.0))
         {
            DoScaling = true;
            /*
            scale.x() = 1.0/scale.x();
            scale.y() = 1.0/scale.y();
            scale.z() = 1.0/scale.z();
            */
         }
         osg::ref_ptr<ModelUpdateVisitor> visitor = new ModelUpdateVisitor(DoScaling, scale);
         m_ModelNode->accept(*visitor.get());
      }
   }

   SetupStateSet();

   // apply the shape flags if present
   if (m_LoadData->nShapeFlags)
      SetShapeFlags(m_LoadData->nShapeFlags, m_LoadData->nShapeFlags);

   delete m_LoadData;
   m_LoadData = nullptr;

   return model_loaded;
}

bool UtoModelShapeImp::ReadAttribute(const std::string& attr, const std::string& buf)
{
   assert(m_Attributes);

   AttributeMap::iterator it = m_Attributes->find(attr.c_str());
   if (it != m_Attributes->end())
   {
      ReadAttrFnPtr fn = it->second;
      (this->*fn)(attr, buf);
   }
   else
   {
      // unknown field so insert as a user metric.
      ReadOther(attr, buf);
   }
   return true;
}

void UtoModelShapeImp::ReadFile(const std::string& attr, const std::string& buf)
{
   // set model filename.
   if (m_LoadData->is_model)
   {
      if (ReadModelFile(buf) == 0)
      {
         m_LoadData->is_loaded = true;
      }
   }
   else
   {
      ReadImageFile(buf);
   }
}

void UtoModelShapeImp::ReadRotateX(const std::string& attr, const std::string& buf)
{
   double val;
   // only bother if the angle isn't zero
   if (Utok::ReadSingle(buf, val) == 0 && val)
   {
      *GetOrCreateStaticRotation() *= osg::Matrix::rotate(DtoR(val), osg::Vec3(1, 0, 0));
   }
}

void UtoModelShapeImp::ReadRotateY(const std::string& attr, const std::string& buf)
{
   double val;
   if (Utok::ReadSingle(buf, val) == 0 && val)
   {
      *GetOrCreateStaticRotation() *= osg::Matrix::rotate(DtoR(val), osg::Vec3(0, 1, 0));
   }
}

void UtoModelShapeImp::ReadRotateZ(const std::string& attr, const std::string& buf)
{
   double val;
   if (Utok::ReadSingle(buf, val) == 0 && val)
   {
      *GetOrCreateStaticRotation() *= osg::Matrix::rotate(DtoR(val), osg::Vec3(0, 0, 1));
   }
}

void UtoModelShapeImp::ReadRotate3(const std::string& attr, const std::string& buf)
{
   double val[3];
   if (Utok::ReadTriple(buf, val) == 0 && (val[0] || val[1] || val[2]))
   {
      *GetOrCreateStaticRotation() *= osg::Matrix::rotate(DtoR(val[2]),
                                                          osg::Vec3(0, 0, 1),
                                                          DtoR(val[1]),
                                                          osg::Vec3(0, 1, 0),
                                                          DtoR(val[0]),
                                                          osg::Vec3(1, 0, 0));
   }
}

void UtoModelShapeImp::ReadScale3(const std::string& attr, const std::string& buf)
{
   double val[3];
   if (Utok::ReadTriple(buf, val) == 0)
   {
      if (fabs(val[0]) == fabs(val[1]) && fabs(val[1]) == fabs(val[2]))
      {
         // Store the scale's magnitude.
         m_StaticScale *= fabs(val[0]);

         // Remove the scale's magnitude, but leave the sign.
         val[0] = (val[0] < 0.0) ? -1.0 : 1.0;
         val[1] = (val[1] < 0.0) ? -1.0 : 1.0;
         val[2] = (val[2] < 0.0) ? -1.0 : 1.0;
      }
      *GetOrCreateStaticRotation() *= osg::Matrix::scale(val[2], val[0], val[1]);

      m_LoadData->scale.x() *= val[2];
      m_LoadData->scale.y() *= val[0];
      m_LoadData->scale.z() *= val[1];
   }
}

void UtoModelShapeImp::ReadScale(const std::string& attr, const std::string& buf)
{
   double val;
   if (Utok::ReadSingle(buf, val) == 0)
   {
      // Store the scale's magnitude.
      m_StaticScale *= fabs(val);

      // Remove the scale's magnitude, but leave the sign.
      val = (val < 0.0) ? -1.0 : 1.0;
      *GetOrCreateStaticRotation() *= osg::Matrix::scale(val, val, val);

      m_LoadData->scale.x() *= val;
      m_LoadData->scale.y() *= val;
      m_LoadData->scale.z() *= val;
   }
}

void UtoModelShapeImp::ReadLODScale(const std::string& attr, const std::string& buf) {}

void UtoModelShapeImp::ReadTranslate(const std::string& attr, const std::string& buf)
{
   // set model translation.
   double val[3];
   if (Utok::ReadTriple(buf, val) == 0)
   {
      UtoPosition offset(val[0], val[1], val[2]);
      m_StaticOffset += offset;
   }
}

void UtoModelShapeImp::ReadWingTip(const std::string& attr, const std::string& buf)
{
   // set model wingtips(symmetric).
   double val[3];
   if (Utok::ReadTriple(buf, val) == 0)
   {
      m_Data->m_Metrics.insert(std::make_pair(std::string("right_tip_x"), val[0]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("right_tip_y"), val[1]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("right_tip_z"), val[2]));

      m_Data->m_Metrics.insert(std::make_pair(std::string("left_tip_x"), -val[0]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("left_tip_y"), val[1]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("left_tip_z"), val[2]));
   }
}

void UtoModelShapeImp::ReadRightTip(const std::string& attr, const std::string& buf)
{
   // set model right wing tip.
   double val[3];
   if (Utok::ReadTriple(buf, val) == 0)
   {
      m_Data->m_Metrics.insert(std::make_pair(std::string("right_tip_x"), val[0]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("right_tip_y"), val[1]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("right_tip_z"), val[2]));
   }
}

void UtoModelShapeImp::ReadPilotEye(const std::string& attr, const std::string& buf)
{
   // set model pilot eye.
   double val[3];
   if (Utok::ReadTriple(buf, val) == 0)
   {
      m_Data->m_Metrics.insert(std::make_pair(std::string("pilot_eye_x"), val[0]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("pilot_eye_y"), val[1]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("pilot_eye_z"), val[2]));
   }
}

void UtoModelShapeImp::ReadLeftTip(const std::string& attr, const std::string& buf)
{
   // set model left wing tip.
   double val[3];
   if (Utok::ReadTriple(buf, val) == 0)
   {
      m_Data->m_Metrics.insert(std::make_pair(std::string("left_tip_x"), val[0]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("left_tip_y"), val[1]));
      m_Data->m_Metrics.insert(std::make_pair(std::string("left_tip_z"), val[2]));
   }
}

void UtoModelShapeImp::ReadWidth(const std::string& attr, const std::string& buf)
{
   Utok::ReadSingle(buf, m_LoadData->width);
}

void UtoModelShapeImp::ReadHeight(const std::string& attr, const std::string& buf)
{
   Utok::ReadSingle(buf, m_LoadData->height);
}

void UtoModelShapeImp::ReadNoAntiAlias(const std::string& attr, const std::string& buf)
{
   m_LoadData->anti_alias = false;
}

void UtoModelShapeImp::ReadNoMipMapping(const std::string& attr, const std::string& buf)
{
   m_LoadData->mip_mapping = false;
}

void UtoModelShapeImp::ReadAntiAlias(const std::string& attr, const std::string& buf)
{
   m_LoadData->anti_alias = true;
}

void UtoModelShapeImp::ReadMipMapping(const std::string& attr, const std::string& buf)
{
   m_LoadData->mip_mapping = true;
}

void UtoModelShapeImp::ReadBbox(const std::string& attr, const std::string& in)
{
   // "bbox <xmin> <xmax> <ymin> <ymax> <zmin> <zmax>"
   std::istringstream sstr(in);
   std::string        name;
   double             value;

   sstr >> value;
   int size = name.size() + 1;
   if (!sstr.fail())
   {
      name += "_xmin";
      m_Data->m_Metrics.insert(std::make_pair(name, value));
   }

   sstr >> value;
   if (!sstr.fail())
   {
      name.replace(size, 4, "xmax");
      m_Data->m_Metrics.insert(std::make_pair(name, value));
   }

   sstr >> value;
   if (!sstr.fail())
   {
      name.replace(size, 4, "ymin");
      m_Data->m_Metrics.insert(std::make_pair(name, value));
   }

   sstr >> value;
   if (!sstr.fail())
   {
      name.replace(size, 4, "ymax");
      m_Data->m_Metrics.insert(std::make_pair(name, value));
   }

   sstr >> value;
   if (!sstr.fail())
   {
      name.replace(size, 4, "zmin");
      m_Data->m_Metrics.insert(std::make_pair(name, value));
   }

   sstr >> value;
   if (!sstr.fail())
   {
      name.replace(size, 4, "zmax");
      m_Data->m_Metrics.insert(std::make_pair(name, value));
   }
}

void UtoModelShapeImp::ReadGroup(const std::string& attr, const std::string& in)
{
   // "group <group1>"
   std::istringstream sstr(in);
   std::string        name, group;

   sstr >> group;
   if (!sstr.fail())
   {
      m_Data->m_Groups.push_back(group);
      // DEBUG_LEV(3, ("Model shape %s added group %s\n",
      //                 m_Data->m_sType.c_str(), group.c_str()));
   }
}

void UtoModelShapeImp::ReadOther(const std::string& attr, const std::string& in)
{
   // "<metric> <value>"
   double             value;
   std::istringstream sstr(in);

   sstr >> value;
   if (!sstr.fail())
   {
      m_Data->m_Metrics.insert(std::make_pair(attr, value));
   }
}

/*void UtoModelShapeImp::Dump(std::ostream &o)
{
   o << "  Type: UtoModelShape" << std::endl;
   o << "  File: " << m_sFileName << std::endl;
   o << "  Section: " << m_sSection << std::endl;
   o << "  Metrics: " << std::endl;

   MetricMap::iterator it, end(m_Metrics.end());
   for(it = m_Metrics.begin(); it != end; ++it)
   {
      o << "    " << it->first << " : " << it->second << std::endl;
   }

   o << "  Groups: " << std::endl;

   GroupVec::iterator it2, end2 (m_Groups.end());
   for(it2 = m_Groups.begin(); it2 != end2; ++it2)
   {
      o << "    " << *it2 << std::endl;
   }

   if(m_pModel)
   {
      if(UtoDebug::Instance()->Verbose() >= 2)
      {
         ostrstream ostr;
         m_pModel->DumpTree(ostr);
         o << ostr.str();
      }
   }
}
*/

int UtoModelShapeImp::ReadModelFile(const std::string& in)
{
   int success = -1;

   // "file <filename>"
   if (!in.empty())
   {
      std::string value = in;

      std::string file_path = UtoFindFirstPath(value, UtoOptions::Instance()->ModelsPath());

      m_ModelNode = osgDB::readNodeFile(file_path.c_str());
      if (m_ModelNode.get())
      {
         osgUtil::Optimizer optimizer;
         optimizer.optimize(m_ModelNode.get());
         success = 0;
         m_ModelNode->setName(m_Data->m_sType + ": " + file_path.c_str());
         AddShapeChildNode(m_ModelNode.get());
      }
      else
      {
         // DEBUG(("Failed to load model %s", file_path.c_str()));
      }
   }

   return success;
}

int UtoModelShapeImp::ReadImageFile(const std::string& in)
{
   int success = 0;

   // "file <filename>"
   if (!in.empty())
   {
      std::string value = in;

      m_LoadData->file_name = UtoFindFirstPath(value, UtoOptions::Instance()->ModelsPath());
   }

   return success;
}

void UtoModelShapeImp::ReadRect(const std::string& attr, const std::string& buf)
{
   // construct the input string stream
   std::istringstream iStream(buf);

   // read the values from the stream
   iStream >> m_LoadData->SImgRect.x0;
   iStream >> m_LoadData->SImgRect.y0;
   iStream >> m_LoadData->SImgRect.x1;
   iStream >> m_LoadData->SImgRect.y1;

   // reading a rect means the image is a subimage
   m_LoadData->is_subimage = true;
   m_LoadData->is_image    = false;
}

void UtoModelShapeImp::ReadInvertY(const std::string& attr, const std::string& buf)
{
   // read the buffer
   m_LoadData->bInvertRectY = buf.compare(0, 4, "true") == 0;
}

void UtoModelShapeImp::ReadShapeFlags(const std::string& attr, const std::string& buf)
{
   // clear the shape flags if present
   m_LoadData->nShapeFlags = 0;

   // define current position
   std::string::size_type nCurrent = buf.size() ? 0 : std::string::npos;

   // read the attributes from the buffer
   while (nCurrent != std::string::npos)
   {
      // obtain the marker position
      std::string::size_type nMarker = buf.find('|', nCurrent);
      // substring the value
      std::string sFlag = buf.substr(nCurrent, nMarker != std::string::npos ? nMarker - nCurrent : nMarker);
      // trim the whitespace
      std::string::size_type nBegin = sFlag.find_first_not_of(' ');
      std::string::size_type nEnd   = sFlag.find_last_not_of(' ');
      // set the beginning up incase there was no whitespace at the beginning
      if (nBegin == std::string::npos)
         nBegin = 0;
      if (nEnd != std::string::npos)
         nEnd = nEnd - nBegin + 1;
      // do the final substring
      sFlag = sFlag.substr(nBegin, nEnd);
      // set the shape flag
      if (STRICASECMP(sFlag.c_str(), "billboard") == 0)
         m_LoadData->nShapeFlags |= UtoShape::BILLBOARD;
      else if (STRICASECMP(sFlag.c_str(), "autoscale") == 0)
         m_LoadData->nShapeFlags |= UtoShape::AUTO_SCALE;
      // update the current position
      nMarker != std::string::npos ? nCurrent += (nMarker - nCurrent + 1) : nCurrent = nMarker;
   }
}

void UtoModelShapeImp::SetColor(const UtoColor& Color, unsigned int nClrMask, unsigned int nReserved)
{
   // call the base class
   UtoShapeImp::SetColor(Color, nClrMask, nReserved);
   // set color states
   SetupColorState();
}

void UtoModelShapeImp::SetupColorState()
{
   // first, if the model is an image and is shared then we need to make clone
   // two references, #1: m_ModelNode, #2: UtoShapeImp::AddChildNode
   osg::Geometry* geometry = nullptr;
   if (m_TexturedPolygon)
   {
      if (m_ModelNode->referenceCount() > 2)
      {
         // determine the type of items to copy
         // we removed images and textures so as they can be shared by the
         // objects
         osg::CopyOp::CopyFlags nCopyFlags =
            osg::CopyOp::DEEP_COPY_OBJECTS | osg::CopyOp::DEEP_COPY_NODES | osg::CopyOp::DEEP_COPY_DRAWABLES |
            osg::CopyOp::DEEP_COPY_ARRAYS | osg::CopyOp::DEEP_COPY_PRIMITIVES | osg::CopyOp::DEEP_COPY_SHAPES |
            osg::CopyOp::DEEP_COPY_STATESETS | osg::CopyOp::DEEP_COPY_STATEATTRIBUTES | osg::CopyOp::DEEP_COPY_UNIFORMS;

         osg::Geode* geode = dynamic_cast<osg::Geode*>(m_ModelNode->clone(nCopyFlags));
         if (geode)
         {
            RemoveShapeChildNode(m_ModelNode.get());

            geometry = geode->getDrawable(0)->asGeometry();
            assert(geometry);
            if (geometry)
            {
               m_ColorArray = dynamic_cast<osg::Vec4Array*>(geometry->getColorArray());
            }
            assert(m_ColorArray.get());

            AddShapeChildNode(geode);
            m_ModelNode = geode;
         }
         else
         {
            assert(!"Failed to clone a model");
         }
      }
      else
      {
         geometry = dynamic_cast<osg::Geode*>(m_ModelNode.get())->getDrawable(0)->asGeometry();
      }
   }

   osg::Vec4 v4color(m_oBlendColor.RedF(), m_oBlendColor.GreenF(), m_oBlendColor.BlueF(), m_oBlendColor.AlphaF());

   if (geometry)
   {
      m_ColorArray->at(0) = v4color;
      geometry->dirtyDisplayList();
   }
}

void UtoModelShapeImp::SetupStateSet()
{
   osg::ref_ptr<osg::StateSet> stateset = m_RootNode->getOrCreateStateSet();

   // Textured Polygons look much better when blending is enabled.
   // The border is blended with the background even when the image's
   // pixels are fully transparent or fully opaque.  It looks bad enough
   // that we don't allow the user to disable blending for 2D models.
   if (m_TexturedPolygon || m_oBlendColor.HasTransparency())
   {
      stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
      if (stateset->getNestRenderBins())
      {
         stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      }
   }
   else
   {
      stateset->setMode(GL_BLEND, osg::StateAttribute::OFF);
      if (stateset->getNestRenderBins())
      {
         stateset->setRenderBinToInherit();
      }
   }
}

void UtoModelShapeImp::SetDepthTesting(bool aState)
{
   osg::ref_ptr<osg::StateSet> stateset = m_RootNode->getOrCreateStateSet();

   stateset->setMode(GL_DEPTH_TEST, aState);
}

void UtoModelShapeImp::SetLighting(bool aState)
{
   osg::ref_ptr<osg::StateSet> stateset = m_RootNode->getOrCreateStateSet();

   stateset->setMode(GL_LIGHTING, aState);
}

// Some shapes are loaded with a shallow copy initially,
// and then only fully copy later if necessary
void UtoModelShapeImp::FullCopy()
{
   mIsShallowCopy = false;
   m_CopyFlags |= osg::CopyOp::DEEP_COPY_OBJECTS | osg::CopyOp::DEEP_COPY_NODES;
   if (m_ModelNode.get())
   {
      RemoveShapeChildNode(m_ModelNode.get());
      m_ModelNode = dynamic_cast<osg::Node*>(m_ModelNode->clone(m_CopyFlags));
      if (m_ModelNode.get())
      {
         AddShapeChildNode(m_ModelNode);
      }
   }
}

osg::Matrix* UtoModelShapeImp::GetStaticRotation()
{
   return m_StaticRotation.get();
}
