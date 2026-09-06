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

#ifndef INTEGERCONFIGWIDGET_H
#define INTEGERCONFIGWIDGET_H

#include <QString>

#include "ConfigWidget.hpp"

class QLabel;
class QLineEdit;

namespace wsfg
{

namespace spaceg
{

//! A ConfigWidget subclass that provides a single labeled integer input.
class IntegerConfigWidget : public ConfigWidget
{
   Q_OBJECT

public:
   explicit IntegerConfigWidget(QWidget* aParentPtr = nullptr);
   ~IntegerConfigWidget() override {}

   void           Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override;
   QJsonObject    GetValues() const override;
   bool           SetValues(const QJsonObject& aData) override;
   void           ClearValues() override;
   static QString Type();

private:
   static bool ValidateObject(const QJsonObject& aConfiguration);

   QString    mPropertyName;
   QLabel*    mLabelPtr;
   QLineEdit* mValuePtr;
};

} // namespace spaceg

} // namespace wsfg

#endif // INTEGERCONFIGWIDGET_H
