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

#ifndef PAK_LOG_SERVER_SUBSCRIBER_HPP
#define PAK_LOG_SERVER_SUBSCRIBER_HPP

#include "packetio_export.h"

#include "PakProcessor.hpp"
#include "PakTCP_IO.hpp"
#include "UtLogMessage.hpp"
#include "UtLogSubscriberT.hpp"

namespace log_server
{
//! Responsible for processing ut::log::Messages and sending them
//! to the log server.
class PACKETIO_EXPORT LogServerMessageProcessor final : public ut::log::MessageProcessor
{
public:
   //! the contracts for the logging system
   LogServerMessageProcessor();
   void Process(const ut::log::Message& aMessage) override;
   //! Starts the server
   bool Init(const std::string& aLogServerHost, int aLogServerPort);

private:
   // Responsible for serialization of log messages.
   std::unique_ptr<PakProcessor> mProcessor;
   // Abstraction over TCP connections
   std::unique_ptr<PakTCP_IO> mTcpClient;
   int                        mLogServerPort = 18888;
   std::string                mLogServerHost = "localhost";
};

//! This wires up the subscriber upon instantiation.
using LogSubscriber = ut::log::SubscriberT<LogServerMessageProcessor>;
} // namespace log_server


#endif
