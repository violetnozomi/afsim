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

#ifndef MOVER_CREATOR_ENVIRONMENT_HPP
#define MOVER_CREATOR_ENVIRONMENT_HPP

#include <QObject>

namespace Designer
{
   //MoverCreatorEnvironment is a singleton intended to provide access to file paths and
   //any other data that needs to be consistent across the entire application.
   class MoverCreatorEnvironment : public QObject
   {
      public:
         static MoverCreatorEnvironment& GetEnvironment();

         static void Create();
         static bool Exists() { return mInstancePtr != nullptr; }

         void Shutdown();

         QString GetDataDir() const { return mDataDir; }
         QString GetAFSIM_Dir() const { return mAFSIM_Dir; }
         QString GetWarlockDir() const { return mWarlockDir; }
         QString GetAppDataDir() const { return mAppDataDir; }

         //Delete all copy and move operations;
         MoverCreatorEnvironment(const MoverCreatorEnvironment& aSrc) = delete;
         MoverCreatorEnvironment(MoverCreatorEnvironment&& aSrc) = delete;
         MoverCreatorEnvironment& operator=(const MoverCreatorEnvironment& aSrc) = delete;
         MoverCreatorEnvironment& operator=(MoverCreatorEnvironment&& aSrc) = delete;

      protected:
         MoverCreatorEnvironment();
         ~MoverCreatorEnvironment() override;

      private:

         static MoverCreatorEnvironment* mInstancePtr;

         QString mDataDir;
         QString mAFSIM_Dir;
         QString mWarlockDir;
         QString mAppDataDir;
   };
}
#endif // !MOVER_CREATOR_ENVIRONMENT_HPP
