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

#ifndef UTGRAPH_HPP
#define UTGRAPH_HPP

#include "ut_export.h"

#include <algorithm>
#include <cassert>
#include <climits>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>

#include "UtMath.hpp"
#include "UtMemory.hpp"


#ifdef DEBUG_ON
#define DIAG_ON
#include "UtLog.hpp"
#define DO_DEBUG(A) A
#else
#define DO_DEBUG(A) /* A */
#endif

#ifndef SCOPE
#if defined(_WIN32) && (_MSC_VER < 1300)
#define SCOPE(X)
#else
#define SCOPE(X) X
#endif
#endif

//! A simple graph implementation that allows for inherited vertex and edge classes
//! vertices' and edges' memory is managed by the graph.
class UT_EXPORT UtGraph
{
   template<typename T>
   static void FindAndRemove(std::vector<T>& aVector, T aValue)
   {
      typename std::vector<T>::iterator i = std::find(aVector.begin(), aVector.end(), aValue);
      if (i != aVector.end())
      {
         aVector.erase(i);
      }
   }

public:
   class Vertex;
   class Edge;
   using VertexList = std::vector<Vertex*>;
   using EdgeList   = std::vector<Edge*>;
   using UniqueId   = unsigned long long;

   class UT_EXPORT Vertex
   {
   public:
      friend class UtGraph;
      virtual ~Vertex() = default;
      virtual Vertex*           Clone() const { return new Vertex(*this); }
      const std::vector<Edge*>& Edges() const { return mEdges; }

   protected:
      std::vector<Edge*> mEdges;
   };

   class UT_EXPORT Edge
   {
   public:
      friend class UtGraph;
      virtual ~Edge() = default;
      virtual Edge* Clone() const { return new Edge(*this); }
      Vertex*       From() const { return mFrom; }
      Vertex*       To() const { return mTo; }
      Vertex*       Other(Vertex* aVert) const { return mFrom == aVert ? mTo : mFrom; }

   protected:
      Vertex* mFrom;
      Vertex* mTo;
   };

private:
   void Copy(const UtGraph& aRhs);

public:
   void                 AddEdge(Edge* aEdgePtr, Vertex* aFrom, Vertex* aTo);
   void                 AddVertex(Vertex* aVertexPtr);
   void                 RemoveVertex(Vertex* aVertexPtr);
   void                 RemoveEdge(Edge* aEdge);
   void                 RemoveAllEdges(Vertex* v);
   Edge*                FindEdge(Vertex* a, Vertex* b, bool aIsDirectional) const;
   std::vector<Vertex*> FindNeighbors(Vertex* a);
   void                 Clear();
   UtGraph&             operator=(const UtGraph& aRhs);
   const VertexList&    GetVertices() const { return mVertices; }
   EdgeList             CollectEdges() const
   {
      EdgeList allEdges;
      for (Vertex* v : mVertices)
      {
         for (size_t j = 0; j < v->mEdges.size(); ++j)
         {
            Edge* e = v->mEdges[j];
            if (e->mFrom == v)
            {
               allEdges.push_back(e);
            }
         }
      }
      return allEdges;
   }

   struct DefaultCostFunction
   {
      int EdgeCost(UtGraph::Edge* /*aEdge*/) const { return 1; }
      int Heuristic(UtGraph::Vertex* aSrc, UtGraph::Vertex* aDest) const
      {
         if (aSrc != aDest)
         {
            return 1;
         }
         return 0;
      }
      bool ConsiderVertex(Vertex* /*aNode*/) const { return true; }
   };

protected:
   VertexList mVertices;
};

namespace UtGraphAlgorithm
{
template<typename COST_TYPE, typename COST_FUNC>
bool ShortestPath(const UtGraph&       aGraph,
                  UtGraph::Vertex*     aSourcePtr,
                  UtGraph::Vertex*     aDestPtr,
                  UtGraph::VertexList& aVertexList,
                  COST_TYPE&           aCost,
                  const COST_FUNC&     aCostFunc,
                  bool                 aIsDirectional = false)
{
   using Vertex         = UtGraph::Vertex;
   using Edge           = UtGraph::Edge;
   using TotalAndActual = std::pair<COST_TYPE, COST_TYPE>;
   using CostAndVertex  = std::pair<TotalAndActual, UtGraph::Vertex*>;
   using CostMap        = std::unordered_map<Vertex*, COST_TYPE>;
   using PredecessorMap = std::unordered_map<Vertex*, Vertex*>;

   std::priority_queue<CostAndVertex> queue;
   CostMap                            bestCosts;

   PredecessorMap predecessors;
   bestCosts[aSourcePtr]    = COST_TYPE();
   predecessors[aSourcePtr] = nullptr;
   CostAndVertex current    = CostAndVertex(TotalAndActual(), aSourcePtr);

   bool      hasA_Solution = false;
   COST_TYPE solutionCost(0);
   while (true)
   {
      COST_TYPE currentTotal = current.first.first;
      if (hasA_Solution && solutionCost <= currentTotal)
      {
         break;
      }
      COST_TYPE currentActual = current.first.second;
      if (currentActual == bestCosts[current.second])
      {
         Vertex*                   cv    = current.second;
         const std::vector<Edge*>& edges = cv->Edges();
         for (auto edge : edges)
         {
            if (aIsDirectional && edge->From() != cv)
            {
               continue;
            }
            Vertex* next = edge->Other(cv);
            if (!aCostFunc.ConsiderVertex(next))
            {
               continue;
            }
            COST_TYPE nextActualCost = aCostFunc.EdgeCost(edge) + currentActual;

            // COST_TYPE heuristic = aCostFunc.Heuristic(next, aDestPtr);
            // COST_TYPE totalCost = edgeCost + heuristic;
            typename CostMap::iterator iter     = bestCosts.find(next);
            bool                       newEntry = false;
            if (iter != bestCosts.end())
            {
               if (iter->second > nextActualCost)
               {
                  newEntry           = true;
                  iter->second       = nextActualCost;
                  predecessors[next] = cv;
               }
            }
            else
            {
               newEntry           = true;
               bestCosts[next]    = nextActualCost;
               predecessors[next] = cv;
            }
            if (newEntry)
            {
               if (next != aDestPtr)
               {
                  COST_TYPE totalCost = nextActualCost + aCostFunc.Heuristic(next, aDestPtr);
                  if (!hasA_Solution || totalCost < solutionCost)
                  {
                     queue.push(CostAndVertex(TotalAndActual(totalCost, nextActualCost), next));
                  }
               }
               else
               {
                  solutionCost  = nextActualCost;
                  hasA_Solution = true;
               }
            }
         }
      }
      if (queue.empty())
      {
         break;
      }
      CostAndVertex top = queue.top();
      current           = top;
      queue.pop();
   }
   if (hasA_Solution)
   {
      aVertexList.clear();
      Vertex* cv = aDestPtr;
      aCost      = solutionCost;
      while (cv)
      {
         aVertexList.push_back(cv);
         Vertex* next = predecessors[cv];
         cv           = next;
      }
      std::reverse(aVertexList.begin(), aVertexList.end());
   }
   return hasA_Solution;
}
} // namespace UtGraphAlgorithm

//! @brief Represents a general graph and allows manipulation of nodes and edges.
//!
//! The graph may be directed or undirected. The nodes labels may not be unique,
//! e.g. you may have more than one Node with the same label. There may be more
//! than one edge between the same two nodes. There may also be edges from one
//! Node to itself.
//!
//! @param NODE A type that will be used to label nodes.
//! @param EDGE A type that will be used to label edges.
//! @param DIRECTED Boolean parameter, if set to false the graph is undirected, else
//!                 it is directed. By default, DIRECTED = true
//! @param AUX A type that will be attached to each Node and can be modified.
template<typename NODE, typename EDGE, bool DIRECTED = true, typename AUX = int>
class UtGraphT
{
public:
   // Predeclare all public nested classes.

   class edge_iterator;
   class const_edge_iterator;
   class edge_iterator_base;
   class cost_func;
   class Node;
   class Edge;

   //! Binary predicate for unique map sorting of Node pointers. (see: EdgeMap)
   struct NodeCompare
   {
      bool operator()(const Node* aLhs, const Node* aRhs) const { return aLhs->get_index() < aRhs->get_index(); }
   };

   typedef UtGraphT<NODE, EDGE, DIRECTED, AUX> UtGraphType;
   using NodeMap             = typename std::multimap<NODE, Node*>;
   using NodeMapIterator     = typename NodeMap::iterator;
   using NodeMapValueType    = typename NodeMap::value_type;
   using EdgeMap             = typename std::multimap<Node*, Edge*, NodeCompare>;
   using EdgeMapIterator     = typename EdgeMap::iterator;
   using EdgeMapValueType    = typename EdgeMap::value_type;
   using NodeList            = typename std::vector<NODE>;
   using node_iterator       = typename NodeMap::iterator;
   using const_node_iterator = typename NodeMap::const_iterator;

