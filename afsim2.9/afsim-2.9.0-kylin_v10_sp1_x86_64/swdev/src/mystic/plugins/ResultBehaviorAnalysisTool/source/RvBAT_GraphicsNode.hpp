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

#ifndef RVBAT_GRAPHICSNODE_HPP
#define RVBAT_GRAPHICSNODE_HPP

#include <QGraphicsItem>
#include <QPainter>

#include "RvEventPipeClasses.hpp"

using BlackboardData = rv::BehaviorTreeBlackboardData;

namespace RvBAT
{
/** The return status of a node at a given time step (tick).
 * SUCCESS - The node has completed running.
 * FAILURE - The node has determined it will not be able to complete its task.
 * RUNNING - The node has successfully moved forward during this tick, has not finished yet.
 * IDLE - The node has not run yet.
 * HALTED - The node has been halted by its parent.
 */
enum class NodeState
{
   cRUNNING  = 1,
   cSUCCESS  = 2,
   cFAILURE  = 3,
   cIDLE     = 4,
   cHALTED   = 5,
   cDISABLED = 6
};

/** Type of the Node. */
enum class NodeType
{
   cNONE,
   cTASK,
   cSELECTOR,
   cSELECTOR_WITH_MEMORY,
   cSEQUENCE,
   cSEQUENCE_WITH_MEMORY,
   cPARALLEL,
   cWEIGHTED_RANDOM,
   cPRIORITY_SELECTOR,
};

/** Struct for Tidy Tree algorithm. */
class LowestYIndex
{
public:
   LowestYIndex(double aLowY, int aIndex, LowestYIndex* aNext);
   double        mLowY;
   int           mIndex;
   LowestYIndex* mNext;
};

class GraphicsNode : public QGraphicsItem
{
public:
   GraphicsNode(size_t aId, QRectF aRect, NodeType aNodeType, bool aHasRun = false, bool aSuccess = false);
   ~GraphicsNode();

   /** Add a child node to this node.
    * @param aNode Node to child.
    */
   void AddChildNode(GraphicsNode* aNode);

   /** Get the node's state. */
   const NodeState GetNodeState() const { return mNodeState; }

   /** Set the node's state.
    * @param aState state to set the node to
    */
   void SetNodeState(const NodeState aState);

   /** Get the node's type. */
   const NodeType GetNodeType() const { return mNodeType; }

   /** Set the node's tooltip description.
    * @param aTooltipDesc description to set in the tooltip.
    */
   void SetTooltipDesc(const QString& aTooltipDesc);

   /** Set the nodes precondition tooltip.
    * @param aTooltipText text to set in the tooltip.
    */
   void SetPreconditionTooltip(const QString& aTooltipText) { mPreconditionTooltip = aTooltipText; }

   /** Set the nodes execute tooltip.
    * @param aTooltipText text to set in the tooltip.
    */
   void SetExecuteTooltip(const QString& aTooltipText) { mExecuteTooltip = aTooltipText; }

   /** Update the node's text color according to the current theme. */
   void UpdateTextColor();

   /** Get the node's text. */
   const QGraphicsTextItem* GetNodeText() const { return mNodeText; }

   /** Set the text that will be displayed on the node.
    * @param aText Text to set
    */
   void SetNodeText(const std::string& aText);

   /** Update the node's image (load new image if state changes).
    * @param aImageName Name of image file
    */
   void SetNodeImage(const std::string& aImageName);

   /** Return the width of this node. */
   const int GetWidth() const { return mWidth; }

   /** Return the height of this node. */
   const int GetHeight() const { return mHeight; }

   /** Return a pointer to the parent of this node */
   const GraphicsNode* GetParent() const { return mParent; }

   /** Sets the starting Y coordinate for a node.
    * @param aNode node to set Y for.
    */
   static void SetStartingY(GraphicsNode* aNode);

   /** Layout the tree, using the modified Reingold-Tilford algorithm.
    * @param aNode tree(root node) to operate on.
    */
   static void Layout(GraphicsNode* aNode);

   /** get the blackboard data for this node. */
   const QVector<BlackboardData>& GetBlackboardData() { return mBlackboardData; }

   /** Set the blackboard data for this node.
    * @param aData blackboard data to use.
    */
   void SetBlackboardData(const QVector<BlackboardData>& aData);

   /** Clear the blackboard data for this node. */
   void ClearBlackboardData();

   /** Toggle whether this node is selected or not. */
   bool ToggleSelected()
   {
      mSelected = !mSelected;
      return mSelected;
   }

   /** Return whether or not this node has children. */
   bool HasChildren() const { return !mChildren.empty(); }

   /** Return whether this nodes children are being hidden or not. */
   bool AreChildrenVisible() const { return mChildrenVisible; }

