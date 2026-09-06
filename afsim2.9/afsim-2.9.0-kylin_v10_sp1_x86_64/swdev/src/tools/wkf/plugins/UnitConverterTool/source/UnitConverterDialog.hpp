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

#ifndef UNITCONVERTERDIALOG_HPP
#define UNITCONVERTERDIALOG_HPP

#include <functional>
#include <map>

#include <QDialog>

#include "UtQtUnitValueEdit.hpp"
#include "UtUnitTypes.hpp"
#include "UtUnits.hpp"
#include "ui_UnitConverterDialog.h"

namespace UnitConverter
{
class Dialog : public QDialog
{
   Q_OBJECT

public:
   Dialog(QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());

private:
   Ui::UnitConverterDialog mUi;

   void SetupMaps();

   void UnitTypeChanged(const QString& aString);
   void Convert();

   template<typename T, typename T2>
   void Repopulate();

   template<typename T, typename T2>
   void ConvertToUnit();

   QString mCurrentUnits;

   std::map<QString, std::function<void()>> mRepopulateMap;
   std::map<QString, std::function<void()>> mConvertMap;
};
} // namespace UnitConverter

#endif // !UNITCONVERTERDIALOG_HPP
