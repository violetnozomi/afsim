// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtQtGradientEditor.hpp"

#include <cassert>

#include <QCloseEvent>
#include <QDialogButtonBox>
#include <QIcon>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>

#include "UtQtGradientBar.hpp"
#include "UtQtGradientColorListItem.hpp"
#include "UtQtGradientEditDelegate.hpp"
#include "UtQtGradientMap.hpp"

//! Initializing constructor.
//! @param aGradientMap The gradient map to edit.
//! @param aSelectedGradientName The name of the gradient to initially edit.
//! @param aMinValue The minimum [known] valid data value for the set of data
//! currently being visualized. See the note below.
//! @param aMaxValue The maximum [known] valid data value for the set of data
//! currently being visualized. See the note below.
//! @param aParent The usual optional parent widget.
//! @note The min and max value parameters will set the bounds for acceptable
//! data values that can be input by the user. It will also normalize the tick
//! labels alongside the gradient bar according to those bounds. This will assist
//! in making gradients that are specific to a certain data set or type of data.
UtQtGradientEditor::UtQtGradientEditor(UtQtGradientMap& aGradientMap,
                                       const QString&   aSelectedGradientName,
                                       double           aMinValue,
                                       double           aMaxValue,
                                       QWidget*         aParent)
   : QWidget(aParent)
   , mGradientMap(aGradientMap)
   , mMinValue(aMinValue)
   , mMaxValue(aMaxValue)
{
   assert(mGradientMap.Contains(UtQtGradientMap::cDEFAULT_GRADIENT_KEY));
   mUi.setupUi(this);
   mUi.gradientLegend->SetVariableName("");
   mUi.gradientLegend->SetDataBounds(mMinValue, mMaxValue);
   setAttribute(Qt::WA_DeleteOnClose);

   InitGradientList(aSelectedGradientName);

   connect(mUi.buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), SLOT(ApplyColorChanges()));
   connect(mUi.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), SLOT(DiscardColorChanges()));
   connect(mUi.addGradientBn, SIGNAL(clicked()), SLOT(AddGradient()));
   connect(mUi.removeGradientBn, SIGNAL(clicked()), SLOT(RemoveGradient()));
   connect(mUi.addColorBn, SIGNAL(clicked()), SLOT(AddColor()));
   connect(mUi.removeColorBn, SIGNAL(clicked()), SLOT(RemoveColors()));
   connect(mUi.gradientComboBox, SIGNAL(currentIndexChanged(int)), SLOT(HandleGradientSelectionChanged()));
   connect(mUi.colorList, SIGNAL(itemSelectionChanged()), SLOT(UpdateButtonStates()));
   connect(mUi.colorList, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(ColorUpdated(QListWidgetItem*)));

   mUi.colorList->setItemDelegate(new UtQtGradientEditDelegate(mMinValue, mMaxValue, this));
}

void UtQtGradientEditor::closeEvent(QCloseEvent* aEvent)
{
   PromptUserToSave();
   emit GradientChosenOnClose(mCurrentGradientName);
   QWidget::closeEvent(aEvent);
}

//! When the user edits a color, this will re-sort the color list and refresh
//! the gradient bar.
void UtQtGradientEditor::ColorUpdated(QListWidgetItem* aUpdatedItem)
{
   const QColor updatedIconColor = aUpdatedItem->data(Qt::UserRole).value<QColor>();
   const QIcon  updatedIcon      = IconForColor(updatedIconColor);
   mUi.colorList->blockSignals(true); // Avoid infinite loop on next line
   aUpdatedItem->setIcon(updatedIcon);
   mUi.colorList->blockSignals(false);
   mUi.colorList->sortItems(Qt::DescendingOrder);
   QGradient gradient;
   for (int i = 0; i < mUi.colorList->count(); ++i)
   {
      const QListWidgetItem* item     = mUi.colorList->item(i);
      const qreal            position = item->text().toDouble();
      const QColor           color    = item->data(Qt::UserRole).value<QColor>();
      gradient.setColorAt(TranslateToGradientPosition(position), color);
   }
   mCurrentGradientStops = gradient.stops();
   mUi.gradientLegend->GradientBar()->SetGradientStops(gradient.stops());
   UpdateButtonStates();
}

//! Saves the current gradient.
void UtQtGradientEditor::ApplyColorChanges()
{
   mGradientMap.SetGradientStops(mCurrentGradientName, mCurrentGradientStops);
   UpdateButtonStates();
}

