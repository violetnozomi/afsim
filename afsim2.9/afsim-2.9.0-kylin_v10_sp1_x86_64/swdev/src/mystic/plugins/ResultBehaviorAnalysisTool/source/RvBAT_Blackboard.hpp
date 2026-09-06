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

#ifndef RVBAT_BLACKBOARD_HPP
#define RVBAT_BLACKBOARD_HPP

#include <QLabel>
#include <QTreeWidget>
#include <QWidget>

namespace rv
{
class BehaviorTreeBlackboardData;
}

namespace RvBAT
{
class GraphicsNode;
class Blackboard : public QWidget
{
   Q_OBJECT

public:
   Blackboard(QWidget* aParentPtr, size_t aWidth);

   /** Return the tree widget object. */
   QTreeWidget& GetTreeWidget() { return mTree; }

   /** Set the blackboard data on the tree using a list of blackboard messages.
    * @param aMsgList list of blackboard messages to read from.
    */
   void SetBlackboardData(const QVector<rv::BehaviorTreeBlackboardData>& aMsgList);

   /** Clear the blackboard data tree. */
   void ClearBlackboardData();

   /** Set the blackboard title text.
    * @param aText text to set.
    */
   void SetBlackboardTitle(const QString& aText);

private:
   QLabel        mTitle; ///< Title text for the what blackboard is showing.
   QTreeWidget   mTree;  ///< Pointer to the tree widget that will hold blackboard data.
   GraphicsNode* mSelectedNode;
};
} // namespace RvBAT
#endif // RVBAT_BLACKBOARD_HPP
