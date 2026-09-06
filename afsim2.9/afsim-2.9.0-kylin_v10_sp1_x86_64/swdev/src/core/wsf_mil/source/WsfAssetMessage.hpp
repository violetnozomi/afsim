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

#ifndef WSFASSETMESSAGE_HPP
#define WSFASSETMESSAGE_HPP

#include "wsf_mil_export.h"

#include <memory>

#include "UtCloneablePtr.hpp"
class UtScriptClass;
class UtScriptTypes;
#include "WsfAssetPerception.hpp"
#include "WsfMessage.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! A specialization of WsfMessage that represents an asset (WsfAssetPerception)
//! to be transmitted over a communications network.
class WSF_MIL_EXPORT WsfAssetMessage : public WsfMessage
{
public:
   WsfAssetMessage();
   WsfAssetMessage(WsfPlatform* aPlatformPtr); // must use SetAsset() if you construct like this
                                               // WsfAssetMessage(WsfPlatform* aPlatformPtr, WsfAssetPerception& aAsset);
   WsfAssetMessage(WsfPlatform* aPlatformPtr, std::unique_ptr<WsfAssetPerception> aAssetPtr);

   WsfAssetMessage(const WsfAssetMessage& aSrc) = default;
   WsfAssetMessage(WsfAssetMessage&&)           = default;
   WsfAssetMessage& operator                    =(const WsfAssetMessage& aSrc);
   WsfAssetMessage& operator=(WsfAssetMessage&&) = default;

   ~WsfAssetMessage() override = default;

   //! Clone this object by creating an identical copy and returning a pointer to it.
   //! @returns A pointer to the cloned object.
   WsfMessage* Clone() const override { return new WsfAssetMessage(*this); }

   const char* GetScriptClassName() const override;

   static UtScriptClass* CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! Return the pointer to the asset
   //! @returns a pointer to the asset contained in the message
   //! @note This pointer is valid only during the processing of the message.
   //! If the information is to be saved then the asset must be cloned.
   WsfAssetPerception* GetAsset() const { return mAssetPtr.get(); }

   void SetAsset(WsfAssetPerception* aAssetPtr);

   bool CanBeReplacedBy(const WsfMessage& aMessage) const override;

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mAssetPtr;
   }

private:
   ut::CloneablePtr<WsfAssetPerception> mAssetPtr;
};


// =================================================================================================
// The script interface 'class'
// =================================================================================================

class WSF_MIL_EXPORT WsfScriptAssetMessageClass : public WsfScriptMessageClass
{
public:
   WsfScriptAssetMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(SetAsset);
   UT_DECLARE_SCRIPT_METHOD(Asset);
};


#endif
