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
#include "WkfRingPropertiesWidget.hpp"

#include <unordered_set>

#include <QStandardItemModel>

#include "UtCast.hpp"
#include "VaEntity.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfVtkEnvironment.hpp"

Annotation::RingPropertiesWidget::RingPropertiesWidget(QWidget*        parent /*= nullptr*/,
                                                       Qt::WindowFlags f /*= Qt::WindowFlags()*/)
   : QWidget(parent, f | Qt::Tool | Qt::MSWindowsFixedSizeDialogHint)
   , mEntityPtr(nullptr)
   , mCurrentFlags(false, true)
   , mRingPtr(nullptr)
   , mActiveIndex(-1)
{
   mUi.setupUi(this);
   setEnabled(false);
   mUi.mCenterRadius->SetBounds(1.0, 1000000.0);
   mUi.mAdditionalRadii->SetBounds(1.0, 500000.0);
   mUi.mCenterAngle->SetBounds(0.0, UtMath::cTWO_PI);
   mUi.mAngleWidth->SetBounds(0.0, UtMath::cTWO_PI);
   mUi.mRadialRate->SetBounds(0.0, UtMath::cTWO_PI);

   mCallbacks.Add(wkf::Observer::AttachmentDeleted.Connect(&RingPropertiesWidget::AttachmentDeletedCB, this));

   mUtCallbacks += mUi.mLatEdit->ValueChanged.Connect(&RingPropertiesWidget::SetPosition, this);
   mUtCallbacks += mUi.mLonEdit->ValueChanged.Connect(&RingPropertiesWidget::SetPosition, this);
   connect(mUi.mName, &QComboBox::editTextChanged, this, &RingPropertiesWidget::NameChanged);
   connect(mUi.mName, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RingPropertiesWidget::SelectionChanged);
   connect(mUi.mCenterRadius, &UtQtLengthValueEdit::ValueChanged, this, &RingPropertiesWidget::SetCenterRadius);
   connect(mUi.mAdditionalRadii, &UtQtLengthValueEdit::ValueChanged, this, &RingPropertiesWidget::SetAdditionalRadii);
   connect(mUi.mAdditionalRings,
           QOverload<int>::of(&QSpinBox::valueChanged),
           this,
           &RingPropertiesWidget::SetNumberAdditonalRings);
   connect(mUi.mCenterAngle, &UtQtAngleValueEdit::ValueChanged, this, &RingPropertiesWidget::SetCenterAngle);
   connect(mUi.mAngleWidth, &UtQtAngleValueEdit::ValueChanged, this, &RingPropertiesWidget::SetAngleWidth);
   connect(mUi.mRadialRate, &UtQtAngleValueEdit::ValueChanged, this, &RingPropertiesWidget::SetRadialRate);
   connect(mUi.mRingColor, &UtQtColorButton::ColorChanged, this, &RingPropertiesWidget::RingColorUpdated);
   connect(mUi.mRadialColor, &UtQtColorButton::ColorChanged, this, &RingPropertiesWidget::RadialColorUpdated);
   connect(mUi.mRingLineWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &RingPropertiesWidget::SetRingLineWidth);
   connect(mUi.mRadialLineWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &RingPropertiesWidget::SetRadialLineWidth);
   connect(mUi.mRangeLabels, &QCheckBox::toggled, this, &RingPropertiesWidget::RangeLabelsToggled);
   connect(mUi.mAlignWithPlatformCheckBox, &QCheckBox::toggled, this, &RingPropertiesWidget::SetAlignWithPlatform);
   connect(mUi.mAngleLabels, &QCheckBox::toggled, this, &RingPropertiesWidget::AngleLabelsToggled);
   connect(mUi.mRangeLabelColor, &UtQtColorButton::ColorChanged, this, &RingPropertiesWidget::RangeLabelColorUpdated);
   connect(mUi.mAngleLabelColor, &UtQtColorButton::ColorChanged, this, &RingPropertiesWidget::AngleLabelColorUpdated);

   connect(mUi.mButtonBox->button(QDialogButtonBox::Ok),
           &QPushButton::clicked,
           this,
           [&]()
           {
              if (mUi.mCenterAngle->IsValid() && mUi.mCenterRadius->IsValid() && mUi.mAdditionalRadii->IsValid() &&
                  mUi.mAngleWidth->IsValid() && mUi.mRadialRate->IsValid())
              {
                 mAcceptChanges = true;
                 close();
              }
              else
              {
                 QMessageBox::warning(
                    this,
                    "Invalid Input",
                    "Please make sure the range ring inputs are valid before finalizing any changes.");
              }
           });
   connect(mUi.mButtonBox->button(QDialogButtonBox::Cancel),
           &QPushButton::clicked,
           this,
           [&]()
           {
              mAcceptChanges = false;
              close();
           });
   connect(mUi.mLocationSelector,
           &wkf::LocationSelector::Selected,
           this,
           [this](double aLat, double aLon)
           {
              mUi.mLatEdit->SetValue(aLat);
              mUi.mLonEdit->SetValue(aLon);
           });

   mUi.mCenterRadius->SetBounds(1.0, 3000000.0);
   mUi.mAdditionalRadii->SetBounds(1.0, 500000.0);
   QRegExp validInput("([^/#\\s]|/[^*#\\s])*");
   mUi.mName->setValidator(new QRegExpValidator(validInput, mUi.mName));
}

