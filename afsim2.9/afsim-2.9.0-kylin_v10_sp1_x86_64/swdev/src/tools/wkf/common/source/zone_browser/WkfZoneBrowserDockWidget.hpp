// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WkDockWidgetZoneBrowser_HPP
#define WkDockWidgetZoneBrowser_HPP

#include "wkf_common_export.h"

#include <set>

#include <QCheckBox>
#include <QDockWidget>
#include <QLabel>

#include "UtQtColorButton.hpp"
#include "UtRandom.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfObserver.hpp"
#include "WkfZoneBrowserDataContainer.hpp"
#include "ui_WkfZoneBrowserDockWidget.h"

namespace vespa
{
class VaEntity;
} // namespace vespa

namespace wkf
{
class Platform;
class AttachmentZone;

struct AttachmentZoneStruct
{
   AttachmentZone* mZonePtr;
   double          mMinAltitude;
   bool            mAttachedToOwner;
   std::string     mReferenceName;
};

struct ZoneIdentifier
{
   std::string                       mPlatformName;
   std::string                       mZoneName;
   std::vector<AttachmentZoneStruct> mZones;
   std::set<std::string>             mReferenceNames;
};

class WKF_COMMON_EXPORT ZoneSelectionWidget : public QWidget
{
   Q_OBJECT
public:
   ZoneSelectionWidget(QString aZoneName, QWidget* aParentPtr = nullptr);

   std::string      GetName() const { return mNameLabelPtr->text().toStdString(); }
   QCheckBox*       mVisibilityBoxPtr;
   UtQtColorButton* mLineColorButtonPtr;
   UtQtColorButton* mFillColorButtonPtr;
   QLabel*          mNameLabelPtr;

   // Identifying variables used for easier removal of associated zones
   std::string mOwningPlatformName;
   std::string mReferencePlatformName;
};

class WKF_COMMON_EXPORT ZoneBrowserDockWidget : public QDockWidget
{
   Q_OBJECT
public:
   ZoneBrowserDockWidget(ZoneBrowserDataContainer& aDataContainer,
                         QWidget*                  aParentPtr = nullptr,
                         Qt::WindowFlags           aFlags     = Qt::WindowFlags());
   ~ZoneBrowserDockWidget() override = default;

   virtual void ApplyRandomColorPreferences();
   virtual void ApplySetColorPreferences(const QColor& aLineColor, const QColor& aFillColor);
   virtual void PlatformAdded(const std::string& aPlatform);

protected:
   void         ClearData();
   virtual void RemovePlatform(const std::string& aPlatformName);
   void         AddZoneToBrowser(QTreeWidgetItem* aParentPtr, ZoneSetData* aZoneDataPtr, bool aSetChecked);
   virtual void DrawZone(const std::string& aPlatform,
                         const std::string& aName,
                         const QColor&      aLineColor,
                         const QColor&      aFillColor,
                         bool               aPreview = false);
   virtual void HideZone(const std::string& aPlatform, const std::string& aName);

   virtual void AddZone(ZoneSetData* aDataPtr, bool aShow = false);
   virtual void DeleteZone(ZoneSetData* aDataPtr);

   virtual void ZoneUpdatedCB(const std::string& aZoneName, const std::string& aPlatformName);
   void         PreviewUpdatedCB();
   virtual void PreviewPendingDeletionCB();

   virtual QColor GetFillColor(bool aRandom = false);

   virtual QColor GetLineColor(bool aRandom = false);

   void         CheckAllZoneBoxes(Qt::CheckState aCheckState);
   virtual void SetZoneLineColor(const std::string& aPlatform, const std::string& aName, const QColor& aColor);
   virtual void SetZoneFillColor(const std::string& aPlatform, const std::string& aName, const QColor& aColor);
   virtual bool GetZoneLineColor(const std::string& aPlatform, const std::string& aName, QColor& aColor);
   virtual bool GetZoneFillColor(const std::string& aPlatform, const std::string& aName, QColor& aColor);
   virtual void GetAnchorEntityData(const std::string&                       aPlatform,
                                    const std::string&                       aName,
                                    ZoneSetData*&                            aDataPtr,
                                    std::map<std::string, vespa::VaEntity*>& aAnchors,
                                    bool                                     aPreview = false);
   virtual vespa::VaEntity* GetActiveAnchor(const std::string& aReferenceName);
   void                     EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving);
   void                     ScenarioRemovedCB(wkf::Scenario* aScenarioPtr) { mEntityTracker.clear(); }
   // void RefEntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving);
   void           VisibilityChangedHandler();
   virtual void   AttachmentDeletedHandler(vespa::VaAttachment* aAttachmentPtr, const vespa::VaAttachmentSubId& aId);
   Qt::CheckState CalculateAllBoxState() const;

   Ui::ZoneBrowserDockWidget mUI;
   ZoneBrowserDataContainer& mDataContainer;

   // Keep track of various properties of the last removed zone for use in the ZoneUpdated callback.
   // Note: If the datacontainer's ZoneUpdated function actually updates the zone instead of removing it entirely and
   // creating a new one, these maybe can be removed entirely in the future
   bool   mPrevVisible;
   QColor mPrevLineColor;
   QColor mPrevFillColor;
   bool   mPrevExpanded{false};
   int    mPrevIndex{0};
   int    mPrevChildIndex{0};

   std::vector<ZoneIdentifier>   mDrawZones; // track which zones are currently displayed
   ZoneIdentifier                mPreviewZone;
   ut::Random                    mRandom;        // used to "randomly" assign initial color values in a reproducible way
   std::vector<vespa::VaEntity*> mEntityTracker; // track entities that are not platforms
   QColor                        mDefaultLineColor;
   QColor                        mDefaultFillColor;
   bool                          mButtonClicked{false};

   std::unordered_map<std::string, std::set<std::pair<std::string, std::string>>>
      mDelayedZones; // Key = reference platform, Value = list of parent/zone name combinations that reference it

   vespa::VaCallbackHolder                                   mCallbacks;
   std::map<unsigned int, std::vector<int>>                  mCallbackMap;
   std::map<std::string, vespa::VaEntity*>                   mAnchorZoneMap;
   std::map<unsigned int, std::vector<AttachmentZoneStruct>> mAttachmentZoneMap;

   std::map<std::pair<std::string, std::string>, QColor> mCustomLineColors;
   std::map<std::pair<std::string, std::string>, QColor> mCustomFillColors;

private:
   void GetReferenceAnchor(const std::string& aReferenceName, vespa::VaEntity*& aAnchorPtr);
};
} // namespace wkf

#endif
