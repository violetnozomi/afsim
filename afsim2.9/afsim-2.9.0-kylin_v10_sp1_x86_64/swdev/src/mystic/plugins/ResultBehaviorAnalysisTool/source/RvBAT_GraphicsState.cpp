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

#include "RvBAT_GraphicsState.hpp"

#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QIcon>
#include <QImage>
#include <QLineF>
#include <QTextItem>
#include <QtMath>

#include "RvBAT_GraphicsColors.hpp"
#include "RvBAT_Scene.hpp"
#include "RvEventPipeClasses.hpp"

RvBAT::GraphicsArrow::GraphicsArrow(QGraphicsItem* aStartItem, QGraphicsItem* aEndItem, QGraphicsItem* aParent)
   : QGraphicsLineItem(aParent)
   , mStartItem(aStartItem)
   , mEndItem(aEndItem)
{
}

QRectF RvBAT::GraphicsArrow::boundingRect() const
{
   qreal extra = (pen().width() + 20) / 2.0;

   return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(), line().p2().y() - line().p1().y()))
      .normalized()
      .adjusted(-extra, -extra, extra, extra);
}

QPainterPath RvBAT::GraphicsArrow::shape() const
{
   QPainterPath path = QGraphicsLineItem::shape();
   path.addPolygon(mStartToEndArrow);
   return path;
}

void RvBAT::GraphicsArrow::updatePosition()
{
   QLineF line(mapFromItem(mStartItem, 0, 0), mapFromItem(mEndItem, 0, 0));
   setLine(line);
}

void RvBAT::GraphicsArrow::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
   // Arrow size
   double arrowSize   = 20;
   bool   isDarkTheme = (QIcon::themeName() == "Dark") ? true : false;

   // Set pen and brush defaults
   if (mState == RunState::cACTIVE)
   {
      setZValue(-9);
      auto p = QPen(isDarkTheme ? cRUNNING_PEN_COLOR_DARK : cRUNNING_PEN_COLOR_LIGHT);
      p.setWidth(2);
      painter->setPen(p);
      painter->setBrush(QBrush(isDarkTheme ? cRUNNING_PEN_COLOR_DARK : cRUNNING_PEN_COLOR_LIGHT));
   }
   else
   {
      setZValue(-10);
      painter->setPen(QPen(isDarkTheme ? cDEFAULT_PEN_COLOR_DARK : cDEFAULT_PEN_COLOR_LIGHT));
      painter->setBrush(QBrush(isDarkTheme ? cDEFAULT_PEN_COLOR_DARK : cDEFAULT_PEN_COLOR_LIGHT));
   }

   // Get our start and end points for the arrows by checking
   // the line ellipse collision
   QPointF arrowStartToEndItem, arrowEndToStartItem;
   GetArrowPoints(arrowEndToStartItem, arrowStartToEndItem);

   // Create a line from those points then set and paint our line.
   QLineF pathAsLine(arrowEndToStartItem, arrowStartToEndItem);
   setLine(pathAsLine);
   painter->drawLine(line());

   // Create the two side points of each arrow using the angle
   // and the arrow size
   double  angle = std::atan2(-line().dy(), line().dx());
   QPointF sArrowP1 =
      arrowStartToEndItem + QPointF(sin(angle + M_PI / 3) * -arrowSize, cos(angle + M_PI / 3) * -arrowSize);
   QPointF sArrowP2 = arrowStartToEndItem +
                      QPointF(sin(angle + M_PI - M_PI / 3) * -arrowSize, cos(angle + M_PI - M_PI / 3) * -arrowSize);

   // Clear the arrow (in case it moved) and set the vertices
   mStartToEndArrow.clear();
   mStartToEndArrow << arrowStartToEndItem << sArrowP1 << sArrowP2;
   painter->drawPolygon(mStartToEndArrow);
}

QPointF RvBAT::GraphicsArrow::ClosestPointTo(const QPointF& aTarget, const QPainterPath& aSourcePath)
{
   // Calculate the closest point to a target
   QPointF closestPoint;
   if (!aSourcePath.isEmpty())
   {
      qreal shortestLength = std::numeric_limits<int>::max();

      for (int i = 0; i < aSourcePath.elementCount(); ++i)
      {
         const QPointF distance(aSourcePath.elementAt(i) - aTarget);
         const qreal   length = distance.manhattanLength();
         if (length < shortestLength)
         {
            closestPoint   = aSourcePath.elementAt(i);
            shortestLength = length;
         }
      }
   }
   return closestPoint;
}