void Annotation::RingPropertiesWidget::SetEntity(vespa::VaEntity* aEntityPtr, const RingFlags& aFlags)
{
   // Position selection should only be allowed when a range ring is being created via the "Add At Location" option
   mUi.mLatEdit->setVisible(aFlags.mNew && !aFlags.mEntityContext);
   mUi.mLonEdit->setVisible(aFlags.mNew && !aFlags.mEntityContext);
   mUi.mLatLabel->setVisible(aFlags.mNew && !aFlags.mEntityContext);
   mUi.mLonLabel->setVisible(aFlags.mNew && !aFlags.mEntityContext);
   mUi.mLocationSelector->setVisible(aFlags.mNew && !aFlags.mEntityContext);
   QSize size(width(), sizeHint().height());
   resize(size);

   if (!mAcceptChanges)
   {
      CancelChanges();
   }
   // old position does not need to be saved because position is only editable in the viewer context
   mAcceptChanges = false;
   if (mRingPtr)
   {
      mOldRing = mRingPtr->GetProperties();
   }

   mEntityPtr = aEntityPtr;
   if (!mEntityPtr)
   {
      mRingPtr = nullptr;
   }
   else
   {
      BlockRingParameterSignals(true);
      std::vector<wkf::AttachmentRangeRing*> atts = mEntityPtr->FindAttachmentsOfType<wkf::AttachmentRangeRing>();
      mRingPtr                                    = nullptr;
      mUi.mName->clear();
      int          idx    = 0;
      int          selidx = 0;
      unsigned int maxuid = 0;
      for (auto* it : atts)
      {
         mUi.mName->addItem(QString::fromStdString(it->GetName()), it->GetUniqueId());
         if (it->GetUniqueId() > maxuid)
         {
            maxuid   = it->GetUniqueId();
            mRingPtr = it;
            selidx   = idx;
         }
         idx++;
      }
      if (mRingPtr)
      {
         mUi.mName->setCurrentIndex(selidx);
         mActiveIndex = selidx;
         if (aFlags.mNew) // if it is a new ring, and there was an old ring, inherit (except for the name)
         {
            wkf::AttachmentRangeRing::Properties prop = mOldRing;
            if (!mRingPtr->GetName().empty())
            {
               prop.mName = QString::fromStdString(mRingPtr->GetName());
            }
            mRingPtr->SetProperties(prop);
         }

         RingAssignedToPlatform();
         // The "old" ring pointer should become the current one
         mOldRing = mRingPtr->GetProperties();

         // Populates the Ring Properties Widget Dialog with current or default values.
         mUi.mLatEdit->SetValue(mEntityPtr->GetPosition().GetLat());
         mUi.mLonEdit->SetValue(mEntityPtr->GetPosition().GetLon());

         UtLengthValue currentCenterRadius = mRingPtr->GetCenterRadius();
         currentCenterRadius.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit());
         mUi.mCenterRadius->SetValue(currentCenterRadius);

         mUi.mAdditionalRings->setValue(mRingPtr->GetNumberAdditionalRings());
         mUi.mAdditionalRadii->setEnabled(mRingPtr->GetNumberAdditionalRings() > 0);

         UtLengthValue currentAdditionalRadii = mRingPtr->GetAdditionalRadii();
         currentAdditionalRadii.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLengthUnit());
         mUi.mAdditionalRadii->SetValue(currentAdditionalRadii);

         UtAngleValue currentCenterAngle = mRingPtr->GetCenterAngle();
         currentCenterAngle.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit());
         mUi.mCenterAngle->SetValue(currentCenterAngle);

         UtAngleValue currentAngleWidth = mRingPtr->GetAngleWidth();
         currentAngleWidth.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit());
         mUi.mAngleWidth->SetValue(currentAngleWidth);

         UtAngleValue currentRadialRate = mRingPtr->GetRadialRate();
         currentRadialRate.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit());
         mUi.mRadialRate->SetValue(currentRadialRate);

         mUi.mRingColor->SetColor(mRingPtr->GetRingColor());
         mUi.mRadialColor->SetColor(mRingPtr->GetRadialColor());
         mUi.mRingLineWidth->setValue(mRingPtr->GetRingLineWidth());
         mUi.mRadialLineWidth->setValue(mRingPtr->GetRadialLineWidth());
         mUi.mRangeLabels->setChecked(mRingPtr->GetRangeLabelsEnabled());
         mUi.mAngleLabels->setChecked(mRingPtr->GetAngleLabelsEnabled());

         mUi.mRangeLabelColor->setEnabled(mRingPtr->GetRangeLabelsEnabled());
         mUi.mRangeLabelColor->SetColor(mRingPtr->GetRangeLabelColor());

         mUi.mAlignWithPlatformCheckBox->setVisible(dynamic_cast<wkf::Platform*>(&mRingPtr->GetParent()) != nullptr);
         mUi.mAlignWithPlatformCheckBox->setChecked(mRingPtr->GetAlignWithPlatformHeading());

         mUi.mAngleLabelColor->setEnabled(mRingPtr->GetAngleLabelsEnabled());
         mUi.mAngleLabelColor->SetColor(mRingPtr->GetAngleLabelColor());
      }
      BlockRingParameterSignals(false);
   }

   // When creating a new range ring, disable the ability to switch range rings from the dialog
   const QStandardItemModel* model      = qobject_cast<const QStandardItemModel*>(mUi.mName->model());
   int                       entryCount = mUi.mName->count();
   for (int i = 1; i < entryCount; ++i)
   {
      QStandardItem* item = model->item(i);
      item->setEnabled(!aFlags.mNew);
   }
   mCurrentFlags = aFlags;

   UpdateWidget();
}

