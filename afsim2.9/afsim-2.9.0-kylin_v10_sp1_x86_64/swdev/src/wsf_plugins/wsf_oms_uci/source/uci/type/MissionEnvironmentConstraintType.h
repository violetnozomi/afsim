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
#ifndef Uci__Type__MissionEnvironmentConstraintType_h
#define Uci__Type__MissionEnvironmentConstraintType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ConstrainedEntityType_h)
# include "uci/type/ConstrainedEntityType.h"
#endif

#if !defined(Uci__Type__ConstrainedOpPointType_h)
# include "uci/type/ConstrainedOpPointType.h"
#endif

#if !defined(Uci__Type__ConstrainedOpLineType_h)
# include "uci/type/ConstrainedOpLineType.h"
#endif

#if !defined(Uci__Type__ConstrainedOpZoneType_h)
# include "uci/type/ConstrainedOpZoneType.h"
#endif

#if !defined(Uci__Type__SystemStatusMDT_h)
# include "uci/type/SystemStatusMDT.h"
#endif

#if !defined(Uci__Type__TaskRiskAdjustmentType_h)
# include "uci/type/TaskRiskAdjustmentType.h"
#endif

#if !defined(Uci__Type__ParameterAssertType_h)
# include "uci/type/ParameterAssertType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionEnvironmentConstraintType choice accessor class */
      class MissionEnvironmentConstraintType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionEnvironmentConstraintType()
         { }

         /** Returns this accessor's type constant, i.e. MissionEnvironmentConstraintType
           *
           * @return This accessor's type constant, i.e. MissionEnvironmentConstraintType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionEnvironmentConstraintType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionEnvironmentConstraintType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum MissionEnvironmentConstraintTypeChoice {
            MISSIONENVIRONMENTCONSTRAINTTYPE_CHOICE_NONE,
            MISSIONENVIRONMENTCONSTRAINTTYPE_CHOICE_CONSTRAINEDENTITY,
            MISSIONENVIRONMENTCONSTRAINTTYPE_CHOICE_CONSTRAINEDOPPOINT,
            MISSIONENVIRONMENTCONSTRAINTTYPE_CHOICE_CONSTRAINEDOPLINE,
            MISSIONENVIRONMENTCONSTRAINTTYPE_CHOICE_CONSTRAINEDOPZONE,
            MISSIONENVIRONMENTCONSTRAINTTYPE_CHOICE_SYSTEM,
            MISSIONENVIRONMENTCONSTRAINTTYPE_CHOICE_RISKADJUSTMENT,
            MISSIONENVIRONMENTCONSTRAINTTYPE_CHOICE_PARAMETER
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual MissionEnvironmentConstraintTypeChoice getMissionEnvironmentConstraintTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getMissionEnvironmentConstraintTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In
           * addition, the access methods associated with the chosen element will be enabled and will provide access to the chosen
           * element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setMissionEnvironmentConstraintTypeChoiceOrdinal(MissionEnvironmentConstraintTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedEntity.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedEntity.
           */
         virtual const uci::type::ConstrainedEntityType& getConstrainedEntity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedEntity.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedEntity.
           */
         virtual uci::type::ConstrainedEntityType& getConstrainedEntity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConstrainedEntity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConstrainedEntity
           */
         virtual void setConstrainedEntity(const uci::type::ConstrainedEntityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedOpPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedOpPoint.
           */
         virtual const uci::type::ConstrainedOpPointType& getConstrainedOpPoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedOpPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedOpPoint.
           */
         virtual uci::type::ConstrainedOpPointType& getConstrainedOpPoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConstrainedOpPoint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConstrainedOpPoint
           */
         virtual void setConstrainedOpPoint(const uci::type::ConstrainedOpPointType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedOpLine.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedOpLine.
           */
         virtual const uci::type::ConstrainedOpLineType& getConstrainedOpLine() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedOpLine.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedOpLine.
           */
         virtual uci::type::ConstrainedOpLineType& getConstrainedOpLine()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConstrainedOpLine to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConstrainedOpLine
           */
         virtual void setConstrainedOpLine(const uci::type::ConstrainedOpLineType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedOpZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedOpZone.
           */
         virtual const uci::type::ConstrainedOpZoneType& getConstrainedOpZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConstrainedOpZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConstrainedOpZone.
           */
         virtual uci::type::ConstrainedOpZoneType& getConstrainedOpZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConstrainedOpZone to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConstrainedOpZone
           */
         virtual void setConstrainedOpZone(const uci::type::ConstrainedOpZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the System.
           *
           * @return The acecssor that provides access to the complex content that is identified by System.
           */
         virtual const uci::type::SystemStatusMDT& getSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the System.
           *
           * @return The acecssor that provides access to the complex content that is identified by System.
           */
         virtual uci::type::SystemStatusMDT& getSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the System to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by System
           */
         virtual void setSystem(const uci::type::SystemStatusMDT& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RiskAdjustment.
           *
           * @return The acecssor that provides access to the complex content that is identified by RiskAdjustment.
           */
         virtual const uci::type::TaskRiskAdjustmentType& getRiskAdjustment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RiskAdjustment.
           *
           * @return The acecssor that provides access to the complex content that is identified by RiskAdjustment.
           */
         virtual uci::type::TaskRiskAdjustmentType& getRiskAdjustment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RiskAdjustment to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RiskAdjustment
           */
         virtual void setRiskAdjustment(const uci::type::TaskRiskAdjustmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Parameter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Parameter.
           */
         virtual const uci::type::ParameterAssertType& getParameter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Parameter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Parameter.
           */
         virtual uci::type::ParameterAssertType& getParameter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Parameter to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Parameter
           */
         virtual void setParameter(const uci::type::ParameterAssertType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionEnvironmentConstraintType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionEnvironmentConstraintType to copy from
           */
         MissionEnvironmentConstraintType(const MissionEnvironmentConstraintType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionEnvironmentConstraintType to the contents of the
           * MissionEnvironmentConstraintType on the right hand side (rhs) of the assignment
           * operator.MissionEnvironmentConstraintType [only available to derived classes]
           *
           * @param rhs The MissionEnvironmentConstraintType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::MissionEnvironmentConstraintType
           * @return A constant reference to this MissionEnvironmentConstraintType.
           */
         const MissionEnvironmentConstraintType& operator=(const MissionEnvironmentConstraintType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionEnvironmentConstraintType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionEnvironmentConstraintType_h

