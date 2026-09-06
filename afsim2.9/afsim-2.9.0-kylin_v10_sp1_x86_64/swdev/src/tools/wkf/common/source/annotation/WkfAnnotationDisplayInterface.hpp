// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ANNOTATIONDISPLAYINTERFACE_HPP
#define ANNOTATIONDISPLAYINTERFACE_HPP

#include "wkf_common_export.h"

#include <unordered_set>

#include <QDialog>
#include <QDialogButtonBox>
#include <QObject>
#include <QSettings>

#include "VaCallbackHolder.hpp"
#include "VaScenario.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlatformGrouping.hpp"
#include "WkfPointOfInterestPropertiesWidget.hpp"
#include "WkfRingPropertiesWidget.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"
#include "selectors/WkfImageSelectorWidget.hpp"

namespace wkf
{
class Action;
class Bullseye;
class Environment;
class PointOfInterest;
} // namespace wkf

namespace Annotation
{
class BullseyeWidget;

class ImageDialog : public QDialog
{
   Q_OBJECT
public:
   explicit ImageDialog(QWidget* parent = nullptr);
   wkf::ImageSelectionWidget* mImageSelectionWidget;
   QDialogButtonBox*          mDialogButtonBox;

protected:
   void accept() override;

signals:
   void ImageSelected(const QString& aImagePath);
};

class WKF_COMMON_EXPORT AnnotationDisplayInterface : public QObject
{
   Q_OBJECT
public:
   explicit AnnotationDisplayInterface(QObject* aParent);
   ~AnnotationDisplayInterface() override = default;

   struct CustomVectorData
   {
      CustomVectorData() = default;
      CustomVectorData(QString aName, const wkf::PlatformGrouping& aGrouping, QColor aColor)
         : mName(std::move(aName))
         , mGrouping(aGrouping)
         , mColor(std::move(aColor))
      {
      }

      QString               mName;
      wkf::PlatformGrouping mGrouping;
      QColor                mColor;
   };
   using CustomVectorMap = std::map<int, std::map<int, CustomVectorData>>;

   template<class T>
   T* AddPOI(const std::string& aTypeStr, double aLat, double aLon, const std::string& aName = "");

   template<class T>
   T* AddDetailedPOI(const std::string& aTypeStr,
                     double             aLat,
                     double             aLon,
                     double             aAlt        = 0,
                     const std::string& aName       = "",
                     bool               aExportable = true);

   QString DecorationPathToPreset(const QString& aPath);
   QString DecorationPresetToPath(const std::string& aPreset);

   void AddRangeRing(wkf::Platform& aPlatform, const wkf::AttachmentRangeRing::Properties& aProperties);
   void AddRangeRing(const double& aLat, const double& aLon, const wkf::AttachmentRangeRing::Properties& aProperties);
   void AddRangeRing(wkf::PointOfInterest* aPoi, const wkf::AttachmentRangeRing::Properties& aProperties);
   void AddRangeRing(const double& aLat, const double& aLon);
   void AddRangeRing(wkf::Entity* aEntityPtr, bool aUI_Request);

   void         EditRangeRing(wkf::Entity* aEntityPtr, const Annotation::RingPropertiesWidget::RingFlags& aFlags);
   void         ShowPOIProperties(wkf::Entity* aEntityPtr, bool aCreate);
   wkf::Action* GetBullseyeAction() const { return mBullseyeActionPtr; }

   void CustomVector(wkf::Entity* aEntityPtr, bool aUI_Request);
   void Decorate(const QString& aEntityName, const QIcon& aType, const QString& aDescriptor);
   bool DecorateWithText(const QString& aEntityName);
   void DecorateWithText(wkf::Entity& aEntity, const QString& aText);
   bool DecorateWithImage(const QString& aEntityName);

   CustomVectorMap GetVectors() const { return mCustomVectors; }

   void ShowVector(wkf::Entity& aEntity, int aTypeIndex, bool aState);
   void AddCustomVector(wkf::Entity& aEntity, const wkf::PlatformGrouping& aGrouping, const QColor& aColor, bool aState);
   void RemoveCustomVector(int aIndex, wkf::Entity* aEntityPtr);
   void AddDecoration(wkf::Entity& aEntity, const QString& aText, const QString& aImageDesc);

   QIcon GetDecorationIcon(const QString& aPath) const;

   void AddDeferredRangeRing(const QString& aPlatformName, const wkf::AttachmentRangeRing::Properties& aData);
   void AddDeferredDecoration(const QString& aPlatformName, const std::pair<QString, QString>& aDesc);

   bool PoiEditOpen() const { return mPOI_PropertiesPtr->hasFocus(); }

