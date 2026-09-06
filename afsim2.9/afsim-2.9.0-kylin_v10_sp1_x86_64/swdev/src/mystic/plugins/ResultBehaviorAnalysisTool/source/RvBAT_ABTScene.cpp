// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvBAT_ABTScene.hpp"

RvBAT::ABTScene::ABTScene(const size_t aSceneId, const SceneType aSceneType, const BehaviorTree& aBehaviorTree, QObject* aParent)
   : Scene(aSceneId, aSceneType, aParent)
{
   // Go through each unique node and do some initialization
   for (size_t id : aBehaviorTree.mChildren.keys())
   {
      NodeType nType;
      GetNodeType(nType, aBehaviorTree.mChildren[id].nodeType());

      // Create the node with a default width and height
      auto node = new GraphicsNode(id, QRect(0, 0, 160, 100), nType);

      // Set the node's text
      node->SetNodeText(aBehaviorTree.mChildren[id].nodeName());

      // Set the node's description
      node->SetTooltipDesc(QString::fromStdString(aBehaviorTree.mChildren[id].nodeDesc()));

      // Set if the node is a tree node
      node->SetIsTreeNode(aBehaviorTree.mChildren[id].isRootNode());
      node->SetTreeId(aBehaviorTree.mChildren[id].treeId());

      // Set the node's child fsm id
      node->SetChildFSM_Id(aBehaviorTree.mChildren[id].childFsmId());

      // If this node is the root node, set the root node
      if (aBehaviorTree.mChildren[id].isRootNode() && aSceneId == aBehaviorTree.mChildren[id].treeId())
      {
         SetRootNode(node);
      }

      // Add the node to the view
      AddNode(id, node);
   }

   // After all nodes have been created, go back through and set up children.
   // We're using a second loop here rather than doing the childId stuff above
   // so that we confidently know all nodes exist already.  If we called
   // AddChildNode and referenced a childId that doesn't exist in mNodes yet
   // that could be problematic.
   for (size_t id : aBehaviorTree.mChildren.keys())
   {
      auto childrenIds = aBehaviorTree.mChildren.value(id).childrenIds();
      for (size_t childId : childrenIds)
      {
         mNodes[id]->AddChildNode(mNodes[childId]);
      }
   }

   // Set the starting Y for the nodes, then layout the tree.
   for (GraphicsNode* n : mNodes)
   {
      GraphicsNode::SetStartingY(n);
   }
   GraphicsNode::Layout(mRootNodePtr);

   // All of the positional data should be set by here, add all the nodes to the scene
   addItem(GetRootNode());
}

RvBAT::ABTScene::~ABTScene()
{
   // GraphicNodes are owned by the QGraphicsScene, it will delete them for us so don't try to double delete them
   mNodes.clear();
   mRootNodePtr = nullptr;
}

const QRectF RvBAT::ABTScene::GetSceneRect() const
{
   double x1 = INT_MAX;
   double x2 = INT_MIN;
   double y1 = INT_MAX;
   double y2 = INT_MIN;
   for (GraphicsNode* n : mNodes)
   {
      if (n->isVisible())
      {
         // If this is further left, set the new x1
         if (n->scenePos().x() < x1)
         {
            x1 = n->scenePos().x();
         }
         // If this is further right, set the new x2
         if ((n->scenePos().x() + n->GetWidth()) > x2)
         {
            x2 = n->scenePos().x() + n->GetWidth();
         }
         // If this is the top-most node, set the new y1
         if (n->scenePos().y() < y1)
         {
            y1 = n->scenePos().y();
         }
         // If this is the bottom-most node, set the new y2
         if ((n->scenePos().y() + n->GetHeight()) > y2)
         {
            y2 = n->scenePos().y() + n->GetHeight();
         }
      }
   }
   return QRectF(x1, y1, (x2 - x1), (y2 - y1));
}

void RvBAT::ABTScene::UpdateState(const rv::MsgBehaviorTreeState* aState)
{
   if (aState != nullptr)
   {
      // Go through the exec list and evaluate each node
      for (const rv::BehaviorTreeNodeExec& e : aState->execList())
      {
         EvaluateNode(e, mNodes[e.nodeId()]);
      }
   }
   else
   {
      for (auto& node : mNodes)
      {
         node->SetNumBlackboardVars(0);
         node->ClearBlackboardData();
         node->SetPreconditionTooltip("");
         node->SetExecuteTooltip("");
         node->SetNodeState(NodeState::cIDLE);
         if (node->GetNodeType() == NodeType::cTASK)
         {
            node->SetNodeImage("task");
         }
      }
   }
}

void RvBAT::ABTScene::UpdateState(const rv::MsgBehaviorTreeState*          aState,
                                  QVector<rv::BehaviorTreeBlackboardData>& aSharedBlackboardData)
{
   // aState is non-null if we make it here

   // Go through the exec list and evaluate each node
   for (const rv::BehaviorTreeNodeExec& e : aState->execList())
   {
      EvaluateNode(e, mNodes[e.nodeId()]);
   }

   // Fill in shared bb info for nodes that are trees.  Get this data from
   // other state messages
   for (auto& node : mNodes)
   {
      if (node->GetIsTreeNode())
      {
         QVector<rv::BehaviorTreeBlackboardData> tmpSharedBlackboardMsgs;
         for (rv::BehaviorTreeBlackboardData b : aState->blackboardList())
         {
            // If we have a sharedBlackboard name, this message is shared.
            if (!b.sharedBlackboard().empty())
            {
               tmpSharedBlackboardMsgs.push_back(b);
            }
         }
         node->SetBlackboardData(tmpSharedBlackboardMsgs);
      }
   }

   // Handle the blackboardList
   for (rv::BehaviorTreeBlackboardData b : aState->blackboardList())
   {
      // If we have a sharedBlackboard name, this message goes on the root node (for now).
      if (!b.sharedBlackboard().empty())
      {
         aSharedBlackboardData.push_back(b);
      }
      else // If we don't have a sharedBlackboard name, set blackboard text for a nodes
      {
         QVector<rv::BehaviorTreeBlackboardData> tmp;
         tmp.push_back(b);
         mNodes[b.nodeId()]->SetBlackboardData(tmp);
      }
   }
}

