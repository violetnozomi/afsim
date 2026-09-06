// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   trackRecord.hpp
 *  @author Kenneth R. Sewell III
 *          Kenneth.Sewell@wpafb.af.mil
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#ifndef IADSLIB_TRACKRECORD_HPP
#define IADSLIB_TRACKRECORD_HPP

#include <vector>
#include <map>
#include <memory>

#include <iadsLib/enum.hpp>
#include <iadsLib/airTargetMessage.hpp>
#include <iadsLib/idRecord.hpp>
#include <iadsLib/positionRecord.hpp>
#include <iadsLib/unitTypeRecord.hpp>
#include <iadsLib/trackContainers.hpp>



/**
 *  @brief Encapsulate all IADSLIB functions and classes.
 */
namespace il
{

	typedef idRecord tTrackID;

    /**
       @brief Class to represent a single track.
    */
    class IADSLIB_EXPORT trackRecord
    {
    public:

      trackRecord();
      ~trackRecord() = default;

      bool operator==( const trackRecord &record ) const;
      bool operator<( const trackRecord &record ) const;

      void setID( const idRecord &ID );
      const idRecord &getID() const;
      idRecord &getID();

      const idRecord &getSendingSystemID() const;

      void setPosition( const positionRecord &Position );
      const positionRecord &getPosition() const;
      positionRecord &getPosition();

      void setVelocityVector( const double &dX,
			      const double &dY,
			      const double &dZ );
      void setVelocityVector( const vm::Vector3<double> &Velocity );
      const vm::Vector3<double> &getVelocityVector() const;
      vm::Vector3<double> &getVelocityVector();
      const double getVelocity() const;

      void setCovarianceMatrix( const std::vector<double> &covar );
      const std::vector<double> &getCovarianceMatrix() const;
      std::vector<double> getCovarianceMatrix();

      void setHeadingRads( const double Heading_rads );
      const double getHeadingRads() const;

      void setHeadingDegs( const double Heading_degs );
      const double getHeadingDegs() const;

      void setTime( const double &Time );
      const double getTime() const;

      const double getUpdateInterval() const;

      void processMessage( const airTargetMessage &atm );

      void setType( const unitTypeRecord &Type );
      const unitTypeRecord &getType() const;

      void setTrackingStatus( const unsigned char &TrackingStatus );
      const unsigned char getTrackingStatus() const;

      void resetOutOfWindowCount();
      void incrementOutOfWindowCount();
      const unsigned int getOutOfWindowCount() const;

      double distance( const trackRecord &track ) const; // both return slant range
      double distance( const il::positionRecord &position ) const;


      bool isInWindow( const trackRecord &track,
		       const double &windowRange ) const;

      void setIFF( const eIFFType IFF );
      const eIFFType getIFF() const;

      void setTargetTruthName( const std::string &Name );
      const std::string &getTargetTruthName() const;

      void setTargetTruthID (const idRecord &record);
      idRecord getTargetTruthID () const;

	  void setTargetTruthSide (const std::string &Side );
	  const std::string &getTargetTruthSide() const;

      void setReportingSensorType (const std::string &type);
      const std::string &getReportingSensorType () const; // something a bit more generic for the IADSLib
      il::SensorEnum getReportingSensorTypeEnum () const;

      const double getJammingPower() const;

      positionRecord getPropagatedPosition (const double currentTime) const;

      static trackRecord extrapolateTrack( const trackRecord &st,
						                         const double deltaTime );

      bool generateAirTargetMessage( airTargetMessage &atm ) const;

      void calculateStrength(const double SimTime, const trackMap &tracks, const double maxGroupingDistance);

      const unsigned int getStrength() const;

      const unsigned int getQuantity() const;

    protected:
      idRecord            _ID;
      idRecord            _SendingSystemID;
      bool                _AltitudeReliable;
      positionRecord      _Position;
      vm::Vector3<double> _VelocityVector;
      double              _Velocity;
      std::vector<double> _CovarianceMatrix;
      unitTypeRecord      _Type;
      double              _TimeCollected;
      double              _Time;
      double              _UpdateInterval;
      unsigned char       _TrackingStatus;
      /// Orientation (rad) wrt ECEF
      double              _Psi_rads;
      double              _Theta_rads;
      double              _Phi_rads;

      double              _Heading_rads;
      double              _JammingPower;
      unsigned int        _Quantity;
      unsigned int        _EstimatedStrength;

      eIFFType            _IFF;

      std::string         _TruthName;
      idRecord            _TruthID;
      std::string         _ReportingSensorType;
	  std::string		  _TruthSide;

      void extrapolateTrack( const double deltaTime );
    private:

    };


}
#endif
