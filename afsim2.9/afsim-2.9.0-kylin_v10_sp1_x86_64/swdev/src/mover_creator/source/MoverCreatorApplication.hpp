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

#ifndef MOVER_CREATOR_APPLICATION_HPP
#define MOVER_CREATOR_APPLICATION_HPP

#include <QApplication>
#include <QSettings>
#include "MoverCreatorMainWindow.hpp"

namespace Designer
{
   class MoverCreatorApplication : public QApplication
   {
      public:
         MoverCreatorApplication(int& argc, char* argv[]);
         ~MoverCreatorApplication() override;

         int Execute();

      private:
         MoverCreatorMainWindow* mMainDialog;
   };
}
#endif // !MOVER_CREATOR_APPLICATION_HPP
