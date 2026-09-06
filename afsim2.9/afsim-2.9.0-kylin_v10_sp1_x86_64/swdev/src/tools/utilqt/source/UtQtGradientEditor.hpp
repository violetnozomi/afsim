// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTQTGRADIENTEDITOR_HPP
#define UTQTGRADIENTEDITOR_HPP

#include "utilqt_export.h"

#include <QGradientStops>
#include <QIcon>
#include <QString>
#include <QWidget>

#include "ui_UtQtGradientEditor.h"

class QCloseEvent;
class QListWidgetItem;

class UtQtGradientMap;

//! A GUI for editing gradients in a UtQtGradientMap. This will modify the map
//! directly. See the signals on UtQtGradientMap to intercept the changes made.
class UTILQT_EXPORT UtQtGradientEditor : public QWidget
{
   Q_OBJECT

public:
   explicit UtQtGradientEditor(UtQtGradientMap& aGradientMap,
                               const QString&   aSelectedGradientName,
                               double           aMinValue = 0.0,
                               double           aMaxValue = 1.0,
                               QWidget*         aParent   = nullptr);
   ~UtQtGradientEditor() override = default;


protected:
   void closeEvent(QCloseEvent* aEvent) override;
signals:
   void GradientChosenOnClose(const QString& aGradientName);
private slots:
   void ColorUpdated(QListWidgetItem* aUpdatedItem);
   void ApplyColorChanges();
   void HandleGradientSelectionChanged();
   void UpdateColorList();
   void AddGradient();
   void RemoveGradient();
   void DiscardColorChanges();
   void AddColor();
   void RemoveColors();
   void UpdateButtonStates();

private:
   void             PromptUserToSave();
   double           TranslateToGradientPosition(double aPosition);
   double           TranslateToDataValue(double aPosition);
   QListWidgetItem* AddColor(qreal aPosition, const QColor& aColor);
   QString          GetNextCustomGradientName();
   QIcon            IconForColor(const QColor& aColor);
   void             InitGradientList(const QString& aSelectedGradient);

   UtQtGradientMap&         mGradientMap;
   QString                  mCurrentGradientName;
   double                   mMinValue;
   double                   mMaxValue;
   QGradientStops           mCurrentGradientStops;
   Ui::UtQtGradientEditorUi mUi;
};

#endif // UTQTGRADIENTEDITOR_HPP
