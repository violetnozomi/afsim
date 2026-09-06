// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__WeatherDatasetMDT_h
#define Uci__Type__WeatherDatasetMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__WeatherDatasetID_Type_h)
# include "uci/type/WeatherDatasetID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Type__AnglePositiveType_h)
# include "uci/type/AnglePositiveType.h"
#endif

#if !defined(Uci__Type__AltitudeType_h)
# include "uci/type/AltitudeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__WeatherDataType_h)
# include "uci/type/WeatherDataType.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ScheduleType_h)
# include "uci/type/ScheduleType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherDatasetMDT sequence accessor class */
      class WeatherDatasetMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~WeatherDatasetMDT()
         { }

         /** Returns this accessor's type constant, i.e. WeatherDatasetMDT
           *
           * @return This accessor's type constant, i.e. WeatherDatasetMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherDatasetMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherDatasetMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element contains weather data for the dataset that should be used for planning purposes. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::WeatherDataType, uci::type::accessorType::weatherDataType> WeatherData;

         /** One or more Systems to which the weather dataset apply. If omitted, the dataset applies to all Systems. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> SystemID;

         /** Returns the accessor that provides access to the complex content that is identified by the WeatherDatasetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherDatasetID.
           */
         virtual const uci::type::WeatherDatasetID_Type& getWeatherDatasetID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the WeatherDatasetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by WeatherDatasetID.
           */
         virtual uci::type::WeatherDatasetID_Type& getWeatherDatasetID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the WeatherDatasetID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by WeatherDatasetID
           */
         virtual void setWeatherDatasetID(const uci::type::WeatherDatasetID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Created.
           *
           * @return The value of the simple primitive data type identified by Created.
           */
         virtual uci::type::DateTimeTypeValue getCreated() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Created.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCreated(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Updated.
           *
           * @return The value of the simple primitive data type identified by Updated.
           */
         virtual uci::type::DateTimeTypeValue getUpdated() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Updated.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUpdated(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DatasetArea.
           *
           * @return The acecssor that provides access to the complex content that is identified by DatasetArea.
           */
         virtual const uci::type::ZoneType& getDatasetArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DatasetArea.
           *
           * @return The acecssor that provides access to the complex content that is identified by DatasetArea.
           */
         virtual uci::type::ZoneType& getDatasetArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DatasetArea to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DatasetArea
           */
         virtual void setDatasetArea(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DegreeStep.
           *
           * @return The value of the simple primitive data type identified by DegreeStep.
           */
         virtual uci::type::AnglePositiveTypeValue getDegreeStep() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DegreeStep.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDegreeStep(uci::type::AnglePositiveTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DegreeStep exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DegreeStep is emabled or not
           */
         virtual bool hasDegreeStep() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DegreeStep
           *
           * @param type = uci::type::accessorType::anglePositiveType This Accessor's accessor type
           */
         virtual void enableDegreeStep(uci::base::accessorType::AccessorType type = uci::type::accessorType::anglePositiveType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DegreeStep */
         virtual void clearDegreeStep()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AltitudeStep.
           *
           * @return The value of the simple primitive data type identified by AltitudeStep.
           */
         virtual uci::type::AltitudeTypeValue getAltitudeStep() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AltitudeStep.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAltitudeStep(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AltitudeStep exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AltitudeStep is emabled or not
           */
         virtual bool hasAltitudeStep() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AltitudeStep
           *
           * @param type = uci::type::accessorType::altitudeType This Accessor's accessor type
           */
         virtual void enableAltitudeStep(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AltitudeStep */
         virtual void clearAltitudeStep()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the WeatherData.
           *
           * @return The bounded list identified by WeatherData.
           */
         virtual const uci::type::WeatherDatasetMDT::WeatherData& getWeatherData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the WeatherData.
           *
           * @return The bounded list identified by WeatherData.
           */
         virtual uci::type::WeatherDatasetMDT::WeatherData& getWeatherData()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the WeatherData.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWeatherData(const uci::type::WeatherDatasetMDT::WeatherData& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual const uci::type::WeatherDatasetMDT::SystemID& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SystemID.
           *
           * @return The bounded list identified by SystemID.
           */
         virtual uci::type::WeatherDatasetMDT::SystemID& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSystemID(const uci::type::WeatherDatasetMDT::SystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Schedule.
           *
           * @return The acecssor that provides access to the complex content that is identified by Schedule.
           */
         virtual const uci::type::ScheduleType& getSchedule() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Schedule.
           *
           * @return The acecssor that provides access to the complex content that is identified by Schedule.
           */
         virtual uci::type::ScheduleType& getSchedule()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Schedule to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Schedule
           */
         virtual void setSchedule(const uci::type::ScheduleType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Schedule exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Schedule is emabled or not
           */
         virtual bool hasSchedule() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Schedule
           *
           * @param type = uci::type::accessorType::scheduleType This Accessor's accessor type
           */
         virtual void enableSchedule(uci::base::accessorType::AccessorType type = uci::type::accessorType::scheduleType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Schedule */
         virtual void clearSchedule()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherDatasetMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherDatasetMDT to copy from
           */
         WeatherDatasetMDT(const WeatherDatasetMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherDatasetMDT to the contents of the WeatherDatasetMDT on the
           * right hand side (rhs) of the assignment operator.WeatherDatasetMDT [only available to derived classes]
           *
           * @param rhs The WeatherDatasetMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::WeatherDatasetMDT
           * @return A constant reference to this WeatherDatasetMDT.
           */
         const WeatherDatasetMDT& operator=(const WeatherDatasetMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherDatasetMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherDatasetMDT_h

