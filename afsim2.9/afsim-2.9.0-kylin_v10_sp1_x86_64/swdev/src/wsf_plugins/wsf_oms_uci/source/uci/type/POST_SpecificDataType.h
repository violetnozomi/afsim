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
#ifndef Uci__Type__POST_SpecificDataType_h
#define Uci__Type__POST_SpecificDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__POST_CapabilityEnum_h)
# include "uci/type/POST_CapabilityEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_SpecificDataType sequence accessor class */
      class POST_SpecificDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_SpecificDataType()
         { }

         /** Returns this accessor's type constant, i.e. POST_SpecificDataType
           *
           * @return This accessor's type constant, i.e. POST_SpecificDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_SpecificDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_SpecificDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual const uci::type::POST_CapabilityEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::POST_CapabilityEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::POST_CapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::POST_CapabilityEnum::EnumerationItem value)
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
           * @param type = uci::type::accessorType::pOST_CapabilityEnum This Accessor's accessor type
           */
         virtual void enableCapabilityType(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_CapabilityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearCapabilityType()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_SpecificDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_SpecificDataType to copy from
           */
         POST_SpecificDataType(const POST_SpecificDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_SpecificDataType to the contents of the POST_SpecificDataType
           * on the right hand side (rhs) of the assignment operator.POST_SpecificDataType [only available to derived classes]
           *
           * @param rhs The POST_SpecificDataType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::POST_SpecificDataType
           * @return A constant reference to this POST_SpecificDataType.
           */
         const POST_SpecificDataType& operator=(const POST_SpecificDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_SpecificDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_SpecificDataType_h