   using node_index_t                          = unsigned int;
   static constexpr node_index_t cInvalidIndex = std::numeric_limits<node_index_t>::max();

public:
   UtGraphT();

   virtual ~UtGraphT();

   UtGraphT(const UtGraphType& aGraph);

   UtGraphT& operator=(const UtGraphType& aGraph);

   node_iterator       begin();
   const_node_iterator begin() const;

   node_iterator       end();
   const_node_iterator end() const;

   node_iterator insert(const NODE& aLabel);

   void erase(node_iterator aNodeIter);

   void erase(const NODE& aNode);

   edge_iterator insert_edge(node_iterator aNode1Iter, node_iterator aNode2Iter, const EDGE& aEdge);

   void erase_all_edges();

   void erase_all_edges(node_iterator aNodeIter);

   void erase_edge(node_iterator aNode1Iter, node_iterator aNode2Iter);

   node_iterator find(const NODE& aLabel);

   const_node_iterator find(const NODE& aLabel) const;

   int count_nodes() const;

   void clear();

   bool is_directed() const;

   virtual bool shortest_path(const_node_iterator aSrcNodeIter,
                              const_node_iterator aDstNodeIter,
                              NodeList&           aPath,
                              double&             aCost,
                              const cost_func*    aCostFuncPtr = nullptr) const;

   node_index_t mNextNodeIndex;

private:
   void copy(const UtGraphType& aGraph);

   //! the nodes collection
   //! why use mutable? here's why:
   //! - if the UtGraphT is const, then its members are const and
   //! mNodes.begin() and mNodes.end() return const_iterators.
   //! This conflicts with our design of having a node_iterator_base
   //! we prevent it from being qualified with const when the container is const.
   //! It will always be modifiable from any method of UtGraphT (be the method
   //! const or not), but that doesn't bother us because we are careful when
   //! to actually modify the multimap.
   mutable NodeMap mNodes;

public:
   // Define all the public nested classes.

   //! @brief The edge iterator base implementation.
   //!
   //! The edge iterator is used to browse and modify the edges of a Node.
   //! This class contains the common operations of its descendents:
   //! %edge_iterator and %const_edge_iterator.

   class edge_iterator_base
   {
   public:
      friend class UtGraphT<NODE, EDGE, DIRECTED, AUX>;
      friend class Node;
      friend class Edge;
      friend class edge_iterator;
      friend class const_edge_iterator;

      //! Protected constructor used to initialize th eiterator from a multimap iterator.

      edge_iterator_base(EdgeMapIterator aIter)
         : mIter(aIter)
      {
      }

      //! @brief Default constructor.
      //!
      //! Useful for declaring edge_iterators without initialization.
      edge_iterator_base() = default;

      //! @brief Indicates whether the edge is valid (i.e. traversable).
      //! @return Returns true if the edge is valid.

      bool valid() const { return this->mIter->second->mValid; }

      //! @brief Indicates whether the edge is valid (i.e. traversable).
      //! @return Returns true if the edge is valid.

      void valid(bool aValid) { this->mIter->second->mValid = aValid; }

      //! @brief Compares an edge_iterator with another one.
      //!
      //! @param aEdgeIter The edge_iterator to compare this to.
      //!
      //! @return True if the two iterators point to the same position in the edges collection.

      bool operator==(const edge_iterator_base& aEdgeIter) const { return (mIter == aEdgeIter.mIter); }

      //! @brief Compares an edge_iterator with another one
      //!
      //! @param aEdgeIter The edge_iterator to compare this to.
      //!
      //! @return True if the two iterators are different, i.e. they do not point to the same
      //! position in the edges collection.

      bool operator!=(const edge_iterator_base& aEdgeIter) const { return (mIter != aEdgeIter.mIter); }

      //! @brief Gets the label of the start Node of the edge.
      //!
      //! In the case of undirected graphs, it gets the label of one of
      //! the two nodes connected by the edge. To get the other Node's label,
      //! or the end Node label in the case of directed graphs, use operator +.
      //!
      //! @return The label of the start Node.

      const NODE& operator-() const { return this->mIter->second->mN1Ptr->mNodeIter.mIter->first; }

      //! @brief Gets the label of the end Node of the edge.
      //!
      //! In the case of undirected graphs, it gets the label of one of
      //! the two nodes connected by the edge. To get the other Node's label,
      //! or the start Node label in the case of directed graphs, use operator -.
      //!
      //! @return The label of the end Node.

      const NODE& operator+() const { return this->mIter->second->mN2Ptr->mNodeIter.mIter->first; }

      //! @brief INTERNAL. Copies multimap iterator from argument.

      //! @param aEdgeIter The edge_iterator from which to copy the multimap iterator.

   protected:
      void copy(const edge_iterator_base& aEdgeIter) { this->mIter = aEdgeIter.mIter; }

      // The multimap iterator. Always a iterator (non-const).
      EdgeMapIterator mIter;
   };

   //! @brief The edge iterator implementation.
   //!
   //! The edge iterator may be used to perform insertions and deletions of edges
   //! and to change the labels of the edges. It is used as a return value when
   //! searching for edges.
   class edge_iterator : public edge_iterator_base
   {
   public:
      friend class UtGraphT<NODE, EDGE, DIRECTED, AUX>;
      friend class Node;
      friend class Edge;

      //! @brief Default constructor.

      edge_iterator()
         : edge_iterator_base()
      {
      }

      //! @brief Copy constructor.
      //!
      //! Useful for statements like: edge_iterator edgeIter = ...

      edge_iterator(const edge_iterator& aEdgeIter)
         : edge_iterator_base(aEdgeIter.mIter)
      {
      }

      //! @brief Assignment.

      edge_iterator& operator=(const edge_iterator& aEdgeIter)
      {
         if (this == &aEdgeIter)
         {
            return *this;
         }
         this->mIter = aEdgeIter.mIter;
         return *this;
      }

      //! @brief Gets the label of the edge.
      //!
      //! Using this operator one can modify the label of
      //! the edge by a simple assignment operation, e.g.
      //! *edgeIter = "value";
      //!
      //! @return A reference to the label of the edge.

      EDGE& operator*() { return this->mIter->second->mEdge; }

      //! @brief Gets a pointer to the edge label.
      //!
      //! If the label type is a complex type, this operator is
      //! useful to perform operations directly on the edge type
      //! by using a simple syntax, e.g.: ei->c_str(), where ei is
      //! the edge iterator and the type is std::string
      //!
      //! @return A pointer to the label of the edge.:

      EDGE* operator->() { return &this->mIter->second->mEdge; }

      Node* fromNode() const { return this->mIter->second->mN1Ptr; }
      Node* toNode() const { return this->mIter->second->mN2Ptr; }

      //! @brief Gets the start Node of the edge.
      //!
      //! For undirected graphs, it just gets one of the two endpoints.
      //! Use to() to get the other endpoint. This method is synonymous to
      //! using the [] operator, with index = 0, e.g.: ei.from() == ei[0]
      //!
      //! @return A %node_iterator to the start Node.

      typename SCOPE(UtGraphType::) node_iterator from() { return this->mIter->second->mN1Ptr->mNodeIter; }

      //! @brief Gets the end Node of the edge.
      //!
      //! For undirected graphs, it just gets one of the two endpoints.
      //! Use from() to get the other endpoint. This method is synonymous to
      //! using the [] operator, with index = 1, e.g.: ei.to() == ei[1]
      //!
      //! @return An iterator to the end Node.

      typename SCOPE(UtGraphType::) node_iterator to() { return this->mIter->second->mN2Ptr->mNodeIter; }

      typename SCOPE(UtGraphType::) node_iterator opposite(node_iterator aNodeIter)
      {
         return (to() == aNodeIter) ? from() : to();
      }

      //! @brief Gets the endpoints of the edge
      //!
      //! @param aIndex This may have two values: 0 - gets the start of the edge,
      //! 1 - gets the end of the edge. In the case of undirected graphs, the meaning
      //! of 'start'/'end' of the edge is lost, and this method may be used to return
      //! any of the two endpoints.
      //!
      //! @return An iterator to the required Node. To obtain the Node value, just
      //! add * in front of the result, e.g.: *ei[0] gives the label of the start Node.

      typename SCOPE(UtGraphType::) node_iterator operator[](int aIndex)
      {
         switch (aIndex)
         {
         case 0:
            return this->mIter->second->mN1Ptr->mNodeIter;
         case 1:
            return this->mIter->second->mN2Ptr->mNodeIter;
         default:
            throw "An edge has only two nodes";
         }
      }

      //! @brief Advance to the next edge in the collection.
      //!
      //! @return An %edge_iterator previous to the increment operation.

