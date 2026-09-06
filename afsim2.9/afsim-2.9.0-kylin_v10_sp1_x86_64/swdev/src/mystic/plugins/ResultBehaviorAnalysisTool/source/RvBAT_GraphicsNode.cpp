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

#include "RvBAT_GraphicsNode.hpp"

#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QIcon>
#include <QImage>
#include <QTextItem>

#include "RvBAT_GraphicsColors.hpp"
#include "RvEventPipeClasses.hpp"

RvBAT::GraphicsNode::GraphicsNode(size_t aId, QRectF aRect, NodeType aNodeType, bool aHasRun, bool aSuccess)
   : QGraphicsItem(nullptr)
   , mId(aId)
   , mNodeText(new QGraphicsTextItem(this))
   , mNodeType(aNodeType)
   , mInputLine(new QGraphicsLineItem(this))
   , mOutputLine(new QGraphicsLineItem(this))
   , mToParentLine(new QGraphicsLineItem(this))
   , mWidth(aRect.width())
   , mHeight(aRect.height())
   , mX(aRect.x())
   , mY(aRect.y())
   , mChildren(0)
{
   setPos(aRect.x(), aRect.y());
   // Enable hover events for tooltips
   setAcceptHoverEvents(true);

   setFlag(QGraphicsItem::GraphicsItemFlag::ItemDoesntPropagateOpacityToChildren);
   setFlag(QGraphicsItem::ItemIsMovable);

   switch (mNodeType)
   {
   case NodeType::cPARALLEL:
      SetNodeImage("parallel");
      break;
   case NodeType::cSEQUENCE:
   case NodeType::cSEQUENCE_WITH_MEMORY:
      SetNodeImage("sequence");
      break;
   case NodeType::cSELECTOR:
   case NodeType::cSELECTOR_WITH_MEMORY:
      SetNodeImage("selector");
      break;
   case NodeType::cTASK:
   case NodeType::cWEIGHTED_RANDOM:
   case NodeType::cPRIORITY_SELECTOR:
   default:
      SetNodeImage("task");
      break;
   }

   QFont f;
   f.setPointSize(24);
   mNodeText->setFont(f);
}

RvBAT::GraphicsNode::~GraphicsNode()
{
   if (mNodeText != nullptr)
   {
      delete mNodeText;
      mNodeText = nullptr;
   }

   if (mInputLine != nullptr)
   {
      delete mInputLine;
      mInputLine = nullptr;
   }

   if (mOutputLine != nullptr)
   {
      delete mOutputLine;
      mOutputLine = nullptr;
   }

   if (mToParentLine != nullptr)
   {
      delete mToParentLine;
      mToParentLine = nullptr;
   }

   for (auto c : mChildren)
   {
      delete c;
      c = nullptr;
   }
   mChildren.clear();
}