   PointOfInterestPropertiesWidget* GetPoiPropertiesWidget() const { return mPOI_PropertiesPtr; }
   RingPropertiesWidget*            GetRingPropertiesWidget() const { return mRingPropertiesPtr; }

signals:
   void SetRangeRingOptionState(bool aState, wkf::Platform* aPlatformPtr);
   void SetDecorationOptionState(bool aState, wkf::Platform* aPlatformPtr);
   void TestChanged();

private:
   void EntityDecoratedCB(wkf::Entity* aPlatform, const QIcon& aIcon, const std::string& aDescription);
   void EntitySelectedCB(vespa::VaViewer* aViewerPtr, int aSelected, vespa::VaEntity* aEntityPtr);
   void PointOfInterestMovedCB(vespa::VaEntity* aPointOfInterest, bool aUIDriven);
   void PointOfInterestDeletedCB(vespa::VaEntity* aPointOfInterest);
   void PlatformAddedCB(const wkf::PlatformList& aPlatform);
   void StandardScenarioAddedCB(wkf::Scenario* aScenarioPtr);
   void StandardScenarioRemovedCB(wkf::Scenario* aScenarioPtr);
   void PlatformDeletedCB(vespa::VaEntity* aPlatformPtr);
   void AttachmentDeletedCB(vespa::VaAttachment* aAttachmentPtr, const vespa::VaAttachmentSubId& aSubpartId);
   void ToggleBullseye(unsigned int aId, bool aBullseye);
   void ClearAnnotations();
   void ExportAnnotations();
   void MoveSelectionAndAttachmentComplete(const wkf::PlatformList&       aPlatformList,
                                           const wkf::EntityList&         aNonPlatformList,
                                           const vespa::VaAttachmentList& aAttachmentList);
   vespa::VaCallbackHolder mCallbacks;

   vespa::VaEntity*                                                     mPOI_Ptr;
   PointOfInterestPropertiesWidget*                                     mPOI_PropertiesPtr;
   RingPropertiesWidget*                                                mRingPropertiesPtr;
   QString                                                              mActiveAnnotationFile;
   std::map<QString, std::vector<wkf::AttachmentRangeRing::Properties>> mDeferredRangeRingMap;
   std::map<QString, std::pair<QString, QString>>                       mDeferredDecorationMap;
   std::map<QString, std::vector<CustomVectorData>>                     mDeferredCustomVectorMap;

   // Annotations that are added to platforms that are subsequently deleted should still be exported
   std::map<QString, std::vector<wkf::AttachmentRangeRing::Properties>> mDeletedPlatformRangeRingMap;
   std::map<QString, std::pair<QString, QString>>                       mDeletedPlatformDecorationMap;
   std::map<QString, std::vector<CustomVectorData>>                     mDeletedCustomVectorMap;
   int                                                                  mEntityAddedUID;
   std::unique_ptr<QSettings>                                           mTempSettings;
   // maps entity to vector to data
   CustomVectorMap mCustomVectors;

   wkf::Action*    mBullseyeActionPtr;
   BullseyeWidget* mBullseyeWidgetPtr;
};

template<class T>
T* Annotation::AnnotationDisplayInterface::AddPOI(const std::string& aTypeStr,
                                                  double             aLat,
                                                  double             aLon,
                                                  const std::string& aName /*= ""*/)
{
   static_assert(std::is_base_of<wkf::PointOfInterest, T>::value,
                 "Template argument to AddPOI must be a subclass of PointOfInterest");

   wkf::Viewer*   viewer   = vaEnv.GetStandardViewer();
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();

   bool nameTaken = false;
   if (!aName.empty())
   {
      vespa::VaEntityList list;
      vaEnv.GetEntityListOfType<wkf::PointOfInterest>(list);
      for (auto&& it : list)
      {
         if (aName == it->GetName())
         {
            nameTaken = true;
            break;
         }
      }
   }
   if (viewer && scenario && !nameTaken)
   {
      T* entity = vaEnv.CreateEntity<T>(aTypeStr);
      entity->SetPosition(vespa::VaPosition(aLat, aLon, 0.0));
      scenario->AddEntity(entity);
      entity->Initialize();
      entity->Load();
      mCallbacks.Add(wkf::Observer::EntityMoved.Connect(&AnnotationDisplayInterface::PointOfInterestMovedCB,
                                                        this,
                                                        entity->GetUniqueId()));
      mCallbacks.Add(wkf::Observer::EntityDeleted.Connect(&AnnotationDisplayInterface::PointOfInterestDeletedCB,
                                                          this,
                                                          entity->GetUniqueId()));

      std::string name = (aName.empty()) ? ("point_of_interest_" + entity->GetUniqueIdString()) : aName;
      entity->SetName(name);
      return entity;
   }
   return nullptr;
}

// Used to add more information to POIs. Right now, this only consists of an altitude and exportability (previously used
// to also do decorations before they were removed)
template<class T>
T* Annotation::AnnotationDisplayInterface::AddDetailedPOI(const std::string& aTypeStr,
                                                          double             aLat,
                                                          double             aLon,
                                                          double             aAlt,
                                                          const std::string& aName,
                                                          bool               aExportable)
{
   static_assert(std::is_base_of<wkf::PointOfInterest, T>::value,
                 "Template argument to AddPOI must be a subclass of PointOfInterest");

   T* entity = AddPOI<T>(aTypeStr, aLat, aLon, aName);
   if (entity != nullptr)
   {
      entity->SetExportable(aExportable);
      entity->SetPosition(vespa::VaPosition(aLat, aLon, aAlt));
   }
   return entity;
}
} // namespace Annotation
#endif

// TODO: Custom Vectors? Not included as part of the spec document