std::string Annotation::RingPropertiesWidget::CheckDuplicateName(const std::string& aName) const
{
   // Make sure there are no duplicate range ring names
   auto        ringList = GetRingList();
   std::string retVal   = aName;
   size_t      i        = 1;
   while (ringList.count(retVal) > 0)
   {
      retVal = aName + "(" + std::to_string(i++) + ")";
   }
   return retVal;
}

void Annotation::RingPropertiesWidget::closeEvent(QCloseEvent* aEvent)
{
   if (mAcceptChanges)
   {
      AcceptChanges();
   }
   else
   {
      CancelChanges();
   }
   QWidget::closeEvent(aEvent);
}

void Annotation::RingPropertiesWidget::hideEvent(QHideEvent* aEvent)
{
   if (!mAcceptChanges)
   {
      CancelChanges();
   }
   QWidget::hideEvent(aEvent);
}

void Annotation::RingPropertiesWidget::RingAssignedToPlatform()
{
   wkf::Platform* plat = dynamic_cast<wkf::Platform*>(mEntityPtr);
   if (plat)
   {
      emit RingAssigned(plat);
   }
}

std::unordered_set<std::string> Annotation::RingPropertiesWidget::GetRingList() const
{
   vespa::VaEntityList entList;
   vaEnv.GetEntityList(entList);
   std::unordered_set<std::string> ringList;
   for (const auto& entIt : entList)
   {
      auto attachList = entIt->FindAttachmentsOfType<wkf::AttachmentRangeRing>();
      for (const auto& attIt : attachList)
      {
         if (mRingPtr == nullptr || (attIt->GetUniqueId() != mRingPtr->GetUniqueId()))
         {
            ringList.emplace(attIt->GetName());
         }
      }
   }
   return ringList;
}

