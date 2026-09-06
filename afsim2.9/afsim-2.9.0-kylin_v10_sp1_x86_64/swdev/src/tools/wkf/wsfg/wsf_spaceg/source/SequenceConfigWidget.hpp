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

#ifndef SEQUENCECONFIGWIDGET_H
#define SEQUENCECONFIGWIDGET_H

#include <map>

#include "ConfigWidget.hpp"

namespace wsfg
{

namespace spaceg
{

//! A ConfigWidget subclass representing an ungrouped set of properties.
class SequenceConfigWidget : public ConfigWidget
{
public:
   explicit SequenceConfigWidget(QWidget* aParentPtr = nullptr);
   ~SequenceConfigWidget() override = default;

   void           Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override;
   QJsonObject    GetValues() const override;
   bool           SetValues(const QJsonObject& aData) override;
   void           ClearValues() override;
   static QString Type();

private:
   static bool ValidateObject(const QJsonObject& aConfiguration);

   std::map<QString, ConfigWidget*> mProperties;
};

} // namespace spaceg

} // namespace wsfg

#endif // SEQUENCECONFIGWIDGET_H
