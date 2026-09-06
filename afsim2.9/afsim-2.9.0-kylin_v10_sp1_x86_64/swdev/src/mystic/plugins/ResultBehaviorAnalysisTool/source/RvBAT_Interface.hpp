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

#ifndef RVBAT_INTERFACE_HPP
#define RVBAT_INTERFACE_HPP

#include <QMap>
#include <QMutex>
#include <QObject>

#include "RvEventPipeClasses.hpp"

namespace rv
{
class ResultData;
}

namespace RvBAT
{
/** Struct that holds mappings to messages, creating a behavior tree. */
struct BehaviorTree
{
   std::string                        mName{""};       ///< Name of the tree
   QMap<size_t, rv::BehaviorTreeNode> mChildren;       ///< Children (node references)
   rv::MsgBehaviorTreeState*          mState{nullptr}; ///< Current tree state
};

/** Struct that holds the necessary data to draw a FSM. This includes the states and
 * their transitions as well as the current execution state.
 */
struct FSM
{
   QMap<size_t, rv::State>   mStates;            ///< Mapping of states
   rv::MsgStateMachineState* mState{nullptr};    ///< Current FSM state
   size_t                    mInitialStateIndex; ///< Initial State index
};

class Interface : public QObject
{
   Q_OBJECT

public:
   explicit Interface(QObject* parent);
   ~Interface() override = default;

   /** Get the behavior trees from the event pipe data, and populate our member vars. */
   const QMap<size_t, QMap<size_t, RvBAT::BehaviorTree>>& GetBehaviorTrees() const { return mTrees; }

   /** Get the FSMs from the event pipe data, and populate our member vars. */
   const QMap<size_t, QMap<size_t, RvBAT::FSM>>& GetFSMs() const { return mFSMs; }

   /** Return if the behavior tool has been loaded. */
   bool IsLoaded() const { return mBehaviorToolLoaded; }

   /** Advance time and update using ResultData.
    * @param aData data to update with
    */
   void AdvanceTimeRead(const rv::ResultData& aData);

   /** Called when Plugin::ClearScenario is called. */
   void Reset();

   /** Return a string for a tree by Id if it exists.
    * @param aId Id to search for
    */
   const QString GetTreeNameFromId(const size_t aId) const;

private:
   mutable QMutex mMutex;                                  ///< Mutex
   bool           mBehaviorToolLoaded{false};              ///< Flag for if the behavior tool has been loaded
   QMap<size_t, QMap<size_t, RvBAT::BehaviorTree>> mTrees; ///< Map of platform indices to their map of trees
   QMap<size_t, QMap<size_t, RvBAT::FSM>>          mFSMs;  ///< Map of platform indices to their map of fsms
};
} // namespace RvBAT
#endif // RVBAT_INTERFACE_HPP