   /** Toggle children visibility. */
   void ToggleChildrenVisible();

   /** Set if this node is a tree node.
    * @param aValue value to set.
    */
   void SetIsTreeNode(const bool aValue) { mIsTree = aValue; }

   /** Return whether or not this node is a tree node. */
   bool GetIsTreeNode() const { return mIsTree; }

   /** Get the tree id for this node. */
   const size_t GetTreeId() const { return mTreeId; }

   /** Return the id of the child FSM for this state. */
   const size_t GetChildFSM_Id() const { return mChildFSM_Id; }

   /** Set the id of the child FSM for this state. */
   void SetChildFSM_Id(const size_t aChildFSM_Id) { mChildFSM_Id = aChildFSM_Id; }

   /** Set the tree id for this node. */
   void SetTreeId(const size_t aId) { mTreeId = aId; }

   /** Set the number of blackboard vars for this node.
    * @param aValue value to set.
    */
   void SetNumBlackboardVars(const size_t aValue) { mBlackboardVarCount = aValue; }

protected:
   /** Get the number of blackboard variables to display on the node. */
   const size_t GetNumBlackboardVars() const { return mBlackboardVarCount; }

   /** Reset's all variables used for the Reingold-Tilford algorithm.
    * This recursively hits all children, so it should be called on root.
    * @param aNode node to reset vars for.
    */
   static void ResetLayoutVars(GraphicsNode* aNode);

   /** Position the node text (title), expanding the node's width if necessary. */
   void PositionNodeText();

   /** Qt - Bounding rect of the node. */
   QRectF boundingRect() const override { return QRectF(0, 0, mWidth, mHeight); }

   /** Qt - This is where the object draws itself onto the scene.
    * @param painter QPainter used to paint this object
    * @param option Style options to use when painting
    * @param widget used for cached painting
    */
   void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

   /** Qt - Handle a hover event (tooltip)
    * @param event hover event to handle
    */
   void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

   /** Get the horizontal line connecting the far left child to the far right child. */
   const QLineF GetHorizontalLine() const;

   /** Get the input line extending from the top of this node. */
   const QLineF GetInputLine() const;

   /** Get the output line extending from the bottom of this node. */
   const QLineF GetOutputLine() const;

   /** Get a line from the parent's output to the child's input (used for success lines). */
   const QLineF GetLineFromParentOutputToInput() const;

   /** Get the evaluation lines (used when a node is successful to trace back add green lines). */
   const QLineF* GetEvaluationLines() const;

   /** Checks if any child has evaluated, to update the output line. */
   const NodeState GetChildEvaluation() const;

   /** All of the functions below are a part of a modified Reingold-Tilford tidy tree
    * algorithm for drawing Non-layered tidy trees in linear time, published on
    * August 5, 2013, by A.J. van der Ploeg.
    * https://www.researchgate.net/publication/264676991_Drawing_Non-Layered_Tidy_Trees_In_Linear_Time
    */

   /** Do the first walk over the tree.  This will set preliminary positioning.
    * @param aNode subtree to do first pass on.
    */
   static void FirstWalk(GraphicsNode* aNode);

   /** Do the second walk over the tree. This will finalize positioning.
    * @param aNode node to do second pass on.
    * @param aModSum mod sum to utilize and pass down for shifting positioning.
    */
   static void SecondWalk(GraphicsNode* aNode, double aModSum);

   /** Sets the extreme values for a node.
    * @param aNode node to set extremes for.
    */
   static void SetExtremes(GraphicsNode* aNode);

   /** Check a subtree and separate children if needed.
    * @param aNode subtree to pass over.
    * @param aI index of child to check at.
    * @param aIh lowest y value and index to use to find conflicts.
    */
   static void Separate(GraphicsNode* aNode, int aI, RvBAT::LowestYIndex* aIh);


   /** Shifts a subtree by a distance to resolve a conflict, and distributes the whitespace in between the subtrees.
    * @param aNode tree to operate on.
    * @param aI index of subtree to shift.
    * @param aSi index of node to check for intermediate children.
    * @param aDist distance to move the subtree.
    */
   static void MoveSubtree(GraphicsNode* aNode, int aI, int aSi, double aDist);

   /** Return the next left contour of a node. This is the furthest left child.
    * @param aNode node to get contour from.
    */
   static GraphicsNode* NextLeftContour(GraphicsNode* aNode);

   /** Return the next right contour of a node. This is the furthest right child.
    * @param aNode node to get contour from.
    */
   static GraphicsNode* NextRightContour(GraphicsNode* aNode);

   /** Return the bottom Y value for a node.
    * @param aNode node to get bottom of.
    */
   static double Bottom(GraphicsNode* aNode);