      edge_iterator operator++(int)
      {
         edge_iterator tmp = *this;
         this->mIter++;
         return tmp;
      }

      //! @brief Advance to the next edge in the collection.
      //!
      //! @return An %edge_iterator after the increment operation.

      edge_iterator& operator++()
      {
         ++this->mIter;
         return *this;
      }

      //! @brief Go back to the previous edge in the collection.
      //!
      //! @return An %edge_iterator previous to the decrement operation.

      edge_iterator operator--(int)
      {
         edge_iterator tmp = *this;
         --this->mIter;
         return tmp;
      }

      //! @brief Go back to the previous edge in the collection.
      //!
      //! @return An %edge_iterator after the increment operation.

      edge_iterator& operator--()
      {
         --this->mIter;
         return *this;
      }

   private:
      //! Private constructor used to initialize the iterator from a multimap iterator.
      //!
      //! @param The edge multimap iterator.

      edge_iterator(EdgeMapIterator aIter)
         : edge_iterator_base(aIter)
      {
      }
   };

   //! @brief The const edge iterator implementation.
   //!
   //! The edge iterator may be used to browse the edges of a Node.
   //! It may not be used to modify the edges or their labels.

   class const_edge_iterator : public edge_iterator_base
   {
   public:
      friend class UtGraphT<NODE, EDGE, DIRECTED, AUX>;
      friend class Node;
      friend class Edge;


      //! @brief Default constructor.
      //!
      //! Useful for declaring edge_iterators without initialization.

      const_edge_iterator()
         : edge_iterator_base()
      {
      }


      //! @brief Copy constructor.
      //!
      //! Useful for statements like: edge_iterator edgeIter = ...
      //!
      //! @param aEdgeIter An %edge_iterator or a %const_edge_iterator.

      const_edge_iterator(const edge_iterator_base& aEdgeIter)
         : edge_iterator_base(aEdgeIter.mIter)
      {
      }


      //! @brief Assignment.

      const_edge_iterator& operator=(const edge_iterator_base& aEdgeIter)
      {
         if (this != &aEdgeIter)
         {
            edge_iterator_base::copy(aEdgeIter.mIter);
         }
         return *this;
      }

      //! @brief Gets the label of the edge (const).
      //!
      //! Using this operator one retrieve the edge label.
      //!
      //! @return A const reference to the label of the edge.

      const EDGE& operator*() const { return this->mIter->second->mEdge; }

      //! @brief Gets a pointer to the edge label (const).
      //!
      //! If the label type is a complex type, this operator is
      //! useful to perform operations directly on the edge type
      //! by using a simple syntax, e.g.: ei->c_str(), where ei is
      //! the edge iterator and the type is std::string
      //!
      //! @return A const pointer to the label of the edge.

      const EDGE* operator->() const { return &this->mIter->second->mEdge; }

      const Node* fromNode() const { return this->mIter->second->mN1Ptr; }
      const Node* toNode() const { return this->mIter->second->mN2Ptr; }

      //! @brief Gets the start Node of the edge.
      //!
      //! For undirected graphs, it just gets one of the two endpoints.
      //! Use to() to get the other endpoint. This method is synonymous to
      //! using the [] operator, with index = 0, e.g.: ei.from() == ei[0]
      //!
      //! @return A %const_node_iterator to the start Node.

      typename SCOPE(UtGraphType::) const_node_iterator from() const { return this->mIter->second->mN1Ptr->mNodeIter; }

      //! @brief Gets the end Node of the edge.
      //!
      //! For undirected graphs, it just gets one of the two endpoints.
      //! Use from() to get the other endpoint. This method is synonymous to
      //! using the [] operator, with index = 1, e.g.: ei.to() == ei[1]
      //!
      //! @return An %const_node_iterator to the end Node.

      typename SCOPE(UtGraphType::) const_node_iterator to() const { return this->mIter->second->mN2Ptr->mNodeIter; }

      //! @brief Gets the endpoints of the edge
      //!
      //! @param aIndex This may have two values: 0 - gets the start of the edge,
      //! 1 - gets the end of the edge. In the case of undirected graphs, the meaning
      //! of 'start'/'end' of the edge is lost, and this method may be used to return
      //! any of the two endpoints.
      //!
      //! @return An iterator to the required Node. To obtain the Node value, just
      //! add * in front of the result, e.g.: *ei[0] gives the label of the start Node.

      typename SCOPE(UtGraphType::) const_node_iterator operator[](int aIndex) const
      {
         switch (aIndex)
         {
         case 0:
            return this->mIter->second->mN1Ptr->mNodeIter;
         case 1:
            return this->mIter->second->mN2Ptr->mNodeIter;
         default:
            throw "An edge has only two nodes";
         }
      }

      //! @brief Advance to the next edge in the collection.
      //!
      //! @return A %const_edge_iterator previous to the increment operation.

      const_edge_iterator operator++(int)
      {
         const_edge_iterator tmp = *this;
         ++this->mIter;
         return tmp;
      }

      //! @brief Advance to the next edge in the collection.
      //!
      //! @return A %const_edge_iterator after the increment operation.

      const_edge_iterator& operator++()
      {
         ++this->mIter;
         return *this;
      }

      //! @brief Go back to the previous edge in the collection.
      //!
      //! @return A %const_edge_iterator previous to the decrement operation.

      const_edge_iterator operator--(int)
      {
         const_edge_iterator tmp = *this;
         --this->mIter;
         return tmp;
      }

      //! @brief Go back to the previous edge in the collection.
      //!
      //! @return A %const_edge_iterator after the increment operation.

      const_edge_iterator& operator--()
      {
         --this->mIter;
         return *this;
      }

   private:
      //! Private constructor used to initialize th eiterator from a multimap iterator.
      //!
      //! @param aIter The edge multimap iterator.

      const_edge_iterator(EdgeMapIterator aIter)
         : edge_iterator_base(aIter)
      {
      }
   };

   //! @brief Used to manipulate a Node externally (modify it's edges) and
   //! to store a Node internally.
   //!
   //! A Node cannot be instantiated from the outside, but its public methods
   //! may be accessed via the -> operator of the %node_iterator.
   //!
   //! This stores a collection of edges leaving the Node and another collection
   //! of edges entering the Node. In the case of undirected graphs, the two
   //! collections are the same.
   //!
   //! The Node label is not kept in the Node because it's stored
   //! as a key in the nodes multimap. For quick retrieval of the
   //! Node label when a Node * exists, the contained node_iterator should
   //! be used, with the * operator.
   //!
   //! The fact that a node_iterator is contained in this class is due
   //! to the property of the multimap that the node_iterator does not change
   //! when its contents are being modified. This does not mean that it is safe
   //! to iterate a multimap and to remove nodes at the same time,
   //! as the relative order of the nodes may change.

   class Node
   {
   public:
      friend class UtGraphT<NODE, EDGE, DIRECTED, AUX>;
      friend class Edge;
      friend class edge_iterator;
      friend class edge_iterator_base;
      friend class const_edge_iterator;
      friend class compare_node_cost;

      //! @brief Gets the beginning of the collection of edges leaving the Node.
      //!
      //! @return An %edge_iterator pointing to the beginning of the collection.


      edge_iterator begin() { return edge_iterator(mEdges.begin()); }

      //! @brief Gets the beginning of the collection of edges leaving the Node.
      //!
      //! @return A %const_edge_iterator pointing to the beginning of the collection.


      const_edge_iterator begin() const { return const_edge_iterator(mEdges.begin()); }

      //! @brief Gets the end of the collection of edges leaving the Node.
      //!
      //! @return An %edge_iterator pointing to the end of the collection.


      edge_iterator end() { return edge_iterator(mEdges.end()); }

      //! @brief Gets the end of the collection of edges leaving the Node.
      //!
      //! @return An %const_edge_iterator pointing to the end of the collection.


      const_edge_iterator end() const { return const_edge_iterator(mEdges.end()); }

      //! @brief Gets the beginning of the collection of edges entering the Node.
      //!
      //! @return An %edge_iterator pointing to the beginning of the collection.


      edge_iterator begin_rev() { return edge_iterator(mRevEdges.begin()); }

      //! @brief Gets the beginning of the collection of edges entering the Node.
      //!
      //! @return A %const_edge_iterator pointing to the beginning of the collection.


      const_edge_iterator begin_rev() const { return const_edge_iterator(mRevEdges.begin()); }

      //! @brief Gets the end of the collection of edges leaving the Node.
      //!
      //! @return An %edge_iterator pointing to the end of the collection.


      edge_iterator end_rev() { return edge_iterator(mRevEdges.end()); }

      //! @brief Gets the end of the collection of edges leaving the Node.
      //!
      //! @return An %const_edge_iterator pointing to the end of the collection.


