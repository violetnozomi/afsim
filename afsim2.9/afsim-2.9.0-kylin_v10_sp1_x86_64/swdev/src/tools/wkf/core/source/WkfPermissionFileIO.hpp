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

#ifndef WKF_PERMISSION_FILE_IO_HPP
#define WKF_PERMISSION_FILE_IO_HPP

#include "UtReflectConstVisitor.hpp"
#include "WkfPermissionPrimitives.hpp"

class QSettings;

namespace wkf
{
namespace permission
{
class FileReader : public ut::reflect::ConstVisitor<FileReader, PermissionNode, void(QSettings&)>
{
public:
   FileReader() noexcept;

   void Fallback(PermissionNode& aNode, QSettings& aSettings) const override;

   void SetupDefaultReaders() noexcept;

private:
   void ReadBool(BoolPermission& aNode, QSettings& aSettings) const;
   void ReadInt(IntPermission& aNode, QSettings& aSettings) const;
   void ReadString(StringPermission& aNode, QSettings& aSettings) const;
   void ReadStringList(StringListPermission& aNode, QSettings& aSettings) const;
   void ReadObject(ObjectPermission& aNode, QSettings& aSettings) const;
   void ReadCategory(Category& aNode, QSettings& aSettings) const;
};

class FileWriter : public ut::reflect::ConstVisitor<FileWriter, const PermissionNode, void(QSettings&)>
{
public:
   FileWriter() noexcept;

   void Fallback(const PermissionNode& aNode, QSettings& aSettings) const override;

   void SetupDefaultWriters() noexcept;

private:
   void WriteBool(const BoolPermission& aNode, QSettings& aSettings) const;
   void WriteInt(const IntPermission& aNode, QSettings& aSettings) const;
   void WriteString(const StringPermission& aNode, QSettings& aSettings) const;
   void WriteStringList(const StringListPermission& aNode, QSettings& aSettings) const;
   void WriteObject(const ObjectPermission& aNode, QSettings& aSettings) const;
   void WriteCategory(const Category& aNode, QSettings& aSettings) const;
};
} // namespace permission
} // namespace wkf

#endif