void RvBAT::GraphicsArrow::GetArrowPoints(QPointF& aArrowStartItem, QPointF& aArrowEndItem)
{
   // If we have an existing start/end item for a transition
   if (mStartItem != nullptr && mEndItem != nullptr)
   {
      // Get the center position of the start and end items
      QPointF startPos = mStartItem->sceneBoundingRect().center();
      QPointF endPos   = mEndItem->sceneBoundingRect().center();

      // QLineF has normalVector(), which is useful for extending our path to a rectangle.
      // The path needs to be a rectangle, as QPainterPath::intersected() only accounts
      // for intersections between fill areas, which projectilePath doesn't have.
      QLineF pathAsLine(startPos, endPos);

      // Extend the first point in the path out by 1 pixel.
      QLineF startEdge = pathAsLine.normalVector();
      startEdge.setLength(1);

      // Swap the points in the line so the normal vector is at the other end of the line.
      pathAsLine.setPoints(pathAsLine.p2(), pathAsLine.p1());
      QLineF endEdge = pathAsLine.normalVector();

      // The end point is currently pointing the wrong way; move it to face the same
      // direction as startEdge.
      endEdge.setLength(-1);

      // Now we can create a rectangle from our edges.
      QPainterPath rectPath(startEdge.p1());
      rectPath.lineTo(startEdge.p2());
      rectPath.lineTo(endEdge.p2());
      rectPath.lineTo(endEdge.p1());
      rectPath.lineTo(startEdge.p1());

      // Translate the start and end items shape() because it's
      // local coords and we need it in scene coords here.
      QPainterPath startItemPathTranslated = mStartItem->shape();
      startItemPathTranslated.translate(mStartItem->pos());
      QPainterPath endItemPathTranslated = mEndItem->shape();
      endItemPathTranslated.translate(mEndItem->pos());

      // Get the points on the edge of each ellipse. This is done by getting the closest point
      // to the QPainterPath using a start point and the intersection of a shape and the rectPath
      // we created from our line.

      // Get the point for drawing the arrow from the end item to the start item.
      aArrowStartItem = ClosestPointTo(endPos, startItemPathTranslated.intersected(rectPath));

      // Get the point for drawing the arrow from the start item to the end item.
      aArrowEndItem = ClosestPointTo(startPos, endItemPathTranslated.intersected(rectPath));
   }
}

RvBAT::GraphicsState::GraphicsState(const rv::State& aState, QRectF aRect)
   : QGraphicsEllipseItem(aRect, nullptr)
   , mId(aState.stateIndex())
   , mStateText(new QGraphicsTextItem(this))
   , mWidth(aRect.width())
   , mHeight(aRect.height())
   , mX(aRect.x())
   , mY(aRect.y())
   , mTransitions()
   , mChildABT_Id(aState.childAbtId())
   , mChildFSM_Id(aState.childFsmId())
{
   // Set some initial values
   setPos(aRect.x(), aRect.y());
   setRect(0, 0, aRect.width(), aRect.height());
   // Enable hover events for tooltips
   setAcceptHoverEvents(true);
   setFlag(QGraphicsItem::GraphicsItemFlag::ItemDoesntPropagateOpacityToChildren);
   setFlag(QGraphicsItem::ItemIsMovable);

   // Set the state's text
   QFont f;
   f.setPointSize(24);
   mStateText.setFont(f);
   SetStateText(aState.stateName());
}

void RvBAT::GraphicsState::AddTransition(const size_t aIndex, GraphicsState* aGraphicsState)
{
   // Add a transition to this state and the scene
   auto arrow = new GraphicsArrow(this, aGraphicsState);
   mTransitions.insert(aIndex, arrow);
   scene()->addItem(arrow);
}

RvBAT::GraphicsArrow* RvBAT::GraphicsState::GetTransitionGraphic(const size_t aIndex)
{
   // Return a pointer to the graphic if found
   if (mTransitions.find(aIndex) != mTransitions.end())
   {
      return mTransitions.value(aIndex);
   }
   return nullptr;
}

void RvBAT::GraphicsState::SetActiveTransition(const size_t aTransitionIndex)
{
   // Clear the other transitions
   SetIdleTransitions();

   // If we found the transition, set it active
   if (mTransitions.find(aTransitionIndex) != mTransitions.end())
   {
      mTransitions[aTransitionIndex]->SetState(RunState::cACTIVE);
   }
}

void RvBAT::GraphicsState::SetIdleTransitions()
{
   for (auto transition : mTransitions)
   {
      transition->SetState(RunState::cIDLE);
   }
}

void RvBAT::GraphicsState::UpdateTextColor()
{
   mStateText.setDefaultTextColor(QIcon::themeName() == "Dark" ? cDEFAULT_TEXT_COLOR_DARK : cDEFAULT_TEXT_COLOR_LIGHT);
}

void RvBAT::GraphicsState::SetStateText(const std::string& aText)
{
   // Set and position the state text
   mStateText.setPlainText(QString::fromStdString(aText));
   PositionStateText();
}

