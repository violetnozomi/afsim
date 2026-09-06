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

//
// This class inherits from GenIOMsg
//


#include "GenIOMsgConnect.hpp"

#include "GenI.hpp"
#include "GenO.hpp"


GenIOMsgConnect::GenIOMsgConnect()
   : GenIOMsg(TypeConnect)
{
}

GenIOMsgConnect::GenIOMsgConnect(double timeA, int recvOnPortA, const std::string& nameA)
   : GenIOMsg(timeA, TypeConnect)
   , recvOnPortM(recvOnPortA)
   , frameTimeM(0.05F)
   , rudpRetransmitTimeOutM(0)
   , rudpSendQueueLengthM(0)
   , rudpRecvQueueLengthM(0)
   , nameM(nameA)

{
}

GenIOMsgConnect::GenIOMsgConnect(double             timeA,
                                 int                recvOnPortA,
                                 int                useUdpA,
                                 const std::string& nameA,
                                 float              frameTimeA,
                                 int                rudpRetransmitTimeOutA,
                                 int                rudpSendQueueLengthA,
                                 int                rudpRecvQueueLengthA)
   : GenIOMsg(timeA, TypeConnect)
   , recvOnPortM(recvOnPortA | useUdpA << 31)
   , frameTimeM(frameTimeA)
   , rudpRetransmitTimeOutM(rudpRetransmitTimeOutA)
   , rudpSendQueueLengthM(rudpSendQueueLengthA)
   , rudpRecvQueueLengthM(rudpRecvQueueLengthA)
   , nameM(nameA)

{
}

GenIOMsgConnect::GenIOMsgConnect(const GenIOMsg& genIOMsgA, GenI& genIA)
   : GenIOMsg(genIOMsgA)
{
   GetGenIOMsgClassData(genIA);
   CheckMismatchedSize(genIA);
}

GenIOMsgConnect::GenIOMsgConnect(GenI& genIA)
   : GenIOMsg(genIA)
{
   GetGenIOMsgClassData(genIA);
   CheckMismatchedSize(genIA);
}

GenIOMsgConnect::~GenIOMsgConnect() {}

// Virtual
int GenIOMsgConnect::GetGenIOMsgClassLength() const
{
   // Add 4, because when we write the string it takes
   // an extra 4 characters to send the size of the string
   int fixedVariables = GetVersion() > 1 ? 20 : 4;

   return fixedVariables +                   // recvOnPortM through rudpRecvQueueLengthM
          static_cast<int>(nameM.length()) + // nameM length
          4 +                                // string pad
          GetPad();
}

// Virtual
void GenIOMsgConnect::GetGenIOMsgClassData(GenI& genIA)
{
   genIA >> recvOnPortM;

   if (GetVersion() > 1)
   {
      genIA >> frameTimeM;
      genIA >> rudpRetransmitTimeOutM;
      genIA >> rudpSendQueueLengthM;
      genIA >> rudpRecvQueueLengthM;
   }
   else
   {
      frameTimeM             = 0.05F;
      rudpRetransmitTimeOutM = 0;
      rudpSendQueueLengthM   = 0;
      rudpRecvQueueLengthM   = 0;
   }

   genIA.GetString(nameM);

   int numPad = GetPad();
   for (int j = 0; j < numPad; ++j)
   {
      char pad;
      genIA >> pad;
   }
}

// Virtual
void GenIOMsgConnect::PutGenIOMsgClassData(GenO& genOA) const
{
   genOA << recvOnPortM;

   if (GetVersion() > 1)
   {
      genOA << frameTimeM;
      genOA << rudpRetransmitTimeOutM;
      genOA << rudpSendQueueLengthM;
      genOA << rudpRecvQueueLengthM;
   }

   genOA.PutString(nameM);

   int numPad = GetPad();
   for (int j = 0; j < numPad; ++j)
   {
      char pad = 0;
      genOA << pad;
   }
}

int GenIOMsgConnect::GetPad() const
{
   int fixedVariables = GetVersion() > 1 ? 20 : 4;
   return GetPad1(fixedVariables + static_cast<int>(nameM.length()) + 4);
}