void Annotation::RingPropertiesWidget::AcceptChanges()
{
   std::string name     = mUi.mName->currentText().toStdString();
   std::string dupeName = CheckDuplicateName(name);
   if (mRingPtr->GetName() != name && dupeName != name)
   {
      QMessageBox::warning(nullptr,
                           "Name Exists",
                           "A range ring with the name " + QString::fromStdString(name) +
                              " already exists. The name has been changed to " + QString::fromStdString(dupeName) + ".");
      mRingPtr->SetName(dupeName);
      mUi.mName->setItemText(mUi.mName->currentIndex(), QString::fromStdString(dupeName));
   }
   AcceptChangesP();
}

void Annotation::RingPropertiesWidget::AcceptChangesP()
{
   SetExportable();
}

void Annotation::RingPropertiesWidget::SetExportable()
{
   wkf::AttachmentRangeRing::Properties properties = mRingPtr->GetProperties();
   properties.mExportable                          = true;
   mRingPtr->SetProperties(properties);
}

void Annotation::RingPropertiesWidget::AttachmentDeletedCB(vespa::VaAttachment*            aAttachmentPtr,
                                                           const vespa::VaAttachmentSubId& aSubpartId)
{
   if (mEntityPtr)
   {
      int index = mUi.mName->findData(aAttachmentPtr->GetUniqueId());
      if (index != -1)
      {
         mUi.mName->removeItem(index);
      }
   }
}

void Annotation::RingPropertiesWidget::UpdateWidget()
{
   setEnabled(mEntityPtr != nullptr);
}

void Annotation::RingPropertiesWidget::BlockRingParameterSignals(bool aState)
{
   mUi.mName->blockSignals(aState);
   mUi.mLatEdit->blockSignals(aState);
   mUi.mLonEdit->blockSignals(aState);
   mUi.mCenterRadius->blockSignals(aState);
   mUi.mAdditionalRings->blockSignals(aState);
   mUi.mAdditionalRadii->blockSignals(aState);
   mUi.mCenterAngle->blockSignals(aState);
   mUi.mAngleWidth->blockSignals(aState);
   mUi.mRadialRate->blockSignals(aState);
   mUi.mRingColor->blockSignals(aState);
   mUi.mRadialColor->blockSignals(aState);
   mUi.mRingLineWidth->blockSignals(aState);
   mUi.mRadialLineWidth->blockSignals(aState);
   mUi.mRangeLabels->blockSignals(aState);
   mUi.mAlignWithPlatformCheckBox->blockSignals(aState);
   mUi.mAngleLabels->blockSignals(aState);
   mUi.mRangeLabelColor->blockSignals(aState);
   mUi.mAngleLabelColor->blockSignals(aState);
}

