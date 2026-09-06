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
#ifndef INPUTENUMERATION_HPP
#define INPUTENUMERATION_HPP

#include <list>
#include <memory>
#include <string>

class UtInput;

//! Reads values from UtInput to set an enumeration.
class InputEnumeration
{
public:
   InputEnumeration();
   InputEnumeration(const InputEnumeration& aSrc);
   ~InputEnumeration();
   void Clear();

   void Add(int aValue, const char* aString);
   void Add(int aValue, const std::string& aString);

   bool ProcessInput(const std::string& aCommand, UtInput& aInput, int& aEnumValue);

   typedef std::pair<std::string, int> Pair;
   typedef std::list<Pair>             EnumList;

   // Allow the use the << operator to define the enumerations
   // Ex:   inputClass << cAIR << "air" << cLAND << "land";
   class Inserter
   {
      Inserter(int aEnumVal, InputEnumeration* aEnumInput)
         : mEnumVal(aEnumVal)
         , mEnumInput(aEnumInput)
      {
      }

   public:
      friend class InputEnumeration;
      InputEnumeration& operator<<(const char* aStrPtr)
      {
         mEnumInput->Add(mEnumVal, aStrPtr);
         return *mEnumInput;
      }
      InputEnumeration& operator<<(const std::string& aString)
      {
         mEnumInput->Add(mEnumVal, aString);
         return *mEnumInput;
      }

   private:
      void              operator=(const Inserter&);
      int               mEnumVal;
      InputEnumeration* mEnumInput;
   };

   Inserter operator<<(int aEnumVal) { return Inserter(aEnumVal, this); }

private:
   EnumList&                 GetList();
   std::shared_ptr<EnumList> mValueList;
};

#endif
