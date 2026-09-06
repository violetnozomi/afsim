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
#ifndef Uci__Type__SatelliteIdentifierType_h
#define Uci__Type__SatelliteIdentifierType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SatelliteNumberType_h)
# include "uci/type/SatelliteNumberType.h"
#endif

#if !defined(Uci__Type__SatelliteNameType_h)
# include "uci/type/SatelliteNameType.h"
#endif

#if !defined(Uci__Type__InternationalDesignatorType_h)
# include "uci/type/InternationalDesignatorType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** The tuple of NORAD Catalog Number, Name, and International Designator that identifies a Satellite. */
      class SatelliteIdentifierType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SatelliteIdentifierType()
         { }

         /** Returns this accessor's type constant, i.e. SatelliteIdentifierType
           *
           * @return This accessor's type constant, i.e. SatelliteIdentifierType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::satelliteIdentifierType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SatelliteIdentifierType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SatelliteNumber.
           *
           * @return The value of the simple primitive data type identified by SatelliteNumber.
           */
         virtual uci::type::SatelliteNumberTypeValue getSatelliteNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SatelliteNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSatelliteNumber(uci::type::SatelliteNumberTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SatelliteName.
           *
           * @return The value of the string data type identified by SatelliteName.
           */
         virtual const uci::type::SatelliteNameType& getSatelliteName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SatelliteName.
           *
           * @return The value of the string data type identified by SatelliteName.
           */
         virtual uci::type::SatelliteNameType& getSatelliteName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SatelliteName to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSatelliteName(const uci::type::SatelliteNameType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SatelliteName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSatelliteName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SatelliteName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSatelliteName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SatelliteName exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SatelliteName is emabled or not
           */
         virtual bool hasSatelliteName() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SatelliteName
           *
           * @param type = uci::type::accessorType::satelliteNameType This Accessor's accessor type
           */
         virtual void enableSatelliteName(uci::base::accessorType::AccessorType type = uci::type::accessorType::satelliteNameType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SatelliteName */
         virtual void clearSatelliteName()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InternationalDesignator.
           *
           * @return The acecssor that provides access to the complex content that is identified by InternationalDesignator.
           */
         virtual const uci::type::InternationalDesignatorType& getInternationalDesignator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InternationalDesignator.
           *
           * @return The acecssor that provides access to the complex content that is identified by InternationalDesignator.
           */
         virtual uci::type::InternationalDesignatorType& getInternationalDesignator()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InternationalDesignator to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InternationalDesignator
           */
         virtual void setInternationalDesignator(const uci::type::InternationalDesignatorType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InternationalDesignator exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InternationalDesignator is emabled or not
           */
         virtual bool hasInternationalDesignator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InternationalDesignator
           *
           * @param type = uci::type::accessorType::internationalDesignatorType This Accessor's accessor type
           */
         virtual void enableInternationalDesignator(uci::base::accessorType::AccessorType type = uci::type::accessorType::internationalDesignatorType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InternationalDesignator */
         virtual void clearInternationalDesignator()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SatelliteIdentifierType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SatelliteIdentifierType to copy from
           */
         SatelliteIdentifierType(const SatelliteIdentifierType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SatelliteIdentifierType to the contents of the
           * SatelliteIdentifierType on the right hand side (rhs) of the assignment operator.SatelliteIdentifierType [only
           * available to derived classes]
           *
           * @param rhs The SatelliteIdentifierType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SatelliteIdentifierType
           * @return A constant reference to this SatelliteIdentifierType.
           */
         const SatelliteIdentifierType& operator=(const SatelliteIdentifierType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SatelliteIdentifierType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SatelliteIdentifierType_h

