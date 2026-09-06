// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSA_ENTITYMESSAGE_HPP
#define WSFSA_ENTITYMESSAGE_HPP

#include "wsf_air_combat_export.h"

#include <memory>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
#include "WsfSA_EntityPerception.hpp"
#include "script/WsfScriptMessageClass.hpp"

//! A specialization of WsfMessage that represents an asset (WsfSA_EntityPerception)
//! to be transmitted over a communications network.
class WSF_AIR_COMBAT_EXPORT WsfSA_EntityMessage : public WsfMessage
{
public:
   WsfSA_EntityMessage();
   WsfSA_EntityMessage(WsfPlatform* aPlatformPtr); // must use SetEntity() if you construct like this
   // WsfSA_Message(WsfPlatform* aPlatformPtr, WsfSA_EntityPerception& aEntity);
   WsfSA_EntityMessage(WsfPlatform* aPlatformPtr, WsfSA_EntityPerception* aEntityPtr);

   WsfSA_EntityMessage(const WsfSA_EntityMessage& aSrc);
   WsfSA_EntityMessage(WsfSA_EntityMessage&&) = default;
   WsfSA_EntityMessage& operator              =(const WsfSA_EntityMessage& aSrc);
   WsfSA_EntityMessage& operator=(WsfSA_EntityMessage&&) = default;

   ~WsfSA_EntityMessage() override = default;

   //! Clone this object by creating an identical copy and returning a pointer to it.
   //! @returns A pointer to the cloned object.
   WsfMessage* Clone() const override { return new WsfSA_EntityMessage(*this); }

   const char* GetScriptClassName() const override;

   static UtScriptClass* CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! Return the pointer to the asset
   //! @returns a pointer to the asset contained in the message
   //! @note This pointer is valid only during the processing of the message.
   //! If the information is to be saved then the asset must be cloned.
   WsfSA_EntityPerception* GetEntity() const { return mEntityPtr.get(); }

   void SetEntity(WsfSA_EntityPerception* aEntityPtr);

   bool CanBeReplacedBy(const WsfMessage& aMessage) const override;

   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mEntityPtr;
   }

private:
   std::unique_ptr<WsfSA_EntityPerception> mEntityPtr;
};

// =================================================================================================
// The script interface 'class'
// =================================================================================================

class WSF_AIR_COMBAT_EXPORT WsfScriptSA_EntityMessage : public WsfScriptMessageClass
{
public:
   WsfScriptSA_EntityMessage(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(SetEntity);
   UT_DECLARE_SCRIPT_METHOD(Entity);
};

#endif
