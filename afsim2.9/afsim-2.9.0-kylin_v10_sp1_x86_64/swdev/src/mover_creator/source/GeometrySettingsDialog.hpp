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

#ifndef GEOMETRY_SETTINGS_DIALOG_HPP
#define GEOMETRY_SETTINGS_DIALOG_HPP

#include <QDialog>

class QListWidgetItem;
namespace Ui
{
   class GeometrySettingsDialog;
}

namespace Designer
{
   class GeometrySettingsDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit GeometrySettingsDialog(QWidget*        aParent,
                                         Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~GeometrySettingsDialog();
         void showEvent(QShowEvent* aEvent);
         void SetTestAltitude(double aAltitude);
         void SetTestMach(double aMach);
         void SetMaxMach(double aMach);
         void SetMinAlpha(double aAlpha);
         void SetMaxAlpha(double aAlpha);
         void SetMaxBeta(double aBeta);
         void SetAlphaSymmetry(bool aSymmetric);
         void SetBetaSymmetry(bool aSymmetric);

         double GetAltitude();
         double GetTestMach();
         double GetMaxMach();
         double GetMinAlpha();
         double GetMaxAlpha();
         double GetMaxBeta();
         bool   GetAlphaSymmetry();
         bool   GetBetaSymmetry();

      private slots:
         void PageChanged(QListWidgetItem* aCurrent, QListWidgetItem* aPrevious);


      signals:
         void MoveToCG_X();
         void MoveToCG_Z();
         void MoveToCG_XZ();
         void MoveToCG_XYZ();

         void AlphaIncrease();
         void AlphaDecrease();
         void BetaIncrease();
         void BetaDecrease();
         void YawRateIncrease();
         void YawRateDecrease();
         void PitchRateIncrease();
         void PitchRateDecrease();
         void RollRateIncrease();
         void RollRateDecrease();
         void SpeedIncrease();
         void SpeedDecrease();
         void SurfaceControlAngleIncrease();
         void SurfaceControlAngleDecrease();


      private:
         Ui::GeometrySettingsDialog* mUIPtr;
   };
}
#endif // !GEOMETRY_SETTINGS_DIALOG_HPP



