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

#ifndef COMPOSITECONFIGWIDGET_H
#define COMPOSITECONFIGWIDGET_H

#include <QString>

#include "ConfigWidget.hpp"

class QGroupBox;

namespace wsfg
{

namespace spaceg
{

//! A ConfigWidget subclass that collects a set of other ConfigWidgets into a group box.
class CompositeConfigWidget : public ConfigWidget
{
   Q_OBJECT

public:
   explicit CompositeConfigWidget(QWidget* aParentPtr = nullptr);
   ~CompositeConfigWidget() override {}

   void           Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override;
   QJsonObject    GetValues() const override;
   bool           SetValues(const QJsonObject& aData) override;
   void           ClearValues() override;
   static QString Type();

private:
   static bool ValidateObject(const QJsonObject& aConfiguration);

   QString                          mName;
   std::map<QString, ConfigWidget*> mProperties;
   QGroupBox*                       mGroupPtr;
};

} // namespace spaceg

} // namespace wsfg

#endif // COMPOSITECONFIGWIDGET_H
