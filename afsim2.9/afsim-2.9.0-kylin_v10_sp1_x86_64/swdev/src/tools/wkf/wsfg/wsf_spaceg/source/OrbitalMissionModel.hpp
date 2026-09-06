// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ORBITALMISSIONMODEL_HPP
#define ORBITALMISSIONMODEL_HPP

#include "wsf_spaceg_export.h"

#include <memory>

#include <QAbstractItemModel>
#include <QBrush>

#include "EventOrbitDetails.hpp"
#include "OrbitalMissionData.hpp"
#include "OrbitalMissionItem.hpp"

namespace wsfg
{

namespace spaceg
{

class MissionVerificationResults;

//! The representation of the mission sequence used by the Astrolabe UI.
class WSF_SPACEG_EXPORT OrbitalMissionModel : public QAbstractItemModel
{
   Q_OBJECT

public:
   OrbitalMissionModel();
   ~OrbitalMissionModel() override;

   QModelIndex parent(const QModelIndex& aChild) const override;
   QVariant    data(const QModelIndex& aIndex, int aRole = Qt::DisplayRole) const override;
   QVariant    headerData(int aSection, Qt::Orientation aOrientation, int aRole = Qt::DisplayRole) const override;
   int         columnCount(const QModelIndex& aParent) const override;
   int         rowCount(const QModelIndex& aParent) const override;

   QModelIndex index(int aRow, int aColumn, const QModelIndex& aParent) const override;

   bool HasAnyModifications() const;
   void RejectAllModifications();
   void SaveAllModifications();

   bool IndexIsFirstChild(const QModelIndex& aIndex) const;
   bool IndexIsLastChild(const QModelIndex& aIndex) const;
   bool IndexIsRemovable(const QModelIndex& aIndex) const;
   bool IndexIsTransformable(const QModelIndex& aIndex) const;

   int IndexDepth(const QModelIndex& aIndex) const;

   QModelIndex AppendEvent(const QString& aType);
   QModelIndex MoveEventUp(const QModelIndex& aIndex);
   QModelIndex MoveEventDown(const QModelIndex& aIndex);
   bool        RemoveEvent(const QModelIndex& aIndex);
   bool        TransformEvent(const QModelIndex& aIndex, const QString& aType);

   bool        SetItemData(const QModelIndex& aIndex, const QJsonObject& aData);
   void        SetModifiedData(const QModelIndex& aIndex, const QJsonObject& aData);
   QJsonObject GetItemData(const QModelIndex& aIndex) const;

   bool    IsItemModified(const QModelIndex& aIndex) const;
   void    RevertItem(const QModelIndex& aIndex);
   QString GetItemEventType(const QModelIndex& aIndex) const;

   void AppendItem(const OrbitalMissionItem& aItem, const QModelIndex& aParent);

   QModelIndex GetSequenceIndex() const;
   QModelIndex GetIndexFromOrderedIndex(int aOrderedIndex) const;

   void SetMissionVerificationResults(const MissionVerificationResults& aResults);
   void SetVerificationResults(const QModelIndex& aIndex, const VerificationResults& aResults);
   void ClearVerificationResultsAfter(const QModelIndex& aIndex);

   void BeginInsert(OrbitalMissionItem& aParent, int aIndex, int aCount = 1);
   void EndInsert();
   void BeginRemove(OrbitalMissionItem& aParent, int aIndex, int aCount = 1);
   void EndRemove();

   OrbitalMissionData GetMissionData() const;

   //! Traverse the model items, invoking an operation on each.
   //!
   //! The provided @p aOperation will be invoked on each item in the
   //! model, starting with the given @p aIndex. This is a pre-order
   //! traversal, and the order of the event will be provided to the
   //! operation, along with the per-order index for the parent of the
   //! event, and the row of that event in its parent. The provided
   //! @p aOperation can be any callable construct compatible with the
   //! following signature: void (const QModelIndex& aModelIndex, int aOrderedIndex, int aParent, int aRow,
   //! OrbitalMissionItem* aItemPtr).
   template<typename Callable>
   void Traverse(Callable& aOperation, QModelIndex aIndex = QModelIndex{}) const
   {
      if (!aIndex.isValid())
      {
         aIndex = GetSequenceIndex();
      }
      int orderedIndex{0};
      TraverseHelper(aIndex, aOperation, orderedIndex, -1, 0);
   }

   EventOrbitDetails GetEventOrbitDetails(const QModelIndex& aIndex) const;

private:
   OrbitalMissionItem* GetItemFromIndex(const QModelIndex& aIndex) const;
   bool                RemoveAllChildren(const QModelIndex& aIndex);
   void                SetupCompoundChildren(const QModelIndex& aIndex);

   void        SetRoot(OrbitalMissionItem* aItemPtr);
   QModelIndex GetRowIndex(OrbitalMissionItem& aItem);

   template<typename Callable>
   void TraverseHelper(const QModelIndex& aIndex,
                       Callable&          aOperation,
                       int&               aOrderedIndex,
                       int                aParentOrderedIndex,
                       int                aRowIndex) const
   {
      int indexHere = aOrderedIndex;
      if (aIndex.isValid())
      {
         OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
         aOperation(aIndex, aOrderedIndex, aParentOrderedIndex, aRowIndex, itemPtr);
         ++aOrderedIndex;
      }
      if (hasChildren(aIndex))
      {
         int numRows = rowCount(aIndex);
         for (int row = 0; row < numRows; ++row)
         {
            TraverseHelper(index(row, 0, aIndex), aOperation, aOrderedIndex, indexHere, row);
         }
      }
   }

   OrbitalMissionItem* mRootPtr;
   bool                mDirty;
};

} // namespace spaceg

} // namespace wsfg

#endif // ORBITALMISSIONMODEL_HPP