void RvBAT::ABTScene::UpdateNodeImage(GraphicsNode* aNode)
{
   switch (aNode->GetNodeType())
   {
   case NodeType::cSEQUENCE:
      aNode->SetNodeImage("sequence");
      break;
   case NodeType::cSELECTOR:
      aNode->SetNodeImage("selector");
      break;
   case NodeType::cPARALLEL:
      aNode->SetNodeImage("parallel");
      break;
   case NodeType::cTASK:
      switch (aNode->GetNodeState())
      {
      case NodeState::cSUCCESS:
         aNode->SetNodeImage("tasksuccess");
         break;
      case NodeState::cFAILURE:
         aNode->SetNodeImage("taskfailed");
         break;
      case NodeState::cHALTED:
         aNode->SetNodeImage("taskhalted");
         break;
      default:
         aNode->SetNodeImage("task");
         break;
      }
      break;
   default:
      break;
   }
}

void RvBAT::ABTScene::RedrawVisuals()
{
   // Update the nodes to match the current theme
   for (GraphicsNode* n : mNodes)
   {
      UpdateNodeImage(n);
      n->UpdateTextColor();
   }
}

void RvBAT::ABTScene::ResetLayout()
{
   // Layout the tree, and update the scene
   GraphicsNode::Layout(GetRootNode());
   update();
}

const bool RvBAT::ABTScene::IsObjectAtPoint(const QPointF& aPointF) const
{
   for (auto item : items(aPointF))
   {
      auto node = dynamic_cast<GraphicsNode*>(item);
      if (node != nullptr)
      {
         return true;
      }
   }
   return false;
}

void RvBAT::ABTScene::GetNodeType(RvBAT::NodeType& aNodeType, const std::string& aTypeStr)
{
   if (aTypeStr == "sequence")
   {
      aNodeType = NodeType::cSEQUENCE;
   }
   else if (aTypeStr == "sequence*")
   {
      aNodeType = NodeType::cSEQUENCE_WITH_MEMORY;
   }
   else if (aTypeStr == "selector")
   {
      aNodeType = NodeType::cSELECTOR;
   }
   else if (aTypeStr == "selector*")
   {
      aNodeType = NodeType::cSELECTOR_WITH_MEMORY;
   }
   else if (aTypeStr == "parallel")
   {
      aNodeType = NodeType::cPARALLEL;
   }
   else if (aTypeStr == "weighted_random")
   {
      aNodeType = NodeType::cWEIGHTED_RANDOM;
   }
   else if (aTypeStr == "priority_selector")
   {
      aNodeType = NodeType::cPRIORITY_SELECTOR;
   }
   else
   {
      aNodeType = NodeType::cTASK;
   }
}

void RvBAT::ABTScene::AddNode(size_t aId, GraphicsNode* aNode)
{
   mNodes[aId] = aNode;
}

void RvBAT::ABTScene::AddChildNode(GraphicsNode* aParent, GraphicsNode* aChild)
{
   aParent->AddChildNode(aChild);
   addItem(aChild);
}

void RvBAT::ABTScene::EvaluateNode(const rv::BehaviorTreeNodeExec& aExec, GraphicsNode* aNode)
{
   // Handle the node's state, setting tooltip text and image
   NodeState execState = (NodeState)((int)aExec.execState());

   // Set the node's tooltips
   aNode->SetPreconditionTooltip(QString::fromStdString(aExec.preconditionTooltip()));
   aNode->SetExecuteTooltip(QString::fromStdString(aExec.executeTooltip()));

   // Set the node's exec state
   if (execState == NodeState::cSUCCESS)
   {
      aNode->SetNodeState(NodeState::cSUCCESS);
      if (aNode->GetNodeType() == NodeType::cTASK)
      {
         aNode->SetNodeImage("tasksuccess");
      }
   }
   else if (execState == NodeState::cRUNNING)
   {
      aNode->SetNodeState(NodeState::cRUNNING);
      if (aNode->GetNodeType() == NodeType::cTASK)
      {
         aNode->SetNodeImage("taskrunning");
      }
   }
   else if (execState == NodeState::cIDLE)
   {
      aNode->SetNodeState(NodeState::cIDLE);
      if (aNode->GetNodeType() == NodeType::cTASK)
      {
         aNode->SetNodeImage("task");
      }
   }
   else if (execState == NodeState::cHALTED)
   {
      aNode->SetNodeState(NodeState::cHALTED);
      if (aNode->GetNodeType() == NodeType::cTASK)
      {
         aNode->SetNodeImage("taskhalted");
      }
   }
   else if (execState == NodeState::cDISABLED)
   {
      aNode->SetNodeState(NodeState::cDISABLED);
      if (aNode->GetNodeType() == NodeType::cTASK)
      {
         aNode->SetNodeImage("task");
      }
   }
   else
   {
      aNode->SetNodeState(NodeState::cFAILURE);
      if (aNode->GetNodeType() == NodeType::cTASK)
      {
         aNode->SetNodeImage("taskfailed");
      }
   }
}
