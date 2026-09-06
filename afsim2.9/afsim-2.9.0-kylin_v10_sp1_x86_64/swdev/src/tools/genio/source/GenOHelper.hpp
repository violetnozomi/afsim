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

#ifndef GEN_O_HELPER_HPP
#define GEN_O_HELPER_HPP

/// \file  GenOHelper.hpp
/// \brief GenOHelper
/// \class GenOHelper GenOHelper.hpp
/// \brief This class provides an easy way to send messages to a GenO derived class.
///
/// The use of this class is not an absolute necessity, but it does provide the
/// code most users would otherwise implement to send messages to a GenO derived class.
/// Most users find it advantages to use this class.
///
/// The message set or sets used with this class must support the method:\n
/// int MsgType::GetLength() const;

#include "GenO.hpp"
#include "GenThreadControl.hpp"

//#define DEBUG_ON
#ifdef DEBUG_ON
#define DEBUG_IT(A) A
#else
#define DEBUG_IT(A) /* A */
#endif

template<typename MsgType>
class GenOHelper
{
public:
   /// Constructor
   /// \param aGenO             A GenO derived class to send the messages.
   /// \param aGenThreadControl A pointer to a GenThreadControl class.
   GenOHelper(GenO& aGenO, GenThreadControl* aGenThreadControl = 0);
   ~GenOHelper();

   /// Put a message
   /// \param A pointer to a message
   /// \return The number of bytes put on the buffer if successful.
   int PutMsg(MsgType* aMsg);

   /// Put a message
   /// \param A pointer to a const message
   /// \return The number of bytes put on the buffer if successful.
   int PutMsg(const MsgType* aMsg);

   /// Ensure any remaining "Put"  messages are flushed.
   /// User should call this function after all messages have
   /// been "Put" on a given simulation update cycle.
   /// \retval >=0   Data sent.  This value could optionally be the number of bytes sent.
   /// \retval <0    An error has occurred.
   int FlushMsg();

private:
   GenO&             mGenO;
   GenThreadControl* mGenThreadControl;
   unsigned int      mOwnThreadControl;
};

template<typename MsgType>
GenOHelper<MsgType>::GenOHelper(GenO& aGenO, GenThreadControl* aGenThreadControl)
   : mGenO(aGenO)
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

template<typename MsgType>
GenOHelper<MsgType>::~GenOHelper()
{
   if (mOwnThreadControl)
   {
      delete mGenThreadControl;
   }
}


template<typename MsgType>
int GenOHelper<MsgType>::PutMsg(MsgType* aMsg)
{
   int status = 1;

   // Determine if there is enough space in the buffer for this PDU
   // and flush it if there isn't
   int length = aMsg->GetLength();
   if (aMsg->GetLength() > static_cast<int>(mGenO.GetPutOffset(GenBuf::FromPhyEnd)))
   {
      status = FlushMsg();
   }

   if (status >= 0)
   {
      aMsg->Put(mGenO);
      status = length;
   }

   return status;
}

template<typename MsgType>
int GenOHelper<MsgType>::PutMsg(const MsgType* aMsg)
{
   int status = 1;

   // Determine if there is enough space in the buffer for this PDU
   // and flush it if there isn't
   int length = aMsg->GetLength();
   if (length > static_cast<int>(mGenO.GetPutOffset(GenBuf::FromPhyEnd)))
   {
      status = FlushMsg();
   }

   if (status >= 0)
   {
      aMsg->Put(mGenO);
      status = length;
   }

   return status;
}

template<typename MsgType>
int GenOHelper<MsgType>::FlushMsg()
{
   mGenThreadControl->Lock();

   int status = mGenO.Send();

   mGenThreadControl->Unlock();

   return status;
}

#endif
