// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTSPATIALTREE_HPP
#define UTSPATIALTREE_HPP

#include <iostream>
#include <map>
#include <string>

#include "UtSpatialDatum.hpp"
#include "UtSpatialNode.hpp"

//#define DEBUG_UTSPATIALTREE

/**
   A representation of a generalized spatial tree, parameterized by
   dimension.  A quad-tree is a spatial tree of dimension 2;
   an octree is a spatial tree of dimension 3.  Typically a quad-tree
   is used for storing 2-d map-like information.
   Octrees are used to store 3-d information, such as a polygonized
   database for use with collision detection.  They are generally useful
   for applications for which data are searched spatially. In this
   implementation, a QuadTree is a UtSpatialTree<Dim = 2>, and an
   Octree is a UtSpatialTree<Dim = 3>.  Both the quad-tree and cctree
   have typedefs as UtQuadTree and UtOctree.

   Each tree consists of a root node (see UtSpatialNode) with spatal extent defined by a
   bounding box (see UtBoundingBox). This root node is divided equally
   into 2^Dimension children, so that, for instance, a quadtree's root
   node is bisected in each dimension, and the bounding rectangle is
   divided into four equal, smaller rectangles for the child nodes.
   This hierarchical arrangement extends down to the lowest user-specified depth
   in the tree.

   Data are only contained in the leaf nodes.  New leaf nodes are created if
   more than 'MaxDatumsPerNode' reside in a node, and the depth of a node does
   not exceed the specified depth limit.
*/
template<size_t Dim>
class UtSpatialTree
{
public:
   static const int            cORDER = 1 << Dim;
   typedef UtSpatialNode<Dim>  NodeType;
   typedef UtBoundingBoxI<Dim> BoundingBox;
   struct DatumData
   {
      DatumData()
         : mDatumPtr()
         , mNodePtr()
      {
      }
      UtSpatialDatum* mDatumPtr;
      NodeType*       mNodePtr;
   };
   typedef typename std::map<int, DatumData> DatumMap;

   UtSpatialTree(const BoundingBox& aBoundingBox, int aDepth, int aMaxDatumsPerNode = 5)
      : mDepth(aDepth)
      , mMaxDatumsPerNode(aMaxDatumsPerNode)
      , mRootPtr(nullptr)
      , mDatumMap()
   {
      mRootPtr = new NodeType(aBoundingBox);
   }

   ~UtSpatialTree()
   {
      delete mRootPtr;

      typename DatumMap::iterator iter = mDatumMap.begin();
      while (iter != mDatumMap.end())
      {
         UtSpatialDatum* datumPtr = iter->second.mDatumPtr;
         ++iter;
         delete datumPtr;
      }
   }

   //! Subdivide the node to a given depth.
   //! This method is made public primarily for testing purposes.
   void Subdivide(int aDepth)
   {
      SubdivideNode(aDepth, mRootPtr);
      mDepth = aDepth;
   }

   //! Completely clear all data from the tree.  Calling this method
   //! results in destruction of the old tree and creation of a new one.
   void Clear()
   {
      BoundingBox rootBBox = (BoundingBox&)mRootPtr->GetBoundingBox();
      delete mRootPtr;
      mRootPtr = new NodeType(rootBBox);
   }

   //! Remove a datum from the tree.  The datum itself is also deleted.
   //! This only works for point-datums, inserted with Insert().
   bool Remove(int aDatumId)
   {
      bool                        success = false;
      typename DatumMap::iterator iter    = mDatumMap.find(aDatumId);
      if (iter != mDatumMap.end() && iter->second.mNodePtr != nullptr)
      {
         UtSpatialDatum* datumPtr = iter->second.mDatumPtr;
         NodeType*       nodePtr  = iter->second.mNodePtr;
         success                  = nodePtr->Remove(aDatumId);
         if (success)
         {
            delete datumPtr;
            mDatumMap.erase(iter);
         }
      }
      return success;
   }

