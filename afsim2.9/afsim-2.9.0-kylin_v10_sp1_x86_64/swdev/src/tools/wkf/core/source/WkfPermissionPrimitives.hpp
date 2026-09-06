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

#ifndef WKF_PERMISSION_PRIMITIVES_HPP
#define WKF_PERMISSION_PRIMITIVES_HPP

#include "wkf_export.h"

#include <QObject>
#include <QString>

#include "UtReflectObject.hpp"

namespace wkf
{
namespace permission
{
struct WKF_EXPORT PermissionNode : ut::reflect::Node
{
   PermissionNode(QString aName, QString aDescription = {}) noexcept;

   const QString mName;
   QString       mDescription;

   virtual void RestoreDefaults() = 0;
};

template<typename T>
using DeriveNode = ut::reflect::Derive<T, PermissionNode>;

class WKF_EXPORT BoolPermission : public QObject, public DeriveNode<BoolPermission>
{
   Q_OBJECT

public:
   BoolPermission(QString aName, QString aDescription = {}, bool aDefault = true) noexcept;

   bool Get() const noexcept;
   //! Returns true if the value changed.
   bool Set(bool aSet) noexcept;

   bool GetDefault() const noexcept;
   void RestoreDefaults() override;

signals:
   void Changed(bool aNewValue);

private:
   bool mValue;
   bool mDefault;
};

class WKF_EXPORT IntPermission : public QObject, public DeriveNode<IntPermission>
{
   Q_OBJECT

public:
   IntPermission(QString aName, QString aDescription = {}, int aDefault = 0) noexcept;

   int Get() const noexcept;
   //! Returns true if the value changed.
   bool Set(int aSet) noexcept;

   int  GetDefault() const noexcept;
   void RestoreDefaults() override;

signals:
   void Changed(int aNewValue);

private:
   int mValue;
   int mDefault;
};

class WKF_EXPORT StringPermission : public QObject, public DeriveNode<StringPermission>
{
   Q_OBJECT

public:
   StringPermission(QString aName, QString aDescription = {}, QString aDefault = {}) noexcept;

   const QString& Get() const noexcept;
   //! Returns true if the value changed.
   bool Set(QString aSet) noexcept;

   const QString& GetDefault() const noexcept;
   void           RestoreDefaults() override;

signals:
   void Changed(QString aNewValue);

private:
   QString mValue;
   QString mDefault;
};

class WKF_EXPORT StringListPermission : public QObject, public DeriveNode<StringListPermission>
{
   Q_OBJECT

public:
   StringListPermission(QString aName, QString aDescription = {}, QStringList aDefault = {}) noexcept;

   const QStringList& Get() const noexcept;

   //! Returns true if the value changed.
   bool Set(QStringList aSet) noexcept;
   //! Appends aValue/aValues to the value.
   //! Prevents duplicates.
   //! Returns true if the value changed.
   bool Append(QString aValue) noexcept;
   bool Append(QStringList aValues) noexcept;

   //! Returns true if the value changed.
   bool Clear() noexcept;

   //! Removes aValue/aValues from the value.
   //! Returns true if the value changed.
   bool Remove(QString aValue) noexcept;
   bool Remove(QStringList aValues) noexcept;

   const QStringList& GetDefault() const noexcept;
   void               RestoreDefaults() override;

signals:
   void Changed(QStringList aNewValue);

private:
   QStringList mValue;
   QStringList mDefault;
};

class WKF_EXPORT ObjectPermission : public ut::reflect::Derive<ObjectPermission, ut::reflect::ObjectNode<PermissionNode>>
{
public:
   using ReflectBase::ReflectBase;

   void RestoreDefaults() override;
};

class WKF_EXPORT Category : public QObject, public DeriveNode<Category>
{
public:
   Category(QString aName, QString aDescription = {}) noexcept;

   Category(const Category&) = delete;
   void operator=(const Category&) = delete;

   //! Constructs a permission of type T and stores it.
   //! Returns a reference to the constructed permission.
   template<typename T, typename... Args>
   ut::enable_if_t<std::is_constructible<T, Args...>::value && std::is_base_of<PermissionNode, T>::value, T&>
   AddPermission(Args&&... aArgs) noexcept(std::is_nothrow_constructible<T, Args...>::value)
   {
      auto ptr    = ut::make_unique<T>(std::forward<Args>(aArgs)...);
      T&   retval = *ptr;
      mPermissions.emplace_back(std::move(ptr));
      return retval;
   }

   int PermissionCount() const noexcept;

   //! Throws std::out_of_range.
   //! @{
   PermissionNode&       PermissionAt(int aIndex);
   const PermissionNode& PermissionAt(int aIndex) const;
   //! @}

   //! Finds the permission with the given name.
   //! Returns nullptr if there is none.
   //! @{
   PermissionNode*       FindPermission(const QString& aName) noexcept;
   const PermissionNode* FindPermission(const QString& aName) const noexcept;
   //! @}

   void RestoreDefaults() override;

private:
   std::vector<std::unique_ptr<PermissionNode>> mPermissions;
};
} // namespace permission
} // namespace wkf

#endif
