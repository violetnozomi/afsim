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

#ifndef CONVERT_BRAWLER_AIRCRAFT_DIALOG_HPP
#define CONVERT_BRAWLER_AIRCRAFT_DIALOG_HPP

#include "MoverCreatorDialogBase.hpp"

namespace Ui
{
   class ConvertBrawlerAircraftDialog;
}

namespace Designer
{
   class ConvertBrawlerAircraftDialog : public MoverCreatorDialogBase
   {
         Q_OBJECT
      public:
         explicit ConvertBrawlerAircraftDialog(QWidget*        aParent,
                                               Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~ConvertBrawlerAircraftDialog();

         void showEvent(QShowEvent* aEvent) override;
         void ChangeTheme(QString aThemeName) override;

      private slots:
         void BrowseClicked(bool aClicked);
         void OkClicked(bool aClicked) override;

      private:
         Ui::ConvertBrawlerAircraftDialog* mUIPtr;
   };
}
#endif // !CONVERT_BRAWLER_AIRCRAFT_DIALOG_HPP
