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
#ifndef Uci__Type__TaskMDT_h
#define Uci__Type__TaskMDT_h 1

#if !defined(Uci__Type__TaskDataBaseType_h)
# include "uci/type/TaskDataBaseType.h"
#endif

#if !defined(Uci__Type__TaskType_h)
# include "uci/type/TaskType.h"
#endif

#if !defined(Uci__Type__TaskAllocationParametersType_h)
# include "uci/type/TaskAllocationParametersType.h"
#endif

#if !defined(Uci__Type__RiskPrioritizationEnum_h)
# include "uci/type/RiskPrioritizationEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Type__TimingConstraintsType_h)
# include "uci/type/TimingConstraintsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class TaskMDT : public virtual uci::type::TaskDataBaseType {
      public:

         /** The destructor */
         virtual ~TaskMDT()
         { }

         /** Returns this accessor's type constant, i.e. TaskMDT
           *
           * @return This accessor's type constant, i.e. TaskMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::taskMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const TaskMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskType.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskType.
           */
         virtual const uci::type::TaskType& getTaskType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskType.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskType.
           */
         virtual uci::type::TaskType& getTaskType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskType to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskType
           */
         virtual void setTaskType(const uci::type::TaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AllocationConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocationConstraint.
           */
         virtual const uci::type::TaskAllocationParametersType& getAllocationConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AllocationConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by AllocationConstraint.
           */
         virtual uci::type::TaskAllocationParametersType& getAllocationConstraint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AllocationConstraint to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AllocationConstraint
           */
         virtual void setAllocationConstraint(const uci::type::TaskAllocationParametersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RiskPrioritization.
           *
           * @return The value of the enumeration identified by RiskPrioritization.
           */
         virtual const uci::type::RiskPrioritizationEnum& getRiskPrioritization() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RiskPrioritization.
           *
           * @return The value of the enumeration identified by RiskPrioritization.
           */
         virtual uci::type::RiskPrioritizationEnum& getRiskPrioritization()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RiskPrioritization.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRiskPrioritization(const uci::type::RiskPrioritizationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RiskPrioritization.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRiskPrioritization(uci::type::RiskPrioritizationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield RiskPrioritizationis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasRiskPrioritization() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getRiskPrioritization will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::riskPrioritizationEnum This Accessor's accessor type
           */
         virtual void enableRiskPrioritization(uci::base::accessorType::AccessorType type = uci::type::accessorType::riskPrioritizationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearRiskPrioritization()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ExecutionApprovalRequired.
           *
           * @return The value of the simple primitive data type identified by ExecutionApprovalRequired.
           */
         virtual xs::Boolean getExecutionApprovalRequired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ExecutionApprovalRequired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExecutionApprovalRequired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AcceptableClassificationLevel.
           *
           * @return The acecssor that provides access to the complex content that is identified by AcceptableClassificationLevel.
           */
         virtual const uci::type::SecurityInformationType& getAcceptableClassificationLevel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AcceptableClassificationLevel.
           *
           * @return The acecssor that provides access to the complex content that is identified by AcceptableClassificationLevel.
           */
         virtual uci::type::SecurityInformationType& getAcceptableClassificationLevel()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AcceptableClassificationLevel to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AcceptableClassificationLevel
           */
         virtual void setAcceptableClassificationLevel(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AcceptableClassificationLevel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AcceptableClassificationLevel is emabled or not
           */
         virtual bool hasAcceptableClassificationLevel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AcceptableClassificationLevel
           *
           * @param type = uci::type::accessorType::securityInformationType This Accessor's accessor type
           */
         virtual void enableAcceptableClassificationLevel(uci::base::accessorType::AccessorType type = uci::type::accessorType::securityInformationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AcceptableClassificationLevel */
         virtual void clearAcceptableClassificationLevel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SpecialInstructions.
           *
           * @return The value of the string data type identified by SpecialInstructions.
           */
         virtual const uci::type::VisibleString1024Type& getSpecialInstructions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SpecialInstructions.
           *
           * @return The value of the string data type identified by SpecialInstructions.
           */
         virtual uci::type::VisibleString1024Type& getSpecialInstructions()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SpecialInstructions to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSpecialInstructions(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SpecialInstructions to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSpecialInstructions(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SpecialInstructions to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSpecialInstructions(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SpecialInstructions exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpecialInstructions is emabled or not
           */
         virtual bool hasSpecialInstructions() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpecialInstructions
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableSpecialInstructions(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpecialInstructions */
         virtual void clearSpecialInstructions()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimingConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimingConstraints.
           */
         virtual const uci::type::TimingConstraintsType& getTimingConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimingConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimingConstraints.
           */
         virtual uci::type::TimingConstraintsType& getTimingConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimingConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimingConstraints
           */
         virtual void setTimingConstraints(const uci::type::TimingConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimingConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimingConstraints is emabled or not
           */
         virtual bool hasTimingConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimingConstraints
           *
           * @param type = uci::type::accessorType::timingConstraintsType This Accessor's accessor type
           */
         virtual void enableTimingConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::timingConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimingConstraints */
         virtual void clearTimingConstraints()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         TaskMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The TaskMDT to copy from
           */
         TaskMDT(const TaskMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this TaskMDT to the contents of the TaskMDT on the right hand side
           * (rhs) of the assignment operator.TaskMDT [only available to derived classes]
           *
           * @param rhs The TaskMDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::TaskMDT
           * @return A constant reference to this TaskMDT.
           */
         const TaskMDT& operator=(const TaskMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // TaskMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__TaskMDT_h

