// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfPermissionGuiFactory.hpp"

#include <stdexcept>

#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QTextCursor>
#include <QVBoxLayout>

#include "UtMemory.hpp"
#include "UtQtLabeledInput.hpp"
#include "WkfEnvironment.hpp"
#include "WkfGeneralPrefObject.hpp"
#include "WkfPermissionManager.hpp"

wkf::permission::CategoryPage::CategoryPage(QWidget* aParent, QString aCategory) noexcept
   : QWidget(aParent)
   , mCategory(std::move(aCategory))
{
}

const QString& wkf::permission::CategoryPage::GetCategory() const noexcept
{
   return mCategory;
}

wkf::permission::GuiFactory::GuiFactory() noexcept
{
   SetupDefaultFactories();
}

QWidget* wkf::permission::GuiFactory::Fallback(PermissionNode& aNode, CategoryPage& aPage) const
{
   throw std::runtime_error("GuiFactory: Unrecognized node type.");
}

void wkf::permission::GuiFactory::SetupDefaultFactories() noexcept
{
   Reset();

   Register<BoolPermission, &GuiFactory::MakeCheckBox>();
   Register<IntPermission, &GuiFactory::MakeSpinBox>();
   Register<StringPermission, &GuiFactory::MakeLineEdit>();
   Register<StringListPermission, &GuiFactory::MakeTextEdit>();
   Register<ObjectPermission, &GuiFactory::MakeGroupBox>();
}

wkf::permission::CategoryPage* wkf::permission::GuiFactory::MakeCategoryPage(Category& aNode) const
{
   // Eventually, QWidget will likely be replaced with QScrollArea.
   auto retval = ut::make_unique<CategoryPage>(nullptr, aNode.mName);
   auto layout = ut::make_unique<QVBoxLayout>();

   layout->setSpacing(4);

   retval->setToolTip(aNode.mDescription);

   for (int i = 0; i < aNode.PermissionCount(); i++)
   {
      layout->addWidget(Visit(aNode.PermissionAt(i), *retval));
   }

   layout->addStretch(1);

   retval->setLayout(layout.release());

   QObject::connect(retval.get(), &CategoryPage::RestoreDefaults, &aNode, &Category::RestoreDefaults);

   return retval.release();
}

QWidget* wkf::permission::GuiFactory::MakeCheckBox(BoolPermission& aNode, CategoryPage& aPage) const
{
   auto retval = ut::qt::make_qt_ptr<QCheckBox>();

   retval->setChecked(aNode.Get());
   retval->setText(aNode.mName);
   retval->setToolTip(aNode.mDescription);

   QObject::connect(&aNode, &BoolPermission::Changed, retval, &QCheckBox::setChecked);

   QObject::connect(&aPage, &CategoryPage::ApplyChanges, [&aNode, retval]() { aNode.Set(retval->isChecked()); });
   QObject::connect(&aPage, &CategoryPage::RevertChanges, [&aNode, retval]() { retval->setChecked(aNode.Get()); });

   return retval;
}

QWidget* wkf::permission::GuiFactory::MakeSpinBox(IntPermission& aNode, CategoryPage& aPage) const
{
   auto* retval = ut::qt::make_qt_ptr<ut::qt::LabeledInput<QSpinBox>>(nullptr, aNode.mName);
   auto* widget = retval->GetWidget();

   widget->setValue(aNode.Get());
   retval->setToolTip(aNode.mDescription);

   QObject::connect(&aNode, &IntPermission::Changed, widget, &QSpinBox::setValue);

   QObject::connect(&aPage, &CategoryPage::ApplyChanges, [&aNode, widget]() { aNode.Set(widget->value()); });
   QObject::connect(&aPage, &CategoryPage::RevertChanges, [&aNode, widget]() { widget->setValue(aNode.Get()); });

   return retval;
}

QWidget* wkf::permission::GuiFactory::MakeLineEdit(StringPermission& aNode, CategoryPage& aPage) const
{
   auto* retval = ut::qt::make_qt_ptr<ut::qt::LabeledInput<QLineEdit>>(nullptr, aNode.mName);
   auto* widget = retval->GetWidget();

   widget->setText(aNode.Get());
   retval->setToolTip(aNode.mDescription);
   auto* colorPrefs = wkfEnv.GetPreferenceObject<wkf::GeneralPrefObject>();
   if (colorPrefs && colorPrefs->GetPreferences().mThemeName == "Dark")
   {
      widget->setStyleSheet("color: lightGray");
   }

   QObject::connect(&aNode, &StringPermission::Changed, widget, &QLineEdit::setText);

   QObject::connect(&aPage, &CategoryPage::ApplyChanges, [&aNode, widget]() { aNode.Set(widget->text()); });
   QObject::connect(&aPage, &CategoryPage::RevertChanges, [&aNode, widget]() { widget->setText(aNode.Get()); });

   return retval;
}

//! Due to an apparent bug in Qt 5.12's QPlainTextEdit, placeholder text will not appear after a call to setPlainText.
//! This function manually adds and removes a character to trick the text edit into thinking it has been edited.
static void UpdatePlainTextEdit(QPlainTextEdit& aEditor)
{
   auto cursor = aEditor.textCursor();
   cursor.movePosition(QTextCursor::End);
   cursor.insertText("\n");
   cursor.selectionStart();
   cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
   cursor.selectionEnd();
   cursor.removeSelectedText();
}

QWidget* wkf::permission::GuiFactory::MakeTextEdit(StringListPermission& aNode, CategoryPage& aPage) const
{
   auto* retval = ut::qt::make_qt_ptr<ut::qt::LabeledInput<QPlainTextEdit>>(nullptr, aNode.mName);
   auto* widget = retval->GetWidget();

   widget->setPlainText(aNode.Get().join('\n'));
   widget->setPlaceholderText("Place each item on a separate line.");
   retval->setToolTip(aNode.mDescription);
   UpdatePlainTextEdit(*widget);

   QObject::connect(&aNode,
                    &StringListPermission::Changed,
                    widget,
                    [widget](QStringList aNewValue)
                    {
                       widget->setPlainText(aNewValue.join('\n'));
                       UpdatePlainTextEdit(*widget);
                    });
   QObject::connect(&aPage,
                    &CategoryPage::ApplyChanges,
                    [&aNode, widget]() { aNode.Set(widget->toPlainText().split('\n', QString::SkipEmptyParts)); });
   QObject::connect(&aPage,
                    &CategoryPage::RevertChanges,
                    [&aNode, widget]()
                    {
                       widget->setPlainText(aNode.Get().join('\n'));
                       UpdatePlainTextEdit(*widget);
                    });

   return retval;
}

QWidget* wkf::permission::GuiFactory::MakeGroupBox(ObjectPermission& aNode, CategoryPage& aPage) const
{
   auto retval = ut::make_unique<QGroupBox>();
   auto layout = ut::make_unique<QVBoxLayout>();

   layout->setSpacing(4);

   retval->setTitle(aNode.mName);
   retval->setToolTip(aNode.mDescription);

   for (int i = 0; i < aNode.FieldCount(); i++)
   {
      layout->addWidget(Visit(aNode.GetField(i), aPage));
   }

   retval->setLayout(layout.release());
   return retval.release();
}
