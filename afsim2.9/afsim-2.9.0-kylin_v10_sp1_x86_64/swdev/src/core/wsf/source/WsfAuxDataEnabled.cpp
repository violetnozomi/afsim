// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfAuxDataEnabled.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"

UtCallbackListN<void(const WsfAuxDataEnabled*)> WsfAuxDataEnabled::AuxDataAccessed;
UtCallbackListN<void(const WsfAuxDataEnabled*)> WsfAuxDataEnabled::AuxDataDestroyed;

WsfAuxDataEnabled& WsfAuxDataEnabled::operator=(const WsfAuxDataEnabled& aRhs)
{
   if (this != &aRhs)
   {
      mAuxData = aRhs.mAuxData;

      AuxDataAccessed(this);
   }
   return *this;
}

WsfAuxDataEnabled::~WsfAuxDataEnabled()
{
   AuxDataDestroyed(this);
}

WsfAttributeContainer& WsfAuxDataEnabled::GetAuxData()
{
   AuxDataAccessed(this);
   return mAuxData;
}

const WsfAttributeContainer& WsfAuxDataEnabled::GetAuxData() const noexcept
{
   return mAuxData;
}

const WsfAttributeContainer& WsfAuxDataEnabled::GetAuxDataConst() const noexcept
{
   return mAuxData;
}

void WsfAuxDataEnabled::SetAuxData(const WsfAttributeContainer& aAuxData)
{
   mAuxData = aAuxData;
   AuxDataAccessed(this);
}

void WsfAuxDataEnabled::DeleteAuxData()
{
   mAuxData.Clear();
   AuxDataAccessed(this);
}

void WsfAuxDataEnabled::MergeAuxData(const WsfAuxDataEnabled& aSrc)
{
   if (HasAuxData())
   {
      if (aSrc.HasAuxData())
      {
         mAuxData.Merge(aSrc.GetAuxDataConst());
         AuxDataAccessed(this);
      }
   }
   else if (aSrc.HasAuxData())
   {
      SetAuxData(aSrc.GetAuxDataConst());
      AuxDataAccessed(this);
   }
}

void WsfAuxDataEnabled::UpdateAuxData(const WsfAuxDataEnabled& aSrc)
{
   if (HasAuxData() && aSrc.HasAuxData())
   {
      mAuxData.Update(aSrc.GetAuxData());
      AuxDataAccessed(this);
   }
}

bool WsfAuxDataEnabled::HasAuxData() const
{
   return mAuxData.HasAttributes();
}

bool WsfAuxDataEnabled::ProcessInput(UtInput& aInput)
{
   bool myCommand{false};
   if (aInput.GetCommand() == "aux_data")
   {
      UtInputBlock inputBlock(aInput);
      inputBlock.ProcessInput(&GetAuxData());
      myCommand = true;
   }
   return myCommand;
}
