// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "packetio_export.h"

#include "PakHeader.hpp"
#include "PakPacket.hpp"
#include "PakProcessor.hpp"
#include "PakSerialize.hpp"
#include "PakSerializeImpl.hpp"
#include "PakSerializeTypes.hpp"
#include "UtLog.hpp"


namespace log_server
{
//! Responsible for serializing and deserializing log messages
//! for the logging server.
//! @details The way this works is the save and load methods in PakSerializeTypes.cpp
//! The PakProcessor overloads the & operator to use the correct methods on
//! >> incoming and << outgoing log messages.
class PACKETIO_EXPORT LogPacket : public PakPacket
{
public:
   constexpr static int cPACKET_ID = 1001;

   LogPacket()
      : PakPacket(cPACKET_ID)
   {
   }

   //! Serializes the packet header.  Must be called first from derived class Serialize()
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& message;
   }
   ut::log::Message message;
};
} // namespace log_server
