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

#include "WkfFilterRules.hpp"

#include "UtMemory.hpp"

namespace WkfFilter
{
static bool PredicateNumbers(const QAbstractItemModel* aModel, int aColumn)
{
   if (aModel != nullptr)
   {
      const int rows = aModel->rowCount();

      for (int i = 0; i < rows; i++)
      {
         bool isNumber;
         aModel->index(i, aColumn).data().toDouble(&isNumber);
         if (!isNumber)
         {
            return false;
         }
      }

      return true;
   }
   return false;
}

// Equal

QString RuleEquals::Name() const
{
   return "==";
}

bool RuleEquals::TestValue(const QVariant& aModelData, const QString& aFilterData) const
{
   // a QVariant can compare directly to a QString, but a QVariant of bool type and true value will compare positively
   // to any string with any non-empty value
   return aModelData.toString() == aFilterData;
}

// Not Equal

QString RuleNotEquals::Name() const
{
   return "!=";
}

bool RuleNotEquals::TestValue(const QVariant& aModelData, const QString& aFilterData) const
{
   return aModelData.toString() != aFilterData;
}

// Less

QString RuleLessThan::Name() const
{
   return "<";
}

bool RuleLessThan::TestValue(const QVariant& aModelData, const QString& aFilterData) const
{
   return aModelData < aFilterData;
}

bool RuleLessThan::Predicate(const QAbstractItemModel* aModel, int aColumn) const
{
   return PredicateNumbers(aModel, aColumn);
}

// Less or Equal

QString RuleLessOrEqual::Name() const
{
   return "<=";
}

bool RuleLessOrEqual::TestValue(const QVariant& aModelData, const QString& aFilterData) const
{
   return aModelData <= aFilterData;
}

bool RuleLessOrEqual::Predicate(const QAbstractItemModel* aModel, int aColumn) const
{
   return PredicateNumbers(aModel, aColumn);
}

// Greater

QString RuleGreaterThan::Name() const
{
   return ">";
}

bool RuleGreaterThan::TestValue(const QVariant& aModelData, const QString& aFilterData) const
{
   return aModelData > aFilterData;
}

bool RuleGreaterThan::Predicate(const QAbstractItemModel* aModel, int aColumn) const
{
   return PredicateNumbers(aModel, aColumn);
}

// Greater or Equal

QString RuleGreaterOrEqual::Name() const
{
   return ">=";
}

bool RuleGreaterOrEqual::TestValue(const QVariant& aModelData, const QString& aFilterData) const
{
   return aModelData >= aFilterData;
}

bool RuleGreaterOrEqual::Predicate(const QAbstractItemModel* aModel, int aColumn) const
{
   return PredicateNumbers(aModel, aColumn);
}

// Contains

QString RuleContains::Name() const
{
   return "contains";
}

bool RuleContains::TestValue(const QVariant& aModelData, const QString& aFilterData) const
{
   return aModelData.toString().contains(aFilterData, Qt::CaseInsensitive);
}
} // namespace WkfFilter
