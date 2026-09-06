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

#ifndef DISJUNCTIONCONFIGWIDGET_H
#define DISJUNCTIONCONFIGWIDGET_H

#include <QString>
class QComboBox;
class QGroupBox;
class QStackedWidget;

#include "ConfigWidget.hpp"

namespace wsfg
{

namespace spaceg
{

//! A ConfigWidget subclass that displays one of a set of other ConfigWidgets.
//!
//! This is similar to the CompositeConfigWidget but instead of displaying a
//! collection of widgets simultaneously, only a single widget is displayed
//! at a time. The widget displayed is selected using a combo box labeled
//! with the available options.
class DisjunctionConfigWidget : public ConfigWidget
{
   Q_OBJECT

public:
   explicit DisjunctionConfigWidget(QWidget* aParentPtr = nullptr);
   ~DisjunctionConfigWidget() override {}

   void           Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override;
   QJsonObject    GetValues() const override;
   bool           SetValues(const QJsonObject& aData) override;
   void           ClearValues() override;
   static QString Type();

private:
   bool ValidateObject(const QJsonObject& aConfiguration);
   void ChangePanel(int aIndex);

   QString                    mName;
   QGroupBox*                 mGroupBoxPtr;
   QComboBox*                 mComboBoxPtr;
   QStackedWidget*            mStackedWidgetPtr;
   std::vector<ConfigWidget*> mProperties;
};

} // namespace spaceg

} // namespace wsfg

#endif // DISJUNCTIONCONFIGWIDGET_H
