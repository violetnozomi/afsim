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

#include "UtoTreeDump.hpp"

#include <iostream>
#include <sstream>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Sequence>
#include <osg/StateSet>
#include <osg/Switch>
#include <osg/Transform>

#include "UtLog.hpp"

UtoTreeDump::UtoTreeDump()
   : NextNode_(1)
   , out_(nullptr)
{
   NodeAttribute_.insert(NodeAttributeMap::value_type(TYPE_DEFAULT, "shape = hexagon, color = lightblue2, style = filled"));
   NodeAttribute_.insert(NodeAttributeMap::value_type(TYPE_GROUP, "shape = ellipse, color = lightcyan"));
   NodeAttribute_.insert(NodeAttributeMap::value_type(TYPE_GEODE, "shape = house, color = hotpink"));
   NodeAttribute_.insert(NodeAttributeMap::value_type(TYPE_SWITCH, "shape = invhouse, color = red1"));
   NodeAttribute_.insert(NodeAttributeMap::value_type(TYPE_SEQUENCE, "shape = invhouse, color = red1"));
   NodeAttribute_.insert(NodeAttributeMap::value_type(TYPE_TRANSFORM, "shape = parallelogram, color = springgreen"));
   NodeAttribute_.insert(NodeAttributeMap::value_type(TYPE_ARRAY, "shape = octagon, color = maroon1"));
   NodeAttribute_.insert(NodeAttributeMap::value_type(TYPE_DRAWABLE, "shape = octagon, color = maroon1"));
   NodeAttribute_.insert(NodeAttributeMap::value_type(TYPE_STATESET, "shape = ellipse"));
}

UtoTreeDump::~UtoTreeDump() {}

int UtoTreeDump::Dump(std::ostream& out_arg, osg::Node* tree, SkipList ignore)
{
   // reset the tree for another dump
   if (NodeDisplayed_.size())
      NodeDisplayed_.clear();
   if (NodeNumber_.size())
      NodeNumber_.clear();
   NextNode_ = 1;

   out_    = &out_arg;
   ignore_ = ignore;
   *out_ << "digraph SceneTree {\n"
            "//size=\"45,300\";\n"
            "//page=\"8.4,10.9\";\n"
            "// Landscape;\n"
            "// Usage: dot -Tps file.dot -o file.ps\n"
            "//rotate=\"90\";\n";
   NodeAttributeMap::iterator attribute = NodeAttribute_.find(TYPE_DEFAULT);
   if (attribute != NodeAttribute_.end())
      *out_ << "node [ " << attribute->second << " ];\n";

   ProcessNode(tree);
   *out_ << "}\n";
   return NodeNumber_.size();
}

void UtoTreeDump::NodeAttributeClear()
{
   NodeAttribute_.clear();
}

void UtoTreeDump::NodeAttributeSet(ShapeColorType entry, const std::string& attributes)
{
   NodeAttribute_[entry] = attributes;
}

std::string UtoTreeDump::NodeAttributeGet(ShapeColorType entry)
{
   NodeAttributeMap::iterator value = NodeAttribute_.find(entry);
   if (value == NodeAttribute_.end())
      return std::string();
   else
      return value->second;
}

void UtoTreeDump::PrintInfo(int index, void* address, const std::string& TypeInfo)
{
   if (NodeDisplayed_.find(address) != NodeDisplayed_.end())
      return;
   NodeDisplayed_.insert(address);

   *out_ << "I" << index << " [ label = \"" << TypeInfo;
   *out_ << "\\nIndex " << index;
   *out_ << "\\nAddress 0x" << address << "\" ];\n";
}

void UtoTreeDump::PrintNodeInfo(osg::Node* node)
{
   if (NodeDisplayed_.find(node) != NodeDisplayed_.end())
      return;
   NodeDisplayed_.insert(node);

   int index = AddressIndex(node);
   *out_ << "I" << index << " [ label = \"" << node->className();

   std::string name = node->getName();
   if (!name.empty())
      *out_ << "\\n" << name;

   *out_ << "\\nIndex " << index;
   *out_ << "\\nAddress 0x" << (void*)node << "\"\n\t";

   // remember order is important, pick the first type which the
   // node can be converted to and has an entry in the hash map
   NodeAttributeMap::iterator attribute, end = NodeAttribute_.end();
   if ((dynamic_cast<osg::Geode*>(node) && (attribute = NodeAttribute_.find(TYPE_GEODE)) != end) ||
       (dynamic_cast<osg::Switch*>(node) && (attribute = NodeAttribute_.find(TYPE_SWITCH)) != end) ||
       (dynamic_cast<osg::Sequence*>(node) && (attribute = NodeAttribute_.find(TYPE_SEQUENCE)) != end) ||
       (dynamic_cast<osg::Transform*>(node) && (attribute = NodeAttribute_.find(TYPE_TRANSFORM)) != end) ||
       (dynamic_cast<osg::Group*>(node) && (attribute = NodeAttribute_.find(TYPE_GROUP)) != end))
   {
      *out_ << attribute->second;
   }


   *out_ << " ];\n";
}

