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

#include "MoverCreatorEnvironment.hpp"

#include <QApplication>
#include <QStandardPaths>

#include "UtLog.hpp"

namespace Designer
{

MoverCreatorEnvironment* MoverCreatorEnvironment::mInstancePtr = nullptr;

MoverCreatorEnvironment::MoverCreatorEnvironment()
{
   // Check if an instance already exists
   if (mInstancePtr != nullptr)
   {
      ut::log::fatal() << "Only one instance of MoverCreatorEnvironment may exist at any time. Exiting...";
      exit(1);
   }

   mDataDir = QApplication::applicationDirPath() + "/../../mover_creator/data";
   mAFSIM_Dir = QApplication::applicationDirPath() + "/mission";
   mWarlockDir = QApplication::applicationDirPath() + "/warlock";
   mAppDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

MoverCreatorEnvironment::~MoverCreatorEnvironment()
{
   mInstancePtr = nullptr;
}

MoverCreatorEnvironment& MoverCreatorEnvironment::GetEnvironment()
{
   return *mInstancePtr;
}

void MoverCreatorEnvironment::Create()
{
   if (mInstancePtr == nullptr)
   {
      mInstancePtr = new MoverCreatorEnvironment();
   }
}

void MoverCreatorEnvironment::Shutdown()
{
   if (mInstancePtr != nullptr)
   {
      delete mInstancePtr;
   }
}

} // namespace Designer