//! When the user selects a different gradient, this prompts the user to save
//! any changes that have been made to the current gradient and then updates the
//! GUI for the newly selected gradient.
void UtQtGradientEditor::HandleGradientSelectionChanged()
{
   PromptUserToSave();
   mCurrentGradientName = mUi.gradientComboBox->currentText();
   if (mGradientMap.Contains(mCurrentGradientName))
   {
      mCurrentGradientStops = mGradientMap.GradientStops(mCurrentGradientName);
   }
   UpdateButtonStates();
   UpdateColorList();
}

//! Clears and repopulates the color list with the colors of the currently
//! selected gradient. Sets the gradient bar color.
void UtQtGradientEditor::UpdateColorList()
{
   mUi.colorList->clear();
   for (int i = 0; i < mCurrentGradientStops.count(); ++i)
   {
      AddColor(mCurrentGradientStops[i].first, mCurrentGradientStops[i].second);
   }
   mUi.colorList->sortItems(Qt::DescendingOrder);
   mUi.gradientLegend->GradientBar()->SetGradientStops(mCurrentGradientStops);
}

//! Prompts the user to add a new gradient after the user clicks the add (+) button.
//! The user cannot add a gradient with an empty or duplicate name.
void UtQtGradientEditor::AddGradient()
{
   QInputDialog dlg(this);
   dlg.setWindowTitle("Gradient Name");
   dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
   dlg.setLabelText("New gradient name:");
   dlg.setTextValue(GetNextCustomGradientName());
   dlg.setOkButtonText("Add");
   while (dlg.exec() == QInputDialog::Accepted)
   {
      const QString& gradientName = dlg.textValue();
      if (gradientName.isEmpty())
      {
         QMessageBox::warning(this, "Invalid Name", "The gradient name cannot be empty.");
      }
      else if (mGradientMap.Contains(gradientName))
      {
         QMessageBox::warning(this,
                              "Duplicate Name",
                              QString("A gradient with the name <b>%1</b> already exists.").arg(gradientName));
      }
      else
      {
         mUi.gradientComboBox->addItem(gradientName);
         mUi.gradientComboBox->setCurrentIndex(mUi.gradientComboBox->count() - 1);
         mCurrentGradientName = gradientName;
         mGradientMap.SetGradientStops(mCurrentGradientName, mCurrentGradientStops);
         UpdateButtonStates();
         break;
      }
   }
}

//! Removes the gradient from the list and from the gradient map.
//! @note This first discards any changes that may have been applied to the
//! gradient, so that the user is not prompted when HandleGradientSelectionChanged
//! is triggered by the removal. A little ugly, but it works.
void UtQtGradientEditor::RemoveGradient()
{
   const QString gradientToRemove = mCurrentGradientName;
   mCurrentGradientStops          = mGradientMap.GradientStops(gradientToRemove); // Discard changes
   mUi.gradientComboBox->removeItem(mUi.gradientComboBox->currentIndex());
   mGradientMap.RemoveGradient(gradientToRemove);
}

//! Resets any changes to the current gradient's colors since the last save.
void UtQtGradientEditor::DiscardColorChanges()
{
   mCurrentGradientStops = mGradientMap.GradientStops(mCurrentGradientName);
   UpdateButtonStates();
   UpdateColorList();
}

//! Adds a new color to the gradient when the user clicks the add (+) button.
//! The color is added to the list inline.
void UtQtGradientEditor::AddColor()
{
   QListWidgetItem* item = AddColor(0.0, QColor(Qt::black));
   mUi.colorList->scrollToBottom();
   mUi.colorList->editItem(item);
}

//! Removes the selected colors in the color list from the gradient.
void UtQtGradientEditor::RemoveColors()
{
   QList<QListWidgetItem*> selectedColors = mUi.colorList->selectedItems();
   for (int i = 0; i < selectedColors.count(); ++i)
   {
      delete selectedColors[i];
   }
   QGradient gradient;
   for (int i = 0; i < mUi.colorList->count(); ++i)
   {
      QListWidgetItem* item     = mUi.colorList->item(i);
      const qreal      position = item->text().toDouble();
      const QColor     color    = item->data(Qt::UserRole).value<QColor>();
      gradient.setColorAt(TranslateToGradientPosition(position), color);
   }
   mCurrentGradientStops = gradient.stops();
   UpdateButtonStates();
   UpdateColorList();
}

//! Translates a data value from the range of [mMinValue, mMaxValue] to a
//! gradient position in the range of [0.0, 1.0].
double UtQtGradientEditor::TranslateToGradientPosition(double aPosition)
{
   return (aPosition - mMinValue) / (mMaxValue - mMinValue);
}

