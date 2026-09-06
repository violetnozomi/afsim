// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFDISPDUFACTORY_HPP
#define WSFDISPDUFACTORY_HPP

#include "wsf_export.h"

#include "DisPduFactory.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisPduFactory : public DisPduFactory
{
public:
   WsfDisPduFactory(WsfDisInterface* aInterfacePtr);
   ~WsfDisPduFactory() override;

   // User overrides this method to create their own DIS classes
   DisPdu* UserCreate(DisPdu& aDisPdu, GenI& aGenI) override;

   class CreateFunction
   {
   public:
      virtual ~CreateFunction() {}
      virtual DisPdu* Create(DisPdu& aDisPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr) = 0;
   };


   template<typename T>
   class CreateFunctionT : public CreateFunction
   {
   public:
      DisPdu* Create(DisPdu& aDisPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr) override
      {
         return new T(aDisPdu, aGenI, aInterfacePtr);
      }
   };

   void SetPduCreateFunction(int aPduType, CreateFunction* aFunctionPtr);

protected:
   WsfDisInterface* mInterfacePtr;

   std::vector<CreateFunction*> mCreateFunctions;
};

#endif
