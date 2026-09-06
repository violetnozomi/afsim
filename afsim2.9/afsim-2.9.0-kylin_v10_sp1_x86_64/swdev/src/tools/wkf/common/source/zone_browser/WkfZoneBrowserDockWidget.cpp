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

#include "WkfZoneBrowserDockWidget.hpp"

#include "UtQt.hpp"
#include "UtQtMemory.hpp"
#include "UtUnitTypes.hpp"
#include "VaEnvironment.hpp"
#include "VaScenario.hpp"
#include "VaUtils.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "zone/WkfAttachmentZone.hpp"
#include "zone/WkfAttachmentZoneCircle.hpp"
#include "zone/WkfAttachmentZoneEllipse.hpp"
#include "zone/WkfAttachmentZonePolygon.hpp"
#include "zone/WkfAttachmentZoneSphere.hpp"
#include "zone/WkfZonePoint.hpp"

namespace
{
unsigned int cRANDOM_SEED = 15;

bool ZoneMatch(const std::string& aPlatform, const std::string& aName, const wkf::ZoneIdentifier& aZone)
{
   return (aPlatform == aZone.mPlatformName && aName == aZone.mZoneName);
}

void UpdateItemVisibility(QTreeWidgetItem* aItemPtr, const std::string& aPlatformName)
{
   if (aPlatformName == wkf::cGLOBAL_NAME)
   {
      aItemPtr->setHidden(false);
   }
   else
   {
      wkf::Platform* platformPtr = nullptr;
      auto*          scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr != nullptr)
      {
         platformPtr = scenarioPtr->FindPlatformByName(aPlatformName);
      }
      aItemPtr->setHidden(!wkfEnv.IsPlatformVisible(platformPtr));
   }

   // What is the purpose of the following code?
   if (aItemPtr->isExpanded())
   {
      // this is essentially a redraw of the dropdown menu,
      //  complications with how the dropdown is implemented make it difficult to do otherwise
      aItemPtr->setExpanded(false);
      aItemPtr->setExpanded(true);
   }
}

void UpdateZoneWidgetState(const std::string& aPlatformName, wkf::ZoneSelectionWidget* aZoneWidget)
{
   if (!aPlatformName.empty())
   {
      if (aPlatformName == wkf::cGLOBAL_NAME)
      {
         aZoneWidget->setEnabled(true);
         aZoneWidget->setHidden(false);
      }
      else
      {
         wkf::Platform* platformPtr{nullptr};
         auto*          scenarioPtr = vaEnv.GetStandardScenario();
         if (scenarioPtr != nullptr)
         {
            platformPtr = scenarioPtr->FindPlatformByName(aPlatformName);
         }

         if (wkfEnv.IsPlatformVisible(platformPtr))
         {
            aZoneWidget->setEnabled(true);
            aZoneWidget->setHidden(false);
         }
         else
         {
            aZoneWidget->mVisibilityBoxPtr->setChecked(false);
            aZoneWidget->setEnabled(false);
            aZoneWidget->setHidden(true);
         }
      }
   }
};

void SetCommonZoneAttachmentData(wkf::AttachmentZone* aZonePtr,
                                 wkf::ZoneData*       aDataPtr,
                                 vespa::VaEntity*&    aAnchorPtr,
                                 const QColor&        aLineColor,
                                 const QColor&        aFillColor)
{
   aZonePtr->Initialize();
   aZonePtr->Load();
   aZonePtr->SetAuxData(aDataPtr->GetAuxData());
   double headingOffset = aDataPtr->GetHeading() - aAnchorPtr->GetHeading();
   double pitchOffset   = 0;
   double rollOffset    = 0;
   aZonePtr->SetOrientation(headingOffset, pitchOffset, rollOffset);
   aZonePtr->SetBaseHeight(aDataPtr->GetMinAlt() - aAnchorPtr->GetPosition().GetAlt());
   aZonePtr->SetHeight(aDataPtr->GetMaxAlt() - aDataPtr->GetMinAlt());
   aZonePtr->SetMaxAltitudeSet(aDataPtr->IsMaxAltSet());
   aZonePtr->SetMinAltitudeSet(aDataPtr->IsMinAltSet());
   aZonePtr->SetMaxAltitude(aDataPtr->GetMaxAlt());
   aZonePtr->SetMinAltitude(aDataPtr->GetMinAlt());
   aZonePtr->SetColor(UtQtColor(aLineColor));
   aZonePtr->SetEnableFill(true);
   aZonePtr->SetBlendFactor(1 - aFillColor.alphaF());
   aZonePtr->SetFillColor(UtQtColor(aFillColor));
   aZonePtr->SetStateVisibility(true);
}


void SetCircularZoneAttachmentData(wkf::AttachmentZoneCircle* aCircleZonePtr, wkf::CircularZoneData* aCircularZoneData)
{
   aCircleZonePtr->SetRadius(aCircularZoneData->GetRadius());
   aCircleZonePtr->SetMinRadius(aCircularZoneData->GetMinRadius());
   // The Uto Shape implementation expects angles to be in degrees
   aCircleZonePtr->SetBegAngle(UtUnitAngle::ConvertFromStandard(aCircularZoneData->GetStartAngle(), UtUnitAngle::cDEGREES));
   aCircleZonePtr->SetEndAngle(UtUnitAngle::ConvertFromStandard(aCircularZoneData->GetStopAngle(), UtUnitAngle::cDEGREES));
}

void SetEllipticalZoneAttachmentData(wkf::AttachmentZoneEllipse* aEllipseZonePtr,
                                     wkf::EllipticalZoneData*    aEllipticalZoneData)
{
   // Elliptical Zones in AFSIM store the axis' length values as the diameter, not the semi-major / semi-minor lengths.
   // The zones on the visual side use radius (semi-major, semi-minor) length values, so divide by 2.
   aEllipseZonePtr->SetLonAxisRadius(aEllipticalZoneData->GetLonAxis() / 2);
   aEllipseZonePtr->SetLatAxisRadius(aEllipticalZoneData->GetLatAxis() / 2);
   aEllipseZonePtr->SetMinRadius(aEllipticalZoneData->GetMinRadius());
   // The Uto Shape implementation expects angles to be in degrees
   aEllipseZonePtr->SetBegAngle(
      UtUnitAngle::ConvertFromStandard(aEllipticalZoneData->GetStartAngle(), UtUnitAngle::cDEGREES));
   aEllipseZonePtr->SetEndAngle(
      UtUnitAngle::ConvertFromStandard(aEllipticalZoneData->GetStopAngle(), UtUnitAngle::cDEGREES));
}

void SetSphericalZoneAttachmentData(wkf::AttachmentZoneSphere* aSphereZonePtr, wkf::SphericalZoneData* aSphericalZoneData)
{
   aSphereZonePtr->SetRadius(aSphericalZoneData->GetRadius());
   aSphereZonePtr->SetMinRadius(aSphericalZoneData->GetMinRadius());
   // The Uto Shape implementation expects angles to be in degrees
   aSphereZonePtr->SetBegAngle(UtUnitAngle::ConvertFromStandard(aSphericalZoneData->GetStartAngle(), UtUnitAngle::cDEGREES));
   aSphereZonePtr->SetEndAngle(UtUnitAngle::ConvertFromStandard(aSphericalZoneData->GetStopAngle(), UtUnitAngle::cDEGREES));
}

