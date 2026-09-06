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


#ifndef GENIO_MSG_CONNECT_HPP
#define GENIO_MSG_CONNECT_HPP

#include "genio_export.h"

#include <string>

#include "GenIOMsg.hpp"

class GenI;
class GenO;

class GENIO_EXPORT GenIOMsgConnect : public GenIOMsg
{
public:
   GenIOMsgConnect();

   GenIOMsgConnect(double timeA, int recvOnPortA, const std::string& nameA);

   GenIOMsgConnect(double             timeA,
                   int                recvOnPortA,
                   int                useUdpA,
                   const std::string& nameA,
                   float              frameTimeA,
                   int                rudpRetransmitTimeOutA,
                   int                rudpSendQueueLengthA,
                   int                rudpRecvQueueLengthA);

   GenIOMsgConnect(const GenIOMsg& genIOMsgA, GenI& genIA);

   GenIOMsgConnect(GenI& genIA);

   ~GenIOMsgConnect() override;

   // Accessors
   int                GetRecvOnPort() const;
   int                IsUdpRequested() const;
   float              GetFrameTime() const;
   int                GetRudpRetransmitTimeOut() const;
   int                GetRudpSendQueueLength() const;
   int                GetRudpRecvQueueLength() const;
   const std::string& GetName() const;

   // Mutators
   void SetRecvOnPort(int recvOnPortA);
   void SetName(const std::string& nameA);

protected:
   // Input/output
   int  GetGenIOMsgClassLength() const override;
   void GetGenIOMsgClassData(GenI& genIA) override;
   void PutGenIOMsgClassData(GenO& genOA) const override;

private:
   int         GetPad() const;
   int         recvOnPortM;
   float       frameTimeM;
   int         rudpRetransmitTimeOutM;
   int         rudpSendQueueLengthM;
   int         rudpRecvQueueLengthM;
   std::string nameM;
};

// Accessors
inline int GenIOMsgConnect::GetRecvOnPort() const
{
   // Note this bit logic torpedoes any chance of getting a negative number through.
   // It needs to be removed on the next update.
   return recvOnPortM & 0x7FFFFFFF;
}

inline int GenIOMsgConnect::IsUdpRequested() const
{
   return recvOnPortM >> 31 & 1;
}

inline const std::string& GenIOMsgConnect::GetName() const
{
   return nameM;
}

inline float GenIOMsgConnect::GetFrameTime() const
{
   return frameTimeM;
}

inline int GenIOMsgConnect::GetRudpRetransmitTimeOut() const
{
   return rudpRetransmitTimeOutM;
}

inline int GenIOMsgConnect::GetRudpSendQueueLength() const
{
   return rudpSendQueueLengthM;
}

inline int GenIOMsgConnect::GetRudpRecvQueueLength() const
{
   return rudpRecvQueueLengthM;
}

// Mutators
inline void GenIOMsgConnect::SetRecvOnPort(int recvOnPortA)
{
   recvOnPortM = recvOnPortA;
}

inline void GenIOMsgConnect::SetName(const std::string& nameA)
{
   nameM = nameA;
}

#endif
