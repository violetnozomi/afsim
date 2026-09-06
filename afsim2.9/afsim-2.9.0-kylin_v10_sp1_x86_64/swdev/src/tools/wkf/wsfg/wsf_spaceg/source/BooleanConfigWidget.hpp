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

#ifndef BOOLEANCONFIGWIDGET_H
#define BOOLEANCONFIGWIDGET_H

#include "ConfigWidget.hpp"

class QCheckBox;

namespace wsfg
{

namespace spaceg
{

//! A ConfigWidget subclass that shows a single check box.
class BooleanConfigWidget : public ConfigWidget
{
   Q_OBJECT

public:
   explicit BooleanConfigWidget(QWidget* aParentPtr = nullptr);
   ~BooleanConfigWidget() override {}

   void           Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override;
   QJsonObject    GetValues() const override;
   bool           SetValues(const QJsonObject& aData) override;
   void           ClearValues() override;
   static QString Type();

private:
   static bool ValidateObject(const QJsonObject& aConfiguration);

   QString    mPropertyName;
   QCheckBox* mCheckboxPtr;
};

} // namespace spaceg

} // namespace wsfg

#endif // BOOLEANCONFIGWIDGET_H
