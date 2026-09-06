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

#include "WsfPkTable.hpp"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "TblLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtDictionary.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace
{
const double cDEFAULT_WPN_SPEED = 10.0;
const double cDEFAULT_TGT_SPEED = 10.0;

const char* cDEFAULT1 = "DEFAULT";   // Preferred placeholder.
const char* cDEFAULT2 = "default";   // Alternate placeholder.
const char* cDEFAULT3 = "<unknown>"; // Second alternate placeholder.

struct Interp
{
   Interp(double aI_Lower, double aI_Upper, double aI_Value)
      : mI_Lower(aI_Lower)
      , mI_Upper(aI_Upper)
      , mI_Value(aI_Value)
   {
      mI_Span = mI_Upper - mI_Lower;
      mInterp = 0.0;
      if (mI_Span > 0.0000001)
      {
         mInterp = (mI_Value - mI_Lower) / mI_Span;
      }
      mI_Value = mI_Lower + mInterp * mI_Span;
   }
   double Get(double aDV_Lower, double aDV_Upper) const
   {
      double dSpan = (aDV_Upper - aDV_Lower);
      return aDV_Lower + mInterp * dSpan;
   }
   double mI_Lower;
   double mI_Upper;
   double mI_Span;
   double mI_Value;
   double mInterp;
};

bool IsDefault(WsfStringId aStringId)
{
   return aStringId.IsNull() || aStringId == cDEFAULT1 || aStringId == cDEFAULT2 || aStringId == cDEFAULT3;
}
} // namespace

namespace WsfPk
{

// ======================== Sub Class TableManager =======================================

// =======================================================================================
Table* TableManager::FindTargetType(WsfStringId aTargetType)
{
   // First look through the name-specific tables for a match.
   std::vector<Table*>::const_iterator it = mTables.begin();
   while (it != mTables.end())
   {
      if ((*it)->TargetType() == aTargetType)
      {
         return *it;
      }
      ++it;
   }

   // Second, allow wild card matches for Target Type.
   it = mTables.begin();
   while (it != mTables.end())
   {
      bool oneIsDefault = IsDefault((*it)->TargetType()) || IsDefault(aTargetType);
      if (oneIsDefault)
      {
         return *it;
      }
      ++it;
   }

   // Not found; return 0.
   return nullptr;
}

// =======================================================================================
Table* TableManager::FindTableName(WsfStringId aId)
{
   std::vector<Table*>::const_iterator it = mTables.begin();
   // Look through the name-specific tables for a match.
   while (it != mTables.end())
   {
      if ((*it)->GetNameId() == aId)
      {
         return *it;
      }
      ++it;
   }
   // Not found; return 0.
   return nullptr;
}

// =======================================================================================
TableManager::TableManager()
   : mCallbacks()
   , mTables()
{
}

// =======================================================================================
TableManager::~TableManager()
{
   for (size_t i = 0; i != mTables.size(); ++i)
   {
      delete mTables[i];
   }
   mTables.clear();
}

// =======================================================================================
bool TableManager::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   // string command = aInput.GetCommand();