   //! Given a location, find the leaf node that contains the point
   const NodeType* Search(const double aPos[Dim], const NodeType* aStartNode)
   {
      NodeType* n;

      if (aStartNode == 0) // caller doesn't know
      {
         n = mRootPtr;
      }
      else
      {
         n = const_cast<NodeType*>(aStartNode);
      }

      while ((n != mRootPtr) && (!n->IsContained(aPos)))
      {
         n = n->GetParent();
      }

      NodeType* child = n;

      // descend child nodes until we encounter a leaf,
      // then return that leaf (only leaves contain data).
      while (!child->IsLeaf())
      {
         child = child->FindChild(aPos);
      }

      return child;
   }

   //! Given an input bounding box, populate a list of all nodes intersecting that box
   void Search(const UtBoundingBox&  aBBox,
               std::list<NodeType*>& aNodeList) // out
   {
      aNodeList.clear();
      Search(aBBox, mRootPtr, aNodeList);
   }

   //! Given an input segment defined by two end points, populate a list of all nodes intersecting that segment
   void Search(const double aPos1[Dim], const double aPos2[Dim],
               std::list<NodeType*>& aNodeList) // out
   {
      aNodeList.clear();
      Search(aPos1, aPos2, mRootPtr, aNodeList);
   }

   //! Insert a datum into the tree. Insert() may be called update a datum's position
   //! in the spatial tree.  Insert() should only be used for point-datums.
   // @return The pointer to the node within which the datum was inserted.
   // UtSpatialTree maintains ownership of the supplied datum, and will delete the
   // datum when it is removed or the tree is destroyed.
   const NodeType* Insert(UtSpatialDatum* aDatumPtr)
   {
      typename DatumMap::iterator iter    = mDatumMap.find(aDatumPtr->GetId());
      NodeType*                   nodePtr = nullptr;

      if (iter == mDatumMap.end())
      {
         UtSpatialDatum* datumPtr = aDatumPtr;
         nodePtr                  = Insert(datumPtr, mRootPtr, 0);
      }
      else // already have this datum mapped
      {
         nodePtr = iter->second.mNodePtr;
         assert(nodePtr != nullptr);

         // Replace existing datum, if it is different.
         UtSpatialDatum* datumPtr = iter->second.mDatumPtr;
         if (datumPtr != aDatumPtr)
         {
            delete datumPtr;
            iter->second.mDatumPtr = aDatumPtr;
         }

         int depth = mDepth;

         while (!aDatumPtr->IsWhollyContainedIn(nodePtr->GetBoundingBox()))
         {
            if (depth == mDepth) // original node
            {
               nodePtr->Remove(datumPtr->GetId());
            }
            nodePtr = nodePtr->GetParent();
            --depth;
            assert(depth >= 0);
         }

         if (depth != mDepth) // moved out of node.
         {
            mDatumMap.erase(iter);
            nodePtr = Insert(datumPtr, nodePtr, depth);
         }
         // else datum still associated with this node; do nothing.
      }
      return nodePtr;
   }

   //! Insert a datum with spatial extents into the tree.
   // UtSpatialTree maintains ownership of the supplied datum, and will delete the
   // datum when it is removed or the tree is destroyed.
   bool InsertArea(UtSpatialDatum* aDatumPtr)
   {
      bool                        inserted = false;
      typename DatumMap::iterator iter     = mDatumMap.find(aDatumPtr->GetId());
      if (iter == mDatumMap.end())
      {
         DatumData data;
         data.mDatumPtr                = aDatumPtr;
         data.mNodePtr                 = nullptr;
         mDatumMap[aDatumPtr->GetId()] = data;
         inserted                      = InsertAreaP(data.mDatumPtr, mRootPtr, 0);
      }
      else
      {
         delete aDatumPtr;
      }
      return inserted;
   }

