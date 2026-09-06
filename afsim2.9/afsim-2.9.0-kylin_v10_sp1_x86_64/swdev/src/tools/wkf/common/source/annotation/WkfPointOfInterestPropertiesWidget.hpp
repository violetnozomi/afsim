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
#ifndef POINTOFINTERESTPROPERTIESWIDGET_HPP
#define POINTOFINTERESTPROPERTIESWIDGET_HPP

#include <memory>

#include <QWidget>

#include "WkfAnnotationDecorator.hpp"
#include "ui_WkfPointOfInterestProperties.h"

namespace vespa
{
class VaEntity;
}

namespace Annotation
{
class WKF_COMMON_EXPORT PointOfInterestPropertiesWidget : public QWidget
{
   Q_OBJECT
public:
   PointOfInterestPropertiesWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   ~PointOfInterestPropertiesWidget() override = default;

   virtual void SetEntity(vespa::VaEntity* aEntityPtr, bool aCreate);

   const vespa::VaEntity* GetEntity() const { return mEntityPtr; }

   struct PoiInfo
   {
      std::string mName;
      UtLatPos    mLat;
      UtLonPos    mLon;
      UtLength    mAlt;
      bool        mBullseye;
   };

   virtual void OverwriteOriginalValues(const PoiInfo& aInfo) { mOriginalValues = aInfo; }

signals:
   void PoiBullseyeToggled(unsigned int aId, bool aBullseye);

protected:
   void         closeEvent(QCloseEvent* aEvent) override;
   void         hideEvent(QHideEvent* aEvent) override;
   virtual void AcceptChanges();
   virtual void NameOperation();
   virtual void PositionOperation(const vespa::VaPosition& aPosition);
   virtual void BullseyeOperation(bool aBullseye);
   void         CancelChanges();

   PoiInfo                       mOriginalValues;
   Ui::PointOfInterestProperties mUi;
   bool                          mCreatingNewPoi;
   vespa::VaEntity*              mEntityPtr;

private:
   void SetExportable();
   void UpdateWidget();
   void NameChanged();
   void BlockPositionSignals(bool aState);
   void PositionUpdated();
   void BullseyeUpdated(int aState);

   bool mAcceptChanges{false};
};
} // namespace Annotation
#endif
