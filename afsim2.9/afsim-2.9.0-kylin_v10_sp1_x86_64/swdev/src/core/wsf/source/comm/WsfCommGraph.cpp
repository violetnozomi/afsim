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

#include "WsfCommGraph.hpp"

#include "UtMemory.hpp"
#include "script/WsfScriptCommAddress.hpp"

namespace wsf
{
namespace comm
{
namespace graph
{

// =================================================================================================
// CommNodeNew
// =================================================================================================
Node::Node(const Address& aAddress)
   : mAddress(aAddress)
   , mEnabled(true)
{
}

// =================================================================================================
bool Node::operator==(const Node& aRhs) const
{
   return (mAddress == aRhs.mAddress);
}

// =================================================================================================
bool Node::operator!=(const Node& aRhs) const
{
   return (mAddress != aRhs.mAddress);
}

// =================================================================================================
bool Node::operator<(const Node& aRhs) const
{
   return (mAddress < aRhs.mAddress);
}

// =================================================================================================
Node* Node::Clone() const
{
   return new Node(*this);
}

// =================================================================================================
ScriptCommGraphNodeClass::ScriptCommGraphNodeClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCommGraphNode");

   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<GetAddress>());
   AddMethod(ut::make_unique<IsEnabled>());
}

// =================================================================================================
void ScriptCommGraphNodeClass::Destroy(void* aNodePtr)
{
   delete static_cast<Node*>(aNodePtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphNodeClass, Node, GetAddress, 0, "WsfAddress", "")
{
   aReturnVal.SetPointer(WsfScriptCommAddressClass::Create(aObjectPtr->GetAddress()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphNodeClass, Node, IsEnabled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsEnabled());
}

// =================================================================================================
// CommEdgeNew
// =================================================================================================
Edge::Edge(bool aEnabled, const Address& aSourceAddress, const Address& aDestinationAddress)
   : mSourceAddress(aSourceAddress)
   , mDestinationAddress(aDestinationAddress)
   , mWeight(1.0)
   , mEnabled(aEnabled)
   , mStatic(false)
{
}

// =================================================================================================
Edge* Edge::Clone() const
{
   return new Edge(*this);
}

// =================================================================================================
ScriptCommGraphEdgeClass::ScriptCommGraphEdgeClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCommGraphEdge");

   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<SourceAddress>());
   AddMethod(ut::make_unique<DestinationAddress>());
   AddMethod(ut::make_unique<Weight>());
   AddMethod(ut::make_unique<IsEnabled>());
}

// =================================================================================================
void ScriptCommGraphEdgeClass::Destroy(void* aEdgePtr)
{
   delete static_cast<Edge*>(aEdgePtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphEdgeClass, Edge, SourceAddress, 0, "WsfAddress", "")
{
   aReturnVal.SetPointer(WsfScriptCommAddressClass::Create(aObjectPtr->GetSourceAddress()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphEdgeClass, Edge, DestinationAddress, 0, "WsfAddress", "")
{
   aReturnVal.SetPointer(WsfScriptCommAddressClass::Create(aObjectPtr->GetDestinationAddress()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphEdgeClass, Edge, Weight, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetWeight());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphEdgeClass, Edge, IsEnabled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsEnabled());
}

// =================================================================================================
Node* Graph::FindNode(const Address& aAddress) const
{
   auto  it      = mGraph.find(Node(aAddress));
   Node* nodePtr = nullptr;
   if (it != mGraph.end())
   {
      nodePtr = const_cast<Node*>(&(it->first));
   }
   return nodePtr;
}

// =================================================================================================
const Node* Graph::CreateNode(const Address& aAddress)
{
   auto it = mGraph.insert(Node(aAddress));
   return &(it->first);
}

// =================================================================================================
bool Graph::RemoveNode(const Node* aNodePtr)
{
   if (aNodePtr)
   {
      mGraph.erase(*aNodePtr);
      return true;
   }
   return false;
}

// =================================================================================================
bool Graph::RemoveNode(const Address& aAddress)
{
   auto node = FindNode(aAddress);
   return RemoveNode(node);
}

// =================================================================================================
Edge* Graph::InsertEdge(const Node* aSourceNode, const Node* aDestinationNode, bool aIsEnabled, bool aIsStatic /* = false*/)
{
   if (aSourceNode && aDestinationNode)
   {
      auto srcIt  = mGraph.find(*aSourceNode);
      auto destIt = mGraph.find(*aDestinationNode);
      auto edgeIt =
         mGraph.insert_edge(srcIt, destIt, Edge(aIsEnabled, aSourceNode->GetAddress(), aDestinationNode->GetAddress()));
      edgeIt->SetStatic(aIsStatic);
      return &(*edgeIt);
   }
   return nullptr;
}

// =================================================================================================
Edge* Graph::InsertEdge(const Address& aSourceAddress,
                        const Address& aDestinationAddress,
                        bool           aIsEnabled,
                        bool           aIsStatic /* = false*/)
{
   auto nodeSrc = FindNode(aSourceAddress);
   auto nodeDst = FindNode(aDestinationAddress);
   return InsertEdge(nodeSrc, nodeDst, aIsEnabled, aIsStatic);
}

// =================================================================================================
bool Graph::EraseEdge(const Node* aSourceNode, const Node* aDestinationNode)
{
   if (aSourceNode && aDestinationNode)
   {
      auto srcIt  = mGraph.find(*aSourceNode);
      auto destIt = mGraph.find(*aDestinationNode);
      if (srcIt == mGraph.end() || destIt == mGraph.end())
      {
         return false;
      }
      mGraph.erase_edge(srcIt, destIt);
      return true;
   }
   return false;
}

// =================================================================================================
bool Graph::EraseEdge(const Address& aSourceAddress, const Address& aDestinationAddress)
{
   auto nodeSrc = FindNode(aSourceAddress);
   auto nodeDst = FindNode(aDestinationAddress);
   return EraseEdge(nodeSrc, nodeDst);
}

// =================================================================================================
Edge* Graph::FindEdge(const Node* aSourceNode, const Node* aDestinationNode)
{
   if (aSourceNode && aDestinationNode)
   {
      auto srcIt  = mGraph.find(*aSourceNode);
      auto destIt = mGraph.find(*aDestinationNode);
      auto edgeIt = srcIt->second->find_edge(destIt);
      if (edgeIt == srcIt->second->end())
      {
         return nullptr;
      }
      return &(*edgeIt);
   }
   return nullptr;
}

// =================================================================================================
Edge* Graph::FindEdge(const Address& aSourceAddress, const Address& aDestinationAddress)
{
   auto nodeSrc = FindNode(aSourceAddress);
   auto nodeDst = FindNode(aDestinationAddress);
   return FindEdge(nodeSrc, nodeDst);
}

EdgeList Graph::GetIncomingNodeEdges(const Node* aNodePtr)
{
   EdgeList edgeList;
   if (aNodePtr)
   {
      auto it = mGraph.find(*aNodePtr);
      if (it == mGraph.end())
      {
         return edgeList;
      }

      edgeList.reserve(static_cast<size_t>(it->second->count_edges_rev()));
      for (auto edgeIt = it->second->begin_rev(); edgeIt != it->second->end_rev(); ++edgeIt)
      {
         edgeList.push_back(&(*edgeIt));
      }
   }
   return edgeList;
}

EdgeList Graph::GetIncomingNodeEdges(const Address& aAddress)
{
   auto nodePtr = FindNode(aAddress);
   return GetIncomingNodeEdges(nodePtr);
}

// =================================================================================================
EdgeList Graph::GetOutgoingNodeEdges(const Node* aNodePtr) const
{
   EdgeList edgeList;
   if (aNodePtr)
   {
      auto it = mGraph.find(*aNodePtr);
      if (it == mGraph.end())
      {
         return edgeList;
      }

      edgeList.reserve(static_cast<size_t>(it->second->count_edges()));
      for (auto& edgeIt : *it->second)
      {
         edgeList.push_back(&edgeIt);
      }
   }
   return edgeList;
}

// =================================================================================================
EdgeList Graph::GetOutgoingNodeEdges(const Address& aAddress) const
{
   auto nodePtr = FindNode(aAddress);
   return GetOutgoingNodeEdges(nodePtr);
}

// =================================================================================================
EdgeList Graph::GetAllNodeEdges(const Node* aNodePtr)
{
   EdgeList edgeList;
   if (aNodePtr)
   {
      // outgoing
      auto it = mGraph.find(Node(aNodePtr->GetAddress()));
      edgeList.reserve(static_cast<size_t>(it->second->count_edges() + it->second->count_edges_rev()));
      for (auto& edgeIt : *it->second)
      {
         edgeList.push_back(&edgeIt);
      }

      // incoming
      for (auto edgeIt = it->second->begin_rev(); edgeIt != it->second->end_rev(); ++edgeIt)
      {
         edgeList.push_back(&(*edgeIt));
      }
   }
   return edgeList;
}

// =================================================================================================
EdgeList Graph::GetAllNodeEdges(const Address& aAddress)
{
   auto nodePtr = FindNode(aAddress);
   return GetAllNodeEdges(nodePtr);
}

// =================================================================================================
void Graph::Clear()
{
   mGraph.clear();
}

// =================================================================================================
bool Graph::FindPath(const Node*                 aSourceNode,
                     const Node*                 aDestinationNode,
                     GraphImpl::NodeList&        aPath,
                     double&                     aCost,
                     const GraphImpl::cost_func* aCostClass) const // = nullptr
{
   auto sourceIt      = mGraph.find(*aSourceNode);
   auto destinationIt = mGraph.find(*aDestinationNode);
   return mGraph.shortest_path(sourceIt, destinationIt, aPath, aCost, aCostClass);
}

// =================================================================================================
bool Graph::FindPath(const Address&              aSourceAddress,
                     const Address&              aDestinationAddress,
                     AddressList&                aAddressList,
                     double&                     aCost,
                     const GraphImpl::cost_func* aCostClass) const // = nullptr
{
   auto                sourceIt      = mGraph.find(aSourceAddress);
   auto                destinationIt = mGraph.find(aDestinationAddress);
   GraphImpl::NodeList path;
   aAddressList.clear();

   if (sourceIt == mGraph.end() || destinationIt == mGraph.end())
   {
      aCost = std::numeric_limits<double>::max();
      return false;
   }

   bool pathExists = mGraph.shortest_path(sourceIt, destinationIt, path, aCost, aCostClass);
   if (pathExists)
   {
      aAddressList.reserve(path.size());
      for (auto& it : path)
      {
         aAddressList.push_back(it.GetAddress());
      }
   }
   return pathExists;
}

bool Graph::FindAnyPath(const Address& aSourceAddress,
                        const Address& aDestinationAddress,
                        AddressList*   aAddressListPtr, // nullptr if we don't need to know which path is used
                        double&        aCost,
                        const GraphImpl::cost_func* aCostClass /* = nullptr*/) const
{
   GraphImpl::cost_func defaultCost;
   if (aCostClass == nullptr)
   {
      aCostClass = &defaultCost;
   }
   auto sourceIt      = mGraph.find(aSourceAddress);
   auto destinationIt = mGraph.find(aDestinationAddress);

   GraphImpl::NodeList path;
   bool                pathExists = mGraph.find_path(sourceIt,
                                      destinationIt,
                                      aCost,
                                      false, // don't have to find optimal path
                                      aAddressListPtr ? &path : nullptr,
                                      *aCostClass, // edge weight
                                      *aCostClass, // heuristic
                                      [aCostClass](const Node& n) { return aCostClass->consider_node(n); });
   if (aAddressListPtr)
   {
      aAddressListPtr->clear();
      if (pathExists)
      {
         aAddressListPtr->reserve(path.size());
         for (auto& it : path)
         {
            aAddressListPtr->push_back(it.GetAddress());
         }
      }
   }
   return pathExists;
}

// =================================================================================================
//! Removes all incoming and outgoing edges to this node.
void Graph::RemoveNodeEdges(Node* aNode)
{
   auto edges = GetAllNodeEdges(aNode);
   for (auto& edge : edges)
   {
      EraseEdge(aNode->GetAddress(), edge->GetDestinationAddress());
   }
}

// =================================================================================================
std::vector<const Node*> Graph::GetNodes() const
{
   std::vector<const Node*> nodes;
   nodes.reserve(mGraph.count_nodes());
   for (const auto& nodeIt : mGraph)
   {
      nodes.push_back(&nodeIt.first);
   }

   return nodes;
}

// =================================================================================================
ScriptCommGraphClass::ScriptCommGraphClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCommGraph");

   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<HasNode>());
   AddMethod(ut::make_unique<HasEdge>());
   AddMethod(ut::make_unique<GetNode>());
   AddMethod(ut::make_unique<GetNodes>());
   AddMethod(ut::make_unique<GetEdge>());
   AddMethod(ut::make_unique<GetIncomingNodeEdges>());
   AddMethod(ut::make_unique<GetOutgoingNodeEdges>());
   AddMethod(ut::make_unique<FindPath>());

   AddMethod(ut::make_unique<CreateNode>());
   AddMethod(ut::make_unique<RemoveNode>());
   AddMethod(ut::make_unique<InsertEdge>());
   AddMethod(ut::make_unique<EraseEdge>());
   AddMethod(ut::make_unique<Clear>());

   AddMethod(ut::make_unique<SetEdgeWeight>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, HasNode, 1, "bool", "WsfAddress")
{
   auto address = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   aReturnVal.SetBool(aObjectPtr->FindNode(*address) != nullptr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, HasEdge, 2, "bool", "WsfAddress, WsfAddress")
{
   auto sourceAddress      = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   auto destinationAddress = aVarArgs[1].GetPointer()->GetAppObject<Address>();
   aReturnVal.SetBool(aObjectPtr->FindEdge(*sourceAddress, *destinationAddress) != nullptr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, GetNode, 1, "WsfCommGraphNode", "WsfAddress")
{
   auto address      = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   auto checkNodePtr = aObjectPtr->FindNode(*address);
   if (checkNodePtr)
   {
      auto nodePtr = ut::clone(checkNodePtr);
      aReturnVal.SetPointer(new UtScriptRef(nodePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, GetNodes, 0, "Array<WsfAddress>", "")
{
   auto nodes = aObjectPtr->GetNodes();

   auto data = ut::make_unique<std::vector<UtScriptData>>();
   for (auto nodePtr : nodes)
   {
      data->emplace_back(WsfScriptCommAddressClass::Create(nodePtr->GetAddress()));
   }

   aReturnVal.SetPointer(new UtScriptRef(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, GetEdge, 2, "WsfCommGraphEdge", "WsfAddress, WsfAddress")
{
   auto sourceAddress      = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   auto destinationAddress = aVarArgs[1].GetPointer()->GetAppObject<Address>();
   auto checkEdgePtr       = aObjectPtr->FindEdge(*sourceAddress, *destinationAddress);
   if (checkEdgePtr)
   {
      auto edgePtr = ut::clone(checkEdgePtr);
      aReturnVal.SetPointer(new UtScriptRef(edgePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, GetIncomingNodeEdges, 1, "Array<WsfCommGraphEdge>", "WsfAddress")
{
   auto           address      = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   auto           edgeList     = aObjectPtr->GetIncomingNodeEdges(*address);
   UtScriptClass* edgeClassPtr = aContext.GetTypes()->GetClass("WsfCommGraphEdge");

   auto data = ut::make_unique<std::vector<UtScriptData>>();
   for (auto edgePtr : edgeList)
   {
      data->emplace_back(new UtScriptRef(edgePtr->Clone(), edgeClassPtr, UtScriptRef::cMANAGE));
   }

   aReturnVal.SetPointer(new UtScriptRef(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, GetOutgoingNodeEdges, 1, "Array<WsfCommGraphEdge>", "WsfAddress")
{
   auto           address      = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   auto           edgeList     = aObjectPtr->GetOutgoingNodeEdges(*address);
   UtScriptClass* edgeClassPtr = aContext.GetTypes()->GetClass("WsfCommGraphEdge");

   auto data = ut::make_unique<std::vector<UtScriptData>>();
   for (auto edgePtr : edgeList)
   {
      data->emplace_back(new UtScriptRef(edgePtr->Clone(), edgeClassPtr, UtScriptRef::cMANAGE));
   }

   aReturnVal.SetPointer(new UtScriptRef(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, FindPath, 2, "Array<WsfAddress>", "WsfAddress, WsfAddress")
{
   auto sourceAddress      = aVarArgs[0].GetPointer()->GetAppObject<Address>();
   auto destinationAddress = aVarArgs[1].GetPointer()->GetAppObject<Address>();

   AddressList path;
   double      cost = 0.0;
   aObjectPtr->FindPath(*sourceAddress, *destinationAddress, path, cost);

   auto data = ut::make_unique<std::vector<UtScriptData>>();
   for (const auto& address : path)
   {
      data->emplace_back(WsfScriptCommAddressClass::Create(address));
   }

   aReturnVal.SetPointer(new UtScriptRef(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, CreateNode, 1, "bool", "WsfAddress")
{
   bool created = false;

   if (aObjectPtr->IsUserModifiable())
   {
      auto address = aVarArgs[0].GetPointer()->GetAppObject<Address>();
      if (!aObjectPtr->FindNode(*address))
      {
         if (aObjectPtr->CreateNode(*address))
         {
            created = true;
         }
      }
   }

   aReturnVal.SetBool(created);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, RemoveNode, 1, "bool", "WsfAddress")
{
   bool removed = false;

   if (aObjectPtr->IsUserModifiable())
   {
      auto address = aVarArgs[0].GetPointer()->GetAppObject<Address>();
      if (aObjectPtr->FindNode(*address))
      {
         if (aObjectPtr->RemoveNode(*address))
         {
            removed = true;
         }
      }
   }

   aReturnVal.SetBool(removed);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, InsertEdge, 2, "bool", "WsfAddress, WsfAddress")
{
   bool inserted = false;

   if (aObjectPtr->IsUserModifiable())
   {
      auto sourceAddress      = aVarArgs[0].GetPointer()->GetAppObject<Address>();
      auto destinationAddress = aVarArgs[1].GetPointer()->GetAppObject<Address>();

      if (!aObjectPtr->FindEdge(*sourceAddress, *destinationAddress))
      {
         if (aObjectPtr->InsertEdge(*sourceAddress, *destinationAddress, true))
         {
            inserted = true;
         }
      }
   }

   aReturnVal.SetBool(inserted);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, EraseEdge, 2, "bool", "WsfAddress, WsfAddress")
{
   bool erased = false;

   if (aObjectPtr->IsUserModifiable())
   {
      auto sourceAddress      = aVarArgs[0].GetPointer()->GetAppObject<Address>();
      auto destinationAddress = aVarArgs[1].GetPointer()->GetAppObject<Address>();

      if (aObjectPtr->FindEdge(*sourceAddress, *destinationAddress))
      {
         if (aObjectPtr->EraseEdge(*sourceAddress, *destinationAddress))
         {
            erased = true;
         }
      }
   }

   aReturnVal.SetBool(erased);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, Clear, 0, "bool", "")
{
   bool cleared = false;

   if (aObjectPtr->IsUserModifiable())
   {
      aObjectPtr->Clear();
      cleared = true;
   }

   aReturnVal.SetBool(cleared);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptCommGraphClass, Graph, SetEdgeWeight, 3, "bool", "WsfAddress, WsfAddress, double")
{
   bool weightChanged = false;

   if (aObjectPtr->IsUserModifiable())
   {
      auto sourceAddress      = aVarArgs[0].GetPointer()->GetAppObject<Address>();
      auto destinationAddress = aVarArgs[1].GetPointer()->GetAppObject<Address>();
      auto newWeight          = aVarArgs[2].GetDouble();

      auto edgePtr = aObjectPtr->FindEdge(*sourceAddress, *destinationAddress);

      if (edgePtr)
      {
         edgePtr->SetWeight(newWeight);
         weightChanged = true;
      }
   }

   aReturnVal.SetBool(weightChanged);
}

} // namespace graph
} // namespace comm
} // namespace wsf