void SetPolygonalZoneAttachmentData(wkf::AttachmentZonePolygon* aPolygonZonePtr,
                                    wkf::PolygonalZoneData*     aPolygonalDataPtr,
                                    double                      aAnchorAlt,
                                    bool                        aPreview = false)
{
   if (aPolygonalDataPtr->UseLatLon())
   {
      aPolygonZonePtr->SetRelative(false);
      aPolygonZonePtr->SetBaseHeight(aPolygonalDataPtr->GetMinAlt());
      aPolygonZonePtr->SetHeight(aPolygonalDataPtr->GetMaxAlt() - aPolygonalDataPtr->GetMinAlt());
      for (const auto& point : aPolygonalDataPtr->GetZonePoints())
      {
         vespa::VaPosition pos;
         pos.SetPositionIsXY(false);
         pos.SetLat(point.mX);
         pos.SetLon(point.mY);
         // Set the altitude of each point in the polygon to account for differences between point altitude and
         // anchor/originating platform altitude
         pos.SetAlt(aPolygonalDataPtr->GetMinAlt());
         wkf::ZonePoint* zonePointPtr = new wkf::ZonePoint(*aPolygonZonePtr, pos);
         aPolygonZonePtr->AddZonePoint(zonePointPtr, vaEnv.GetStandardViewer());
      }
   }
   else
   {
      // Altitude and height are negative for XY due to differences in frame of reference
      aPolygonZonePtr->SetRelative(true);
      aPolygonZonePtr->SetBaseHeight(aAnchorAlt - aPolygonalDataPtr->GetMinAlt());
      aPolygonZonePtr->SetHeight(-(aPolygonalDataPtr->GetMaxAlt() - aPolygonalDataPtr->GetMinAlt()));
      for (const auto& it : aPolygonalDataPtr->GetZonePoints())
      {
         vespa::VaPosition pos;
         pos.SetPositionIsXY(true);

         // Reflect points across the x-axis to keep consistent with how AFSIM handles relative zones.
         pos.SetX(-it.mX);
         pos.SetY(it.mY);
         pos.SetAlt(aAnchorAlt - aPolygonalDataPtr->GetMinAlt());
         wkf::ZonePoint* zonePointPtr = new wkf::ZonePoint(*aPolygonZonePtr, pos);
         aPolygonZonePtr->AddZonePoint(zonePointPtr, vaEnv.GetStandardViewer());
      }
   }
   aPolygonZonePtr->SetClosed(true);
   if (!aPreview)
   {
      aPolygonZonePtr->SetNeverShowZones(true);
      aPolygonZonePtr->SetHideZonePoints(true);
   }
   // reload/build is necessary to avoid unusual rotation behavior
   aPolygonZonePtr->Load();
}
} // namespace

wkf::ZoneSelectionWidget::ZoneSelectionWidget(QString aZoneName, QWidget* parent)
   : QWidget(parent)
   , mVisibilityBoxPtr(ut::qt::make_qt_ptr<QCheckBox>(this))
   , mLineColorButtonPtr(ut::qt::make_qt_ptr<UtQtColorButton>(this))
   , mFillColorButtonPtr(ut::qt::make_qt_ptr<UtQtColorButton>(this))
   , mNameLabelPtr(ut::qt::make_qt_ptr<QLabel>(aZoneName))
   , mOwningPlatformName("")
   , mReferencePlatformName("")
{
   mVisibilityBoxPtr->setFixedWidth(16);
   mLineColorButtonPtr->setFixedWidth(30);
   mLineColorButtonPtr->ShowAlpha(true);
   mLineColorButtonPtr->SetColorWindowTitle("Change Line Color");
   mFillColorButtonPtr->setFixedWidth(30);
   mFillColorButtonPtr->ShowAlpha(true);
   mFillColorButtonPtr->SetColorWindowTitle("Change Fill Color");

   QLabel*      lineLabel = ut::qt::make_qt_ptr<QLabel>("Line Color:");
   QLabel*      fillLabel = ut::qt::make_qt_ptr<QLabel>("Fill Color:");
   QSpacerItem* spacer    = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

   QHBoxLayout* horizontalBox = ut::qt::make_qt_ptr<QHBoxLayout>();
   QGridLayout* gridLayout    = ut::qt::make_qt_ptr<QGridLayout>();
   horizontalBox->addWidget(lineLabel);
   horizontalBox->addWidget(mLineColorButtonPtr);
   horizontalBox->addWidget(fillLabel);
   horizontalBox->addWidget(mFillColorButtonPtr);
   horizontalBox->addSpacerItem(spacer);

   gridLayout->addWidget(mVisibilityBoxPtr, 0, 0);
   gridLayout->addWidget(mNameLabelPtr, 0, 1);
   gridLayout->addLayout(horizontalBox, 1, 1);

   horizontalBox->setContentsMargins(0, 0, 0, 0);
   gridLayout->setContentsMargins(0, 0, 0, 0);
   setLayout(gridLayout);
}

wkf::ZoneBrowserDockWidget::ZoneBrowserDockWidget(wkf::ZoneBrowserDataContainer& aDataContainer,
                                                  QWidget*                       aParentPtr,
                                                  Qt::WindowFlags                aFlags)
   : QDockWidget(aParentPtr, aFlags)
   , mDataContainer(aDataContainer)
   , mPrevVisible(false)
   , mPrevLineColor()
   , mPrevFillColor()
   , mDefaultLineColor()
   , mDefaultFillColor()
{
   mUI.setupUi(this);
   mUI.mTreeWidget->setHeaderHidden(true);
   mUI.gridLayout_2->setContentsMargins(0, 0, 0, 0);
   connect(&mDataContainer, &ZoneBrowserDataContainer::ClearZoneData, this, &ZoneBrowserDockWidget::ClearData);
   connect(&mDataContainer, &ZoneBrowserDataContainer::PlatformRemoved, this, &ZoneBrowserDockWidget::RemovePlatform);
   connect(&mDataContainer, &ZoneBrowserDataContainer::ZoneAdded, this, &ZoneBrowserDockWidget::AddZone);
   connect(&mDataContainer, &ZoneBrowserDataContainer::ZoneColorUpdated, this, &ZoneBrowserDockWidget::ZoneUpdatedCB);
   connect(&mDataContainer, &ZoneBrowserDataContainer::PreviewUpdated, this, &ZoneBrowserDockWidget::PreviewUpdatedCB);
   connect(&mDataContainer,
           &ZoneBrowserDataContainer::PreviewPendingDeletion,
           this,
           &ZoneBrowserDockWidget::PreviewPendingDeletionCB);
   connect(&mDataContainer, &ZoneBrowserDataContainer::ZoneDeleted, this, &ZoneBrowserDockWidget::DeleteZone);
   connect(&wkfEnv, &wkf::Environment::PlatformVisibilityChanged, this, &ZoneBrowserDockWidget::VisibilityChangedHandler);
   mCallbacks.Add(wkf::Observer::StandardScenarioRemoved.Connect(&wkf::ZoneBrowserDockWidget::ScenarioRemovedCB, this));
   mCallbacks.Add(wkf::Observer::AttachmentDeleted.Connect(&ZoneBrowserDockWidget::AttachmentDeletedHandler, this));
   connect(mUI.mAllCheckBox,
           &QCheckBox::clicked,
           this,
           [&](bool aChecked)
           {
              Qt::CheckState state;
              aChecked ? state = Qt::Checked : state = Qt::Unchecked;
              CheckAllZoneBoxes(static_cast<Qt::CheckState>(state));
           });
}

void wkf::ZoneBrowserDockWidget::ApplyRandomColorPreferences()
{
   QList<ZoneSelectionWidget*> widgetList = mUI.mTreeWidget->findChildren<ZoneSelectionWidget*>();
   mRandom.SetSeed(cRANDOM_SEED);
   for (const auto& widgetPtr : widgetList)
   {
      widgetPtr->mLineColorButtonPtr->SetColor(GetLineColor(true));
      widgetPtr->mFillColorButtonPtr->SetColor(GetFillColor(true));
      emit widgetPtr->mLineColorButtonPtr->ColorChanged(widgetPtr->mLineColorButtonPtr->GetColor());
      emit widgetPtr->mFillColorButtonPtr->ColorChanged(widgetPtr->mFillColorButtonPtr->GetColor());
   }
}

