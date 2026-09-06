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
#ifndef Uci__Type__COMINT_SubcapabilityDataCollectType_h
#define Uci__Type__COMINT_SubcapabilityDataCollectType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__COMINT_DataCollectCommandType_h)
# include "uci/type/COMINT_DataCollectCommandType.h"
#endif

#if !defined(Uci__Type__COMINT_SubcapabilityTargetLocationDataType_h)
# include "uci/type/COMINT_SubcapabilityTargetLocationDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_SubcapabilityDataCollectType sequence accessor class */
      class COMINT_SubcapabilityDataCollectType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_SubcapabilityDataCollectType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_SubcapabilityDataCollectType
           *
           * @return This accessor's type constant, i.e. COMINT_SubcapabilityDataCollectType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_SubcapabilityDataCollectType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_SubcapabilityDataCollectType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Collection.
           *
           * @return The acecssor that provides access to the complex content that is identified by Collection.
           */
         virtual const uci::type::COMINT_DataCollectCommandType& getCollection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Collection.
           *
           * @return The acecssor that provides access to the complex content that is identified by Collection.
           */
         virtual uci::type::COMINT_DataCollectCommandType& getCollection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Collection to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Collection
           */
         virtual void setCollection(const uci::type::COMINT_DataCollectCommandType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual const uci::type::COMINT_SubcapabilityTargetLocationDataType& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual uci::type::COMINT_SubcapabilityTargetLocationDataType& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Target to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Target
           */
         virtual void setTarget(const uci::type::COMINT_SubcapabilityTargetLocationDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Target exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Target is emabled or not
           */
         virtual bool hasTarget() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Target
           *
           * @param type = uci::type::accessorType::cOMINT_SubcapabilityTargetLocationDataType This Accessor's accessor type
           */
         virtual void enableTarget(uci::base::accessorType::AccessorType type = uci::type::accessorType::cOMINT_SubcapabilityTargetLocationDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Target */
         virtual void clearTarget()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_SubcapabilityDataCollectType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_SubcapabilityDataCollectType to copy from
           */
         COMINT_SubcapabilityDataCollectType(const COMINT_SubcapabilityDataCollectType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_SubcapabilityDataCollectType to the contents of the
           * COMINT_SubcapabilityDataCollectType on the right hand side (rhs) of the assignment
           * operator.COMINT_SubcapabilityDataCollectType [only available to derived classes]
           *
           * @param rhs The COMINT_SubcapabilityDataCollectType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::COMINT_SubcapabilityDataCollectType
           * @return A constant reference to this COMINT_SubcapabilityDataCollectType.
           */
         const COMINT_SubcapabilityDataCollectType& operator=(const COMINT_SubcapabilityDataCollectType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_SubcapabilityDataCollectType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_SubcapabilityDataCollectType_h

