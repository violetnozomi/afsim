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

#include "WkfFilterPrefObject.hpp"

WkfFilter::FilterPrefObject::FilterPrefObject(QObject* aParent, const QString& aName) noexcept
   : wkf::ExportPrefObject<FilterExport>(aParent, aName)
{
}

WkfFilter::FilterExport WkfFilter::FilterPrefObject::ReadExport(QSettings& aSettings) const
{
   FilterExport retval;

   QString type = aSettings.value("type").toString();
   if (type == "and")
   {
      retval.mType = FilterObject::Type::And;
   }
   else if (type == "or")
   {
      retval.mType = FilterObject::Type::Or;
   }
   else
   {
      retval.mType = FilterObject::Type::Condition;
   }

   retval.mRule     = aSettings.value("rule", "==").toString();
   retval.mEnabled  = aSettings.value("enabled", false).toBool();
   retval.mInverted = aSettings.value("inverted", false).toBool();
   retval.mColumn   = aSettings.value("column", -1).toInt();
   retval.mValue    = aSettings.value("value").toString();

   ut::qt::ReadArrayScope ras{aSettings, "children"};
   for (int i = 0; i < ras.Size(); i++)
   {
      aSettings.setArrayIndex(i);
      retval.mChildren.emplace_back(ReadExport(aSettings));
   }

   return retval;
}

void WkfFilter::FilterPrefObject::SaveExport(QSettings& aSettings, const FilterExport& aPrefs) const
{
   switch (aPrefs.mType)
   {
   case FilterObject::Type::And:
      aSettings.setValue("type", "and");
      break;
   case FilterObject::Type::Or:
      aSettings.setValue("type", "or");
      break;
   case FilterObject::Type::Condition:
   default:
      aSettings.setValue("type", "cond");
   }

   aSettings.setValue("rule", aPrefs.mRule);
   aSettings.setValue("enabled", aPrefs.mEnabled);
   aSettings.setValue("inverted", aPrefs.mInverted);
   aSettings.setValue("column", aPrefs.mColumn);
   aSettings.setValue("value", aPrefs.mValue);

   ut::qt::WriteArrayScope was{aSettings, "children"};

   const int count = static_cast<int>(aPrefs.mChildren.size());
   for (int i = 0; i < count; i++)
   {
      aSettings.setArrayIndex(i);
      SaveExport(aSettings, aPrefs.mChildren[i]);
   }
}
