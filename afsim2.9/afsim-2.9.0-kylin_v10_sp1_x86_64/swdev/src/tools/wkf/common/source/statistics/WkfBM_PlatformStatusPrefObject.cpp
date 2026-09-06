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

#include "WkfBM_PlatformStatusPrefObject.hpp"

#include "UtQtSettingsScopes.hpp"

static QList<QStringList> ReadIndexList(QSettings& aSettings, const QString& aListName)
{
   ut::qt::GroupScope gs{aSettings, aListName};
   auto               keys = aSettings.childKeys();

   QList<QStringList> retval;
   for (const auto& key : keys)
   {
      retval += aSettings.value(key).toStringList();
   }
   return retval;
}

static void WriteIndexList(QSettings& aSettings, const QString& aListName, const QList<QStringList>& aList)
{
   ut::qt::GroupScope gs{aSettings, aListName};

   int i = 0;
   for (const auto& index : aList)
   {
      aSettings.setValue(QString::number(i), index);
      i++;
   }
}

wkf::bm::PlatformStatusPrefObject::PlatformStatusPrefObject(QObject* aParent /* = nullptr */) noexcept
   : wkf::ExportPrefObject<Configuration>(aParent, cNAME)
{
}

wkf::bm::Configuration wkf::bm::PlatformStatusPrefObject::ReadExport(QSettings& aSettings) const
{
   Configuration retval;

   retval.mSplitter = aSettings.value("splitter").toByteArray();

   retval.mWhiteList = ReadIndexList(aSettings, "whiteList");
   retval.mBlackList = ReadIndexList(aSettings, "blackList");

   { // RAII block
      ut::qt::ReadArrayScope ras{aSettings, "columns"};
      for (int i = 0; i < ras.Size(); i++)
      {
         aSettings.setArrayIndex(i);
         retval.mColumns.append(ReadColumn(aSettings));
      }
   }

   // retval.mHeader = aSettings.value("header").toByteArray();

   return retval;
}

void wkf::bm::PlatformStatusPrefObject::SaveExport(QSettings& aSettings, const Configuration& aConfiguration) const
{
   aSettings.setValue("splitter", aConfiguration.mSplitter);

   WriteIndexList(aSettings, "whiteList", aConfiguration.mWhiteList);
   WriteIndexList(aSettings, "blackList", aConfiguration.mBlackList);

   { // RAII block
      ut::qt::WriteArrayScope was{aSettings, "columns"};
      for (int i = 0; i < aConfiguration.mColumns.size(); i++)
      {
         aSettings.setArrayIndex(i);
         SaveColumn(aSettings, aConfiguration.mColumns[i]);
      }
   }

   // aSettings.setValue("header", aConfiguration.mHeader);
}

wkf::bm::ColumnData wkf::bm::PlatformStatusPrefObject::ReadColumn(QSettings& aSettings) const
{
   ColumnData retval;

   retval.mName   = aSettings.value("name").toString();
   retval.mMode   = static_cast<ColorRule::Mode>(aSettings.value("mode").toInt());
   retval.mHidden = aSettings.value("hidden").toBool();

   const auto defaultColor = aSettings.value("defaultColor");
   if (defaultColor.isValid())
   {
      retval.mDefaultColor = defaultColor.value<QColor>();
   }

   ut::qt::ReadArrayScope ras{aSettings, "colors"};
   for (int i = 0; i < ras.Size(); i++)
   {
      aSettings.setArrayIndex(i);
      retval.mColors.append(ReadColor(aSettings));
   }

   return retval;
}

void wkf::bm::PlatformStatusPrefObject::SaveColumn(QSettings& aSettings, const ColumnData& aColumn) const
{
   aSettings.setValue("name", aColumn.mName);
   aSettings.setValue("mode", static_cast<int>(aColumn.mMode));
   aSettings.setValue("hidden", aColumn.mHidden);

   aSettings.remove("defaultColor");
   if (aColumn.mDefaultColor)
   {
      aSettings.setValue("defaultColor", *aColumn.mDefaultColor);
   }

   ut::qt::WriteArrayScope was{aSettings, "colors"};
   for (int i = 0; i < aColumn.mColors.size(); i++)
   {
      aSettings.setArrayIndex(i);
      SaveColor(aSettings, aColumn.mColors[i]);
   }
}

wkf::bm::ColorData wkf::bm::PlatformStatusPrefObject::ReadColor(QSettings& aSettings) const
{
   ColorData retval;

   retval.mColor      = aSettings.value("color").value<QColor>();
   retval.mComparison = aSettings.value("comp").toString();
   retval.mValue      = aSettings.value("value").toString();
   retval.mBool       = aSettings.value("bool").toBool();

   return retval;
}

void wkf::bm::PlatformStatusPrefObject::SaveColor(QSettings& aSettings, const ColorData& aColor) const
{
   aSettings.setValue("color", QVariant::fromValue(aColor.mColor));
   aSettings.setValue("comp", aColor.mComparison);
   aSettings.setValue("value", aColor.mValue);
   aSettings.setValue("bool", aColor.mBool);
}