   //! Get the datum corresponding with the given id.
   UtSpatialDatum* GetDatum(unsigned int aId) const
   {
      UtSpatialDatum*                   datumPtr = nullptr;
      typename DatumMap::const_iterator iter     = mDatumMap.find(aId);
      if (iter != mDatumMap.end())
      {
         datumPtr = iter->second;
      }
      return datumPtr;
   }

   //! Get the node corresponding with the given id.
   NodeType* GetNode(unsigned int aId)
   {
      NodeType*                   nodePtr = 0;
      typename DatumMap::iterator iter    = mDatumMap.find(aId);
      if (iter != mDatumMap.end())
      {
         nodePtr = iter->second.mNodePtr;
      }
      return nodePtr;
   }

   //! Output a user-friendly description of the tree.
   void PrintDescription() const
   {
      auto out = ut::log::info() << "Description for tree:";
      out.AddNote() << "Dim: " << Dim;
      out.AddNote() << "Order: " << cORDER;
      int numNodes = PrintNodeDescription(mDepth, mRootPtr, out);
      out.AddNote() << "Total Number of nodes is: " << numNodes;
   }

   //! Return the number of nodes currently in the tree.
   //! This number increases as data are inserted or moved through the tree.
   int GetNumberOfNodes(const NodeType* aNodePtr = 0, int aDepth = 0) const
   {
      const NodeType* nodePtr  = aNodePtr;
      int             numNodes = 0;
      if (aNodePtr == 0)
      {
         nodePtr  = mRootPtr;
         numNodes = 1;
      }
      if (!(nodePtr->IsLeaf()))
      {
         numNodes += cORDER;
         if (aDepth != (mDepth - 1))
         {
            for (int childNum = 0; childNum < cORDER; ++childNum)
            {
               numNodes += GetNumberOfNodes(nodePtr->GetChild(childNum), ++aDepth);
            }
         }
      }
      return numNodes;
   }

   const NodeType* GetRoot() const { return mRootPtr; }

protected:
   // Copy construction and assignment are not supported.
   UtSpatialTree(const UtSpatialTree<Dim>& aSrc);
   UtSpatialTree<Dim> operator=(const UtSpatialTree<Dim>& aRhs) const;

   //! Given a new datum, insert the datum into the tree.
   NodeType* Insert(UtSpatialDatum* aDatumPtr, NodeType* aNodePtr, int aNodeDepth)
   {
      int             depth    = aNodeDepth;
      NodeType*       nodePtr  = aNodePtr;
      UtSpatialDatum* datumPtr = static_cast<UtSpatialDatum*>(aDatumPtr);

      while (depth < mDepth)
      {
         if (nodePtr->IsLeaf())
         {
            if (nodePtr->GetDatumCount() >= mMaxDatumsPerNode)
            {
               SubdivideNode(1, const_cast<NodeType*>(nodePtr));
            }
         }
         if (!nodePtr->IsLeaf())
         {
            unsigned short numChildren = nodePtr->GetNumChildren();
            for (unsigned short childNum = 0; childNum < numChildren; ++childNum)
            {
               NodeType* childNodePtr = nodePtr->GetChild(childNum);
               if (datumPtr->IsWhollyContainedIn(childNodePtr->GetBoundingBox()))
               {
                  nodePtr = childNodePtr;
                  break;
               }
            }
         }
         else
         {
            break;
         }
      }

      nodePtr->Insert(datumPtr);
      DatumData datumData;
      datumData.mNodePtr  = nodePtr;
      datumData.mDatumPtr = datumPtr;
      mDatumMap.insert(std::make_pair(datumPtr->GetId(), datumData));
      return nodePtr;
   }