      const_edge_iterator end_rev() const { return const_edge_iterator(mRevEdges.end()); }

      //! @brief INTERNAL. Find the iterator pointing to a specified edge in the given collection.

      //! @brief Inserts an edge linking this Node to the specified Node
      //!    and with a given label.
      //!
      //! @param aNodeIter The iterator to the other Node.
      //! @param aEdge     The edge label.
      //!
      //! @return An %edge_iterator pointing to the newly inserted edge


      edge_iterator insert_edge(node_iterator aNodeIter, const EDGE& aEdge)
      {
         bool success = true;

         // get Node pointer.
         Node* n2Ptr = aNodeIter->second;

         // create new edge.
         Edge* edgePtr = new Edge(this, n2Ptr, aEdge);

         // insert new edge into this' collections.
         edge_iterator ret = this->insert_edge(n2Ptr, edgePtr);
         if (ret == this->mEdges.end())
         {
            success = false;
         }

         // If not DIRECTED and it is not linked to itself, add the symmetric edge.
         if (success && !DIRECTED && (this != n2Ptr))
         {
            if (n2Ptr->insert_edge(this, edgePtr) == n2Ptr->mEdges.end())
            {
               success = false;
            }
         }

         if (!success)
         {
            delete edgePtr;
         }

         return ret;
      }

      //! @brief Erases edge given by the iterator
      //!
      //! @param aEdgeIter Iterator to an edge to be deleted.


      void erase_edge(edge_iterator aEdgeIter)
      {
         Edge* edgePtr = aEdgeIter.mIter->second;
         Node* n2Ptr   = (edgePtr->mN1Ptr == this) ? edgePtr->mN2Ptr : edgePtr->mN1Ptr;
         erase_edge(edgePtr, n2Ptr);
      }

      //! @brief Erases the edge between this node and the provided node iterator
      //!
      //! @param aNodeIter Iterator to the other node.


      void erase_edge(node_iterator aNodeIter)
      {
         edge_iterator edgeIter = find_edge(aNodeIter);
         if (edgeIter != mEdges.end())
         {
            erase_edge(edgeIter);
         }
      }

      //! @brief Finds one edge (if it exists) to the given Node.
      //!
      //! In the case of directed graphs, this method returns an edge going
      //! from 'this' Node to the Node specified by aNodeIter. In the case of
      //! undirected graphs, the method returns the same edge regardless if
      //! it is called from 'this' or from 'aNodeIter' Node.
      //!
      //! @param aNodeIter A %node_iterator pointing to a Node whose edge is to be found.
      //!
      //! @return An iterator to an edge linking this Node and the Node
      //! specified by aNodeIter. If more than one edge exist, this method
      //! will return one of them.


      edge_iterator find_edge(node_iterator aNodeIter) { return edge_iterator(mEdges.find(aNodeIter->second)); }

      //! @brief Finds one edge (if it exists) to the given Node. Const version.
      //!
      //! @return A %const_edge_iterator to the found Node, or this->end().


      const_edge_iterator find_edge(node_iterator aNodeIter) const
      {
         return const_edge_iterator(mEdges.find(aNodeIter->second));
      }

      const_edge_iterator find_edge(const_node_iterator aNodeIter) const
      {
         return const_edge_iterator(mEdges.find(aNodeIter->second));
      }

      //! @brief Finds one edge (if it exists) from the given Node.
      //!
      //! In the case of undirected graphs, this function is equivalent to %find_edge.
      //!
      //! @return An %edge_iterator pointing to the found edge.


      edge_iterator find_edge_rev(node_iterator aNodeIter) { return edge_iterator(mRevEdges.find(aNodeIter->second)); }

      //! @brief Finds one edge (if exists) from the given Node. Const version.
      //!
      //! @return A %const_edge_iterator to the found Node, or this->end_rev().


      const_edge_iterator find_edge_rev(node_iterator aNodeIter) const
      {
         return const_edge_iterator(mRevEdges.find(aNodeIter->second));
      }

      //! @brief Gets the size of the out edges collection
      //!
      //! In the case of undirected graphs, the size is equal to that of in edges.
      //!
      //! @return The number of edges going out of the Node.

      int count_edges() const { return static_cast<int>(mEdges.size()); }

      //! @brief Gets the size of the in edges collection
      //!
      //! In the case of undirected graphs, the size is equal to that of out edges.
      //!
      //! @return The number of edges going into the Node.

      int count_edges_rev() const { return (int)mRevEdges.size(); }

      //! @brief Sets the user-defined aux for the Node

      void set_aux(const AUX& aAux) { mAux = aAux; }

      //! @brief Gets the user-defined aux for the Node

      AUX get_aux() const { return mAux; }

      node_index_t get_index() const { return mIndex; }

   private:
      //! @brief Private constructor called only from UtGraphT class.
      //!
      //! @param _ni A %node_iterator pointing to this Node. The multimap
      //!    specifications guarantees us that it will not change during
      //!   modifications made to the map.
      //! @param g A pointer to the containing %UtGraphT.

      // private

      Node()
         : mGraphPtr(0)
      {
      }

      Node(node_iterator aNodeIter, UtGraphType* aGraphPtr, node_index_t aIndex)
         : mNodeIter(aNodeIter)
         , mGraphPtr(aGraphPtr)
         , mIndex(aIndex)
      {
      }

      //! @brief Node destructor.
      //!
      //! The destructor removes all references to edges linking this Node
      //! to other nodes of the graph. It also destroys these edges and frees
      //! memory

      // private

      ~Node() { erase_all_edges(); }

      //!
      //! @param aEdgePtr The edge to be found.
      //! @param aNodePtr A %Node * representing the other endpoint of the edge. Used for search hinting.
      //! @param aEdges   The multimap we're searching in.

      // private

      EdgeMapIterator find(Edge* aEdgePtr, Node* aNodePtr, EdgeMap& aEdges)
      {
         EdgeMapIterator iter;
         for (iter = aEdges.lower_bound(aNodePtr); iter != aEdges.upper_bound(aNodePtr); ++iter)
         {
            if (iter->second == aEdgePtr)
            {
               return iter;
            }
         }
         return aEdges.end();
      }

      //! @brief INTERNAL. Erase a specified edge from the given collection.
      //!
      //! @param aEdgePtr The edge to be deleted.
      //! @param aNodePtr A %Node * representing the other endpoint of the edge. Used for search hinting.
      //!
      //! This method is only used to remove an edge reference from the given collection.
      //! It does not deallocate the Edge pointer.

      // private

      void erase_edge_no_delete(Edge* aEdgePtr, Node* aNodePtr, EdgeMap& aEdges)
      {
         edge_iterator iter;
         for (iter = aEdges.lower_bound(aNodePtr); iter != aEdges.upper_bound(aNodePtr); ++iter)
         {
            if (iter.mIter->second == aEdgePtr)
            {
               aEdges.erase(iter.mIter);
               break;
            }
         }
      }

      //! @brief INTERNAL. Erase a specified edge from both collections.
      //!
      //! @param aEdgePtr The edge to be deleted.
      //! @param aNodePtr A %Node * representing the other endpoint of the edge. Used for search hinting.
      //!
      //! This method is only used to remove an edge reference from both edges and mRevEdges collections.
      //! It does not deallocate the Edge pointer.

      // private

      void erase_edge_no_delete(Edge* aEdgePtr, Node* aNodePtr)
      {
         erase_edge_no_delete(aEdgePtr, aNodePtr, mEdges);
         erase_edge_no_delete(aEdgePtr, aNodePtr, mRevEdges);
      }

      //! @brief INTERNAL. Removes an edge from the %UtGraphT and deallocates it.
      //!
      //! @param aEdgePtr Pointer to the edge to be removed.
      //! @param aNodePtr The other end of the edge, used for search hinting.
      //!
      //! This method removes the specified edge between this Node and Node aNodePtr.
      //! If the graph is directed, it does not remove edges going from Node aNodePtr to
      //! this Node. If the graph is undirected, it removes the simetric edge
      //! from Node aNodePtr to this Node.

      // private

      void erase_edge(Edge* aEdgePtr, Node* aNodePtr)
      {
         // remove e from n's collections.
         aNodePtr->erase_edge_no_delete(aEdgePtr, this);

         // remove e from this' collections.
         erase_edge_no_delete(aEdgePtr, aNodePtr);

         // delete the actual edge structure.
         delete aEdgePtr;
      }

      //! @brief INTERNAL. Removes and deallocates all edges between two nodes.
      //!
      //! @param aNodePtr The other end of the edge.
      //!
      //! This method removes all edges between this Node and Node aNodePtr. If the graph
      //! is directed, it does not remove edges going from Node aNodePtr to this Node. If
      //! the graph is undirected, all edges between this and aNodePtr are removed.

      // private