void RvBAT::GraphicsState::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
   // Create the tooltip string, which should show the name and child ids
   QString tooltipTitle = "<h2>" + mStateText.toPlainText() + "</h2>";

   QString abtName;
   auto    s = dynamic_cast<Scene*>(scene());
   if (s != nullptr)
   {
      abtName = s->GetTreeNameFromId(mChildABT_Id);
   }

   QString childABT = (!abtName.isEmpty() ? "<b> Child ABT: </b>" + abtName : "");
   QString childFSM = (mChildFSM_Id != 0 ? "<b> Child FSM: </b>" + QString("fsm_") + QString::number(mChildFSM_Id) : "");

   QString tooltipString = tooltipTitle + childABT + childFSM;

   setToolTip(tooltipString);
}

void RvBAT::GraphicsState::PositionStateText()
{
   // Make sure our node is wide enough for the text, plus a little bit
   // Take the bigger of the stretched width or 160 pixels
   int stretchedWidth = mStateText.boundingRect().width() + cNODE_HORIZONTAL_SPACING;
   (stretchedWidth < 160) ? mWidth = 160 : mWidth = stretchedWidth;
   mStateText.setDefaultTextColor(QIcon::themeName() == "Dark" ? cDEFAULT_TEXT_COLOR_DARK : cDEFAULT_TEXT_COLOR_LIGHT);
   mStateText.setParentItem(this);
   mStateText.setPos(boundingRect().center().x() - mStateText.boundingRect().center().x(),
                     mStateText.boundingRect().center().y());
   setRect(rect().x(), rect().y(), mWidth, rect().height());
}

void RvBAT::GraphicsState::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
   bool isDarkTheme = (QIcon::themeName() == "Dark") ? true : false;
   // Create a linear gradient pen for the outline of the node
   QPen pen = QPen();

   // Pen for drawing active lines
   QPen runningPen = QPen();
   runningPen.setWidthF(2.2);
   runningPen.setColor(isDarkTheme ? cRUNNING_PEN_COLOR_DARK : cRUNNING_PEN_COLOR_LIGHT);

   // Create a linear gradient
   QLinearGradient g = QLinearGradient();
   g.setStart(mWidth, 0);
   g.setFinalStop(mWidth, mHeight);

   // Color the node based on success
   // Set the gradient colors for the background of the node.
   if (mRunState == RunState::cACTIVE)
   {
      g.setColorAt(0, isDarkTheme ? cRUNNING_GRADIENT_TOP_DARK : cRUNNING_GRADIENT_LOW_LIGHT);
      g.setColorAt(1, isDarkTheme ? cRUNNING_GRADIENT_BOTTOM_DARK : cRUNNING_GRADIENT_HIGH_LIGHT);
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

   // Set the gradient colors for the outline of the node.
   if (mRunState == RunState::cACTIVE)
   {
      g.setColorAt(0, isDarkTheme ? cRUNNING_OUTLINE_GRADIENT_TOP_DARK : cRUNNING_OUTLINE_GRADIENT_TOP_LIGHT);
      g.setColorAt(1, isDarkTheme ? cRUNNING_OUTLINE_GRADIENT_BOTTOM_DARK : cRUNNING_OUTLINE_GRADIENT_BOTTOM_LIGHT);
   }
   else
   {
      g.setColorAt(0, isDarkTheme ? cDEFAULT_OUTLINE_GRADIENT_TOP_DARK : cDEFAULT_OUTLINE_GRADIENT_TOP_LIGHT);
      g.setColorAt(1, isDarkTheme ? cDEFAULT_OUTLINE_GRADIENT_BOTTOM_DARK : cDEFAULT_OUTLINE_GRADIENT_BOTTOM_LIGHT);
   }

   // Set the brush width and set the pen to the painter, for drawing the rect.
   pen.setBrush(QBrush(g));
   pen.setWidthF(2);
   painter->setPen(pen);
   painter->drawEllipse(0, 0, mWidth, mHeight);

   // Draw our attribute icons.  For FSMs this is if the state has a child ABT and/or FSM
   if (mChildABT_Id != 0)
   {
      QPixmap treePixmap =
         QPixmap::fromImage(QImage(QString::fromStdString(":/images/" + QIcon::themeName().toStdString() + "/60x60/root"))
                               .scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      double abtPadding = mChildFSM_Id != 0 ? treePixmap.rect().width() : treePixmap.rect().width() * 0.5;
      painter->drawPixmap(QPointF(boundingRect().center().x() - abtPadding, 10), treePixmap);
   }

   if (mChildFSM_Id != 0)
   {
      QPixmap fsmPixmap =
         QPixmap::fromImage(QImage(QString::fromStdString(":/images/" + QIcon::themeName().toStdString() + "/60x60/fsm"))
                               .scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      double fsmPadding = mChildABT_Id != 0 ? fsmPixmap.rect().width() * 0.5 : -fsmPixmap.rect().width() * 0.5;
      painter->drawPixmap(QPointF(boundingRect().center().x() + fsmPadding, 10), fsmPixmap);
   }
}
