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

#ifndef WKFRINGDATADIALOG_HPP
#define WKFRINGDATADIALOG_HPP

#include "wkf_common_export.h"

#include <QDialog>

#include "ui_WkfRingDataDialog.h"

namespace wkf
{
class WKF_COMMON_EXPORT RingDataDialog : public QDialog
{
   Q_OBJECT

public:
   RingDataDialog(QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());

   void SetMinValue(double aValue);
   void SetMaxValue(double aValue);

   double GetMinValue() const;
   double GetMaxValue() const;
   QColor GetMinColor() const;
   QColor GetMaxColor() const;
   bool   GetClamped() const;

   void SetUnit(const QString& aUnit);

private:
   Ui::WkfRingDataDialog mUi;
};
} // namespace wkf

#endif // !WKFRINGDATADIALOG_HPP