      void erase_all_edges(Node* aNodePtr)
      {
         while (true)
         {
            edge_iterator iter = mEdges.find(aNodePtr);
            if (iter.mIter == mEdges.end())
            {
               break;
            }
            erase_edge(iter.mIter->second, aNodePtr);
         }
      }

      //! @brief Erases all edges linking this Node to the given Node.
      //!
      //! For directed graphs, it removes only edges going from this Node to the
      //! Node received as argument. It does not remove edges going from aNodeIter to this.
      //!
      //! @param aNodeIter A %node_iterator pointing to the Node whose links are to be removed.

      // private

      void erase_all_edges(node_iterator aNodeIter) { erase_all_edges(aNodeIter.mIter->second); }

      //! @brief INTERNAL. Erases all the edges leaving or entering this Node.
      //!
      //! Also handles memory deallocation for them.

      // private

      void erase_all_edges()
      {
         edge_iterator iter, next, i_end;

         // once forward edges, then reverse edges
         for (int i = 0; i < 2; ++i)
         {
            iter  = i ? begin_rev() : begin();
            i_end = i ? end_rev() : end();

            // remove each edge
            for (; iter != i_end; iter = next)
            {
               next = iter;
               ++next;

               Edge* edgePtr = iter.mIter->second;

               // get the other end of the edge.
               Node* n2Ptr = (edgePtr->mN1Ptr == this) ? edgePtr->mN2Ptr : edgePtr->mN1Ptr;
               erase_edge(edgePtr, n2Ptr);
            }
         }
      }

      //! @brief INTERNAL. Add directed link.
      //!
      //! @param aNode2Ptr A %Node* to link this Node with.
      //! @param aEdgePtr  Edge created previously, will be referenced in the mEdges/mRevEdges multimaps.
      //!
      //! @return An %edge_iterator to the newly inserted edge. The iterator is for the out edges collection.
      //! The iterator is for the out edges collection.  mEdges.end()
      //! is returned if either insertion failed.

      // private

      edge_iterator insert_edge(Node* aNode2Ptr, Edge* aEdgePtr)
      {
         // add link to this Node.
         edge_iterator ret1 = mEdges.insert(EdgeMapValueType(aNode2Ptr, aEdgePtr));
         // If failed return the end iterator
         if (ret1 == mEdges.end())
         {
            return ret1;
         }


         // add link to the other end's reverse links.
         edge_iterator ret2 = aNode2Ptr->mRevEdges.insert(EdgeMapValueType(this, aEdgePtr));

         // If either iterator is invalid, then return an invalid iterator.
         if (ret2 == aNode2Ptr->mRevEdges.end())
         {
            mEdges.erase(ret1.mIter);
            return mEdges.end();
         }

         return ret1;
      }

      // Multimap of edges leaving the Node. The key is a Node *,
      // because it uniquely identifies a Node.
      mutable EdgeMap mEdges;

      // Multimap of edges entering the Node. The key is a Node *,
      // because it uniquely identifies a Node.
      mutable EdgeMap mRevEdges;

      //! @brief For performance reasons, a %node_iterator pointing to this Node
      //! is cached.
      //!
      //! Node iterators may be cached because:
      //! "Multimap has the important property that inserting an element into
      //! the multimap does not invalidate iterators that point to existing elements.
      //! Erasing an element from the multimap also does not invalidate any iterators
      //! except, of course, for iterators that actually point to the element
      //! that is being erased"
      node_iterator mNodeIter;

      //! Pointer to the containing graph.
      UtGraphType* mGraphPtr;

      //! A generic aux to be used in various algorithms.
      AUX mAux;

      //! The node's unique index. The indexes are zero based [0, 1, ...]
      node_index_t mIndex;

      //! @name Bidirection A* search properties
      //! @{

      mutable double dist_a;
      mutable double dist_b;

      mutable unsigned dist_a_tag = 0;
      mutable unsigned dist_b_tag = 0;
      mutable unsigned closed_tag = 0;

      mutable node_index_t pq_index_a = cInvalidIndex;
      mutable node_index_t pq_index_b = cInvalidIndex;
      //! check that this node is in the forward frontier priority queue.
      bool IsOpenA() const { return pq_index_a != cInvalidIndex; }
      //! check that this node is in the reverse frontier priority queue.
      bool IsOpenB() const { return pq_index_b != cInvalidIndex; }

      mutable unsigned mChildDepth;
      //! An iterator that can be used by algorithms to chain nodes.
      mutable const_node_iterator mChildIter;
      mutable const_node_iterator mParentIter;

      //! @}
   };

   //! @brief Nested class that represents an edge.
   //!
   //! This will not be used directly from outside the %UtGraphT class.
   //! The motivation of using a separate structure to keep the edge label
   //! end endpoints is in the case of undirected graphs, we
   //! have two references to the same edge. Also, by keeping a collection
   //! of edges 'going into' a Node, we double the references to the edges.

   class Edge
   {
   private:
      friend class Node;
      friend class edge_iterator_base;
      friend class edge_iterator;
      friend class const_edge_iterator;
      friend class UtGraphT<NODE, EDGE, DIRECTED, AUX>;

      //! The Edge constructor is only available to friend classes

      Edge(Node* aN1Ptr, Node* aN2Ptr, const EDGE& aEdge, bool aValid = true)
         : mN1Ptr(aN1Ptr)
         , mN2Ptr(aN2Ptr)
         , mEdge(aEdge)
         , mValid(aValid)
      {
      }

      ~Edge() = default;

      Node* mN1Ptr; //! The 'source' Node of the edge (in case of directed graphs).
      //! For undirected graphs, it's just one of the two extremities.
      Node* mN2Ptr; //! The 'destination' Node of the edge (in case of directed graphs).
      //! For undirected graphs, it's just one of the two extremities.
      EDGE mEdge;  //! The actual edge label is kept here. Only one copy for each edge.
      bool mValid; //! Indicates whether the edge is currently valid.
   };

public:
   //! @brief The cost_func functor is used by the shortest_path
   //!        method to provide a default value for the edge cost (1)
   //!        and heuristic cost (0).
   //!        Users should overload this class to provide a custom
   //!        edge cost function.

   class cost_func
   {
   public:
      cost_func()          = default;
      virtual ~cost_func() = default;

      virtual double operator()(const EDGE& /*aEdge*/, const NODE& /*aNode1*/, const NODE& /*aNode2*/) const
      {
         return 1.0;
      }

      virtual double operator()(const NODE& /*aNode1*/, const NODE& /*aNode2*/) const { return 0.0; }

      virtual bool consider_node(const NODE& /*aNode*/) const { return true; }
   };
   struct AlwaysConsiderNode
   {
      bool operator()(const NODE&) const { return true; }
   };

   template<bool IsA>
   class node_priority_queue
   {
   public:
      struct PQNode
      {
         double      f;
         const Node* node;
      };

      node_priority_queue()                           = default;
      node_priority_queue(const node_priority_queue&) = delete;
      node_priority_queue& operator=(const node_priority_queue&) = delete;
      ~node_priority_queue() { clear(); }

      void clear()
      {
         for (size_t i = 0; i < mSize; ++i)
         {
            set_priority_queue_index(mHeap[i], cInvalidIndex);
         }
         mSize = 0;
      }

      bool empty() const { return mSize == 0; }

      node_index_t size() const { return mSize; }

      double      topF() const { return mHeap[0].f; }
      const Node& top() const { return *mHeap[0].node; }

      void set_capacity(node_index_t aCapacity)
      {
         if (mCapacity < aCapacity)
         {
            clear();
            mHeap     = ut::make_unique<PQNode[]>(aCapacity);
            mCapacity = aCapacity;
         }
      }

      void push(double aPriority, const Node* aNode)
      {
         assert(mSize < mCapacity);
         mHeap[mSize] = {aPriority, aNode};
         ++mSize;
         push_heap(Begin(), End());
      }

      void pop()
      {
         assert(mSize > 0);
         node_index_t& pqindex = IsA ? mHeap[0].node->pq_index_a : mHeap[0].node->pq_index_b;
         assert(pqindex == 0);
         pop_heap(Begin(), End());
         --mSize;
         pqindex = cInvalidIndex;
      }

      void priority_changed(const Node& aNode, double aNewPriority)
      {
         node_index_t index  = IndexInHeap(aNode);
         double       prev_f = mHeap[index].f;
         if (prev_f == aNewPriority)
         {
            return;
         }
         mHeap[index].f = aNewPriority;
         if (compare(prev_f, aNewPriority))
         {
            push_heap(Begin(), Begin() + index + 1);
         }
         else
         {
            bubble_down(Begin(), End(), index);
         }
         // this assert is not active by default because it increases the
         // algorithmic complexity of this function from O(log N) to O(N).
         // assert(std::is_heap(Begin(), End(),[&](const PQNode& a, const PQNode& b) {return compare(a, b); }));
      }