void wkf::ZoneBrowserDockWidget::ApplySetColorPreferences(const QColor& aLineColor, const QColor& aFillColor)
{
   mRandom.SetSeed(0);
   mDefaultLineColor                      = aLineColor;
   mDefaultFillColor                      = aFillColor;
   QList<ZoneSelectionWidget*> widgetList = mUI.mTreeWidget->findChildren<ZoneSelectionWidget*>();
   for (const auto& widgetPtr : widgetList)
   {
      widgetPtr->mLineColorButtonPtr->SetColor(mDefaultLineColor);
      widgetPtr->mFillColorButtonPtr->SetColor(mDefaultFillColor);
      emit widgetPtr->mLineColorButtonPtr->ColorChanged(mDefaultLineColor);
      emit widgetPtr->mFillColorButtonPtr->ColorChanged(mDefaultFillColor);
   }
}

void wkf::ZoneBrowserDockWidget::ClearData()
{
   mUI.mTreeWidget->clear();
   for (auto entityPtr : mEntityTracker)
   {
      vaEnv.DeleteEntity(entityPtr);
   }
   mEntityTracker.clear();
   mAnchorZoneMap.clear();
   mAttachmentZoneMap.clear();
   mDelayedZones.clear();
   mDrawZones.clear();
   mCustomFillColors.clear();
   mCustomLineColors.clear();
   mRandom.SetSeed(0);
   mUI.mAllCheckBox->setChecked(false);
}

void wkf::ZoneBrowserDockWidget::RemovePlatform(const std::string& aPlatformName)
{
   // stop maintaining the WkfAttachmentZone belonging to or referencing the deleted platform
   auto removalLambda = [&, aPlatformName](ZoneIdentifier& aZoneData)
   {
      bool retVal{false};
      if (aZoneData.mPlatformName == aPlatformName)
      {
         retVal = true;
         for (auto& zoneAttachment : aZoneData.mZones)
         {
            // If this zone wasn't attached to its parent platform (e.g. it was a nonrelative polygonal zone),
            // delete the anchor entity. Otherwise, remove the attachment.
            if (zoneAttachment.mZonePtr)
            {
               if (dynamic_cast<wkf::Platform*>(&zoneAttachment.mZonePtr->GetParent()) == nullptr)
               {
                  auto* scenarioPtr = vaEnv.GetStandardScenario();
                  if (scenarioPtr)
                  {
                     scenarioPtr->DeleteEntity(&zoneAttachment.mZonePtr->GetParent());
                  }
                  break;
               }
               else
               {
                  zoneAttachment.mZonePtr->GetParent().RemoveAttachment(zoneAttachment.mZonePtr->GetUniqueId());
               }
            }
         }
      }
      // Account for zones associated with other platforms (or global) that are drawn on platform references
      else if (aZoneData.mReferenceNames.count(aPlatformName) > 0)
      {
         aZoneData.mZones.erase(std::remove_if(aZoneData.mZones.begin(),
                                               aZoneData.mZones.end(),
                                               [&](wkf::AttachmentZoneStruct& aZone)
                                               {
                                                  if (aZone.mReferenceName == aPlatformName)
                                                  {
                                                     aZone.mZonePtr->GetParent().RemoveAttachment(
                                                        aZone.mZonePtr->GetUniqueId());
                                                     return true;
                                                  }
                                                  return false;
                                               }),
                                aZoneData.mZones.end());
      }
      return retVal;
   };
   mDrawZones.erase(std::remove_if(mDrawZones.begin(), mDrawZones.end(), removalLambda), mDrawZones.end());
   mAnchorZoneMap.erase(aPlatformName);

   // Remove the deleted platform from the other maps that are tracking it
   wkf::Platform* plat = vaEnv.GetStandardScenario()->FindPlatformByName(aPlatformName);
   if (plat && mAttachmentZoneMap.count(plat->GetUniqueId()) > 0)
   {
      mAttachmentZoneMap.erase(plat->GetUniqueId());
   }
   mAnchorZoneMap.erase(aPlatformName);
   if (plat)
   {
      for (const auto& cbIt : mCallbackMap[plat->GetUniqueId()])
      {
         mCallbacks.Remove(cbIt);
      }
      mCallbackMap.erase(plat->GetUniqueId());
   }

   // In the browser, get rid of any zones that reference the deleted platform
   QList<ZoneSelectionWidget*> menuOptions = mUI.mTreeWidget->findChildren<ZoneSelectionWidget*>();
   auto                        it          = menuOptions.begin();
   while (it != menuOptions.end())
   {
      if ((*it)->mReferencePlatformName == aPlatformName || (*it)->mOwningPlatformName == aPlatformName)
      {
         delete (*it);
         it = menuOptions.erase(it);
      }
      else
      {
         ++it;
      }
   }
   // in the browser, if the deleted platform had zones, get rid of the platform's entire subtree
   // Note: this does not delete child ZoneSelectionWidgets due to how the widget was constructed, so manually delete those
   for (int i = 0; i < mUI.mTreeWidget->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* itemPtr = mUI.mTreeWidget->topLevelItem(i);
      if (itemPtr->text(0).toStdString() == aPlatformName)
      {
         delete itemPtr;
         i--;
         continue;
      }
      if (itemPtr->isExpanded())
      {
         // this is essentially a redraw of the dropdown menu: complications with how the dropdown is implemented make
         // it difficult to do otherwise
         itemPtr->setExpanded(false);
         itemPtr->setExpanded(true);
      }
   }
}

void wkf::ZoneBrowserDockWidget::AddZoneToBrowser(QTreeWidgetItem* aParentPtr, ZoneSetData* aZoneDataPtr, bool aSetChecked)
{
   QTreeWidgetItem* zonePtr        = ut::qt::make_qt_ptr<QTreeWidgetItem>();
   QFrame*          groupFramePtr  = ut::qt::make_qt_ptr<QFrame>(mUI.mTreeWidget);
   QVBoxLayout*     verticalBoxPtr = ut::qt::make_qt_ptr<QVBoxLayout>(mUI.mTreeWidget);

   ZoneSelectionWidget* commandPtr =
      ut::qt::make_qt_ptr<ZoneSelectionWidget>(QString::fromStdString(aZoneDataPtr->GetSetName()));
   commandPtr->mOwningPlatformName = aZoneDataPtr->GetPlatformName();
   connect(commandPtr->mVisibilityBoxPtr,
           &QCheckBox::stateChanged,
           this,
           [&, aParentPtr, commandPtr](int aState)
           {
              if (aState == Qt::Checked)
              {
                 DrawZone(aParentPtr->text(0).toStdString(),
                          commandPtr->GetName(),
                          commandPtr->mLineColorButtonPtr->GetColor(),
                          commandPtr->mFillColorButtonPtr->GetColor());
              }
              else if (aState == Qt::Unchecked)
              {
                 HideZone(aParentPtr->text(0).toStdString(), commandPtr->GetName());
              }
              mUI.mAllCheckBox->setCheckState(CalculateAllBoxState());
           });

   connect(commandPtr->mLineColorButtonPtr, &UtQtColorButton::DialogClosed, this, [&]() { mButtonClicked = false; });
   connect(commandPtr->mFillColorButtonPtr, &UtQtColorButton::DialogClosed, this, [&]() { mButtonClicked = false; });
   connect(commandPtr->mLineColorButtonPtr, &UtQtColorButton::Clicked, this, [&]() { mButtonClicked = true; });
   connect(commandPtr->mFillColorButtonPtr, &UtQtColorButton::Clicked, this, [&]() { mButtonClicked = true; });

   QColor fillColor(GetFillColor(mRandom.GetSeed() == cRANDOM_SEED));
   QColor lineColor(GetLineColor(mRandom.GetSeed() == cRANDOM_SEED));

   auto   definedFill = aZoneDataPtr->GetFillColor();
   QColor oldColor;
   if (GetZoneFillColor(aParentPtr->text(0).toStdString(), commandPtr->GetName(), oldColor))
   {
      fillColor = oldColor;
   }
   else if (definedFill.has_value())
   {
      fillColor = UtQtColor(definedFill.value());
   }

   auto definedLine = aZoneDataPtr->GetLineColor();
   if (GetZoneLineColor(aParentPtr->text(0).toStdString(), commandPtr->GetName(), oldColor))
   {
      lineColor = oldColor;
   }
   else if (definedLine.has_value())
   {
      lineColor = UtQtColor(definedLine.value());
   }

   commandPtr->mFillColorButtonPtr->SetColor(fillColor);
   commandPtr->mLineColorButtonPtr->SetColor(lineColor);

   // Connect these afterwards so that the previous SetColor doesn't automattically call them.
   connect(commandPtr->mLineColorButtonPtr,
           &UtQtColorButton::ColorChanged,
           this,
           [&, aParentPtr, commandPtr](const QColor& aColor)
           {
              SetZoneLineColor(aParentPtr->text(0).toStdString(), commandPtr->GetName(), aColor);
              mButtonClicked = false;
           });
   connect(commandPtr->mFillColorButtonPtr,
           &UtQtColorButton::ColorChanged,
           this,
           [&, aParentPtr, commandPtr](const QColor& aColor)
           {
              SetZoneFillColor(aParentPtr->text(0).toStdString(), commandPtr->GetName(), aColor);
              mButtonClicked = false;
           });

   // If the zone is a reference to another, disable the changing of its color.
   if (!aZoneDataPtr->GetZones()[0]->GetSourceZone().empty())
   {
      QString warning("This is a reference zone and its colors must be changed on the zone it references.");
      commandPtr->mLineColorButtonPtr->DisableWithWarning(warning);
      commandPtr->mFillColorButtonPtr->DisableWithWarning(warning);
   }

   mPrevFillColor = fillColor;
   mPrevLineColor = lineColor;
   verticalBoxPtr->addWidget(commandPtr);

   if (aSetChecked || mUI.mAllCheckBox->checkState() == Qt::Checked)
   {
      commandPtr->mVisibilityBoxPtr->setChecked(true);
   }

   verticalBoxPtr->setContentsMargins(0, 0, 8, 0);
   groupFramePtr->setLayout(verticalBoxPtr);
   if (mPrevChildIndex < aParentPtr->childCount())
   {
      aParentPtr->insertChild(mPrevChildIndex, zonePtr);
   }
   else
   {
      aParentPtr->addChild(zonePtr);
   }
   mPrevChildIndex = aParentPtr->childCount();
   mUI.mTreeWidget->setItemWidget(zonePtr, 0, groupFramePtr);

   // Instead of calling VisibilityChangedHandler() which will loop over all the zones,
   //  explicitly update the visibility for just the zone we are adding.
   UpdateZoneWidgetState(commandPtr->mReferencePlatformName, commandPtr);
   UpdateZoneWidgetState(commandPtr->mOwningPlatformName, commandPtr);

   mUI.mAllCheckBox->setCheckState(CalculateAllBoxState());
}

void wkf::ZoneBrowserDockWidget::DrawZone(const std::string& aPlatform,
                                          const std::string& aName,
                                          const QColor&      aLineColor,
                                          const QColor&      aFillColor,
                                          bool               aPreview)
{
   ZoneIdentifier addedZone;

   auto setCommonDataLambda = [&, aPlatform, aName, aLineColor, aFillColor](wkf::AttachmentZone* aZonePtr,
                                                                            ZoneData*            aDataPtr,
                                                                            vespa::VaEntity*&    aAnchorPtr)
   {
      SetCommonZoneAttachmentData(aZonePtr, aDataPtr, aAnchorPtr, aLineColor, aFillColor);
      AttachmentZoneStruct attachmentZone;
      attachmentZone.mAttachedToOwner = !aDataPtr->GetLocationDefined();
      attachmentZone.mMinAltitude     = aDataPtr->GetMinAlt();
      attachmentZone.mZonePtr         = aZonePtr;
      attachmentZone.mReferenceName   = aDataPtr->GetReferencePlatformName();

      if (attachmentZone.mAttachedToOwner)
      {
         auto iter = mAttachmentZoneMap.find(aAnchorPtr->GetUniqueId());
         if (iter != mAttachmentZoneMap.end())
         {
            iter->second.push_back(attachmentZone);
         }
         else
         {
            std::vector<AttachmentZoneStruct> vec = {attachmentZone};
            mAttachmentZoneMap.emplace(aAnchorPtr->GetUniqueId(), vec);
         }
      }

      addedZone.mZones.push_back(attachmentZone);
      if (!attachmentZone.mReferenceName.empty())
      {
         addedZone.mReferenceNames.emplace(attachmentZone.mReferenceName);
      }
   };

   auto setFillerDataLambda = [&, aPlatform, aName, aLineColor, aFillColor](ZoneData* aDataPtr)
   {
      AttachmentZoneStruct attachmentZone;
      attachmentZone.mAttachedToOwner = !aDataPtr->GetLocationDefined();
      attachmentZone.mMinAltitude     = aDataPtr->GetMinAlt();
      attachmentZone.mZonePtr         = nullptr;
      attachmentZone.mReferenceName   = aDataPtr->GetReferencePlatformName();
      addedZone.mZones.push_back(attachmentZone);
   };

   auto it = std::find_if(mDrawZones.begin(),
                          mDrawZones.end(),
                          [&](const ZoneIdentifier& aZone) { return ZoneMatch(aPlatform, aName, aZone); });
   if (it != mDrawZones.end())
   {
      for (const auto& zone : it->mZones)
      {
         if (zone.mZonePtr)
         {
            zone.mZonePtr->SetStateVisibility(true);
            zone.mZonePtr->Load();
         }
         else if (!zone.mReferenceName.empty())
         {
            mDelayedZones[zone.mReferenceName].emplace(aPlatform, aName);
         }
      }
   }
   else
   {
      vespa::VaScenario* currentScenario = dynamic_cast<vespa::VaScenario*>(vaEnv.GetStandardScenario());
      if (currentScenario)
      {
         std::map<std::string, vespa::VaEntity*> anchorMap;
         ZoneSetData*                            zoneDataPtr = nullptr;
         GetAnchorEntityData(aPlatform, aName, zoneDataPtr, anchorMap, aPreview);
         if (zoneDataPtr)
         {
            for (const auto& zonePtr : zoneDataPtr->GetZones())
            {
               auto zoneIter = anchorMap.find(zonePtr->GetZoneName());
               if (zoneIter != anchorMap.end())
               {
                  auto& anchorPtr = zoneIter->second;
                  if (zonePtr->GetZoneType() == ZoneType::cCIRCULAR)
                  {
                     CircularZoneData* circularDataPtr = dynamic_cast<CircularZoneData*>(zonePtr.get());
                     if (circularDataPtr)
                     {
                        auto circleZonePtr =
                           vespa::make_attachment<wkf::AttachmentZoneCircle>(*anchorPtr, vaEnv.GetStandardViewer());
                        setCommonDataLambda(circleZonePtr, circularDataPtr, anchorPtr);
                        SetCircularZoneAttachmentData(circleZonePtr, circularDataPtr);
                     }
                  }
                  else if (zonePtr->GetZoneType() == ZoneType::cPOLYGONAL)
                  {
                     PolygonalZoneData* polygonalDataPtr = dynamic_cast<PolygonalZoneData*>(zonePtr.get());
                     if (polygonalDataPtr)
                     {
                        auto polygonZonePtr =
                           vespa::make_attachment<wkf::AttachmentZonePolygon>(*anchorPtr, vaEnv.GetStandardViewer());
                        setCommonDataLambda(polygonZonePtr, polygonalDataPtr, anchorPtr);
                        SetPolygonalZoneAttachmentData(polygonZonePtr,
                                                       polygonalDataPtr,
                                                       anchorPtr->GetPosition().GetAlt(),
                                                       aPreview);
                     }
                  }
                  else if (zonePtr->GetZoneType() == ZoneType::cELLIPTICAL)
                  {
                     EllipticalZoneData* ellipticalDataPtr = dynamic_cast<EllipticalZoneData*>(zonePtr.get());
                     if (ellipticalDataPtr)
                     {
                        auto ellipseZonePtr =
                           vespa::make_attachment<wkf::AttachmentZoneEllipse>(*anchorPtr, vaEnv.GetStandardViewer());
                        setCommonDataLambda(ellipseZonePtr, ellipticalDataPtr, anchorPtr);
                        SetEllipticalZoneAttachmentData(ellipseZonePtr, ellipticalDataPtr);
                     }
                  }
                  else if (zonePtr->GetZoneType() == ZoneType::cSPHERICAL)
                  {
                     SphericalZoneData* sphericalDataPtr = dynamic_cast<SphericalZoneData*>(zonePtr.get());
                     if (sphericalDataPtr)
                     {
                        auto sphericalZonePtr =
                           vespa::make_attachment<wkf::AttachmentZoneSphere>(*anchorPtr, vaEnv.GetStandardViewer());
                        setCommonDataLambda(sphericalZonePtr, sphericalDataPtr, anchorPtr);
                        SetSphericalZoneAttachmentData(sphericalZonePtr, sphericalDataPtr);
                     }
                  }
               }
               // Placeholder zones are necessary for zones with references that do not exist, but can exist in the
               // future (so they can be automatically drawn)
               else if (!zonePtr->GetLocationDefined() && !zonePtr->GetReferencePlatformName().empty())
               {
                  setFillerDataLambda(zonePtr.get());
                  mDelayedZones[zonePtr->GetReferencePlatformName()].emplace(aPlatform, aName);
               }
            }
            if (!addedZone.mZones.empty())
            {
               addedZone.mPlatformName = aPlatform;
               addedZone.mZoneName     = aName;
               mDrawZones.emplace_back(addedZone);
            }
         }
      }
   }
}

