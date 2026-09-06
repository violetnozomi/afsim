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

#include "RvBAT_FSMScene.hpp"

#include <QGraphicsView>
#include <QtMath>

#include "RvBAT_GraphicsState.hpp"

RvBAT::FruchtermanReingold::FruchtermanReingold(const RvBAT::Graph& aGraph, double aK)
   : mGraph(aGraph)
   , mK(aK)
   , mKSquared(aK * aK)
   , mTemp(10 * sqrt(aGraph.size()))
   , mMvmts()
{
}

void RvBAT::FruchtermanReingold::operator()(QMap<size_t, UtVec2d>& aPositions)
{
   // Repulsion force between vertex pairs
   for (auto vertexId : mGraph.keys())
   {
      for (auto otherVertexId : mGraph.keys())
      {
         if (vertexId == otherVertexId)
         {
            continue;
         }

         UtVec2d delta    = aPositions[vertexId] - aPositions[otherVertexId];
         double  distance = delta.Normalize();

         // > 1000.0: not worth computing
         if (distance > 1000.0)
         {
            continue;
         }

         double repulsion = mKSquared / distance;

         mMvmts[vertexId] += delta / distance * repulsion;
         mMvmts[otherVertexId] -= delta / distance * repulsion;
      }

      // Attraction force between edges
      for (size_t adjId : mGraph.value(vertexId))
      {
         if (adjId > vertexId)
         {
            continue;
         }

         UtVec2d delta    = aPositions[vertexId] - aPositions[adjId];
         double  distance = delta.Normalize();
         if (distance == 0.0)
         {
            continue;
         }

         double attraction = distance * distance / mK;

         mMvmts[vertexId] -= delta / distance * attraction;
         mMvmts[adjId] += delta / distance * attraction;
      }
   }

   // Max movement capped by current temperature
   for (size_t vertexId = 0; vertexId < static_cast<size_t>(mGraph.size()); vertexId++)
   {
      double mvmtNorm = mMvmts[vertexId].Normalize();
      // < 1.0: not worth computing
      if (mvmtNorm < 1.0)
      {
         continue;
      }
      double  cappedMvmtNorm = std::min(mvmtNorm, mTemp);
      UtVec2d cappedMvmt     = mMvmts[vertexId] / mvmtNorm * cappedMvmtNorm;

      aPositions[vertexId] += cappedMvmt;
   }

   // Cool down fast until we reach 1.5, then stay at low temperature
   if (mTemp > 1.5)
   {
      mTemp *= 0.85;
   }
   else
   {
      mTemp = 1.5;
   }
}

RvBAT::FSMScene::FSMScene(const size_t aSceneId, const RvBAT::SceneType aSceneType, const RvBAT::FSM aFSM, QObject* aParent)
   : Scene(aSceneId, aSceneType, aParent)
{
   // Create the graphic items that go on the scene for each state
   for (auto& state : aFSM.mStates)
   {
      auto graphicsState          = new GraphicsState(state, QRect(0, 0, 140, 100));
      mStates[state.stateIndex()] = graphicsState;

      addItem(graphicsState);
   }

   // Loop back through and create the graph object for laying out the
   // positions of each state
   Graph fsm;
   for (auto& state : aFSM.mStates)
   {
      fsm[state.stateIndex()] = std::vector<size_t>();
      if (state.transitions().size() > 0)
      {
         for (auto& t : state.transitions())
         {
            if (t != state.stateIndex())
            {
               mStates[state.stateIndex()]->AddTransition(t, mStates[t]);
               fsm[state.stateIndex()].push_back(t);
            }
         }
      }
   }

   // Create the positions map and run the Reingold algorithm
   QMap<size_t, UtVec2d> positions;
   auto                  v = dynamic_cast<QGraphicsView*>(parent());
   double                w = v->width();
   double                h = v->height();
   positions               = LayoutFruchtermanReingold(fsm, w, h);

   // After the algorithm has run, set the positions for the states
   for (auto state : positions.keys())
   {
      mStates[state]->setPos(QPointF(positions.value(state)[0], positions.value(state)[1]));
   }
}

RvBAT::FSMScene::~FSMScene()
{
   // GraphicStates are owned by the QGraphicsScene, it will delete them for us so don't try to double delete them
   mStates.clear();
}

const QRectF RvBAT::FSMScene::GetSceneRect() const
{
   double x1 = INT_MAX;
   double x2 = INT_MIN;
   double y1 = INT_MAX;
   double y2 = INT_MIN;
   for (auto state : mStates)
   {
      if (state->isVisible())
      {
         // If this is further left, set the new x1
         if (state->scenePos().x() < x1)
         {
            x1 = state->scenePos().x();
         }
         // If this is further right, set the new x2
         if ((state->scenePos().x() + state->GetWidth()) > x2)
         {
            x2 = state->scenePos().x() + state->GetWidth();
         }
         // If this is the top-most state, set the new y1
         if (state->scenePos().y() < y1)
         {
            y1 = state->scenePos().y();
         }
         // If this is the bottom-most state, set the new y2
         if ((state->scenePos().y() + state->GetHeight()) > y2)
         {
            y2 = state->scenePos().y() + state->GetHeight();
         }
      }
   }
   return QRectF(x1, y1, (x2 - x1), (y2 - y1));
}