      double priority_of(const Node& aNode) { return mHeap[IndexInHeap(aNode)].f; }

   private:
      bool compare(const PQNode& lhs, const PQNode& rhs) const
      {
         return lhs.f > rhs.f && !UtMath::NearlyEqual(lhs.f, rhs.f);
      }
      bool compare(double lhs_f, double rhs_f) const { return lhs_f > rhs_f && !UtMath::NearlyEqual(lhs_f, rhs_f); }

      node_index_t IndexInHeap(const Node& aNode) const
      {
         node_index_t index = IsA ? aNode.pq_index_a : aNode.pq_index_b;
         assert(index < mSize);
         assert(mHeap[index].node == &aNode);
         return index;
      }

      void set_priority_queue_index(const PQNode& aNode, node_index_t aIndex) const
      {
         if (IsA)
            aNode.node->pq_index_a = aIndex;
         else
            aNode.node->pq_index_b = aIndex;
      }

      using iterator = PQNode*;
      iterator Begin() { return mHeap.get(); }
      iterator End() { return Begin() + mSize; }

      //! manual implementation of std::push_heap to ensure the
      //! priority_queue_index invariant on nodes is preserved.
      void push_heap(iterator begin, iterator end)
      {
         --end;
         PQNode       value = std::move(*end);
         node_index_t index = static_cast<node_index_t>(end - begin);
         while (index > 0)
         {
            node_index_t parent = (index - 1) / 2;
            if (!compare(begin[parent], value))
               break;
            PQNode& child = begin[index];
            child         = std::move(begin[parent]);
            set_priority_queue_index(child, index);
            index = parent;
         }
         set_priority_queue_index(value, index);
         begin[index] = std::move(value);
      }

      //! manual implementation of std::pop_heap to ensure the
      //! priority_queue_index invariant on nodes is preserved.
      void pop_heap(iterator begin, iterator end)
      {
         --end;
         *begin = std::move(*end);
         bubble_down(begin, end, 0);
      }

      void bubble_down(iterator begin, iterator end, node_index_t index)
      {
         node_index_t length = static_cast<node_index_t>(end - begin);
         PQNode       value  = std::move(begin[index]);
         for (;;)
         {
            node_index_t last_child  = index * 2 + 2;
            node_index_t first_child = last_child - 1;
            if (last_child < length)
            {
               iterator largest_child = begin + first_child;
               // branchless add 0 or 1 instead of conditional add 1.
               largest_child += !!compare(largest_child[0], largest_child[1]);

               if (!compare(value, *largest_child))
                  break;
               set_priority_queue_index(*largest_child, index);
               begin[index] = std::move(*largest_child);
               index        = static_cast<node_index_t>(largest_child - begin);
            }
            else if (first_child < length)
            {
               iterator largest_child = begin + first_child;
               if (compare(value, *largest_child))
               {
                  set_priority_queue_index(*largest_child, index);
                  begin[index] = std::move(*largest_child);
                  index        = static_cast<node_index_t>(largest_child - begin);
               }
               break;
            }
            else
               break;
         }
         set_priority_queue_index(value, index);
         begin[index] = std::move(value);
      }

      std::unique_ptr<PQNode[]> mHeap;
      node_index_t              mSize     = 0;
      node_index_t              mCapacity = 0;
   };

   //! Uses the 'new bidirection A* search algorithm (NBA*)'.
   //! Becomes a bidirectional breadth first search if no weighting or heuristic is provided.
   //!
   //! @param aSrcNodeIter - the point that the searched path shall start at.
   //! @param aDstNodeIter - the point that the searched path shall end at.
   //! @param aCost - the total cost of the discovered path
   //! @param aFindOptimalPath - true if the optimal shortest (least costly) path is to be discovered,
   //!                           otherwise the algorithm will terminate when it first finds a path.
   //! @param aPathPtr - if non-null, will contain the discovered path.
   //!                   use nullptr if this information does not need to be constructed.
   //! @param aEdgeWeight - a functor that returns the weight of an edge. defaults to constant 1.0
   //!                      signature: double operator()(const EDGE&,const NODE&,const NODE&)
   //! @param aHeuristic - a functor that returns the heuristic from a node to the start or end points.
   //!                     The optimal path is not guarenteed if the heuristic overestimates the true distance between
   //!                     nodes. The closer a heuristic is to the true cost between nodes, the faster the algorithm
   //!                     will complete. defaults to constant 0.0 signature: double operator()(const NODE&,const NODE&)
   //! @param aConsiderNode - a functor returns whether or not a node should be ignored as a candidate in the search
   //! path
   //!                        default all nodes are considered
   //!                        signature: bool operator()(const NODE&)
   template<class EdgeWeight = cost_func, class Heuristic = cost_func, class NodeConsidered = AlwaysConsiderNode>
   bool find_path(const_node_iterator aSrcNodeIter,
                  const_node_iterator aDstNodeIter,
                  double&             aCost,
                  bool                aFindOptimalPath,
                  NodeList*           aPathPtr      = nullptr,
                  EdgeWeight&&        aEdgeWeight   = {},
                  Heuristic&&         aHeuristic    = {},
                  NodeConsidered&&    aConsiderNode = {}) const;

private:
   mutable node_priority_queue<true>  openA;
   mutable node_priority_queue<false> openB;
   mutable unsigned                   tag = 0;
};

//! Define a couple of macros to simplify the syntax required to scope UtGraphT.

#define UT_GRAPH_TEMPLATE_ARGS template<typename NODE, typename EDGE, bool DIRECTED, typename AUX>
#define UT_GRAPH UtGraphT<NODE, EDGE, DIRECTED, AUX>

// UT_GRAPH_TEMPLATE_ARGS
// unsigned int UT_GRAPH::sNodeIndex = 0;

//! UtGraphT method implementations

//! @brief Default constructor.

UT_GRAPH_TEMPLATE_ARGS
UT_GRAPH::UtGraphT()
   : mNextNodeIndex(0)
   , mNodes()
{
}

//! @brief Destructor. Deletes all nodes and edges of a graph

// virtual
UT_GRAPH_TEMPLATE_ARGS
UT_GRAPH::~UtGraphT()
{
   clear();
}

//! @brief Copy constructor
//!
//! @param aGraph A %UtGraphT to be copied.
//!
//! Creates a new graph by copying all the structures and labels of the
//! argument. This may be used in assignment operations, e.g.: g1 = g2;

UT_GRAPH_TEMPLATE_ARGS
UT_GRAPH::UtGraphT(const UtGraphType& aGraph)
{
   copy(aGraph);
}

//! @brief Assignment operator.
//!
//! @param aGraph A %UtGraphT to be assigned from.
//!
//! Copies all the nodes and edges of the graph given as argument.
//
//! @return A reference to the right hand side of the assignment.

UT_GRAPH_TEMPLATE_ARGS
UT_GRAPH& UT_GRAPH::operator=(const UtGraphType& aGraph)
{
   copy(aGraph);
   return *this;
}

//! @brief Gets the start of the nodes collection.
//!
//! @return An iterator to the beginning of the collection.

UT_GRAPH_TEMPLATE_ARGS
typename UT_GRAPH::node_iterator UT_GRAPH::begin()
{
   return node_iterator(mNodes.begin());
}

//! @brief Gets the start of the nodes collection.
//!
//! @return An iterator to the beginning of the collection.

UT_GRAPH_TEMPLATE_ARGS
typename UT_GRAPH::const_node_iterator UT_GRAPH::begin() const
{
   return const_node_iterator(mNodes.begin());
}

//! @brief Gets the end of the nodes collection.
//!
//! @return An iterator to the end of the collection.

UT_GRAPH_TEMPLATE_ARGS
typename UT_GRAPH::node_iterator UT_GRAPH::end()
{
   return node_iterator(mNodes.end());
}

//! @brief Gets the end of the nodes collection.
//!
//! @return An iterator to the end of the collection.

UT_GRAPH_TEMPLATE_ARGS
typename UT_GRAPH::const_node_iterator UT_GRAPH::end() const
{
   return const_node_iterator(mNodes.end());
}

//! @brief Inserts a Node into the graph.
//!
//! @param aLabel The label of the Node to be insert. Will be copied into the
//! %UtGraphT structures and it may not be modified later.
//!
//! @return A %node_iterator pointing to the newly inserted Node.

UT_GRAPH_TEMPLATE_ARGS
typename UT_GRAPH::node_iterator UT_GRAPH::insert(const NODE& aLabel)
{
   // Create a Node with an invalid iterator. The iterator will be set after
   // the Node is inserted into the multimap.
   Node*           nodePtr = new Node(mNodes.end(), this, this->mNextNodeIndex++);
   NodeMapIterator iter    = mNodes.insert(NodeMapValueType(aLabel, nodePtr));

   // Set the iterator for the newly added Node.
   nodePtr->mNodeIter = node_iterator(iter);

   return nodePtr->mNodeIter;
}