   Table* tPtr = nullptr;
   if (Table::LoadTable(aInput, tPtr))
   {
      if (!Add(tPtr))
      {
         throw UtInput::BadValue(aInput,
                                 "Duplicate or unnamed 'pk_table' " + tPtr->GetName() +
                                    " could not be added to Pk Table Manager.  (First target type must be 'DEFAULT'.)");
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =======================================================================================
// private
bool TableManager::Add(Table* aTablePtr)
{
   // Assure that the names are present, and are unique!
   if (aTablePtr->GetNameId() == nullptr)
   {
      return false;
   }

   for (size_t i = 0; i != mTables.size(); ++i)
   {
      if (mTables[i]->GetNameId() == aTablePtr->GetNameId())
      {
         return false;
      }
   }

   // Assure that when a new table is added, IF it has more than one table,
   // the first one is for a target type = DEFAULT, and that any others
   // are NOT for "DEFAULT" target type.
   if (!aTablePtr->CanAdd())
   {
      return false;
   }

   mTables.push_back(aTablePtr);
   return true;
}

// ======================== Sub Class Table ==============================================
// =======================================================================================
Table::Table()
   : WsfNamed()
   , mTargetType(cDEFAULT1)
   , mWpnSpdSets()
{
}

// =======================================================================================
// virtual
WsfStringId Table::TargetTypeOfIndex(unsigned int /*aIndex*/)
{
   return mTargetType;
}

// =======================================================================================
bool Table::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   Table*     tablePtr     = nullptr;
   WpnSpdSet* wpnSpdSetPtr = nullptr;
   // TgtMaxSpdAndTable* tgtMaxSpdAndTablePtr = 0;
   // UtAzElTable*       azElTablePtr         = 0;
   // double             wpnSpeed             = cDEFAULT_WPN_SPEED;
   // double             tgtSpeed             = cDEFAULT_TGT_SPEED;

   if (LoadTable(aInput, tablePtr, this))
   {
      if (!Add(tablePtr))
      {
         throw UtInput::BadValue(aInput, "Cannot add table " + tablePtr->GetName() + " as it is not unique.");
      }
   }
   else if (WpnSpdSet::LoadWpnSpdSet(aInput, wpnSpdSetPtr))
   {
      if (Add(*wpnSpdSetPtr))
      {
         delete wpnSpdSetPtr;
         wpnSpdSetPtr = nullptr;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Cannot add weapon_speed set, they must be in increasing speed order.");
      }
   }
   else if (command == "constant")
   {
      double value;
      aInput.ReadValue(value);
      aInput.ValueInClosedRange(value, 0.0, 1.0);
      UtAzElTable table;
      table.CreateConstantTable(float(value));
      WpnSpdSet set(cDEFAULT_WPN_SPEED);
      set.Add(TgtMaxSpdAndTable(cDEFAULT_TGT_SPEED, table));
      if (!Add(set))
      {
         throw UtInput::BadValue(aInput, "Could not add constant pk_table.");
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =======================================================================================
bool Table::Add(const WpnSpdSet& aWpnSpdSet)
{
   // Assure that the max speeds are increasing:
   double maxWpnSpeed = -1.0;
   for (size_t i = 0; i != mWpnSpdSets.size(); ++i)
   {
      maxWpnSpeed = mWpnSpdSets[i].WpnSpeedLimit();
   }
   if (aWpnSpdSet.WpnSpeedLimit() > maxWpnSpeed)
   {
      mWpnSpdSets.push_back(aWpnSpdSet);
      return true;
   }
   return false;
}

// =======================================================================================
// static
bool Table::LoadTable(UtInput& aInput, Table*& aTablePtr, Table* aParentTablePtr)
{
   bool        loaded  = false;
   std::string command = aInput.GetCommand();

   if ((command == "pk_table") || (command == "target_type"))
   {
      loaded = true;
      if (aParentTablePtr == nullptr)
      {
         aTablePtr = new TableSet();
         std::string name;
         aInput.ReadValue(name);
         aTablePtr->SetName(name);
      }
      else if ((command == "target_type") && (aParentTablePtr != nullptr))
      {
         aTablePtr = new Table();
         std::ostringstream oss;
         oss << aParentTablePtr->GetName() << "_" << aParentTablePtr->TableSetSize();
         std::string childName(oss.str());
         aTablePtr->SetName(childName);

         std::string targetType;
         aInput.ReadValue(targetType);
         aTablePtr->SetTargetType(targetType);
      }
      else
      {
         throw;
      }

      std::string  endString = "end_" + command;
      UtInputBlock block(aInput, endString);
      while (block.ReadCommand())
      {
         UtInput& input = block.GetInput();
         if (!aTablePtr->ProcessInput(input))
         {
            throw UtInput::BadValue(input);
         }
      }
   }
   return loaded;
}

// =======================================================================================
// static
bool Table::LoadUtAzElTable(UtInput& aInput, UtAzElTable*& aTablePtr)
{
   bool        loaded  = false;
   std::string command = aInput.GetCommand();

   if (command == "inline_table")
   {
      loaded    = true;
      aTablePtr = new UtAzElTable();

      int numAzimuths;
      int numElevations;
      aInput.ReadValue(numAzimuths);
      aInput.ReadValue(numElevations);

      // Now read the elevation values (in degrees, but convert to radians):
      std::vector<float>              elevationsRad;
      std::vector<float>              azimuthsRad;
      std::vector<std::vector<float>> pkValues;
      for (int iElev = 0; iElev != numElevations; ++iElev)
      {
         double val;
         aInput.ReadValue(val);
         elevationsRad.push_back(float(val * UtMath::cRAD_PER_DEG));
      }

      // Now read in (one) azimuth value, and all the PK values associated with it:
      pkValues.resize(numAzimuths);
      for (int jAz = 0; jAz != numAzimuths; ++jAz)
      {
         pkValues[jAz].resize(numElevations);
         double val;
         aInput.ReadValue(val);
         azimuthsRad.push_back(float(val * UtMath::cRAD_PER_DEG));

         for (int kPk = 0; kPk != numElevations; ++kPk)
         {
            aInput.ReadValue(val);
            aInput.ValueInClosedRange(val, 0.0, 1.0);
            pkValues[jAz][kPk] = float(val);
         }
      }

      // Now place the values into the proper arrays in the table instance:
      aTablePtr->mAzValues.SetValues(azimuthsRad);
      aTablePtr->mElValues.SetValues(elevationsRad);
      aTablePtr->mDataValues.Resize(azimuthsRad.size(), elevationsRad.size());
      for (size_t i = 0; i != azimuthsRad.size(); ++i)
      {
         for (size_t j = 0; j != elevationsRad.size(); ++j)
         {
            aTablePtr->mDataValues.Set(pkValues[i][j], i, j);
         }
      }
   }
   else if (command == "constant")
   {
      loaded = true;
      double val;
      aInput.ReadValue(val);
      aInput.ValueInClosedRange(val, 0.0, 1.0);
      aTablePtr = new UtAzElTable();
      aTablePtr->CreateConstantTable(float(val));
   }

   return loaded;
}

// =======================================================================================
// virtual
double Table::LookUp(WsfStringId aTargetType, double aAzimuth, double aElevation, double aWeaponSpeed, double aTargetSpeed)
{
   double dv = 0.0;

   WpnSpdSet* lowerWpnSpdSetPtr;
   WpnSpdSet* upperWpnSpdSetPtr;

   GetWpnSpdSetsFor(aWeaponSpeed, lowerWpnSpdSetPtr, upperWpnSpdSetPtr);

   TgtMaxSpdAndTable* lowerWpnLowerTgtSpdSetPtr;
   TgtMaxSpdAndTable* upperWpnLowerTgtSpdSetPtr;

   TgtMaxSpdAndTable* lowerWpnUpperTgtSpdSetPtr;
   TgtMaxSpdAndTable* upperWpnUpperTgtSpdSetPtr;

   if (lowerWpnSpdSetPtr == upperWpnSpdSetPtr)
   {
      lowerWpnSpdSetPtr->GetTgtSpdSetsFor(aTargetSpeed, lowerWpnLowerTgtSpdSetPtr, lowerWpnUpperTgtSpdSetPtr);

      if (lowerWpnLowerTgtSpdSetPtr == lowerWpnUpperTgtSpdSetPtr)
      {
         // No interpolation is required, other than 2D Az/El Interp:
         UtAzElTable* tPtr = lowerWpnLowerTgtSpdSetPtr->GetTable();
         UtAzElLookup lookUp;
         lookUp.SetTable(tPtr);
         dv = lookUp.Lookup(aAzimuth, aElevation);
      }
      else
      {
         // 2D Interpolation, first on Az/El, then a 3rd Dimension target speed:

         // lower Weapon lower target:
         UtAzElTable* tPtr = lowerWpnLowerTgtSpdSetPtr->GetTable();
         UtAzElLookup lookUp;
         lookUp.SetTable(tPtr);
         double lowerWpnLowerTgtPk = lookUp.Lookup(aAzimuth, aElevation);

         // lower Weapon upper target:
         tPtr = lowerWpnUpperTgtSpdSetPtr->GetTable();
         lookUp.SetTable(tPtr);
         double lowerWpnUpperTgtPk = lookUp.Lookup(aAzimuth, aElevation);

         // Set the interpolation structure:
         Interp tgtSpeedInterp(lowerWpnLowerTgtSpdSetPtr->TgtSpeedLimit(),
                               lowerWpnUpperTgtSpdSetPtr->TgtSpeedLimit(),
                               aTargetSpeed);

         // Now return the double-interpolated result:
         dv = tgtSpeedInterp.Get(lowerWpnLowerTgtPk, lowerWpnUpperTgtPk);
      }
   }
   else // weapon speed sets are different:
   {
      lowerWpnSpdSetPtr->GetTgtSpdSetsFor(aTargetSpeed, lowerWpnLowerTgtSpdSetPtr, lowerWpnUpperTgtSpdSetPtr);

      upperWpnSpdSetPtr->GetTgtSpdSetsFor(aTargetSpeed, upperWpnLowerTgtSpdSetPtr, upperWpnUpperTgtSpdSetPtr);

      if (lowerWpnLowerTgtSpdSetPtr == upperWpnLowerTgtSpdSetPtr)
      {
         // 2D Interpolation, first on Az/El, then a 3rd Dimension weapon speed:

         // lower Weapon lower target:
         UtAzElTable* tPtr = lowerWpnLowerTgtSpdSetPtr->GetTable();
         UtAzElLookup lookUp;
         lookUp.SetTable(tPtr);
         double lowerWpnLowerTgtPk = lookUp.Lookup(aAzimuth, aElevation);

         // upper Weapon lower target:
         tPtr = upperWpnLowerTgtSpdSetPtr->GetTable();
         lookUp.SetTable(tPtr);
         double upperWpnLowerTgtPk = lookUp.Lookup(aAzimuth, aElevation);

         // Set the interpolation structure:
         Interp wpnSpeedInterp(lowerWpnSpdSetPtr->WpnSpeedLimit(), upperWpnSpdSetPtr->WpnSpeedLimit(), aWeaponSpeed);

         // Now return the double-interpolated result:
         dv = wpnSpeedInterp.Get(lowerWpnLowerTgtPk, upperWpnLowerTgtPk);
      }
      else
      {
         // 2D Interpolation, first on Az/El, then 3rd and 4th Dimension on weapon speed then target speed:

         // lower Weapon lower target:
         UtAzElTable* tPtr = lowerWpnLowerTgtSpdSetPtr->GetTable();
         UtAzElLookup lookUp;
         lookUp.SetTable(tPtr);
         double dv00 = lookUp.Lookup(aAzimuth, aElevation);

         // lower Weapon upper target:
         tPtr = lowerWpnUpperTgtSpdSetPtr->GetTable();
         lookUp.SetTable(tPtr);
         double dv01 = lookUp.Lookup(aAzimuth, aElevation);

         // upper Weapon lower target:
         tPtr = upperWpnLowerTgtSpdSetPtr->GetTable();
         lookUp.SetTable(tPtr);
         double dv10 = lookUp.Lookup(aAzimuth, aElevation);

         // upper Weapon upper target:
         tPtr = upperWpnUpperTgtSpdSetPtr->GetTable();
         lookUp.SetTable(tPtr);
         double dv11 = lookUp.Lookup(aAzimuth, aElevation);

         // Interp on target speed, constant lower weapon speed:

         Interp lowWpnTgtSpdInterp(lowerWpnLowerTgtSpdSetPtr->TgtSpeedLimit(),
                                   lowerWpnUpperTgtSpdSetPtr->TgtSpeedLimit(),
                                   aTargetSpeed);

         // Interp on target speed, constant upper weapon speed:
         Interp upperWpnTgtSpdInterp(upperWpnLowerTgtSpdSetPtr->TgtSpeedLimit(),
                                     upperWpnUpperTgtSpdSetPtr->TgtSpeedLimit(),
                                     aTargetSpeed);

         double dv0 = lowWpnTgtSpdInterp.Get(dv00, dv01);
         double dv1 = upperWpnTgtSpdInterp.Get(dv10, dv11);

         Interp wpnSpeedInterp(lowerWpnSpdSetPtr->WpnSpeedLimit(), upperWpnSpdSetPtr->WpnSpeedLimit(), aWeaponSpeed);

         dv = wpnSpeedInterp.Get(dv0, dv1);
      }
   }
   return dv;
}

// ==================================================================================================
// private
void Table::GetWpnSpdSetsFor(double aWeaponSpeed, WpnSpdSet*& aLowerPtr, WpnSpdSet*& aUpperPtr)
{
   if (mWpnSpdSets.size() == 0)
   {
      aLowerPtr = nullptr;
      aUpperPtr = nullptr;
      return;
   }
   std::vector<WpnSpdSet>::iterator it = mWpnSpdSets.begin();
   aLowerPtr                           = &(*it);
   aUpperPtr                           = &(*it);
   while (it != mWpnSpdSets.end() && it->WpnSpeedLimit() < aWeaponSpeed)
   {
      aLowerPtr = aUpperPtr;
      aUpperPtr = &(*it);
      ++it;
   }
}

// ======================== Sub Class TableSet =====================================
// =================================================================================
TableSet::TableSet()
   : Table()
   , mTablePtrs()
{
}

// =======================================================================================
TableSet::~TableSet()
{
   for (size_t i = 0; i != mTablePtrs.size(); ++i)
   {
      delete mTablePtrs[i];
   }
}

// =======================================================================================
// virtual
WsfStringId TableSet::TargetTypeOfIndex(unsigned int aIndex)
{
   return (mTablePtrs.empty()) ? WsfStringId(nullptr) : mTablePtrs[aIndex % mTablePtrs.size()]->TargetType();
}

// =======================================================================================
// virtual
bool TableSet::CanAdd() const
{
   // Assure that when a new table is added, IF it has more than one table,
   // the first one is for a target type = DEFAULT, and that any others
   // are NOT for "DEFAULT" target type.
   bool canAdd = !mTablePtrs.empty() && IsDefault(mTablePtrs[0]->TargetType());
   for (size_t i = 1; i != mTablePtrs.size(); ++i)
   {
      if (IsDefault(mTablePtrs[i]->TargetType()))
      {
         canAdd = false;
      }
   }
   return canAdd;
}

// =======================================================================================
bool TableSet::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   // string command = aInput.GetCommand();

   Table* tablePtr = nullptr;

   if (Table::LoadTable(aInput, tablePtr))
   {
      if (Add(tablePtr))
      {
      }
      else
      {
         std::string name = tablePtr->GetName();
         delete tablePtr;
         tablePtr = nullptr;
         throw UtInput::BadValue(aInput, "Could not load new table instance named " + name + ".");
      }
   }
   else
   {
      myCommand = Table::ProcessInput(aInput);
   }

   return myCommand;
}

// =======================================================================================
bool TableSet::Add(Table* aTablePtr)
{
   // Assure that the table names and target types are unique.
   for (size_t i = 0; i != mTablePtrs.size(); ++i)
   {
      if (mTablePtrs[i]->GetNameId() == aTablePtr->GetNameId())
      {
         return false;
      }

      if (mTablePtrs[i]->TargetType() == aTablePtr->TargetType())
      {
         return false;
      }
   }

   mTablePtrs.push_back(aTablePtr);
   return true;
}


// =================================================================================
// virtual
double TableSet::LookUp(WsfStringId aTargetType, double aAzimuth, double aElevation, double aWeaponSpeed, double aTargetSpeed)
{
   if (mTablePtrs.empty())
   {
      return 0.0;
   }
   else if (mTablePtrs.size() == 1)
   {
      ut::log::info() << "TableSet::LookUp() has only one default table, so is using it.";
      return mTablePtrs[0]->LookUp(aTargetType, aAzimuth, aElevation, aWeaponSpeed, aTargetSpeed);
   }
   else
   {
      // Find the right table (matching target type), and call its Lookup function:
      for (size_t i = 0; i != mTablePtrs.size(); ++i)
      {
         if (mTablePtrs[i]->TargetType() == aTargetType)
         {
            // cout << "   ... (TableSet::LookUp() is using a table with the target type matched exactly.)" << endl;
            return mTablePtrs[i]->LookUp(aTargetType, aAzimuth, aElevation, aWeaponSpeed, aTargetSpeed);
         }
      }
      ut::log::info() << "TableSet::LookUp() is using its DEFAULT table, since no target type matched exactly.";
      return mTablePtrs[0]->LookUp(aTargetType, aAzimuth, aElevation, aWeaponSpeed, aTargetSpeed);
   }
   ut::log::info() << "TableSet::LookUp() returned a null lookup value of zero.";
   return 0.0;
}

// ============================= Subclass TgtMaxSpdAndTable =========================================
// ==================================================================================================
// static
bool TgtMaxSpdAndTable::LoadTgtMaxSpdAndTable(UtInput& aInput, TgtMaxSpdAndTable*& aPtr)
{
   bool        loaded  = false;
   std::string command = aInput.GetCommand();

   if (command == "target_speed")
   {
      loaded = true;

      UtAzElTable* tPtr = nullptr;
      double       tgtSpeed;
      aInput.ReadValueOfType(tgtSpeed, UtInput::cSPEED);

      UtInputBlock block(aInput, "end_target_speed");
      while (block.ReadCommand())
      {
         UtInput& input           = block.GetInput();
         bool     azElTableLoaded = Table::LoadUtAzElTable(input, tPtr);
         if (!azElTableLoaded)
         {
            throw UtInput::BadValue(aInput,
                                    "Cannot add target_speed table, as the inline table was formatted incorrectly.");
         }
         else
         {
            // cppcheck-suppress nullPointer
            aPtr = new TgtMaxSpdAndTable(tgtSpeed, *tPtr);
            delete tPtr;
         }
      }
   }

   return loaded;
}

// ==================================================================================================
TgtMaxSpdAndTable::TgtMaxSpdAndTable(double aTgtMaxSpeed, UtAzElTable& aTable)
   : mTgtMaxSpeed(aTgtMaxSpeed)
   , mTable(aTable)
{
}

// ============================= Subclass WpnSpdSet =================================================
// ==================================================================================================
// static
bool WpnSpdSet::LoadWpnSpdSet(UtInput& aInput, WpnSpdSet*& aPtr)
{
   bool        loaded  = false;
   std::string command = aInput.GetCommand();

   double       wpnSpeed = cDEFAULT_WPN_SPEED;
   UtAzElTable* tablePtr = nullptr;

   if (command == "weapon_speed")
   {
      loaded = true;
      aInput.ReadValueOfType(wpnSpeed, UtInput::cSPEED);
      aPtr = new WpnSpdSet(wpnSpeed);
      UtInputBlock block(aInput, "end_weapon_speed");
      while (block.ReadCommand())
      {
         TgtMaxSpdAndTable* tgtTablePtr = nullptr;

         if (TgtMaxSpdAndTable::LoadTgtMaxSpdAndTable(aInput, tgtTablePtr))
         {
            if (!aPtr->Add(*tgtTablePtr))
            {
               throw UtInput::BadValue(aInput, "Cannot add target_speed group, as target speeds must be increasing.");
            }
            delete tgtTablePtr;
            tgtTablePtr = nullptr;
         }
         else
         {
            throw UtInput::BadValue(aInput,
                                    "Cannot add target_speed group, as the inline table was formatted incorrectly.");
         }
      }
   }
   else if (Table::LoadUtAzElTable(aInput, tablePtr))
   {
      // This is a pk table with no specified target speed or weapon speed.
      // Use default speeds for both:
      loaded = true;
      aPtr   = new WpnSpdSet(cDEFAULT_WPN_SPEED);
      if (!aPtr->Add(TgtMaxSpdAndTable(cDEFAULT_TGT_SPEED, *tablePtr)))
      {
         throw UtInput::BadValue(aInput, "Cannot add a <default weapon speed and default target speed inline_table, as it was formatted incorrectly.");
      }
      delete tablePtr;
   }

   return loaded;
}

// ==================================================================================================
WpnSpdSet::WpnSpdSet(double aWpnMaxSpeed)
   : mWpnMaxSpeed(aWpnMaxSpeed)
   , mTgtMaxSpdAndTables()
{
}

// ==================================================================================================
void WpnSpdSet::GetTgtSpdSetsFor(double aTargetSpeed, TgtMaxSpdAndTable*& aLowerPtr, TgtMaxSpdAndTable*& aUpperPtr)
{
   if (mTgtMaxSpdAndTables.size() == 0)
   {
      aLowerPtr = nullptr;
      aUpperPtr = nullptr;
      return;
   }
   std::vector<TgtMaxSpdAndTable>::iterator it = mTgtMaxSpdAndTables.begin();
   aLowerPtr                                   = &(*it);
   aUpperPtr                                   = &(*it);
   while (it != mTgtMaxSpdAndTables.end() && it->TgtSpeedLimit() < aTargetSpeed)
   {
      aLowerPtr = aUpperPtr;
      aUpperPtr = &(*it);
      ++it;
   }
}

// ==================================================================================================
bool WpnSpdSet::Add(const TgtMaxSpdAndTable& aTgtMaxSpdAndTable)
{
   // Assure that the target speeds are increasing:
   double maxTgtSpeed = -1.0;
   for (size_t i = 0; i != mTgtMaxSpdAndTables.size(); ++i)
   {
      maxTgtSpeed = mTgtMaxSpdAndTables[i].TgtSpeedLimit();
   }
   if (aTgtMaxSpdAndTable.TgtSpeedLimit() > maxTgtSpeed)
   {
      mTgtMaxSpdAndTables.push_back(aTgtMaxSpdAndTable);
      return true;
   }
   return false;
}

} // namespace WsfPk
