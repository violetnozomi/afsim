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
#include "WkfAnnotationPlugin.hpp"

#include <QApplication>
#include <QDialogButtonBox>
#include <QEvent>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QMenu>

#include "UtMemory.hpp"
#include "UtQtColorButton.hpp"
#include "UtQtMemory.hpp"
#include "VaAttachmentModel.hpp"
#include "VaAttachmentVectors.hpp"
#include "VaCallbackHolder.hpp"
#include "VaUtils.hpp"
#include "WkfAction.hpp"
#include "WkfAnnotationDecorator.hpp"
#include "WkfAttachmentRangeRing.hpp"
#include "WkfBullseye.hpp"
#include "WkfBullseyeWidget.hpp"
#include "WkfEntity.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPointOfInterest.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "selectors/WkfGroupingSelector.hpp"

Annotation::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId, const QFlags<Roles>& aRoles)
   : wkf::Plugin(aPluginName, aUniqueId, aRoles)
   , mDisplayInterface(new AnnotationDisplayInterface(this))
{
   mRangeRingOption         = RegisterOption(nullptr, "Range Rings", "", false, false, true)->type();
   mDecorationOption        = RegisterOption(nullptr, "Decoration", "", false, false, true)->type();
   QTreeWidgetItem* vectors = RegisterOptionGroup(nullptr, "Vectors");
   mBodyFrameOption         = RegisterOption(vectors, "Body Frame")->type();
   mVelocityOption          = RegisterOption(vectors, "Velocity")->type();
   mEarthOption             = RegisterOption(vectors, "Earth Nadir")->type();
   mSunOption               = RegisterOption(vectors, "Sun")->type();
   mMoonOption              = RegisterOption(vectors, "Moon")->type();
   vectors->setExpanded(false);

   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mPOI_VisibilityWidget          = ut::qt::make_qt_ptr<POI_VisibilityWidget>(mainWindowPtr);
   mainWindowPtr->addDockWidget(Qt::RightDockWidgetArea, mPOI_VisibilityWidget);
   mPOI_VisibilityWidget->hide();

   connect(mDisplayInterface,
           &AnnotationDisplayInterface::SetRangeRingOptionState,
           [&](bool aState, wkf::Platform* aPlatformPtr)
           { SetPlatformOptionState(mRangeRingOption, aState, aPlatformPtr); });
   connect(mDisplayInterface,
           &AnnotationDisplayInterface::SetDecorationOptionState,
           [&](bool aState, wkf::Platform* aPlatformPtr)
           { SetPlatformOptionState(mDecorationOption, aState, aPlatformPtr); });

   mCallbacks.Add(wkf::Observer::AttachmentAdded.Connect(&Plugin::AttachmentAddedHandler, this));
}

QList<wkf::PrefWidget*> Annotation::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   return prefs;
}

QList<wkf::Action*> Annotation::Plugin::GetActions() const
{
   QList<wkf::Action*> list;
   list << mDisplayInterface->GetBullseyeAction() << mPOI_VisibilityWidget->GetActions();
   return list;
}