void UtoTreeDump::PrintDrawableInfo(osg::Drawable* drawable, const std::string& extra)
{
   if (NodeDisplayed_.find(drawable) != NodeDisplayed_.end())
      return;
   NodeDisplayed_.insert(drawable);

   int index = AddressIndex(drawable);
   *out_ << "I" << index << " [ label = \"" << drawable->className();
   std::string name = drawable->getName();
   if (!name.empty())
      *out_ << "\\n" << name;
   *out_ << "\\nIndex " << index;
   *out_ << "\\nAddress 0x" << (void*)drawable;
   if (!extra.empty())
      *out_ << "\\n" << extra;

   *out_ << "\"\n\t";

   NodeAttributeMap::iterator attribute = NodeAttribute_.find(TYPE_DRAWABLE);
   if (attribute != NodeAttribute_.end())
      *out_ << attribute->second;

   *out_ << " ];\n";
}

void UtoTreeDump::PrintStateSetInfo(osg::StateSet* stateset, int parentId, const std::string& extra)
{
   if (NodeDisplayed_.find(stateset) != NodeDisplayed_.end())
      return;
   NodeDisplayed_.insert(stateset);

   int index = AddressIndex(stateset);
   // A state isn't a child node so display it on the same level as
   // the node that it is connected to.
   *out_ << "subgraph s" << index << " {\n";
   *out_ << "rank = same;\n";
   *out_ << "I" << index << " [ label = \"" << stateset->className();
   *out_ << "\\nIndex " << index;
   *out_ << "\\nAddress 0x" << (void*)stateset;
   if (!extra.empty())
      *out_ << extra;

   *out_ << "\"\n\t";

   NodeAttributeMap::iterator attribute = NodeAttribute_.find(TYPE_STATESET);
   if (attribute != NodeAttribute_.end())
      *out_ << attribute->second;

   *out_ << " ];\n";
   *out_ << "I" << parentId << ";\n";
   *out_ << "}\n";
}

/* PrintArrayInfo provides a generic way to print all different types of
 * arrays just by giving the parent, address, how many elements in the array,
 * and a string identifying what type it is.
 */
void UtoTreeDump::PrintArrayInfo(int parent, void* address, int size, const std::string& TypeInfo)
{
   int index = AddressIndex(address);
   *out_ << "I" << index << " [ label = \"Array\\n" << TypeInfo;
   *out_ << "\\nSize " << size;
   *out_ << "\\nIndex " << index;
   *out_ << "\\nAddress 0x" << address << "\"\n\t";
   *out_ << "shape = tripleoctagon, color = magenta1";
   *out_ << "];\n";
   *out_ << "I" << parent << " -> I" << index << ";\n";
}

void UtoTreeDump::PrintEdge(int from, int to, const std::string& Note)
{
   // skip a parent node of 0 as it is above the start node
   if (from)
   {
      *out_ << "I" << from << " -> I" << to;
      if (!Note.empty())
         *out_ << " [ label = \"" << Note << "\" ]";
      *out_ << ";\n";
   }
}

/* Lookup the address in the hash map, if there is an entry, return that
 * number, otherwise insert it in the hash assigning it the next available
 * address.
 */
int UtoTreeDump::AddressIndex(void* address)
{
   int               index;
   NodeMap::iterator entry = NodeNumber_.find(address);
   if (entry == NodeNumber_.end())
   {
      index = NextNode_++;
      NodeNumber_.insert(NodeMap::value_type(address, index));
   }
   else
   {
      index = entry->second;
   }
   return index;
}

void UtoTreeDump::ProcessNode(osg::Node* node)
{
   if (ignore_ & SKIP_NODE)
      return;
   if (osg::StateSet* stateset = node->getStateSet())
   {
      int i = AddressIndex(node);
      PrintEdge(i, AddressIndex(stateset));
      ProcessStateSet(stateset, i);
   }
   if (osg::Group* group = dynamic_cast<osg::Group*>(node))
   {
      ProcessGroup(group);
   }
   else if (osg::Geode* geode = dynamic_cast<osg::Geode*>(node))
   {
      ProcessGeode(geode);
   }
   else
   {
      // not a type that needs further procesing
      PrintNodeInfo(node);
   }
}

void UtoTreeDump::ProcessGroup(osg::Group* group)
{
   if (ignore_ & SKIP_GROUP)
      return;

   PrintNodeInfo(group);

   bool         labelEdge = false;
   osg::Switch* Switch    = dynamic_cast<osg::Switch*>(group);
   if (dynamic_cast<osg::Sequence*>(group) || Switch)
   {
      labelEdge = true;
   }
   std::ostringstream sstr;
   osg::Node*         node;
   int                index = AddressIndex(group);
   int                count = group->getNumChildren();
   for (int i = 0; i < count; i++)
   {
      if (labelEdge)
      {
         sstr.str("");
         if (Switch)
            sstr << (Switch->getValue(i) ? '+' : '0');
         sstr << i;
      }
      node = group->getChild(i);
      PrintEdge(index, AddressIndex(node), sstr.str());
      ProcessNode(node);
   }
}

