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

#include "OrbitalMissionItem.hpp"

#include "OrbitalMissionModel.hpp"
#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "WsfSpaceVerifyTypes.hpp"

namespace // {anonymous}
{

const QString cEVENT_HEADER{"Event Type"};
const QString cVERIFICATION_STATUS{"Verification"};
const QString cEVENT_TIME{"Date"};
const QString cEVENT_SECONDS{"Sim. Time"};
const QString cDELTA_V_HEADER{"Delta V"};
const QString cDELTA_V_BUDGET{"Delta V Budget"};
const QString cOPTION_KEY{"option"};

} // namespace

namespace wsfg
{

namespace spaceg
{

OrbitalMissionItem::OrbitalMissionItem(const QJsonObject& aData, OrbitalMissionItem* aParentPtr)
   : mSavedData{aData}
   , mModifiedData{}
   , mResults{}
   , mParentPtr{aParentPtr}
   , mModelPtr{nullptr}
   , mIndex{-1}
   , mChildren{}
{
}

//! Return the number of defined data columns.
int OrbitalMissionItem::ColumnCount()
{
   return 6;
}

//! Get the column data for various roles.
QVariant OrbitalMissionItem::ColumnData(const QModelIndex& aIndex, int aRole) const
{
   QVariant retval{};
   int      col = aIndex.column();
   if (aRole == Qt::DisplayRole)
   {
      switch (col)
      {
      case 0:
         retval = GetEventType(true);
         break;
      case 1:
         retval = GetItemVerificationStatus();
         break;
      case 2:
         retval = GetItemDeltaV();
         break;
      case 3:
         retval = GetItemDeltaV_Budget();
         break;
      case 4:
         retval = GetItemEventSeconds();
         break;
      case 5:
         retval = GetItemEventTime();
         break;
      }
   }
   else if (aRole == Qt::DecorationRole)
   {
      if (col == 0)
      {
         if (mResults.VerificationAttempted())
         {
            UtColor color = mResults.GetPreviewColor();
            int     red   = color[0] * 255.0;
            int     green = color[1] * 255.0;
            int     blue  = color[2] * 255.0;
            retval        = QColor(red, green, blue);
         }
         else
         {
            retval = QColor(0, 0, 0, 0);
         }
      }
   }
   else if (aRole == Qt::TextAlignmentRole)
   {
      if (col == 2 || col == 3 || col == 4)
      {
         retval = Qt::AlignRight;
      }
      else
      {
         retval = Qt::AlignLeft;
      }
   }
   else if (aRole == Qt::ToolTipRole)
   {
      if (col == 1)
      {
         retval = GetItemVerifyLog();
      }
   }
   else if (aRole == Qt::ForegroundRole)
   {
      if (col == 1)
      {
         retval = GetItemVerifyBrush();
      }
   }
   return retval;
}

//! Return the child at the given @p aIndex of this item.
OrbitalMissionItem* OrbitalMissionItem::GetChild(int aIndex) const
{
   return mChildren[aIndex];
}

//! Return the number of children of this item.
int OrbitalMissionItem::NumChildren() const
{
   return ut::safe_cast<int>(mChildren.size());
}

//! Return which child this item is of its parent.
int OrbitalMissionItem::WhichChild(const OrbitalMissionItem* aChildPtr) const
{
   int retval{-1};
   for (int i = 0; i < NumChildren(); ++i)
   {
      if (GetChild(i) == aChildPtr)
      {
         retval = i;
         break;
      }
   }
   return retval;
}

//! Add a child to this item.
//!
//! Add a child with the given @p aData at the given @p aIndex to this item.
//!
//! \param aData  - the data to use to initialize the added item.
//! \param aIndex - the position of the added child among this item's list of children.
void OrbitalMissionItem::AddChild(const QJsonObject aData, int aIndex)
{
   OrbitalMissionItem* addedPtr = new OrbitalMissionItem{aData, this};
   AddItem(addedPtr, aIndex);
}

//! Remove the child at the given @p aIndex.
void OrbitalMissionItem::RemoveChild(int aIndex)
{
   auto itemPtr = RemoveItem(aIndex);
   delete itemPtr;
}

//! Remove all children of this item.
void OrbitalMissionItem::RemoveAllChildren()
{
   if (!mChildren.empty())
   {
      if (IsInModel())
      {
         mModelPtr->BeginRemove(*this, 0, NumChildren());
      }
      for (auto child : mChildren)
      {
         delete child;
      }
      mChildren.clear();
      if (IsInModel())
      {
         mModelPtr->EndRemove();
      }
   }
}

//! Return the type of orbital event this item represents.
QString OrbitalMissionItem::GetEventType(bool aShowModification) const
{
   QString retval{};
   auto    iter = mSavedData.constFind(cOPTION_KEY);
   if (iter != mSavedData.constEnd())
   {
      QString eventType = iter->toString();
      retval            = eventType.toUpper();
      if (aShowModification && IsModified())
      {
         retval += QString(" ~");
      }
   }
   return retval;
}

//! Return if this item has been modified.
bool OrbitalMissionItem::IsModified() const
{
   return !mModifiedData.isEmpty() && mModifiedData != mSavedData;
}

//! Return if this item or any of its descendants are modified.
bool OrbitalMissionItem::IsAnyModified() const
{
   bool retval{IsModified()};
   for (int i = 0; i < NumChildren(); ++i)
   {
      retval = retval || GetChild(i)->IsAnyModified();
   }
   return retval;
}

//! Return the saved item data.
//!
//! The saved item data is the data used to initialize the object, or
//! the data which has been explicitly saved to the item.
QJsonObject OrbitalMissionItem::GetSavedData() const
{
   return mSavedData;
}

//! Return the current item data.
//!
//! If the item has not been modified, this will return the same data
//! as GetSavedData. If the item has been modified, then this will
//! instead return the modified data.
QJsonObject OrbitalMissionItem::GetData() const
{
   if (!mModifiedData.isEmpty())
   {
      return mModifiedData;
   }
   else
   {
      return mSavedData;
   }
}

//! Set the saved data for this item.
bool OrbitalMissionItem::SetSavedData(const QJsonObject& aData)
{
   mSavedData = aData;
   ResetModifications();
   return true;
}

//! Set the modified data for this item.
bool OrbitalMissionItem::SetData(const QJsonObject& aData)
{
   mModifiedData = aData;
   return true;
}

// Reset the modified data in this object.
void OrbitalMissionItem::ResetModifications()
{
   mModifiedData = QJsonObject{};
}

//! Reset the modified data in this object and all of its descendants.
void OrbitalMissionItem::ResetAllModifications()
{
   for (int i = 0; i < NumChildren(); ++i)
   {
      GetChild(i)->ResetAllModifications();
   }
   ResetModifications();
}

//! Save the modified data.
void OrbitalMissionItem::SaveModifications()
{
   mSavedData = mModifiedData;
   ResetModifications();
}

//! Save the modified data of this object and all of its descendants.
void OrbitalMissionItem::SaveAllModifications()
{
   for (int i = 0; i < NumChildren(); ++i)
   {
      GetChild(i)->SaveAllModifications();
   }
   if (IsModified())
   {
      SaveModifications();
   }
}

//! Move a child of this item.
//!
//! This will move the child item at @p aChildIndex to @p aFinalIndex.
//!
//! \param aChildIndex - the index of the child to move.
//! \param aFinalIndex - the index for the final location of the moved child. This index is
//!                      counted as if the child at @p aChildIndex has been removed.
void OrbitalMissionItem::MoveChild(int aChildIndex, int aFinalIndex)
{
   auto* itemPtr = RemoveItem(aChildIndex);
   AddItem(itemPtr, aFinalIndex);
}

//! Set the model for which this item is an item.
bool OrbitalMissionItem::SetModel(OrbitalMissionModel* aModelPtr)
{
   bool completed = BeginSetModel(aModelPtr);
   if (completed)
   {
      mIndex = 0;
      FinalizeSetModel();
   }
   return completed;
}

//! Return the header for the given column.
QString OrbitalMissionItem::ColumnHeader(int aColumn)
{
   QString retval{};
   switch (aColumn)
   {
   case 0:
      retval = cEVENT_HEADER;
      break;
   case 1:
      retval = cVERIFICATION_STATUS;
      break;
   case 2:
      retval = cDELTA_V_HEADER;
      break;
   case 3:
      retval = cDELTA_V_BUDGET;
      break;
   case 4:
      retval = cEVENT_SECONDS;
      break;
   case 5:
      retval = cEVENT_TIME;
      break;
   }
   return retval;
}

//! Clear the verification results for this item, and all of its children.
void OrbitalMissionItem::ClearVerificationResults()
{
   mResults.Clear();
   for (int i = 0; i < NumChildren(); ++i)
   {
      GetChild(i)->ClearVerificationResults();
   }
}

//! Clear the verification results for the given child, and all subsequent children.
//!
//! This will clear the verification results of the given @p aChild of this item,
//! as well as all subsequent children. When cleared, all descendants of the
//! cleared children will also be cleared.
//!
//! \param aChild - the child at which to be clearing verification results.
void OrbitalMissionItem::ClearVerificationResultsAfter(int aChild)
{
   for (int i = aChild; i < NumChildren(); ++i)
   {
      GetChild(i)->ClearVerificationResults();
   }
}

OrbitalMissionItem* OrbitalMissionItem::RemoveItem(int aIndex)
{
   int                 idx     = aIndex;
   OrbitalMissionItem* itemPtr = mChildren[idx];
   if (IsInModel())
   {
      mModelPtr->BeginRemove(*this, idx);
   }

   mChildren.erase(mChildren.begin() + idx);
   ReIndexChildren(idx);
   itemPtr->mParentPtr = nullptr;
   itemPtr->mIndex     = -1;
   itemPtr->mModelPtr  = nullptr;

   if (IsInModel())
   {
      mModelPtr->EndRemove();
   }

   return itemPtr;
}

QString OrbitalMissionItem::GetItemVerifyLog() const
{
   QString     retval{};
   std::string log = mResults.GetOutputLog();
   if (!log.empty())
   {
      retval = QString::fromStdString(log);
   }
   return retval;
}

QBrush OrbitalMissionItem::GetItemVerifyBrush() const
{
   QBrush retval{Qt::SolidPattern};
   switch (mResults.GetStatus())
   {
   case wsf::space::verify::Status::cNONE:
      // Empty
      break;
   case wsf::space::verify::Status::cUNVERIFIABLE:
      retval.setColor(Qt::yellow);
      break;
   case wsf::space::verify::Status::cFAIL:
      retval.setColor(Qt::red);
      break;
   case wsf::space::verify::Status::cPASS:
      retval.setColor(Qt::darkGreen);
      break;
   }
   return retval;
}

QString OrbitalMissionItem::GetItemVerificationStatus() const
{
   return QString::fromStdString(VerificationStatusDescriptor(mResults.GetStatus()));
}

QString OrbitalMissionItem::GetItemEventTime() const
{
   QString retval{};
   if (mResults.VerificationAttempted())
   {
      retval = QString::fromStdString(mResults.GetEventTime().GetISO8601());
   }
   return retval;
}

QString OrbitalMissionItem::GetItemEventSeconds() const
{
   QString retval{};
   if (mResults.VerificationAttempted())
   {
      retval = QString::number(mResults.GetEventSeconds(), 'f', 3) + " s";
   }
   return retval;
}

QString OrbitalMissionItem::GetItemDeltaV() const
{
   QString retval{};
   if (mResults.VerificationAttempted())
   {
      retval = QString::number(mResults.GetDeltaV(), 'f', 2) + QString(" m/s");
   }
   return retval;
}

QString OrbitalMissionItem::GetItemDeltaV_Budget() const
{
   QString retval{};
   if (mResults.VerificationAttempted())
   {
      retval = QString::number(mResults.GetDeltaV_Budget(), 'f', 2) + QString(" m/s");
   }
   return retval;
}

void OrbitalMissionItem::AddItem(OrbitalMissionItem* aItemPtr, int aIndex)
{
   if (aIndex == -1)
   {
      aIndex = ut::safe_cast<int, size_t>(mChildren.size());
   }

   if (IsInModel())
   {
      mModelPtr->BeginInsert(*this, aIndex);
   }
   aItemPtr->mParentPtr = this;
   aItemPtr->mIndex     = aIndex;
   bool addedToModel    = aItemPtr->BeginSetModel(mModelPtr);
   mChildren.insert(mChildren.begin() + aIndex, aItemPtr);
   ReIndexChildren(aIndex + 1);
   if (IsInModel())
   {
      mModelPtr->EndInsert();
   }
   if (addedToModel)
   {
      aItemPtr->FinalizeSetModel();
   }
}

bool OrbitalMissionItem::BeginSetModel(OrbitalMissionModel* aModelPtr)
{
   bool retval{false};
   if (mModelPtr != aModelPtr)
   {
      mModelPtr = aModelPtr;
      if (mModelPtr)
      {
         retval = true;
      }
   }
   return retval;
}

void OrbitalMissionItem::FinalizeSetModel()
{
   if (mModelPtr && !mChildren.empty())
   {
      mModelPtr->BeginInsert(*this, 0, NumChildren());
      for (auto&& it : mChildren)
      {
         it->BeginSetModel(mModelPtr);
      }
      mModelPtr->EndInsert();
      for (auto&& it : mChildren)
      {
         it->FinalizeSetModel();
      }
   }
}

void OrbitalMissionItem::ReIndexChildren(int aFirstIndex)
{
   for (int i = aFirstIndex; i < ut::safe_cast<int, size_t>(mChildren.size()); ++i)
   {
      mChildren[i]->mIndex = i;
   }
}

} // namespace spaceg

} // namespace wsfg
