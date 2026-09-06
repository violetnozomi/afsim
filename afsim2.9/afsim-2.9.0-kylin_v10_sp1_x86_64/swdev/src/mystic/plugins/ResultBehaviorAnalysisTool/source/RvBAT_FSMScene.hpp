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

#ifndef RVBAT_FSMSCENE_HPP
#define RVBAT_FSMSCENE_HPP

#include <QMap>

#include "RvBAT_Interface.hpp"
#include "RvBAT_Scene.hpp"
#include "RvEventPipeClasses.hpp"
#include "UtVec2.hpp"

namespace RvBAT
{
using Graph = QMap<size_t, std::vector<size_t>>;

/** Distribute vertices equally on a 1.0 radius circle
 * @param aG the graph to use
 * @param aPositions positions to fill
 */
void Circle(const Graph& aG, QMap<size_t, UtVec2d>& aPositions);

/** Center and scale vertices so the graph fits on a canvas of given dimensions
 * @param aG the graph to use
 * @param aWidth width to scale to
 * @param aHeight height to scale to
 * @param aPositions positions to set
 */
void CenterAndScale(const Graph& aG, unsigned int aWidth, unsigned int aHeight, QMap<size_t, UtVec2d>& aPositions);

class FruchtermanReingold
{
public:
   explicit FruchtermanReingold(const Graph& aGraph, double aK = 15.0);
   void operator()(QMap<size_t, UtVec2d>& aPositions);

private:
   const Graph&                           mGraph;    ///< Graph to run the layout over
   const double                           mK;        ///< Optimal distance between vertices
   const double                           mKSquared; ///< mK squared
   double                                 mTemp;     ///< Temperature that controls displacement
   std::vector<std::pair<size_t, size_t>> mEdges;    ///< Vector of edge pairs
   QMap<size_t, UtVec2d>                  mMvmts;    ///< Map of vertex movements
};

class GraphicsState;
class FSMScene : public Scene
{
   Q_OBJECT

public:
   explicit FSMScene(const size_t aSceneId, const SceneType aSceneType, const FSM aFSM, QObject* aParent);
   ~FSMScene();

   /** Get the rect representing the scene. */
   const QRectF GetSceneRect() const override;

   /** Redraw the visuals of the scene. */
   void RedrawVisuals() override;

   /** Reset the layout for the scene. */
   void ResetLayout() override;

   /** Update the states/transitions using the appropriate state message.
    * @param aState state message to read from
    */
   void UpdateState(const rv::MsgStateMachineState* aState);

   /** Return whether or not an object is at a point and
    * return the point if so.
    * @param aPointF point object to populate
    */
   const bool IsObjectAtPoint(const QPointF& aPointF) const override;

private:
   /** Applies the Freuchterman Reingold algorithm to layout the graph in a given
    * width and height with a certain amount of iterations.
    * @param aG graph to use for layout
    * @param aWidth width to target
    * @param aHeight height to target
    * @param aItersCount iterations to max at
    * @param aK optimal distance between vertices
    */
   QMap<size_t, UtVec2d> LayoutFruchtermanReingold(const Graph& aG,
                                                   unsigned int aWidth,
                                                   unsigned int aHeight,
                                                   unsigned int aItersCount = 300,
                                                   double       aK          = 15.0);

   QMap<size_t, GraphicsState*> mStates; ///< Map of ids to the matching GraphicsState
};
} // namespace RvBAT
#endif // RVBAT_FSMSCENE_HPP
