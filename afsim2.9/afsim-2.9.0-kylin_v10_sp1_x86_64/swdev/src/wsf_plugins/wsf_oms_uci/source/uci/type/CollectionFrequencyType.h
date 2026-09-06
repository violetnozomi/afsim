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
#ifndef Uci__Type__CollectionFrequencyType_h
#define Uci__Type__CollectionFrequencyType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CollectionFrequencyType sequence accessor class */
      class CollectionFrequencyType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CollectionFrequencyType()
         { }

         /** Returns this accessor's type constant, i.e. CollectionFrequencyType
           *
           * @return This accessor's type constant, i.e. CollectionFrequencyType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::collectionFrequencyType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CollectionFrequencyType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TuneFrequency.
           *
           * @return The value of the simple primitive data type identified by TuneFrequency.
           */
         virtual uci::type::FrequencyTypeValue getTuneFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TuneFrequency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTuneFrequency(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Range.
           *
           * @return The acecssor that provides access to the complex content that is identified by Range.
           */
         virtual const uci::type::FrequencyRangeType& getRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Range.
           *
           * @return The acecssor that provides access to the complex content that is identified by Range.
           */
         virtual uci::type::FrequencyRangeType& getRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Range to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Range
           */
         virtual void setRange(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Range exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Range is emabled or not
           */
         virtual bool hasRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Range
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Range */
         virtual void clearRange()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CollectionFrequencyType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CollectionFrequencyType to copy from
           */
         CollectionFrequencyType(const CollectionFrequencyType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CollectionFrequencyType to the contents of the
           * CollectionFrequencyType on the right hand side (rhs) of the assignment operator.CollectionFrequencyType [only
           * available to derived classes]
           *
           * @param rhs The CollectionFrequencyType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CollectionFrequencyType
           * @return A constant reference to this CollectionFrequencyType.
           */
         const CollectionFrequencyType& operator=(const CollectionFrequencyType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CollectionFrequencyType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CollectionFrequencyType_h

