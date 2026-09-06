// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__RouteValidationRequestType_h
#define Uci__Type__RouteValidationRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__PathID_Type_h)
# include "uci/type/PathID_Type.h"
#endif

#if !defined(Uci__Type__WeatherAreaDataType_h)
# include "uci/type/WeatherAreaDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The inputs used in generating and/or requesting a Route Validation Assessment. */
      class RouteValidationRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RouteValidationRequestType()
         { }

         /** Returns this accessor's type constant, i.e. RouteValidationRequestType
           *
           * @return This accessor's type constant, i.e. RouteValidationRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::routeValidationRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RouteValidationRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanID
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathID.
           */
         virtual const uci::type::PathID_Type& getPathID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathID.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathID.
           */
         virtual uci::type::PathID_Type& getPathID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PathID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PathID
           */
         virtual void setPathID(const uci::type::PathID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PathID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PathID is emabled or not
           */
         virtual bool hasPathID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PathID
           *
           * @param type = uci::type::accessorType::pathID_Type This Accessor's accessor type
           */
         virtual void enablePathID(uci::base::accessorType::AccessorType type = uci::type::accessorType::pathID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PathID */
         virtual void clearPathID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherAreaData.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherAreaData.
           */
         virtual const uci::type::WeatherAreaDataType& getWeatherAreaData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherAreaData.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherAreaData.
           */
         virtual uci::type::WeatherAreaDataType& getWeatherAreaData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeatherAreaData to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeatherAreaData
           */
         virtual void setWeatherAreaData(const uci::type::WeatherAreaDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WeatherAreaData exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WeatherAreaData is emabled or not
           */
         virtual bool hasWeatherAreaData() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WeatherAreaData
           *
           * @param type = uci::type::accessorType::weatherAreaDataType This Accessor's accessor type
           */
         virtual void enableWeatherAreaData(uci::base::accessorType::AccessorType type = uci::type::accessorType::weatherAreaDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WeatherAreaData */
         virtual void clearWeatherAreaData()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RouteValidationRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RouteValidationRequestType to copy from
           */
         RouteValidationRequestType(const RouteValidationRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RouteValidationRequestType to the contents of the
           * RouteValidationRequestType on the right hand side (rhs) of the assignment operator.RouteValidationRequestType [only
           * available to derived classes]
           *
           * @param rhs The RouteValidationRequestType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::RouteValidationRequestType
           * @return A constant reference to this RouteValidationRequestType.
           */
         const RouteValidationRequestType& operator=(const RouteValidationRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RouteValidationRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RouteValidationRequestType_h

