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

#include "Argo8Model.hpp"

#include <algorithm>
#include <iostream>


std::map<std::string, UtDynamicLibrary*> Argo8Model::sLibraryPtrMap;
std::map<std::string, int> Argo8Model::sNumInstancesMap;

Argo8Model::Argo8Model()
   : mUpdateInterval(0.01),
     iModel(-1)
{
   // Set function pointers to NULL, set in Initialize
   pProcCreateModel          = NULL;
   pProcFreeModel            = NULL;
   pProcInitializeModel      = NULL;
   pProcSetInput             = NULL;
   pProcSetParameter         = NULL;
   pProcSetParameterString   = NULL;
   pProcUpdateModel          = NULL;
   pProcGetOutput            = NULL;
   pProcSetInputArray        = NULL;
   pProcSetParameterArray    = NULL;
   pProcGetOutputArray       = NULL;
   pProcEnableLogging        = NULL;
   pProcDisableLogging       = NULL;
}

Argo8Model::Argo8Model(const Argo8Model& aSrc)
   : mUpdateInterval(aSrc.mUpdateInterval),
     iModel(-1)
{
   // Set function pointers to NULL, set in Initialize
   pProcCreateModel          = NULL;
   pProcFreeModel            = NULL;
   pProcInitializeModel      = NULL;
   pProcSetInput             = NULL;
   pProcSetParameter         = NULL;
   pProcSetParameterString   = NULL;
   pProcUpdateModel          = NULL;
   pProcGetOutput            = NULL;
   pProcSetInputArray        = NULL;
   pProcSetParameterArray    = NULL;
   pProcGetOutputArray       = NULL;
   pProcEnableLogging        = NULL;
   pProcDisableLogging       = NULL;
}

//virtual
Argo8Model::~Argo8Model()
{
   std::string modelName = GetModelName();
   if (sLibraryPtrMap[modelName] != 0)
   {
      if (iModel >= 0)
      {
         // Terminate
         pProcFreeModel(iModel);
         --sNumInstancesMap[modelName];
      }

      if (sNumInstancesMap[modelName] <= 0)
      {
         delete sLibraryPtrMap[modelName];
         sLibraryPtrMap[modelName] = 0;
      }
   }
}

//virtual
Argo8Model* Argo8Model::Clone() const
{
   return new Argo8Model(*this);
}

//virtual
const std::string Argo8Model::GetModelName()
{
   // Override this with proper return in class that is inheriting
   std::cout << "***ERROR: Must Specify the ARGO missile model name in the ArgoModel file for the missile." << std::endl;
   return "";
}

//virtual
const std::string Argo8Model::GetLibraryName()
{
   // Override this with proper return in class that is inheriting
   std::cout << "***ERROR: Must Specify the ARGO missile library name in the ArgoModel file for the missile." << std::endl;
   return "";
}

//virtual
void Argo8Model::SetModelLibraryName(std::string aLibraryName)
{
}



bool Argo8Model::Initialize()
{
   if ((GetModelName() == "") || (GetLibraryName() == ""))
   {
      std::cout << "Failed to create ARGO model due to model name not being specified." << std::endl;
      return false;
   }

   // Object creation section
   iModel = pProcCreateModel();

   if (iModel < 0)
   {
      std::cout << "Failed to create ARGO model instance from model name " << GetModelName() << std::endl;
      return false;
   }
   ++sNumInstancesMap[GetModelName()];

   // Object initialization section
   return (pProcInitializeModel(iModel) != 0);
}

bool Argo8Model::LoadModel()
{
   if ((GetModelName() == "") || (GetLibraryName() == ""))
   {
      std::cout << "Failed to load ARGO model due to library name not being specified." << std::endl;
      return false;
   }

   // Initialize function pointers to NULL
   pProcCreateModel          = NULL;
   pProcFreeModel            = NULL;
   pProcInitializeModel      = NULL;
   pProcSetInput             = NULL;
   pProcSetParameter         = NULL;
   pProcSetParameterString   = NULL;
   pProcUpdateModel          = NULL;
   pProcGetOutput            = NULL;
   pProcSetInputArray        = NULL;
   pProcSetParameterArray    = NULL;
   pProcGetOutputArray       = NULL;
   pProcEnableLogging        = NULL;
   pProcDisableLogging       = NULL;

   std::string errorMessage;
   #ifdef _WIN32
   std::string libName = GetLibraryName() + ".dll";
   #elif defined __unix__
   std::string libName = GetLibraryName() + ".so";
   #endif // defined

   UtPath path = UtPath::GetExePath();
   path.Up();
   std::string fileToLoad = path.GetSystemPath();
   fileToLoad.append("/");
   fileToLoad.append(libName);
   path = UtPath(fileToLoad);

   if ((sLibraryPtrMap.find(libName) == sLibraryPtrMap.end()) ||
       (sLibraryPtrMap[libName] == 0))
   {
      sLibraryPtrMap[libName] = UtDynamicLibrary::Load(path.GetSystemPath(), errorMessage);
   }
   UtDynamicLibrary* libraryPtr = sLibraryPtrMap[libName];
   if (libraryPtr != 0)
   {
       pProcCreateModel          = (Argo8ModelDefs::CreateModel_fn)(libraryPtr->GetSymbol("CreateModel"));
       pProcFreeModel            = (Argo8ModelDefs::FreeModel_fn)(libraryPtr->GetSymbol("FreeModel"));
       pProcInitializeModel      = (Argo8ModelDefs::InitializeModel_fn)(libraryPtr->GetSymbol("InitializeModel"));
       pProcSetInput             = (Argo8ModelDefs::SetInput_fn)(libraryPtr->GetSymbol("SetInput"));
       pProcSetParameter         = (Argo8ModelDefs::SetParameter_fn)(libraryPtr->GetSymbol("SetParameter"));
       pProcSetParameterString   = (Argo8ModelDefs::SetParameterString_fn)(libraryPtr->GetSymbol("SetParameterString"));
       pProcUpdateModel          = (Argo8ModelDefs::UpdateModel_fn)(libraryPtr->GetSymbol("UpdateModel"));
       pProcGetOutput            = (Argo8ModelDefs::GetOutput_fn)(libraryPtr->GetSymbol("GetOutput"));
       pProcSetInputArray        = (Argo8ModelDefs::SetInputArray_fn)(libraryPtr->GetSymbol("SetInputArray"));
       pProcSetParameterArray    = (Argo8ModelDefs::SetParameterArray_fn)(libraryPtr->GetSymbol("SetParameterArray"));
       pProcGetOutputArray       = (Argo8ModelDefs::GetOutputArray_fn)(libraryPtr->GetSymbol("GetOutputArray"));
       pProcEnableLogging        = (Argo8ModelDefs::EnableLogging_fn)(libraryPtr->GetSymbol("EnableLogging"));
       pProcDisableLogging       = (Argo8ModelDefs::DisableLogging_fn)(libraryPtr->GetSymbol("DisableLogging"));
   }

   if (!(libraryPtr && pProcCreateModel          &&
                       pProcFreeModel            &&
                       pProcInitializeModel      &&
                       pProcSetInput             &&
                       pProcSetParameter         &&
                       pProcSetParameterString   &&
                       pProcUpdateModel          &&
                       pProcGetOutput            &&
                       pProcSetInputArray        &&
                       pProcSetParameterArray    &&
                       pProcGetOutputArray       &&
                       pProcEnableLogging        &&
                       pProcDisableLogging
                       ))
   {
      std::cout << "Failed to load " << libName << " and all Functions" << std::endl;
      return false;
   }

   return true;
}

