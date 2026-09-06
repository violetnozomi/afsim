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

#ifndef SPATIAL_NODE_H
#define SPATIAL_NODE_H

#include <cassert>
#include <iostream>
#include <list>

#include "UtBoundingBox.hpp"
#include "UtSpatialDatum.hpp"

/**
   The UtSpatialNode class is a node in the tree.
   Each node acts as a container of UtSpatalDatum objects
   (points, triangles, etc).  There are four (4) nodes per
   level of depth in a QuadTree (UtQuadTree) and eight (8)
   nodes per level of depth in an Octree (UtOctree).
*/
template<size_t Dim>
class UtSpatialNode
{
public:
   static const int cORDER = 1 << Dim;

   UtSpatialNode(int aChildNum, UtSpatialNode* aParentPtr, UtBoundingBoxI<Dim> aBoundingBox)
      : mDatumCount(0)
      , mChildNum(aChildNum)
      , mParentPtr(aParentPtr)
      , mBBox(aBoundingBox)
      , mChildVecPtr(nullptr)
   {
   }

   UtSpatialNode(int aChildNum, UtSpatialNode* aParentPtr)
      : mDatumCount(0)
      , mChildNum(aChildNum)
      , mParentPtr(aParentPtr)
      , mBBox()
      , mChildVecPtr(nullptr)
   {
   }

   UtSpatialNode(UtBoundingBoxI<Dim> aBoundingBox)
      : mDatumCount(0)
      , mChildNum(0)
      , mParentPtr(nullptr)
      , mBBox(aBoundingBox)
      , mChildVecPtr(nullptr)
   {
   }

   ~UtSpatialNode(void)
   {
      if (mChildVecPtr)
      {
         for (int i = 0; i < cORDER; ++i)
         {
            delete mChildVecPtr[i];
         }
         delete[] mChildVecPtr;
      }
   }

   // Accessors
   //! Return the number of children that this node contains.
   unsigned short GetNumChildren() const
   {
      if (mChildVecPtr == nullptr) // i.e., it's a leaf node.
      {
         return 0;
      }
      else
      {
         return cORDER;
      }
   }

   //! Return a reference to the bounding box that defines this node.
   const UtBoundingBox& GetBoundingBox() const { return mBBox; }

   //! Return the child node of the given index.  Valid nodes indices are [0..(order-1].
   UtSpatialNode* GetChild(int aChildNum) const
   {
      assert(aChildNum < cORDER);
      return mChildVecPtr[aChildNum];
   }

   //! Return the number assigned to this child (0 .. (order - 1))
   int GetChildNum() const { return mChildNum; }

   //! Return this object's parent node.  If the current node is root, 0 is returned.
   UtSpatialNode* GetParent() const { return mParentPtr; }

   //! Return whether the given point is contained within the node's bounding box.
   bool IsContained(const double aPoint[Dim]) const { return mBBox.IsContained(aPoint); }

   bool IsRoot() const { return (mParentPtr == 0); }
   bool IsLeaf() const { return (mChildVecPtr == nullptr); }

   //! Return the data contained within the node.
   const SpatialData& GetData() const { return mData; }

   int GetDatumCount() const { return mDatumCount; }

   //! Return the data contained in this node and in any child nodes.  May have duplicates
   //! if datums are shared in multiple children.
   void ChildrenData(SpatialData& aDataList) const
   {
      if (IsLeaf())
      {
         aDataList.insert(aDataList.end(), mData.begin(), mData.end());
      }
      else
      {
         for (size_t i = 0; i < Dim; ++i)
         {
            mChildVecPtr[i]->ChildrenData(aDataList);
         }
      }
   }

   //! Find the child whose bounding UtSpatialNode contains the input vector
   //! If the point is outside the bounding box of the SpatialData, or
   //! if we are at the bottom of the tree, the pointer is 0 (NULL).
   UtSpatialNode* FindChild(const double aPos[Dim])
   {
      unsigned short childIndex = 0;
      UtSpatialNode* childPtr   = 0;

      if (!IsLeaf())
      {
         if (mBBox.IsContained(aPos))
         {
            int delta = 1;

            for (size_t dim = 0; dim < Dim; ++dim)
            {
               if (aPos[dim] > mBBox.GetCentroid()[dim])
               {
                  childIndex += delta;
               }
               delta *= 2;
            }
         }

         childPtr = mChildVecPtr[childIndex];
      }

      return childPtr;
   }

   void SetBoundingBox(const UtBoundingBoxI<Dim>& aBBox) { mBBox = aBBox; }

