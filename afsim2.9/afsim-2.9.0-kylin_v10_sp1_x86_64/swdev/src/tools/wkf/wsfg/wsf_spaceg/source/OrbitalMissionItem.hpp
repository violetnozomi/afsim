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

#ifndef ORBITALMISSIONITEM_HPP
#define ORBITALMISSIONITEM_HPP

#include <memory>
#include <utility>
#include <vector>

#include <QJsonObject>
#include <QVariant>

#include "VerificationResults.hpp"

namespace wsfg
{

namespace spaceg
{

class OrbitalMissionModel;

//! An item in the UI's model of the mission sequence.
class OrbitalMissionItem
{
public:
   explicit OrbitalMissionItem(const QJsonObject& aData, OrbitalMissionItem* aParentPtr = nullptr);
   ~OrbitalMissionItem() = default;

   static int ColumnCount();
   QVariant   ColumnData(const QModelIndex& aIndex, int aRole) const;

   OrbitalMissionItem* GetParent() const { return mParentPtr; }

   OrbitalMissionItem* GetChild(int aIndex) const;
   bool                HasChildren() const { return NumChildren() != 0; }
   int                 NumChildren() const;
   int                 WhichChild(const OrbitalMissionItem* aChildPtr) const;
   int                 GetIndex() const { return mIndex; }

   void AddChild(const QJsonObject aData, int aIndex = -1);
   void RemoveChild(int aIndex);
   void RemoveAllChildren();

   QString GetEventType(bool aShowModification) const;

   bool        IsModified() const;
   bool        IsAnyModified() const;
   QJsonObject GetSavedData() const;
   QJsonObject GetData() const;
   bool        SetSavedData(const QJsonObject& aData);
   bool        SetData(const QJsonObject& aData);

   void ResetModifications();
   void ResetAllModifications();
   void SaveModifications();
   void SaveAllModifications();

   void MoveChild(int aChildIndex, int aFinalIndex);
   bool SetModel(OrbitalMissionModel* aModelPtr);

   static QString ColumnHeader(int aColumn);

   //! Set the verification results for this item.
   void SetVerificationResults(const VerificationResults& aResults) { mResults = aResults; }

   void ClearVerificationResults();
   void ClearVerificationResultsAfter(int aChild);

   bool              VerificationAttempted() const { return mResults.VerificationAttempted(); }
   double            GetEventSimTimeBefore() const { return mResults.GetSecondsBefore(); }
   double            GetEventSimTimeAfter() const { return mResults.GetSecondsAfter(); }
   UtOrbitalElements GetElementsBefore() const { return mResults.GetElementsBefore(); }
   UtOrbitalElements GetElementsAfter() const { return mResults.GetElementsAfter(); }
   UtColor           GetPreviewColor() const { return mResults.GetPreviewColor(); }

   OrbitalMissionItem* RemoveItem(int aIndex);

private:
   QString GetItemVerifyLog() const;
   QBrush  GetItemVerifyBrush() const;
   QString GetItemVerificationStatus() const;
   QString GetItemEventTime() const;
   QString GetItemEventSeconds() const;
   QString GetItemDeltaV() const;
   QString GetItemDeltaV_Budget() const;

   void AddItem(OrbitalMissionItem* aItemPtr, int aIndex);
   bool IsInModel() const { return mModelPtr != nullptr; }
   bool BeginSetModel(OrbitalMissionModel* aModelPtr);
   void FinalizeSetModel();
   void ReIndexChildren(int aFirstIndex);

   QJsonObject         mSavedData;
   QJsonObject         mModifiedData;
   VerificationResults mResults;

   OrbitalMissionItem*              mParentPtr;
   OrbitalMissionModel*             mModelPtr;
   int                              mIndex;
   std::vector<OrbitalMissionItem*> mChildren;
};

} // namespace spaceg

} // namespace wsfg

#endif // ORBITALMISSIONITEM_HPP
