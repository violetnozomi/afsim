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
#ifndef Uci__Type__ZoneType_h
#define Uci__Type__ZoneType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AreaChoiceType_h)
# include "uci/type/AreaChoiceType.h"
#endif

#if !defined(Uci__Type__AltitudeType_h)
# include "uci/type/AltitudeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** A zone comprises a two dimension region and an optional altitude band. */
      class ZoneType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ZoneType()
         { }

         /** Returns this accessor's type constant, i.e. ZoneType
           *
           * @return This accessor's type constant, i.e. ZoneType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::zoneType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ZoneType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Category.
           *
           * @return The acecssor that provides access to the complex content that is identified by Category.
           */
         virtual const uci::type::AreaChoiceType& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Category.
           *
           * @return The acecssor that provides access to the complex content that is identified by Category.
           */
         virtual uci::type::AreaChoiceType& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Category to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Category
           */
         virtual void setCategory(const uci::type::AreaChoiceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinAltitude.
           *
           * @return The value of the simple primitive data type identified by MinAltitude.
           */
         virtual uci::type::AltitudeTypeValue getMinAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinAltitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinAltitude(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinAltitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinAltitude is emabled or not
           */
         virtual bool hasMinAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinAltitude
           *
           * @param type = uci::type::accessorType::altitudeType This Accessor's accessor type
           */
         virtual void enableMinAltitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinAltitude */
         virtual void clearMinAltitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaxAltitude.
           *
           * @return The value of the simple primitive data type identified by MaxAltitude.
           */
         virtual uci::type::AltitudeTypeValue getMaxAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaxAltitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaxAltitude(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaxAltitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaxAltitude is emabled or not
           */
         virtual bool hasMaxAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaxAltitude
           *
           * @param type = uci::type::accessorType::altitudeType This Accessor's accessor type
           */
         virtual void enableMaxAltitude(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaxAltitude */
         virtual void clearMaxAltitude()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ZoneType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ZoneType to copy from
           */
         ZoneType(const ZoneType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ZoneType to the contents of the ZoneType on the right hand side
           * (rhs) of the assignment operator.ZoneType [only available to derived classes]
           *
           * @param rhs The ZoneType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::ZoneType
           * @return A constant reference to this ZoneType.
           */
         const ZoneType& operator=(const ZoneType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ZoneType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ZoneType_h

