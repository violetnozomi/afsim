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
#ifndef Uci__Type__ObjectCorrelationType_h
#define Uci__Type__ObjectCorrelationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CorrelationIndicatorEnum_h)
# include "uci/type/CorrelationIndicatorEnum.h"
#endif

#if !defined(Uci__Type__ThreatTargetPrioritizationItemType_h)
# include "uci/type/ThreatTargetPrioritizationItemType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Describes the correlation of objects */
      class ObjectCorrelationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ObjectCorrelationType()
         { }

         /** Returns this accessor's type constant, i.e. ObjectCorrelationType
           *
           * @return This accessor's type constant, i.e. ObjectCorrelationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::objectCorrelationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ObjectCorrelationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CorrelationIndicator.
           *
           * @return The value of the enumeration identified by CorrelationIndicator.
           */
         virtual const uci::type::CorrelationIndicatorEnum& getCorrelationIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CorrelationIndicator.
           *
           * @return The value of the enumeration identified by CorrelationIndicator.
           */
         virtual uci::type::CorrelationIndicatorEnum& getCorrelationIndicator()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CorrelationIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCorrelationIndicator(const uci::type::CorrelationIndicatorEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CorrelationIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCorrelationIndicator(uci::type::CorrelationIndicatorEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OldReferenceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OldReferenceID.
           */
         virtual const uci::type::ThreatTargetPrioritizationItemType& getOldReferenceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OldReferenceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OldReferenceID.
           */
         virtual uci::type::ThreatTargetPrioritizationItemType& getOldReferenceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OldReferenceID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OldReferenceID
           */
         virtual void setOldReferenceID(const uci::type::ThreatTargetPrioritizationItemType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ObjectCorrelationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ObjectCorrelationType to copy from
           */
         ObjectCorrelationType(const ObjectCorrelationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ObjectCorrelationType to the contents of the ObjectCorrelationType
           * on the right hand side (rhs) of the assignment operator.ObjectCorrelationType [only available to derived classes]
           *
           * @param rhs The ObjectCorrelationType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ObjectCorrelationType
           * @return A constant reference to this ObjectCorrelationType.
           */
         const ObjectCorrelationType& operator=(const ObjectCorrelationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ObjectCorrelationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ObjectCorrelationType_h

