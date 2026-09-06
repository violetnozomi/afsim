// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfPermissionManager.hpp"

bool wkf::permission::Manager::IsEmpty() const noexcept
{
   return mCategories.empty();
}

wkf::permission::BoolPermission& wkf::permission::Manager::AddBool(const QString& aCategory,
                                                                   QString        aName,
                                                                   QString        aDescription /*= {}*/,
                                                                   bool           aDefault /*= true*/) noexcept
{
   return AddPermission<BoolPermission>(aCategory, std::move(aName), std::move(aDescription), aDefault);
}

wkf::permission::IntPermission& wkf::permission::Manager::AddInt(const QString& aCategory,
                                                                 QString        aName,
                                                                 QString        aDescription /*= {}*/,
                                                                 int            aDefault /*= 0*/) noexcept
{
   return AddPermission<IntPermission>(aCategory, std::move(aName), std::move(aDescription), aDefault);
}

wkf::permission::StringPermission& wkf::permission::Manager::AddString(const QString& aCategory,
                                                                       QString        aName,
                                                                       QString        aDescription /*= {}*/,
                                                                       QString        aDefault /*= {}*/) noexcept
{
   return AddPermission<StringPermission>(aCategory, std::move(aName), std::move(aDescription), std::move(aDefault));
}

wkf::permission::StringListPermission& wkf::permission::Manager::AddStringList(const QString& aCategory,
                                                                               QString        aName,
                                                                               QString        aDescription /*= {}*/,
                                                                               QStringList aDefault /*= {}*/) noexcept
{
   return AddPermission<StringListPermission>(aCategory, std::move(aName), std::move(aDescription), std::move(aDefault));
}

void wkf::permission::Manager::SetDefaultCategory(QString aCategory) noexcept
{
   mDefaultCategory = std::move(aCategory);
}

const QString& wkf::permission::Manager::GetDefaultCategory() const noexcept
{
   return mDefaultCategory;
}

wkf::permission::Category* wkf::permission::Manager::GetCategory(const QString& aCategory) noexcept
{
   auto it = mCategories.find(aCategory);
   if (it != mCategories.end())
   {
      return &it->second;
   }
   return nullptr;
}

const wkf::permission::Category* wkf::permission::Manager::GetCategory(const QString& aCategory) const noexcept
{
   auto it = mCategories.find(aCategory);
   if (it != mCategories.end())
   {
      return &it->second;
   }
   return nullptr;
}

wkf::permission::PermissionNode* wkf::permission::Manager::FindPermission(const QString& aCategory, const QString& aName) noexcept
{
   auto cat = GetCategory(aCategory);
   if (cat)
   {
      return cat->FindPermission(aName);
   }
   return nullptr;
}

const wkf::permission::PermissionNode* wkf::permission::Manager::FindPermission(const QString& aCategory,
                                                                                const QString& aName) const noexcept
{
   auto cat = GetCategory(aCategory);
   if (cat)
   {
      return cat->FindPermission(aName);
   }
   return nullptr;
}

QStringList wkf::permission::Manager::GetCategories() const noexcept
{
   QStringList retval;

   for (const auto& cat : mCategories)
   {
      retval += cat.first;
   }

   return retval;
}

void wkf::permission::Manager::ReadFile(QSettings& aSettings)
{
   for (auto& cat : mCategories)
   {
      mReader.Visit(cat.second, aSettings);
   }
}

void wkf::permission::Manager::WriteFile(QSettings& aSettings) const
{
   for (const auto& cat : mCategories)
   {
      mWriter.Visit(cat.second, aSettings);
   }
}

void wkf::permission::Manager::RestoreDefaults()
{
   for (auto& cat : mCategories)
   {
      cat.second.RestoreDefaults();
   }
}

wkf::permission::Category& wkf::permission::Manager::GetOrAddCategory(const QString& aCategory) noexcept
{
   auto it = mCategories.find(aCategory);
   if (it != mCategories.end())
   {
      return it->second;
   }

   auto emp =
      mCategories.emplace(std::piecewise_construct, std::forward_as_tuple(aCategory), std::forward_as_tuple(aCategory));
   return emp.first->second;
}
