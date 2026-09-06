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
#ifndef RINGPROPERTIESWIDGET_HPP
#define RINGPROPERTIESWIDGET_HPP

#include <memory>
#include <unordered_set>

#include <QWidget>

#include "WkfAttachmentRangeRing.hpp"
#include "ui_WkfRingProperties.h"

namespace vespa
{
class VaEntity;
}

namespace Annotation
{
class WKF_COMMON_EXPORT RingPropertiesWidget : public QWidget
{
   Q_OBJECT
public:
   RingPropertiesWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   ~RingPropertiesWidget() override = default;

   struct RingFlags
   {
      RingFlags(bool aNew, bool aEntityContext)
         : mNew(aNew)
         , mEntityContext(aEntityContext)
      {
      }
      bool mNew{false};
      bool mEntityContext{true};
   };

   virtual void                    SetEntity(vespa::VaEntity* aEntityPtr, const RingFlags& aFlags);
   const vespa::VaEntity*          GetEntity() const { return mEntityPtr; }
   const wkf::AttachmentRangeRing* GetRing() const { return mRingPtr; }
   std::string                     CheckDuplicateName(const std::string& aName) const;
   virtual void OverwriteOriginalValues(const wkf::AttachmentRangeRing::Properties& aInfo) { mOldRing = aInfo; }

signals:
   void RingAssigned(wkf::Platform* aPlatform);

protected:
   void                                    closeEvent(QCloseEvent* aEvent) override;
   void                                    hideEvent(QHideEvent* aEvent) override;
   void                                    RingAssignedToPlatform();
   virtual std::unordered_set<std::string> GetRingList() const;

   virtual void SetCenterRadius();
   virtual void SetAdditionalRadii();
   virtual void SetNumberAdditonalRings(const int aValue);
   virtual void SetCenterAngle();
   virtual void SetAngleWidth();
   virtual void SetRadialRate();
   virtual void RingColorUpdated(const QColor& aColor);
   virtual void RadialColorUpdated(const QColor& aColor);
   virtual void SetRingLineWidth(const int aValue);
   virtual void SetRadialLineWidth(const int aValue);
   virtual void RangeLabelsToggled(bool aState);
   virtual void AngleLabelsToggled(bool aState);
   virtual void RangeLabelColorUpdated(const QColor& aColor);
   virtual void AngleLabelColorUpdated(const QColor& aColor);
   virtual void SetAlignWithPlatform(bool aState);
   virtual void NameChanged(const QString& aName);
   virtual void SelectionChanged(int aIndex);

   Ui::RingProperties        mUi;
   vespa::VaEntity*          mEntityPtr;
   RingFlags                 mCurrentFlags;
   wkf::AttachmentRangeRing* mRingPtr;
   int                       mActiveIndex;

private:
   void SetExportable();
   void AttachmentDeletedCB(vespa::VaAttachment* aAttachmentPtr, const vespa::VaAttachmentSubId& aSubpartId);
   void UpdateWidget();
   void BlockRingParameterSignals(bool aState);
   void SetPosition();

   void         AcceptChanges();
   virtual void AcceptChangesP();
   void         CancelChanges();

   wkf::AttachmentRangeRing::Properties mOldRing;
   vespa::VaCallbackHolder              mCallbacks;
   UtCallbackHolder                     mUtCallbacks;
   bool                                 mAcceptChanges{false};
};
} // namespace Annotation
#endif