//! Erases a Node from the nodes collection and destroys it.
//! @brief Erase a Node from the %UtGraphT
//!
//! @param aNodeIter A %node_iterator or %const_node_iterator pointing to the Node to be removed.

UT_GRAPH_TEMPLATE_ARGS
void UT_GRAPH::erase(node_iterator aNodeIter)
{
   // delete Node *, this also deletes all links.
   delete aNodeIter->second;

   // remove from map.
   mNodes.erase(aNodeIter);
}

//! @brief Erase one or more nodes from the %UtGraphT
//!
//! Note that a graph may have multiple nodes with the same
//! label. In this case, calling this function will remove all of
//! them.
//!
//! @param Node The label of the Node to be deleted.

UT_GRAPH_TEMPLATE_ARGS
void UT_GRAPH::erase(const NODE& aNode)
{
   // delete the nodes
   NodeMapIterator iter;
   NodeMapIterator endIter = mNodes.upper_bound(aNode);
   for (iter = mNodes.lower_bound(aNode); iter != endIter; ++iter)
   {
      delete iter->second;
   }

   // erase from map.
   mNodes.erase(aNode);
}

//! @brief Inserts an edge into the graph.
//!
//! @param aNode1Iter A %node_iterator or %const_node_iterator pointing to the start Node of the edge.
//!  If the %UtGraphT is undirected, this argument represents one end of the edge.
//! @param aNode2Iter A %node_iterator or %const_node_iterator pointing to the end Node of the edge.
//! If the %UtGraphT is undirected, this argument represents the other end of the edge.
//! @param edge The label of the edge to be inserted.
//!
//! @return A %edge_iterator pointing to the newly inserted edge.

UT_GRAPH_TEMPLATE_ARGS
typename UT_GRAPH::edge_iterator UT_GRAPH::insert_edge(node_iterator aNode1Iter, node_iterator aNode2Iter, const EDGE& aEdge)
{
   return aNode1Iter->second->insert_edge(aNode2Iter, aEdge);
}

//! @brief Erases all the edges connecting two nodes.
//!
//! @param aNode1Iter A %node_iterator or %const_node_iterator pointing to the start Node of the edge(s) to be removed.
//! If the %UtGraphT is undirected, this argument represents one end of the edge.
//! @param aNode2Iter A %node_iterator or %const_node_iterator pointing to the end Node of the edge(s) to be removed.
//! If the %UtGraphT is undirected, this argument represents the other end of the edge.

UT_GRAPH_TEMPLATE_ARGS
void UT_GRAPH::erase_edge(node_iterator aNode1Iter, node_iterator aNode2Iter)
{
   aNode1Iter->second->erase_edge(aNode2Iter);
}

//! @brief Finds a Node by its label.
//!
//! @param aLabel The label to be searched.
//!
//! Note that a %UtGraphT may contain several nodes with the same label.
//! In that case, the method will arbitrarily return one of them.
//!
//! @return A %node_iterator pointing to the found Node, or %UtGraphT::end()
//!   in case it doesn't find anything.

UT_GRAPH_TEMPLATE_ARGS
typename UT_GRAPH::node_iterator UT_GRAPH::find(const NODE& aLabel)
{
   typename NodeMap::iterator iter = mNodes.find(aLabel);
   return node_iterator(iter);
}

//! @brief Finds a Node by its label (const version)
//!
//! @param aLabel The label to be seached.

UT_GRAPH_TEMPLATE_ARGS
typename UT_GRAPH::const_node_iterator UT_GRAPH::find(const NODE& aLabel) const
{
   return const_node_iterator(mNodes.find(aLabel));
}

//! @brief Returns the number of nodes.
//!
//! @return The number of nodes in the graph.

UT_GRAPH_TEMPLATE_ARGS
int UT_GRAPH::count_nodes() const
{
   return static_cast<int>(mNodes.size());
}

//! @brief deletes all edges from all nodes

UT_GRAPH_TEMPLATE_ARGS
void UT_GRAPH::erase_all_edges()
{
   mNextNodeIndex = 0;

   // Delete all the edges before deleting the nodes
   NodeMapIterator iter;
   for (iter = mNodes.begin(); iter != mNodes.end(); ++iter)
   {
      iter->second->erase_all_edges();
   }
}

//! @brief deletes all edges from a node.

UT_GRAPH_TEMPLATE_ARGS
void UT_GRAPH::erase_all_edges(node_iterator aNodeIter)
{
   aNodeIter->second->erase_all_edges();
}

//! @brief Deletes all nodes and edges of a graph.

UT_GRAPH_TEMPLATE_ARGS
void UT_GRAPH::clear()
{
   mNextNodeIndex = 0;

   // Delete all the edges before deleting any nodes.
   NodeMapIterator iter;
   for (iter = mNodes.begin(); iter != mNodes.end(); ++iter)
   {
      iter->second->erase_all_edges();
   }

   // Delete all the nodes.
   for (iter = mNodes.begin(); iter != mNodes.end(); ++iter)
   {
      delete iter->second;
   }
   mNodes.clear();
}

UT_GRAPH_TEMPLATE_ARGS
bool UT_GRAPH::is_directed() const
{
   return DIRECTED;
}

