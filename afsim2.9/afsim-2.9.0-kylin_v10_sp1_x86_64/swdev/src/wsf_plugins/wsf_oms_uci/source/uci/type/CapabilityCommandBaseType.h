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
#ifndef Uci__Type__CapabilityCommandBaseType_h
#define Uci__Type__CapabilityCommandBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityCommandID_Type_h)
# include "uci/type/CapabilityCommandID_Type.h"
#endif

#if !defined(Uci__Type__MessageStateEnum_h)
# include "uci/type/MessageStateEnum.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityCommandRankingType_h)
# include "uci/type/CapabilityCommandRankingType.h"
#endif

#if !defined(Uci__Type__CapabilityCommandTemporalConstraintsType_h)
# include "uci/type/CapabilityCommandTemporalConstraintsType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__TraceabilityType_h)
# include "uci/type/TraceabilityType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityCommandBaseType sequence accessor class */
      class CapabilityCommandBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityCommandBaseType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityCommandBaseType
           *
           * @return This accessor's type constant, i.e. CapabilityCommandBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityCommandBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityCommandBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual const uci::type::CapabilityCommandID_Type& getCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual uci::type::CapabilityCommandID_Type& getCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandID
           */
         virtual void setCommandID(const uci::type::CapabilityCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandState.
           *
           * @return The value of the enumeration identified by CommandState.
           */
         virtual const uci::type::MessageStateEnum& getCommandState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandState.
           *
           * @return The value of the enumeration identified by CommandState.
           */
         virtual uci::type::MessageStateEnum& getCommandState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandState(const uci::type::MessageStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandState(uci::type::MessageStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Ranking.
           *
           * @return The acecssor that provides access to the complex content that is identified by Ranking.
           */
         virtual const uci::type::CapabilityCommandRankingType& getRanking() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Ranking.
           *
           * @return The acecssor that provides access to the complex content that is identified by Ranking.
           */
         virtual uci::type::CapabilityCommandRankingType& getRanking()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Ranking to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Ranking
           */
         virtual void setRanking(const uci::type::CapabilityCommandRankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TemporalConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TemporalConstraints.
           */
         virtual const uci::type::CapabilityCommandTemporalConstraintsType& getTemporalConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TemporalConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TemporalConstraints.
           */
         virtual uci::type::CapabilityCommandTemporalConstraintsType& getTemporalConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TemporalConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TemporalConstraints
           */
         virtual void setTemporalConstraints(const uci::type::CapabilityCommandTemporalConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TemporalConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TemporalConstraints is emabled or not
           */
         virtual bool hasTemporalConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TemporalConstraints
           *
           * @param type = uci::type::accessorType::capabilityCommandTemporalConstraintsType This Accessor's accessor type
           */
         virtual void enableTemporalConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityCommandTemporalConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TemporalConstraints */
         virtual void clearTemporalConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OverrideRejection.
           *
           * @return The value of the simple primitive data type identified by OverrideRejection.
           */
         virtual xs::Boolean getOverrideRejection() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OverrideRejection.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOverrideRejection(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OverrideRejection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OverrideRejection is emabled or not
           */
         virtual bool hasOverrideRejection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OverrideRejection
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableOverrideRejection(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OverrideRejection */
         virtual void clearOverrideRejection()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual const uci::type::TraceabilityType& getTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual uci::type::TraceabilityType& getTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Traceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Traceability
           */
         virtual void setTraceability(const uci::type::TraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Traceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Traceability is emabled or not
           */
         virtual bool hasTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Traceability
           *
           * @param type = uci::type::accessorType::traceabilityType This Accessor's accessor type
           */
         virtual void enableTraceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::traceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Traceability */
         virtual void clearTraceability()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Classification.
           *
           * @return The acecssor that provides access to the complex content that is identified by Classification.
           */
         virtual const uci::type::SecurityInformationType& getClassification() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Classification.
           *
           * @return The acecssor that provides access to the complex content that is identified by Classification.
           */
         virtual uci::type::SecurityInformationType& getClassification()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Classification to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Classification
           */
         virtual void setClassification(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Classification exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Classification is emabled or not
           */
         virtual bool hasClassification() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Classification
           *
           * @param type = uci::type::accessorType::securityInformationType This Accessor's accessor type
           */
         virtual void enableClassification(uci::base::accessorType::AccessorType type = uci::type::accessorType::securityInformationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Classification */
         virtual void clearClassification()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityCommandBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityCommandBaseType to copy from
           */
         CapabilityCommandBaseType(const CapabilityCommandBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityCommandBaseType to the contents of the
           * CapabilityCommandBaseType on the right hand side (rhs) of the assignment operator.CapabilityCommandBaseType [only
           * available to derived classes]
           *
           * @param rhs The CapabilityCommandBaseType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CapabilityCommandBaseType
           * @return A constant reference to this CapabilityCommandBaseType.
           */
         const CapabilityCommandBaseType& operator=(const CapabilityCommandBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityCommandBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityCommandBaseType_h

