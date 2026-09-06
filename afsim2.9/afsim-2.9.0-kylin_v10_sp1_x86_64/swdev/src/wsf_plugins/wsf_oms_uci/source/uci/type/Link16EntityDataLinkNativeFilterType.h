// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__Link16EntityDataLinkNativeFilterType_h
#define Uci__Type__Link16EntityDataLinkNativeFilterType_h 1

#if !defined(Uci__Type__DataLinkNativeFilterPET_h)
# include "uci/type/DataLinkNativeFilterPET.h"
#endif

#if !defined(Uci__Type__GatewayLink16ConfigurationClassEnum_h)
# include "uci/type/GatewayLink16ConfigurationClassEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CommLink16NPGType_h)
# include "uci/type/CommLink16NPGType.h"
#endif

#if !defined(Uci__Type__GatewayLink16ConfigurationAreaType_h)
# include "uci/type/GatewayLink16ConfigurationAreaType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__GatewayLink16ConfigurationEnvironmentType_h)
# include "uci/type/GatewayLink16ConfigurationEnvironmentType.h"
#endif

#if !defined(Uci__Type__GatewayLink16ConfigurationIdentityType_h)
# include "uci/type/GatewayLink16ConfigurationIdentityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base DataLinkNativeFilterPET for Link-16 Entity
        * Ingress / Egress.
        */
      class Link16EntityDataLinkNativeFilterType : public virtual uci::type::DataLinkNativeFilterPET {
      public:

         /** The destructor */
         virtual ~Link16EntityDataLinkNativeFilterType()
         { }

         /** Returns this accessor's type constant, i.e. Link16EntityDataLinkNativeFilterType
           *
           * @return This accessor's type constant, i.e. Link16EntityDataLinkNativeFilterType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::link16EntityDataLinkNativeFilterType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const Link16EntityDataLinkNativeFilterType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The Link-16 Network Participation Groups for which this Entity should be produced on or listened for on. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CommLink16NPGType, uci::type::accessorType::commLink16NPGType> Link16NPG;

         /** Returns the value of the enumeration that is identified by the Class.
           *
           * @return The value of the enumeration identified by Class.
           */
         virtual const uci::type::GatewayLink16ConfigurationClassEnum& getClass() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Class.
           *
           * @return The value of the enumeration identified by Class.
           */
         virtual uci::type::GatewayLink16ConfigurationClassEnum& getClass()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Class.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setClass(const uci::type::GatewayLink16ConfigurationClassEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Class.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setClass(uci::type::GatewayLink16ConfigurationClassEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SpecialProcessingIndicator.
           *
           * @return The value of the simple primitive data type identified by SpecialProcessingIndicator.
           */
         virtual xs::Boolean getSpecialProcessingIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SpecialProcessingIndicator.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSpecialProcessingIndicator(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SimulationIndicator.
           *
           * @return The value of the simple primitive data type identified by SimulationIndicator.
           */
         virtual xs::Boolean getSimulationIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SimulationIndicator.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSimulationIndicator(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TargetSorting12_6.
           *
           * @return The value of the simple primitive data type identified by TargetSorting12_6.
           */
         virtual xs::Boolean getTargetSorting12_6() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TargetSorting12_6.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTargetSorting12_6(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Track3X.
           *
           * @return The value of the simple primitive data type identified by Track3X.
           */
         virtual xs::Boolean getTrack3X() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Track3X.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTrack3X(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Link16NPG.
           *
           * @return The bounded list identified by Link16NPG.
           */
         virtual const uci::type::Link16EntityDataLinkNativeFilterType::Link16NPG& getLink16NPG() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Link16NPG.
           *
           * @return The bounded list identified by Link16NPG.
           */
         virtual uci::type::Link16EntityDataLinkNativeFilterType::Link16NPG& getLink16NPG()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Link16NPG.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLink16NPG(const uci::type::Link16EntityDataLinkNativeFilterType::Link16NPG& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeographicArea.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeographicArea.
           */
         virtual const uci::type::GatewayLink16ConfigurationAreaType& getGeographicArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeographicArea.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeographicArea.
           */
         virtual uci::type::GatewayLink16ConfigurationAreaType& getGeographicArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GeographicArea to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GeographicArea
           */
         virtual void setGeographicArea(const uci::type::GatewayLink16ConfigurationAreaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GeographicArea exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GeographicArea is emabled or not
           */
         virtual bool hasGeographicArea() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GeographicArea
           *
           * @param type = uci::type::accessorType::gatewayLink16ConfigurationAreaType This Accessor's accessor type
           */
         virtual void enableGeographicArea(uci::base::accessorType::AccessorType type = uci::type::accessorType::gatewayLink16ConfigurationAreaType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GeographicArea */
         virtual void clearGeographicArea()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Number.
           *
           * @return The value of the simple primitive data type identified by Number.
           */
         virtual xs::UnsignedInt getNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Number.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumber(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Number exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Number is emabled or not
           */
         virtual bool hasNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Number
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableNumber(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Number */
         virtual void clearNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Environment.
           *
           * @return The acecssor that provides access to the complex content that is identified by Environment.
           */
         virtual const uci::type::GatewayLink16ConfigurationEnvironmentType& getEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Environment.
           *
           * @return The acecssor that provides access to the complex content that is identified by Environment.
           */
         virtual uci::type::GatewayLink16ConfigurationEnvironmentType& getEnvironment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Environment to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Environment
           */
         virtual void setEnvironment(const uci::type::GatewayLink16ConfigurationEnvironmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Environment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Environment is emabled or not
           */
         virtual bool hasEnvironment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Environment
           *
           * @param type = uci::type::accessorType::gatewayLink16ConfigurationEnvironmentType This Accessor's accessor type
           */
         virtual void enableEnvironment(uci::base::accessorType::AccessorType type = uci::type::accessorType::gatewayLink16ConfigurationEnvironmentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Environment */
         virtual void clearEnvironment()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::GatewayLink16ConfigurationIdentityType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::GatewayLink16ConfigurationIdentityType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::GatewayLink16ConfigurationIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Identity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Identity is emabled or not
           */
         virtual bool hasIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Identity
           *
           * @param type = uci::type::accessorType::gatewayLink16ConfigurationIdentityType This Accessor's accessor type
           */
         virtual void enableIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::gatewayLink16ConfigurationIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Identity */
         virtual void clearIdentity()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         Link16EntityDataLinkNativeFilterType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Link16EntityDataLinkNativeFilterType to copy from
           */
         Link16EntityDataLinkNativeFilterType(const Link16EntityDataLinkNativeFilterType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Link16EntityDataLinkNativeFilterType to the contents of the
           * Link16EntityDataLinkNativeFilterType on the right hand side (rhs) of the assignment
           * operator.Link16EntityDataLinkNativeFilterType [only available to derived classes]
           *
           * @param rhs The Link16EntityDataLinkNativeFilterType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::Link16EntityDataLinkNativeFilterType
           * @return A constant reference to this Link16EntityDataLinkNativeFilterType.
           */
         const Link16EntityDataLinkNativeFilterType& operator=(const Link16EntityDataLinkNativeFilterType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Link16EntityDataLinkNativeFilterType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__Link16EntityDataLinkNativeFilterType_h