void wkf::ZoneBrowserDockWidget::HideZone(const std::string& aPlatform, const std::string& aName)
{
   auto it = std::find_if(mDrawZones.begin(),
                          mDrawZones.end(),
                          [&](const ZoneIdentifier& aZone) { return ZoneMatch(aPlatform, aName, aZone); });
   if (it != mDrawZones.end())
   {
      for (auto& zone : (*it).mZones)
      {
         if (zone.mZonePtr)
         {
            zone.mZonePtr->SetStateVisibility(false);
            zone.mZonePtr->Load();
         }
         // Remove any delayed reference zones as well
         if (!zone.mReferenceName.empty())
         {
            if (mDelayedZones.count(zone.mReferenceName) > 0)
            {
               auto& delayedZones = mDelayedZones.at(zone.mReferenceName);
               for (auto& delayedIt : delayedZones)
               {
                  if (aPlatform == delayedIt.first && aName == delayedIt.second)
                  {
                     delayedZones.erase(delayedIt);
                     break;
                  }
               }
            }
         }
      }
   }
}

void wkf::ZoneBrowserDockWidget::PlatformAdded(const std::string& aPlatform)
{
   // Check if the reference has been added as something to be drawn, remove placeholder status, then draw the zone
   if (mDelayedZones.count(aPlatform) > 0)
   {
      for (const auto& delayIt : mDelayedZones.at(aPlatform))
      {
         auto zoneIt =
            std::find_if(mDrawZones.begin(),
                         mDrawZones.end(),
                         [&](const ZoneIdentifier& aZone) { return ZoneMatch(delayIt.first, delayIt.second, aZone); });
         if (zoneIt != mDrawZones.end())
         {
            // Get the stored zone data from the data container. Sift through the individual zones in the set and draw
            // them accordingly
            ZoneSetData* zoneSetData = (zoneIt->mPlatformName == "Global Zones") ?
                                          mDataContainer.GetGlobalZone(zoneIt->mZoneName) :
                                          mDataContainer.GetLocalZone(zoneIt->mPlatformName, zoneIt->mZoneName);
            if (zoneSetData)
            {
               const auto&           zoneList = zoneSetData->GetZones();
               std::set<std::string> alreadyUsed; // the list of zones involves unique pointers so just use the set to
                                                  // track which ones are accounted for
               // Address the zones in the drawn zoneset that actually have a delayed reference. The number of zones
               // that are referenced should be the same as the number of zones in the data container that reference the
               // same platform. The order drawn doesn't matter.
               for (auto& attIt : zoneIt->mZones)
               {
                  if (attIt.mReferenceName == aPlatform && attIt.mZonePtr == nullptr)
                  {
                     // Get the existing anchor and the relevant data from the data container
                     const auto&      dataIt = std::find_if(zoneList.begin(),
                                                       zoneList.end(),
                                                       [&](const std::unique_ptr<wkf::ZoneData>& aPtr)
                                                       {
                                                          std::string uniqueName = aPtr->GetZoneName();
                                                          ;
                                                          if (aPtr->GetReferencePlatformName() == aPlatform &&
                                                              alreadyUsed.count(uniqueName) == 0)
                                                          {
                                                             alreadyUsed.emplace(uniqueName);
                                                             return true;
                                                          }
                                                          return false;
                                                       });
                     vespa::VaEntity* anchorPtr;
                     GetReferenceAnchor(aPlatform, anchorPtr);
                     if (anchorPtr && dataIt != zoneList.end())
                     {
                        auto zoneWidgets  = mUI.mTreeWidget->findChildren<ZoneSelectionWidget*>();
                        auto zoneWidgetIt = std::find_if(zoneWidgets.begin(),
                                                         zoneWidgets.end(),
                                                         [&](ZoneSelectionWidget* aWidget) {
                                                            return aWidget->mOwningPlatformName == zoneIt->mPlatformName &&
                                                                   aWidget->GetName() == zoneIt->mZoneName;
                                                         });
                        // This iterator should always be valid otherwise there are other issues.
                        QColor lineColor = (*zoneWidgetIt)->mLineColorButtonPtr->GetColor();
                        QColor fillColor = (*zoneWidgetIt)->mFillColorButtonPtr->GetColor();
                        zoneSetData->GetLineColor();
                        auto zoneDataPtr = dataIt->get();
                        if ((*zoneWidgetIt)->mVisibilityBoxPtr->isChecked())
                        {
                           if (zoneDataPtr->GetZoneType() == ZoneType::cCIRCULAR)
                           {
                              CircularZoneData* circularDataPtr = dynamic_cast<CircularZoneData*>(zoneDataPtr);
                              if (circularDataPtr)
                              {
                                 auto circleZonePtr =
                                    vespa::make_attachment<wkf::AttachmentZoneCircle>(*anchorPtr,
                                                                                      vaEnv.GetStandardViewer());
                                 SetCommonZoneAttachmentData(circleZonePtr, zoneDataPtr, anchorPtr, lineColor, fillColor);
                                 SetCircularZoneAttachmentData(circleZonePtr, circularDataPtr);
                                 attIt.mZonePtr = circleZonePtr;
                              }
                           }
                           else if (zoneDataPtr->GetZoneType() == ZoneType::cELLIPTICAL)
                           {
                              EllipticalZoneData* ellipticalDataPtr = dynamic_cast<EllipticalZoneData*>(zoneDataPtr);
                              if (ellipticalDataPtr)
                              {
                                 auto ellipseZonePtr =
                                    vespa::make_attachment<wkf::AttachmentZoneEllipse>(*anchorPtr,
                                                                                       vaEnv.GetStandardViewer());
                                 SetCommonZoneAttachmentData(ellipseZonePtr, zoneDataPtr, anchorPtr, lineColor, fillColor);
                                 SetEllipticalZoneAttachmentData(ellipseZonePtr, ellipticalDataPtr);
                                 attIt.mZonePtr = ellipseZonePtr;
                              }
                           }
                           else if (zoneDataPtr->GetZoneType() == ZoneType::cPOLYGONAL)
                           {
                              PolygonalZoneData* polygonalDataPtr = dynamic_cast<PolygonalZoneData*>(zoneDataPtr);
                              if (polygonalDataPtr)
                              {
                                 auto polygonZonePtr =
                                    vespa::make_attachment<wkf::AttachmentZonePolygon>(*anchorPtr,
                                                                                       vaEnv.GetStandardViewer());
                                 SetCommonZoneAttachmentData(polygonZonePtr, zoneDataPtr, anchorPtr, lineColor, fillColor);
                                 SetPolygonalZoneAttachmentData(polygonZonePtr,
                                                                polygonalDataPtr,
                                                                anchorPtr->GetPosition().GetAlt());
                                 attIt.mZonePtr = polygonZonePtr;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

void wkf::ZoneBrowserDockWidget::AddZone(ZoneSetData* aDataPtr, bool aShow)
{
   // Find the platform this zone is attached to in the tree and call PopulateDropdown on it.
   // Global zones will have the platform name "Global Zones"
   std::string             platformName = aDataPtr->GetPlatformName();
   QList<QTreeWidgetItem*> platformCategory =
      mUI.mTreeWidget->findItems(QString::fromStdString(platformName), Qt::MatchExactly);
   aShow = mUI.mAllCheckBox->checkState() == Qt::Checked ? true : aShow;

   QTreeWidgetItem* itemPtr = nullptr;
   if (!platformCategory.isEmpty())
   {
      itemPtr = platformCategory[0];
   }
   else
   {
      itemPtr = ut::qt::make_qt_ptr<QTreeWidgetItem>(mUI.mTreeWidget->invisibleRootItem(),
                                                     QStringList(QString::fromStdString(platformName)));
      // On update, reinsert the platform to its previous position. Automatically place global zones at the top
      auto currentIndex = mUI.mTreeWidget->indexOfTopLevelItem(itemPtr);
      if (platformName == "Global Zones")
      {
         mPrevIndex = 0;
      }
      if (currentIndex != mPrevIndex && mPrevIndex < mUI.mTreeWidget->topLevelItemCount())
      {
         mUI.mTreeWidget->takeTopLevelItem(currentIndex);
         mUI.mTreeWidget->insertTopLevelItem(mPrevIndex, itemPtr);
      }
      mPrevIndex = mUI.mTreeWidget->topLevelItemCount();
   }
   AddZoneToBrowser(itemPtr, aDataPtr, aShow);
   UpdateItemVisibility(itemPtr, platformName);

   mUI.mAllCheckBox->setCheckState(CalculateAllBoxState());
}

void wkf::ZoneBrowserDockWidget::ZoneUpdatedCB(const std::string& aZoneName, const std::string& aPlatformName)
{
   std::string platformName{aPlatformName};
   if (platformName.empty())
   {
      platformName = cGLOBAL_NAME;
   }
   QList<QTreeWidgetItem*> platformItem =
      mUI.mTreeWidget->findItems(QString::fromStdString(platformName), Qt::MatchExactly);
   if (!platformItem.isEmpty())
   {
      for (int i = 0; i < platformItem[0]->childCount(); i++)
      {
         QWidget*             globalWidgetPtr = mUI.mTreeWidget->itemWidget(platformItem[0]->child(i), 0);
         QWidget*             widgetPtr       = globalWidgetPtr->layout()->itemAt(0)->widget();
         ZoneSelectionWidget* zoneWidgetPtr   = dynamic_cast<ZoneSelectionWidget*>(widgetPtr);
         if (zoneWidgetPtr != nullptr)
         {
            std::string label = zoneWidgetPtr->GetName();
            if (label == aZoneName)
            {
               ZoneSetData* containerData = (aPlatformName == "Global Zones") ?
                                               mDataContainer.GetGlobalZone(aZoneName) :
                                               mDataContainer.GetLocalZone(aPlatformName, aZoneName);

               QColor fillColor(GetFillColor(mRandom.GetSeed() == cRANDOM_SEED));
               QColor lineColor(GetLineColor(mRandom.GetSeed() == cRANDOM_SEED));

               if (containerData)
               {
                  auto definedFill = containerData->GetFillColor();
                  if (definedFill.has_value())
                  {
                     fillColor = UtQtColor(definedFill.value());
                  }
                  auto definedLine = containerData->GetLineColor();
                  if (definedLine.has_value())
                  {
                     lineColor = UtQtColor(definedLine.value());
                  }
               }
               if (!mButtonClicked)
               {
                  zoneWidgetPtr->mLineColorButtonPtr->SetColor(lineColor);
                  zoneWidgetPtr->mFillColorButtonPtr->SetColor(fillColor);
               }

               ZoneBrowserDockWidget::SetZoneLineColor(aPlatformName, aZoneName, lineColor);
               ZoneBrowserDockWidget::SetZoneFillColor(aPlatformName, aZoneName, fillColor);
               break;
            }
         }
      }
      if (platformItem[0]->isExpanded())
      {
         platformItem[0]->setExpanded(false);
         platformItem[0]->setExpanded(true);
      }
   }
}

void wkf::ZoneBrowserDockWidget::PreviewUpdatedCB()
{
   ZoneSetData* previewPtr = mDataContainer.GetPreviewZone();
   QColor       fillColor{Qt::white};
   QColor       lineColor{Qt::white};
   if (previewPtr->GetFillColor().has_value())
   {
      fillColor = UtQtColor(previewPtr->GetFillColor().value());
   }
   if (previewPtr->GetLineColor().has_value())
   {
      lineColor = UtQtColor(previewPtr->GetLineColor().value());
   }
   DrawZone(previewPtr->GetPlatformName(), previewPtr->GetSetName(), lineColor, fillColor, true);
}

void wkf::ZoneBrowserDockWidget::PreviewPendingDeletionCB()
{
   DeleteZone(mDataContainer.GetPreviewZone());
}

QColor wkf::ZoneBrowserDockWidget::GetFillColor(bool aRandom)
{
   QColor color(mDefaultFillColor);
   if (aRandom)
   {
      color = QColor(mRandom.Uniform(0, 255), mRandom.Uniform(0, 255), mRandom.Uniform(0, 255), 63);
   }
   return color;
}

QColor wkf::ZoneBrowserDockWidget::GetLineColor(bool aRandom)
{
   QColor color(mDefaultLineColor);
   if (aRandom)
   {
      color = QColor(mRandom.Uniform(0, 255), mRandom.Uniform(0, 255), mRandom.Uniform(0, 255), 255);
   }
   return color;
}

void wkf::ZoneBrowserDockWidget::DeleteZone(ZoneSetData* aDataPtr)
{
   auto it = std::find_if(mDrawZones.begin(),
                          mDrawZones.end(),
                          [&](const ZoneIdentifier& aZone)
                          { return ZoneMatch(aDataPtr->GetPlatformName(), aDataPtr->GetSetName(), aZone); });


   // If the zone is currently being drawn, delete the attachment and remove it from mDrawZones.
   if (it != mDrawZones.end())
   {
      if (it->mPlatformName.empty() || it->mPlatformName == cGLOBAL_NAME)
      {
         // If there is a reference platform, then this zone is attached to a platform and only the attachment should be deleted.
         for (auto& zone : it->mZones)
         {
            if (!zone.mReferenceName.empty() && zone.mAttachedToOwner)
            {
               if (vaEnv.GetStandardScenario() != nullptr)
               {
                  if (vaEnv.GetStandardScenario()->FindPlatformByName(zone.mReferenceName) != nullptr)
                  {
                     if (zone.mZonePtr)
                     {
                        zone.mZonePtr->GetParent().RemoveAttachment(zone.mZonePtr->GetUniqueId());
                     }
                  }
               }
            }
            else
            {
               auto entity = std::find(mEntityTracker.begin(), mEntityTracker.end(), &zone.mZonePtr->GetParent());
               if (entity != mEntityTracker.end())
               {
                  mEntityTracker.erase(entity);
               }
               vaEnv.DeleteEntity(&zone.mZonePtr->GetParent());
            }
         }
      }
      else
      {
         // This isn't a global zone, so it must be attached to a platform. Don't delete the platform -- only the attachment.
         if (vaEnv.GetStandardScenario() != nullptr)
         {
            if (vaEnv.GetStandardScenario()->FindPlatformByName(it->mPlatformName) != nullptr)
            {
               for (auto zone : it->mZones)
               {
                  if (zone.mZonePtr)
                  {
                     zone.mZonePtr->GetParent().RemoveAttachment(zone.mZonePtr->GetUniqueId());
                  }
               }
            }
         }
      }
      mDrawZones.erase(it);
   }

   // Find the platform this zone is attached to in the tree and remove the zone entry.
   QList<QTreeWidgetItem*> platformCategory =
      mUI.mTreeWidget->findItems(QString::fromStdString(aDataPtr->GetPlatformName()), Qt::MatchExactly);
   if (!platformCategory.isEmpty())
   {
      mPrevIndex = mUI.mTreeWidget->indexOfTopLevelItem(platformCategory[0]);
      for (int i = 0; i < platformCategory[0]->childCount(); i++)
      {
         QWidget* platformWidgetPtr = mUI.mTreeWidget->itemWidget(platformCategory[0]->child(i), 0);
         auto     zoneWidgetPtr = dynamic_cast<ZoneSelectionWidget*>(platformWidgetPtr->layout()->itemAt(0)->widget());
         if (zoneWidgetPtr != nullptr)
         {
            std::string label = zoneWidgetPtr->GetName();
            if (label == aDataPtr->GetSetName())
            {
               // Store various values for use when updating a zone.
               mPrevVisible   = zoneWidgetPtr->mVisibilityBoxPtr->isChecked();
               auto fillColor = aDataPtr->GetFillColor();
               if (fillColor.has_value())
               {
                  mPrevFillColor = UtQtColor(fillColor.value());
               }
               else
               {
                  mPrevFillColor = zoneWidgetPtr->mFillColorButtonPtr->GetColor();
               }

               auto lineColor = aDataPtr->GetLineColor();
               if (lineColor.has_value())
               {
                  mPrevLineColor = UtQtColor(lineColor.value());
               }
               else
               {
                  mPrevLineColor = zoneWidgetPtr->mLineColorButtonPtr->GetColor();
               }
               mPrevChildIndex = i;
               mPrevExpanded   = platformCategory[0]->isExpanded();
               platformCategory[0]->removeChild(platformCategory[0]->child(i));

               // After removing the child, if the platform entry is empty, remove the entry.
               if (platformCategory[0]->childCount() == 0)
               {
                  mUI.mTreeWidget->invisibleRootItem()->removeChild(platformCategory[0]);
               }
               break;
            }
         }
      }
   }
}

void wkf::ZoneBrowserDockWidget::CheckAllZoneBoxes(Qt::CheckState aCheckState)
{
   if (aCheckState == Qt::Unchecked || aCheckState == Qt::Checked)
   {
      QList<ZoneSelectionWidget*> menuOptionsPtr = mUI.mTreeWidget->findChildren<ZoneSelectionWidget*>();
      for (auto it : menuOptionsPtr)
      {
         if (it->isEnabled())
         {
            it->mVisibilityBoxPtr->setCheckState(aCheckState);
         }
      }
   }
}

void wkf::ZoneBrowserDockWidget::SetZoneLineColor(const std::string& aPlatform, const std::string& aName, const QColor& aColor)
{
   auto it = std::find_if(mDrawZones.begin(),
                          mDrawZones.end(),
                          [&](const ZoneIdentifier& aZone) { return ZoneMatch(aPlatform, aName, aZone); });
   if (it != mDrawZones.end())
   {
      auto color(UtQtColor(aColor));
      for (auto& zone : it->mZones)
      {
         if (zone.mZonePtr)
         {
            zone.mZonePtr->SetColor(color);
            zone.mZonePtr->Load();
         }
      }
   }

   std::pair<std::string, std::string> index{aPlatform, aName};
   mCustomLineColors[index] = aColor;
}

void wkf::ZoneBrowserDockWidget::SetZoneFillColor(const std::string& aPlatform, const std::string& aName, const QColor& aColor)
{
   auto it = std::find_if(mDrawZones.begin(),
                          mDrawZones.end(),
                          [&](const ZoneIdentifier& aZone) { return ZoneMatch(aPlatform, aName, aZone); });
   if (it != mDrawZones.end())
   {
      auto color(UtQtColor(aColor));
      for (auto& zone : it->mZones)
      {
         if (zone.mZonePtr)
         {
            zone.mZonePtr->SetFillColor(color);
            zone.mZonePtr->SetBlendFactor(1 - aColor.alphaF());
            zone.mZonePtr->Load();
         }
      }
   }

   std::pair<std::string, std::string> index{aPlatform, aName};
   mCustomFillColors[index] = aColor;
}

bool wkf::ZoneBrowserDockWidget::GetZoneLineColor(const std::string& aPlatform, const std::string& aName, QColor& aColor)
{
   auto color = mCustomLineColors.find(std::pair<std::string, std::string>(aPlatform, aName));
   if (color != mCustomLineColors.end())
   {
      aColor = color->second;
      return true;
   }
   return false;
}

bool wkf::ZoneBrowserDockWidget::GetZoneFillColor(const std::string& aPlatform, const std::string& aName, QColor& aColor)
{
   auto color = mCustomFillColors.find(std::pair<std::string, std::string>(aPlatform, aName));
   if (color != mCustomFillColors.end())
   {
      aColor = color->second;
      return true;
   }
   return false;
}

void wkf::ZoneBrowserDockWidget::VisibilityChangedHandler()
{
   auto scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      for (int i = 0; i < mUI.mTreeWidget->topLevelItemCount(); ++i)
      {
         QTreeWidgetItem* itemPtr = mUI.mTreeWidget->topLevelItem(i);
         UpdateItemVisibility(itemPtr, itemPtr->text(0).toStdString());
      }

      // in the browser, disable any zones that reference or are owned by a platform that isn't visible
      // uncheck the associated checkbox to trigger the slot that hides already drawn zones
      QList<ZoneSelectionWidget*> menuOptions = mUI.mTreeWidget->findChildren<ZoneSelectionWidget*>();
      for (const auto& it : menuOptions)
      {
         UpdateZoneWidgetState(it->mReferencePlatformName, it);
         UpdateZoneWidgetState(it->mOwningPlatformName, it);
      }

      mUI.mAllCheckBox->setCheckState(CalculateAllBoxState());
   }
}

Qt::CheckState wkf::ZoneBrowserDockWidget::CalculateAllBoxState() const
{
   // Count which widgets are checked, only count enabled widgets
   QList<ZoneSelectionWidget*> widgetList    = mUI.mTreeWidget->findChildren<ZoneSelectionWidget*>();
   unsigned int                numberChecked = 0;
   unsigned int                totalCount    = 0;
   auto                        it            = widgetList.begin();
   while (it != widgetList.end())
   {
      if (!(*it)->isHidden())
      {
         totalCount++;
      }

      if ((*it)->isEnabled() && (*it)->mVisibilityBoxPtr->isChecked())
      {
         numberChecked++;
      }
      ++it;
   }

   if (numberChecked == 0)
   {
      return Qt::Unchecked;
   }
   else if (numberChecked == totalCount)
   {
      return Qt::Checked;
   }
   else
   {
      return Qt::PartiallyChecked;
   }
}

void wkf::ZoneBrowserDockWidget::GetAnchorEntityData(const std::string&                       aPlatform,
                                                     const std::string&                       aName,
                                                     ZoneSetData*&                            aDataPtr,
                                                     std::map<std::string, vespa::VaEntity*>& aAnchors,
                                                     bool                                     aPreview)
{
   auto getAnchorLambda = [&](ZoneData* aZonePtr, vespa::VaEntity*& aAnchorPtr, vespa::VaEntity*& aParentPtr)
   {
      const std::string& zoneName = aZonePtr->GetZoneName();
      auto               it       = std::find_if(mEntityTracker.begin(),
                             mEntityTracker.end(),
                             [zoneName](const vespa::VaEntity* aEntityPtr) { return aEntityPtr->GetName() == zoneName; });
      if (it != mEntityTracker.end())
      {
         aAnchorPtr = *it;
      }
      else
      {
         auto* scenarioPtr = vaEnv.GetStandardScenario();
         if (scenarioPtr)
         {
            aAnchorPtr = new vespa::VaEntity(zoneName);
            if (scenarioPtr->AddEntity(aAnchorPtr))
            {
               aAnchorPtr->Initialize();
               vespa::VaPosition position;
               position.SetLat(aZonePtr->GetReferenceLat());
               position.SetLon(aZonePtr->GetReferenceLon());
               aAnchorPtr->SetPosition(position);
               aAnchorPtr->SetHeading(aZonePtr->GetHeading());
               mEntityTracker.emplace_back(aAnchorPtr);
            }
         }
      }
   };

   ZoneSetData* zoneSetPtr{nullptr};
   if (aPlatform == cGLOBAL_NAME)
   {
      zoneSetPtr = (aPreview ? mDataContainer.GetPreviewZone() : mDataContainer.GetGlobalZone(aName));

      if (zoneSetPtr != nullptr)
      {
         if (vaEnv.GetStandardScenario() != nullptr)
         {
            vespa::VaEntity* anchorPtr{nullptr};
            for (const auto& zonePtr : zoneSetPtr->GetZones())
            {
               if (zonePtr->GetLocationDefined() || zonePtr->GetReferencePlatformName().empty() ||
                   zonePtr->GetReferencePlatformName() == cGLOBAL_NAME)
               {
                  getAnchorLambda(zonePtr.get(), anchorPtr, anchorPtr);
               }
               else
               {
                  GetReferenceAnchor(zonePtr->GetReferencePlatformName(), anchorPtr);
               }
               if (anchorPtr)
               {
                  aAnchors.emplace(zonePtr->GetZoneName(), anchorPtr);
               }
            }
         }
      }
   }
   else
   {
      zoneSetPtr = (aPreview ? mDataContainer.GetPreviewZone() : mDataContainer.GetLocalZone(aPlatform, aName));

      if (zoneSetPtr != nullptr)
      {
         vespa::VaEntity* platformAnchorPtr{nullptr};
         auto             it = mAnchorZoneMap.find(aPlatform);
         if (it != mAnchorZoneMap.end())
         {
            platformAnchorPtr = it->second;
         }
         else
         {
            auto* scenarioPtr = vaEnv.GetStandardScenario();
            if (scenarioPtr)
            {
               platformAnchorPtr = scenarioPtr->FindPlatformByName(aPlatform);
               if (platformAnchorPtr)
               {
                  int cbId = mCallbacks.Add(wkf::Observer::EntityMoved.Connect(&ZoneBrowserDockWidget::EntityMovedCB,
                                                                               this,
                                                                               platformAnchorPtr->GetUniqueId()));
                  mCallbackMap[platformAnchorPtr->GetUniqueId()].push_back(cbId);
                  mAnchorZoneMap.emplace(aPlatform, platformAnchorPtr);
               }
            }
         }

         for (const auto& zonePtr : zoneSetPtr->GetZones())
         {
            std::string      zoneName = zonePtr->GetZoneName();
            vespa::VaEntity* anchorPtr{nullptr};
            if (zonePtr->GetLocationDefined())
            {
               getAnchorLambda(zonePtr.get(), anchorPtr, platformAnchorPtr);
            }
            else if (!zonePtr->GetReferencePlatformName().empty())
            {
               GetReferenceAnchor(zonePtr->GetReferencePlatformName(), anchorPtr);
            }
            else if (platformAnchorPtr)
            {
               anchorPtr = platformAnchorPtr;
            }

            if (anchorPtr)
            {
               aAnchors.emplace(zoneName, anchorPtr);
            }
         }
      }
   }
   aDataPtr = zoneSetPtr;
}

vespa::VaEntity* wkf::ZoneBrowserDockWidget::GetActiveAnchor(const std::string& aReferenceName)
{
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      return scenarioPtr->FindPlatformByName(aReferenceName);
   }
   return nullptr;
}

void wkf::ZoneBrowserDockWidget::GetReferenceAnchor(const std::string& aReferenceName, vespa::VaEntity*& aAnchorPtr)
{
   auto it = mAnchorZoneMap.find(aReferenceName);
   if (it != mAnchorZoneMap.end())
   {
      aAnchorPtr = it->second;
   }
   else
   {
      aAnchorPtr = GetActiveAnchor(aReferenceName);
      if (aAnchorPtr)
      {
         mCallbacks.Add(
            wkf::Observer::EntityMoved.Connect(&ZoneBrowserDockWidget::EntityMovedCB, this, aAnchorPtr->GetUniqueId()));
         mAnchorZoneMap.emplace(aReferenceName, aAnchorPtr);
      }
   }
}

void wkf::ZoneBrowserDockWidget::AttachmentDeletedHandler(vespa::VaAttachment*            aAttachmentPtr,
                                                          const vespa::VaAttachmentSubId& aId)
{
   // Handle the deletion of a platform that owns zones referencing other platforms
   auto attIt = mAttachmentZoneMap.find(aAttachmentPtr->GetParent().GetUniqueId());
   if (attIt != mAttachmentZoneMap.end())
   {
      auto& attList = attIt->second;
      attList.erase(std::remove_if(attList.begin(),
                                   attList.end(),
                                   [aAttachmentPtr](const wkf::AttachmentZoneStruct& aAtt)
                                   { return aAtt.mZonePtr->GetUniqueId() == aAttachmentPtr->GetUniqueId(); }),
                    attList.end());
   }

   // In the draw tracker, maintain the deleted attachment as a placeholder by setting it to nullptr
   for (auto& drawIt : mDrawZones)
   {
      for (auto& zoneIt : drawIt.mZones)
      {
         if (zoneIt.mZonePtr && zoneIt.mZonePtr->GetUniqueId() == aAttachmentPtr->GetUniqueId())
         {
            zoneIt.mZonePtr = nullptr;
         }
      }
   }
}

void wkf::ZoneBrowserDockWidget::EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving)
{
   if (aMoving)
   {
      auto it = mAttachmentZoneMap.find(aEntityPtr->GetUniqueId());
      if (it != mAttachmentZoneMap.end())
      {
         for (auto& zone : it->second)
         {
            if (zone.mZonePtr)
            {
               wkf::AttachmentZonePolygon* polygonPtr = dynamic_cast<wkf::AttachmentZonePolygon*>(zone.mZonePtr);
               if (polygonPtr && polygonPtr->IsRelative())
               {
                  zone.mZonePtr->SetBaseHeight(aEntityPtr->GetPosition().GetAlt() - zone.mMinAltitude);
                  // a non-relative polygonal zone will always have a base height of mMinAltitude (no change)
               }
               else if (polygonPtr == nullptr)
               {
                  zone.mZonePtr->SetBaseHeight(zone.mMinAltitude - aEntityPtr->GetPosition().GetAlt());
               }
            }
         }
      }
   }
}
