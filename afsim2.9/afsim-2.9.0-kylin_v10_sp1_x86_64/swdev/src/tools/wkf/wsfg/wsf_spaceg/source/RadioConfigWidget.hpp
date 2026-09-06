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

#ifndef RADIOCONFIGWIDGET_HPP
#define RADIOCONFIGWIDGET_HPP

class QComboBox;
#include <QString>
class QLabel;

#include "ConfigWidget.hpp"

namespace wsfg
{

namespace spaceg
{

//! A ConfigWidget subclass that provides a mutually exclusive set of options.
//!
//! This widget displays a label identifying the property, as well as a combo box
//! that will allow the user to select one of the mutually exclusive options.
class RadioConfigWidget : public ConfigWidget
{
   Q_OBJECT

public:
   explicit RadioConfigWidget(QWidget* aParentPtr = nullptr);
   ~RadioConfigWidget() override {}

   void           Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override;
   QJsonObject    GetValues() const override;
   bool           SetValues(const QJsonObject& aData) override;
   void           ClearValues() override;
   static QString Type();

private:
   static bool ValidateObject(const QJsonObject& aConfiguration);

   QString    mName;
   QLabel*    mLabelPtr;
   QComboBox* mComboBoxPtr;
};

} // namespace spaceg

} // namespace wsfg

#endif // RADIOCONFIGWIDGET_HPP