   /** Sets the left thread for a node's leftmost child.
    * @param aNode node to set leftmost child's thread for.
    * @param aI index of the extreme node to set our extreme node values to.
    * @param aCl left thread node.
    * @param aModSumCl mod sum of our left thread node.
    */
   static void SetLeftThread(GraphicsNode* aNode, int aI, GraphicsNode* aCl, double aModSumCl);

   /** Sets the right thread for a node's rightmost child. (Symmetrical to SetLeftNode)
    * @param aNode node to set rightmost child's thread for.
    * @param aI index of the extreme node to set our extreme node values to.
    * @param aSr right thread node.
    * @param aModSumSr mod sum of our right thread node.
    */
   static void SetRightThread(GraphicsNode* aNode, int aI, GraphicsNode* aSr, double aModSumSr);

   /** Position root between children, taking into account their mod.
    * @param aNode node to position.
    */
   static void PositionRoot(GraphicsNode* aNode);

   /** Distrubute the whitespace in between two subtrees.
    * @param aNode node to distribute space for.
    * @param aI index of left child to shift.
    * @param aSi index of right child to shift.
    * @param aDist distance to distribute.
    */
   static void DistributeExtra(GraphicsNode* aNode, int aI, int aSi, double aDist);

   /** Process change and shift to add intermediate spacing to a node's mod.
    * @param aNode node to add spacing for.
    */
   static void AddChildSpacing(GraphicsNode* aNode);

   /** Updates and returns an object giving the lowest y value and index for a subtree.
    * @param aMinY minimum Y to compare against.
    * @param aI index to pass into the object for checking later.
    * @param aIh pointer to the next lowest Y object.
    */
   static LowestYIndex* UpdateLowestYIndex(double aMinY, int aI, LowestYIndex* aIh);

   /** Return a string describing the node type for the tooltip. */
   const QString GetNodeTypeString();

private:
   size_t             mId;                          ///< Unique id of the node.
   NodeState          mNodeState{NodeState::cIDLE}; ///< State of this node currently (Has it evaluated?).
   QGraphicsTextItem* mNodeText;                    ///< Text to display at the bottom middle of the node.
   QString mTooltipDesc{""}; ///< Tooltip description.  This will show the description for what a node does if it has one.
   QString            mPreconditionTooltip{""}; ///< Precondition tooltip text. Shows when moused over.
   QString            mExecuteTooltip{""};      ///< Execute tooltip text. Shows when moused over.
   NodeType           mNodeType;                ///< What type of node is this?
   GraphicsNode*      mParent{nullptr};         ///< The parent node of this node, if applicable.
   QGraphicsLineItem* mInputLine;               ///< Input line for this node.
   QGraphicsLineItem* mOutputLine;              ///< Output line for this node.
   QGraphicsLineItem* mToParentLine;            ///< Horizontal line from this node's input to its parent's output.
   QPixmap            mImage;                   ///< Image to draw in the center of the node, visualizing the node type
   bool mSelected{false}; ///< True if this is the node selected to show its blackboard data, false otherwise.
   QVector<BlackboardData> mBlackboardData; ///< Current blackboard data for this graphics node.
   bool mChildrenVisible{true}; ///< True if children graphics items are visible, false otherwise.  Used for hiding subtrees.
   bool   mIsTree{false};       ///< True if this node is the root node of a tree, false otherwise.
   size_t mTreeId{0};           ///< Id of the tree that owns this node.
   size_t mBlackboardVarCount{
      0}; ///< Number of blackboard vars, to show on the node. (Variable is needed to pass shared bb data from view).
   size_t mChildFSM_Id{0}; ///< Id of the child FSM if it exists

   // Tidy tree variables
   int                    mWidth;                        ///< Width of the node.
   int                    mHeight;                       ///< Height of the node.
   int                    mX;                            ///< X coordinate of the node's position.
   int                    mY;                            ///< Y coordinate of the node's position.
   double                 mPrelim{0.0};                  ///< Preliminary x position for a node.
   double                 mMod{0.0};                     ///< Modified distance for a node.
   double                 mShift{0.0};                   ///< Shift for a node.
   double                 mChange{0.0};                  ///< Change for a node.
   GraphicsNode*          mLeftThread{nullptr};          ///< The left thread.
   GraphicsNode*          mRightThread{nullptr};         ///< The right thread.
   GraphicsNode*          mExtremeLeft{nullptr};         ///< The extreme left node.
   GraphicsNode*          mExtremeRight{nullptr};        ///< The extreme right node.
   double                 mSumModifierExtremeLeft{0.0};  ///< Sum of modifiers at the extreme left.
   double                 mSumModifierExtremeRight{0.0}; ///< Sum of modifiers at the extreme right.
   QVector<GraphicsNode*> mChildren;                     ///< The children of this node, if applicable.
};
} // namespace RvBAT
#endif // RVBAT_GRAPHICSNODE_HPP
