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
#ifndef Uci__Type__OperatorLocationOfInterestClauseType_h
#define Uci__Type__OperatorLocationOfInterestClauseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OperatorLocationOfInterestComparativeType_h)
# include "uci/type/OperatorLocationOfInterestComparativeType.h"
#endif

#if !defined(Uci__Type__OperatorLocationOfInterestID_Type_h)
# include "uci/type/OperatorLocationOfInterestID_Type.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OperatorLocationOfInterestClauseType sequence accessor class */
      class OperatorLocationOfInterestClauseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OperatorLocationOfInterestClauseType()
         { }

         /** Returns this accessor's type constant, i.e. OperatorLocationOfInterestClauseType
           *
           * @return This accessor's type constant, i.e. OperatorLocationOfInterestClauseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operatorLocationOfInterestClauseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperatorLocationOfInterestClauseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element indicates an OperatorLocationOfInterest related clause (requiring a comparative test between a value in
           * the OperatorLocationOfInterest associated with the Trigger, and/or a related message, and a value given here) that
           * must be met before the response for the Rule is initiated. Multiple instances or up to two non-conflicting
           * comparators are allowed and are logically ANDed. [Minimum occurrences: 0] [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::OperatorLocationOfInterestComparativeType, uci::type::accessorType::operatorLocationOfInterestComparativeType> Comparative;

         /** Indicates the unique ID of a specific OLOI instance that the Rule applies to. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::OperatorLocationOfInterestID_Type, uci::type::accessorType::operatorLocationOfInterestID_Type> OperatorLocationOfInterestID;

         /** Returns the value of the SimplePrimitive data type that is identified by the DesignatedTarget.
           *
           * @return The value of the simple primitive data type identified by DesignatedTarget.
           */
         virtual xs::Boolean getDesignatedTarget() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DesignatedTarget.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDesignatedTarget(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Comparative.
           *
           * @return The bounded list identified by Comparative.
           */
         virtual const uci::type::OperatorLocationOfInterestClauseType::Comparative& getComparative() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Comparative.
           *
           * @return The bounded list identified by Comparative.
           */
         virtual uci::type::OperatorLocationOfInterestClauseType::Comparative& getComparative()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Comparative.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComparative(const uci::type::OperatorLocationOfInterestClauseType::Comparative& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorLocationOfInterestID.
           *
           * @return The bounded list identified by OperatorLocationOfInterestID.
           */
         virtual const uci::type::OperatorLocationOfInterestClauseType::OperatorLocationOfInterestID& getOperatorLocationOfInterestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OperatorLocationOfInterestID.
           *
           * @return The bounded list identified by OperatorLocationOfInterestID.
           */
         virtual uci::type::OperatorLocationOfInterestClauseType::OperatorLocationOfInterestID& getOperatorLocationOfInterestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OperatorLocationOfInterestID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOperatorLocationOfInterestID(const uci::type::OperatorLocationOfInterestClauseType::OperatorLocationOfInterestID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableZone.
           */
         virtual const uci::type::ZoneType& getApplicableZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApplicableZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApplicableZone.
           */
         virtual uci::type::ZoneType& getApplicableZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApplicableZone to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApplicableZone
           */
         virtual void setApplicableZone(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ApplicableZone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ApplicableZone is emabled or not
           */
         virtual bool hasApplicableZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ApplicableZone
           *
           * @param type = uci::type::accessorType::zoneType This Accessor's accessor type
           */
         virtual void enableApplicableZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ApplicableZone */
         virtual void clearApplicableZone()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperatorLocationOfInterestClauseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperatorLocationOfInterestClauseType to copy from
           */
         OperatorLocationOfInterestClauseType(const OperatorLocationOfInterestClauseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperatorLocationOfInterestClauseType to the contents of the
           * OperatorLocationOfInterestClauseType on the right hand side (rhs) of the assignment
           * operator.OperatorLocationOfInterestClauseType [only available to derived classes]
           *
           * @param rhs The OperatorLocationOfInterestClauseType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::OperatorLocationOfInterestClauseType
           * @return A constant reference to this OperatorLocationOfInterestClauseType.
           */
         const OperatorLocationOfInterestClauseType& operator=(const OperatorLocationOfInterestClauseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperatorLocationOfInterestClauseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperatorLocationOfInterestClauseType_h

