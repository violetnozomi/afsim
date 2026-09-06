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

#ifndef UT_QT_SETTINGS_SCOPES_HPP
#define UT_QT_SETTINGS_SCOPES_HPP

#include <QSettings>

namespace ut
{
namespace qt
{
class GroupScope final
{
public:
   GroupScope(QSettings& aSettings, QString aGroupName)
      : mSettings(aSettings)
   {
      mSettings.beginGroup(aGroupName);
   }

   GroupScope(const GroupScope&) = delete;

   ~GroupScope() noexcept { mSettings.endGroup(); }

private:
   QSettings& mSettings;
};

/*! Example use case:
{
   ut::qt::WriteArrayScope was{ settings, "name" };

   for (const auto& value : container)
   {
      was.Next();
      settings.setValue("a", value.a);
      settings.setValue("b", value.b);
   }
} // was will now go out of scope; the "name" array will no longer be written to
*/
class WriteArrayScope final
{
public:
   WriteArrayScope(QSettings& aSettings, QString aArrayName)
      : mSettings(aSettings)
   {
      mSettings.beginWriteArray(aArrayName);
   }

   WriteArrayScope(const WriteArrayScope&) = delete;

   ~WriteArrayScope() noexcept { mSettings.endArray(); }

   //! Returns the current index.
   //! Does not work when mixing Next() and QSettings::setArrayIndex()!
   int Index() const noexcept { return mIndex; }

   //! Do not mix calling Next() and QSettings::setArrayIndex()!
   //! To be called at the beginning of the loop.
   //! Returns the new index.
   int Next() noexcept
   {
      mSettings.setArrayIndex(mIndex);
      return mIndex++;
   }

private:
   QSettings& mSettings;
   int        mIndex = 0;
};

/*! Example use case:
{
   ut::qt::ReadArrayScope ras{ settings, "name" };

   while (ras.Next())
   {
      Type value;

      value.a = settings.value("a").toInt();
      value.b = settings.value("b").toInt();

      container.push(value);
   }
} // ras will now go out of scope; the "name" array will no longer be written to
*/
class ReadArrayScope final
{
public:
   ReadArrayScope(QSettings& aSettings, QString aArray)
      : mSettings(aSettings)
   {
      mSize = mSettings.beginReadArray(aArray);
   }

   ReadArrayScope(const ReadArrayScope&) = delete;

   ~ReadArrayScope() noexcept { mSettings.endArray(); }

   //! Returns the size of the array being read.
   int Size() const noexcept { return mSize; }

   //! Returns the current index.
   //! Does not work when mixing Next() and QSettings::setArrayIndex()!
   int Index() const noexcept { return mIndex; }

   //! Do not mix calling Next() and QSettings::setArrayIndex()!
   //! while(scope.Next()) { ... }
   bool Next() noexcept
   {
      if (mIndex < mSize)
      {
         mSettings.setArrayIndex(mIndex);
         mIndex++;
         return true;
      }
      return false;
   }

private:
   QSettings& mSettings;
   int        mSize;
   int        mIndex = 0;
};
} // namespace qt
} // namespace ut

#endif
