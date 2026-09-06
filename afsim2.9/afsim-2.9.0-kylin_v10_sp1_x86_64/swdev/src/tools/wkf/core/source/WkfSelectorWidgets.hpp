// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WkfSelectorWidgetBase_HPP
#define WkfSelectorWidgetBase_HPP

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

#include "UtQtUnitLineEdit.hpp"
#include "VaCallbackHolder.hpp"
namespace vespa
{
class VaViewer;
}

#include "wkf_export.h"

#include "WkfPlatform.hpp"
#include "WkfTrack.hpp"

namespace wkf
{
// Base class for Selector Widgets
class WKF_EXPORT SelectorWidgetBase : public QPushButton
{
   Q_OBJECT

public:
   SelectorWidgetBase(QWidget* aParent, const QString& aToolTip);
   ~SelectorWidgetBase() override;

   // Allows other classes to terminate the selection
   void CancelSelection();

   // Prevents selection from terminating after the first selection
   void SetMultiSelectionEnabled(bool aMultiSelect) { mMultiSelectionEnabled = aMultiSelect; }

signals:
   void SelectionStarted();
   void SelectionTerminated();

protected:
   virtual void UpdateSelection(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY) {}
   virtual void Clicked(int aButton, int aMouseX, int aMouseY, int aState) {}

   bool eventFilter(QObject* aObject, QEvent* aEvent) override;

   void HandleButtonClicked();
   void StartSelection();
   bool ClickedP(vespa::VaViewer* aViewerPtr, int aButton, int aMouseX, int aMouseY, int aState);

   void Revert();

   bool mSelectionActive;
   bool mMultiSelectionEnabled;

   vespa::VaCallback* mUpdateSelectionCallback;
   vespa::VaCallback* mEndSelectionCallback;
   vespa::VaViewer*   mViewer;
};

// A widget that when clicked, allows to user to select a Location from a VaViewer (World Map)
class WKF_EXPORT LocationSelector : public SelectorWidgetBase
{
   Q_OBJECT

public:
   LocationSelector(QWidget* aParent = nullptr)
      : SelectorWidgetBase(aParent, "Select a location on the map or use right-click to cancel.")
   {
   }

signals:
   // emitted when the user picks a location on the viewer
   void Selected(double aLatitude, double aLongitude, double aAltitude_m, bool aAltitudeValid);
   // emitted whenever the mouse is moved over the viewer
   void LocationChanged(double aLatitude, double aLongitude);

protected:
   void UpdateSelection(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY) override;
   void Clicked(int aButton, int aMouseX, int aMouseY, int aState) override;
};

// A widget that when clicked, allows to user to select a WkfPlatform from a VaViewer (World Map)
class WKF_EXPORT PlatformSelector : public SelectorWidgetBase
{
   Q_OBJECT

public:
   PlatformSelector(QWidget* aParent = nullptr)
      : SelectorWidgetBase(aParent, "Select a platform on the map or use right-click to cancel.")
   {
   }

   void SetFilter(const QString& aFilterType, const QString& aValue, bool aIsInclusive);

signals:
   void Selected(Platform* aPlatform);

protected:
   void Clicked(int aButton, int aMouseX, int aMouseY, int aState) override;
   void ValidateFilter();
   bool Evaluate(const Platform* aPlatformPtr);

   std::string mFilterType;
   std::string mFilterValue;
   bool        mIsInclusive{true};
};

// A widget that when clicked, allows to user to select a WkfTrack from a VaViewer (World Map)
class WKF_EXPORT TrackSelector : public SelectorWidgetBase
{
   Q_OBJECT

public:
   TrackSelector(QWidget* aParent = nullptr)
      : SelectorWidgetBase(aParent, "Select a track on the map or use right-click to cancel.")
   {
   }

   void SetFilter(const QString& aFilterType, const QString& aFilterValue, bool aIsInclusive);

signals:
   void Selected(Track* aTrack);

protected:
   void Clicked(int aButton, int aMouseX, int aMouseY, int aState) override;
   void ValidateFilter();
   bool Evaluate(const Track* aTrackPtr);

   std::string mFilterType;
   std::string mFilterValue;
   bool        mIsInclusive{true};
};

// Complex widget that contains line edits for the values of a Location,
// along with a LocationSelector which a user can use to change the current Location
class WKF_EXPORT LocationWidget : public QWidget
{
public:
   LocationWidget(QWidget* aParent = nullptr);

   QVector<double> GetLocation() const;
   void            SetLocation(const QVector<double>& aLLA);
   bool            LocationValid() { return mLatLineEdit->IsValid() && mLonLineEdit->IsValid(); }

private:
   UtQtLatPosLineEdit* mLatLineEdit;
   UtQtLonPosLineEdit* mLonLineEdit;
   QLineEdit*          mAltitudeLineEdit;
   QLabel*             mAltitudeUnitLabel;
};

// Complex widget that contains a QLineEdit for the name of a WkfPlatform,
// along with a PlatformSelector which a user can use to change the current platform
class WKF_EXPORT PlatformWidget : public QWidget
{
   // The data that is edited by an editor is stored in the editor's user data property, and the delegate uses Qt's
   // property system to access it by name.
   //  We declare our user data property with the Q_PROPERTY macro
   Q_OBJECT
   Q_PROPERTY(QString platformName READ GetPlatformName WRITE SetPlatformName USER true)

public:
   PlatformWidget(QWidget* aParent = nullptr);

   void SetPlatformName(const QString& aName)
   {
      mLineEdit->setText(aName);
      emit PlatformChanged(aName);
   }
   void SetFilter(const QString& aFilterType, const QString& aValue, bool aIsInclusive)
   {
      mSelectorPtr->SetFilter(aFilterType, aValue, aIsInclusive);
   }

   QString GetPlatformName() const { return mLineEdit->text(); }
signals:
   void PlatformChanged(const QString& aName);

private:
   PlatformSelector* mSelectorPtr;
   QLineEdit*        mLineEdit;
};

// Complex widget that contains a QLineEdit for the id of a WkfTrack,
// along with a TrackSelector which a user can use to change the current track
class WKF_EXPORT TrackWidget : public QWidget
{
public:
   TrackWidget(QWidget* aParent = nullptr);

   void         SetTrackId(const wkf::TrackId& aId);
   wkf::TrackId GetTrackId() const;

   void SetFilter(const QString& aFilterType, const QString& aFilterValue, bool aIsInclusive)
   {
      mSelectorPtr->SetFilter(aFilterType, aFilterValue, aIsInclusive);
   }
   void Clear() { mLineEdit->clear(); }

private:
   TrackSelector* mSelectorPtr;
   QLineEdit*     mLineEdit;
};
} // namespace wkf


#endif
