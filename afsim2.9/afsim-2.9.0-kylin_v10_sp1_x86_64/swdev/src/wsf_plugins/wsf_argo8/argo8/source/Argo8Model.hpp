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

#ifndef ARGO8MODEL_HPP
#define ARGO8MODEL_HPP

#include <map>
#include <string>

#include "UtDynamicLibrary.hpp"
#include "UtPath.hpp"

#include "argo8_export.h"
#include "Argo8ModelDefs.h"
#include "Argo8Structs.h"

class ARGO8_EXPORT Argo8Model
{
   public:

      Argo8Model();
      Argo8Model(const Argo8Model& aSrc);
      virtual ~Argo8Model();

      virtual Argo8Model* Clone() const;

      virtual bool Initialize();

      bool LoadModel();

      int GetInstance() { return iModel; }

      void Update(double aSimTime);

      virtual const std::string GetModelName();
      virtual const std::string GetLibraryName();

      virtual void SetModelLibraryName(std::string aLibraryName);

      double GetUpdateInterval() const { return mUpdateInterval; }
      double GetMaxLaunchAngle() const { return mMaxLaunchAngle; }
      double GetMaximumSpeed() const   { return mMaximumSpeed; }

      void SetupLogging(bool aLoggingEnabled, std::string aFilename);
      void DumpRailData(RailData aRailData);
      void DumpDynamicData(DynamicData aDynamicData);

      virtual void SetInputs()  {}
      virtual void SetParams()  {}
      virtual void GetOutputs() {}

      // Mutators
      virtual void SetRailData(RailData aRailData);
      virtual void SetUpdateFlag(bool aUpdate);
      virtual void SetDynamicData(DynamicData aDynamicData);
      //Accessors
      virtual void GetFlyout(Argo8Output& aOutput);
      virtual void GetSeekerState(bool& aSeeker);
      virtual void GetFuzeData(FuzeData &aFuzeData);
      protected:

         // Define function pointers
         Argo8ModelDefs::CreateModel_fn          pProcCreateModel;
         Argo8ModelDefs::FreeModel_fn            pProcFreeModel;
         Argo8ModelDefs::InitializeModel_fn      pProcInitializeModel;
         Argo8ModelDefs::SetInput_fn             pProcSetInput;
         Argo8ModelDefs::SetParameter_fn         pProcSetParameter;
         Argo8ModelDefs::SetParameterString_fn   pProcSetParameterString;
         Argo8ModelDefs::UpdateModel_fn          pProcUpdateModel;
         Argo8ModelDefs::GetOutput_fn            pProcGetOutput;
         Argo8ModelDefs::SetInputArray_fn        pProcSetInputArray;
         Argo8ModelDefs::SetParameterArray_fn    pProcSetParameterArray;
         Argo8ModelDefs::GetOutputArray_fn       pProcGetOutputArray;
         Argo8ModelDefs::EnableLogging_fn        pProcEnableLogging;
         Argo8ModelDefs::DisableLogging_fn       pProcDisableLogging;

      private:

         typedef std::map<std::string, UtDynamicLibrary*> LibraryPtrMap;
         typedef std::map<std::string, int> NumInstancesMap;

         double                 mUpdateInterval;   //!< The required update interval
         double                 mMaxLaunchAngle;   //!< The maximum launch angle
         double                 mMaximumSpeed;     //!< The maximum Speed

         bool                   mEnableLogging;
         std::string            mLogFilePath;      //!< Log file path, filename set in model

         int                    iModel;            //!< Model instance Id within the library

         static LibraryPtrMap   sLibraryPtrMap;    //!< Map of the model name to the library pointers
         static NumInstancesMap sNumInstancesMap;  //!< Map of the number of instance of each model present

         // Define variables to support the model input data structure
};

#endif