void RvBAT::FSMScene::RedrawVisuals()
{
   // Update the nodes to match the current theme
   for (auto state : mStates)
   {
      state->UpdateTextColor();
   }
}

void RvBAT::FSMScene::ResetLayout()
{
   // Loop back through and create the graph object for laying out the
   // positions of each state
   Graph fsm;
   for (auto& stateIndex : mStates.keys())
   {
      auto transitions = mStates.value(stateIndex)->GetTransitions();
      fsm[stateIndex]  = std::vector<size_t>();
      if (transitions.size() > 0)
      {
         for (auto& t : transitions)
         {
            fsm[stateIndex].push_back(t);
         }
      }
   }

   // Create the positions map and run the Reingold algorithm
   QMap<size_t, UtVec2d> positions;
   auto                  v = dynamic_cast<QGraphicsView*>(parent());
   double                w = v->width();
   double                h = v->height();
   positions               = LayoutFruchtermanReingold(fsm, w, h);

   // After the algorithm has run, set the positions for the states
   for (auto state : positions.keys())
   {
      mStates[state]->setPos(QPointF(positions.value(state)[0], positions.value(state)[1]));
   }
}

void RvBAT::FSMScene::UpdateState(const rv::MsgStateMachineState* aState)
{
   if (aState != nullptr)
   {
      // If we can find both states from the message
      if ((mStates.find(aState->oldStateIndex()) != mStates.end() || aState->oldStateIndex() == 0) &&
          mStates.find(aState->newStateIndex()) != mStates.end())
      {
         // Reset the visuals before loading the state
         for (auto& state : mStates)
         {
            state->SetState(RunState::cIDLE);
            state->SetIdleTransitions();
         }

         // If the old state just transitioned to the new state, set the transition arrow to active
         if (mStates.find(aState->oldStateIndex()) != mStates.end())
         {
            mStates.value(aState->oldStateIndex())->SetActiveTransition(aState->newStateIndex());
         }
         // Set the new state as active
         mStates.value(aState->newStateIndex())->SetState(RunState::cACTIVE);
      }
   }
   else
   {
      // Reset the visuals before loading the state
      for (auto& state : mStates)
      {
         state->SetState(RunState::cIDLE);
         state->SetIdleTransitions();
      }
   }
}

const bool RvBAT::FSMScene::IsObjectAtPoint(const QPointF& aPointF) const
{
   // If an object is at the point return it
   for (auto item : items(aPointF))
   {
      auto state = dynamic_cast<GraphicsState*>(item);
      if (state != nullptr)
      {
         return true;
      }
   }
   return false;
}

QMap<size_t, UtVec2d> RvBAT::FSMScene::LayoutFruchtermanReingold(const RvBAT::Graph& aG,
                                                                 unsigned int        aWidth,
                                                                 unsigned int        aHeight,
                                                                 unsigned int        aItersCount,
                                                                 double              aK)
{
   // Create our container for state positions
   QMap<size_t, UtVec2d> positions;

   // Initial layout on a circle
   Circle(aG, positions);

   // Run the layout until it completes
   FruchtermanReingold fr(aG, aK);
   for (unsigned int i = 0; i < aItersCount; i++)
   {
      fr(positions);
   }

   // Center and scale the layout to 90% of the screen
   CenterAndScale(aG, aWidth, aHeight, positions);

   return positions;
}

void RvBAT::Circle(const RvBAT::Graph& aG, QMap<size_t, UtVec2d>& aPositions)
{
   // Position all of the graph positions in a circle
   double angle = 2.0 * M_PI / aG.size();
   for (auto vertexId : aG.keys())
   {
      aPositions[vertexId][0] = cos(vertexId * angle);
      aPositions[vertexId][1] = sin(vertexId * angle);
   }
}

void RvBAT::CenterAndScale(const RvBAT::Graph& aG, unsigned int aWidth, unsigned int aHeight, QMap<size_t, UtVec2d>& aPositions)
{
   // Find current dimensions
   double xMin = std::numeric_limits<double>::max();
   double xMax = std::numeric_limits<double>::lowest();
   double yMin = std::numeric_limits<double>::max();
   double yMax = std::numeric_limits<double>::lowest();

   // Find our min and maxes
   for (auto vertexId : aG.keys())
   {
      if (aPositions[vertexId][0] < xMin)
      {
         xMin = aPositions[vertexId][0];
      }
      if (aPositions[vertexId][0] > xMax)
      {
         xMax = aPositions[vertexId][0];
      }

      if (aPositions[vertexId][1] < yMin)
      {
         yMin = aPositions[vertexId][1];
      }
      if (aPositions[vertexId][1] > yMax)
      {
         yMax = aPositions[vertexId][1];
      }
   }

   double curWidth  = xMax - xMin;
   double curHeight = yMax - yMin;

   // Compute the scale factor (0.9: keep some margin)
   double xScale = aWidth / curWidth;
   double yScale = aHeight / curHeight;
   double scale  = 0.9 * (xScale < yScale ? xScale : yScale);

   // Compute the offset and apply it to every position
   UtVec2d center(xMax + xMin, yMax + yMin);
   UtVec2d offset = center / 2.0 * scale;
   for (auto vertexId : aG.keys())
   {
      aPositions[vertexId] = aPositions[vertexId] * scale - offset;
   }
}
