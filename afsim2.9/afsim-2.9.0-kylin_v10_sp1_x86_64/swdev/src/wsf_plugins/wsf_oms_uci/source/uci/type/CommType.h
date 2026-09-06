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
#ifndef Uci__Type__CommType_h
#define Uci__Type__CommType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommCapabilityEnum_h)
# include "uci/type/CommCapabilityEnum.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommType sequence accessor class */
      class CommType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommType()
         { }

         /** Returns this accessor's type constant, i.e. CommType
           *
           * @return This accessor's type constant, i.e. CommType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual const uci::type::CommCapabilityEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::CommCapabilityEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::CommCapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::CommCapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommTypeVariant.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommTypeVariant.
           */
         virtual const uci::type::ForeignKeyType& getCommTypeVariant() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommTypeVariant.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommTypeVariant.
           */
         virtual uci::type::ForeignKeyType& getCommTypeVariant()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommTypeVariant to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommTypeVariant
           */
         virtual void setCommTypeVariant(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommTypeVariant exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommTypeVariant is emabled or not
           */
         virtual bool hasCommTypeVariant() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommTypeVariant
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableCommTypeVariant(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommTypeVariant */
         virtual void clearCommTypeVariant()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommType to copy from
           */
         CommType(const CommType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommType to the contents of the CommType on the right hand side
           * (rhs) of the assignment operator.CommType [only available to derived classes]
           *
           * @param rhs The CommType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::CommType
           * @return A constant reference to this CommType.
           */
         const CommType& operator=(const CommType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommType_h