//! Translates a gradient position from the range of [0.0, 1.0] to a data value
//! in the range of [mMinValue, mMaxValue].
double UtQtGradientEditor::TranslateToDataValue(double aPosition)
{
   return (aPosition * (mMaxValue - mMinValue)) + mMinValue;
}

//! Updates the enabled state of various buttons.
void UtQtGradientEditor::UpdateButtonStates()
{
   const bool isDefaultGradientSelected = UtQtGradientMap::IsDefaultGradient(mCurrentGradientName);
   const bool isGradientModified        = (mCurrentGradientStops != mGradientMap.GradientStops(mCurrentGradientName));
   mUi.removeGradientBn->setEnabled(!isDefaultGradientSelected);
   mUi.addColorBn->setEnabled(!isDefaultGradientSelected);
   mUi.removeColorBn->setEnabled(!isDefaultGradientSelected && !mUi.colorList->selectedItems().isEmpty());
   mUi.buttonBox->button(QDialogButtonBox::Reset)->setEnabled(isGradientModified);
   mUi.buttonBox->button(QDialogButtonBox::Apply)->setEnabled(isGradientModified);
   ;
}

//! Prompts the users to save changes to the current gradient. Note that the
//! user is forced to Save or Discard - there is no Cancel option to undo the
//! action they took to cause the prompt in the first place. Mainly this is
//! because when the user changes the gradient combo box selection, there's no
//! way of knowing what was selected before without manually tracking it. It can
//! be added later if it's important.
void UtQtGradientEditor::PromptUserToSave()
{
   if (!mGradientMap.Contains(mCurrentGradientName) ||
       (mCurrentGradientStops != mGradientMap.GradientStops(mCurrentGradientName)))
   {
      if (QMessageBox::question(this,
                                "Save Gradient?",
                                QString("The gradient <b>%1</b> has been modified. Would you like to save the changes?")
                                   .arg(mCurrentGradientName),
                                QMessageBox::Save,
                                QMessageBox::Discard) == QMessageBox::Save)
      {
         mGradientMap.SetGradientStops(mCurrentGradientName, mCurrentGradientStops);
      }
   }
}

//! Adds a color to the color list.
QListWidgetItem* UtQtGradientEditor::AddColor(qreal aPosition, const QColor& aColor)
{
   const bool                 isDefaultGradientSelected = UtQtGradientMap::IsDefaultGradient(mCurrentGradientName);
   const QIcon                itemIcon                  = IconForColor(aColor);
   const QString&             itemText                  = QString::number(TranslateToDataValue(aPosition));
   UtQtGradientColorListItem* item                      = new UtQtGradientColorListItem(itemIcon, itemText);
   if (!isDefaultGradientSelected)
   {
      item->setFlags(item->flags() | Qt::ItemIsEditable);
   }
   item->setData(Qt::UserRole, aColor);
   mUi.colorList->addItem(item);
   return item;
}

//! Returns an automatically generated gradient name that can be used to prefill
//! forms for the user when adding a gradient.
QString UtQtGradientEditor::GetNextCustomGradientName()
{
   const QString customGradientName("Custom Gradient %1");
   int           num = 1;
   while (mUi.gradientComboBox->findText(customGradientName.arg(num)) != -1)
   {
      ++num;
   }
   return customGradientName.arg(num);
}

//! Returns an icon filled with the given color.
QIcon UtQtGradientEditor::IconForColor(const QColor& aColor)
{
   QPixmap itemIconColorPixmap(32, 32);
   itemIconColorPixmap.fill(aColor);
   return QIcon(itemIconColorPixmap);
}

//! Initializes the list of gradients and selects the appropriate one.
void UtQtGradientEditor::InitGradientList(const QString& aSelectedGradient)
{
   const UtQtGradientList& gradients = mGradientMap.Gradients();
   for (int i = 0; i < gradients.count(); ++i)
   {
      const QString& gradientName = gradients[i].first;
      mUi.gradientComboBox->addItem(gradientName);
   }

   const int selectedIdx = mUi.gradientComboBox->findText(aSelectedGradient);
   if (selectedIdx >= 0)
   {
      mUi.gradientComboBox->setCurrentIndex(selectedIdx);
      mCurrentGradientName  = aSelectedGradient;
      mCurrentGradientStops = mGradientMap.GradientStops(mCurrentGradientName);
      UpdateButtonStates();
      UpdateColorList();
   }
}