void Annotation::RingPropertiesWidget::SetPosition()
{
   if (mUi.mLatEdit->IsValid() && mUi.mLonEdit->IsValid())
   {
      if (mEntityPtr)
      {
         vespa::VaPosition pos = mEntityPtr->GetPosition();
         pos.SetLat(mUi.mLatEdit->GetValue());
         pos.SetLon(mUi.mLonEdit->GetValue());
         mEntityPtr->SetPosition(pos);
      }
   }
}

void Annotation::RingPropertiesWidget::SetCenterRadius()
{
   if (mUi.mCenterRadius->IsValid())
   {
      if (mRingPtr)
      {
         mRingPtr->SetCenterRadius(mUi.mCenterRadius->GetValue());
      }
   }
}

void Annotation::RingPropertiesWidget::SetAdditionalRadii()
{
   if (mUi.mAdditionalRadii->IsValid())
   {
      if (mRingPtr)
      {
         mRingPtr->SetAdditionalRings(mUi.mAdditionalRings->value(), mUi.mAdditionalRadii->GetValue());
      }
   }
}

void Annotation::RingPropertiesWidget::SetNumberAdditonalRings(const int aValue)
{
   if (aValue == 0)
   {
      mUi.mAdditionalRadii->setEnabled(false);
   }
   else
   {
      mUi.mAdditionalRadii->setEnabled(true);
   }
   if (mRingPtr)
   {
      mRingPtr->SetAdditionalRings(mUi.mAdditionalRings->value(), mUi.mAdditionalRadii->GetValue());
   }
}

void Annotation::RingPropertiesWidget::SetCenterAngle()
{
   if (mUi.mCenterAngle->IsValid())
   {
      if (mRingPtr)
      {
         mRingPtr->SetCenterAngle(mUi.mCenterAngle->GetValue());
      }
   }
}

void Annotation::RingPropertiesWidget::SetAngleWidth()
{
   if (mUi.mAngleWidth->IsValid())
   {
      if (mRingPtr)
      {
         mRingPtr->SetAngleWidth(mUi.mAngleWidth->GetValue());
      }
   }
}

void Annotation::RingPropertiesWidget::SetRadialRate()
{
   if (mUi.mRadialRate->IsValid())
   {
      if (mRingPtr)
      {
         mRingPtr->SetRadialRate(mUi.mRadialRate->GetValue());
      }
   }
}

void Annotation::RingPropertiesWidget::SetAlignWithPlatform(bool aState)
{
   if (mRingPtr)
   {
      mRingPtr->SetAlignWithPlatformHeading(aState);
   }
}

void Annotation::RingPropertiesWidget::RingColorUpdated(const QColor& aColor)
{
   if (mRingPtr)
   {
      mRingPtr->SetRingColor(aColor);
   }
}

void Annotation::RingPropertiesWidget::RadialColorUpdated(const QColor& aColor)
{
   if (mRingPtr)
   {
      mRingPtr->SetRadialColor(aColor);
   }
}

void Annotation::RingPropertiesWidget::SetRingLineWidth(const int aValue)
{
   if (mRingPtr)
   {
      mRingPtr->SetRingLineWidth(aValue);
   }
}

void Annotation::RingPropertiesWidget::SetRadialLineWidth(const int aValue)
{
   if (mRingPtr)
   {
      mRingPtr->SetRadialLineWidth(aValue);
   }
}

void Annotation::RingPropertiesWidget::RangeLabelsToggled(bool aState)
{
   if (mRingPtr)
   {
      mRingPtr->SetRangeLabelsEnabled(aState);
      mUi.mRangeLabelColor->setEnabled(aState);
   }
}

void Annotation::RingPropertiesWidget::AngleLabelsToggled(bool aState)
{
   if (mRingPtr)
   {
      mRingPtr->SetAngleLabelsEnabled(aState);
      mUi.mAngleLabelColor->setEnabled(aState);
   }
}

void Annotation::RingPropertiesWidget::RangeLabelColorUpdated(const QColor& aColor)
{
   if (mRingPtr)
   {
      mRingPtr->SetRangeLabelColor(aColor);
   }
}

