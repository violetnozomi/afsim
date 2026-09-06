// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @class  baseMessage
 *  @file   baseMessage.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 *
 */



#ifndef IADSLIB_BASECLASS_HPP
#define IADSLIB_BASECLASS_HPP

#include <iostream>
#include <iosfwd>
#include <iadsLib/buffer.hpp>
#include <string>
#include <iadsLib/idRecord.hpp>
#include "logger.hpp"

#if DEBUG_VCL
#include <cassert>
#endif

namespace il
{
enum messageType
{
   UNKNOWN_MESSAGE         = 0,
   AIR_TARGET_MESSAGE,
   PLOT_MESSAGE,
   BEARING_REPORT_MESSAGE,
   COMBAT_STATUS_MESSAGE,
   ASSIGNMENT_ACK_MESSAGE,
   ASSIGNMENT_MESSAGE,
   WEAPONS_CONTROL_MESSAGE,
   CUE_MESSAGE,
   CUE_ACK_MESSAGE,
   ASSIGNMENT_TRACK_MESSAGE,
};

/// Parent class for all iadslib messages.
class IADSLIB_EXPORT baseMessage
{
   public:
      /**
         Default constructor.
      */
      baseMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger, const unsigned short& Type = UNKNOWN_MESSAGE);
      virtual ~baseMessage() = default;
      virtual baseMessage* clone() const = 0;

      virtual void logSTD(std::ostream& os) const;
      virtual void logCSV(std::ostream& os, const double& Time) const;
      virtual std::string getMessageTypeString() const;

      void setSenderID(const idRecord& ID);
      const idRecord& getSenderID() const;

      void setDestinationID(const idRecord& ID);
      const idRecord& getDestinationID() const;

      bool isDestinationBroadcast() const;
      void setDestinationBroadcast(bool value = true);

      /**
         \brief Returns the message type.
         \return Type of Message.
      */
      const unsigned short& getMessageType() const;

      const unsigned int& getMessageNumber() const;

      void setMessageNumber(const unsigned int& Number);

      void setTransmitTime(const double& TransmitTime);
      const double& getTransmitTime() const;

      void setLatentProcessTime(const double future_time_s);
      bool isTimeToProcess(const double curr_time_s) const;
      bool isLatentProcessFlagSet() const;
      double getLatentProcessTime() const;
      void resetLatentProcessFlags();

   protected:
      /**
         \brief Set the message type.  See vclLink::messageType
         \param Type - Message type.  See vclLink::messageType
      */
      void setMessageType(const unsigned short& Type);

      std::weak_ptr<logger::GlobalLogger> mGlobalLogger;

   private:

      idRecord       _SenderID;
      idRecord       _DestinationID;
      unsigned short _MessageType;
      unsigned int   _MessageNumber;
      double         _TransmitTime;
      unsigned int   _IsDestinationBroadcast;
      bool           _IsLatentMessage;
      double         _LatentProcessTime; // send/receive agnostic, can be used at both ends

      static const double C_IMMEDIATE_PROCESS_TIME;
};
}

#endif
