// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @class  weaponsControlMessage
 *  @file   weaponsControlMessage.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <iadsLib/baseMessage.hpp>

#ifndef WEAPONSCONTROLMESSAGE_HPP
#define WEAPONSCONTROLMESSAGE_HPP

namespace il
{
class weaponsControlMessage : public baseMessage
{
   public:

      weaponsControlMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      virtual ~weaponsControlMessage() = default;

      baseMessage* clone() const override;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;

      void setDataTime(const double& DataTime)
      {
         _DataTime = DataTime;
      }

      const double& getDataTime() const
      {
         return _DataTime;
      }

      void setPrepareMissileCommand(
         const unsigned char& PrepareMissileCommand)
      {
         _PrepareMissileCommand = PrepareMissileCommand;
      }

      const unsigned char& getPrepareMissileCommand() const
      {
         return _PrepareMissileCommand;
      }

      void setPrepareMissileQuantity(
         const unsigned short& PrepareMissileQuantity)
      {
         _PrepareMissileQuantity = PrepareMissileQuantity;
      }

      const unsigned short& getPrepareMissileQuantity() const
      {
         return _PrepareMissileQuantity;
      }

   protected:
      // Timestamp of message data.
      double _DataTime;

      // Prepare fixed amount, continuously or cancel
      unsigned char _PrepareMissileCommand;

      // Quantity of missiles to prepare
      unsigned short _PrepareMissileQuantity;
};
}

#endif