   //! Given a new datum, insert the datum into the tree.
   bool InsertAreaP(UtSpatialDatum* aDatumPtr, NodeType* aNodePtr, int aNodeDepth)
   {
      bool ok = false;
      if (aDatumPtr->IsPartiallyContainedIn(aNodePtr->GetBoundingBox()))
      {
         if ((aNodeDepth < mDepth) && (aNodePtr->IsLeaf()) && (aNodePtr->GetDatumCount() >= mMaxDatumsPerNode))
         {
            SubdivideNode(1, const_cast<NodeType*>(aNodePtr));
         }
         if (!aNodePtr->IsLeaf())
         {
            unsigned short numChildren = aNodePtr->GetNumChildren();
            for (unsigned short childNum = 0; childNum < numChildren; ++childNum)
            {
               ok = InsertAreaP(aDatumPtr, aNodePtr->GetChild(childNum), aNodeDepth + 1) || ok;
            }
         }
         else
         {
            ok = true;
            aNodePtr->Insert(aDatumPtr);
         }
      }
      return ok;
   }

   const DatumMap& GetDatumMap() const { return mDatumMap; }

private:
   UtSpatialTree();

   void SubdivideNode(int aDepth, NodeType* aNodePtr)
   {
      aNodePtr->Subdivide();

      if (aDepth > 1)
      {
         for (int childNum = 0; childNum < cORDER; ++childNum)
         {
            // Recursive subdivide
            SubdivideNode((aDepth - 1), aNodePtr->GetChild(childNum));
         }
      }
   }

   int PrintNodeDescription(int aDepth, const NodeType* aNodePtr, ut::log::MessageStream& aStream) const
   {
      int numNodes = 1;
      if (aNodePtr->IsRoot())
      {
         aStream.AddNote() << "Root node.";
      }
      aStream.AddNote() << "Depth: " << (mDepth - aDepth);
      aNodePtr->PrintDescription(aStream);

      if (aNodePtr->IsLeaf())
      {
         aStream.AddNote() << "Leaf node.";
      }
      else
      {
         for (int childNum = 0; childNum < cORDER; ++childNum)
         {
            auto      note    = aStream.AddNote() << "Child " << childNum << ":";
            NodeType* nodePtr = aNodePtr->GetChild(childNum);
            numNodes += PrintNodeDescription((aDepth - 1), nodePtr, note);
         }
      }
      return numNodes;
   }

   void Search(const UtBoundingBox& aBBox, NodeType* aNodePtr,
               std::list<NodeType*>& aNodeList) // out
   {
      if (aNodePtr->GetBoundingBox().Intersects(aBBox))
      {
         if (aNodePtr->IsLeaf())
         {
            if ((aNodePtr->GetDatumCount() > 0))
            {
               aNodeList.push_back(aNodePtr);
            }
         }
         else
         {
            unsigned numChildren = aNodePtr->GetNumChildren();
            for (unsigned short childNum = 0; childNum < numChildren; ++childNum)
            {
               Search(aBBox, aNodePtr->GetChild(childNum), aNodeList);
            }
         }
      }
   }

   void Search(const double          aPos1[Dim],
               const double          aPos2[Dim],
               NodeType*             aNodePtr,
               std::list<NodeType*>& aNodeList) // out
   {
      if (aNodePtr->GetBoundingBox().Intersects(aPos1, aPos2))
      {
         if (aNodePtr->IsLeaf())
         {
            if ((aNodePtr->GetDatumCount() > 0))
            {
               aNodeList.push_back(aNodePtr);
            }
         }
         else
         {
            unsigned numChildren = aNodePtr->GetNumChildren();
            for (unsigned short childNum = 0; childNum < numChildren; ++childNum)
            {
               Search(aPos1, aPos2, aNodePtr->GetChild(childNum), aNodeList);
            }
         }
      }
   }

   // Attributes.
   int mDepth;
   //! Maximum number of datums before a node is split -- up to mDepth splits
   int       mMaxDatumsPerNode;
   NodeType* mRootPtr;
   DatumMap  mDatumMap;
};

//! Typedefs for UtQuadTree and UtOctree.
typedef UtSpatialTree<2> UtQuadTree;
typedef UtSpatialTree<3> UtOctree;

#endif
