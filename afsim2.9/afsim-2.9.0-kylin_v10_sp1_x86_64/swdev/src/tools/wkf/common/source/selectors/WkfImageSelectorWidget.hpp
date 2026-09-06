// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFIMAGESELECTORWIDGET_HPP
#define WKFIMAGESELECTORWIDGET_HPP

#include "wkf_common_export.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace wkf
{
// A generic LineEdit that supports Drag and Drop
class DragAndDropLineEdit : public QLineEdit
{
   Q_OBJECT
public:
   DragAndDropLineEdit(QWidget* parent = nullptr)
      : QLineEdit(parent){};

signals:
   void ItemDropped();

protected:
   void dragEnterEvent(QDragEnterEvent* event) override;
   void dropEvent(QDropEvent* event) override;
};

class WKF_COMMON_EXPORT ImageSelectionWidget : public QWidget
{
   Q_OBJECT
public:
   ImageSelectionWidget(QWidget* parent = nullptr);

   void    SetText(const QString& aText);
   QString GetText() const { return mEditor->text(); }

   bool IsValid() const { return mIsValid; }
   void SetEnabledUpdate(bool aEnable);

protected:
   void resizeEvent(QResizeEvent* event) override;
   void showEvent(QShowEvent* event) override;

private:
   void HandleBrowseButtonClicked();
   bool UpdateDisplayedImage();
   void CheckValidPath();

   DragAndDropLineEdit* mEditor;
   QPushButton*         mBrowser;
   QLabel*              mIcon;

   bool mIsValid;
};
} // namespace wkf

#endif
