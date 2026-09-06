// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef COVERAGEOVERLAYDOCKWIDGET_HPP
#define COVERAGEOVERLAYDOCKWIDGET_HPP

#include <QDockWidget>
class QString;
class QStringList;

#include "UtQtGradientMap.hpp"
#include "ui_CoverageOverlayDockWidget.h"

namespace CoverageOverlay
{

//! This class presents the user with information and controls for the loaded coverage heatmaps.
class CoverageOverlayDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit CoverageOverlayDockWidget(const UtQtGradientMap& aGradientMap,
                                      QWidget*               aPrentPtr = nullptr,
                                      Qt::WindowFlags        aFlags    = Qt::WindowFlags{});
   ~CoverageOverlayDockWidget() override = default;

   //! Called when a coverage overlay file is loaded.
   void CoverageLoaded(const QString& aFileName);

   //! Called when a coverage overlay file is unloaded.
   void CoverageUnloaded(const QString& aFileName);

   //! Reset the UI to its resting state.
   void ClearOptions();

   //! Set options on the UI so that it reflects a selected coverage overlay.
   void SetOptions(const QString&     aCoverageName,
                   const QStringList& aFieldNames,
                   const QString&     aFieldName,
                   const QString&     aFieldType,
                   const QString&     aFieldSubtype,
                   int                aFieldUnitType,
                   unsigned int       aOpacity,
                   bool               aAutoScale,
                   float              aMinValue,
                   float              aMaxValue,
                   const QString&     aGradientSelection);

   //! Set options on the UI so that it reflects a selected field.
   void SetOptions(const QString& aFieldType,
                   const QString& aFieldSubtype,
                   int            aFieldUnitType,
                   unsigned int   aOpacity,
                   bool           aAutoScale,
                   float          aMinValue,
                   float          aMaxValue,
                   const QString& aGradientSelection);

   //! Set the range of field values for the current field.
   void SetValueRange(float aMinValue, float aMaxValue);

   //! Return the name of selected coverage overlay file.
   QString GetSelectedCoverage() const;

signals:
   void CoveragesSelectedForLoad(const QStringList& aFileList);
   void CoverageSelectedForUnload(const QString& aFileName);
   void CoverageVisibilityChanged(const QString& aFileName, bool aVisible);
   void RequestCenterOnCoverage(const QString& aFileName);
   void CoverageSelected(const QString& aFileName);
   void FieldChanged(const QString& aFileName, const QString& aFieldName);
   void OpacityChanged(const QString& aFileName, int aValue);
   void AutoScaleStateChanged(const QString& aFileName, bool aChecked);
   void RangeChanged(const QString& aFileName, float aMinValue, float aMaxValue);
   void GradientSelected(const QString& aFileName, const QString& aGradientName);

private:
   //! Get the minimum field value selected.
   float GetRangeMinimum() const;

   //! Get the maximum field value selected.
   float GetRangeMaximum() const;

   void OnLoadDataPressed();
   void OnUnloadDataPressed();
   void OnCoverageDataSelected();
   void OnItemChanged(QListWidgetItem* aItemPtr);
   void OnItemDoubleClicked(QListWidgetItem* aItemPtr);
   void OnFieldSelection(const QString& aFieldName);
   void OnOpacityChanged(int aValue);
   void OnAutoScaleStateChanged(int aState);
   void OnRangeChanged();
   void OnGradientSelection(const QString& aGradientName);
   void OnUnitsUpdate();

   void ConnectToUnitsChanged();

   QString GetUnitSuffixString() const;
   double  GetConvertedValue(double aValue) const;
   double  GetStandardValue(double aValue) const;
   int     GetUnitPrecision() const;

   Ui::CoverageOverlayDockWidget mUI;
   const UtQtGradientMap&        mGradientMap;
   int                           mCurrentUnitType;
};

} // namespace CoverageOverlay

#endif // COVERAGEOVERLAYDOCKWIDGET_HPP
