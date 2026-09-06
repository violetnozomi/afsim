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
#ifndef UTOTREE_DUMP_HPP
#define UTOTREE_DUMP_HPP

// $Id: UtoTreeDump.hpp,v 1.3 2012/05/15 17:40:05 jj672c Exp $

#include <ostream>
#include <unordered_map>
#include <unordered_set>

namespace osg
{
class Node;
class StateSet;
class Drawable;
class Group;
class Geode;
class Geometry;
} // namespace osg

/* This prints just enough information to know what nodes are where in
 * the tree.  It does not dump the details about nodes, just that they
 * exist.  The output is in a format that can be read into a graph
 * by the graphviz program.
 * http://www.research.att.com/sw/tools/graphviz/
 */

class UtoTreeDump
{
public:
   enum SkipList
   {
      DISPLAY_ALL         = 0,
      SKIP_NODE           = 1,
      SKIP_GROUP          = 2,
      SKIP_GEODE          = 4,
      SKIP_DRAWABLE       = 8,
      SKIP_GEOMETRY       = 16,
      SKIP_GEOMETRY_ARRAY = 32,
      SKIP_STATESET       = 64
   };
   enum ShapeColorType
   {
      TYPE_DEFAULT,
      TYPE_GEODE,
      TYPE_SWITCH,
      TYPE_SEQUENCE,
      TYPE_TRANSFORM,
      TYPE_GROUP,
      TYPE_DRAWABLE,
      TYPE_ARRAY,
      TYPE_STATESET,
   };


   UtoTreeDump();
   ~UtoTreeDump();

   int         Dump(std::ostream& out_arg, osg::Node* tree, SkipList Ignore = SKIP_GEOMETRY_ARRAY);
   void        NodeAttributeClear();
   void        NodeAttributeSet(ShapeColorType entry, const std::string& attributes);
   std::string NodeAttributeGet(ShapeColorType entry);

private:
   class hash_compare
   { // traits class for hash containers
   public:
      enum
      {                   // parameters for hash table
         bucket_size = 4, // 0 < bucket_size
         min_buckets = 8  // min_buckets = 2 ^^ N, 0 < N
      };

      hash_compare() {}

      size_t operator()(const ShapeColorType _Keyval) const
      { // hash _Keyval to size_t value
         return (size_t)(_Keyval);
      }
      size_t operator()(const void* _Keyval) const
      { // hash _Keyval to size_t value
         return (size_t)(_Keyval);
      }
      bool operator()(const void* _Keyval1, const void* _Keyval2) const
      { // test if _Keyval1 ordered before _Keyval2
         return _Keyval1 < _Keyval2;
      }
   };

   using NodeAttributeMap = std::unordered_map<ShapeColorType, std::string, hash_compare>;
   using NodeMap          = std::unordered_map<void*, int, hash_compare>;
   using NodeSet          = std::unordered_set<void*, hash_compare>;

   UtoTreeDump(const UtoTreeDump&);
   UtoTreeDump& operator=(const UtoTreeDump&);

   int  AddressIndex(void* address);
   void PrintInfo(int index, void* address, const std::string& TypeInfo);
   void PrintNodeInfo(osg::Node* node);
   void PrintStateSetInfo(osg::StateSet* stateset, int parentId, const std::string& extra = "");
   void PrintDrawableInfo(osg::Drawable* drawable, const std::string& extra = "");
   void PrintArrayInfo(int parent, void* address, int size, const std::string& TypeInfo);
   void PrintEdge(int from, int to, const std::string& Note = "");
   void ProcessNode(osg::Node* node);
   void ProcessGroup(osg::Group* group);
   void ProcessStateSet(osg::StateSet* stateset, int parentId);
   void ProcessGeode(osg::Geode* geode);
   void ProcessDrawable(osg::Drawable* drawable);
   void ProcessGeometry(osg::Geometry* geometry);

   NodeAttributeMap NodeAttribute_; // No public attributes
   // maps an address (cast to an int), to a node number indexed from 0
   NodeSet       NodeDisplayed_;
   NodeMap       NodeNumber_;
   int           NextNode_;
   std::ostream* out_;
   SkipList      ignore_;
};

#endif // _UTOTREE_DUMP_HPP
