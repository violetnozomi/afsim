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

#include "UtGraph.hpp"

#include <cassert>
#include <unordered_map>

void UtGraph::Copy(const UtGraph& aRhs)
{
   Clear();
   mVertices = aRhs.mVertices;
   std::unordered_map<Edge*, Edge*> oldToNewEdges;
   for (size_t i = 0; i < mVertices.size(); ++i)
   {
      Vertex* v = mVertices[i] = mVertices[i]->Clone();
      for (size_t j = 0; j < v->mEdges.size(); ++j)
      {
         Edge*& e = v->mEdges[i];
         if (oldToNewEdges.find(e) == oldToNewEdges.end())
         {
            e = oldToNewEdges[e] = e->Clone();
         }
         else
         {
            e = oldToNewEdges[e];
         }
      }
   }
}

void UtGraph::AddEdge(Edge* aEdgePtr, Vertex* aFrom, Vertex* aTo)
{
   assert(aFrom && aTo);
   aEdgePtr->mFrom = aFrom;
   aEdgePtr->mTo   = aTo;
   aFrom->mEdges.push_back(aEdgePtr);
   if (aFrom != aTo)
   {
      aTo->mEdges.push_back(aEdgePtr);
   }
}

void UtGraph::AddVertex(Vertex* aVertexPtr)
{
   mVertices.push_back(aVertexPtr);
}

void UtGraph::RemoveVertex(Vertex* aVertexPtr)
{
   FindAndRemove(mVertices, aVertexPtr);
   RemoveAllEdges(aVertexPtr);
   delete aVertexPtr;
}

void UtGraph::RemoveEdge(Edge* aEdge)
{
   FindAndRemove(aEdge->mFrom->mEdges, aEdge);
   if (aEdge->mFrom != aEdge->mTo)
   {
      FindAndRemove(aEdge->mTo->mEdges, aEdge);
   }
}

void UtGraph::RemoveAllEdges(Vertex* v)
{
   for (size_t i = 0; i < v->mEdges.size(); ++i)
   {
      Edge*   e         = v->mEdges[i];
      Vertex* otherVert = e->Other(v);
      if (otherVert != v)
      {
         FindAndRemove(otherVert->mEdges, e);
      }
      delete e;
   }
   v->mEdges.clear();
}

UtGraph::Edge* UtGraph::FindEdge(Vertex* a, Vertex* b, bool aIsDirectional) const
{
   if (aIsDirectional)
   {
      for (Edge* e : a->mEdges)
      {
         if (e->mTo == b)
         {
            return e;
         }
      }
   }
   else
   {
      for (Edge* e : a->mEdges)
      {
         if (e->mTo == b || e->mFrom == b)
         {
            return e;
         }
      }
   }
   return nullptr;
}

std::vector<UtGraph::Vertex*> UtGraph::FindNeighbors(Vertex* a)
{
   std::vector<Vertex*> neighbors;
   for (size_t i = 0; i < a->mEdges.size(); ++i)
   {
      neighbors.push_back(a->mEdges[i]->Other(a));
   }
   return neighbors;
}

void UtGraph::Clear()
{
   for (auto& vert : mVertices)
   {
      delete vert;
   }
   mVertices.clear();
}

UtGraph& UtGraph::operator=(const UtGraph& aRhs)
{
   if (this != &aRhs)
   {
      Copy(aRhs);
   }
   return *this;
}
