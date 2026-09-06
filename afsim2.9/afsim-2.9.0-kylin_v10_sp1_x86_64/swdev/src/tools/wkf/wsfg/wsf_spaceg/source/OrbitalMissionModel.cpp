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

#include "OrbitalMissionModel.hpp"

#include "AstrolabeConfig.hpp"
#include "MissionVerificationResults.hpp"
#include "OrbitalMissionItem.hpp"

namespace wsfg
{

namespace spaceg
{

OrbitalMissionModel::OrbitalMissionModel()
   : QAbstractItemModel{}
   , mRootPtr{new OrbitalMissionItem{QJsonObject{}}}
   , mDirty{false}
{
   SetRoot(mRootPtr);
   mRootPtr->AddChild(AstrolabeConfig::DefaultEventProperties("SEQUENCE"));
}

OrbitalMissionModel::~OrbitalMissionModel()
{
   SetRoot(nullptr);
   delete mRootPtr;
}

QModelIndex OrbitalMissionModel::parent(const QModelIndex& aChild) const
{
   // Note that the internal pointers for the model indices in this model point
   // to the parent of the data item.
   QModelIndex retval{};
   if (aChild.isValid())
   {
      OrbitalMissionItem* parentPtr = static_cast<OrbitalMissionItem*>(aChild.internalPointer());
      if (parentPtr && parentPtr->GetParent())
      {
         retval = createIndex(parentPtr->GetIndex(), 0, parentPtr->GetParent());
      }
   }
   return retval;
}

QVariant OrbitalMissionModel::data(const QModelIndex& aIndex, int aRole) const
{
   QVariant            retval{};
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
   if (itemPtr)
   {
      retval = itemPtr->ColumnData(aIndex, aRole);
   }
   return retval;
}

QVariant OrbitalMissionModel::headerData(int aSection, Qt::Orientation aOrientation, int aRole) const
{
   QVariant retval{};
   if (aOrientation == Qt::Horizontal && aRole == Qt::DisplayRole)
   {
      retval = OrbitalMissionItem::ColumnHeader(aSection);
   }
   return retval;
}

int OrbitalMissionModel::columnCount(const QModelIndex& aParent) const
{
   return OrbitalMissionItem::ColumnCount();
}

int OrbitalMissionModel::rowCount(const QModelIndex& aParent) const
{
   int                       retval{0};
   const OrbitalMissionItem* itemPtr = GetItemFromIndex(aParent);
   if (itemPtr)
   {
      retval = itemPtr->NumChildren();
   }
   return retval;
}

QModelIndex OrbitalMissionModel::index(int aRow, int aColumn, const QModelIndex& aParent) const
{
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aParent);
   return createIndex(aRow, aColumn, itemPtr);
}

//! Return if there are any modifications to the model.
bool OrbitalMissionModel::HasAnyModifications() const
{
   return mDirty || mRootPtr->IsAnyModified();
}

//! Reset all mission items to their unmodified state.
void OrbitalMissionModel::RejectAllModifications()
{
   mRootPtr->ResetAllModifications();
}

//! Save modifications on all mission items.
void OrbitalMissionModel::SaveAllModifications()
{
   mRootPtr->SaveAllModifications();
}

//! Return if the given model index is the first child of its parent.
bool OrbitalMissionModel::IndexIsFirstChild(const QModelIndex& aIndex) const
{
   return aIndex.isValid() && aIndex.row() == 0;
}

//! Return if the given model index is the last child of its parent.
bool OrbitalMissionModel::IndexIsLastChild(const QModelIndex& aIndex) const
{
   QModelIndex         parent    = aIndex.parent();
   OrbitalMissionItem* parentPtr = GetItemFromIndex(parent);
   return parentPtr != nullptr && aIndex.row() == parentPtr->NumChildren() - 1;
}

//! Return if the given model index represents an item than can be removed.
//!
//! Certain mission items cannot be removed, they are: the overall mission sequence,
//! and children of compound maneuvers.
bool OrbitalMissionModel::IndexIsRemovable(const QModelIndex& aIndex) const
{
   return aIndex.parent().isValid() && !aIndex.parent().parent().isValid();
}

//! Return if the given model index represents an item that can be transformed.
//!
//! Only the top level mission sequence item is not transformable.
bool OrbitalMissionModel::IndexIsTransformable(const QModelIndex& aIndex) const
{
   return aIndex.parent().isValid();
}

//! Return the depth of the given index in the sequence tree.
int OrbitalMissionModel::IndexDepth(const QModelIndex& aIndex) const
{
   int         retval{0};
   QModelIndex current = aIndex;
   while (current.isValid())
   {
      ++retval;
      current = current.parent();
   }
   return retval;
}

//! Add an event of the given type to the mission sequence.
//!
//! This will add an event of the given @p aType to the end of the mission
//! sequence. The model index of the added event will be returned if the
//! item was successfully added. Otherwise, an invalid model index is returned.
//!
//! \param aType - The type of event to append to the mission.
//! \returns     - The model index of the added event if successful; an invalid index otherwise.
QModelIndex OrbitalMissionModel::AppendEvent(const QString& aType)
{
   QModelIndex         retval{};
   int                 count     = rowCount(GetSequenceIndex());
   OrbitalMissionItem* parentPtr = GetItemFromIndex(GetSequenceIndex());
   if (parentPtr != nullptr)
   {
      parentPtr->AddChild(AstrolabeConfig::DefaultEventProperties(aType));
      retval = index(count, 0, GetSequenceIndex());

      if (aType == "COMPOUND")
      {
         SetupCompoundChildren(retval);
      }
   }

   return retval;
}

//! Move the event with the given index earlier in the sequence.
//!
//! This will swap the order of the event with the given @p aIndex with the
//! immediately preceding event. This will return the new index of the
//! moved event, even if that event was not moved (in which case the
//! return value will equal @p aIndex).
//!
//! \param aIndex - The index of the event to move earlier in the sequence.
//! \returns      - The index of the final location of the event.
QModelIndex OrbitalMissionModel::MoveEventUp(const QModelIndex& aIndex)
{
   QModelIndex retval{aIndex};
   if (!IndexIsFirstChild(aIndex))
   {
      QModelIndex         parentIndex = aIndex.parent();
      OrbitalMissionItem* parentPtr   = GetItemFromIndex(parentIndex);
      if (parentPtr != nullptr)
      {
         parentPtr->MoveChild(aIndex.row(), aIndex.row() - 1);
         retval = index(aIndex.row() - 1, 0, parentIndex);
         ClearVerificationResultsAfter(retval);
         mDirty = true;
      }
   }
   return retval;
}

//! Move the event with the given index later in the sequence.
//!
//! This will swap the order if the event with the given @p aIndex with the
//! event immediately following it. This will return the new index of the
//! moved event, even if that event was not moved (in which case the return
//! value will equal @p aIndex).
//!
//! \param aIndex - The index of the event to move later in the sequence.
//! \returns      - The index of the final location of the event.
QModelIndex OrbitalMissionModel::MoveEventDown(const QModelIndex& aIndex)
{
   QModelIndex retval{aIndex};
   if (!IndexIsLastChild(aIndex))
   {
      QModelIndex         parentIndex = aIndex.parent();
      OrbitalMissionItem* parentPtr   = GetItemFromIndex(parentIndex);
      if (parentPtr != nullptr)
      {
         parentPtr->MoveChild(aIndex.row(), aIndex.row() + 1);
         retval = index(aIndex.row() + 1, 0, parentIndex);
         ClearVerificationResultsAfter(aIndex);
         mDirty = true;
      }
   }
   return retval;
}

//! Remove the event with the given index.
//!
//! This will remove the event with the given @p aIndex if it is removable.
//!
//! \param aIndex - The index of the event to remove.
//! \returns      - true if the event was successfully removed; false otherwise.
bool OrbitalMissionModel::RemoveEvent(const QModelIndex& aIndex)
{
   bool retval{false};
   if (IndexIsRemovable(aIndex))
   {
      QModelIndex         parentIndex = aIndex.parent();
      OrbitalMissionItem* parentPtr   = GetItemFromIndex(parentIndex);
      if (parentPtr != nullptr && aIndex.row() >= 0 && aIndex.row() < parentPtr->NumChildren())
      {
         auto* removedPtr = parentPtr->RemoveItem(aIndex.row());
         delete removedPtr;
         ClearVerificationResultsAfter(aIndex);
         mDirty = true;
         retval = true;
      }
   }
   return retval;
}

//! Transform the event with the given index into an event with the given type.
//!
//! This will transform the event with the given @p aIndex into being an event with
//! the given @p aType. There are some transformations that are disallowed.
//! A member of a compound maneuver cannot itself be transformed into a compound
//! maneuver.
//!
//! \param aIndex - The index of the event to transform.
//! \param aType  - The type of event into which the event with the given @p aIndex should be transformed.
//! \returns      - true if successfully transformed; false otherwise.
bool OrbitalMissionModel::TransformEvent(const QModelIndex& aIndex, const QString& aType)
{
   bool retval{false};

   // Cannot make a member of a compound event a compound event
   if (aIndex.isValid() && (aType != "COMPOUND" || GetItemEventType(aIndex.parent()) != "COMPOUND"))
   {
      QString currentType = GetItemEventType(aIndex);

      if (currentType != aType)
      {
         SetItemData(aIndex, AstrolabeConfig::DefaultEventProperties(aType));
         SetModifiedData(aIndex, AstrolabeConfig::DefaultEventProperties(aType));
         retval = true;

         if (currentType == "COMPOUND")
         {
            retval = RemoveAllChildren(aIndex);
         }
         else if (aType == "COMPOUND")
         {
            SetupCompoundChildren(aIndex);
         }

         ClearVerificationResultsAfter(aIndex);
      }
   }
   return retval;
}

//! Set the data for the given item.
//!
//! This will set the data for the event with the given @p aIndex to be equal to the
//! given @p aData. This is the saved data of the event, and so represents the 'original'
//! state of the event before any modifications.
//!
//! \param aIndex - The index of the event for which data will be set.
//! \param aData  - The data to set.
//! \returns      - true if successful; false otherwise.
bool OrbitalMissionModel::SetItemData(const QModelIndex& aIndex, const QJsonObject& aData)
{
   bool                retval{false};
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
   if (itemPtr != nullptr)
   {
      retval = itemPtr->SetSavedData(aData);
      emit dataChanged(aIndex, aIndex);
   }
   return retval;
}

//! Set the modified data for the given item.
//!
//! This will set the modified data for the event with the given @p aIndex to be equal to
//! the given @p aData. This is the modified data of the event, and so represents any
//! changes to the event provided through Astrolabe's UI.
//!
//! \param aIndex - The index of the event for the modified data will be set.
//! \param aData  - The data to set.
//! \returns      - true if successful; false otherwise.
void OrbitalMissionModel::SetModifiedData(const QModelIndex& aIndex, const QJsonObject& aData)
{
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
   if (itemPtr != nullptr)
   {
      auto before = itemPtr->GetData();
      itemPtr->SetData(aData);
      if (before != itemPtr->GetData())
      {
         emit dataChanged(aIndex, aIndex);
         ClearVerificationResultsAfter(aIndex);
      }
   }
}

//! Get the data for the given item.
QJsonObject OrbitalMissionModel::GetItemData(const QModelIndex& aIndex) const
{
   QJsonObject         retval{};
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
   if (itemPtr != mRootPtr)
   {
      retval = itemPtr->GetData();
   }
   return retval;
}

//! Return if the item has been modified from its original state.
bool OrbitalMissionModel::IsItemModified(const QModelIndex& aIndex) const
{
   bool                retval{false};
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
   if (itemPtr != mRootPtr)
   {
      retval = itemPtr->IsModified();
   }
   return retval;
}

//! Revert the given item to its original state.
void OrbitalMissionModel::RevertItem(const QModelIndex& aIndex)
{
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
   if (itemPtr != nullptr)
   {
      itemPtr->ResetModifications();
      emit dataChanged(aIndex, aIndex);
      ClearVerificationResultsAfter(aIndex);
   }
}

//! Return the item's event type.
//!
//! This will return the type of the event with the given @p aIndex. Additionally,
//! if @p aShowModification is true, this will append an indicator that the item has
//! been modified with the type.
//!
//! \param aIndex            - The index of the event in question.
//! \returns                 - The event type, with optionals marker indicating if the event has been modified.
QString OrbitalMissionModel::GetItemEventType(const QModelIndex& aIndex) const
{
   QString             retval{};
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
   if (itemPtr != nullptr)
   {
      retval = itemPtr->GetEventType(false);
   }
   return retval;
}

//! Append the given item to the given parent.
//!
//! Append the given @p aItem as a child of the event with the given @p aParent index.
//!
//! \param aItem   - The mission event to append.
//! \param aParent - The index of the event to which the given @p aItem will be appended.
void OrbitalMissionModel::AppendItem(const OrbitalMissionItem& aItem, const QModelIndex& aParent)
{
   if (aParent.isValid())
   {
      int                 row       = rowCount(aParent);
      OrbitalMissionItem* parentPtr = GetItemFromIndex(aParent);
      if (parentPtr != nullptr)
      {
         parentPtr->AddChild(aItem.GetSavedData());

         QModelIndex addedIndex = index(row, 0, aParent);
         for (int i = 0; i < aItem.NumChildren(); ++i)
         {
            AppendItem(*aItem.GetChild(i), addedIndex);
         }
      }
   }
}

//! Return the index of the overall mission sequence.
QModelIndex OrbitalMissionModel::GetSequenceIndex() const
{
   return index(0, 0, QModelIndex{});
}

//! Return the model index for the event with the given ordered index.
//!
//! This will return the model index for the event that has the index
//! @p OrderedIndex in a pre-order traversal.
QModelIndex OrbitalMissionModel::GetIndexFromOrderedIndex(int aOrderedIndex) const
{
   QModelIndex retval{};

   auto operation =
      [&retval, aOrderedIndex](const QModelIndex& aIndex, int aLoopIndex, int aParent, int aRow, OrbitalMissionItem* aItemPtr)
   {
      if (aLoopIndex == aOrderedIndex)
      {
         retval = aIndex;
      }
   };
   Traverse(operation);

   return retval;
}

//! Set the verification results for the entire mission.
void OrbitalMissionModel::SetMissionVerificationResults(const MissionVerificationResults& aResults)
{
   auto mapOp =
      [this, &aResults](const QModelIndex& aIndex, int aOrderedIndex, int aParent, int aRow, OrbitalMissionItem* aItemPtr)
   { SetVerificationResults(aIndex, aResults.GetResults(aOrderedIndex)); };
   Traverse(mapOp);
}

//! Set the verification results for the given event.
//!
//! This will set the verification results to be @p aResults for the event with the given
//! @p aIndex. If there is no such event, then this operation does nothing.
//!
//! \param aIndex   - The index of the event of interest.
//! \param aResults - The verification results to set for the event of interest.
void OrbitalMissionModel::SetVerificationResults(const QModelIndex& aIndex, const VerificationResults& aResults)
{
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
   if (itemPtr != nullptr)
   {
      itemPtr->SetVerificationResults(aResults);
      QModelIndex finalIndex = aIndex.sibling(aIndex.row(), 1);
      emit        dataChanged(aIndex, finalIndex);
   }
}

//! Clear the verification results for the given event, and all subsequent events with the same parent.
//!
//! This will clear the verification results for the event with the given @p aIndex and all
//! subsequence events sharing a parent with the given event. Any event having its verification
//! results cleared will also have any of their children's verification results cleared.
//!
//! \param aIndex - The index of the event of interest.
void OrbitalMissionModel::ClearVerificationResultsAfter(const QModelIndex& aIndex)
{
   if (aIndex.isValid())
   {
      // figure out which child of the parent is this. Then call the method on the parent.
      OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
      if (itemPtr != nullptr)
      {
         OrbitalMissionItem* parentPtr = itemPtr->GetParent();
         int                 which     = parentPtr->WhichChild(itemPtr);
         if (which != -1)
         {
            parentPtr->ClearVerificationResultsAfter(which);
            QModelIndex finalIndex = index(parentPtr->NumChildren() - 1, 1, aIndex.parent());
            emit        dataChanged(aIndex, finalIndex);
         }
         if (IndexDepth(aIndex) == 3)
         {
            ClearVerificationResultsAfter(aIndex.parent());
         }
      }
   }
}

void OrbitalMissionModel::BeginInsert(OrbitalMissionItem& aParent, int aIndex, int aCount)
{
   beginInsertRows(GetRowIndex(aParent), aIndex, aIndex + aCount - 1);
}

void OrbitalMissionModel::EndInsert()
{
   endInsertRows();
}

void OrbitalMissionModel::BeginRemove(OrbitalMissionItem& aParent, int aIndex, int aCount)
{
   beginRemoveRows(GetRowIndex(aParent), aIndex, aIndex + aCount - 1);
}

void OrbitalMissionModel::EndRemove()
{
   endRemoveRows();
}

//! Return the mission data for this model.
//!
//! The mission data is equivalent to the mission model, but without any reference to
//! the actual instance of the OrbitalMissionModel. Instead of indexing with QModelIndex,
//! the OrbitalMissionData indexes with the pre-order traversal index of the events.
//!
//! \returns - The OrbitalMissionData equivalent to this model.
OrbitalMissionData OrbitalMissionModel::GetMissionData() const
{
   OrbitalMissionData retval{};
   auto               operation =
      [&retval](const QModelIndex& aIndex, int aOrderedIndex, int aParent, int aRow, OrbitalMissionItem* aItemPtr)
   { retval.SetItemData(aOrderedIndex, aItemPtr->GetData(), aParent, aRow); };
   Traverse(operation);
   return retval;
}

EventOrbitDetails OrbitalMissionModel::GetEventOrbitDetails(const QModelIndex& aIndex) const
{
   EventOrbitDetails   retval{};
   OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
   if (itemPtr && itemPtr->VerificationAttempted())
   {
      retval.mElementsBefore = itemPtr->GetElementsBefore();
      retval.mElementsAfter  = itemPtr->GetElementsAfter();
      retval.mSimTimeBefore  = itemPtr->GetEventSimTimeBefore();
      retval.mSimTimeAfter   = itemPtr->GetEventSimTimeAfter();
      retval.mPreviewColor   = itemPtr->GetPreviewColor();
      retval.mEventName      = itemPtr->GetEventType(false).toStdString();
   }
   return retval;
}

//! Return the mission event given the index of that event.
OrbitalMissionItem* OrbitalMissionModel::GetItemFromIndex(const QModelIndex& aIndex) const
{
   OrbitalMissionItem* retvalPtr{nullptr};
   if (aIndex.isValid())
   {
      OrbitalMissionItem* itemPtr = static_cast<OrbitalMissionItem*>(aIndex.internalPointer());
      if (itemPtr && aIndex.row() < itemPtr->NumChildren())
      {
         retvalPtr = itemPtr->GetChild(aIndex.row());
      }
   }
   else
   {
      retvalPtr = mRootPtr;
   }
   return retvalPtr;
}

//! Remove all children of the event with the given index.
bool OrbitalMissionModel::RemoveAllChildren(const QModelIndex& aIndex)
{
   bool retval{false};
   if (aIndex != GetSequenceIndex())
   {
      OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
      if (itemPtr != nullptr)
      {
         itemPtr->RemoveAllChildren();
         retval = true;
      }
   }
   return retval;
}

//! Setup default children for a compound maneuver.
void OrbitalMissionModel::SetupCompoundChildren(const QModelIndex& aIndex)
{
   if (aIndex != GetSequenceIndex())
   {
      OrbitalMissionItem* itemPtr = GetItemFromIndex(aIndex);
      if (itemPtr != nullptr)
      {
         itemPtr->AddChild(AstrolabeConfig::DefaultEventProperties("TANGENT"));
         itemPtr->AddChild(AstrolabeConfig::DefaultEventProperties("NORMAL"));
      }
   }
}

void OrbitalMissionModel::SetRoot(OrbitalMissionItem* aItemPtr)
{
   if (mRootPtr)
   {
      mRootPtr->SetModel(nullptr);
      mRootPtr = nullptr;
   }
   if (aItemPtr)
   {
      mRootPtr = aItemPtr;
      mRootPtr->SetModel(this);
   }
}

QModelIndex OrbitalMissionModel::GetRowIndex(OrbitalMissionItem& aItem)
{
   QModelIndex retval{};
   if (&aItem != mRootPtr)
   {
      retval = createIndex(aItem.GetIndex(), 0, aItem.GetParent());
   }
   return retval;
}

} // namespace spaceg

} // namespace wsfg
