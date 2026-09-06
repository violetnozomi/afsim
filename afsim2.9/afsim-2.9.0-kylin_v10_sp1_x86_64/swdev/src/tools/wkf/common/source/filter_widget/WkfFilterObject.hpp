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

#ifndef WKF_FILTER_OBJECT_HPP
#define WKF_FILTER_OBJECT_HPP

#include "wkf_common_export.h"

#include <memory>
#include <vector>

#include <QAbstractItemModel>
#include <QString>

class QSettings;

namespace WkfFilter
{
struct FilterObject;
struct Rule;

using FilterPtr  = std::unique_ptr<FilterObject>;
using FilterList = std::vector<FilterPtr>;

using RulePtr  = std::unique_ptr<Rule>;
using RuleList = std::vector<RulePtr>;

//! A rule for how to compare a value in the event list with a fixed value.
//! E.g. Equals, NotEquals, LessThan, etc.
struct WKF_COMMON_EXPORT Rule
{
   virtual QString Name() const                                                            = 0;
   virtual bool    TestValue(const QVariant& aModelData, const QString& aFilterData) const = 0;

   //! If false, this rule is ignored when assigning rules to filters.
   //! Default implementation returns true unconditionally.
   virtual bool Predicate(const QAbstractItemModel* aModel, int aColumn) const;
};

//! An object representation of a filter or part of a filter.
//! Can either represent a condition, or a way to combine its children to get a result.
struct WKF_COMMON_EXPORT FilterObject
{
   //! Private default (and only) constructor limits inheritance to friends.
   explicit FilterObject(const Rule* aRule = nullptr);

   FilterObject(const FilterObject&) = delete;
   FilterObject& operator=(const FilterObject&) = delete;

   virtual ~FilterObject() noexcept = default;

   enum class Type
   {
      Condition,
      And,
      Or
   };

   //! A filter is valid if it is enabled.
   //! For Conditions, mColumn must also be non-negative.
   //! For And/Or, there must also be at least one valid child.
   bool IsValid() const noexcept;

   //! Accessor for mType.
   //! Any value other than And / Or will be considered a Condition.
   bool IsCondition() const noexcept;

   //! Looks through the line of parents to see whether this filter will ever be reached.
   bool IsReachable() const noexcept;

   //! Uses mRule's GetName() if it exists.
   //! Else returns empty string.
   QString GetName() const;

   const FilterObject* GetParent() const noexcept;

   //! Handles inversion.
   //! If aModel is nullptr, returns false.
   //! Otherwise returns true if not valid.
   //!
   bool TestRow(const QAbstractItemModel* aModel, int aRow) const;

   FilterObject& AddChild(const Rule* aRule);
   void          InsertChild(FilterPtr aPtr);
   void          RemoveChild(FilterObject* aChild);

   int                 ChildCount() const noexcept;
   FilterObject*       GetChild(int aIndex);
   const FilterObject* GetChild(int aIndex) const;

   FilterPtr Clone() const;

   Type        mType = Type::Condition;
   const Rule* mRule = nullptr;

   bool mEnabled  = true;
   bool mInverted = false;

   int     mColumn = -1; //!< Column less than 0 implies no column given
   QString mValue;

private:
   bool TestRowCondition(const QAbstractItemModel* aModel, int aRow) const;
   bool TestRowAnd(const QAbstractItemModel* aModel, int aRow) const;
   bool TestRowOr(const QAbstractItemModel* aModel, int aRow) const;

   const FilterObject* mParent = nullptr;
   FilterList          mChildren;
};
} // namespace WkfFilter

#endif
