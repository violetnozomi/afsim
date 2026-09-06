// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "PerformanceTestWidget.hpp"

#include <QTimer>

#include "ui_PerformanceTestWidget.h"

namespace Designer
{

PerformanceTestWidget::PerformanceTestWidget(QWidget* aParent,
                                             Qt::WindowFlags aFlags)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::PerformanceTestWidget)
{
   mUIPtr->setupUi(this);

   // TODO: Once these are fully implemented, remove this line
   mUIPtr->mPerformanceTestComboBox->setHidden(true);

   connect(mUIPtr->mStartTestButton, &QPushButton::clicked, this, &PerformanceTestWidget::RunTestClicked);
   connect(mUIPtr->mPerformanceTestComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &PerformanceTestWidget::HandleTestChanged);

   mProgressTimer = new QTimer(this);
   connect(mProgressTimer, &QTimer::timeout, this, &PerformanceTestWidget::UpdateProgressBar);

   QString curTest = mUIPtr->mPerformanceTestComboBox->currentText();
   QStringList strList = curTest.split("@");
   QStringList varList = strList[1].split("/");
   mUIPtr->mVarOneLabel->setText(varList[0]);
   mUIPtr->mVarTwoLabel->setText(varList[1]);
}


PerformanceTestWidget::~PerformanceTestWidget()
{
   delete mProgressTimer;
   delete mUIPtr;
}

void PerformanceTestWidget::RunTestClicked(bool aClicked)
{
   double returnValue = 0.0;
   switch (mUIPtr->mPerformanceTestComboBox->currentIndex())
   {
      case 0:
         returnValue = RunMaxSpeedTest();
         break;
      case 1:
         returnValue = RunMinSpeedTest();
         break;
      case 2:
         returnValue = RunMaxGLoadTest();
         break;
      case 3:
         returnValue = RunMaxSustainedGLoadTest();
         break;
      case 4:
         returnValue = RunMaxLevelFlightTest();
         break;
      case 5:
         returnValue = RunMaxLoftedRangeTest();
         break;
      case 6:
         returnValue = RunMaxSustainedPitchAngleTest();
         break;
      case 7:
         returnValue = RunMaxRollRateTest();
         break;
      case 8:
         returnValue = RunTimeToDampRollTest();
         break;
      case 9:
         returnValue = RunMaxPitchRateTest();
         break;
      case 10:
         returnValue = RunTimeToDampPitchTest();
         break;
      case 11:
         returnValue = RunMaxYawRateTest();
         break;
      case 12:
         returnValue = RunTimeToDampYawTest();
         break;
      default:
         returnValue = RunMaxSpeedTest();
   }
   mUIPtr->mResultLineEdit->setText(QString::number(returnValue));
}

void PerformanceTestWidget::UpdateProgressBar()
{
   int curVal = mUIPtr->mTestProgressBar->value();
   if (curVal < 100)
   {
      mUIPtr->mTestProgressBar->setValue(curVal + 1);
   }
   else
   {
      mProgressTimer->stop();
      mUIPtr->mTestProgressBar->setMaximum(100);
      mUIPtr->mTestProgressBar->setValue(100);
      mUIPtr->mTestProgressBar->setFormat("Test Complete");
      mUIPtr->mTestProgressBar->setTextVisible(true);
      mUIPtr->mStartTestButton->setText("Start Test");
      mUIPtr->mStartTestButton->setDisabled(false);

      //srand(time(NULL));
      //int randVal = rand() % 100 + 1; //random from 1 - 100
      //mUIPtr->mResultLineEdit->setText(QString::number(randVal));

      //static int runCount = 1;
      //double answer = 1.8;
      //if (runCount == 2)
      //{
      //   answer = 2.1;
      //}
      //else if (runCount == 3)
      //{
      //   answer = 1.99;
      //}
      //QString answerStr = QString::number(answer) + " Mach";
      //mUIPtr->mResultLineEdit->setText(answerStr);
      //++runCount;
   }
}


void PerformanceTestWidget::HandleTestChanged()
{
   mProgressTimer->stop();
   mUIPtr->mTestProgressBar->setMaximum(100);
   mUIPtr->mTestProgressBar->setValue(0);
   mUIPtr->mTestProgressBar->setTextVisible(false);
   mUIPtr->mStartTestButton->setText("Start Test");
   mUIPtr->mStartTestButton->setDisabled(false);

   mUIPtr->mResultLineEdit->setText("");

   QString curTest = mUIPtr->mPerformanceTestComboBox->currentText();
   QStringList strList = curTest.split("@");
   QStringList varList = strList[1].split("/");

   if (varList.size() == 1)
   {
      mUIPtr->mVarTwoLabel->hide();
      mUIPtr->mVarTwoLineEdit->hide();
      mUIPtr->mVarOneLabel->setText(varList[0]);
   }
   else
   {
      mUIPtr->mVarTwoLabel->show();
      mUIPtr->mVarTwoLineEdit->show();

      mUIPtr->mVarOneLabel->setText(varList[0]);
      mUIPtr->mVarTwoLabel->setText(varList[1]);
   }

}

double PerformanceTestWidget::RunMaxSpeedTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunMinSpeedTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunMaxGLoadTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunMaxSustainedGLoadTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunMaxLevelFlightTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunMaxLoftedRangeTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunMaxSustainedPitchAngleTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunMaxRollRateTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunTimeToDampRollTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunMaxPitchRateTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunTimeToDampPitchTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunMaxYawRateTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

double PerformanceTestWidget::RunTimeToDampYawTest()
{
   if (mProgressTimer)
   {
      mProgressTimer->start(25);
      mUIPtr->mTestProgressBar->setMaximum(0);
      mUIPtr->mTestProgressBar->setValue(-1);
      mUIPtr->mStartTestButton->setText("Running Test");
      mUIPtr->mStartTestButton->setDisabled(true);
      mUIPtr->mResultLineEdit->setText("");
   }

   // Note: This will return a real value after implementing the performance calculation function
   return 0.0;
}

} // namespace Designer
