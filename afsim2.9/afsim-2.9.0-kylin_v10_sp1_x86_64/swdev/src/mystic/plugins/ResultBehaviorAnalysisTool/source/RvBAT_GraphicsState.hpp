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

#ifndef RVBAT_GRAPHICSSTATE_HPP
#define RVBAT_GRAPHICSSTATE_HPP

#include <QGraphicsItem>
#include <QPainter>

#include "RvEventPipeClasses.hpp"

namespace RvBAT
{
/** The run status of a state at a given time.
 * IDLE - The state is not running, e.g the FSM is in another state
 * ACTIVE - The state is active, e.g the FSM is in this state
 */
enum class RunState
{
   cIDLE   = 1,
   cACTIVE = 2
};


class GraphicsArrow : public QGraphicsLineItem
{
public:
   /** Qt type definition */
   enum
   {
      Type = UserType + 1
   };

   GraphicsArrow(QGraphicsItem* aStartItem, QGraphicsItem* aEndItem, QGraphicsItem* aParent = nullptr);

   /** Return the type (Qt). */
   int type() const override { return Type; }

   /** Return the bounding rect for this object. */
   QRectF boundingRect() const override;

   /** Return the shape for this object. */
   QPainterPath shape() const override;

   /** Set the state of the transition line/arrow.
    * @param aState state to set (Idle/Active)
    */
   void SetState(const RunState aState) { mState = aState; }

   /** Return the closest point to a point given a path.
    * @param aTarget target point to calculate from
    * @param aSourcePath path to use for calculations
    */
   QPointF ClosestPointTo(const QPointF& aTarget, const QPainterPath& aSourcePath);

   /** Calculate the points required to draw the arrows correctly.
    * @param aArrowStartItem point to populate where line starts from the start item
    * @param aArrowEndItem point to populate where the arrow hits the end item
    */
   void GetArrowPoints(QPointF& aArrowStartItem, QPointF& aArrowEndItem);

   /** Update the line. */
   void updatePosition();

protected:
   /** Paint the line and arrow. */
   void paint(QPainter* aPainter, const QStyleOptionGraphicsItem* aOption, QWidget* aWidget = nullptr) override;

private:
   QGraphicsItem* mStartItem;              ///< The start item for the transition line/arrow
   QGraphicsItem* mEndItem;                ///< The end item for the transition line/arrow
   QPolygonF      mStartToEndArrow;        ///< Polygon representing the arrow from start to end
   RunState       mState{RunState::cIDLE}; ///< Current state of the transition line/arrow
};

class GraphicsState : public QGraphicsEllipseItem
{
public:
   GraphicsState(const rv::State& aState, QRectF aRect);

   /** Get the states run state. */
   const RunState GetState() const { return mRunState; }

   /** Set the states run state.
    * @param aState state to set
    */
   void SetState(const RunState aState) { mRunState = aState; }

   /** Add a transition from this state to another state.
    * @param aIndex index of the transition to add
    * @param aGraphicsState state the transition will go to
    */
   void AddTransition(const size_t aIndex, GraphicsState* aGraphicsState);

   /** Return the GraphicArrow of a transition by index.
    * @param aIndex index of transition to retrieve
    */
   GraphicsArrow* GetTransitionGraphic(const size_t aIndex);

   /** Return the list of transition indices for this state. */
   const QList<size_t> GetTransitions() const { return mTransitions.keys(); }

   /** Set the active transition by index
    * @param aTransitionIndex index of transition to set active
    */
   void SetActiveTransition(const size_t aTransitionIndex);

   /** Set the transitions to idle before setting new state. */
   void SetIdleTransitions();

   /** Update the state's text color according to the current theme. */
   void UpdateTextColor();

   /** Return the id of the child ABT for this state. */
   const size_t GetChildABT_Id() const { return mChildABT_Id; }

   /** Return the id of the child FSM for this state. */
   const size_t GetChildFSM_Id() const { return mChildFSM_Id; }

   /** Set the text that will be displayed on the state.
    * @param aText Text to set
    */
   void SetStateText(const std::string& aText);

   /** Return the width of this state. */
   const int GetWidth() const { return mWidth; }

   /** Return the height of this state. */
   const int GetHeight() const { return mHeight; }

protected:
   /** Qt - Handle a hover event (tooltip)
    * @param event hover event to handle
    */
   void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

   /** Position the state text (title), expanding the state's width if necessary. */
   void PositionStateText();

   /** Qt - Bounding rect of the state. */
   QRectF boundingRect() const override { return QRectF(0, 0, mWidth, mHeight); }

   /** Qt - This is where the object draws itself onto the scene.
    * @param painter QPainter used to paint this object
    * @param option Style options to use when painting
    * @param widget used for cached painting
    */
   void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
   size_t                       mId;                        ///< Unique id of the state.
   RunState                     mRunState{RunState::cIDLE}; ///< Run state of this state currently.
   QGraphicsTextItem            mStateText;                 ///< Text to display at the bottom middle of the state.
   int                          mWidth;                     ///< Width of the state.
   int                          mHeight;                    ///< Height of the state.
   int                          mX;                         ///< X coordinate of the state's position.
   int                          mY;                         ///< Y coordinate of the state's position.
   QMap<size_t, GraphicsArrow*> mTransitions;               ///< Map of transitions for this state
   size_t                       mChildABT_Id{0};            ///< Id of the child ABT if it exists
   size_t                       mChildFSM_Id{0};            ///< Id of the child FSM if it exists
};
} // namespace RvBAT
#endif // RVBAT_GRAPHICSSTATE_HPP
