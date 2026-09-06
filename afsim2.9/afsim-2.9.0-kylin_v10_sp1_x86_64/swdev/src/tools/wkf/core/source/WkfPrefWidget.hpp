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

#ifndef WkfPrefWidget_HPP
#define WkfPrefWidget_HPP

#include "wkf_export.h"

#include <QWidget>

#include "UtMemory.hpp"
#include "WkfPrefObject.hpp"

namespace wkf
{
//! PrefWidget is an interface for a widget that presents a set of options to the user within the Preferences dialog.
class WKF_EXPORT PrefWidget : public QWidget
{
   Q_OBJECT // Necessary for QObject function templates, e.g. qobject_cast

      public :
      PrefWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
      : QWidget(parent, f)
   {
   }

   ~PrefWidget() override = default;

   // This returns the requested Category to place the Pref Widget in when displaying the widget in the Preferences Dialog
   virtual QString GetCategoryHint() const { return QString(); }
   // If True, this PrefWidget will be shown when the Category item is selected in Preferences.
   virtual bool GetCategoryHintPriority() const { return false; }

   //! Apply the changes that were made in the widget (e.g. if Apply or OK button is clicked)
   virtual void ApplyChanges() = 0;
   //! Reject any changes made in the widget (e.g. if Cancel button is clicked)
   virtual void RejectChanges() = 0;
   //! Restore the preferences back to their 'default' state
   virtual void RestoreDefaults() = 0;
};

//! Class template PrefWidgetT extends PrefWidget by managing an underlying PrefObject (the template argument). Most
//! preference widgets will derive from an implicit instantiation of this template, and implement the two pure virtual
//! methods to read and write the preference data.
template<class PREF_OBJECT>
class PrefWidgetT : public PrefWidget
{
   static_assert(std::is_base_of<PrefObject, PREF_OBJECT>::value,
                 "Template argument to PrefWidgetT must be a subclass of PrefObject");

public:
   PrefWidgetT(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
      : PrefWidget(parent, f)
      , mPrefObjectPtr(ut::make_unique<PREF_OBJECT>())
   {
      connect(mPrefObjectPtr.get(),
              &PrefObject::PreferencesChanged,
              [this](const QVariant& var) { ReadPreferenceData(var.value<PrefDataType>()); });
   }

   const PREF_OBJECT* GetPreferenceObject() const { return mPrefObjectPtr.get(); }
   PREF_OBJECT*       GetPreferenceObject() { return mPrefObjectPtr.get(); }

protected:
   using PrefDataType   = typename PREF_OBJECT::PrefDataType;
   using BasePrefWidget = PrefWidgetT<PREF_OBJECT>;

   std::unique_ptr<PREF_OBJECT> mPrefObjectPtr;

   void ApplyChanges() override;
   void RejectChanges() override;
   void RestoreDefaults() override;

   //! Update the widget to reflect the preference data
   virtual void ReadPreferenceData(const PrefDataType& aPrefData) = 0;
   //! Write the preference data to reflect the state of the widget
   virtual void WritePreferenceData(PrefDataType& aPrefData) = 0;
};

template<class PREF_OBJECT>
void PrefWidgetT<PREF_OBJECT>::ApplyChanges()
{
   PrefDataType pd = mPrefObjectPtr->GetPreferences();
   WritePreferenceData(pd);
   mPrefObjectPtr->SetPreferenceData(QVariant::fromValue(pd));
}

template<class PREF_OBJECT>
void PrefWidgetT<PREF_OBJECT>::RejectChanges()
{
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

template<class PREF_OBJECT>
void PrefWidgetT<PREF_OBJECT>::RestoreDefaults()
{
   mPrefObjectPtr->RestoreDefaults();
}
} // namespace wkf
#endif
