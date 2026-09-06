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

#ifndef ENABLERCONFIGWIDGET_H
#define ENABLERCONFIGWIDGET_H

#include <QString>

#include "ConfigWidget.hpp"

class QGroupBox;

namespace wsfg
{

namespace spaceg
{

//! A ConfigWidget subclass that represents a set of properties that are enabled or disabled as a group.
class EnablerConfigWidget : public ConfigWidget
{
   Q_OBJECT

public:
   explicit EnablerConfigWidget(QWidget* aParentPtr = nullptr);
   ~EnablerConfigWidget() override {}

   void           Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override;
   QJsonObject    GetValues() const override;
   bool           SetValues(const QJsonObject& aData) override;
   void           ClearValues() override;
   static QString Type();

private:
   static bool ValidateObject(const QJsonObject& aConfiguration);

   QString                          mName;
   QGroupBox*                       mGroupPtr;
   std::map<QString, ConfigWidget*> mProperties;
};

} // namespace spaceg

} // namespace wsfg

#endif // ENABLERCONFIGWIDGET_H