void Argo8Model::Update(double aSimTime)
{
   // Set inputs
   SetInputs();

   // Set parameters
   SetParams();

   // Update/Step Model
   pProcUpdateModel(iModel);

   // Set outputs
   GetOutputs();
}

void Argo8Model::SetupLogging(bool aEnabledLogging, std::string aFilename)
{
   if (! aEnabledLogging)
   {
      pProcDisableLogging(iModel);
   }
   else if (! aFilename.empty())
   {
      pProcEnableLogging(iModel, const_cast<char*>(aFilename.c_str()));
      std::cout << "ENABLING LOGGING " << aFilename << std::endl;
   }
   else
   {
      std::cout << "**** WARNING: ARGO Model: " << GetModelName() << ':' << GetLibraryName()
                << " logging filename was not set." << std::endl;
   }
}

//virtual
void Argo8Model::SetDynamicData(DynamicData aDynamicData)
{
}
void Argo8Model::SetUpdateFlag(bool aUpdate)
{
}
//virtual
void Argo8Model::SetRailData(RailData aRailData)
{
}
void Argo8Model::DumpDynamicData(DynamicData aDynamicData)
{
   std::cout << "DUMPING ARGO DYNAMIC DATA" << std::endl;
   std::cout << "Guidance Update Position: " << aDynamicData.guidx[0] << ", " << aDynamicData.guidx[1] << ", " << aDynamicData.guidx[2] << ", " << std::endl;
   std::cout << "Guidance Update Velocity: " << aDynamicData.guidv[0] << ", " << aDynamicData.guidv[1] << ", " << aDynamicData.guidv[2] << ", " << std::endl;
   std::cout << "Target Truth Position: " << aDynamicData.tgt_truthx[0] << ", " << aDynamicData.tgt_truthx[1] << ", " << aDynamicData.tgt_truthx[2] << ", " << std::endl;
   std::cout << "Target Truth Velocity: " << aDynamicData.tgt_truthv[0] << ", " << aDynamicData.tgt_truthv[1] << ", " << aDynamicData.tgt_truthv[2] << ", " << std::endl;
}

void Argo8Model::DumpRailData(RailData aRailData)
{
   std::cout << "DUMPING ARGO INITIAL DATA" << std::endl;
   std::cout << "Target Size: " << aRailData.tgt_size << std::endl;
   std::cout << "Cue Position: " << aRailData.cue_pos[0] << ", " << aRailData.cue_pos[1]<< ", " << aRailData.cue_pos[2] << std::endl;
   std::cout << "Cue Velocity: " << aRailData.cue_vel[0] << ", " << aRailData.cue_vel[1] << ", " << aRailData.cue_vel[2] << std::endl;
   std::cout << "Launch Position: " << aRailData.lnch_pos[0] << ", " << aRailData.lnch_pos[1] << ", " << aRailData.lnch_pos[2] << std::endl;
   std::cout << "Launch Velocity: " << aRailData.lnch_vel[0] << ", " << aRailData.lnch_vel[1] << ", " << aRailData.lnch_vel[2] << std::endl;
   std::cout << "Euler Angles: " << aRailData.phi << ", " << aRailData.theta << ", "<< aRailData.psi << std::endl;
   std::cout << "Body Rotational Rates:" << aRailData.rot_rates[0] << ", " << aRailData.rot_rates[1] << ", " << aRailData.rot_rates[2] << std::endl;
}
//virtual
void Argo8Model::GetFlyout(Argo8Output& aOutput)
{
}
void Argo8Model::GetSeekerState(bool& aSeeker)
{
}
void Argo8Model::GetFuzeData(FuzeData &aFuzeData)
{
}
