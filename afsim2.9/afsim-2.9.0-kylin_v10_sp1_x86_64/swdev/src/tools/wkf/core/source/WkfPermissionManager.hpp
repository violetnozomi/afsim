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

#ifndef WKF_PERMISSION_MANAGER_HPP
#define WKF_PERMISSION_MANAGER_HPP

#include <map>

#include "WkfPermissionFileIO.hpp"
#include "WkfPermissionPrimitives.hpp"

namespace wkf
{
namespace permission
{
class WKF_EXPORT Manager : public QObject
{
   Q_OBJECT

public:
   Manager() = default;

   Manager(const Manager&) = delete;
   void operator=(const Manager&) = delete;

   bool IsEmpty() const noexcept;

   template<typename T, typename... Args>
   ut::enable_if_t<std::is_base_of<PermissionNode, T>::value && std::is_constructible<T, Args...>::value, T&>
   AddPermission(const QString& aCategory, Args&&... aArgs) noexcept
   {
      return GetOrAddCategory(aCategory).AddPermission<T>(std::forward<Args>(aArgs)...);
   }

   BoolPermission& AddBool(const QString& aCategory, QString aName, QString aDescription = {}, bool aDefault = true) noexcept;

   IntPermission& AddInt(const QString& aCategory, QString aName, QString aDescription = {}, int aDefault = 0) noexcept;

   StringPermission& AddString(const QString& aCategory, QString aName, QString aDescription = {}, QString aDefault = {}) noexcept;

   StringListPermission& AddStringList(const QString& aCategory,
                                       QString        aName,
                                       QString        aDescription = {},
                                       QStringList    aDefault     = {}) noexcept;

   void           SetDefaultCategory(QString aCategory) noexcept;
   const QString& GetDefaultCategory() const noexcept;

   Category*       GetCategory(const QString& aCategory) noexcept;
   const Category* GetCategory(const QString& aCategory) const noexcept;

   PermissionNode*       FindPermission(const QString& aCategory, const QString& aName) noexcept;
   const PermissionNode* FindPermission(const QString& aCategory, const QString& aName) const noexcept;

   QStringList GetCategories() const noexcept;

   void ReadFile(QSettings& aSettings);
   void WriteFile(QSettings& aSettings) const;

   void RestoreDefaults();

private:
   std::map<QString, Category> mCategories;
   QString                     mDefaultCategory;
   FileReader                  mReader;
   FileWriter                  mWriter;

   //! Returns the category with the given name.
   //! If none exists, constructs it with the given name and returns it.
   Category& GetOrAddCategory(const QString& aCategory) noexcept;
};
} // namespace permission
} // namespace wkf

#endif
