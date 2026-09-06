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
#ifndef Uci__Type__SAR_SpecificDataType_h
#define Uci__Type__SAR_SpecificDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SAR_CapabilityEnum_h)
# include "uci/type/SAR_CapabilityEnum.h"
#endif

#if !defined(Uci__Type__SAR_SubCapabilityEnum_h)
# include "uci/type/SAR_SubCapabilityEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_SpecificDataType sequence accessor class */
      class SAR_SpecificDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SAR_SpecificDataType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_SpecificDataType
           *
           * @return This accessor's type constant, i.e. SAR_SpecificDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_SpecificDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_SpecificDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual const uci::type::SAR_CapabilityEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::SAR_CapabilityEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::SAR_CapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::SAR_CapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield CapabilityTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getCapabilityType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::sAR_CapabilityEnum This Accessor's accessor type
           */
         virtual void enableCapabilityType(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_CapabilityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubCapabilityType.
           *
           * @return The value of the enumeration identified by SubCapabilityType.
           */
         virtual const uci::type::SAR_SubCapabilityEnum& getSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubCapabilityType.
           *
           * @return The value of the enumeration identified by SubCapabilityType.
           */
         virtual uci::type::SAR_SubCapabilityEnum& getSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubCapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubCapabilityType(const uci::type::SAR_SubCapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubCapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubCapabilityType(uci::type::SAR_SubCapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SubCapabilityTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSubCapabilityType will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::sAR_SubCapabilityEnum This Accessor's accessor type
           */
         virtual void enableSubCapabilityType(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_SubCapabilityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSubCapabilityType()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_SpecificDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_SpecificDataType to copy from
           */
         SAR_SpecificDataType(const SAR_SpecificDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_SpecificDataType to the contents of the SAR_SpecificDataType
           * on the right hand side (rhs) of the assignment operator.SAR_SpecificDataType [only available to derived classes]
           *
           * @param rhs The SAR_SpecificDataType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::SAR_SpecificDataType
           * @return A constant reference to this SAR_SpecificDataType.
           */
         const SAR_SpecificDataType& operator=(const SAR_SpecificDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_SpecificDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_SpecificDataType_h

