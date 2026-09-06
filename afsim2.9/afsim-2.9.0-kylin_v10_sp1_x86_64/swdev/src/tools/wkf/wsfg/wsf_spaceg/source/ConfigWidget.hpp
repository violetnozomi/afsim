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

#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QJsonObject>
#include <QWidget>

namespace wsfg
{

namespace spaceg
{

class ConfigWidgetFactory;

//! This class provides the interface for derived ConfigWidget classes.
//! ConfigWidget provides a configurable set of widgets that can be
//! used to present structured data to the user, and have the user provide
//! structured data to the application. The configuration of these objects is
//! done via a JSON format text file that outlines the set of properties and
//! their types that are to be collected by this widget.
class ConfigWidget : public QWidget
{
   Q_OBJECT

public:
   explicit ConfigWidget(QWidget* parent = nullptr);
   ~ConfigWidget() override = default;

   //! Configure the widget given an input configuration object.
   //!
   //! This function will configure the widget according to the provided @p aConfiguration,
   //! constructing child widgets as needed using @p aFactory. See the individual subclasses
   //! for details about what fields can be used to configure each class.
   //!
   //! \param aFactory       - a widget factory used to create various ConfigWidget subclasses.
   //! \param aConfiguration - a JSON object containing the widget's configuration.
   virtual void Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) = 0;

   //! Retrieve the current values in this ConfigWidget.
   virtual QJsonObject GetValues() const = 0;

   //! Set the values in this widget and its children.
   //!
   //! This will populate the ConfigWidgets with the given @p aData. This will
   //! operate not only on this widget, but the children of this widget. If
   //! a value cannot be set for any property in @p aData, this will return false.
   //!
   //! \param aData - the values to set in this widget and its children.
   //! \return      - true if all properties in @p aData could be set in this widget; false otherwise.
   virtual bool SetValues(const QJsonObject& aData) = 0;

   //! Clear the current values in this widget and its children.
   virtual void ClearValues() = 0;

   static const QString cNAME;
   static const QString cTYPE;
   static const QString cDISPLAY;
   static const QString cPROPERTIES;
   static const QString cLABEL;
   static const QString cOPTION;
};

} // namespace spaceg

} // namespace wsfg

#endif // CONFIGWIDGET_H