   bool Insert(const UtSpatialDatum* aDatumPtr) { return AddDatum(aDatumPtr); }

   bool Remove(int aDatumId) { return RemoveDatum(aDatumId); }

   //! Subdivide the given node into (order) subnodes.  The bounding box is
   //! similarly subdivided.
   void Subdivide()
   {
      assert(mChildVecPtr == nullptr);
      // Subdivide along all Dim axes of a bounding box (2 or 3).
      mChildVecPtr = new UtSpatialNode<Dim>*[cORDER];
      int childNum = 0;
      // using placement new to construct each node after the array of children is allocated
      mChildVecPtr[0] = new UtSpatialNode<Dim>(childNum, this, mBBox);
      std::swap(mChildVecPtr[0]->mData, mData);
      std::swap(mChildVecPtr[0]->mDatumCount, mDatumCount);

      for (size_t i = 0; i < Dim; ++i)
      {
         unsigned char size = childNum + 1;
         for (unsigned char j = 0; j < size; ++j)
         {
            UtSpatialNode<Dim>* leftNodePtr = mChildVecPtr[j];
            ++childNum;
            UtSpatialNode<Dim>* rightNodePtr = mChildVecPtr[childNum] = new UtSpatialNode<Dim>(childNum, this);
            leftNodePtr->mBBox.SubdivideAlongAxis(static_cast<UtBoundingBox::Axis>(i),
                                                  leftNodePtr->mBBox,
                                                  rightNodePtr->mBBox);
            leftNodePtr->SubdivideData(rightNodePtr); // adds the data to the vector.
         }
      }
   }

   void PrintDescription(ut::log::MessageStream& aStream) const
   {
      auto note = aStream.AddNote() << "Node #" << mChildNum << ':';
      mBBox.PrintExtent(note);
      aStream.AddNote() << "Number of data points: " << mData.size();
   }

protected:
private:
   UtSpatialNode(); // Not Allowed

   //! Add the given UtSpatialDatum object to the list of data
   bool AddDatum(const UtSpatialDatum* aDatumPtr)
   {
      bool canInsert = aDatumPtr->IsPartiallyContainedIn(mBBox);
      if (canInsert)
      {
         mData.push_back(aDatumPtr);
         ++mDatumCount;
      }
      return canInsert;
   }

   //! Remove a datum with the specified id.
   bool RemoveDatum(int aDatumId)
   {
      std::list<const UtSpatialDatum*>::iterator iter  = mData.begin();
      bool                                       found = false;
      while (iter != mData.end())
      {
         if ((*iter)->GetId() == (unsigned int)aDatumId)
         {
            found = true;
            mData.erase(iter);
            --mDatumCount;
            break;
         }
         ++iter;
      }
      return found;
   }

   //! This function Subdivides the data along the number of dimensions of the tree.
   //! It is called multiple times to help create an additional level
   //! of depth in the tree.
   void SubdivideData(UtSpatialNode<Dim>* aNodeRightPtr)
   {
      assert(aNodeRightPtr->mDatumCount == 0);
      // iterate through list of data
      if (!mData.empty())
      {
         SpatialData::iterator iter = mData.begin();

         while (iter != mData.end())
         {
            const UtSpatialDatum& dataRef = **iter;
            if (dataRef.IsPoint())
            {
               // Points can only be in one node
               if (dataRef.IsWhollyContainedIn(aNodeRightPtr->GetBoundingBox()))
               {
                  aNodeRightPtr->mData.push_back(&dataRef);
                  ++aNodeRightPtr->mDatumCount;
                  mData.erase(iter++);
                  --mDatumCount;
               }
               else
               {
                  ++iter;
               }
            }
            else
            {
               if (dataRef.IsPartiallyContainedIn(aNodeRightPtr->GetBoundingBox()))
               {
                  aNodeRightPtr->mData.push_back(&dataRef);
                  ++aNodeRightPtr->mDatumCount;
               }
               if (!dataRef.IsPartiallyContainedIn(GetBoundingBox()))
               {
                  mData.erase(iter++);
                  --mDatumCount;
               }
               else
               {
                  ++iter;
               }
            }
         }
      }
   }

   //! Number of datums in this node.  Equals mData.size().  (std::list::size() is O(N) on some implementations)
   int                 mDatumCount;
   int                 mChildNum;
   UtSpatialNode*      mParentPtr; // a back-pointer to the parent
   SpatialData         mData;
   UtBoundingBoxI<Dim> mBBox;
   UtSpatialNode**     mChildVecPtr;
};

typedef UtSpatialNode<2> UtQuadNode;
typedef UtSpatialNode<3> UtOctNode;

#endif
