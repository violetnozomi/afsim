// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFPREFOBJECT_HPP
#define WKFPREFOBJECT_HPP

#include "wkf_export.h"

#include <QMessageBox>
#include <QObject>
#include <QSettings>

namespace wkf
{
//! PrefObject is an interface for an object that manages preference data,
//! including loading/saving using QSettings, and restoring defaults.
class WKF_EXPORT PrefObject : public QObject
{
   Q_OBJECT

public:
   PrefObject(QObject* parent, const QString& aName);
   ~PrefObject() override;

   //! Set the preference data in the object by using a QVariant
   virtual void SetPreferenceData(const QVariant& aPrefData) = 0;

   //! Load settings into the object from the QSettings
   virtual void LoadSettings(QSettings& aAppSettings, QSettings& aSharedSettings) = 0;
   //! Save settings from the object into the QSettings
   virtual void SaveSettings(QSettings& aAppSettings, QSettings& aSharedSettings) const = 0;

   //! Restore the default preference data for the object
   virtual void RestoreDefaults() = 0;

   //! Get the name of the object that should uniquely identify it in the Environment's registry
   const QString& GetName() const { return mName; }

   //! Returns true if the preference applies across multiple WKF applications.
   //! By default, preferences are application-specific.
   virtual bool IsSharedPreference() const { return false; }

signals:
   //! Signal emitted whenever preferences are changed
   void PreferencesChanged(const QVariant&);

private:
   QString mName;
};

//! Class template PrefObjectT extends PrefObject by managing two sets of preference data (the current preferences and
//! the default preferences) using a data structure provided as the template argument. This type must be registered with
//! Qt's meta-object system using the Q_DECLARE_METATYPE macro to make it QVariant-convertible. Most preference objects
//! will derive from an implicit instantiation of this template, and implement the pure virtual method to apply the
//! preferences. Optionally, virtual methods to set preference data using the custom type and load/save using QSettings
//! may be overridden.
template<typename PREF_DATA>
class PrefObjectT : public PrefObject
{
   static_assert(std::is_default_constructible<PREF_DATA>::value,
                 "Template type of PrefObjectT must be default constructible");

public:
   using PrefDataType = PREF_DATA;

   PrefObjectT(QObject* parent, const QString& aName)
      : PrefObject(parent, aName)
   {
   }

   void SetPreferenceData(const QVariant& aPrefData) final;

   //! Get the current preferences
   const PREF_DATA& GetPreferences() const { return mCurrentPrefs; }

   void RestoreDefaults() override;
   void LoadSettings(QSettings& aAppSettings, QSettings& aSharedSettings) final;
   void SaveSettings(QSettings& aAppSettings, QSettings& aSharedSettings) const final;

protected:
   using BasePrefObject = PrefObjectT<PREF_DATA>;

   //! Set the preference data from the template type passed in as argument. Default implementation simply uses copy
   //! assignment to set the current preferences. A derived class may override this method, for example, to detect
   //! certain changes in the preferences and emit a signal or to assign preferences in a custom way.
   virtual void SetPreferenceDataP(const PREF_DATA& aPrefData) { mCurrentPrefs = aPrefData; }
   //! Apply the current preferences stored
   virtual void Apply() = 0;

   //! Returns a template type representing the updated preferences. This method does not directly modify the current
   //! preferences (this is delegated to SetPreferenceDataP instead). This is necessary so that preference objects
   //! setting boolean flags based on whether any preferences are different actually compare the correct values.
   virtual PREF_DATA ReadSettings(QSettings& aSettings) const  = 0;
   virtual void      SaveSettingsP(QSettings& aSettings) const = 0;

   PREF_DATA       mCurrentPrefs{};
   const PREF_DATA mDefaultPrefs{};
};

template<typename PREF_DATA>
void PrefObjectT<PREF_DATA>::SetPreferenceData(const QVariant& aPrefData)
{
   SetPreferenceDataP(aPrefData.value<PREF_DATA>());
   Apply();
}

template<typename PREF_DATA>
void PrefObjectT<PREF_DATA>::RestoreDefaults()
{
   SetPreferenceDataP(mDefaultPrefs);
   Apply();
   emit PreferencesChanged(QVariant::fromValue(mCurrentPrefs));
}

namespace detail
{
class WKF_EXPORT PrefObjectScopeGuard
{
public:
   PrefObjectScopeGuard(QSettings& aSettings, const PrefObject& aObject) noexcept;
   ~PrefObjectScopeGuard() noexcept;

   PrefObjectScopeGuard(const PrefObjectScopeGuard&) = delete;
   PrefObjectScopeGuard(PrefObjectScopeGuard&&)      = delete;

   void operator=(const PrefObjectScopeGuard&) = delete;
   void operator=(PrefObjectScopeGuard&&) = delete;

private:
   QSettings& mSettings;
   QString    mGroup;
};
} // namespace detail

template<typename PREF_DATA>
void PrefObjectT<PREF_DATA>::LoadSettings(QSettings& aAppSettings, QSettings& aSharedSettings)
{
   try
   {
      QSettings&                   settings = IsSharedPreference() ? aSharedSettings : aAppSettings;
      detail::PrefObjectScopeGuard scopeGuard{settings, *this};
      SetPreferenceDataP(ReadSettings(settings));
   }
   catch (std::exception&)
   {
      // PrefObjectScopeGuard handles logic errors, but is unable to catch exceptions.
   }
   Apply();
   emit PreferencesChanged(QVariant::fromValue(mCurrentPrefs));
}

template<typename PREF_DATA>
void PrefObjectT<PREF_DATA>::SaveSettings(QSettings& aAppSettings, QSettings& aSharedSettings) const
{
   try
   {
      QSettings&                   settings = IsSharedPreference() ? aSharedSettings : aAppSettings;
      detail::PrefObjectScopeGuard scopeGuard{settings, *this};
      SaveSettingsP(settings);
   }
   catch (std::exception&)
   {
      // PrefObjectScopeGuard handles logic errors, but is unable to catch exceptions.
   }
}
} // namespace wkf
#endif