void RvBAT::GraphicsNode::SetNodeImage(const std::string& aImageName)
{
   mImage = QPixmap::fromImage(
      QImage(QString::fromStdString(":/images/" + QIcon::themeName().toStdString() + "/60x60/" + aImageName))
         .scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void RvBAT::GraphicsNode::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
   QString toolTipTitle        = "<h2>" + mNodeText->toPlainText() + "</h2>";
   QString toolTipType         = "<h4>Type: " + GetNodeTypeString() + " node</h4>";
   QString toolTipDesc         = "<h4>" + mTooltipDesc + "</h4>";
   QString preconditionTooltip = (!mPreconditionTooltip.isEmpty() ? "<b> Precondition: </b>" + mPreconditionTooltip : "");
   QString executeTooltip =
      (!mExecuteTooltip.isEmpty() ?
          (!mPreconditionTooltip.isEmpty() ? "<br><br><b> Execute: </b>" : "<b> Execute: </b>") + mExecuteTooltip :
          "");

   // The final tooltip string
   QString toolTipString = toolTipTitle + (!mTooltipDesc.isEmpty() ? toolTipDesc : "") +
                           (!toolTipType.isEmpty() ? toolTipType : "") + preconditionTooltip + executeTooltip;
   setToolTip(toolTipString);
}

const QLineF RvBAT::GraphicsNode::GetHorizontalLine() const
{
   // Start the line a certain amount of spacing below the bottom of the parent node
   int y = boundingRect().bottom() + cNODE_VERTICAL_SPACING;
   // p1 is the center x position of the first node (far left)
   QPointF p1 = QPointF(mChildren.first()->pos().x() + mChildren.first()->boundingRect().center().x(), y);
   // p2 is the center x position of the last node (far right)
   QPointF p2 = QPointF(mChildren.last()->pos().x() + mChildren.last()->boundingRect().center().x(), y);
   return QLineF(p1, p2);
}

const QLineF RvBAT::GraphicsNode::GetInputLine() const
{
   // p1 is the top point
   QPointF p1 = QPointF(boundingRect().center().x(), -1.0 * cNODE_LINE_LENGTH);

   // p2 is the bottom point
   QPointF p2 = QPointF(boundingRect().center().x(), 0);

   return QLineF(p1, p2);
}

const QLineF RvBAT::GraphicsNode::GetOutputLine() const
{
   // p1 is the top point
   QPointF p1 = QPointF(boundingRect().center().x(), boundingRect().bottom());

   // p2 is the bottom point
   QPointF p2 = QPointF(boundingRect().center().x(), boundingRect().bottom() + cNODE_LINE_LENGTH);

   return QLineF(p1, p2);
}

const QLineF RvBAT::GraphicsNode::GetLineFromParentOutputToInput() const
{
   // p1 is the top point of the input line for this node
   QLineF  l1 = GetInputLine();
   QPointF p1 = QPointF(l1.x1(), l1.y1());

   // p2 is the bottom point of the output line for the parent node
   QLineF  l2 = mParent->GetOutputLine();
   QPointF p2 = mapFromItem(mParent, QPointF(l2.x2(), l2.y2()));

   return QLineF(p1, p2);
}

const QLineF* RvBAT::GraphicsNode::GetEvaluationLines() const
{
   auto evalLines = new QLineF[3];

   // line 0 is the output line extending from the parent
   // map the parent's local coordinates to this child's local coordinates
   QLineF  l1   = mParent->GetOutputLine();
   QPointF p1   = mapFromItem(mParent, QPointF(l1.x1(), l1.y1()));
   QPointF p2   = mapFromItem(mParent, QPointF(l1.x2(), l1.y2()));
   evalLines[0] = QLineF(p1, p2);

   // line 1 is the line connecting from the parent output to the child input
   evalLines[1] = GetLineFromParentOutputToInput();

   // line 2 is the input line extending from the child
   evalLines[2] = GetInputLine();

   return evalLines;
}

const RvBAT::NodeState RvBAT::GraphicsNode::GetChildEvaluation() const
{
   NodeState tmp = NodeState::cIDLE;

   // make a map of priority values to check against
   std::map<NodeState, int> priorityMap;
   priorityMap[NodeState::cRUNNING]  = 5;
   priorityMap[NodeState::cSUCCESS]  = 4;
   priorityMap[NodeState::cFAILURE]  = 3;
   priorityMap[NodeState::cHALTED]   = 2;
   priorityMap[NodeState::cIDLE]     = 1;
   priorityMap[NodeState::cDISABLED] = 0;

   // For sequences, we want failure to override success, as it makes sense visually.  This is also checked in paint()
   if (GetNodeType() == NodeType::cSEQUENCE)
   {
      priorityMap[NodeState::cSUCCESS] = 3;
      priorityMap[NodeState::cFAILURE] = 4;
   }

   // We want the highest priority line (SUCCESS > RUNNING > FAILURE > HALTED > IDLE > DISABLED)
   for (GraphicsNode* n : mChildren)
      if (priorityMap[n->GetNodeState()] > priorityMap[tmp])
      {
         tmp = n->GetNodeState();
      }
   return tmp;
}

void RvBAT::GraphicsNode::SetStartingY(GraphicsNode* aNode)
{
   aNode->mY = (aNode->GetParent() != nullptr) ?
                  aNode->GetParent()->mY + aNode->GetParent()->mHeight + ((int)cNODE_VERTICAL_SPACING * 2) :
                  aNode->scenePos().y();
}

void RvBAT::GraphicsNode::ToggleChildrenVisible()
{
   mChildrenVisible = !mChildrenVisible;
   for (auto& child : mChildren)
   {
      child->setVisible(mChildrenVisible);
   }
   mOutputLine->setVisible(mChildrenVisible);
}

void RvBAT::GraphicsNode::ResetLayoutVars(GraphicsNode* aNode)
{
   // Vars to reset
   aNode->mPrelim                  = 0.0;
   aNode->mMod                     = 0.0;
   aNode->mShift                   = 0.0;
   aNode->mChange                  = 0.0;
   aNode->mLeftThread              = nullptr;
   aNode->mRightThread             = nullptr;
   aNode->mExtremeLeft             = nullptr;
   aNode->mExtremeRight            = nullptr;
   aNode->mSumModifierExtremeLeft  = 0.0;
   aNode->mSumModifierExtremeRight = 0.0;
   GraphicsNode::SetStartingY(aNode);

   // Run this recursively over each child as well
   for (int i = 0; i < aNode->mChildren.size(); i++)
   {
      aNode->mChildren[i]->ResetLayoutVars(aNode->mChildren[i]);
   }
}

void RvBAT::GraphicsNode::UpdateTextColor()
{
   if (mSelected)
   {
      mNodeText->setDefaultTextColor(QIcon::themeName() == "Dark" ? QColor("#EEEEEE") : cDEFAULT_TEXT_COLOR_LIGHT);
   }
   else
   {
      mNodeText->setDefaultTextColor(QIcon::themeName() == "Dark" ? cDEFAULT_TEXT_COLOR_DARK : cDEFAULT_TEXT_COLOR_LIGHT);
   }
}

void RvBAT::GraphicsNode::SetNodeText(const std::string& aText)
{
   mNodeText->setPlainText(QString::fromStdString(aText));

   PositionNodeText();
}

void RvBAT::GraphicsNode::PositionNodeText()
{
   // Make sure our node is wide enough for the text, plus a little bit
   // Take the bigger of the stretched width or 160 pixels
   int stretchedWidth = mNodeText->boundingRect().width() + cNODE_HORIZONTAL_SPACING;
   (stretchedWidth < 160) ? mWidth = 160 : mWidth = stretchedWidth;
   mNodeText->setDefaultTextColor(QIcon::themeName() == "Dark" ? cDEFAULT_TEXT_COLOR_DARK : cDEFAULT_TEXT_COLOR_LIGHT);
   mNodeText->setParentItem(this);
   mNodeText->setPos(boundingRect().center().x() - mNodeText->boundingRect().center().x(), mImage.rect().bottom() + 10);
}

void RvBAT::GraphicsNode::SetNodeState(const NodeState aState)
{
   mNodeState = aState;
}

void RvBAT::GraphicsNode::SetTooltipDesc(const QString& aTooltipDesc)
{
   mTooltipDesc = aTooltipDesc;
}

void RvBAT::GraphicsNode::AddChildNode(GraphicsNode* aNode)
{
   aNode->mParent = this;
   aNode->setParentItem(this);
   mChildren.append(aNode);
}

void RvBAT::GraphicsNode::SetBlackboardData(const QVector<BlackboardData>& aData)
{
   mBlackboardData = aData;

   size_t num = 0;
   for (auto& bb : mBlackboardData)
   {
      num += bb.boolDict().size();
      num += bb.doubleDict().size();
      num += bb.intDict().size();
      num += bb.platformDict().size();
      num += bb.stringDict().size();
      num += bb.trackDict().size();
   }
   mBlackboardVarCount = num;
}

void RvBAT::GraphicsNode::ClearBlackboardData()
{
   mBlackboardData.clear();
}

void RvBAT::GraphicsNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
   if (mNodeState == NodeState::cDISABLED)
   {
      setOpacity(0.5);
      mInputLine->setOpacity(0.5);
      mOutputLine->setOpacity(0.5);
      mToParentLine->setOpacity(0.5);
   }
   else
   {
      setOpacity(1.0);
      mInputLine->setOpacity(1.0);
      mOutputLine->setOpacity(1.0);
      mToParentLine->setOpacity(1.0);
   }

   bool isDarkTheme = (QIcon::themeName() == "Dark") ? true : false;
   // Create a linear gradient pen for the outline of the node
   QPen pen = QPen();

   // Pen for drawing running lines
   QPen runningPen = QPen();
   runningPen.setWidthF(2.2);
   runningPen.setColor(isDarkTheme ? cRUNNING_PEN_COLOR_DARK : cRUNNING_PEN_COLOR_LIGHT);

   // Pen for drawing success lines
   QPen successPen = QPen();
   successPen.setWidthF(2.2);
   successPen.setColor(isDarkTheme ? cSUCCESS_PEN_COLOR_DARK : cSUCCESS_PEN_COLOR_LIGHT);

   // Pen for drawing failure lines
   QPen failurePen = QPen();
   failurePen.setWidthF(2.2);
   failurePen.setColor(isDarkTheme ? cFAILURE_PEN_COLOR_DARK : cFAILURE_PEN_COLOR_LIGHT);

   // Pen for drawing halted lines
   QPen haltedPen = QPen();
   haltedPen.setWidthF(2.2);
   haltedPen.setColor(isDarkTheme ? cHALTED_PEN_COLOR_DARK : cHALTED_PEN_COLOR_LIGHT);

   // Create a linear gradient
   QLinearGradient g = QLinearGradient();
   g.setStart(mWidth, 0);
   g.setFinalStop(mWidth, mHeight);

   // Color the node based on success
   // Set the gradient colors for the background of the node.
   if (mNodeState == NodeState::cRUNNING)
   {
      g.setColorAt(0, isDarkTheme ? cRUNNING_GRADIENT_TOP_DARK : cRUNNING_GRADIENT_LOW_LIGHT);
      g.setColorAt(1, isDarkTheme ? cRUNNING_GRADIENT_BOTTOM_DARK : cRUNNING_GRADIENT_HIGH_LIGHT);
   }
   else if (mNodeState == NodeState::cSUCCESS)
   {
      g.setColorAt(0, isDarkTheme ? cSUCCESS_GRADIENT_TOP_DARK : cSUCCESS_GRADIENT_TOP_LIGHT);
      g.setColorAt(1, isDarkTheme ? cSUCCESS_GRADIENT_BOTTOM_DARK : cSUCCESS_GRADIENT_BOTTOM_LIGHT);
   }
   else if (mNodeState == NodeState::cFAILURE)
   {
      g.setColorAt(0, isDarkTheme ? cFAILURE_GRADIENT_TOP_DARK : cFAILURE_GRADIENT_LOW_LIGHT);
      g.setColorAt(1, isDarkTheme ? cFAILURE_GRADIENT_BOTTOM_DARK : cFAILURE_GRADIENT_HIGH_LIGHT);
   }
   else if (mNodeState == NodeState::cHALTED)
   {
      g.setColorAt(0, isDarkTheme ? cHALTED_GRADIENT_TOP_DARK : cHALTED_GRADIENT_LOW_LIGHT);
      g.setColorAt(1, isDarkTheme ? cHALTED_GRADIENT_BOTTOM_DARK : cHALTED_GRADIENT_HIGH_LIGHT);
   }
   else
   {
      g.setColorAt(0, isDarkTheme ? cDEFAULT_GRADIENT_TOP_DARK : cDEFAULT_GRADIENT_TOP_LIGHT);
      g.setColorAt(1, isDarkTheme ? cDEFAULT_GRADIENT_BOTTOM_DARK : cDEFAULT_GRADIENT_BOTTOM_LIGHT);
   }

   // Set the current brush to the gradient.
   painter->setBrush(QBrush(g));


   // Set the width and color of the pen, and set it to the painter, for drawing the branching lines
   pen.setWidthF(2.0);
   pen.setColor(isDarkTheme ? cDEFAULT_PEN_COLOR_DARK : cDEFAULT_PEN_COLOR_LIGHT);
   painter->setPen(pen);

   // If this node has a parent, we want to draw the input line.  This will call paint and draw itself.
   if (mParent != nullptr)
   {
      // Set the line color if the node has evaluated
      if (mNodeState == NodeState::cRUNNING)
      {
         mInputLine->setPen(runningPen);
      }
      else if (mNodeState == NodeState::cSUCCESS)
      {
         mInputLine->setPen(successPen);
      }
      else if (mNodeState == NodeState::cFAILURE)
      {
         mInputLine->setPen(failurePen);
      }
      else if (mNodeState == NodeState::cHALTED)
      {
         mInputLine->setPen(haltedPen);
      }
      else
      {
         mInputLine->setPen(pen);
      }
      mInputLine->setLine(GetInputLine());
      mInputLine->show();
   }
   else
   {
      mInputLine->hide();
   }

   // If this node has at least 1 child, we want to draw an output line.  This will call paint and draw itself.
   if (mChildren.count() > 0)
   {
      NodeState childEval = GetChildEvaluation();
      // Set the line color if the node has evaluated
      if (childEval == NodeState::cRUNNING)
      {
         mOutputLine->setPen(runningPen);
      }
      else if (childEval == NodeState::cSUCCESS)
      {
         mOutputLine->setPen(successPen);
      }
      else if (childEval == NodeState::cFAILURE)
      {
         mOutputLine->setPen(failurePen);
      }
      else if (childEval == NodeState::cHALTED)
      {
         mOutputLine->setPen(haltedPen);
      }
      else
      {
         mOutputLine->setPen(pen);
      }
      mOutputLine->setLine(GetOutputLine());
      if (mChildrenVisible)
      {
         mOutputLine->show();
      }
   }
   else
   {
      mOutputLine->hide();
   }

   // If this node evaluated draw the eval lines
   // Set the Z value accordingly for line colors
   if (mParent != nullptr)
   {
      // Set the line color if the node has evaluated
      if (mNodeState == NodeState::cRUNNING)
      {
         mToParentLine->setPen(runningPen);
         setZValue(4);
      }
      else if (mNodeState == NodeState::cSUCCESS)
      {
         mToParentLine->setPen(successPen);
         setZValue(2);
      }
      else if (mNodeState == NodeState::cFAILURE)
      {
         mToParentLine->setPen(failurePen);

         // If we are working with a sequence, then we want failure lines drawn over success lines
         if (GetNodeType() == NodeType::cSEQUENCE)
         {
            setZValue(3);
         }
         else
         {
            setZValue(1);
         }
      }
      else if (mNodeState == NodeState::cHALTED)
      {
         mToParentLine->setPen(haltedPen);
         setZValue(1);
      }
      else
      {
         mToParentLine->setPen(pen);
         setZValue(0);
      }
      mToParentLine->setLine(GetLineFromParentOutputToInput());
      mToParentLine->show();
   }
   else
   {
      mToParentLine->hide();
   }

   // Set the gradient colors for the outline of the node.  If successful, we want green.  If it has failed, we want
   // red.  Otherwise, Grey.

   if (mSelected)
   {
      g.setColorAt(0, isDarkTheme ? QColor("#EEEEEE") : cDEFAULT_OUTLINE_GRADIENT_BOTTOM_LIGHT);
      g.setColorAt(1, isDarkTheme ? QColor("#EEEEEE") : cDEFAULT_OUTLINE_GRADIENT_BOTTOM_LIGHT);
   }
   else if (mNodeState == NodeState::cRUNNING)
   {
      g.setColorAt(0, isDarkTheme ? cRUNNING_OUTLINE_GRADIENT_TOP_DARK : cRUNNING_OUTLINE_GRADIENT_TOP_LIGHT);
      g.setColorAt(1, isDarkTheme ? cRUNNING_OUTLINE_GRADIENT_BOTTOM_DARK : cRUNNING_OUTLINE_GRADIENT_BOTTOM_LIGHT);
   }
   else if (mNodeState == NodeState::cSUCCESS)
   {
      g.setColorAt(0, isDarkTheme ? cSUCCESS_OUTLINE_GRADIENT_TOP_DARK : cSUCCESS_OUTLINE_GRADIENT_TOP_LIGHT);
      g.setColorAt(1, isDarkTheme ? cSUCCESS_OUTLINE_GRADIENT_BOTTOM_DARK : cSUCCESS_OUTLINE_GRADIENT_BOTTOM_LIGHT);
   }
   else if (mNodeState == NodeState::cFAILURE)
   {
      g.setColorAt(0, isDarkTheme ? cFAILURE_OUTLINE_GRADIENT_TOP_DARK : cFAILURE_OUTLINE_GRADIENT_TOP_LIGHT);
      g.setColorAt(1, isDarkTheme ? cFAILURE_OUTLINE_GRADIENT_BOTTOM_DARK : cFAILURE_OUTLINE_GRADIENT_BOTTOM_LIGHT);
   }
   else if (mNodeState == NodeState::cHALTED)
   {
      g.setColorAt(0, isDarkTheme ? cHALTED_OUTLINE_GRADIENT_TOP_DARK : cHALTED_OUTLINE_GRADIENT_TOP_LIGHT);
      g.setColorAt(1, isDarkTheme ? cHALTED_OUTLINE_GRADIENT_BOTTOM_DARK : cHALTED_OUTLINE_GRADIENT_BOTTOM_LIGHT);
   }
   else
   {
      g.setColorAt(0, isDarkTheme ? cDEFAULT_OUTLINE_GRADIENT_TOP_DARK : cDEFAULT_OUTLINE_GRADIENT_TOP_LIGHT);
      g.setColorAt(1, isDarkTheme ? cDEFAULT_OUTLINE_GRADIENT_BOTTOM_DARK : cDEFAULT_OUTLINE_GRADIENT_BOTTOM_LIGHT);
   }

   // Set the brush width and set the pen to the painter, for drawing the rect.
   pen.setBrush(QBrush(g));
   if (mSelected)
   {
      pen.setWidthF(4);
   }
   else
   {
      pen.setWidthF(2);
   }
   painter->setPen(pen);
   painter->drawRoundedRect(0, 0, mWidth, mHeight, 5, 5);

   // Lower the opacity slightly and draw the image
   // If the node is disabled lower the opacity more
   if (mNodeState == NodeState::cDISABLED)
   {
      painter->setOpacity(0.40);
   }
   else
   {
      painter->setOpacity(0.6);
   }

   if (mSelected)
   {
      painter->setOpacity(1.0);
   }
   painter->drawPixmap(QPointF(boundingRect().center().x() - mImage.rect().center().x(), 10), mImage);
   painter->setOpacity(1.0);

   // Draw attributes in the corner of the node, such as children being hidden,
   // if a node has blackboard variables, and if the node is a tree.
   if (mIsTree)
   {
      QPixmap treePixmap =
         QPixmap::fromImage(QImage(QString::fromStdString(":/images/" + QIcon::themeName().toStdString() + "/60x60/root"))
                               .scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      painter->drawPixmap(QPointF(boundingRect().topRight().x() - treePixmap.rect().width() - 10, 10), treePixmap);
   }

   if (!mChildrenVisible)
   {
      QIcon   icon = QIcon::fromTheme("hide");
      QPixmap childrenPixmap =
         icon.pixmap(icon.actualSize(QSize(64, 64))).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
      painter->drawPixmap(QPointF(boundingRect().topRight().x() - childrenPixmap.rect().width() - 32, 10), childrenPixmap);
   }

   if (mChildFSM_Id != 0)
   {
      QPixmap fsmPixmap =
         QPixmap::fromImage(QImage(QString::fromStdString(":/images/" + QIcon::themeName().toStdString() + "/60x60/fsm"))
                               .scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      painter->drawPixmap(QPointF(boundingRect().topRight().x() - fsmPixmap.rect().width() - 10, 32), fsmPixmap);
   }

   if (GetNumBlackboardVars() > 0)
   {
      QPen p;
      p.setColor(QIcon::themeName() == "Dark" ? QColor("#EEEEEE") : cDEFAULT_TEXT_COLOR_LIGHT);
      QFont f;
      f.setPointSize(15);
      painter->setFont(f);
      painter->setPen(p);
      painter->drawText(QPointF(boundingRect().topLeft().x() + 10, 28), QString::number(GetNumBlackboardVars()));
   }
}

void RvBAT::GraphicsNode::Layout(GraphicsNode* aNode)
{
   GraphicsNode::ResetLayoutVars(aNode);
   FirstWalk(aNode);
   SecondWalk(aNode, 0);
}

void RvBAT::GraphicsNode::FirstWalk(GraphicsNode* aNode)
{
   if (aNode->mChildren.size() == 0 || !aNode->AreChildrenVisible())
   {
      SetExtremes(aNode);
      return;
   }
   FirstWalk(aNode->mChildren[0]);
   // Create siblings in contour minimal vertical coordinate and index list
   LowestYIndex* ih = UpdateLowestYIndex(Bottom(aNode->mChildren[0]->mExtremeLeft), 0, nullptr);
   for (int i = 1; i < aNode->mChildren.size(); i++)
   {
      FirstWalk(aNode->mChildren[i]);
      // Store lowest vertical coordinate while extreme nodes still point in current subtree
      double minY = Bottom(aNode->mChildren[i]->mExtremeRight);
      Separate(aNode, i, ih);
      ih = UpdateLowestYIndex(minY, i, ih);
   }
   PositionRoot(aNode);
   SetExtremes(aNode);
}

void RvBAT::GraphicsNode::SecondWalk(GraphicsNode* aNode, double aModSum)
{
   aModSum += aNode->mMod;
   // Set absolute (non-relative) horizontal coordinate
   aNode->mX = aNode->mPrelim + aModSum;
   if (aNode->AreChildrenVisible())
   {
      AddChildSpacing(aNode);
      for (int i = 0; i < aNode->mChildren.size(); i++)
      {
         SecondWalk(aNode->mChildren[i], aModSum);
      }
   }
   aNode->setPos((aNode->GetParent() != nullptr) ? aNode->mX - aNode->GetParent()->mX : aNode->scenePos().x(),
                 (aNode->GetParent() != nullptr) ? aNode->mY - aNode->GetParent()->mY : aNode->mY);
}

void RvBAT::GraphicsNode::SetExtremes(GraphicsNode* aNode)
{
   if (aNode->mChildren.size() == 0 || !aNode->AreChildrenVisible())
   {
      aNode->mExtremeLeft             = aNode;
      aNode->mExtremeRight            = aNode;
      aNode->mSumModifierExtremeLeft  = 0;
      aNode->mSumModifierExtremeRight = 0;
   }
   else
   {
      aNode->mExtremeLeft             = aNode->mChildren[0]->mExtremeLeft;
      aNode->mSumModifierExtremeLeft  = aNode->mChildren[0]->mSumModifierExtremeLeft;
      aNode->mExtremeRight            = aNode->mChildren[aNode->mChildren.size() - 1]->mExtremeRight;
      aNode->mSumModifierExtremeRight = aNode->mChildren[aNode->mChildren.size() - 1]->mSumModifierExtremeRight;
   }
}

void RvBAT::GraphicsNode::Separate(GraphicsNode* aNode, int aI, RvBAT::LowestYIndex* aIh)
{
   // Right contour node of left siblings and its sum of modifiers
   GraphicsNode* sr   = nullptr;
   double        mssr = 0.0;
   if (aI > 0)
   {
      sr   = aNode->mChildren[aI - 1];
      mssr = sr->mMod;
   }
   // Left contou node of right siblings and its sum of modifiers
   GraphicsNode* cl   = aNode->mChildren[aI];
   double        mscl = cl->mMod;
   while (sr != nullptr && cl != nullptr)
   {
      if (Bottom(sr) > aIh->mLowY)
      {
         aIh = aIh->mNext;
      }

      // How far to the left of the right side of sr is the left side of cl?
      double dist = (mssr + sr->mPrelim + sr->mWidth + cNODE_HORIZONTAL_SPACING) - (mscl + cl->mPrelim);
      if (dist > 0)
      {
         mscl += dist;
         MoveSubtree(aNode, aI, aIh->mIndex, dist);
      }

      double sy = Bottom(sr);
      double cy = Bottom(cl);
      // Advance highest node(s) and sum(s) of modifiers (Coordinate system increases downwards)
      if (sy <= cy)
      {
         sr = NextRightContour(sr);
         if (sr != nullptr)
         {
            mssr += sr->mMod;
         }
      }
      if (sy >= cy)
      {
         cl = NextLeftContour(cl);
         if (cl != nullptr)
         {
            mscl += cl->mMod;
         }
      }
   }
   // Set threads and update extreem nodes
   // In the first case, the current subtree must be taller than the left siblings
   if (sr == nullptr && cl != nullptr)
   {
      SetLeftThread(aNode, aI, cl, mscl);
   }
   else if (sr != nullptr && cl == nullptr)
   {
      SetRightThread(aNode, aI, sr, mssr);
   }
}

void RvBAT::GraphicsNode::MoveSubtree(GraphicsNode* aNode, int aI, int aSi, double aDist)
{
   aNode->mChildren[aI]->mMod += aDist;
   aNode->mChildren[aI]->mSumModifierExtremeLeft += aDist;
   aNode->mChildren[aI]->mSumModifierExtremeRight +=
      aDist - cNODE_HORIZONTAL_SPACING; // Subtract how much spacing you want to add on the right side of a subtree
                                        // here(e.g. -cNODE_HORIZONTAL_SPACING)
   DistributeExtra(aNode, aI, aSi, aDist);
}

RvBAT::GraphicsNode* RvBAT::GraphicsNode::NextLeftContour(GraphicsNode* aNode)
{
   return (aNode->mChildren.size() == 0 || !aNode->AreChildrenVisible()) ? aNode->mLeftThread : aNode->mChildren[0];
}

RvBAT::GraphicsNode* RvBAT::GraphicsNode::NextRightContour(GraphicsNode* aNode)
{
   return (aNode->mChildren.size() == 0 || !aNode->AreChildrenVisible()) ? aNode->mRightThread :
                                                                           aNode->mChildren[aNode->mChildren.size() - 1];
}

double RvBAT::GraphicsNode::Bottom(GraphicsNode* aNode)
{
   return aNode->mY + aNode->mHeight;
}

void RvBAT::GraphicsNode::SetLeftThread(GraphicsNode* aNode, int aI, GraphicsNode* aCl, double aModSumCl)
{
   GraphicsNode* li = aNode->mChildren[0]->mExtremeLeft;
   li->mLeftThread  = aCl;
   // Change the mod so that the sum of modifiers after following the thread is correct
   double diff = (aModSumCl - aCl->mMod) - aNode->mChildren[0]->mSumModifierExtremeLeft;
   li->mMod += diff;
   // Change preliminary x coordinate so that the node does not move
   li->mPrelim -= diff;
   // Update the extreme node and its sum of modifiers
   aNode->mChildren[0]->mExtremeLeft            = aNode->mChildren[aI]->mExtremeLeft;
   aNode->mChildren[0]->mSumModifierExtremeLeft = aNode->mChildren[aI]->mSumModifierExtremeLeft;
}

void RvBAT::GraphicsNode::SetRightThread(GraphicsNode* aNode, int aI, GraphicsNode* aSr, double aModSumSr)
{
   GraphicsNode* ri = aNode->mChildren[aI]->mExtremeRight;
   ri->mRightThread = aSr;
   // Change the mod so that the sum of modifiers after following the thread is correct
   double diff = (aModSumSr - aSr->mMod) - aNode->mChildren[aI]->mSumModifierExtremeRight;
   ri->mMod += diff;
   // Change preliminary x coordinate so that the node does not move
   ri->mPrelim -= diff;
   // Update the extreme node and its sum of modifiers
   aNode->mChildren[aI]->mExtremeRight            = aNode->mChildren[aI - 1]->mExtremeRight;
   aNode->mChildren[aI]->mSumModifierExtremeRight = aNode->mChildren[aI - 1]->mSumModifierExtremeRight;
}

void RvBAT::GraphicsNode::PositionRoot(GraphicsNode* aNode)
{
   double firstChildPos = aNode->mChildren[0]->mPrelim + aNode->mChildren[0]->mMod;
   double lastChildPos =
      aNode->mChildren[aNode->mChildren.size() - 1]->mPrelim + aNode->mChildren[aNode->mChildren.size() - 1]->mMod;
   int lastChildWidth = aNode->mChildren[aNode->mChildren.size() - 1]->mWidth;

   aNode->mPrelim = ((firstChildPos + lastChildPos + lastChildWidth) / 2) - (aNode->mWidth / 2);
}

void RvBAT::GraphicsNode::DistributeExtra(GraphicsNode* aNode, int aI, int aSi, double aDist)
{
   if (aSi != aI - 1)
   {
      double nr = aI - aSi;
      aNode->mChildren[aSi + 1]->mShift += aDist / nr;
      aNode->mChildren[aI]->mShift -= aDist / nr;
      aNode->mChildren[aI]->mChange -= aDist - (aDist / nr);
   }
}

void RvBAT::GraphicsNode::AddChildSpacing(GraphicsNode* aNode)
{
   double d           = 0;
   double modSumDelta = 0;
   for (int i = 0; i < aNode->mChildren.size(); i++)
   {
      d += aNode->mChildren[i]->mShift;
      modSumDelta += d + aNode->mChildren[i]->mChange;
      aNode->mChildren[i]->mMod += modSumDelta;
   }
}

RvBAT::LowestYIndex::LowestYIndex(double aLowY, int aIndex, LowestYIndex* aNext)
{
   mLowY  = aLowY;
   mIndex = aIndex;
   mNext  = aNext;
}

RvBAT::LowestYIndex* RvBAT::GraphicsNode::UpdateLowestYIndex(double aMinY, int aI, LowestYIndex* aIh)
{
   while (aIh != nullptr && aMinY >= aIh->mLowY)
   {
      aIh = aIh->mNext;
   }
   return new LowestYIndex(aMinY, aI, aIh);
}

const QString RvBAT::GraphicsNode::GetNodeTypeString()
{
   switch (mNodeType)
   {
   case NodeType::cTASK:
      return QString("action");
   case NodeType::cPARALLEL:
      return QString("parallel");
   case NodeType::cSELECTOR:
      return QString("selector");
   case NodeType::cSELECTOR_WITH_MEMORY:
      return QString("selector with memory");
   case NodeType::cSEQUENCE:
      return QString("sequence");
   case NodeType::cSEQUENCE_WITH_MEMORY:
      return QString("sequence with memory");
   case NodeType::cWEIGHTED_RANDOM:
      return QString("weighted random");
   case NodeType::cPRIORITY_SELECTOR:
      return QString("priority selector");
   default:
      return QString("");
   }
}
