// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************

#ifndef ARGO8MOVER_HPP
#define ARGO8MOVER_HPP

#include <string>

#include "argo8_export.h"
#include "Argo8Model.hpp"
#include "Argo8Structs.h"

//! Represents a missile systems.
//!
//! This class is basically a collection of Argo objects that represent
//! all of the various complex air-to-air missile system.

class ARGO8_EXPORT Argo8Missile
{
   public:

      Argo8Missile();
      Argo8Missile(const Argo8Missile& aSrc);
      virtual ~Argo8Missile();

      //! @name Static methods to maintain the list of AMBER model's type' objects.
      //@{
      static void AddMissileModelType(const std::string& aTypeName,
                                    Argo8Model*        aTypePtr);
      static Argo8Model* CloneMissileModelType(const std::string& aTypeName);
      static Argo8Model* FindMissileModelType(const std::string& aTypeName);

      static void ClearTypes();
      static void CreateTypes();
      static void ResetTypes();
      //@}

      void Update(double aSimTime);

      virtual Argo8Model* GenerateMissileModel(const std::string aRadarMoverType);

      bool Initialize(const std::string& aMoverModelType,
                      const std::string& aLibraryName);

      void SetupLogging(bool aLoggingEnabled, std::string aFilename = "");

      double GetUpdateInterval();

      // Mutators
      void SetMissileLaunchTime(double aLaunchTime);
      void SetRailData(bool aTriggerPress,
                                      double atgt_size,
                                      double acue_pos[3],
                                      double acue_vel[3],
                                      double alnch_pos[3],
                                      double alnch_vel[3],
                                      double aphi,
                                      double atheta,
                                      double apsi,
                                      double asep_acc[3],
                                      double arot_rates[3],
                                      double afoffset[3]);
      void SetUpdateFlag(bool aUpdate);
      void SetDynamicData(double atgt_truthx[3],
                                     double atgt_truthv[3],
                                     bool aupdate,
                                     double aguidx[3],
                                     double aguidv[3],
                                     bool ainFOV);

      void SetMissileState(MissileState aMissileState) { mMissileState = aMissileState; }

      // Accessors
      void GetLocationESD(double alocESD[3]);
      void GetgAvail(double& aGavail);
      void GetMissileState(double aSimTime,
                           double alocNED[3],
                           double avelNED[3],
                           double aaccNED[3],
                           double& aPsi,
                           double& aTheta,
                           double& aPhi);

      void GetFlyout(bool amslfzd,
                              bool aproxfuze,
                              double amslx_est[3],
                              double amslv_est[3],
                              double amsla_est[3],
                              double aphidot,
                              double athedot,
                              double apsidot,
                              double aflytime,
                              bool amslplume,
                              double aweight,
                              double amslx[3],
                              double amslv[3],
                              double amsla[3],
                              double aspecforce[3],
                              double aphi,
                              double athe,
                              double apsi,
                              double amslmach,
                              double amslalpha,
                              double atgtpos[3],
                              bool adetached,
                              bool aignitemotor,
                              bool asafety,
                              double apitch_gimang,
                              double ayaw_gimang,
                              bool aactivate_skr,
                              bool abepsy,
                              bool abepsz,
                              double avertgc,
                              double ahorgc,
                              double aalpha,
                              double abeta,
                              double aalphadot,
                              double abetadot,
                              bool aselfdestruct,
                              bool amaxtime,
                              bool afailedlaunch,
                              bool alowmslmach,
                              bool alowgavail,
                              bool alowclosingvelocity,
                              bool agimballimit,
                              double agavail,
                              double athrust);

      MissileState GetMissileState() { return mMissileState; }
      void GetSeekerState(bool& aSeeker);
      // Other
      MissileState Update(double      aSimTime,
                          int&        aFuzeStateMask,
                          std::string aMissileName = "");
      void ProcessMissileTermination(double  aSimTime,
 				                         int&    aFuzeStateMask);


   protected:

      // Struture-based Mutators
         void SetRailData(RailData aRailData);
         void SetDynamicData(DynamicData aDynamicData);
      // Struture-based Accessors
         void GetFlyout(Argo8Output& aOutput);

   private:

      Argo8Model*   mArgoModel;
      double       mLaunchTime;
      MissileState mMissileState;
      int          mFuzeStateMask;

};

#endif
