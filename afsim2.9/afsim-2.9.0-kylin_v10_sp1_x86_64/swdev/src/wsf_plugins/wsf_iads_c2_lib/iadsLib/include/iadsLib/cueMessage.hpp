// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @class  assignmentMessage
 *  @file   assignmentMessage.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 **  Classification: UNCLASSIFIED
 */



#ifndef CUEMESSAGE_HPP
#define CUEMESSAGE_HPP

#include <iostream>

#include <iadsLib/baseMessage.hpp>
#include <iadsLib/idRecord.hpp>


namespace il
{
class IADSLIB_EXPORT cueMessage : public baseMessage
{
   public:

      enum CueReason
      {
         NEW_CUE = 0,
         CANCEL  = 1
      };

      cueMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      virtual ~cueMessage() = default;
      baseMessage* clone() const override;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;

      virtual void create(const double simTime,
                           const CueReason reason,
                           const idRecord& reference_track_number,
                           const idRecord& initiating_unit_id,
                           const idRecord& cued_unit_id,
                           const idRecord& cued_sensor_id);

      virtual void setCueTime(const double time_s);
      virtual double getCueTime() const;

      virtual void setReferenceTrackID(const idRecord& value);
      virtual idRecord getReferenceTrackID() const;

      virtual void setLocalTrackID(const idRecord& value);
      virtual idRecord getLocalTrackID() const;

      virtual void setInitiatingID(const idRecord& value);
      virtual idRecord getInitiatingID() const;

      virtual void setCuedUnitID(const idRecord& value);
      virtual idRecord getCuedUnitID() const;

      virtual void setCuedSensorID(const idRecord& value);
      virtual idRecord getCuedSensorID() const;

      virtual void setCueReason(const CueReason reason);
      virtual CueReason getCueReason() const;

      static std::string ReasonToStr(const CueReason reason);

   protected:

      // Cue Time //
      double _Time; // simulation time in seconds

      // Track ID //
      // ID of track to be engaged, cancelled, re-assigned...
      idRecord _ReferenceTrackID;
      idRecord _LocalTrackID;

      // Controlling ID //
      idRecord _InitiatingID;

      // Sensor to be Cued //
      idRecord _CuedUnitID;
      idRecord _CuedSensorID;


      CueReason _Reason;
};
}

#endif
