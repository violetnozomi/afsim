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

#ifndef GEN_UMP_IO_HELPER_HPP
#define GEN_UMP_IO_HELPER_HPP

#include "GenExport.hpp"

// This class will hopefully be deprecated soon.  Unless you need ThreadControl
// logic you are encouraged to use GenIHelper and GenOHelper.  (5/10/2004)

// This class can be used for a family of message classes that want
// to use GenUmpIO.  The family of message classes must derive from
// a base class that supports the three methods:
//
// static MsgBase* MsgBase::Create(GenUmpIO&, MsgFactory*)
// static int      MsgBase::GetBaseLength()
//        int      MsgBase::GetLength() const  <--- const only needed if using
//                                                  a const Put method
//
// If a Message factory is not desired, MsgFactory could be of type int
// with a value of a null pointer.  The MsgBase::Create method would in
// this case ignore the MsgFactory pointer.


#include <iostream>

#include "GenError.hpp"
#include "GenThreadControl.hpp"
#include "GenUmpIO.hpp"

//#define DEBUG_ON
#ifdef DEBUG_ON
#define DEBUG_IT(A) A
#else
#define DEBUG_IT(A) /* A */
#endif

template<class MsgBase, class MsgFactory>
class GEN_EXPORT GenUmpIOHelper
{
public:
   GenUmpIOHelper(GenUmpIO& aGenUmpIO, MsgFactory* aMsgFactory, GenThreadControl* aGenThreadControl = 0);

   ~GenUmpIOHelper();

   // int      Init(DisEntityId&   myIdA,
   //               VTrackFactory* vTrackFactoryA,
   //               const char*    simGroupA,
   //               const char*    simNameA);

   // User is responsible for MsgBase pointer.  User should call
   // this function until a null pointer is returned.
   MsgBase* GetMsg();

   // Returns a positive number if successful
   // Returns a negative number if there was an error.
   int PutMsg(MsgBase* aMsgBase);
   int PutMsg(const MsgBase* aMsgBase);

   // Ensure any remaining "Put"  messages are flushed.
   // User should call this function after all messages have
   // been "Put" on a given simulation update cycle.
   // Returns the number of bytes sent.
   // Returns a negative number if there was an error.
   int FlushMsg();

private:
   int               ReceiveBytes();
   GenUmpIO&         mGenUmpIO;
   MsgFactory*       mMsgFactory;
   GenThreadControl* mGenThreadControl;
   unsigned int      mOwnThreadControl;
};

template<class MsgBase, class MsgFactory>
GenUmpIOHelper<MsgBase, MsgFactory>::GenUmpIOHelper(GenUmpIO&         aGenUmpIO,
                                                    MsgFactory*       aMsgFactory,
                                                    GenThreadControl* aGenThreadControl)
   : mGenUmpIO(aGenUmpIO)
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

template<class MsgBase, class MsgFactory>
GenUmpIOHelper<MsgBase, MsgFactory>::~GenUmpIOHelper()
{
   if (mOwnThreadControl)
   {
      delete mGenThreadControl;
   }
}

template<class MsgBase, class MsgFactory>
MsgBase* GenUmpIOHelper<MsgBase, MsgFactory>::GetMsg()
{
   long int bytesLeft = mGenUmpIO.GetGetOffset(GenBuf::FromDataEnd);
   if (!bytesLeft)
   {
      bytesLeft = ReceiveBytes();
   }
   while (bytesLeft)
   {
      // Got to have enough bytes for at least the base class
      MsgBase* ptr;
      if (bytesLeft < MsgBase::GetBaseLength())
      {
         DEBUG_IT(std::cout << "Discarding " << bytesLeft << " bytes" << std::endl);
         mGenUmpIO.SetGetOffset(0, GenBuf::FromDataEnd);
         ptr = 0;
      }
      else
      {
         ptr = MsgBase::Create(mGenUmpIO, mMsgFactory);
      }

      if (ptr)
      {
         return ptr;
      }
      else
      {
         bytesLeft = ReceiveBytes();
      }
   }

   // If we got here then nothing is out there
   return 0;
}

template<class MsgBase, class MsgFactory>
int GenUmpIOHelper<MsgBase, MsgFactory>::ReceiveBytes()
{
   mGenThreadControl->Lock();

   int status = mGenUmpIO.Receive(0);

   mGenThreadControl->Unlock();

   if (status < 0)
   {
      if (status < -1)
      {
         if (GenError::ShowError(GenError::cIO_ERRORS))
         {
            std::cerr << "Ump error " << status << " in GenUmpIOHelper::Receive()" << std::endl;
         }
      }
      status = 0;
   }
   return status;
}


template<class MsgBase, class MsgFactory>
int GenUmpIOHelper<MsgBase, MsgFactory>::PutMsg(MsgBase* aMsgBase)
{
   int status = 1;

   // Determine if there is enough space in the buffer for this PDU
   // and flush it if there isn't
   if (aMsgBase->GetLength() > static_cast<int>(mGenUmpIO.GetNumUnusedBytesInOutputBuffer()))
   {
      status = FlushMsg();
   }

   aMsgBase->Put(mGenUmpIO);

   return status;
}

template<class MsgBase, class MsgFactory>
int GenUmpIOHelper<MsgBase, MsgFactory>::PutMsg(const MsgBase* aMsgBase)
{
   int status = 1;

   // Determine if there is enough space in the buffer for this PDU
   // and flush it if there isn't
   if (aMsgBase->GetLength() > static_cast<int>(mGenUmpIO.GetNumUnusedBytesInOutputBuffer()))
   {
      status = FlushMsg();
   }

   aMsgBase->Put(mGenUmpIO);

   return status;
}

template<class MsgBase, class MsgFactory>
int GenUmpIOHelper<MsgBase, MsgFactory>::FlushMsg()
{
   mGenThreadControl->Lock();

   int status = mGenUmpIO.Send();

   mGenThreadControl->Unlock();

   return status;
}

#endif
