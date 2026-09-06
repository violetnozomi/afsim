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

#include "MoverCreatorApplication.hpp"
#include "MoverCreatorEnvironment.hpp"

#include "MoverCreatorWidget.hpp"
#include <QStyleFactory>
#include <QStandardPaths>

#include "mover_creator_version_defines.hpp"

namespace Designer
{

MoverCreatorApplication::MoverCreatorApplication(int& argc, char* argv[])
   : QApplication(argc, argv)
   , mMainDialog(nullptr)
{
   qApp->setOrganizationName(MOVER_CREATOR_COMPANYNAME_STR);
   qApp->setOrganizationDomain(MOVER_CREATOR_COMPANYDOMAIN_STR);
   qApp->setApplicationName(MOVER_CREATOR_PRODUCTNAME_STR);
   qApp->setStyle(QStyleFactory::create("Fusion"));
   MoverCreatorEnvironment::Create();
}

MoverCreatorApplication::~MoverCreatorApplication()
{
   MoverCreatorEnvironment::GetEnvironment().Shutdown();
   delete mMainDialog;
}

int MoverCreatorApplication::Execute()
{
   mMainDialog = &MoverCreatorMainWindow::GetInstance();
   mMainDialog->Initialize(this);
   mMainDialog->show();
   return exec();
}

} // namespace Designer