UT_GRAPH_TEMPLATE_ARGS
template<class EdgeWeight, class Heuristic, class NodeConsidered>
bool UT_GRAPH::find_path(const_node_iterator aSrcNodeIter,
                         const_node_iterator aDstNodeIter,
                         double&             aCost,
                         bool                aFindOptimalPath,
                         NodeList*           aPathPtr,         // = nullptr
                         EdgeWeight&&        aEdgeWeight,      // = always 1.0
                         Heuristic&&         aHeuristic,       // = always 0.0
                         NodeConsidered&&    aConsiderNode) const // = always true
{
   // This function is an implementation of the New Bidirectional A* (NBA*) algorithm.

   aCost = 0.0;
   if (aPathPtr)
   {
      aPathPtr->clear();
   }

   // When asked to find a path from a node to the same node, the NBA*
   // algorithm determines there is no path. This check handles that case
   // explicitly, so that a zero-length path is returned instead.
   if (aSrcNodeIter == aDstNodeIter)
   {
      if (aPathPtr)
      {
         aPathPtr->push_back(aSrcNodeIter->first);
      }
      return true;
   }

   const NODE& dstN = aDstNodeIter->first;

   assert(openA.empty());
   assert(openB.empty());
   auto nodeCount = static_cast<node_index_t>(mNodes.size());
   openA.set_capacity(nodeCount);
   openB.set_capacity(nodeCount);

   ++tag;
   if (tag == 0)
   {
      // tag counter has rolled over, need to reset all tags to ensure
      // there isn't any collisions from previous searches.
      for (const auto& n : mNodes)
      {
         n.second->dist_a_tag = tag;
         n.second->dist_b_tag = tag;
         n.second->closed_tag = tag;
      }
      tag = 1;
   }

   double lMin = std::numeric_limits<double>::max();

   const Node* srcNode = aSrcNodeIter->second;
   srcNode->dist_a     = 0.0;
   srcNode->dist_a_tag = tag;
   // srcNode->mParentDepth = 0;
   const NODE& srcN = srcNode->mNodeIter->first;
   double      fA   = aHeuristic(srcN, dstN);
   double      fB   = fA;
   openA.push(fA, srcNode);

   const Node* dstNode  = aDstNodeIter->second;
   dstNode->dist_b      = 0.0;
   dstNode->dist_b_tag  = tag;
   dstNode->mChildDepth = 0;
   openB.push(fB, dstNode);

   const Node* minPoint = nullptr;

   auto is_closed  = [&](const Node& n) { return n.closed_tag == tag; };
   auto set_closed = [&](const Node& n) { n.closed_tag = tag; };

   // ============================================
   auto forwardSearch = [&]()
   {
      double      cameFromF = openA.topF();
      const Node& cameFrom  = openA.top();
      openA.pop();
      if (is_closed(cameFrom))
         return;
      set_closed(cameFrom);

      const NODE& cameFromT = cameFrom.mNodeIter->first;
      if (cameFromF >= lMin)
      {
         openA.clear();
      }
      else if (cameFrom.dist_a + fB - aHeuristic(srcN, cameFromT) < lMin)
      {
         const_edge_iterator edgeIter;
         for (edgeIter = cameFrom.begin(); edgeIter != cameFrom.end(); ++edgeIter)
         {
            // If the edge is invalid, skip it.
            if (!edgeIter.valid())
               continue;

            const Node& nextN = (edgeIter.fromNode() == &cameFrom) ? *edgeIter.toNode() : *edgeIter.fromNode();

            // Detect and avoid cases where a node has an edge to its self.
            if (&nextN == &cameFrom)
               continue;

            if (is_closed(nextN))
               continue;

            const NODE& nextT = nextN.mNodeIter->first;

            // If the user has specified to ignore the next node, skip this edge.
            if (!aConsiderNode(nextT))
               continue;

            // Calculate the new propagated edge cost.
            // The default edge cost from one node to the next is '1', but users can define there
            // 'edge cost function' that calculates the cost.
            double new_dist = cameFrom.dist_a + aEdgeWeight(*edgeIter, cameFromT, nextT);

            if (nextN.dist_a_tag != tag || new_dist < nextN.dist_a)
            {
               nextN.dist_a      = new_dist;
               nextN.mParentIter = cameFrom.mNodeIter;
               // add/update nextN on the forward (A) frontier.
               double f = new_dist + aHeuristic(nextT, dstN);

               if (!nextN.IsOpenA())
               {
                  nextN.dist_a_tag = tag;
                  openA.push(f, &nextN);
               }
               else
               {
                  assert(nextN.dist_a_tag == tag);
                  openA.priority_changed(nextN, f);
               }

               if (nextN.dist_b_tag == tag)
               {
                  // found intersection on the A and B frontiers, check if this intersection
                  // is the most optimal path so far.
                  double potentialMin = nextN.dist_a + nextN.dist_b;
                  if (potentialMin < lMin)
                  {
                     lMin     = potentialMin;
                     minPoint = &nextN;
                     if (!aFindOptimalPath)
                     {
                        return;
                     }
                  }
               }
            }
         } // end for each forward edge
      }
      if (!openA.empty())
      {
         fA = openA.topF();
      }
   };

   // ============================================
   auto reverseSearch = [&]()
   {
      double      cameFromF = openB.topF();
      const Node& cameFrom  = openB.top();
      openB.pop();
      if (is_closed(cameFrom))
         return;
      set_closed(cameFrom);

      const NODE& cameFromT = cameFrom.mNodeIter->first;
      if (cameFromF >= lMin)
      {
         openB.clear();
      }
      else if (cameFrom.dist_b + fA - aHeuristic(cameFromT, dstN) < lMin)
      {
         auto                new_depth = cameFrom.mChildDepth + 1;
         const_edge_iterator edgeIter;
         for (edgeIter = cameFrom.begin_rev(); edgeIter != cameFrom.end_rev(); ++edgeIter)
         {
            // If the edge is invalid, skip it.
            if (!edgeIter.valid())
               continue;

            const Node& nextN = (edgeIter.fromNode() == &cameFrom) ? *edgeIter.toNode() : *edgeIter.fromNode();

            // Detect and avoid cases where a node has an edge to its self.
            if (&nextN == &cameFrom)
               continue;

            if (is_closed(nextN))
               continue;

            const NODE& nextT = nextN.mNodeIter->first;

            // If the user has specified to ignore the next node, skip this edge.
            if (!aConsiderNode(nextT))
               continue;

            // Calculate the new propagated edge cost.
            // The default edge cost from one node to the next is '1', but users can define there
            // 'edge cost function' that calculates the cost.
            double new_dist = cameFrom.dist_b + aEdgeWeight(*edgeIter, nextT, cameFromT);

            if (nextN.dist_b_tag != tag || new_dist < nextN.dist_b)
            {
               nextN.dist_b      = new_dist;
               nextN.mChildDepth = new_depth;
               nextN.mChildIter  = cameFrom.mNodeIter;

               // add/update nextN on the reverse (B) frontier.
               double f = new_dist + aHeuristic(srcN, nextT);
               if (!nextN.IsOpenB())
               {
                  nextN.dist_b_tag = tag;
                  openB.push(f, &nextN);
               }
               else
               {
                  assert(nextN.dist_b_tag == tag);
                  openB.priority_changed(nextN, f);
               }

               if (nextN.dist_a_tag == tag)
               {
                  // found intersection on the A and B frontiers, check if this intersection
                  // is the most optimal path so far.
                  double potentialMin = nextN.dist_a + nextN.dist_b;
                  if (potentialMin < lMin)
                  {
                     lMin     = potentialMin;
                     minPoint = &nextN;
                     if (!aFindOptimalPath)
                     {
                        return;
                     }
                  }
               }
            }
         } // end for each reverse edge
      }
      if (!openB.empty())
      {
         fB = openB.topF();
      }
   };

   // ============================================

   while (!openA.empty() && !openB.empty())
   {
      if (openA.size() <= openB.size())
      {
         forwardSearch();
      }
      else
      {
         reverseSearch();
      }

      if (!aFindOptimalPath && minPoint)
         break;
   }

   // ============================================

   openA.clear();
   openB.clear();

   if (minPoint == nullptr)
   {
      // no path discovered
      aCost = -1.0;
      return false;
   }

   aCost = lMin;

   auto build_path = [&]()
   {
      if (!aPathPtr)
         return;


      struct PathBuilder
      {
         NodeList&           mPath;
         const_node_iterator mSource;
         unsigned            mChildDepth;
         void                BuildPath(const_node_iterator aNodeIter, size_t aDepth = 1)
         {
            if (aNodeIter == mSource)
            {
               mPath.reserve(aDepth + mChildDepth);
            }
            else
            {
               BuildPath(aNodeIter->second->mParentIter, aDepth + 1);
            }
            mPath.push_back(aNodeIter->first);
         }
      };
      PathBuilder{*aPathPtr, aSrcNodeIter, minPoint->mChildDepth}.BuildPath(minPoint->mNodeIter);

      const_node_iterator aNodeIter = minPoint->mNodeIter;
      while (aNodeIter != aDstNodeIter)
      {
         aNodeIter = aNodeIter->second->mChildIter;
         aPathPtr->push_back(aNodeIter->first);
      }
   };

   build_path();

   return true;
}

UT_GRAPH_TEMPLATE_ARGS
bool UT_GRAPH::shortest_path(const_node_iterator aSrcNodeIter,
                             const_node_iterator aDstNodeIter,
                             NodeList&           aPath,
                             double&             aCost,
                             const cost_func*    aCostFuncPtr) const
{
   const cost_func defaultCostFunc = cost_func();
   if (aCostFuncPtr == nullptr)
   {
      aCostFuncPtr = &defaultCostFunc;
   }


   return find_path(aSrcNodeIter,
                    aDstNodeIter,
                    aCost,
                    true,
                    &aPath,
                    *aCostFuncPtr,
                    *aCostFuncPtr,
                    [aCostFuncPtr](const NODE& n) { return aCostFuncPtr->consider_node(n); });
}

//! Make a copy of the graph.

// private
UT_GRAPH_TEMPLATE_ARGS
void UT_GRAPH::copy(const UtGraphType& aGraph)
{
   const_node_iterator cni;

   // clear this graph.
   clear();

   // keep a Node* to Node* mapping to find nodes easier later on.
   using NodeToNodeMap = std::map<const Node*, Node*>;
   NodeToNodeMap tmpmap;

   // add nodes from aGraph.
   for (cni = aGraph.begin(); cni != aGraph.end(); ++cni)
   {
      node_iterator                      nodeIter = insert(cni->first);
      typename NodeToNodeMap::value_type val(cni->second, nodeIter->second);
      tmpmap.insert(val);
   }

   // add edges from aGraph.
   for (cni = aGraph.begin(); cni != aGraph.end(); ++cni)
   {
      typename std::map<const Node*, Node*>::iterator it1      = tmpmap.find(cni->second);
      Node*                                           newN1Ptr = it1->second;

      for (const_edge_iterator cei = cni->second->begin(); cei != cni->second->end(); ++cei)
      {
         const Edge* edgePtr = cei.mIter->second;
         const Node* n1Ptr   = (edgePtr->mN1Ptr == cni->second) ? edgePtr->mN1Ptr : edgePtr->mN2Ptr;
         const Node* n2Ptr   = (edgePtr->mN2Ptr != cni->second) ? edgePtr->mN2Ptr : edgePtr->mN1Ptr;

         // For undirected graphs, do a trick:
         // Compare Node pointers and only copy the edge going out of the smaller pointer.
         // Links from nodes to self are handled properly because they are added only once,
         // even though the graph is undirected. By contrast, an edge linking two different nodes
         // is added twice for an undirected graph.
         // For directed graphs we copy the edge directly.

         if (DIRECTED || (n1Ptr < n2Ptr))
         {
            //! find the new Node * corresponding to the old Node.
            typename std::map<const Node*, Node*>::iterator it2      = tmpmap.find(n2Ptr);
            Node*                                           newN2Ptr = it2->second;
            newN1Ptr->insert_edge(newN2Ptr->mNodeIter, *cei);
         }
      }
   }
}

#undef DO_DEBUG

#endif
