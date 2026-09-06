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
#ifndef Uci__Type__WeatherFiltersType_h
#define Uci__Type__WeatherFiltersType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__WeatherDataTypeSimpleEnum_h)
# include "uci/type/WeatherDataTypeSimpleEnum.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the WeatherFiltersType sequence accessor class */
      class WeatherFiltersType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~WeatherFiltersType()
         { }

         /** Returns this accessor's type constant, i.e. WeatherFiltersType
           *
           * @return This accessor's type constant, i.e. WeatherFiltersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::weatherFiltersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const WeatherFiltersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Type of weather data requested in the queried image. More than one instance of this element indicates a logical AND.
           * [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::WeatherDataTypeSimpleEnum, uci::type::accessorType::weatherDataTypeSimpleEnum> WeatherDataType;

         /** Returns the bounded list that is identified by the WeatherDataType.
           *
           * @return The bounded list identified by WeatherDataType.
           */
         virtual const uci::type::WeatherFiltersType::WeatherDataType& getWeatherDataType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the WeatherDataType.
           *
           * @return The bounded list identified by WeatherDataType.
           */
         virtual uci::type::WeatherFiltersType::WeatherDataType& getWeatherDataType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the WeatherDataType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setWeatherDataType(const uci::type::WeatherFiltersType::WeatherDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RequiredPixelWidth.
           *
           * @return The value of the simple primitive data type identified by RequiredPixelWidth.
           */
         virtual xs::UnsignedInt getRequiredPixelWidth() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RequiredPixelWidth.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRequiredPixelWidth(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RequiredPixelWidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RequiredPixelWidth is emabled or not
           */
         virtual bool hasRequiredPixelWidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RequiredPixelWidth
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableRequiredPixelWidth(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RequiredPixelWidth */
         virtual void clearRequiredPixelWidth()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RequiredPixelHeight.
           *
           * @return The value of the simple primitive data type identified by RequiredPixelHeight.
           */
         virtual xs::UnsignedInt getRequiredPixelHeight() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RequiredPixelHeight.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRequiredPixelHeight(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RequiredPixelHeight exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RequiredPixelHeight is emabled or not
           */
         virtual bool hasRequiredPixelHeight() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RequiredPixelHeight
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableRequiredPixelHeight(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RequiredPixelHeight */
         virtual void clearRequiredPixelHeight()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         WeatherFiltersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The WeatherFiltersType to copy from
           */
         WeatherFiltersType(const WeatherFiltersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this WeatherFiltersType to the contents of the WeatherFiltersType on
           * the right hand side (rhs) of the assignment operator.WeatherFiltersType [only available to derived classes]
           *
           * @param rhs The WeatherFiltersType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::WeatherFiltersType
           * @return A constant reference to this WeatherFiltersType.
           */
         const WeatherFiltersType& operator=(const WeatherFiltersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // WeatherFiltersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__WeatherFiltersType_h

