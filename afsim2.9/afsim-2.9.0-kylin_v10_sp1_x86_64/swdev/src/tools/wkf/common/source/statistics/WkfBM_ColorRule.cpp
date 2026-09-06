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

#include "WkfBM_ColorRule.hpp"

#include <random>

#include <QMenu>

#include "UtQtMemory.hpp"
#include "WkfBM_PlatformStatusPrefObject.hpp"

static QColor GetRandomColor()
{
   static std::default_random_engine  engine{std::random_device()()};
   std::uniform_int_distribution<int> dist{0, 255};

   return QColor(dist(engine), dist(engine), dist(engine));
}

wkf::bm::ColorRule::ColorRule(QWidget* aParent, Mode aMode) noexcept
   : QWidget(aParent)
{
   mUi.setupUi(this);
   mUi.colorSelector->setAutoDefault(false);

   SetMode(aMode);
   mUi.colorSelector->SetColor(GetRandomColor());

   connect(this, &QWidget::customContextMenuRequested, this, &ColorRule::ContextMenu);

   connect(mUi.colorSelector, &UtQtColorButton::ColorChanged, [this](const QColor&) { emit RuleChanged(); });

   connect(mUi.comboBox, QOverload<int>::of(&QComboBox::activated), [this](int) { emit RuleChanged(); });

   connect(mUi.valueEdit, &QLineEdit::editingFinished, this, &ColorRule::RuleChanged);
}

QColor wkf::bm::ColorRule::GetColor() const noexcept
{
   return mUi.colorSelector->GetColor();
}

void wkf::bm::ColorRule::SetState(QColor aColor, const QString& aComparison, const QVariant& aValue)
{
   // RAII block
   {
      QSignalBlocker blocker(this);
      mUi.colorSelector->SetColor(aColor);
      switch (mMode)
      {
      case Mode::Boolean:
         mBoolTrue = aValue.toBool();
         mUi.boolLabel->setText(mBoolTrue ? "True" : "False");
         break;
      case Mode::Number:
      case Mode::Text:
         mUi.comboBox->setCurrentText(aComparison);
         mUi.valueEdit->setText(aValue.toString());
         break;
      }
   }

   emit RuleChanged();
}

QString wkf::bm::ColorRule::GetUserValue() const noexcept
{
   return mUi.valueEdit->text();
}

bool wkf::bm::ColorRule::TestValue(const QVariant& aValue, const QVariant& aUserValue) const noexcept
{
   switch (mMode)
   {
   case Mode::Boolean:
      return TestBoolean(aValue.toBool());
      break;
   case Mode::Number:
      return TestNumber(aValue.toDouble(), aUserValue.toDouble());
      break;
   case Mode::Text:
      return TestString(aValue.toString(), aUserValue.toString());
      break;
   }

   return false;
}

wkf::bm::ColorRule::Mode wkf::bm::ColorRule::GetMode() const noexcept
{
   return mMode;
}

void wkf::bm::ColorRule::SetMode(Mode aMode) noexcept
{
   QSignalBlocker blocker{this};
   mMode = aMode;
   switch (aMode)
   {
   case Mode::Boolean:
      mUi.comboBox->hide();
      mUi.valueEdit->hide();
      mUi.boolLabel->show();
      setContextMenuPolicy(Qt::NoContextMenu);
      break;
   case Mode::Number:
      mUi.comboBox->clear();
      mUi.comboBox->setEnabled(true);
      mUi.valueEdit->show();
      mUi.boolLabel->hide();
      // NOTE: The order here is important.
      // If it is changed, make sure TestNumber is updated.
      mUi.comboBox->addItems({"==", "!=", "<", "<=", ">", ">="});
      mUi.valueEdit->clear();
      mUi.valueEdit->setText("0");
      mUi.valueEdit->setPlaceholderText("Value");
      mUi.valueEdit->setValidator(&mNumberValidator);
      setContextMenuPolicy(Qt::CustomContextMenu);
      break;
   case Mode::Text:
      mUi.comboBox->clear();
      mUi.comboBox->setEnabled(true);
      mUi.valueEdit->show();
      mUi.boolLabel->hide();
      // NOTE: The order here is important.
      // If it is changed, make sure TestString is updated.
      mUi.comboBox->addItems({"==", "!=", "contains"});
      mUi.valueEdit->clear();
      mUi.valueEdit->setValidator(nullptr);
      mUi.valueEdit->setPlaceholderText("Text");
      setContextMenuPolicy(Qt::CustomContextMenu);
      break;
   }
}

wkf::bm::ColorData wkf::bm::ColorRule::ExportState() const
{
   ColorData retval;

   retval.mColor      = GetColor();
   retval.mComparison = mUi.comboBox->currentText();
   retval.mValue      = mUi.valueEdit->text();
   retval.mBool       = mBoolTrue;

   return retval;
}

void wkf::bm::ColorRule::ImportState(const ColorData& aColor)
{
   mUi.colorSelector->SetColor(aColor.mColor);
   mUi.comboBox->setCurrentText(aColor.mComparison);
   mUi.valueEdit->setText(aColor.mValue);
   mBoolTrue = aColor.mBool;
   mUi.boolLabel->setText(mBoolTrue ? "True" : "False");
}

void wkf::bm::ColorRule::SetCustomValidator(const QValidator* aValidator)
{
   mUi.valueEdit->setValidator(aValidator);
}

void wkf::bm::ColorRule::ContextMenu(const QPoint& aPoint)
{
   QMenu menu;

   QAction* act = ut::qt::make_qt_ptr<QAction>("Insert Color Before");
   connect(act, &QAction::triggered, [this]() { emit InsertRule(this, true); });
   menu.addAction(act);

   act = ut::qt::make_qt_ptr<QAction>("Insert Color After");
   connect(act, &QAction::triggered, [this]() { emit InsertRule(this, false); });
   menu.addAction(act);

   act = ut::qt::make_qt_ptr<QAction>("Remove Color");
   connect(act, &QAction::triggered, [this]() { emit RemoveRule(this); });
   menu.addAction(act);

   menu.exec(mapToGlobal(aPoint));
}

double wkf::bm::ColorRule::GetDouble() const noexcept
{
   bool   ok;
   double value = mUi.valueEdit->text().toDouble(&ok);
   if (ok)
   {
      return value;
   }
   return 0;
}

bool wkf::bm::ColorRule::TestBoolean(bool aValue) const noexcept
{
   return mBoolTrue == aValue;
}

bool wkf::bm::ColorRule::TestNumber(double aValue, double aUserValue) const noexcept
{
   switch (mUi.comboBox->currentIndex())
   {
   case 0: // ==
      return aValue == aUserValue;
   case 1: // !=
      return aValue != aUserValue;
   case 2: // <
      return aValue < aUserValue;
   case 3: // <=
      return aValue <= aUserValue;
   case 4: // >
      return aValue > aUserValue;
   case 5: // >=
      return aValue >= aUserValue;
   default:
      return false;
   }
}

bool wkf::bm::ColorRule::TestString(const QString& aValue, const QString& aUserValue) const noexcept
{
   switch (mUi.comboBox->currentIndex())
   {
   case 0: // ==
      return aValue.compare(aUserValue, Qt::CaseInsensitive) == 0;
   case 1: // !=
      return aValue.compare(aUserValue, Qt::CaseInsensitive) != 0;
   case 2: // contains
      return aValue.contains(aUserValue, Qt::CaseInsensitive);
   default:
      return false;
   }
}
