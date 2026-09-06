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

#ifndef RVBAT_ABTSCENE_HPP
#define RVBAT_ABTSCENE_HPP

#include "RvBAT_GraphicsNode.hpp"
#include "RvBAT_Interface.hpp"
#include "RvBAT_Scene.hpp"
#include "RvEventPipeClasses.hpp"

namespace RvBAT
{
class ABTScene : public Scene
{
   Q_OBJECT

public:
   explicit ABTScene(const size_t aSceneId, const SceneType aSceneType, const BehaviorTree& aBehaviorTree, QObject* aParent);
   ~ABTScene();

   /** Get the root node. */
   GraphicsNode* GetRootNode() const { return mRootNodePtr; }

   /** Set our root node.
    * @param aNode Node to set as the root node
    */
   void SetRootNode(GraphicsNode* aNode) { mRootNodePtr = aNode; }

   /** Get the rect representing the scene. */
   const QRectF GetSceneRect() const override;

   /** Redraw the visuals of the scene. */
   void RedrawVisuals() override;

   /** Reset the layout for the scene. */
   void ResetLayout() override;

   /** Update the node states using the appropriate state message.
    * @param aState state message to read from
    */
   void UpdateState(const rv::MsgBehaviorTreeState* aState);

   /** Update the node states using the appropriate state message.
    * @param aState state message to read from
    * @param aSharedBlackboardData blackboard data to read from
    */
   void UpdateState(const rv::MsgBehaviorTreeState* aState, QVector<rv::BehaviorTreeBlackboardData>& aSharedBlackboardData);

   /** Return whether or not an object is at a point and
    * return the point if so.
    * @param aPointF point object to populate
    */
   const bool IsObjectAtPoint(const QPointF& aPointF) const override;

private:
   /** Set the nodes types from it's string value.
    * @param aNodeType node type to fill
    * @param aTypeStr type str to read from
    */
   void GetNodeType(NodeType& aNodeType, const std::string& aTypeStr);

   /** Add a node to the node map.
    * @param aId Id of the node to add
    * @param aNode GraphicsNode object to add
    */
   void AddNode(size_t aId, GraphicsNode* aNode);

   /** Evaluate a node to set the correct color and image.
    * @param aExec Execute event to get eval values from
    * @param aNode GraphicsNode object to evaluate
    */
   void EvaluateNode(const rv::BehaviorTreeNodeExec& aExec, GraphicsNode* aNode);

   /** Add a child node to another node.
    * @param aParent The Parent of the node to add
    * @param aChild The node to add
    */
   void AddChildNode(GraphicsNode* aParent, GraphicsNode* aChild);

   /** Update a node's image.  This occurs when the theme changes.
    * @param aNode node to update image for
    */
   void UpdateNodeImage(GraphicsNode* aNode);

   GraphicsNode*               mRootNodePtr{nullptr}; ///< Root node of the behavior trees
   QMap<size_t, GraphicsNode*> mNodes;                ///< Map of ids to GraphicsNodes
};
} // namespace RvBAT
#endif // RVBAT_ABTSCENE_HPP
