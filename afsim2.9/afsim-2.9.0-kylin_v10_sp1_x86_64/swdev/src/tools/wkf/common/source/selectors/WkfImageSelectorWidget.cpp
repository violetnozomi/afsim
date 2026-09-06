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

#include "WkfImageSelectorWidget.hpp"

#include <QLayout>
#include <QMimeData>

#include "WkfEnvironment.hpp"

void wkf::DragAndDropLineEdit::dragEnterEvent(QDragEnterEvent* event)
{
   if (event->mimeData()->hasUrls())
   {
      event->acceptProposedAction();
   }
}

void wkf::DragAndDropLineEdit::dropEvent(QDropEvent* event)
{
   if (event->mimeData()->hasUrls())
   {
      auto urls = event->mimeData()->urls();
      auto text = urls.front().toLocalFile();
      setText(text);
      emit ItemDropped();
   }
}

void wkf::ImageSelectionWidget::SetEnabledUpdate(bool aEnable)
{
   setEnabled(aEnable);
   CheckValidPath();
}

// primarily used for startup and for initial opening of the tab
// Images will be loaded from a path on Warlock startup, if a path was already provided.
void wkf::ImageSelectionWidget::resizeEvent(QResizeEvent* event)
{
   UpdateDisplayedImage();
}

// handles the case where a user edits the image path (the preview image will already change accordingly), but exits
// preferences without applying/saving. when re-opening the preferences, the old image path will be loaded, but without
// this overridden function, no signals/slots are triggered to change the preview image back
void wkf::ImageSelectionWidget::showEvent(QShowEvent* event)
{
   UpdateDisplayedImage();
}

wkf::ImageSelectionWidget::ImageSelectionWidget(QWidget* parent)
   : QWidget(parent)
   , mEditor(new DragAndDropLineEdit(this))
   , mBrowser(new QPushButton(this))
   , mIcon(new QLabel(this))
   , mIsValid(false)
{
   mBrowser->setText("Browse");
   mEditor->setPlaceholderText("Drag and drop or enter an image file...");
   mEditor->setStyleSheet("border: 1px solid gray");
   mEditor->setAcceptDrops(true);

   mIcon->setHidden(true);
   QHBoxLayout* layout = new QHBoxLayout;
   layout->addSpacing(18);
   layout->addWidget(mIcon, 0);
   layout->addWidget(mEditor, 10);
   layout->addWidget(mBrowser, 0);
   layout->setContentsMargins(0, 0, 0, 4);
   setLayout(layout);

   connect(mBrowser, &QPushButton::clicked, this, &wkf::ImageSelectionWidget::HandleBrowseButtonClicked);
   connect(mEditor, &DragAndDropLineEdit::ItemDropped, this, &wkf::ImageSelectionWidget::UpdateDisplayedImage);
   connect(mEditor, &QLineEdit::editingFinished, this, &wkf::ImageSelectionWidget::CheckValidPath);
}

void wkf::ImageSelectionWidget::SetText(const QString& aText)
{
   mEditor->setText(aText);
   CheckValidPath();
}

void wkf::ImageSelectionWidget::CheckValidPath()
{
   const bool isEmptyPath = mEditor->text().isEmpty();
   if (isEmptyPath)
   {
      mIsValid = false;
      mIcon->setHidden(true);
   }
   else if (isVisible())
   {
      mIsValid = UpdateDisplayedImage();
   }
   else
   {
      QPixmap pixmap(mEditor->text());
      mIsValid = !pixmap.isNull();
   }

   if (mIsValid || isEmptyPath)
   {
      mEditor->setStyleSheet("border: 1px solid gray");
   }
   else
   {
      mEditor->setStyleSheet(QString("border: 1px solid gray; color: %1").arg(mEditor->isEnabled() ? "red" : "maroon"));
   }
}

// should only be used when file paths change while tab is open: otherwise pixmap will not size correctly
bool wkf::ImageSelectionWidget::UpdateDisplayedImage()
{
   QString tempPath = mEditor->text();
   QPixmap pixmap(tempPath);
   if (pixmap.isNull())
   {
      mIcon->setHidden(true);
      return false;
   }
   else
   {
      mIcon->setHidden(false);
      mIcon->setPixmap(pixmap.scaled(mEditor->height(), mEditor->height(), Qt::KeepAspectRatio));
      return true;
   }
}

void wkf::ImageSelectionWidget::HandleBrowseButtonClicked()
{
   QString filePath = wkf::getOpenFileName(this, "Choose image", "", "*.jpg *.jpeg *.png *.tif *.tiff *.bmp");
   if (!filePath.isEmpty())
   {
      mEditor->setText(filePath);
      UpdateDisplayedImage();
   }
   CheckValidPath();
}
