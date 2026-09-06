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

#ifndef MAP_HOVER_INFO_LIST_SELECTION_WIDGET_HPP
#define MAP_HOVER_INFO_LIST_SELECTION_WIDGET_HPP

#include "ui_ListSelectionWidget.h"

namespace MapHoverInfo
{
class ListSelectionWidget : public QGroupBox
{
public:
   ListSelectionWidget(QWidget* aParent, const QString& aTitle = QString());

   QStringList GetAvailableOptions() const;
   QStringList GetSelectedOptions() const;

   void Clear();
   void ClearSelected();

   void SetAvailableOptions(const QStringList& aOptions);

   //! Equivalent to clearing selected options list then calling SelectOptions
   void SetSelectedOptions(const QStringList& aOptions);

   void AddOption(const QString& aOption, bool aAddToSelected = false);
   void AddOptions(const QStringList& aOptions, bool aAddToSelected = false);

   void SelectOption(const QString& aOption);
   void SelectOptions(const QStringList& aOptions);

   void DeselectOption(const QString& aOption);
   void DeselectOptions(const QStringList& aOptions);

   void RemoveOption(const QString& aOption);
   void RemoveOptions(const QStringList& aOptions);

private slots:
   void AddClicked();
   void RemoveClicked();
   void UpClicked();
   void DownClicked();

private:
   QStringList GetOptions(const QListWidget& aList) const;

   void RemoveOptionFrom(const QString& aOption, QListWidget& aList);
   void RemoveOptionsFrom(const QStringList& aOptions, QListWidget& aList);

   Ui::ListSelectionWidget mUi;
};
} // namespace MapHoverInfo

#endif
