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

#ifndef SOSM_TABLEVAR_HPP
#define SOSM_TABLEVAR_HPP

//! A helper class for specifying data to the table loaders.
class SOSM_TableVar
{
public:
   SOSM_TableVar()
      : mName("unknown")
      , mMin(-1.0E+37F)
      , mMax(1.0E+37F)
      , mScale(1.0F)
   {
   }

   SOSM_TableVar(const std::string& aName, float aMin, float aMax, float aScale)
      : mName(aName)
      , mMin(aMin)
      , mMax(aMax)
      , mScale(aScale)
   {
   }

   const std::string& Name() const { return mName; }
   float              Min() const { return mMin; }
   float              Max() const { return mMax; }
   float              Scale() const { return mScale; }

private:
   std::string mName;
   float       mMin;
   float       mMax;
   float       mScale;
};

#endif
