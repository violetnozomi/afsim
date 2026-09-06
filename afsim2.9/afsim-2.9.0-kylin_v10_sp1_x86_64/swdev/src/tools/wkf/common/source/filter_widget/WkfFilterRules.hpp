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

#ifndef WKF_FILTER_RULES_HPP
#define WKF_FILTER_RULES_HPP

#include "WkfFilterObject.hpp"

namespace WkfFilter
{
struct WKF_COMMON_EXPORT RuleEquals final : Rule
{
   QString Name() const override;
   bool    TestValue(const QVariant& aModelData, const QString& aFilterData) const override;
};

struct WKF_COMMON_EXPORT RuleNotEquals final : Rule
{
   QString Name() const override;
   bool    TestValue(const QVariant& aModelData, const QString& aFilterData) const override;
};

struct WKF_COMMON_EXPORT RuleLessThan final : Rule
{
   QString Name() const override;
   bool    TestValue(const QVariant& aModelData, const QString& aFilterData) const override;
   bool    Predicate(const QAbstractItemModel* aModel, int aColumn) const override;
};

struct WKF_COMMON_EXPORT RuleLessOrEqual final : Rule
{
   QString Name() const override;
   bool    TestValue(const QVariant& aModelData, const QString& aFilterData) const override;
   bool    Predicate(const QAbstractItemModel* aModel, int aColumn) const override;
};

struct WKF_COMMON_EXPORT RuleGreaterThan final : Rule
{
   QString Name() const override;
   bool    TestValue(const QVariant& aModelData, const QString& aFilterData) const override;
   bool    Predicate(const QAbstractItemModel* aModel, int aColumn) const override;
};

struct WKF_COMMON_EXPORT RuleGreaterOrEqual final : Rule
{
   QString Name() const override;
   bool    TestValue(const QVariant& aModelData, const QString& aFilterData) const override;
   bool    Predicate(const QAbstractItemModel* aModel, int aColumn) const override;
};

struct WKF_COMMON_EXPORT RuleContains final : Rule
{
   QString Name() const override;
   bool    TestValue(const QVariant& aModelData, const QString& aFilterData) const override;
};
} // namespace WkfFilter

#endif
