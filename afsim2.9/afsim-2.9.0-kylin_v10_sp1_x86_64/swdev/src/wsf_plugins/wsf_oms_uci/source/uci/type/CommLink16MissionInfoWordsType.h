// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Type__CommLink16MissionInfoWordsType_h
#define Uci__Type__CommLink16MissionInfoWordsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilized to control the Link 16 Mission Information Words. */
      class CommLink16MissionInfoWordsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommLink16MissionInfoWordsType()
         { }

         /** Returns this accessor's type constant, i.e. CommLink16MissionInfoWordsType
           *
           * @return This accessor's type constant, i.e. CommLink16MissionInfoWordsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLink16MissionInfoWordsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLink16MissionInfoWordsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 0] [Maximum inclusive: 127] */
         typedef uci::base::IntAccessor NonC2_PGA_NetNumber;
         //  The type of primitive data accessed by NonC2_PGA_NetNumber
         typedef xs::Int NonC2_PGA_NetNumberValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 127] */
         typedef uci::base::IntAccessor NonC2_PGB_NetNumber;
         //  The type of primitive data accessed by NonC2_PGB_NetNumber
         typedef xs::Int NonC2_PGB_NetNumberValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 255] */
         typedef uci::base::IntAccessor MissionCorrelatorID;
         //  The type of primitive data accessed by MissionCorrelatorID
         typedef xs::Int MissionCorrelatorIDValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 32767] */
         typedef uci::base::IntAccessor MissionCDR_TrackNumber;
         //  The type of primitive data accessed by MissionCDR_TrackNumber
         typedef xs::Int MissionCDR_TrackNumberValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 32767] */
         typedef uci::base::IntAccessor FlightLeadTrackNumber;
         //  The type of primitive data accessed by FlightLeadTrackNumber
         typedef xs::Int FlightLeadTrackNumberValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the NonC2_PGA_NetNumber.
           *
           * @return The value of the simple primitive data type identified by NonC2_PGA_NetNumber.
           */
         virtual uci::type::CommLink16MissionInfoWordsType::NonC2_PGA_NetNumberValue getNonC2_PGA_NetNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NonC2_PGA_NetNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNonC2_PGA_NetNumber(uci::type::CommLink16MissionInfoWordsType::NonC2_PGA_NetNumberValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NonC2_PGA_NetNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NonC2_PGA_NetNumber is emabled or not
           */
         virtual bool hasNonC2_PGA_NetNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NonC2_PGA_NetNumber
           *
           * @param type = uci::type::commLink16MissionInfoWordsType::accessorType::nonC2_PGA_NetNumber This Accessor's accessor
           *      type
           */
         virtual void enableNonC2_PGA_NetNumber(uci::base::accessorType::AccessorType type = uci::type::commLink16MissionInfoWordsType::accessorType::nonC2_PGA_NetNumber)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NonC2_PGA_NetNumber */
         virtual void clearNonC2_PGA_NetNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NonC2_PGB_NetNumber.
           *
           * @return The value of the simple primitive data type identified by NonC2_PGB_NetNumber.
           */
         virtual uci::type::CommLink16MissionInfoWordsType::NonC2_PGB_NetNumberValue getNonC2_PGB_NetNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NonC2_PGB_NetNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNonC2_PGB_NetNumber(uci::type::CommLink16MissionInfoWordsType::NonC2_PGB_NetNumberValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NonC2_PGB_NetNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NonC2_PGB_NetNumber is emabled or not
           */
         virtual bool hasNonC2_PGB_NetNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NonC2_PGB_NetNumber
           *
           * @param type = uci::type::commLink16MissionInfoWordsType::accessorType::nonC2_PGB_NetNumber This Accessor's accessor
           *      type
           */
         virtual void enableNonC2_PGB_NetNumber(uci::base::accessorType::AccessorType type = uci::type::commLink16MissionInfoWordsType::accessorType::nonC2_PGB_NetNumber)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NonC2_PGB_NetNumber */
         virtual void clearNonC2_PGB_NetNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NonC2_PGA_Status.
           *
           * @return The value of the simple primitive data type identified by NonC2_PGA_Status.
           */
         virtual xs::Boolean getNonC2_PGA_Status() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NonC2_PGA_Status.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNonC2_PGA_Status(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NonC2_PGA_Status exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NonC2_PGA_Status is emabled or not
           */
         virtual bool hasNonC2_PGA_Status() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NonC2_PGA_Status
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableNonC2_PGA_Status(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NonC2_PGA_Status */
         virtual void clearNonC2_PGA_Status()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NonC2_PGB_Status.
           *
           * @return The value of the simple primitive data type identified by NonC2_PGB_Status.
           */
         virtual xs::Boolean getNonC2_PGB_Status() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NonC2_PGB_Status.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNonC2_PGB_Status(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NonC2_PGB_Status exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NonC2_PGB_Status is emabled or not
           */
         virtual bool hasNonC2_PGB_Status() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NonC2_PGB_Status
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableNonC2_PGB_Status(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NonC2_PGB_Status */
         virtual void clearNonC2_PGB_Status()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MissionCorrelatorID.
           *
           * @return The value of the simple primitive data type identified by MissionCorrelatorID.
           */
         virtual uci::type::CommLink16MissionInfoWordsType::MissionCorrelatorIDValue getMissionCorrelatorID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MissionCorrelatorID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMissionCorrelatorID(uci::type::CommLink16MissionInfoWordsType::MissionCorrelatorIDValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionCorrelatorID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionCorrelatorID is emabled or not
           */
         virtual bool hasMissionCorrelatorID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionCorrelatorID
           *
           * @param type = uci::type::commLink16MissionInfoWordsType::accessorType::missionCorrelatorID This Accessor's accessor
           *      type
           */
         virtual void enableMissionCorrelatorID(uci::base::accessorType::AccessorType type = uci::type::commLink16MissionInfoWordsType::accessorType::missionCorrelatorID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionCorrelatorID */
         virtual void clearMissionCorrelatorID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WingmanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by WingmanID.
           */
         virtual const uci::type::ForeignKeyType& getWingmanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WingmanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by WingmanID.
           */
         virtual uci::type::ForeignKeyType& getWingmanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WingmanID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WingmanID
           */
         virtual void setWingmanID(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WingmanID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WingmanID is emabled or not
           */
         virtual bool hasWingmanID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WingmanID
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableWingmanID(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WingmanID */
         virtual void clearWingmanID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MissionCDR_TrackNumber.
           *
           * @return The value of the simple primitive data type identified by MissionCDR_TrackNumber.
           */
         virtual uci::type::CommLink16MissionInfoWordsType::MissionCDR_TrackNumberValue getMissionCDR_TrackNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MissionCDR_TrackNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMissionCDR_TrackNumber(uci::type::CommLink16MissionInfoWordsType::MissionCDR_TrackNumberValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionCDR_TrackNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionCDR_TrackNumber is emabled or not
           */
         virtual bool hasMissionCDR_TrackNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionCDR_TrackNumber
           *
           * @param type = uci::type::commLink16MissionInfoWordsType::accessorType::missionCDR_TrackNumber This Accessor's
           *      accessor type
           */
         virtual void enableMissionCDR_TrackNumber(uci::base::accessorType::AccessorType type = uci::type::commLink16MissionInfoWordsType::accessorType::missionCDR_TrackNumber)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionCDR_TrackNumber */
         virtual void clearMissionCDR_TrackNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FlightLeadTrackNumber.
           *
           * @return The value of the simple primitive data type identified by FlightLeadTrackNumber.
           */
         virtual uci::type::CommLink16MissionInfoWordsType::FlightLeadTrackNumberValue getFlightLeadTrackNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FlightLeadTrackNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFlightLeadTrackNumber(uci::type::CommLink16MissionInfoWordsType::FlightLeadTrackNumberValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FlightLeadTrackNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FlightLeadTrackNumber is emabled or not
           */
         virtual bool hasFlightLeadTrackNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FlightLeadTrackNumber
           *
           * @param type = uci::type::commLink16MissionInfoWordsType::accessorType::flightLeadTrackNumber This Accessor's accessor
           *      type
           */
         virtual void enableFlightLeadTrackNumber(uci::base::accessorType::AccessorType type = uci::type::commLink16MissionInfoWordsType::accessorType::flightLeadTrackNumber)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FlightLeadTrackNumber */
         virtual void clearFlightLeadTrackNumber()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLink16MissionInfoWordsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLink16MissionInfoWordsType to copy from
           */
         CommLink16MissionInfoWordsType(const CommLink16MissionInfoWordsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLink16MissionInfoWordsType to the contents of the
           * CommLink16MissionInfoWordsType on the right hand side (rhs) of the assignment operator.CommLink16MissionInfoWordsType
           * [only available to derived classes]
           *
           * @param rhs The CommLink16MissionInfoWordsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CommLink16MissionInfoWordsType
           * @return A constant reference to this CommLink16MissionInfoWordsType.
           */
         const CommLink16MissionInfoWordsType& operator=(const CommLink16MissionInfoWordsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLink16MissionInfoWordsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLink16MissionInfoWordsType_h

