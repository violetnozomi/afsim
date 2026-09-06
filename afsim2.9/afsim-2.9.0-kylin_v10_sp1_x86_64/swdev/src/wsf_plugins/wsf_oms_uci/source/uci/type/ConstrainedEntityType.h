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
#ifndef Uci__Type__ConstrainedEntityType_h
#define Uci__Type__ConstrainedEntityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RadarEmitterIdentityType_h)
# include "uci/type/RadarEmitterIdentityType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ConstrainedEntityType sequence accessor class */
      class ConstrainedEntityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ConstrainedEntityType()
         { }

         /** Returns this accessor's type constant, i.e. ConstrainedEntityType
           *
           * @return This accessor's type constant, i.e. ConstrainedEntityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::constrainedEntityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ConstrainedEntityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SurrogateID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SurrogateID.
           */
         virtual const uci::type::RadarEmitterIdentityType& getSurrogateID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SurrogateID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SurrogateID.
           */
         virtual uci::type::RadarEmitterIdentityType& getSurrogateID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SurrogateID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SurrogateID
           */
         virtual void setSurrogateID(const uci::type::RadarEmitterIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SurrogateID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SurrogateID is emabled or not
           */
         virtual bool hasSurrogateID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SurrogateID
           *
           * @param type = uci::type::accessorType::radarEmitterIdentityType This Accessor's accessor type
           */
         virtual void enableSurrogateID(uci::base::accessorType::AccessorType type = uci::type::accessorType::radarEmitterIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SurrogateID */
         virtual void clearSurrogateID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MissionPlanningWeightingFactor.
           *
           * @return The value of the simple primitive data type identified by MissionPlanningWeightingFactor.
           */
         virtual uci::type::PercentTypeValue getMissionPlanningWeightingFactor() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MissionPlanningWeightingFactor.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMissionPlanningWeightingFactor(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionPlanningWeightingFactor exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPlanningWeightingFactor is emabled or not
           */
         virtual bool hasMissionPlanningWeightingFactor() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPlanningWeightingFactor
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableMissionPlanningWeightingFactor(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPlanningWeightingFactor */
         virtual void clearMissionPlanningWeightingFactor()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MissionPlanningCandidate.
           *
           * @return The value of the simple primitive data type identified by MissionPlanningCandidate.
           */
         virtual xs::Boolean getMissionPlanningCandidate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MissionPlanningCandidate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMissionPlanningCandidate(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionPlanningCandidate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPlanningCandidate is emabled or not
           */
         virtual bool hasMissionPlanningCandidate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPlanningCandidate
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableMissionPlanningCandidate(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPlanningCandidate */
         virtual void clearMissionPlanningCandidate()
            throw(uci::base::UCIException) = 0;


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



      protected:

         /** The constructor [only available to derived classes]. */
         ConstrainedEntityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ConstrainedEntityType to copy from
           */
         ConstrainedEntityType(const ConstrainedEntityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ConstrainedEntityType to the contents of the ConstrainedEntityType
           * on the right hand side (rhs) of the assignment operator.ConstrainedEntityType [only available to derived classes]
           *
           * @param rhs The ConstrainedEntityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ConstrainedEntityType
           * @return A constant reference to this ConstrainedEntityType.
           */
         const ConstrainedEntityType& operator=(const ConstrainedEntityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ConstrainedEntityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ConstrainedEntityType_h

