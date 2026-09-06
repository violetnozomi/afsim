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

#ifndef WKF_EXPORT_HELPER_HPP
#define WKF_EXPORT_HELPER_HPP

#include <functional>

#include <QSettings>

#include "UtQtSettingsScopes.hpp"
#include "WkfPrefObject.hpp"

class QMenu;

namespace wkf
{
//! Derived classes need to override ReadExport and SaveExport.
template<typename PREF_DATA>
class ExportPrefObject : public PrefObjectT<std::map<QString, PREF_DATA>>
{
public:
   using ExportList = std::map<QString, PREF_DATA>;

   ExportPrefObject(QObject* aParent, const QString& aName) noexcept
      : PrefObjectT<ExportList>(aParent, aName)
   {
   }

   //! Restoring defaults does not delete existing exports.
   void RestoreDefaults() override {}

   void Export(const QString& aName, PREF_DATA&& aPrefs) { this->mCurrentPrefs[aName] = std::move(aPrefs); }

   void DeleteExport(const QString& aName) { this->mCurrentPrefs.erase(aName); }

   QStringList ListExports() const noexcept
   {
      QStringList retval;
      for (const auto& it : this->mCurrentPrefs)
      {
         retval += it.first;
      }
      return retval;
   }

   const PREF_DATA& GetExport(const QString& aName) const { return this->mCurrentPrefs.at(aName); }

protected:
   //! No immediate changes upon applying.
   //! Can be overridden in derived classes.
   void Apply() override {}

   ExportList ReadSettings(QSettings& aSettings) const final
   {
      ExportList retval;

      auto groups = aSettings.childGroups();
      for (const auto& group : groups)
      {
         ut::qt::GroupScope gs{aSettings, group};
         retval[group] = ReadExport(aSettings);
      }

      return retval;
   }

   void SaveSettingsP(QSettings& aSettings) const final
   {
      // Clears ALL previously stored settings.
      aSettings.remove("");

      for (const auto& config : this->mCurrentPrefs)
      {
         ut::qt::GroupScope gs{aSettings, config.first};
         SaveExport(aSettings, config.second);
      }
   }

   //! Override ReadExport and SaveExport to control interaction with QSettings.
   virtual PREF_DATA ReadExport(QSettings& aSettings) const                          = 0;
   virtual void      SaveExport(QSettings& aSettings, const PREF_DATA& aPrefs) const = 0;
};

//! Export widget provides an interface for importing and exporting settings.
class ExportHelper : public QObject
{
   Q_OBJECT

public:
   //! aPreferences is the PrefObject to save to.
   //! aLabel indicates what is being imported or exported. E.g. "Settings", "Plot", etc.
   template<typename T>
   ExportHelper(ExportPrefObject<T>& aPreferences, QString aLabel) noexcept
      : mLabel(std::move(aLabel))
   {
      auto*       ptr  = &aPreferences;
      const auto* cptr = ptr;
      mDeleteExport    = [ptr](QString aName) { ptr->DeleteExport(aName); };
      mListExports     = [cptr]() { return cptr->ListExports(); };
   }

   //! Should be called in the slot object's destructor.
   //! Emits WriteState().
   void WriteFinalState() const;

   //! Should be called in the slot object's constructor, or shortly after construction.
   //! Emits ReadState().
   void ReadInitialState();

   //! Adds Export Settings and Import Settings to a context menu.
   void BuildContextMenu(QMenu& aMenu);

signals:
   //! When called, the slot being connected to should write to the file the widget's current state.
   void WriteState(QString aName) const;

   //! When called, aSettings will be scoped to the correct group.
   //! The slot being connected to should read from the file and set the widget's current state.
   void ReadState(QString aName);

private:
   QString mLabel;

   std::function<void(QString)> mDeleteExport;
   std::function<QStringList()> mListExports;

   //! Creates and executes a dialog for exporting settings.
   void ExportSettingsDialog() const;

   //! Creates and executes a dialog for importing settings.
   void ImportSettingsDialog();
};
} // namespace wkf

#endif
