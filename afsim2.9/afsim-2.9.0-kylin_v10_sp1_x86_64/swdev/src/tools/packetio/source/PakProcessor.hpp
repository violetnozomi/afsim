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

#ifndef PAKPROCESSOR_HPP
#define PAKPROCESSOR_HPP

#include "packetio_export.h"

#include <list>
#include <string>
#include <vector>

#include "PakI.hpp"
#include "PakO.hpp"
#include "UtCallback.hpp"
#include "UtMemory.hpp"
class PakPacket;
class PakSocketIO;
class PakHeader;

namespace PakProcessorDetail
{
// These structs are used to create function pointers.

// Creates a new packet
template<typename PKT_TYPE>
struct NewBind
{
   static PakPacket* NewPacket() { return new PKT_TYPE; }
};

// Calls Serialize() on a packet
template<typename PKT_TYPE, typename SER_CLASS>
struct SerializeBind
{
   static void Serialize(PakPacket& aPkt, SER_CLASS& aBuff) { ((PKT_TYPE&)aPkt).Serialize(aBuff); }
};

template<typename BASE_TYPE>
struct PacketBaseClassId
{
   static const int cPACKET_ID = BASE_TYPE::cPACKET_ID;
};

template<>
struct PacketBaseClassId<bool>
{
   static const int cPACKET_ID = -1;
};

} // namespace PakProcessorDetail

/** Provides a mechanism for receiving packets and doing callbacks

   Each packet you want to send or receive needs to be registered using:
      RegisterPacket()

   Callbacks can be created using Connect()

   ReadPacket() will read any incoming registered packets.

   ProcessPacket() will call any corresponding callbacks
*/
class PACKETIO_EXPORT PakProcessor
{
public:
   class PacketCallbackList : public UtCallbackList
   {
      typedef UtCallbackN<void(PakPacket&)> GenericCallbackType;

   public:
      virtual void Call(PakPacket& aPkt)
      {
         for (IterType it = mCallbackList.begin(); it != mCallbackList.end();)
         {
            static_cast<GenericCallbackType*>(*(it++))->operator()(aPkt);
         }
      }
      void Connect(UtCallback* aCallbackPtr) { UtCallbackList::ConnectP(aCallbackPtr); }
   };

   template<class T>
   class TPacketCallbackList : public PacketCallbackList
   {
      typedef UtCallbackN<void(T&)> CallbackType;

   public:
      void Call(PakPacket& aPkt) override
      {
         for (IterType it = mCallbackList.begin(); it != mCallbackList.end();)
         {
            static_cast<CallbackType*>(*(it++))->operator()(static_cast<T&>(aPkt));
         }
      }

   protected:
   };

   typedef void (*ReadFnPtr)(PakPacket& aPkt, PakI& aBuff);
   typedef void (*WriteFnPtr)(PakPacket& aPkt, PakO& aBuff);
   typedef PakPacket* (*NewFnPtr)();

   //! Contains information about a registered packet.
   class PacketInfo
   {
   public:
      PacketInfo(int aPacketId, std::string aPacketName, PacketCallbackList* aCallbackListPtr, bool aIsUndefined);

      void SetBasePacketId(int aPacketId) { mBasePacketID = aPacketId; }
      void ConnectSpecific(UtCallback* aCallback);
      void ConnectGeneric(UtCallback* aCallback);
      int  GetBasePacketId() const { return mBasePacketID; }

      ~PacketInfo();
      int                GetPacketId() { return mPacketID; }
      PakPacket*         GetNewPacket();
      const std::string& GetPacketName() { return mPacketName; }
      void               Call(PakPacket& aPkt);
      bool               IsUndefinedPacket();

      ReadFnPtr  mReadFn;
      WriteFnPtr mWriteFn;
      NewFnPtr   mNewFn;

   protected:
      int                 mPacketID;
      bool                mUnknownPacket;
      std::string         mPacketName;
      PacketCallbackList* mSpecificCallbackList;
      PacketCallbackList  mGenericCallbackList;
      bool                mIsUndefinedPacket;
      int                 mBasePacketID;
   };

   PakProcessor();
   ~PakProcessor();

   PakPacket* ReadPacket(PakSocketIO& aIO);

   PakPacket* ReadPacket(char* aBuffer);

   PakPacket* ReadPacket(PakI& aInput, PakHeader& aHeader);

   void ProcessPacket(PakPacket* aPkt, bool aDoCleanup = false);


   //! Subscribe to a PakPacket with a method pointer
   //! The function must take the PakPacket type as a reference variable
   //! @param aFuncPtr  A pointer to the member function
   //! @param aThisPtr  The member's class 'this' pointer
   template<class C, class T>
   std::unique_ptr<UtCallbackN<void(T&)>> Connect(void (C::*aFuncPtr)(T&), C* aThisPtr)
   {
      auto newCallback = ut::make_unique<UtCallbackN<void(T&)>>(UtStd::Bind(aFuncPtr, aThisPtr));
      SubscribeP(T::cPACKET_ID, newCallback.get(), true);
      return newCallback;
   }