void Annotation::Plugin::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   if (mEnableFeatures)
   {
      std::string name       = aEntityPtr->GetName();
      QString     qname      = QString::fromStdString(name);
      bool        isPlatform = aEntityPtr->IsA_TypeOf<wkf::Platform>();
      bool        isPOI      = aEntityPtr->IsA_TypeOf<wkf::PointOfInterest>();

      if (isPOI || isPlatform)
      {
         if (vaEnv.GetStandardScenario())
         {
            auto rrMenu = ut::qt::make_qt_ptr<QMenu>(QString("Range Rings"), aMenu);
            rrMenu->setIcon(QIcon::fromTheme("rangering"));
            aMenu->addMenu(rrMenu);
            auto rangeRing = ut::qt::make_qt_ptr<QAction>(QString("Add Range Ring to %1").arg(qname), rrMenu);
            connect(rangeRing,
                    &QAction::triggered,
                    this,
                    [=]()
                    {
                       auto entity = dynamic_cast<wkf::Entity*>(vaEnv.GetStandardScenario()->FindEntity(name));
                       if (entity)
                       {
                          mDisplayInterface->AddRangeRing(entity, true);
                          mDisplayInterface->EditRangeRing(entity, RingPropertiesWidget::RingFlags(true, true));
                       }
                    });
            rrMenu->addAction(rangeRing);

            if (aEntityPtr->FindFirstAttachmentOfType<wkf::AttachmentRangeRing>())
            {
               auto editRangeRing = ut::qt::make_qt_ptr<QAction>(QString("Edit Range Rings on %1").arg(qname), rrMenu);
               connect(editRangeRing,
                       &QAction::triggered,
                       this,
                       [=]()
                       {
                          auto entity = dynamic_cast<wkf::Entity*>(vaEnv.GetStandardScenario()->FindEntity(name));
                          if (entity)
                          {
                             mDisplayInterface->EditRangeRing(entity, RingPropertiesWidget::RingFlags(false, true));
                          }
                       });
               rrMenu->addAction(editRangeRing);
            }
            std::vector<wkf::AttachmentRangeRing*> rrList = aEntityPtr->FindAttachmentsOfType<wkf::AttachmentRangeRing>();
            for (auto&& it : rrList)
            {
               auto remRangeRing =
                  ut::qt::make_qt_ptr<QAction>(QString("Remove Range Ring \"%1\" from %2").arg(it->GetName().c_str(), qname),
                                               rrMenu);
               connect(remRangeRing,
                       &QAction::triggered,
                       this,
                       [=]()
                       {
                          auto entity = dynamic_cast<wkf::Entity*>(vaEnv.GetStandardScenario()->FindEntity(name));
                          if (entity)
                          {
                             ClearRangeRing(entity, it->GetUniqueId());
                          }
                       });
               rrMenu->addAction(remRangeRing);
            }
            if (isPlatform)
            {
               auto vectorMenu = ut::qt::make_qt_ptr<QMenu>("Custom Vectors", aMenu);
               aMenu->addMenu(vectorMenu);
               auto customvector = ut::qt::make_qt_ptr<QAction>("Add Custom Vectors...", vectorMenu);
               connect(customvector,
                       &QAction::triggered,
                       this,
                       [=]()
                       {
                          auto entity = dynamic_cast<wkf::Entity*>(vaEnv.GetStandardScenario()->FindEntity(name));
                          if (entity)
                          {
                             mDisplayInterface->CustomVector(entity, true);
                          }
                       });
               vectorMenu->addAction(customvector);
               vespa::VaAttachmentVectors*              vectors = nullptr;
               std::vector<vespa::VaAttachmentVectors*> vList =
                  aEntityPtr->FindAttachmentsOfType<vespa::VaAttachmentVectors>();
               for (auto&& vatt : vList)
               {
                  if (vatt->GetViewer() == vaEnv.GetStandardViewer())
                  {
                     vectors = vatt;
                  }
               }
               if (vectors)
               {
                  auto vectorList = mDisplayInterface->GetVectors()[aEntityPtr->GetUniqueId()];
                  for (const auto& cust : vectorList)
                  {
                     auto cv = ut::qt::make_qt_ptr<QAction>(QString("Remove ") + cust.second.mName, vectorMenu);
                     vectorMenu->addAction(cv);
                     connect(cv,
                             &QAction::triggered,
                             this,
                             [=]()
                             {
                                auto entity = dynamic_cast<wkf::Entity*>(vaEnv.GetStandardScenario()->FindEntity(name));
                                if (entity)
                                {
                                   mDisplayInterface->RemoveCustomVector(cust.first, entity);
                                }
                             });
                  }
               }
            }
         }
      }
      if (isPOI)
      {
         auto poi = ut::qt::make_qt_ptr<QAction>(QString("Edit Point of Interest"), aMenu);
         connect(poi,
                 &QAction::triggered,
                 this,
                 [=]()
                 {
                    auto entity = dynamic_cast<wkf::Entity*>(vaEnv.GetStandardScenario()->FindEntity(name));
                    if (entity)
                    {
                       mDisplayInterface->ShowPOIProperties(entity, false);
                    }
                 });
         aMenu->addAction(poi);
      }
      // Allow decorations to be added to Tracks and POIs (previously limited to Platforms)
      {
         auto markMenu = ut::qt::make_qt_ptr<QMenu>(QString("Decorate %1").arg(qname), aMenu);
         aMenu->addMenu(markMenu);
         auto spade =
            ut::qt::make_qt_ptr<QAction>(mDisplayInterface->GetDecorationIcon(":/icons/spade.png"), "Spade", markMenu);
         markMenu->addAction(spade);
         auto heart =
            ut::qt::make_qt_ptr<QAction>(mDisplayInterface->GetDecorationIcon(":/icons/heart.png"), "Heart", markMenu);
         markMenu->addAction(heart);
         auto diamond =
            ut::qt::make_qt_ptr<QAction>(mDisplayInterface->GetDecorationIcon(":/icons/diamond.png"), "Diamond", markMenu);
         markMenu->addAction(diamond);
         auto club =
            ut::qt::make_qt_ptr<QAction>(mDisplayInterface->GetDecorationIcon(":/icons/club.png"), "Club", markMenu);
         markMenu->addAction(club);
         auto one = ut::qt::make_qt_ptr<QAction>(mDisplayInterface->GetDecorationIcon(":/icons/one.png"), "One", markMenu);
         markMenu->addAction(one);
         auto two = ut::qt::make_qt_ptr<QAction>(mDisplayInterface->GetDecorationIcon(":/icons/two.png"), "Two", markMenu);
         markMenu->addAction(two);
         auto three =
            ut::qt::make_qt_ptr<QAction>(mDisplayInterface->GetDecorationIcon(":/icons/three.png"), "Three", markMenu);
         markMenu->addAction(three);
         auto four =
            ut::qt::make_qt_ptr<QAction>(mDisplayInterface->GetDecorationIcon(":/icons/four.png"), "Four", markMenu);
         markMenu->addAction(four);
         auto text = ut::qt::make_qt_ptr<QAction>("Annotation", markMenu);
         markMenu->addAction(text);
         auto image = ut::qt::make_qt_ptr<QAction>("Image", markMenu);
         markMenu->addAction(image);
         auto clear = ut::qt::make_qt_ptr<QAction>("None", markMenu);
         markMenu->addAction(clear);

         // Decorations do not need to set exportability, since they're re-created each time they're changed
         connect(heart, &QAction::triggered, this, [=]() { DecoratePreset(qname, ":/icons/heart.png"); });
         connect(spade, &QAction::triggered, this, [=]() { DecoratePreset(qname, ":/icons/spade.png"); });
         connect(diamond, &QAction::triggered, this, [=]() { DecoratePreset(qname, ":/icons/diamond.png"); });
         connect(club, &QAction::triggered, this, [=]() { DecoratePreset(qname, ":/icons/club.png"); });
         connect(one, &QAction::triggered, this, [=]() { DecoratePreset(qname, ":/icons/one.png"); });
         connect(two, &QAction::triggered, this, [=]() { DecoratePreset(qname, ":/icons/two.png"); });
         connect(three, &QAction::triggered, this, [=]() { DecoratePreset(qname, ":/icons/three.png"); });
         connect(four, &QAction::triggered, this, [=]() { DecoratePreset(qname, ":/icons/four.png"); });
         connect(text, &QAction::triggered, this, [=]() { DecorateText(qname); });
         connect(image, &QAction::triggered, this, [=]() { DecorateImage(qname); });
         connect(clear, &QAction::triggered, this, [=]() { ClearDecoration(qname); });
      }
   }
}

void Annotation::Plugin::BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr)
{
   if (aViewerPtr == vaEnv.GetStandardViewer() && mEnableFeatures)
   {
      auto addMenu = aMenu->findChild<QMenu*>("Add at Location");
      if (!addMenu)
      {
         addMenu = aMenu->addMenu("Add at Location");
         addMenu->setObjectName("Add at Location");
         UtQtTranslucentMenu(*addMenu);
      }

      vespa::VaPosition p = vaEnv.GetCurrentCursorPosition();
      auto bullseye       = ut::qt::make_qt_ptr<QAction>(QIcon::fromTheme("bullseye"), QString("Bullseye"), addMenu);
      addMenu->addAction(bullseye);
      auto pointOfInterest = ut::qt::make_qt_ptr<QAction>(QIcon::fromTheme("poi"), QString("Point of Interest"), addMenu);
      addMenu->addAction(pointOfInterest);
      auto rangeRing = ut::qt::make_qt_ptr<QAction>(QIcon::fromTheme("rangering"), QString("Range Ring"), addMenu);
      addMenu->addAction(rangeRing);

      connect(rangeRing, &QAction::triggered, this, [=]() { mDisplayInterface->AddRangeRing(p.GetLat(), p.GetLon()); });
      connect(bullseye,
              &QAction::triggered,
              this,
              [=]() { mDisplayInterface->AddPOI<wkf::Bullseye>("bullseye", p.GetLat(), p.GetLon()); });
      connect(pointOfInterest,
              &QAction::triggered,
              this,
              [=]()
              {
                 auto poi = mDisplayInterface->AddPOI<wkf::PointOfInterest>("pointOfInterest", p.GetLat(), p.GetLon(), "");
                 mDisplayInterface->ShowPOIProperties(poi, true);
              });
   }
}

void Annotation::Plugin::ResetOptionStates()
{
   wkf::Scenario* scenPtr = vaEnv.GetStandardScenario();
   if (scenPtr)
   {
      const std::map<unsigned int, wkf::Platform*>& platMap = scenPtr->GetIndexPlatformMap();
      for (auto& it : platMap)
      {
         std::set<int> idList;

         for (auto& jt : it.second->GetAttachments())
         {
            if (jt->IsA_TypeOf<wkf::AttachmentRangeRing>())
            {
               idList.insert(jt->GetUniqueId());
            }
            if (jt->IsA_TypeOf<vespa::VaAttachmentVectors>())
            {
               idList.insert(jt->GetUniqueId());
            }
         }
         for (auto& jt : idList)
         {
            it.second->RemoveAttachment(jt);
         }
      }
   }
}

void Annotation::Plugin::SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr)
{
   if (aPlatformPtr)
   {
      if (aOptionId == mRangeRingOption)
      {
         mRingVisibility[aPlatformPtr->GetUniqueId()] = aState;
         for (auto&& it : aPlatformPtr->GetAttachments())
         {
            auto* rr = dynamic_cast<wkf::AttachmentRangeRing*>(it.get());
            if (rr)
            {
               rr->SetStateVisibility(aState);
            }
         }
      }
      else if (aOptionId == mDecorationOption)
      {
         mDecorationVisibility[aPlatformPtr->GetUniqueId()] = aState;
         auto* decorator = aPlatformPtr->FindFirstAttachmentOfType<AnnotationDecorator>();
         if (decorator)
         {
            decorator->SetStateVisibility(aState);
         }
      }
      else // is a vector option
      {
         int type = 0;
         if (aOptionId == mBodyFrameOption)
         {
            type = vespa::VaAttachmentVectors::eVECTOR_BODY_XYZ;
         }
         else if (aOptionId == mVelocityOption)
         {
            type = vespa::VaAttachmentVectors::eVECTOR_VELOCITY;
         }
         else if (aOptionId == mEarthOption)
         {
            type = vespa::VaAttachmentVectors::eVECTOR_EARTH;
         }
         else if (aOptionId == mSunOption)
         {
            type = vespa::VaAttachmentVectors::eVECTOR_SUN;
         }
         else if (aOptionId == mMoonOption)
         {
            type = vespa::VaAttachmentVectors::eVECTOR_MOON;
         }
         mDisplayInterface->ShowVector(*aPlatformPtr, type, aState);
      }
   }
}

