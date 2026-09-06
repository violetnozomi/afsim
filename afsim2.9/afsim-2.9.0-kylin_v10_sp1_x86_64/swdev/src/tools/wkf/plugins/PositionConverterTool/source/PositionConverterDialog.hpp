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

#ifndef POSITIONCONVERTERDIALOG_HPP
#define POSITIONCONVERTERDIALOG_HPP

#include <QDialog>

#include "UtQtUnitLineEdit.hpp"
#include "ui_PositionConverterDialog.h"

namespace PositionConverter
{
class Dialog : public QDialog
{
   Q_OBJECT

public:
   Dialog(QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());

private:
   Ui::PositionConverterDialog mUi;
   void                        Convert();
};
} // namespace PositionConverter
#endif