void Annotation::RingPropertiesWidget::AngleLabelColorUpdated(const QColor& aColor)
{
   if (mRingPtr)
   {
      mRingPtr->SetAngleLabelColor(aColor);
   }
}

void Annotation::RingPropertiesWidget::NameChanged(const QString& aName)
{
   if (mActiveIndex == mUi.mName->currentIndex()) // if this is a real text change, and not a selection change
   {
      mUi.mName->setItemText(mUi.mName->currentIndex(), aName);
      mRingPtr->SetName(aName.toStdString());
   }
}

void Annotation::RingPropertiesWidget::SelectionChanged(int aIndex)
{
   mActiveIndex = mUi.mName->currentIndex();
   int uid      = mUi.mName->currentData().toInt();
   if (uid != ut::safe_cast<int>(mRingPtr->GetUniqueId()))
   {
      auto* rr = dynamic_cast<wkf::AttachmentRangeRing*>(mEntityPtr->FindAttachment(uid));
      if (rr)
      {
         CancelChanges();
         mRingPtr = rr;
         mOldRing = mRingPtr->GetProperties();
         RingAssignedToPlatform();

         BlockRingParameterSignals(true);
         mUi.mCenterRadius->SetValue(mRingPtr->GetCenterRadius());
         mUi.mAdditionalRings->setValue(mRingPtr->GetNumberAdditionalRings());
         if (mRingPtr->GetNumberAdditionalRings() > 0)
         {
            mUi.mAdditionalRadii->setEnabled(true);
         }
         else
         {
            mUi.mAdditionalRadii->setEnabled(false);
         }
         mUi.mAdditionalRadii->SetValue(mRingPtr->GetAdditionalRadii());
         mUi.mCenterAngle->SetValue(UtAngleValue(mRingPtr->GetCenterAngle() * UtMath::cDEG_PER_RAD, UtUnitAngle::cDEGREES));
         mUi.mAngleWidth->SetValue(mRingPtr->GetAngleWidth());
         mUi.mRadialRate->SetValue(mRingPtr->GetRadialRate());
         mUi.mRingColor->SetColor(mRingPtr->GetRingColor());
         mUi.mRadialColor->SetColor(mRingPtr->GetRadialColor());
         mUi.mRingLineWidth->setValue(mRingPtr->GetRingLineWidth());
         mUi.mRadialLineWidth->setValue(mRingPtr->GetRadialLineWidth());
         mUi.mRangeLabels->setChecked(mRingPtr->GetRangeLabelsEnabled());
         mUi.mAngleLabels->setChecked(mRingPtr->GetAngleLabelsEnabled());
         mUi.mAlignWithPlatformCheckBox->setChecked(mRingPtr->GetAlignWithPlatformHeading());
         mUi.mRangeLabelColor->setEnabled(mRingPtr->GetRangeLabelsEnabled());
         mUi.mRangeLabelColor->SetColor(mRingPtr->GetRangeLabelColor());
         mUi.mAngleLabelColor->setEnabled(mRingPtr->GetAngleLabelsEnabled());
         mUi.mAngleLabelColor->SetColor(mRingPtr->GetAngleLabelColor());
         // only possible to get here via a dialog generated by the entity context menu, and the ring must already exist
         mCurrentFlags = RingFlags(false, true);
         BlockRingParameterSignals(false);
      }
   }
}

void Annotation::RingPropertiesWidget::CancelChanges()
{
   // removing the attachment resets the entity to null via callback, so check this first (the attachment should be
   // removed automatically)
   if (mEntityPtr)
   {
      if (!mCurrentFlags.mEntityContext)
      {
         vaEnv.DeleteEntity(mEntityPtr);
         mEntityPtr = nullptr;
      }
      else if (mRingPtr && mCurrentFlags.mNew)
      {
         mEntityPtr->RemoveAttachment(mRingPtr->GetUniqueId());
         mRingPtr = nullptr;
      }
      else if (mRingPtr)
      {
         mRingPtr->SetProperties(mOldRing);
      }
   }
}
