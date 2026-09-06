// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPKTABLE_HPP
#define WSFPKTABLE_HPP

#include <ostream>
#include <vector>

class UtInput;
#include "UtAzElLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtCallbackHolder.hpp"
#include "UtInput.hpp"
class UtInputBlock;
#include "UtUnits.hpp"
#include "WsfNamed.hpp"
#include "WsfStringId.hpp"

namespace WsfPk
{
//! Collection of tables to determine Pk for a given engagement.  The primary
//! index (which determine which table to use) is:  target_type.  After a
//! table is chosen, then that table (which _may_ contain sub-tables) is either
//! interpolated, or "binned" to obtain Pk:  Pk = f(Az, El, Speed, TgtSpd).
//! It is assumed that the caller _always_ provides his own UtAzElLookup instance.
//!
//! From a data perspective, all Pk tables are static globals, and constant
//! pointers to them will be served to the engagement application.  No table
//! copies will be made.

class TgtMaxSpdAndTable
{
public:
   TgtMaxSpdAndTable(double aTgtMaxSpeed, UtAzElTable& aTable);
   static bool  LoadTgtMaxSpdAndTable(UtInput& aInput, TgtMaxSpdAndTable*& aTablePtr);
   bool         ProcessInput(UtInput& aInput);
   double       TgtSpeedLimit() const { return mTgtMaxSpeed; }
   double       MinEl() const { return mTable.mElValues.Get(0); }
   double       MaxEl() const { return mTable.mElValues.Get(mTable.mElValues.GetSize() - 1); }
   double       MinAz() const { return mTable.mAzValues.Get(0); }
   double       MaxAz() const { return mTable.mAzValues.Get(mTable.mAzValues.GetSize() - 1); }
   size_t       NumAzimuths() const { return mTable.mAzValues.GetSize(); }
   size_t       NumElevations() const { return mTable.mElValues.GetSize(); }
   UtAzElTable* GetTable() { return &mTable; }

private:
   double      mTgtMaxSpeed;
   UtAzElTable mTable;
};

class WpnSpdSet
{
public:
   WpnSpdSet(double aWpnMaxSpeed);
   bool               ProcessInput(UtInput& aInput);
   static bool        LoadWpnSpdSet(UtInput& aInput, WpnSpdSet*& aWpnSpdSetPtr);
   TgtMaxSpdAndTable& Get(int aIndex) { return mTgtMaxSpdAndTables[aIndex % mTgtMaxSpdAndTables.size()]; }
   size_t             Size() { return mTgtMaxSpdAndTables.size(); }
   bool               Add(const TgtMaxSpdAndTable& aNewTableSet);
   double             WpnSpeedLimit() const { return mWpnMaxSpeed; }
   void GetTgtSpdSetsFor(double aTargetSpeed, TgtMaxSpdAndTable*& aLowerPtr, TgtMaxSpdAndTable*& aUpperPtr);

private:
   double                         mWpnMaxSpeed;
   std::vector<TgtMaxSpdAndTable> mTgtMaxSpdAndTables;
};

class Table : public WsfNamed
{
public:
   Table();
   ~Table() override = default;

   static bool LoadTable(UtInput& aInput, Table*& aTablePtr, Table* aParentTable = nullptr);

   static bool LoadUtAzElTable(UtInput& aInput, UtAzElTable*& aTablePtr);

   virtual bool Add(Table* aTablePtr) { return false; }

   virtual double LookUp(WsfStringId aTargetTypeId, double aAzimuth, double aElevation, double aWeaponSpeed, double aTargetSpeed);

   virtual WsfStringId TargetType() { return mTargetType; }

   bool ProcessInput(UtInput& aInput);

   void       SetTargetType(WsfStringId aTargetType) { mTargetType = aTargetType; }
   WpnSpdSet& Get(int aIndex) { return mWpnSpdSets[aIndex % mWpnSpdSets.size()]; }
   size_t     SpeedsSize() { return mWpnSpdSets.size(); }

   virtual bool        CanAdd() const { return true; }
   virtual bool        IsTableSet() const { return false; }
   virtual size_t      TableSetSize() const { return 1; }
   virtual WsfStringId TargetTypeOfIndex(unsigned int aIndex);

protected:
   void SetPkValuesP(const std::vector<std::vector<double>>& aValues);
   bool Add(const WpnSpdSet& aWpnSpdSet);
   void GetWpnSpdSetsFor(double aWeaponSpeed, WpnSpdSet*& aLowerPtr, WpnSpdSet*& aUpperPtr);

   int  Resize();
   void Increment(int& i, int& j, int& k, int& m, int& total);

   // Prevent use of operator= by declaring, but not defining.
   Table& operator=(const Table& aRhs) = delete;

   // ========== Class Attributes ====================
   WsfStringId            mTargetType;
   std::vector<WpnSpdSet> mWpnSpdSets;

}; // end class

class TableSet : public Table
{
public:
   TableSet();
   ~TableSet() override;

   // virtual bool Initialize();
   bool Add(Table* aTablePtr) override;

   double LookUp(WsfStringId aTargetType, double aAzimuth, double aElevation, double aWeaponSpeed, double aTargetSpeed) override;

   bool ProcessInput(UtInput& aInput);

   bool IsTableSet() const override { return true; }

   const Table& Get(int aTgtTypeId) const;

   bool        CanAdd() const override;
   WsfStringId TargetTypeOfIndex(unsigned int aIndex) override;
   size_t      TableSetSize() const override { return mTablePtrs.size(); }

protected:
   // ========== Class Attributes ====================
   std::vector<Table*> mTablePtrs;

}; // end class

class TableManager
{
public:
   TableManager();
   ~TableManager();

   // There are two ways to find a table, either by name, or by key.
   Table* FindTargetType(WsfStringId aTargetType);
   Table* FindTableName(WsfStringId aNameId);

   bool ProcessInput(UtInput& aInput);

private:
   bool Add(Table* aTablePtr);

   UtCallbackHolder    mCallbacks;
   std::vector<Table*> mTables;
};

} // end namespace WsfPk

#endif
