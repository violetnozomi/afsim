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
#ifndef Uci__Type__ThreatNominationResultType_h
#define Uci__Type__ThreatNominationResultType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EntityNominationResultType_h)
# include "uci/type/EntityNominationResultType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ThreatNominationResultType sequence accessor class */
      class ThreatNominationResultType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThreatNominationResultType()
         { }

         /** Returns this accessor's type constant, i.e. ThreatNominationResultType
           *
           * @return This accessor's type constant, i.e. ThreatNominationResultType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threatNominationResultType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreatNominationResultType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A list of threat nomination results for each individual system. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::EntityNominationResultType, uci::type::accessorType::entityNominationResultType> EntityNominationResults;

         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual const uci::type::EntityID_Type& getEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EntityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EntityID.
           */
         virtual uci::type::EntityID_Type& getEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EntityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EntityID
           */
         virtual void setEntityID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityNominationResults.
           *
           * @return The bounded list identified by EntityNominationResults.
           */
         virtual const uci::type::ThreatNominationResultType::EntityNominationResults& getEntityNominationResults() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EntityNominationResults.
           *
           * @return The bounded list identified by EntityNominationResults.
           */
         virtual uci::type::ThreatNominationResultType::EntityNominationResults& getEntityNominationResults()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EntityNominationResults.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEntityNominationResults(const uci::type::ThreatNominationResultType::EntityNominationResults& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GlobalExposureReduction.
           *
           * @return The value of the simple primitive data type identified by GlobalExposureReduction.
           */
         virtual uci::type::PercentTypeValue getGlobalExposureReduction() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GlobalExposureReduction.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGlobalExposureReduction(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GlobalFlightTimeReduction.
           *
           * @return The value of the simple primitive data type identified by GlobalFlightTimeReduction.
           */
         virtual uci::type::PercentTypeValue getGlobalFlightTimeReduction() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GlobalFlightTimeReduction.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGlobalFlightTimeReduction(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GlobalFlightTimeReduction exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GlobalFlightTimeReduction is emabled or not
           */
         virtual bool hasGlobalFlightTimeReduction() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GlobalFlightTimeReduction
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableGlobalFlightTimeReduction(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GlobalFlightTimeReduction */
         virtual void clearGlobalFlightTimeReduction()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatNominationResultType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatNominationResultType to copy from
           */
         ThreatNominationResultType(const ThreatNominationResultType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatNominationResultType to the contents of the
           * ThreatNominationResultType on the right hand side (rhs) of the assignment operator.ThreatNominationResultType [only
           * available to derived classes]
           *
           * @param rhs The ThreatNominationResultType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ThreatNominationResultType
           * @return A constant reference to this ThreatNominationResultType.
           */
         const ThreatNominationResultType& operator=(const ThreatNominationResultType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatNominationResultType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreatNominationResultType_h

