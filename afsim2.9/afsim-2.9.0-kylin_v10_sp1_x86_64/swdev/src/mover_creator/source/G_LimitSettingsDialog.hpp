// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef GLIMITSETTINGSDIALOG_HPP
#define GLIMITSETTINGSDIALOG_HPP

#include <memory>

#include <QDialog>

#include "G_LimitSettings.hpp"
#include "ui_G_LimitSettingsDialog.h"

class QAbstractButton;

class G_LimitSettings;

namespace Designer
{
   class G_LimitSettingsDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit G_LimitSettingsDialog(QWidget* aParentPtr = nullptr);
         ~G_LimitSettingsDialog() override = default;

         void SetSettings(const G_LimitSettings& aSettings);
         G_LimitSettings GetSettings() const;

      public slots:
         void ButtonClicked(QAbstractButton* aButton);

      signals:
         void SettingsAccepted(G_LimitSettings& aSettings);

      private:
         Ui::G_LimitSettingsDialog mUI;
         G_LimitSettings           mSettings;
   };
}

#endif // GLIMITSETTINGSDIALOG_HPP