void UtoTreeDump::ProcessStateSet(osg::StateSet* stateset, int parentId)
{
   if (ignore_ & SKIP_STATESET)
      return;

   std::string                            attribute;
   osg::StateSet::AttributeList&          v   = stateset->getAttributeList();
   osg::StateSet::AttributeList::iterator cur = v.begin(), end = v.end();
   for (; cur != end; ++cur)
   {
      attribute += "\\n";
      attribute += cur->second.first->className();
   }

   osg::StateSet::TextureAttributeList&          tv   = stateset->getTextureAttributeList();
   osg::StateSet::TextureAttributeList::iterator tcur = tv.begin(), tend = tv.end();
   for (; tcur != tend; ++tcur)
   {
      cur = tcur[0].begin();
      end = tcur[0].end();
      for (; cur != end; ++cur)
      {
         attribute += "\\n";
         attribute += cur->second.first->className();
      }
   }

   PrintStateSetInfo(stateset, parentId, attribute);
}

void UtoTreeDump::ProcessGeode(osg::Geode* geode)
{
   if (ignore_ & SKIP_GEODE)
      return;
   PrintNodeInfo(geode);

   int            index = AddressIndex(geode);
   osg::Drawable* drawable;
   int            count = geode->getNumDrawables();
   for (int i = 0; i < count; i++)
   {
      drawable = geode->getDrawable(i);
      PrintEdge(index, AddressIndex(drawable));
      ProcessDrawable(drawable);
   }
}

void UtoTreeDump::ProcessDrawable(osg::Drawable* drawable)
{
   if (ignore_ & SKIP_DRAWABLE)
      return;
   if (osg::StateSet* stateset = drawable->getStateSet())
   {
      int i = AddressIndex(drawable);
      PrintEdge(i, AddressIndex(stateset));
      ProcessStateSet(stateset, i);
   }
   if (osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(drawable))
   {
      ProcessGeometry(geometry);
   }
   else
   {
      PrintDrawableInfo(drawable);
   }
}

void UtoTreeDump::ProcessGeometry(osg::Geometry* geometry)
{
   if (ignore_ & SKIP_GEOMETRY)
      return;
   int                index = AddressIndex(geometry);
   std::ostringstream sstr;
   sstr << "#PrimitiveSets " << geometry->getNumPrimitiveSets();
   PrintDrawableInfo(geometry, sstr.str());

   if (ignore_ & SKIP_GEOMETRY_ARRAY)
      return;

   if (osg::Array* array = geometry->getVertexArray())
      PrintArrayInfo(index, array, array->getNumElements(), "VertexArray");
   if (osg::Vec3Array* array = dynamic_cast<osg::Vec3Array*>(geometry->getNormalArray()))
      PrintArrayInfo(index, array, array->getNumElements(), "NormalArray");
   if (osg::Array* array = geometry->getColorArray())
      PrintArrayInfo(index, array, array->getNumElements(), "ColorArray");
   if (osg::Array* array = geometry->getSecondaryColorArray())
      PrintArrayInfo(index, array, array->getNumElements(), "SecondaryColorArray");

   osg::Array* array;
   int         stop = geometry->getNumTexCoordArrays();
   for (int i = 0; i < stop; i++)
   {
      array = geometry->getTexCoordArray(i);
      if (array)
      {
         PrintArrayInfo(index, array, array->getNumElements(), "TexCoordArray");
      }
      else
      {
         auto out = ut::log::error() << "Geometry has NULL TexCoordArray.";
         out.AddNote() << "Index: " << i << " / " << stop;
      }
   }

   osg::PrimitiveSet* pset;
   stop = geometry->getNumPrimitiveSets();
   // don't even try to dump huge models
   if (stop < 10)
   {
      for (int i = 0; i < stop; i++)
      {
         pset = geometry->getPrimitiveSet(i);
         if (pset)
         {
            std::string typeinfo(pset->className());
            typeinfo += "\\n";
            switch (pset->getMode())
            {
            case GL_POINTS:
               typeinfo += "GL_POINTS";
               break;
            case GL_LINES:
               typeinfo += "GL_LINES";
               break;
            case GL_LINE_STRIP:
               typeinfo += "GL_LINE_STRIP";
               break;
            case GL_LINE_LOOP:
               typeinfo += "GL_LINE_LOOP";
               break;
            case GL_TRIANGLES:
               typeinfo += "GL_TRIANGLES";
               break;
            case GL_TRIANGLE_STRIP:
               typeinfo += "GL_TRIANGLE_STRIP";
               break;
            case GL_TRIANGLE_FAN:
               typeinfo += "GL_TRIANGLE_FAN";
               break;
            case GL_QUADS:
               typeinfo += "GL_QUADS";
               break;
            case GL_QUAD_STRIP:
               typeinfo += "GL_QUAD_STRIP";
               break;
            case GL_POLYGON:
               typeinfo += "GL_POLYGON";
            }
            PrintArrayInfo(index, pset, pset->getNumIndices(), typeinfo);
         }
         else
         {
            auto out = ut::log::error() << "Geometry has NULL PrimitiveSet.";
            out.AddNote() << "Index: " << i << " / " << stop;
         }
      }
   }
}
