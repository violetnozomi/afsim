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

#include "WkfPermissionPrimitives.hpp"

wkf::permission::PermissionNode::PermissionNode(QString aName, QString aDescription /*= {}*/) noexcept
   : mName(std::move(aName))
   , mDescription(std::move(aDescription))
{
}

wkf::permission::BoolPermission::BoolPermission(QString aName,
                                                QString aDescription, /*= {}*/
                                                bool    aDefault /*= true*/) noexcept
   : ReflectBase(std::move(aName), std::move(aDescription))
   , mValue(aDefault)
   , mDefault(aDefault)
{
}

bool wkf::permission::BoolPermission::Get() const noexcept
{
   return mValue;
}

bool wkf::permission::BoolPermission::Set(bool aSet) noexcept
{
   if (mValue != aSet)
   {
      mValue = aSet;
      emit Changed(mValue);
      return true;
   }
   return false;
}

bool wkf::permission::BoolPermission::GetDefault() const noexcept
{
   return mDefault;
}

void wkf::permission::BoolPermission::RestoreDefaults()
{
   Set(mDefault);
}

///////////////////////////////////////////////////////////////////////////////////////

wkf::permission::IntPermission::IntPermission(QString aName,
                                              QString aDescription, /*= {}*/
                                              int     aDefault /*= 0*/) noexcept
   : ReflectBase(std::move(aName), std::move(aDescription))
   , mValue(aDefault)
   , mDefault(aDefault)
{
}

int wkf::permission::IntPermission::Get() const noexcept
{
   return mValue;
}

bool wkf::permission::IntPermission::Set(int aSet) noexcept
{
   if (mValue != aSet)
   {
      mValue = aSet;
      emit Changed(mValue);
      return true;
   }
   return false;
}

int wkf::permission::IntPermission::GetDefault() const noexcept
{
   return mDefault;
}

void wkf::permission::IntPermission::RestoreDefaults()
{
   Set(mDefault);
}

///////////////////////////////////////////////////////////////////////////////////////

wkf::permission::StringPermission::StringPermission(QString aName,
                                                    QString aDescription, /*= {}*/
                                                    QString aDefault /*= {}*/) noexcept
   : ReflectBase(std::move(aName), std::move(aDescription))
   , mValue(aDefault)
   , mDefault(aDefault)
{
}

const QString& wkf::permission::StringPermission::Get() const noexcept
{
   return mValue;
}

bool wkf::permission::StringPermission::Set(QString aSet) noexcept
{
   if (mValue != aSet)
   {
      mValue = std::move(aSet);
      emit Changed(mValue);
      return true;
   }
   return false;
}

const QString& wkf::permission::StringPermission::GetDefault() const noexcept
{
   return mDefault;
}

void wkf::permission::StringPermission::RestoreDefaults()
{
   Set(mDefault);
}

///////////////////////////////////////////////////////////////////////////////////////

wkf::permission::StringListPermission::StringListPermission(QString     aName,
                                                            QString     aDescription /*= {}*/,
                                                            QStringList aDefault /*= {}*/) noexcept
   : ReflectBase(std::move(aName), std::move(aDescription))
   , mValue(aDefault)
   , mDefault(aDefault)
{
}

const QStringList& wkf::permission::StringListPermission::Get() const noexcept
{
   return mValue;
}

bool wkf::permission::StringListPermission::Set(QStringList aSet) noexcept
{
   if (mValue != aSet)
   {
      mValue = std::move(aSet);
      emit Changed(mValue);
      return true;
   }
   return false;
}

bool wkf::permission::StringListPermission::Append(QString aValue) noexcept
{
   if (!mValue.contains(aValue))
   {
      mValue += aValue;
      emit Changed(mValue);
      return true;
   }
   return false;
}

bool wkf::permission::StringListPermission::Append(QStringList aValues) noexcept
{
   bool changed = false;
   for (const auto& value : aValues)
   {
      if (!mValue.contains(value))
      {
         mValue += value;
         changed = true;
      }
   }
   if (changed)
   {
      emit Changed(mValue);
   }
   return changed;
}

bool wkf::permission::StringListPermission::Clear() noexcept
{
   if (!mValue.empty())
   {
      mValue.clear();
      emit Changed(mValue);
      return true;
   }
   return false;
}

bool wkf::permission::StringListPermission::Remove(QString aValue) noexcept
{
   if (mValue.removeAll(aValue) > 0)
   {
      emit Changed(mValue);
      return true;
   }
   return false;
}

bool wkf::permission::StringListPermission::Remove(QStringList aValues) noexcept
{
   bool changed = false;
   for (const auto& value : aValues)
   {
      changed |= (mValue.removeAll(value) > 0);
   }
   if (changed)
   {
      emit Changed(mValue);
   }
   return changed;
}

const QStringList& wkf::permission::StringListPermission::GetDefault() const noexcept
{
   return mDefault;
}

void wkf::permission::StringListPermission::RestoreDefaults()
{
   Set(mDefault);
}

///////////////////////////////////////////////////////////////////////////////////////

void wkf::permission::ObjectPermission::RestoreDefaults()
{
   for (int i = 0; i < FieldCount(); i++)
   {
      GetField(i).RestoreDefaults();
   }
}

///////////////////////////////////////////////////////////////////////////////////////

int wkf::permission::Category::PermissionCount() const noexcept
{
   return static_cast<int>(mPermissions.size());
}

wkf::permission::PermissionNode& wkf::permission::Category::PermissionAt(int aIndex)
{
   return *mPermissions.at(aIndex);
}

const wkf::permission::PermissionNode& wkf::permission::Category::PermissionAt(int aIndex) const
{
   return *mPermissions.at(aIndex);
}

wkf::permission::PermissionNode* wkf::permission::Category::FindPermission(const QString& aName) noexcept
{
   const int count = PermissionCount();
   for (int i = 0; i < count; i++)
   {
      if (mPermissions[i]->mName == aName)
      {
         return mPermissions[i].get();
      }
   }
   return nullptr;
}

const wkf::permission::PermissionNode* wkf::permission::Category::FindPermission(const QString& aName) const noexcept
{
   const int count = PermissionCount();
   for (int i = 0; i < count; i++)
   {
      if (mPermissions[i]->mName == aName)
      {
         return mPermissions[i].get();
      }
   }
   return nullptr;
}

wkf::permission::Category::Category(QString aName, QString aDescription /*= {}*/) noexcept
   : DeriveNode<Category>(std::move(aName), std::move(aDescription))
{
}

void wkf::permission::Category::RestoreDefaults()
{
   for (const auto& var : mPermissions)
   {
      var->RestoreDefaults();
   }
}
