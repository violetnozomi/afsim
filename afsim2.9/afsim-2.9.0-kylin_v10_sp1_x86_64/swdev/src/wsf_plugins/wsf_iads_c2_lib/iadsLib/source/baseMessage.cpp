// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  baseMessage
 *  \file   baseMessage.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 *  Classification: UNCLASSIFIED
 */

#include <fstream>
#include <iostream>
#include <iomanip>
#include <logger.hpp>
#include <iadsLib/baseMessage.hpp>
#include <iadsLib/util.hpp>
#include <iadsLib/NumericUtils.hpp>

#include <cstring>

namespace il
{

const double baseMessage::C_IMMEDIATE_PROCESS_TIME = -1;

baseMessage::baseMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger, const unsigned short& Type):
   mGlobalLogger(aGlobalLogger),
   _SenderID(0),
   _DestinationID(0),
   _MessageType(Type),
   _MessageNumber(0),
   _TransmitTime(0.0),
   _IsDestinationBroadcast(0),
   _IsLatentMessage(false),
   _LatentProcessTime(C_IMMEDIATE_PROCESS_TIME)
{
}

/**
   void log( )
   \brief Prints class to an ostream.
*/
void baseMessage::logSTD(std::ostream& os) const
{
   os << "     Sender ID: " << _SenderID      << std::endl
      << "Destination ID: " << _DestinationID << std::endl
      << "Is Broadcast dest?: " << BoolToString(_IsDestinationBroadcast ? true : false) << std::endl
      << "  Message Type: " << _MessageType   << std::endl
      << "Message Number: " << _MessageNumber << std::endl
      << " Transmit Time: " << _TransmitTime  << std::endl
      << "    Is Latent?: " << BoolToString(_IsLatentMessage) << std::endl
      << "   Latent Time: " << _LatentProcessTime;
}

/**
   void log( )
   \brief Prints class to any ostream.
*/
void baseMessage::logCSV(std::ostream& os, const double& Time) const
{
   os << Time << ","
      << _SenderID << ","
      << _DestinationID << ","
      << BoolToString(_IsDestinationBroadcast ? true : false) << ","
      << _MessageType   << ","
      << _MessageNumber << ","
      << _TransmitTime << ","
      << BoolToString(_IsLatentMessage) << ","
      << _LatentProcessTime;
}

std::string baseMessage::getMessageTypeString() const
{
   std::string typestring;

   switch (_MessageType)
   {
      case AIR_TARGET_MESSAGE:
         typestring = "Air Target";
         break;
      case PLOT_MESSAGE:
         typestring = "Plot";
         break;
      case BEARING_REPORT_MESSAGE:
         typestring = "Bearing Report";
         break;
      case COMBAT_STATUS_MESSAGE:
         typestring = "Status";
         break;
      case ASSIGNMENT_ACK_MESSAGE:
         typestring = "Assignment Status";
         break;
      case ASSIGNMENT_MESSAGE:
         typestring = "Assignment";
         break;
      case WEAPONS_CONTROL_MESSAGE:
         typestring = "Weapons Control";
         break;
      case CUE_MESSAGE:
         typestring = "Cue";
         break;
      case CUE_ACK_MESSAGE:
         typestring = "Cue Status";
         break;
      case ASSIGNMENT_TRACK_MESSAGE:
         typestring = "Assignment Track";
         break;
      case UNKNOWN_MESSAGE:
         typestring = "Unknown";
         break;
      default:
         typestring = "** Error ** Unset message type";
         break;
   }

   return typestring;
}

void baseMessage::setSenderID(const idRecord& ID)
{
   _SenderID = ID;
}

const idRecord& baseMessage::getSenderID() const
{
   return _SenderID;
}

void baseMessage::setDestinationID(const idRecord& ID)
{
   _DestinationID = ID;
   // this implies it is not a broadcast, reset the value
   setDestinationBroadcast(false);
}

const idRecord& baseMessage::getDestinationID() const
{
   if (isDestinationBroadcast())
   { HCL_WARN_LOGGER(mGlobalLogger) << "called baseMessage::getDestinationID() but message is set to broadcast"; }

   return _DestinationID;
}

// Note: We don't have a special sentinal for the actual id since any framework could really use any id, it is up to the user to check
// specifically for the broadcast flag before deciding to interpret destination id.
bool baseMessage::isDestinationBroadcast() const
{
   return _IsDestinationBroadcast ?  true : false;
}

void baseMessage::setDestinationBroadcast(bool value)
{
   _IsDestinationBroadcast = value;
}

/**
   \brief Returns the message type.
   \return Type of Message.
*/
const unsigned short& baseMessage::getMessageType() const
{
   return _MessageType;
}

const unsigned int& baseMessage::getMessageNumber() const
{
   return _MessageNumber;
}

void baseMessage::setMessageNumber(const unsigned int& Number)
{
   _MessageNumber = Number;
}

/**
   \brief Set the message type.  See vclLink::messageType
   \param Type - Message type.  See vclLink::messageType
*/
void baseMessage::setMessageType(const unsigned short& Type)
{
   _MessageType = Type;
}

void baseMessage::setTransmitTime(const double& TransmitTime)
{
   _TransmitTime = TransmitTime;
}

const double& baseMessage::getTransmitTime() const
{
   return _TransmitTime;
}

void baseMessage::setLatentProcessTime(const double future_time_s)
{
   _IsLatentMessage = true;
   _LatentProcessTime = future_time_s;
}

bool baseMessage::isTimeToProcess(const double curr_time_s) const
{
   return !_IsLatentMessage || NumericUtils::LessEqual(_LatentProcessTime, curr_time_s);
}

bool baseMessage::isLatentProcessFlagSet() const
{
   return _IsLatentMessage;
}

double baseMessage::getLatentProcessTime() const
{
   return _LatentProcessTime;
}

void baseMessage::resetLatentProcessFlags()
{
   _IsLatentMessage = false;
   _LatentProcessTime = C_IMMEDIATE_PROCESS_TIME;
}

} // namespace il