   //! Subscribe to a PakPacket with a static function pointer
   //! The function must take the PakPacket type as a reference variable
   //! @param aFuncPtr  A pointer to the static function
   template<typename T>
   std::unique_ptr<UtCallbackN<void(T&)>> Connect(void (*aFuncPtr)(T&))
   {
      auto newCallback = ut::make_unique<UtCallbackN<void(T&)>>(UtStd::Bind(aFuncPtr));
      SubscribeP(T::cPACKET_ID, newCallback.get(), true);
      return newCallback;
   }

   //! Subscribe to a PakPacket with a static function pointer
   //! The function must take PakPacket as a reference variable
   //! @param aPacketId The packet's ID
   //! @param aFuncPtr  A pointer to the member function
   //! @param aThisPtr  The member's class 'this' pointer
   template<class C>
   std::unique_ptr<UtCallbackN<void(PakPacket&)>> Connect(int aPacketId, void (C::*aFuncPtr)(PakPacket&), C* aThisPtr)
   {
      auto newCallback = ut::make_unique<UtCallbackN<void(PakPacket&)>>(UtStd::Bind(aFuncPtr, aThisPtr));
      SubscribeP(aPacketId, newCallback.get(), false);
      return newCallback;
   }

   //! Subscribe to a PakPacket with a static function pointer
   //! The function must take PakPacket as a reference variable
   //! @param aPacketId The packet's ID
   //! @param aFuncPtr  A pointer to the static function
   std::unique_ptr<UtCallbackN<void(PakPacket&)>> Connect(int aPacketId, void (*aFuncPtr)(PakPacket&))
   {
      auto newCallback = ut::make_unique<UtCallbackN<void(PakPacket&)>>(UtStd::Bind(aFuncPtr));
      SubscribeP(aPacketId, newCallback.get(), false);
      return newCallback;
   }

   //! Subscribe to a PakPacket with a static function pointer
   //! The function must take PakPacket as a reference variable
   //! @param aPacketId The packet's ID
   //! @param aFunc     The reference to the static function
   std::unique_ptr<UtCallbackN<void(PakPacket&)>> Connect(int aPacketId, const std::function<void(PakPacket&)>& aFunc)
   {
      auto newCallback = ut::make_unique<UtCallbackN<void(PakPacket&)>>(aFunc);
      SubscribeP(aPacketId, newCallback.get(), false);
      return newCallback;
   }

   template<typename T>
   std::unique_ptr<UtCallbackN<void(T&)>> Connect(const std::function<void(T&)>& aFunc)
   {
      auto newCallback = ut::make_unique<UtCallbackN<void(T&)>>(aFunc);
      SubscribeP(T::cPACKET_ID, newCallback.get(), false);
      return newCallback;
   }

   //! Register a packet to be received.
   //! @param aPacketName The human readable name given to the packet
   //! @param aPktPtr     A pointer to a packet of the type you want to register
   //!                    This will be deleted when PakProcessor is destroyed
   template<class PKT_TYPE>
   void RegisterPacket(const std::string& aPacketName, PKT_TYPE* aPktPtr)
   {
      NotAPacketTest(*aPktPtr);
      PacketInfo* info = RegisterPacketP(PKT_TYPE::cPACKET_ID,
                                         aPacketName,
                                         new TPacketCallbackList<PKT_TYPE>(),
                                         false,
                                         PakProcessorDetail::PacketBaseClassId<typename PKT_TYPE::BaseType>::cPACKET_ID);
      DefinePacketFunctions(aPktPtr, info);
      delete aPktPtr;
   }

   template<class PKT_TYPE>
   void RegisterPacket(int aPacketId, const std::string& aPacketName, PKT_TYPE* aPktPtr, bool aIsUndefined = false)
   {
      NotAPacketTest(*aPktPtr);
      PacketInfo* info = RegisterPacketP(aPacketId,
                                         aPacketName,
                                         new TPacketCallbackList<PKT_TYPE>(),
                                         aIsUndefined,
                                         PakProcessorDetail::PacketBaseClassId<typename PKT_TYPE::BaseType>::cPACKET_ID);
      DefinePacketFunctions(aPktPtr, info);
      delete aPktPtr;
   }


   PacketInfo* GetPacketInfo(int aPacketId) { return mPacketData[aPacketId]; }

protected:
   PacketInfo* RegisterPacketP(int                 aPacketId,
                               const std::string   aPacketName,
                               PacketCallbackList* aCallbackListPtr,
                               bool                aIsUndefined,
                               int                 aPacketBaseTypeId);

   // Create function pointers for the Serialize methods, and new
   template<class PKT_TYPE>
   void DefinePacketFunctions(PKT_TYPE* /*Unused*/, PacketInfo* aInfoPtr)
   {
      aInfoPtr->mReadFn  = &PakProcessorDetail::SerializeBind<PKT_TYPE, PakI>::Serialize;
      aInfoPtr->mWriteFn = &PakProcessorDetail::SerializeBind<PKT_TYPE, PakO>::Serialize;
      aInfoPtr->mNewFn   = &PakProcessorDetail::NewBind<PKT_TYPE>::NewPacket;
   }

   void SubscribeP(int aPacketId, UtCallback* aCallbackPtr, bool aIsSpecific);

   // If this call fails, a non-packet object is being registered.
   void NotAPacketTest(PakPacket& /*aPkt*/) {}

   std::vector<PacketInfo*> mPacketData;
};
#endif
