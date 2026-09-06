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
#ifndef Uci__Type__ThreatTargetPrioritizationType_h
#define Uci__Type__ThreatTargetPrioritizationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ThreatTargetPrioritizationItemType_h)
# include "uci/type/ThreatTargetPrioritizationItemType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ThreatTargetPrioritizationPriorityType_h)
# include "uci/type/ThreatTargetPrioritizationPriorityType.h"
#endif

#if !defined(Uci__Type__ObjectCorrelationType_h)
# include "uci/type/ObjectCorrelationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ThreatTargetPrioritizationType sequence accessor class */
      class ThreatTargetPrioritizationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThreatTargetPrioritizationType()
         { }

         /** Returns this accessor's type constant, i.e. ThreatTargetPrioritizationType
           *
           * @return This accessor's type constant, i.e. ThreatTargetPrioritizationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::threatTargetPrioritizationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThreatTargetPrioritizationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the priority of the TTPL item. If priority for any child PriorityType is given, then omitted PriorityTypes
           * for the sibling ItemType have no/zero priority. No duplicates allowed; at most one instance of each of the child
           * PriorityTypes is allowed. If the child PriorityType is not used, only one instance is allowed. [Maximum occurrences:
           * 8]
           */
         typedef uci::base::BoundedList<uci::type::ThreatTargetPrioritizationPriorityType, uci::type::accessorType::threatTargetPrioritizationPriorityType> Priority;

         /** Returns the accessor that provides access to the complex content that is identified by the ItemType.
           *
           * @return The acecssor that provides access to the complex content that is identified by ItemType.
           */
         virtual const uci::type::ThreatTargetPrioritizationItemType& getItemType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ItemType.
           *
           * @return The acecssor that provides access to the complex content that is identified by ItemType.
           */
         virtual uci::type::ThreatTargetPrioritizationItemType& getItemType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ItemType to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ItemType
           */
         virtual void setItemType(const uci::type::ThreatTargetPrioritizationItemType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Priority.
           *
           * @return The bounded list identified by Priority.
           */
         virtual const uci::type::ThreatTargetPrioritizationType::Priority& getPriority() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Priority.
           *
           * @return The bounded list identified by Priority.
           */
         virtual uci::type::ThreatTargetPrioritizationType::Priority& getPriority()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Priority.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setPriority(const uci::type::ThreatTargetPrioritizationType::Priority& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ObjectCorrelation.
           *
           * @return The acecssor that provides access to the complex content that is identified by ObjectCorrelation.
           */
         virtual const uci::type::ObjectCorrelationType& getObjectCorrelation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ObjectCorrelation.
           *
           * @return The acecssor that provides access to the complex content that is identified by ObjectCorrelation.
           */
         virtual uci::type::ObjectCorrelationType& getObjectCorrelation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ObjectCorrelation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ObjectCorrelation
           */
         virtual void setObjectCorrelation(const uci::type::ObjectCorrelationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ObjectCorrelation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ObjectCorrelation is emabled or not
           */
         virtual bool hasObjectCorrelation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ObjectCorrelation
           *
           * @param type = uci::type::accessorType::objectCorrelationType This Accessor's accessor type
           */
         virtual void enableObjectCorrelation(uci::base::accessorType::AccessorType type = uci::type::accessorType::objectCorrelationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ObjectCorrelation */
         virtual void clearObjectCorrelation()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThreatTargetPrioritizationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThreatTargetPrioritizationType to copy from
           */
         ThreatTargetPrioritizationType(const ThreatTargetPrioritizationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThreatTargetPrioritizationType to the contents of the
           * ThreatTargetPrioritizationType on the right hand side (rhs) of the assignment operator.ThreatTargetPrioritizationType
           * [only available to derived classes]
           *
           * @param rhs The ThreatTargetPrioritizationType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ThreatTargetPrioritizationType
           * @return A constant reference to this ThreatTargetPrioritizationType.
           */
         const ThreatTargetPrioritizationType& operator=(const ThreatTargetPrioritizationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThreatTargetPrioritizationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThreatTargetPrioritizationType_h

