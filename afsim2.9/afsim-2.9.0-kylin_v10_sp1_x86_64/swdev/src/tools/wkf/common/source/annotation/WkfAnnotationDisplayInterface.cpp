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

#include "WkfAnnotationDisplayInterface.hpp"

#include <QApplication>
#include <QDialogButtonBox>
#include <QEvent>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QSet>
#include <QTextStream>

#include "UtMemory.hpp"
#include "UtQtColorButton.hpp"
#include "UtQtMemory.hpp"
#include "VaAttachmentModel.hpp"
#include "VaAttachmentVectors.hpp"
#include "VaEnvironment.hpp"
#include "VaUtils.hpp"
#include "WkfAction.hpp"
#include "WkfAnnotationDecorator.hpp"
#include "WkfAttachmentRangeRing.hpp"
#include "WkfBullseye.hpp"
#include "WkfBullseyeWidget.hpp"
#include "WkfEntity.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfPointOfInterest.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "selectors/WkfGroupingSelector.hpp"

namespace
{
const QMap<QString, QString> cDECORATOR_PATH_TO_PRESET = {{":/icons/heart.png", "heart"},
                                                          {":/icons/spade.png", "spade"},
                                                          {":/icons/diamond.png", "diamond"},
                                                          {":/icons/club.png", "club"},
                                                          {":/icons/one.png", "one"},
                                                          {":/icons/two.png", "two"},
                                                          {":/icons/three.png", "three"},
                                                          {":/icons/four.png", "four"}};

const std::map<std::string, QString> cDECORATOR_PRESET_TO_PATH = {{"heart", ":/icons/heart.png"},
                                                                  {"spade", ":/icons/spade.png"},
                                                                  {"diamond", ":/icons/diamond.png"},
                                                                  {"club", ":/icons/club.png"},
                                                                  {"one", ":/icons/one.png"},
                                                                  {"two", ":/icons/two.png"},
                                                                  {"three", ":/icons/three.png"},
                                                                  {"four", ":/icons/four.png"}};

const std::string cTAB  = "   ";
const std::string c2TAB = "      ";

std::string GetColorInts(const QColor& aColor)
{
   std::stringstream ss;
   ss << aColor.red() << " " << aColor.green() << " " << aColor.blue();
   return ss.str();
}

std::string GetPoiExport(vespa::VaEntity* aEntity)
{
   std::stringstream poi_ss;
   auto*             poi = dynamic_cast<wkf::PointOfInterest*>(aEntity);
   if (poi && poi->GetExportable())
   {
      poi_ss << cTAB << "poi " << poi->GetName() << "\n";
      poi_ss << c2TAB << "position " << poi->GetPosition().GetLat() << " " << poi->GetPosition().GetLon() << "\n";
      if (dynamic_cast<wkf::Bullseye*>(poi) != nullptr)
      {
         poi_ss << c2TAB << "bullseye\n";
      }
      else // altitude should be omitted for bullseyes (always 0)
      {
         poi_ss << c2TAB << "altitude " << poi->GetPosition().GetAlt() << "\n";
      }
      poi_ss << cTAB << "end_poi\n";
   }
   return poi_ss.str();
}

enum class EntityType
{
   cPLATFORM,
   cPOI,
   cNONE
};

int RoundAngle(double aRad)
{
   return static_cast<int>(std::round(UtMath::cDEG_PER_RAD * aRad));
}

std::string GetRangeRingExport(const EntityType&                           aEntityType,
                               const std::string&                          aEntityName,
                               const wkf::AttachmentRangeRing::Properties& aProperties)
{
   std::stringstream rr_ss;
   rr_ss << cTAB << "range_ring " << aProperties.mName.toStdString() << "\n";
   if (aEntityType == EntityType::cPLATFORM || aEntityType == EntityType::cPOI)
   {
      rr_ss << c2TAB << "entity " << aEntityName << "\n";
   }
   if (aProperties.mShowRangeLabels)
   {
      rr_ss << c2TAB << "show_range_labels\n";
      rr_ss << c2TAB << "range_label_color " << GetColorInts(aProperties.mRangeLabelColor) << "\n";
   }
   rr_ss << c2TAB << "ring_color " << GetColorInts(aProperties.mRingColor) << "\n";
   rr_ss << c2TAB << "ring_width " << aProperties.mRingLineWidth << "\n";
   rr_ss << c2TAB << "center_radius " << aProperties.mBaseRadius << " m\n";
   rr_ss << c2TAB << "additional_rings " << aProperties.mAdditionalRings << "\n";
   rr_ss << c2TAB << "additional_radii " << aProperties.mAdditionalRingRadius << " m\n";
   if (aProperties.mShowAngleLabels)
   {
      rr_ss << c2TAB << "show_angle_labels\n";
      rr_ss << c2TAB << "angle_label_color " << GetColorInts(aProperties.mAngleLabelColor) << "\n";
   }
   rr_ss << c2TAB << "radial_color " << GetColorInts(aProperties.mRadialColor) << "\n";
   rr_ss << c2TAB << "radial_width " << aProperties.mRadialLineWidth << "\n";
   rr_ss << c2TAB << "center_angle " << RoundAngle(aProperties.mCenterAngle) << " deg\n";
   rr_ss << c2TAB << "angle_width " << RoundAngle(aProperties.mWidth) << " deg\n";
   rr_ss << c2TAB << "radial_rate " << RoundAngle(aProperties.mRadialRate) << " deg\n";
   rr_ss << cTAB << "end_range_ring\n";
   return rr_ss.str();
}

std::string GetVerifiedRangeRingExport(vespa::VaEntity* aEntity, vespa::VaAttachment* aAttachment)
{
   wkf::AttachmentRangeRing* rr = dynamic_cast<wkf::AttachmentRangeRing*>(aAttachment);
   if (aAttachment->IsA_TypeOf<wkf::AttachmentRangeRing>() && rr && rr->GetProperties().mExportable)
   {
      EntityType entityType;
      if (aEntity->IsA_TypeOf<wkf::Platform>())
      {
         entityType = EntityType::cPLATFORM;
      }
      else if (aEntity->IsA_TypeOf<wkf::PointOfInterest>())
      {
         entityType = EntityType::cPOI;
      }
      else
      {
         entityType = EntityType::cNONE;
      }
      return GetRangeRingExport(entityType, aEntity->GetName(), rr->GetProperties());
   }
   return "";
}

std::string GetDecorationExport(const std::string& aEntityName, const QString& aText, const QString& aDesc)
{
   std::stringstream dec_ss;
   dec_ss << cTAB << "decoration " << aEntityName << "\n" << c2TAB;
   if (!aText.isEmpty())
   {
      dec_ss << "text \"" << aText.toStdString() << "\"\n";
   }
   else if (!aDesc.isEmpty())
   {
      auto decEnumIt = cDECORATOR_PATH_TO_PRESET.find(aDesc);
      if (decEnumIt != cDECORATOR_PATH_TO_PRESET.end())
      {
         dec_ss << "preset " << cDECORATOR_PATH_TO_PRESET[aDesc].toStdString() << "\n";
      }
      else
      {
         dec_ss << "image \"" << aDesc.toStdString() << "\"\n";
      }
   }
   else
   {
      return "";
   }
   dec_ss << cTAB << "end_decoration\n";
   return dec_ss.str();
}

std::string GetVerifiedDecorationExport(vespa::VaEntity* aEntity, vespa::VaAttachment* aAttachment)
{
   Annotation::AnnotationDecorator* dec = dynamic_cast<Annotation::AnnotationDecorator*>(aAttachment);
   if (dec && dec->GetExportable())
   {
      return GetDecorationExport(aEntity->GetName(), dec->GetText(), dec->GetImageDescription());
   }
   return "";
}
} // namespace

Annotation::AnnotationDisplayInterface::AnnotationDisplayInterface(QObject* aParent)
   : QObject(aParent)
   , mActiveAnnotationFile("")
   , mEntityAddedUID(-1)
   , mTempSettings(nullptr)
   , mBullseyeWidgetPtr(ut::qt::make_qt_ptr<BullseyeWidget>(*this, wkfEnv.GetMainWindow()))
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mPOI_PropertiesPtr             = dynamic_cast<Annotation::PointOfInterestPropertiesWidget*>(
      vaEnv.GetFactory()->CreateWidget("PointOfInterestProperties", mainWindowPtr));
   mRingPropertiesPtr = dynamic_cast<Annotation::RingPropertiesWidget*>(
      vaEnv.GetFactory()->CreateWidget("RangeRingProperties", mainWindowPtr));

   if (wkfEnv.GetApplicationName() != "wizard")
   {
      QMenu* fileMenuPtr = mainWindowPtr->GetMenuByName("File");
      if (fileMenuPtr)
      {
         auto exportAnnotations =
            ut::qt::make_qt_ptr<QAction>(QIcon::fromTheme(""), "Export Annotations...", wkfEnv.GetMainWindow());
         connect(exportAnnotations, &QAction::triggered, this, &AnnotationDisplayInterface::ExportAnnotations);
         fileMenuPtr->insertAction(fileMenuPtr->actions().back(), exportAnnotations);
      }
   }

   mCallbacks.Add(wkf::Observer::EntityDecorated.Connect(&AnnotationDisplayInterface::EntityDecoratedCB, this));
   mCallbacks.Add(wkf::Observer::EntitySelected.Connect(&AnnotationDisplayInterface::EntitySelectedCB, this));
   mCallbacks.Add(wkf::Observer::StandardScenarioAdded.Connect(&AnnotationDisplayInterface::StandardScenarioAddedCB, this));
   mCallbacks.Add(
      wkf::Observer::StandardScenarioRemoved.Connect(&AnnotationDisplayInterface::StandardScenarioRemovedCB, this));
   mCallbacks.Add(wkf::Observer::EntityDeleted.Connect(&AnnotationDisplayInterface::PlatformDeletedCB, this));
   mCallbacks.Add(wkf::Observer::AttachmentDeleted.Connect(&AnnotationDisplayInterface::AttachmentDeletedCB, this));

   mBullseyeActionPtr = ut::qt::make_qt_ptr<wkf::Action>(QString("Bullseye Dialog"),
                                                         wkfEnv.GetMainWindow(),
                                                         QKeySequence(Qt::CTRL + Qt::Key_B));
   connect(mBullseyeActionPtr, &QAction::triggered, this, [this]() { mBullseyeWidgetPtr->Show(); });
   connect(&wkfEnv,
           &wkf::Environment::MoveSelectionAndAttachmentComplete,
           this,
           &Annotation::AnnotationDisplayInterface::MoveSelectionAndAttachmentComplete);
   connect(
      mRingPropertiesPtr,
      &RingPropertiesWidget::RingAssigned,
      this,
      [&](wkf::Platform* aPlat) { emit SetRangeRingOptionState(true, aPlat); },
      Qt::DirectConnection);
   connect(mPOI_PropertiesPtr,
           &PointOfInterestPropertiesWidget::PoiBullseyeToggled,
           this,
           &Annotation::AnnotationDisplayInterface::ToggleBullseye);
}

void Annotation::AnnotationDisplayInterface::AddRangeRing(const double& aLat, const double& aLon)
{
   vespa::VaViewer* viewer   = vaEnv.GetStandardViewer();
   wkf::Scenario*   scenario = vaEnv.GetStandardScenario();
   if (viewer && scenario)
   {
      wkf::PointOfInterest* entity = vaEnv.CreateEntity<wkf::PointOfInterest>("pointOfInterest");
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

      std::string name = entity->GetName() + "_RangeRing";
      // name = GetRingPropertiesWidget()->CheckDuplicateName(name);
      vespa::make_attachment<wkf::AttachmentRangeRing>(*entity, vaEnv.GetStandardViewer(), name);
      EditRangeRing(entity, RingPropertiesWidget::RingFlags(true, false));
   }
}

void Annotation::AnnotationDisplayInterface::ShowVector(wkf::Entity& aEntity, int aTypeIndex, bool aState)
{
   vespa::VaViewer* viewer = vaEnv.GetStandardViewer();
   if (viewer)
   {
      std::vector<vespa::VaAttachmentVectors*> vec     = aEntity.FindAttachmentsOfType<vespa::VaAttachmentVectors>();
      vespa::VaAttachmentVectors*              vectors = nullptr;
      for (auto&& it : vec)
      {
         if (it->IsMyViewer(viewer))
         {
            vectors = it;
         }
      }
      if (aState && !vectors)
      {
         vectors = vaEnv.CreateAttachment<vespa::VaAttachmentVectors>("vectors", aEntity, viewer);
      }
      if (vectors)
      {
         vectors->ShowVector(aTypeIndex, aState);
      }
   }
}

void Annotation::AnnotationDisplayInterface::AddCustomVector(wkf::Entity&                 aEntity,
                                                             const wkf::PlatformGrouping& aGrouping,
                                                             const QColor&                aColor,
                                                             bool                         aState)
{
   vespa::VaViewer* viewer = vaEnv.GetStandardViewer();
   if (viewer)
   {
      std::vector<vespa::VaAttachmentVectors*> vec     = aEntity.FindAttachmentsOfType<vespa::VaAttachmentVectors>();
      vespa::VaAttachmentVectors*              vectors = nullptr;
      for (auto&& it : vec)
      {
         if (it->IsMyViewer(viewer))
         {
            vectors = it;
         }
      }
      if (aState && !vectors)
      {
         vectors = vaEnv.CreateAttachment<vespa::VaAttachmentVectors>("vectors", aEntity, viewer);
      }
      if (vectors)
      {
         if (aGrouping.Type() == wkf::PlatformGrouping::eIndividual)
         {
            wkf::Platform* platform = vaEnv.GetStandardScenario()->FindPlatformByName(aGrouping.Name());
            if (platform)
            {
               QString label = QString::fromStdString(aGrouping.Name());

               int index =
                  vectors->AddVector(label.toStdString(), aColor.redF(), aColor.greenF(), aColor.blueF(), aColor.alphaF());
               mCustomVectors[aEntity.GetUniqueId()][index] = CustomVectorData(label, aGrouping, aColor);
               vectors->AddVectorToEntity(index, platform);
            }
         }
         else
         {
            const std::map<wkf::PlatformGrouping, std::set<int>>& groupMap   = wkfEnv.GetGroupings();
            auto                                                  groupEntry = groupMap.find(aGrouping);
            if (groupEntry != groupMap.end())
            {
               for (auto&& cv : mCustomVectors[aEntity.GetUniqueId()])
               {
                  if (cv.second.mGrouping == aGrouping)
                  {
                     return; // deny duplicate groupings
                  }
               }

               QString label;
               if (aGrouping.Type() < wkf::PlatformGrouping::eGroupingTypeCount)
               {
                  label += QString::fromStdString(aGrouping.TypeString());
                  label += " = ";
               }
               label += QString::fromStdString(aGrouping.Name());

               int index =
                  vectors->AddVector(label.toStdString(), aColor.redF(), aColor.greenF(), aColor.blueF(), aColor.alphaF());
               mCustomVectors[aEntity.GetUniqueId()][index] = CustomVectorData(label, aGrouping, aColor);

               for (auto&& platIdx : groupEntry->second)
               {
                  vespa::VaEntity* ent = vaEnv.GetStandardScenario()->FindPlatformByIndex(platIdx);
                  if (ent)
                  {
                     vectors->AddVectorToEntity(index, ent);
                  }
               }
            }
         }
      }
   }
}

void Annotation::AnnotationDisplayInterface::RemoveCustomVector(int aIndex, wkf::Entity* aEntityPtr)
{
   vespa::VaViewer* viewer = vaEnv.GetStandardViewer();
   if (viewer)
   {
      std::vector<vespa::VaAttachmentVectors*> vec = aEntityPtr->FindAttachmentsOfType<vespa::VaAttachmentVectors>();
      vespa::VaAttachmentVectors*              vectors = nullptr;
      for (auto&& it : vec)
      {
         if (it->IsMyViewer(viewer))
         {
            vectors = it;
         }
      }
      if (vectors)
      {
         vectors->RemoveVector(aIndex);
      }
      mCustomVectors[aEntityPtr->GetUniqueId()].erase(aIndex);
   }
}

void Annotation::AnnotationDisplayInterface::CustomVector(wkf::Entity* aEntityPtr, bool aUI_Request)
{
   auto selector = ut::qt::make_qt_ptr<QDialog>(wkfEnv.GetMainWindow());
   selector->setWindowTitle("Add Vector");
   auto layout      = ut::qt::make_qt_ptr<QVBoxLayout>(selector);
   auto dbb         = ut::qt::make_qt_ptr<QDialogButtonBox>(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, selector);
   auto groupSelect = ut::qt::make_qt_ptr<wkf::GroupingSelector>(selector);
   auto colorButton = ut::qt::make_qt_ptr<UtQtColorButton>(selector);
   colorButton->SetColor(Qt::red);
   layout->addWidget(ut::qt::make_qt_ptr<QLabel>("Point vector to:", selector));
   layout->addWidget(groupSelect);
   layout->addWidget(ut::qt::make_qt_ptr<QLabel>("Color:", selector));
   layout->addWidget(colorButton);
   layout->addWidget(dbb);
   connect(dbb, &QDialogButtonBox::accepted, selector, &QDialog::accept);
   connect(dbb, &QDialogButtonBox::rejected, selector, &QDialog::reject);
   if (QDialog::Accepted == selector->exec())
   {
      QColor                       color = colorButton->GetColor();
      const wkf::PlatformGrouping& g     = groupSelect->GetGrouping();
      AddCustomVector(*aEntityPtr, g, color, true);
   }
   delete dbb;
}

void Annotation::AnnotationDisplayInterface::ShowPOIProperties(wkf::Entity* aEntityPtr, bool aCreate)
{
   if (aEntityPtr)
   {
      mPOI_PropertiesPtr->SetEntity(aEntityPtr, aCreate);
      mPOI_PropertiesPtr->show();
   }
}

void Annotation::AnnotationDisplayInterface::EditRangeRing(wkf::Entity* aEntityPtr,
                                                           const Annotation::RingPropertiesWidget::RingFlags& aFlags)
{
   if (aEntityPtr)
   {
      auto* plat = dynamic_cast<wkf::Platform*>(aEntityPtr);
      if (plat) // if it is a platform
      {
         wkf::PlatformList pl;
         pl.push_back(plat);
         wkfEnv.SetPlatformsSelected(pl);
      }
      else
      {
         vaEnv.GetStandardViewer()->ClearSelection();
         aEntityPtr->SetSelected(vaEnv.GetStandardViewer(), true);
      }
      mRingPropertiesPtr->SetEntity(aEntityPtr, aFlags);
      mRingPropertiesPtr->show();
   }
}

void Annotation::AnnotationDisplayInterface::AddRangeRing(wkf::Entity* aEntityPtr, bool aUI_Request)
{
   if (aEntityPtr)
   {
      std::string name = aEntityPtr->GetName() + "_RangeRing";
      name             = GetRingPropertiesWidget()->CheckDuplicateName(name);
      vespa::make_attachment<wkf::AttachmentRangeRing>(*aEntityPtr, vaEnv.GetStandardViewer(), name);
      if (aUI_Request)
      {
         auto* platform = dynamic_cast<wkf::Platform*>(aEntityPtr);
         if (platform)
         {
            // if this command came from UI
            emit SetRangeRingOptionState(true, platform);
            // set the option as true.  Adding a range ring is an implicit option set on the individual!
         }
      }
   }
}

void Annotation::AnnotationDisplayInterface::Decorate(const QString& aEntityName, const QIcon& aIcon, const QString& aDescriptor)
{
   auto wkfEntity = dynamic_cast<wkf::Entity*>(vaEnv.FindEntity(aEntityName.toStdString()));
   if (wkfEntity)
   {
      wkfEntity->SetDecoration(aIcon, aDescriptor);
      auto* d = wkfEntity->FindFirstAttachmentOfType<AnnotationDecorator>();
      if (d)
      {
         d->SetExportable(true);
         auto* platform = dynamic_cast<wkf::Platform*>(wkfEntity);
         if (platform)
         {
            // User adding decoration via UI changes the option state
            emit SetDecorationOptionState(true, platform);
         }
      }
   }
}

bool Annotation::AnnotationDisplayInterface::DecorateWithText(const QString& aEntityName)
{
   QString pretext = "";

   vespa::VaEntity* entityPtr = vaEnv.FindEntity(aEntityName.toStdString());
   if (entityPtr)
   {
      auto* d = entityPtr->FindFirstAttachmentOfType<AnnotationDecorator>();
      if (d)
      {
         pretext = d->GetText();
      }
   }
   QInputDialog indlg(wkfEnv.GetMainWindow());
   indlg.setInputMode(QInputDialog::TextInput);
   indlg.setWindowTitle("Text Annotation");
   indlg.setTextValue(pretext);
   indlg.setOption(QInputDialog::UsePlainTextEditForTextInput);
   indlg.setMinimumSize(indlg.minimumSizeHint());
   // Note: a bug in Qt 5.9 causes a warning with the following, the setMinimumSize above avoids the warning
   //   QString text = QInputDialog::getMultiLineText(wkfEnv.GetMainWindow(), "Platform Annotation", "", pretext);

   if (indlg.exec() == QDialog::Accepted)
   {
      QString text = indlg.textValue();
      Decorate(aEntityName, GetDecorationIcon(""), "");

      // Find the entity again, to verify it was not removed since the dialog was opened.
      auto* ent = dynamic_cast<wkf::Entity*>(vaEnv.FindEntity(aEntityName.toStdString()));
      if (ent)
      {
         DecorateWithText(*ent, text);
         auto* platform = dynamic_cast<wkf::Platform*>(ent);
         if (platform)
         {
            // User adding decoration via UI changes the option state
            emit SetDecorationOptionState(true, platform);
         }
         return true;
      }
   }
   return false;
}

void Annotation::AnnotationDisplayInterface::DecorateWithText(wkf::Entity& aEntity, const QString& aText)
{
   auto* d = aEntity.FindFirstAttachmentOfType<AnnotationDecorator>();
   if (d)
   {
      d->SetText(aText);
      d->SetExportable(true);
   }
   else
   {
      vespa::make_attachment<AnnotationDecorator>(aEntity, vaEnv.GetStandardViewer(), aText);
   }
}

bool Annotation::AnnotationDisplayInterface::DecorateWithImage(const QString& aEntityName)
{
   QString      preImage  = "";
   wkf::Entity* entityPtr = dynamic_cast<wkf::Entity*>(vaEnv.FindEntity(aEntityName.toStdString()));
   if (entityPtr)
   {
      auto* d = entityPtr->FindFirstAttachmentOfType<AnnotationDecorator>();
      if (d)
      {
         preImage = d->GetImageDescription();
         preImage = (cDECORATOR_PATH_TO_PRESET.find(preImage) != cDECORATOR_PATH_TO_PRESET.end()) ? "" : preImage;
      }
      ImageDialog imgDialog(wkfEnv.GetMainWindow());
      imgDialog.setWindowTitle("Image Annotation");
      imgDialog.mImageSelectionWidget->SetText(preImage);
      connect(imgDialog.mDialogButtonBox, &QDialogButtonBox::accepted, &imgDialog, &QDialog::accept);
      connect(imgDialog.mDialogButtonBox, &QDialogButtonBox::rejected, &imgDialog, &QDialog::reject);
      if (imgDialog.exec() == QDialog::Accepted)
      {
         QString imgPath = imgDialog.mImageSelectionWidget->GetText();
         // Find the entity again, to verify it was not removed since the dialog was opened.
         wkf::Entity* entityPtr = dynamic_cast<wkf::Entity*>(vaEnv.FindEntity(aEntityName.toStdString()));
         if (entityPtr)
         {
            entityPtr->SetDecoration(QIcon(imgPath), imgPath);
            auto* platform = dynamic_cast<wkf::Platform*>(entityPtr);
            if (platform)
            {
               // User adding decoration via UI changes the option state
               emit SetDecorationOptionState(true, platform);
            }
            return true;
         }
      }
   }
   return false;
}

void Annotation::AnnotationDisplayInterface::EntityDecoratedCB(wkf::Entity*       aEntity,
                                                               const QIcon&       aIcon,
                                                               const std::string& aDescription)
{
   // invalid icons are not always null, so use the pixmap to check for validity
   QPixmap pix(QString::fromStdString(aDescription));
   auto*   d = aEntity->FindFirstAttachmentOfType<AnnotationDecorator>();
   if (d)
   {
      if (!pix.isNull())
      {
         d->SetImage(aIcon.pixmap(32, 32).toImage(), QString::fromStdString(aDescription));
      }
      else
      {
         aEntity->RemoveAttachment(d->GetUniqueId());
      }
   }
   else
   {
      if (!pix.isNull())
      {
         vespa::make_attachment<AnnotationDecorator>(*aEntity,
                                                     vaEnv.GetStandardViewer(),
                                                     aIcon.pixmap(32, 32).toImage(),
                                                     QString::fromStdString(aDescription));
      }
   }
}

void Annotation::AnnotationDisplayInterface::EntitySelectedCB(vespa::VaViewer* aViewerPtr,
                                                              int              aSelected,
                                                              vespa::VaEntity* aEntityPtr)
{
   mRingPropertiesPtr->SetEntity(nullptr, RingPropertiesWidget::RingFlags(false, true));
   mRingPropertiesPtr->hide();
   mPOI_PropertiesPtr->SetEntity(nullptr, false);
   mPOI_PropertiesPtr->hide();
   mBullseyeWidgetPtr->hide();
}

void Annotation::AnnotationDisplayInterface::PointOfInterestMovedCB(vespa::VaEntity* aEntityPtr, bool aUiDriven) {}

void Annotation::AnnotationDisplayInterface::PointOfInterestDeletedCB(vespa::VaEntity* aEntityPtr) {}

void Annotation::AnnotationDisplayInterface::PlatformAddedCB(const wkf::PlatformList& aPlatforms)
{
   for (auto& platform : aPlatforms)
   {
      QString name = QString::fromStdString(platform->GetName());
      auto    it   = mDeferredRangeRingMap.find(name);
      if (it != mDeferredRangeRingMap.end())
      {
         for (auto&& jt : it->second)
         {
            AddRangeRing(*platform, jt);
         }
         it->second.clear();
         mDeferredRangeRingMap.erase(it);
      }
      auto jt = mDeferredDecorationMap.find(name);
      if (jt != mDeferredDecorationMap.end())
      {
         AddDecoration(*platform, jt->second.first, jt->second.second);
         auto* dec = platform->FindFirstAttachmentOfType<Annotation::AnnotationDecorator>();
         if (dec)
         {
            dec->SetExportable(false);
         }
         mDeferredDecorationMap.erase(jt);
      }
      auto kt = mDeferredCustomVectorMap.find(name);
      if (kt != mDeferredCustomVectorMap.end())
      {
         for (auto&& lt : kt->second)
         {
            AddCustomVector(*platform, lt.mGrouping, lt.mColor, true);
         }
      }
      if (mDeferredRangeRingMap.empty() && mDeferredDecorationMap.empty() && mDeferredCustomVectorMap.empty())
      {
         mCallbacks.Disconnect(mEntityAddedUID);
         mEntityAddedUID = -1;
      }
   }
}

void Annotation::AnnotationDisplayInterface::StandardScenarioAddedCB(wkf::Scenario* aScenarioPtr)
{
   ClearAnnotations();
}

void Annotation::AnnotationDisplayInterface::StandardScenarioRemovedCB(wkf::Scenario* aScenarioPtr) {}

void Annotation::AnnotationDisplayInterface::PlatformDeletedCB(vespa::VaEntity* aPlatformPtr)
{
   if (dynamic_cast<wkf::Platform*>(aPlatformPtr))
   {
      const auto& al   = aPlatformPtr->GetAttachments();
      QString     name = QString::fromStdString(aPlatformPtr->GetName());
      for (auto&& ait : al)
      {
         if (ait->IsA_TypeOf<wkf::AttachmentRangeRing>())
         {
            auto* rr = dynamic_cast<wkf::AttachmentRangeRing*>(ait.get());
            if (rr->GetProperties().mExportable)
            {
               mDeletedPlatformRangeRingMap[name].push_back(rr->GetProperties());
            }
         }
         else if (ait->IsA_TypeOf<AnnotationDecorator>())
         {
            auto* dec = dynamic_cast<AnnotationDecorator*>(ait.get());
            if (dec->GetExportable())
            {
               mDeletedPlatformDecorationMap[name].first  = dec->GetText();
               mDeletedPlatformDecorationMap[name].second = dec->GetImageDescription();
            }
         }
         for (auto&& jt : mCustomVectors[aPlatformPtr->GetUniqueId()])
         {
            mDeletedCustomVectorMap[name].push_back(jt.second);
         }
      }
   }
}

void Annotation::AnnotationDisplayInterface::AttachmentDeletedCB(vespa::VaAttachment*            aAttachmentPtr,
                                                                 const vespa::VaAttachmentSubId& aSubpartId)
{
   if (mRingPropertiesPtr)
   {
      const wkf::AttachmentRangeRing* rr = mRingPropertiesPtr->GetRing();
      if (rr && ((rr->GetUniqueId() == aAttachmentPtr->GetUniqueId())))
      {
         mRingPropertiesPtr->SetEntity(nullptr, RingPropertiesWidget::RingFlags(false, true));
         mRingPropertiesPtr->hide();
      }
   }
}

// Instead of upcasting/downcasting when necessary, just remove/re-add the entity when toggling between the different classes
void Annotation::AnnotationDisplayInterface::ToggleBullseye(unsigned int aId, bool aBullseye)
{
   wkf::PointOfInterest* poi = dynamic_cast<wkf::PointOfInterest*>(vaEnv.FindEntity(aId));
   if (poi)
   {
      // Copy attachment properties before they are removed
      std::string                                       name       = poi->GetName();
      bool                                              exportable = poi->GetExportable();
      vespa::VaPosition                                 pos        = poi->GetPosition();
      std::vector<wkf::AttachmentRangeRing::Properties> rrProperties;
      auto                                              rrList = poi->FindAttachmentsOfType<wkf::AttachmentRangeRing>();
      for (const auto& rrIt : rrList)
      {
         rrProperties.push_back(rrIt->GetProperties());
      }

      QString textDec;
      QString imageDec;
      auto    decPtr = poi->FindFirstAttachmentOfType<Annotation::AnnotationDecorator>();
      if (decPtr)
      {
         textDec  = decPtr->GetText();
         imageDec = decPtr->GetImageDescription();
      }

      auto reAddRangeRings = [&](wkf::PointOfInterest* aPoi)
      {
         for (const auto& it : rrProperties)
         {
            AddRangeRing(aPoi, it);
         }
      };

      auto reAddDecoration = [&](wkf::PointOfInterest* aPoi)
      {
         if (!textDec.isEmpty() || !imageDec.isEmpty())
         {
            AddDecoration(*aPoi, textDec, imageDec);
         }
      };

      if (poi->IsA_TypeOf<wkf::Bullseye>() && !aBullseye)
      {
         vaEnv.DeleteEntity(poi);
         wkf::PointOfInterest* newPoi =
            AddDetailedPOI<wkf::PointOfInterest>("pointOfInterest", pos.GetLat(), pos.GetLon(), pos.GetAlt(), name, exportable);
         reAddRangeRings(newPoi);
         reAddDecoration(newPoi);
      }
      else if (!poi->IsA_TypeOf<wkf::Bullseye>() && aBullseye)
      {
         vaEnv.DeleteEntity(poi);
         wkf::Bullseye* newPoi =
            AddDetailedPOI<wkf::Bullseye>("bullseye", pos.GetLat(), pos.GetLon(), pos.GetAlt(), name, exportable);
         reAddRangeRings(newPoi);
         reAddDecoration(newPoi);
      }
   }
}


void Annotation::AnnotationDisplayInterface::ClearAnnotations()
{
   vespa::VaEntityList list;
   vaEnv.GetEntityListOfType<wkf::PointOfInterest>(list);

   for (auto&& it : list)
   {
      auto* poi = dynamic_cast<wkf::PointOfInterest*>(it);
      if (poi)
      {
         if (poi->GetManagingAttachment() == nullptr)
         {
            vaEnv.GetStandardScenario()->DeleteEntity(poi);
            // delete it!
         }
      }
   }
   list.clear();
   vaEnv.GetEntityList(list);
   for (auto&& it : list)
   {
      vespa::VaEntity* entity = it;
      if (entity)
      {
         // cannot safely remove attachments when iterating the removal list reference
         std::vector<unsigned int> removalList;
         const auto&               al = entity->GetAttachments();
         for (auto&& ait : al)
         {
            if (ait->IsA_TypeOf<wkf::AttachmentRangeRing>())
            {
               removalList.push_back(ait->GetUniqueId());
            }
            else if (ait->IsA_TypeOf<AnnotationDecorator>())
            {
               Decorate(QString::fromStdString(entity->GetName()), GetDecorationIcon(""), "");
               // delete it
            }
            // Add ruler deletion code here if we decide to manage rulers here
         }
         for (const auto& removalIt : removalList)
         {
            entity->RemoveAttachment(removalIt);
         }
      }
   }
   mDeferredCustomVectorMap.clear();
   mDeferredDecorationMap.clear();
   mDeferredRangeRingMap.clear();
   mDeletedCustomVectorMap.clear();
   mDeletedPlatformDecorationMap.clear();
   mDeletedPlatformRangeRingMap.clear();
}

void Annotation::AnnotationDisplayInterface::ExportAnnotations()
{
   QString file = wkf::getSaveFileName(wkfEnv.GetMainWindow(), "Export Annotations To Text File", QString(), "*.txt");
   if (!file.isEmpty())
   {
      QFile exportFile(file);
      // overwrite data if file already exists
      if (exportFile.open(QIODevice::WriteOnly))
      {
         std::string         exportString = "visual_elements\n";
         vespa::VaEntityList list;
         vaEnv.GetEntityListOfType<wkf::PointOfInterest>(list);
         for (auto&& it : list)
         {
            exportString += GetPoiExport(it);
         }
         list.clear();
         vaEnv.GetEntityList(list);
         for (auto&& it : list)
         {
            vespa::VaEntity* entity = it;
            if (entity)
            {
               // cannot safely remove attachments when iterating the removal list reference
               const auto& al = entity->GetAttachments();
               for (auto&& ait : al)
               {
                  exportString += GetVerifiedRangeRingExport(entity, ait.get());
                  exportString += GetVerifiedDecorationExport(entity, ait.get());
               }
            }
         }
         // Add annotations on platforms that were deleted
         for (const auto& deletedRRs : mDeletedPlatformRangeRingMap)
         {
            for (const auto& propIt : deletedRRs.second)
            {
               exportString += GetRangeRingExport(EntityType::cPLATFORM, deletedRRs.first.toStdString(), propIt);
            }
         }
         for (const auto& deletedDecIt : mDeletedPlatformDecorationMap)
         {
            exportString +=
               GetDecorationExport(deletedDecIt.first.toStdString(), deletedDecIt.second.first, deletedDecIt.second.second);
         }
         exportString += "end_visual_elements\n";

         QTextStream output(&exportFile);
         output << QString::fromStdString(exportString);
         exportFile.close();
      }
   }
}

void Annotation::AnnotationDisplayInterface::MoveSelectionAndAttachmentComplete(const wkf::PlatformList& aPlatformList,
                                                                                const wkf::EntityList& aNonPlatformList,
                                                                                const vespa::VaAttachmentList& aAttachmentList)
{
   // If a poi/bullseye is dragged, it needs to become exportable.
   for (auto& it : aNonPlatformList)
   {
      wkf::PointOfInterest* poi = dynamic_cast<wkf::PointOfInterest*>(it);
      if (poi)
      {
         poi->SetExportable(true);
         wkf::AttachmentRangeRing* rr = poi->FindFirstAttachmentOfType<wkf::AttachmentRangeRing>();
         if (rr)
         {
            auto properties        = rr->GetProperties();
            properties.mExportable = true;
            rr->SetProperties(properties);
         }
      }
   }
}

void Annotation::AnnotationDisplayInterface::AddDecoration(wkf::Entity& aEntity, const QString& aText, const QString& aImageDesc)
{
   auto DecorateHelper = [&](wkf::Entity& aEnt, const QIcon& aIcon, const QString& aImageDesc)
   {
      aEnt.SetDecoration(aIcon, aImageDesc);
      auto* d = aEnt.FindFirstAttachmentOfType<AnnotationDecorator>();
      if (d)
      {
         d->SetExportable(true);
      }
   };

   if (!aText.isEmpty())
   {
      DecorateWithText(aEntity, aText);
   }
   else if (!aImageDesc.isEmpty())
   {
      if (cDECORATOR_PRESET_TO_PATH.find(aImageDesc.toStdString()) != cDECORATOR_PRESET_TO_PATH.end())
      {
         QString imageDesc = cDECORATOR_PRESET_TO_PATH.at(aImageDesc.toStdString());
         DecorateHelper(aEntity, GetDecorationIcon(imageDesc), imageDesc);
      }
      else
      {
         QPixmap icon(aImageDesc);
         if (!icon.isNull())
         {
            DecorateHelper(aEntity, QIcon(icon), aImageDesc);
         }
      }
   }
}

QIcon Annotation::AnnotationDisplayInterface::GetDecorationIcon(const QString& aPath) const
{
   if (cDECORATOR_PATH_TO_PRESET.find(aPath) != cDECORATOR_PATH_TO_PRESET.end())
   {
      return QIcon(aPath);
   }
   return QIcon();
}

void Annotation::AnnotationDisplayInterface::AddDeferredRangeRing(const QString& aPlatformName,
                                                                  const wkf::AttachmentRangeRing::Properties& aData)
{
   mDeferredRangeRingMap[aPlatformName].push_back(aData);
   if (mEntityAddedUID < 0)
   {
      mEntityAddedUID =
         mCallbacks.Add(wkf::Observer::PlatformsAdded.Connect(&AnnotationDisplayInterface::PlatformAddedCB, this));
   }
}

// TODO: Is it necessary to include entities too? The only deferred addition would be a user manually adding a POI after
// the scenario starts running
void Annotation::AnnotationDisplayInterface::AddDeferredDecoration(const QString&                     aPlatformName,
                                                                   const std::pair<QString, QString>& aDesc)
{
   mDeferredDecorationMap[aPlatformName] = aDesc;
   if (mEntityAddedUID < 0)
   {
      mEntityAddedUID =
         mCallbacks.Add(wkf::Observer::PlatformsAdded.Connect(&AnnotationDisplayInterface::PlatformAddedCB, this));
   }
}

QString Annotation::AnnotationDisplayInterface::DecorationPathToPreset(const QString& aPath)
{
   if (cDECORATOR_PATH_TO_PRESET.count(aPath) > 0)
   {
      return cDECORATOR_PATH_TO_PRESET[aPath];
   }
   return "";
}

QString Annotation::AnnotationDisplayInterface::DecorationPresetToPath(const std::string& aPreset)
{
   if (cDECORATOR_PRESET_TO_PATH.count(aPreset))
   {
      return cDECORATOR_PRESET_TO_PATH.at(aPreset);
   }
   return "";
}

void Annotation::AnnotationDisplayInterface::AddRangeRing(wkf::Platform&                              aPlatform,
                                                          const wkf::AttachmentRangeRing::Properties& aProperties)
{
   std::string name = aPlatform.GetName() + "_RangeRing";
   name             = GetRingPropertiesWidget()->CheckDuplicateName(name);
   auto rangeRing   = vespa::make_attachment<wkf::AttachmentRangeRing>(aPlatform, vaEnv.GetStandardViewer(), name);
   rangeRing->SetProperties(aProperties);
}

void Annotation::AnnotationDisplayInterface::AddRangeRing(const double&                               aLat,
                                                          const double&                               aLon,
                                                          const wkf::AttachmentRangeRing::Properties& aProperties)
{
   vespa::VaViewer* viewer   = vaEnv.GetStandardViewer();
   wkf::Scenario*   scenario = vaEnv.GetStandardScenario();
   if (viewer && scenario)
   {
      wkf::PointOfInterest* entity = vaEnv.CreateEntity<wkf::PointOfInterest>("pointOfInterest");
      entity->SetPosition(vespa::VaPosition(aLat, aLon, 0.0));
      entity->SetExportable(aProperties.mExportable);
      scenario->AddEntity(entity);
      entity->Initialize();
      entity->Load();
      mCallbacks.Add(wkf::Observer::EntityMoved.Connect(&AnnotationDisplayInterface::PointOfInterestMovedCB,
                                                        this,
                                                        entity->GetUniqueId()));
      mCallbacks.Add(wkf::Observer::EntityDeleted.Connect(&AnnotationDisplayInterface::PointOfInterestDeletedCB,
                                                          this,
                                                          entity->GetUniqueId()));

      AddRangeRing(entity, aProperties);
   }
}

void Annotation::AnnotationDisplayInterface::AddRangeRing(wkf::PointOfInterest*                       aPoi,
                                                          const wkf::AttachmentRangeRing::Properties& aProperties)
{
   std::string name = aPoi->GetName() + "_RangeRing";
   name             = GetRingPropertiesWidget()->CheckDuplicateName(name);
   auto rangeRing   = vespa::make_attachment<wkf::AttachmentRangeRing>(*aPoi, vaEnv.GetStandardViewer(), name);
   rangeRing->SetProperties(aProperties);
}


// Small Widget for image selection
Annotation::ImageDialog::ImageDialog(QWidget* parent)
   : QDialog(parent)
   , mImageSelectionWidget(new wkf::ImageSelectionWidget(this))
   , mDialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this))
{
   QVBoxLayout* layout = new QVBoxLayout(this);
   layout->addWidget(mImageSelectionWidget, Qt::AlignRight);
   layout->addWidget(mDialogButtonBox, Qt::AlignRight);
   layout->addSpacing(1);
   layout->insertStretch(2, 100);
   setLayout(layout);
   setMinimumWidth(minimumSizeHint().width() * 2);
}

void Annotation::ImageDialog::accept()
{
   QString path = mImageSelectionWidget->GetText();
   QPixmap icon(path);
   if (!icon.isNull())
   {
      QDialog::accept();
   }
   else
   {
      QMessageBox::warning(this, "Image Not Found", "No image was found at \"" + path + ".\"");
   }
}
