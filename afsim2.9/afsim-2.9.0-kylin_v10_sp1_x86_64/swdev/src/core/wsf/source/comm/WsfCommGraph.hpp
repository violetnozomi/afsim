// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMGRAPH_HPP
#define WSFCOMMGRAPH_HPP

#include "wsf_export.h"

#include <map>

#include "UtGraph.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfCommAddress.hpp"

namespace wsf
{
namespace comm
{
namespace graph
{

//! Declare templated version of UtGraph for use with all
//! Graph classes. Forward declare needed classes.
class Node;
class Edge;
using GraphImpl   = UtGraphT<Node, Edge, true, double>;
using AddressList = std::vector<Address>;
using EdgeList    = std::vector<Edge*>;

//! @name Node Class
//! Node represents a single comm device within the graph representation of
//! networks. All Wsf::Comm objects are represented this way, where in previous
//! implementations of WsfComm, only local networks were represented as nodes, and
//! there did not exist any concept of self-deterministic routing capability on a
//! comm device unless it was a gateway. Only addresses are stored within the nodes,
//! and these can be mapped back to their actual object association via the
//! network manager class, if so needed. Nodes may also be set to be disabled via
//! current conditions within the simulation, such as electronic warfare, cyber, or
//! other effects that disable but do not remove a node from the graph.
class WSF_EXPORT Node : public UtScriptAccessible
{
public:
   Node(const Address& aAddress);
   Node(const Node& aSrc) = default;
   virtual ~Node()        = default;

   virtual Node* Clone() const;

   bool  operator==(const Node& aRhs) const;
   bool  operator!=(const Node& aRhs) const;
   bool  operator<(const Node& aRhs) const;
   Node& operator=(const Node& aRhs) = default;

   const Address& GetAddress() const { return mAddress; }

   void SetEnabled() { mEnabled = true; }
   void SetDisabled() { mEnabled = false; }
   bool IsEnabled() const { return mEnabled; }

   const char* GetScriptClassName() const override { return "WsfCommGraphNode"; }

private:
   Address mAddress;
   bool    mEnabled;
};

class WSF_EXPORT ScriptCommGraphNodeClass : public UtScriptClass
{
public:
   ScriptCommGraphNodeClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void Destroy(void* aNodePtr) override;

   UT_DECLARE_SCRIPT_METHOD(GetAddress);
   UT_DECLARE_SCRIPT_METHOD(IsEnabled);
};

//! @name Edge Class
//! An edge represents the capability of two nodes to communicate with each other. The
//! existence of an edge assumes computability between nodes in terms of communications
//! protocols, language, physical restrictions (in the case of RF), etc, and will not perform
//! checks of such at this level. Edges are directed, as nodes may not be capable of two-way
//! communications. Edges are not currently weighted, as it is up to the cost function class
//! to determine the weight of the edge due to various methods of determining the weight of an
//! edge (least jumps, shortest transmission time, line of sight issues in RF, etc.) Edges
//! may be disabled due to current conditions in the simulation, such as damage, line of sight,
//! excessive distance, etc.
class WSF_EXPORT Edge : public UtScriptAccessible
{
public:
   Edge(bool aEnabled, const Address& aSourceAddress, const Address& aDestinationAddress);
   Edge(const Edge& aSrc) = default;
   virtual ~Edge()        = default;
   Edge& operator=(const Edge& aRhs) = default;

   virtual Edge* Clone() const;

   const Address& GetSourceAddress() const { return mSourceAddress; }
   const Address& GetDestinationAddress() const { return mDestinationAddress; }
   double         GetWeight() const { return mWeight; }
   void           SetEnabled() { mEnabled = true; }
   void           SetDisabled() { mEnabled = false; }
   void           SetStatic(bool aIsStatic) { mStatic = aIsStatic; }
   void           SetWeight(double aWeight) { mWeight = aWeight; }
   bool           IsEnabled() const { return mEnabled; }
   bool           IsStatic() const { return mStatic; }

   const char* GetScriptClassName() const override { return "WsfCommGraphEdge"; }

private:
   Address mSourceAddress;
   Address mDestinationAddress;
   double  mWeight;
   bool    mEnabled;
   bool    mStatic;
};

class WSF_EXPORT ScriptCommGraphEdgeClass : public UtScriptClass
{
public:
   ScriptCommGraphEdgeClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void Destroy(void* aEdgePtr) override;

   UT_DECLARE_SCRIPT_METHOD(SourceAddress);
   UT_DECLARE_SCRIPT_METHOD(DestinationAddress);
   UT_DECLARE_SCRIPT_METHOD(Weight);
   UT_DECLARE_SCRIPT_METHOD(IsEnabled);
};

//! @name Graph Class
//! The graph class is a container for a UtGraph template object and associated methods
//! for insertion, removal, and general manipulation of the graph via its nodes and edges.
//! The network manager contains an instance of this class representing the existence and
//! paths of communications between comm objects for subsequent updates and query during
//! normal simulation operations.
//! In addition, the graph contains a simple map container and method calls to support
//! logging of activities taken on the graph. This is useful when comparing graph objects
//! in a simulation state without traversing possibly large graphs to reconcile differences.
class WSF_EXPORT Graph : public UtScriptAccessible
{
public:
   enum class GraphAction
   {
      cADD_NODE,
      cADD_EDGE,
      cREMOVE_NODE,
      cREMOVE_EDGE
   };

