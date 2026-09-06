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

#ifndef GEN_I_HELPER_HPP
#define GEN_I_HELPER_HPP


/// \file  GenIHelper.hpp
/// \brief GenIHelper
/// \class GenIHelper GenIHelper.hpp
/// \brief This class provides an easy way to pull messages from a GenI derived class.
///
/// The use of this class is not an absolute necessity, but it does provide the
/// code most users would otherwise implement to retrieve messages from a GenI derived class.
/// Most users find it advantages to use this class.
///
/// The message set or sets used with this class must support the method:\n
/// static int MsgType::GetBaseLength()
///
/// Also a factory class with the following method must be provided to generate the messages:\n
/// MsgType* MsgFactoryType::GetMsg(GenI&);

#include "GenI.hpp"
#include "GenIFactory.hpp"
#include "GenThreadControl.hpp"
#include "UtLog.hpp"

//#define DEBUG_ON
#ifdef DEBUG_ON
#define DEBUG_IT(A) A
#else
#define DEBUG_IT(A) /* A */
#endif

template<typename MsgType, typename MsgFactoryType = GenIFactory<MsgType>>
class GenIHelper
{
public:
   /// Constructor
   /// \param aGenI             A GenI derived class that receives the messages.
   /// \param aMsgFactory       A pointer to a message factory that has a GetMsg(GenI &) function.
   /// \param aGenThreadControl A pointer to a GenThreadControl class.
   GenIHelper(GenI& aGenI, MsgFactoryType* aMsgFactory, GenThreadControl* aGenThreadControl = 0);
   ~GenIHelper();

   /// Get the message.  User may continuously call this function until.
   /// \param aTimeValue Time in microseconds fuction will wait until a message is received.
   /// \return A pointer to a valid message or a null pointer if the timeout
   ///         is reached or an error occurred trying to read a GenI input stream.\n
   ///         <b>User is responsible for the returned MsgType pointer.</b>
   MsgType* GetMsg(int aTimeValue = 0);

private:
   int               ReceiveBytes(int aTimeValue);
   GenI&             mGenI;
   MsgFactoryType*   mMsgFactory;
   GenThreadControl* mGenThreadControl;
   unsigned int      mOwnThreadControl;
};

template<typename MsgType, typename MsgFactoryType>
GenIHelper<MsgType, MsgFactoryType>::GenIHelper(GenI& aGenI, MsgFactoryType* aMsgFactory, GenThreadControl* aGenThreadControl)
   : mGenI(aGenI)
   , mMsgFactory(aMsgFactory)
{
   if (aGenThreadControl != 0)
   {
      mGenThreadControl = aGenThreadControl;
      mOwnThreadControl = 0;
   }
   else
   {
      mGenThreadControl = new GenThreadControl;
      mOwnThreadControl = 1;
   }
}

template<typename MsgType, typename MsgFactoryType>
GenIHelper<MsgType, MsgFactoryType>::~GenIHelper()
{
   if (mOwnThreadControl)
   {
      delete mGenThreadControl;
   }
}

template<typename MsgType, typename MsgFactoryType>
MsgType* GenIHelper<MsgType, MsgFactoryType>::GetMsg(int aTimeValue)
{
   long int bytesLeft = mGenI.GetGetOffset(GenBuf::FromDataEnd);
   if (!bytesLeft)
   {
      bytesLeft = ReceiveBytes(aTimeValue);
   }
   while (bytesLeft)
   {
      // Got to have enough bytes for at least the base class
      MsgType* ptr;
      if (bytesLeft < MsgType::GetBaseLength())
      {
         DEBUG_IT(ut::log::debug() << "Discarding " << bytesLeft << " bytes.";);

         mGenI.SetGetOffset(0, GenBuf::FromDataEnd);
         ptr = 0;
      }
      else
      {
         ptr = mMsgFactory->GetMsg(mGenI);
      }

      if (ptr)
      {
         return ptr;
      }
      else
      {
         // Should only get here is we had trouble reading
         // the GenI input stream
         bytesLeft = ReceiveBytes(0);
      }
   }

   // If we got here then nothing is out there
   return 0;
}

template<typename MsgType, typename MsgFactoryType>
int GenIHelper<MsgType, MsgFactoryType>::ReceiveBytes(int aTimeValue)
{
   mGenThreadControl->Lock();

   int status = mGenI.Receive(aTimeValue);

   mGenThreadControl->Unlock();

   if (status < 0)
   {
      status = 0;
   }
   return status;
}

#endif
