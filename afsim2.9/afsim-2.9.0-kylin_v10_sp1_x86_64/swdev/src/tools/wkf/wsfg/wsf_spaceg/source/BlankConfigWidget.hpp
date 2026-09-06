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

#ifndef BLANKCONFIGWIDGET_HPP
#define BLANKCONFIGWIDGET_HPP

#include "ConfigWidget.hpp"

namespace wsfg
{

namespace spaceg
{

//! A ConfigWidget subclass that contains no controls and displays no information.
class BlankConfigWidget : public ConfigWidget
{
   Q_OBJECT

public:
   explicit BlankConfigWidget(QWidget* aParentPtr = nullptr)
      : ConfigWidget{aParentPtr}
   {
   }
   ~BlankConfigWidget() override = default;

   void           Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override {}
   QJsonObject    GetValues() const override { return QJsonObject{}; }
   bool           SetValues(const QJsonObject& aData) override { return true; }
   void           ClearValues() override {}
   static QString Type() { return "blank"; }
};

} // namespace spaceg

} // namespace wsfg

#endif // BLANKCONFIGWIDGET_HPP
