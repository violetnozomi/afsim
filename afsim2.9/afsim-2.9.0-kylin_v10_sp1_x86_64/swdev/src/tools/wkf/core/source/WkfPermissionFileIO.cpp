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

#include "WkfPermissionFileIO.hpp"

#include <stdexcept>

#include <QSettings>

#include "UtQtSettingsScopes.hpp"

wkf::permission::FileReader::FileReader() noexcept
{
   SetupDefaultReaders();
}

void wkf::permission::FileReader::Fallback(PermissionNode& aNode, QSettings& aSettings) const
{
   throw std::runtime_error("FileReader: Unrecognized node type.");
}

void wkf::permission::FileReader::SetupDefaultReaders() noexcept
{
   Reset();

   Register<BoolPermission, &FileReader::ReadBool>();
   Register<IntPermission, &FileReader::ReadInt>();
   Register<StringPermission, &FileReader::ReadString>();
   Register<StringListPermission, &FileReader::ReadStringList>();
   Register<ObjectPermission, &FileReader::ReadObject>();
   Register<Category, &FileReader::ReadCategory>();
}

void wkf::permission::FileReader::ReadBool(BoolPermission& aNode, QSettings& aSettings) const
{
   bool value = aSettings.value(aNode.mName, aNode.GetDefault()).toBool();
   if (!aNode.Set(value))
   {
      emit aNode.Changed(value);
   }
}

void wkf::permission::FileReader::ReadInt(IntPermission& aNode, QSettings& aSettings) const
{
   int value = aSettings.value(aNode.mName, aNode.GetDefault()).toInt();
   if (!aNode.Set(value))
   {
      emit aNode.Changed(value);
   }
}

void wkf::permission::FileReader::ReadString(StringPermission& aNode, QSettings& aSettings) const
{
   QString value = aSettings.value(aNode.mName, aNode.GetDefault()).toString();
   if (!aNode.Set(value))
   {
      emit aNode.Changed(value);
   }
}

void wkf::permission::FileReader::ReadStringList(StringListPermission& aNode, QSettings& aSettings) const
{
   QStringList value = aSettings.value(aNode.mName, aNode.GetDefault()).toStringList();
   if (!aNode.Set(value))
   {
      emit aNode.Changed(value);
   }
}

void wkf::permission::FileReader::ReadObject(ObjectPermission& aNode, QSettings& aSettings) const
{
   ut::qt::GroupScope gs{aSettings, aNode.mName};

   for (int i = 0; i < aNode.FieldCount(); i++)
   {
      Visit(aNode.GetField(i), aSettings);
   }
}

void wkf::permission::FileReader::ReadCategory(Category& aNode, QSettings& aSettings) const
{
   ut::qt::GroupScope gs{aSettings, aNode.mName};

   for (int i = 0; i < aNode.PermissionCount(); i++)
   {
      Visit(aNode.PermissionAt(i), aSettings);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

wkf::permission::FileWriter::FileWriter() noexcept
{
   SetupDefaultWriters();
}

void wkf::permission::FileWriter::Fallback(const PermissionNode& aNode, QSettings& aSettings) const
{
   throw std::runtime_error("FileReader: Unrecognized node type.");
}

void wkf::permission::FileWriter::SetupDefaultWriters() noexcept
{
   Reset();

   Register<BoolPermission, &FileWriter::WriteBool>();
   Register<IntPermission, &FileWriter::WriteInt>();
   Register<StringPermission, &FileWriter::WriteString>();
   Register<StringListPermission, &FileWriter::WriteStringList>();
   Register<ObjectPermission, &FileWriter::WriteObject>();
   Register<Category, &FileWriter::WriteCategory>();
}

void wkf::permission::FileWriter::WriteBool(const BoolPermission& aNode, QSettings& aSettings) const
{
   aSettings.setValue(aNode.mName, aNode.Get());
}

void wkf::permission::FileWriter::WriteInt(const IntPermission& aNode, QSettings& aSettings) const
{
   aSettings.setValue(aNode.mName, aNode.Get());
}

void wkf::permission::FileWriter::WriteString(const StringPermission& aNode, QSettings& aSettings) const
{
   aSettings.setValue(aNode.mName, aNode.Get());
}

void wkf::permission::FileWriter::WriteStringList(const StringListPermission& aNode, QSettings& aSettings) const
{
   aSettings.setValue(aNode.mName, aNode.Get());
}

void wkf::permission::FileWriter::WriteObject(const ObjectPermission& aNode, QSettings& aSettings) const
{
   ut::qt::GroupScope gs{aSettings, aNode.mName};

   for (int i = 0; i < aNode.FieldCount(); i++)
   {
      Visit(aNode.GetField(i), aSettings);
   }
}

void wkf::permission::FileWriter::WriteCategory(const Category& aNode, QSettings& aSettings) const
{
   ut::qt::GroupScope gs{aSettings, aNode.mName};

   for (int i = 0; i < aNode.PermissionCount(); i++)
   {
      Visit(aNode.PermissionAt(i), aSettings);
   }
}