Qt::CheckState Annotation::Plugin::GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const
{
   if (aPlatformPtr)
   {
      if (aOptionId == mRangeRingOption)
      {
         // The range ring box may be checked even if there are no range rings on the platform
         unsigned int id         = aPlatformPtr->GetUniqueId();
         bool         visibility = (mRingVisibility.count(id) > 0) ? mRingVisibility.at(id) : false;
         return (visibility) ? Qt::Checked : Qt::Unchecked;
      }
      else if (aOptionId == mDecorationOption)
      {
         unsigned int id         = aPlatformPtr->GetUniqueId();
         bool         visibility = (mDecorationVisibility.count(id) > 0) ? mDecorationVisibility.at(id) : false;
         return (visibility) ? Qt::Checked : Qt::Unchecked;
      }
      else // is a vector option
      {
         std::vector<vespa::VaAttachmentVectors*> vec = aPlatformPtr->FindAttachmentsOfType<vespa::VaAttachmentVectors>();
         vespa::VaAttachmentVectors*              vectors = nullptr;
         for (auto&& it : vec)
         {
            if (it->IsMyViewer(vaEnv.GetStandardViewer()))
            {
               vectors = it;
            }
         }
         if (vectors)
         {
            if (aOptionId == mBodyFrameOption)
            {
               return vectors->IsShown(vespa::VaAttachmentVectors::eVECTOR_BODY_XYZ) ? Qt::Checked : Qt::Unchecked;
            }
            else if (aOptionId == mVelocityOption)
            {
               return vectors->IsShown(vespa::VaAttachmentVectors::eVECTOR_VELOCITY) ? Qt::Checked : Qt::Unchecked;
            }
            else if (aOptionId == mEarthOption)
            {
               return vectors->IsShown(vespa::VaAttachmentVectors::eVECTOR_EARTH) ? Qt::Checked : Qt::Unchecked;
            }
            else if (aOptionId == mSunOption)
            {
               return vectors->IsShown(vespa::VaAttachmentVectors::eVECTOR_SUN) ? Qt::Checked : Qt::Unchecked;
            }
            else if (aOptionId == mMoonOption)
            {
               return vectors->IsShown(vespa::VaAttachmentVectors::eVECTOR_MOON) ? Qt::Checked : Qt::Unchecked;
            }
         }
      }
   }
   return Qt::Unchecked;
}

void Annotation::Plugin::DeleteCurrentlySelectedAnnotations()
{
   auto viewerPtr = vaEnv.GetStandardViewer();
   if (viewerPtr)
   {
      vespa::VaHitSet selection = viewerPtr->GetSelection();
      for (const auto& hit : selection)
      {
         wkf::PointOfInterest* poi = dynamic_cast<wkf::PointOfInterest*>(hit.FindEntity());
         if (poi)
         {
            if (poi->GetManagingAttachment())
            {
               vaEnv.DeleteEntity(&(poi->GetManagingAttachment()->GetParent()));
            }
            else
            {
               vaEnv.DeleteEntity(poi);
            }
         }
      }
   }
}

void Annotation::Plugin::DecoratePreset(const QString& aName, const QString& aPath)
{
   mDisplayInterface->Decorate(aName, mDisplayInterface->GetDecorationIcon(aPath), aPath);
}

bool Annotation::Plugin::DecorateText(const QString& aName)
{
   return mDisplayInterface->DecorateWithText(aName);
}

bool Annotation::Plugin::DecorateImage(const QString& aName)
{
   return mDisplayInterface->DecorateWithImage(aName);
}

void Annotation::Plugin::ClearDecoration(const QString& aName)
{
   mDisplayInterface->Decorate(aName, mDisplayInterface->GetDecorationIcon(""), "");
}

void Annotation::Plugin::ClearRangeRing(wkf::Entity* aEntityPtr, unsigned int aUniqueId)
{
   aEntityPtr->RemoveAttachment(aUniqueId); /*mChanges = true;*/
}

void Annotation::Plugin::AttachmentAddedHandler(vespa::VaAttachment* aAttachment)
{
   auto handler = [aAttachment](bool aCorrectType, const std::unordered_map<unsigned int, bool>& aMap)
   {
      if (aCorrectType)
      {
         wkf::Platform* plat = dynamic_cast<wkf::Platform*>(&(aAttachment->GetParent()));
         if (plat)
         {
            unsigned int id         = plat->GetUniqueId();
            bool         visibility = (aMap.count(id) > 0) ? aMap.at(id) : false;
            aAttachment->SetStateVisibility(visibility);
         }
      }
   };

   handler(aAttachment->IsA_TypeOf<wkf::AttachmentRangeRing>(), mRingVisibility);
   handler(aAttachment->IsA_TypeOf<AnnotationDecorator>(), mDecorationVisibility);
}
