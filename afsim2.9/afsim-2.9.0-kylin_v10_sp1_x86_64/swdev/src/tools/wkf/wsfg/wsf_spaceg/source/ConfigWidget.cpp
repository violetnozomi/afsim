// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ConfigWidget.hpp"

#include <QJsonDocument>
#include <QMessageBox>


namespace wsfg
{

namespace spaceg
{

ConfigWidget::ConfigWidget(QWidget* parent)
   : QWidget(parent)
{
}

//! The key value used in configuration files to give the name of a property.
const QString ConfigWidget::cNAME = QString("name");

//! The key value used in configuration files to give the type of a ConfigWidget.
const QString ConfigWidget::cTYPE = QString("type");

//! The key value used in configuration files to give the display name of a property.
//! This is distinguished from the name of the property in that this is intended for
//! output to users, and is likely to be more meaningful or complete.
const QString ConfigWidget::cDISPLAY = QString("display");

const QString ConfigWidget::cPROPERTIES = QString("properties");
const QString ConfigWidget::cLABEL      = QString("label");
const QString ConfigWidget::cOPTION     = QString("option");

} // namespace spaceg

} // namespace wsfg
