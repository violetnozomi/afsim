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
#include "UtPath.hpp"

int main(int argc, char* argv[])
{
   // Add library path for Qt plugins. Must be done before QApplication is created.
   // Get the executable path
   UtPath binDir = UtPath::GetExePath();
   // Trim the name of the executable
   binDir.Up();

   QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
   QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
   Designer::MoverCreatorApplication app(argc, argv);
   int retVal = app.Execute();
   return retVal;
}
