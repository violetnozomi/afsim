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

#ifndef STRINGCONFIGWIDGET_H
#define STRINGCONFIGWIDGET_H

#include "ConfigWidget.hpp"

class QLabel;
class QLineEdit;

namespace wsfg
{

namespace spaceg
{

//! A ConfigWidget subclass that represents a single labeled string value.
class StringConfigWidget : public ConfigWidget
{
public:
   explicit StringConfigWidget(QWidget* aParentPtr = nullptr);
   ~StringConfigWidget() override {}

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

#endif // STRINGCONFIGWIDGET_H
