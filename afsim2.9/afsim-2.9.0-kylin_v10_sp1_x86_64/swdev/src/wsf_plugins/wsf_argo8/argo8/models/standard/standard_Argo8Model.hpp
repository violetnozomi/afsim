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

#ifndef STANDARD_ARGOMODEL_HPP
#define STANDARD_ARGOMODEL_HPP

#include <map>

#include "../../source/Argo8Structs.h"
#include "../../source/Argo8Model.hpp"

#include "standard_enums.h"

class standard_Argo8Model : public Argo8Model
{
   public:

      standard_Argo8Model();
      standard_Argo8Model(const Argo8Model& aSrc);
      virtual ~standard_Argo8Model();
      virtual Argo8Model* Clone() const;
      virtual void SetModelLibraryName(std::string aLibraryName);
      virtual const std::string GetModelName();
      virtual const std::string GetLibraryName();

      virtual bool Initialize();

      virtual void SetInputs();
      virtual void GetOutputs();

      // Mutators
      virtual void SetRailData(RailData aRailData);
      virtual void SetUpdateFlag(bool aUpdate);
      virtual void SetDynamicData(DynamicData aDynamicData);
      //Accessors
      virtual void GetFlyout(Argo8Output& aOutput);
      virtual void GetSeekerState(bool& aSeeker);
      virtual void GetFuzeData(FuzeData &aFuzeData);
private:

      std::string modelName;
      std::string libraryName;
      // Define variables to support the model input data structure
      double inTriggerPress;
      double intgt_size;
      double incue_pos[3];
      double incue_vel[3];
      double inlnch_pos[3];
      double inlnch_vel[3];
      double inphi;
      double intheta;
      double inpsi;
      double insep_acc[3];
      double inrot_rates[3];
      double infoffset[3];
      double intgt_truthx[3];
      double intgt_truthv[3];
      double inupdate;
      double inguidx[3];
      double inguidv[3];
      double ininFOV;


      // Define variables to support the model output data structure
      double outmslfzd;
      double outproxfuze;
      double outmslx_est[3];
      double outmslv_est[3];
      double outmsla_est[3];
      double outphidot;
      double outthedot;
      double outpsidot;
      double outflytime;
      double outmslplume;
      double outweight;
      double outmslx[3];
      double outmslv[3];
      double outmsla[3];
      double outspecforce[3];
      double outphi;
      double outthe;
      double outpsi;
      double outmslmach;
      double outmslalpha;
      double outtgtpos[3];
      double outdetached;
      double outignitemotor; //unused
      double outsafety;      //unused
      double outpitch_gimang;
      double outyaw_gimang;
      double outactivate_skr;
      double outbepsy; //unused
      double outbepsz; //unused
      double outvertgc; //unused
      double outhorgc; //unused
      double outalpha; //unused
      double outbeta; //unused
      double outalphadot; //unused
      double outbetadot; //unused
      double outselfdestruct;
      double outmaxtime;
      double outfailedlaunch;
      double outlowmslmach;
      double outlowgavail;
      double outlowclosingvelocity;
      double outgimballimit;
      double outgavail;
      double outthrust;

};

#endif

