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

#include "WkfFilterObject.hpp"

#include <stdexcept>

#include "UtMemory.hpp"

namespace WkfFilter
{
bool Rule::Predicate(const QAbstractItemModel* /*aModel*/, int /*aColumn*/) const
{
   return true;
}

FilterObject::FilterObject(const Rule* aRule /*= nullptr*/)
   : mRule(aRule)
{
}

bool FilterObject::IsValid() const noexcept
{
   if (!mEnabled)
   {
      return false;
   }

   if (IsCondition())
   {
      return mColumn >= 0;
   }
   else
   {
      for (const auto& child : mChildren)
      {
         if (child->IsValid())
         {
            return true;
         }
      }
   }

   return false;
}

bool FilterObject::IsCondition() const noexcept
{
   return mType != Type::And && mType != Type::Or;
}

bool FilterObject::IsReachable() const noexcept
{
   if (mParent == nullptr)
   {
      return IsValid();
   }
   else if (mParent->IsCondition())
   {
      return false;
   }
   else
   {
      return IsValid() && mParent->IsReachable();
   }
}

QString FilterObject::GetName() const
{
   if (mRule != nullptr)
   {
      return mRule->Name();
   }
   return QString();
}

const FilterObject* FilterObject::GetParent() const noexcept
{
   return mParent;
}

bool FilterObject::TestRow(const QAbstractItemModel* aModel, int aRow) const
{
   if (aModel == nullptr)
   {
      return false;
   }
   else if (!IsValid())
   {
      return true;
   }

   switch (mType)
   {
   case Type::And:
      return TestRowAnd(aModel, aRow) ^ mInverted;
   case Type::Or:
      return TestRowOr(aModel, aRow) ^ mInverted;
   case Type::Condition:
   default:
      return TestRowCondition(aModel, aRow) ^ mInverted;
   }
}

FilterObject& FilterObject::AddChild(const Rule* aRule)
{
   InsertChild(ut::make_unique<FilterObject>(aRule));
   return *mChildren.back();
}

void FilterObject::InsertChild(FilterPtr aPtr)
{
   if (aPtr == nullptr)
   {
      throw std::logic_error("WkfFilter::FilterObject cannot insert a null child.");
   }
   else if (aPtr->mParent != nullptr)
   {
      throw std::logic_error("WkfFilter::FilterObject cannot insert child that already has a parent.");
   }
   else
   {
      aPtr->mParent = this;
      mChildren.emplace_back(std::move(aPtr));
   }
}

void FilterObject::RemoveChild(FilterObject* aChild)
{
   for (auto it = mChildren.begin(); it != mChildren.end(); it++)
   {
      if (it->get() == aChild)
      {
         mChildren.erase(it);
         return;
      }
   }
}

int FilterObject::ChildCount() const noexcept
{
   return static_cast<int>(mChildren.size());
}

FilterObject* FilterObject::GetChild(int aIndex)
{
   return mChildren.at(aIndex).get();
}

const FilterObject* FilterObject::GetChild(int aIndex) const
{
   return mChildren.at(aIndex).get();
}

FilterPtr FilterObject::Clone() const
{
   FilterPtr retval = ut::make_unique<FilterObject>(mRule);

   retval->mType = mType;

   retval->mEnabled  = mEnabled;
   retval->mInverted = mInverted;

   retval->mColumn = mColumn;
   retval->mValue  = mValue;

   for (const auto& child : mChildren)
   {
      retval->InsertChild(child->Clone());
   }

   return retval;
}

bool FilterObject::TestRowCondition(const QAbstractItemModel* aModel, int aRow) const
{
   if (mRule != nullptr)
   {
      return mRule->TestValue(aModel->index(aRow, mColumn).data(), mValue);
   }
   return false;
}

bool FilterObject::TestRowAnd(const QAbstractItemModel* aModel, int aRow) const
{
   for (const auto& child : mChildren)
   {
      if (child->IsValid() && !child->TestRow(aModel, aRow))
      {
         return false;
      }
   }
   return true;
}

bool FilterObject::TestRowOr(const QAbstractItemModel* aModel, int aRow) const
{
   for (const auto& child : mChildren)
   {
      if (child->IsValid() && child->TestRow(aModel, aRow))
      {
         return true;
      }
   }
   return false;
}
} // namespace WkfFilter