   Graph()                  = default;
   Graph(const Graph& aSrc) = default;
   virtual ~Graph()         = default;
   Graph& operator=(const Graph& aRhs) = default;

   virtual Node* FindNode(const Address& aAddress) const;

   virtual const Node* CreateNode(const Address& aAddress);

   virtual bool RemoveNode(const Node* aNodePtr);
   virtual bool RemoveNode(const Address& aAddress);

   virtual Edge* InsertEdge(const Node* aSourceNode, const Node* aDestinationNode, bool aIsEnabled, bool aIsStatic = false);
   virtual Edge* InsertEdge(const Address& aSourceAddress,
                            const Address& aDestinationAddress,
                            bool           aIsEnabled,
                            bool           aIsStatic = false);

   virtual bool EraseEdge(const Node* aSourceNode, const Node* aDestinationNode);
   virtual bool EraseEdge(const Address& aSourceAddress, const Address& aDestinationAddress);

   virtual Edge* FindEdge(const Node* aSourceNode, const Node* aDestinationNode);
   virtual Edge* FindEdge(const Address& aSourceAddress, const Address& aDestinationAddress);

   std::vector<const Node*> GetNodes() const;

   virtual EdgeList GetIncomingNodeEdges(const Node* aNodePtr);
   virtual EdgeList GetIncomingNodeEdges(const Address& aAddress);
   virtual EdgeList GetOutgoingNodeEdges(const Node* aNodePtr) const;
   virtual EdgeList GetOutgoingNodeEdges(const Address& aAddress) const;

   virtual EdgeList GetAllNodeEdges(const Node* aNodePtr);
   virtual EdgeList GetAllNodeEdges(const Address& aAddress);

   virtual void RemoveNodeEdges(Node* aNode);

   void SetUserModifiable(bool aCanModify) { mUserModifiable = aCanModify; }
   bool IsUserModifiable() const { return mUserModifiable; }


   //! @name FindPath Method
   //! This method finds the correct path from a source node to a
   //! destination node if possible (return false on failure). The
   //! path to the destination and the cost is provided. The user
   //! may provide a derived GraphCost class object that will
   //! provide correct node consideration and edge weighting based
   //! on the defined GraphCost method implementations. If GraphCost
   //! is not provided, the default algorithm considers all nodes and
   //! edges regardless of state, and finds the path using the least
   //! amount of hops to the destination.
   //@{
   bool FindPath(const Node*                 aSourceNode,
                 const Node*                 aDestinationNode,
                 GraphImpl::NodeList&        aPath,
                 double&                     aCost,
                 const GraphImpl::cost_func* aCostClass = nullptr) const;
   bool FindPath(const Address&              aSourceAddress,
                 const Address&              aDestinationAddress,
                 AddressList&                aAddressList,
                 double&                     aCost,
                 const GraphImpl::cost_func* aCostClass = nullptr) const;

   //! Finds any path if one exists, with no guarantee that it will be the optimal shortest path
   bool FindAnyPath(const Address&              aSourceAddress,
                    const Address&              aDestinationAddress,
                    AddressList*                aAddressList, // nullptr if we don't need to know which path is used
                    double&                     aCost,
                    const GraphImpl::cost_func* aCostClass = nullptr) const;
   //@}

   void        Clear();
   const char* GetScriptClassName() const override { return "WsfCommGraph"; }

private:
   //! Indicates if this graph should provide access to script calls that
   //! can modify the graph state.
   bool mUserModifiable{false};

   GraphImpl mGraph;
};

class WSF_EXPORT ScriptCommGraphClass : public UtScriptClass
{
public:
   ScriptCommGraphClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(HasNode);
   UT_DECLARE_SCRIPT_METHOD(HasEdge);
   UT_DECLARE_SCRIPT_METHOD(GetNode);
   UT_DECLARE_SCRIPT_METHOD(GetNodes);
   UT_DECLARE_SCRIPT_METHOD(GetEdge);
   UT_DECLARE_SCRIPT_METHOD(GetIncomingNodeEdges);
   UT_DECLARE_SCRIPT_METHOD(GetOutgoingNodeEdges);
   UT_DECLARE_SCRIPT_METHOD(FindPath);

   //! The following script calls are only available
   //! for use cases where the graph has been indicated
   //! to allow user changes to state.
   UT_DECLARE_SCRIPT_METHOD(CreateNode);
   UT_DECLARE_SCRIPT_METHOD(RemoveNode);
   UT_DECLARE_SCRIPT_METHOD(InsertEdge);
   UT_DECLARE_SCRIPT_METHOD(EraseEdge);
   UT_DECLARE_SCRIPT_METHOD(Clear);

   //! This method, although really belonging to the edge object,
   //! is provided via the graph to restrict modification.
   UT_DECLARE_SCRIPT_METHOD(SetEdgeWeight);
};

} // namespace graph
} // namespace comm
} // namespace wsf

#endif
