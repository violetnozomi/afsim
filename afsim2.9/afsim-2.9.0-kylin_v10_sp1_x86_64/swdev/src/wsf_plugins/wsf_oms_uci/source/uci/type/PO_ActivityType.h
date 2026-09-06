// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__PO_ActivityType_h
#define Uci__Type__PO_ActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Type__SensorConeType_h)
# include "uci/type/SensorConeType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__PO_TargetType_h)
# include "uci/type/PO_TargetType.h"
#endif

#if !defined(Uci__Type__PolygonType_h)
# include "uci/type/PolygonType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

#if !defined(Uci__Type__PO_ComponentStatusType_h)
# include "uci/type/PO_ComponentStatusType.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsStatusType_h)
# include "uci/type/PO_CollectionConstraintsStatusType.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ActivityType sequence accessor class */
      class PO_ActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~PO_ActivityType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ActivityType
           *
           * @return This accessor's type constant, i.e. PO_ActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FOV.
           *
           * @return The acecssor that provides access to the complex content that is identified by FOV.
           */
         virtual const uci::type::SensorConeType& getFOV() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FOV.
           *
           * @return The acecssor that provides access to the complex content that is identified by FOV.
           */
         virtual uci::type::SensorConeType& getFOV()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FOV to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FOV
           */
         virtual void setFOV(const uci::type::SensorConeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FOV_Timestamp.
           *
           * @return The value of the simple primitive data type identified by FOV_Timestamp.
           */
         virtual uci::type::DateTimeTypeValue getFOV_Timestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FOV_Timestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFOV_Timestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FOV_Timestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FOV_Timestamp is emabled or not
           */
         virtual bool hasFOV_Timestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FOV_Timestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableFOV_Timestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FOV_Timestamp */
         virtual void clearFOV_Timestamp()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual const uci::type::PO_TargetType& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual uci::type::PO_TargetType& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Target to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Target
           */
         virtual void setTarget(const uci::type::PO_TargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Target exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Target is emabled or not
           */
         virtual bool hasTarget() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Target
           *
           * @param type = uci::type::accessorType::pO_TargetType This Accessor's accessor type
           */
         virtual void enableTarget(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_TargetType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Target */
         virtual void clearTarget()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FootPrint2D.
           *
           * @return The acecssor that provides access to the complex content that is identified by FootPrint2D.
           */
         virtual const uci::type::PolygonType& getFootPrint2D() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FootPrint2D.
           *
           * @return The acecssor that provides access to the complex content that is identified by FootPrint2D.
           */
         virtual uci::type::PolygonType& getFootPrint2D()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FootPrint2D to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FootPrint2D
           */
         virtual void setFootPrint2D(const uci::type::PolygonType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FootPrint2D exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FootPrint2D is emabled or not
           */
         virtual bool hasFootPrint2D() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FootPrint2D
           *
           * @param type = uci::type::accessorType::polygonType This Accessor's accessor type
           */
         virtual void enableFootPrint2D(uci::base::accessorType::AccessorType type = uci::type::accessorType::polygonType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FootPrint2D */
         virtual void clearFootPrint2D()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaxViewDistance.
           *
           * @return The value of the simple primitive data type identified by MaxViewDistance.
           */
         virtual uci::type::DistanceTypeValue getMaxViewDistance() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaxViewDistance.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaxViewDistance(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaxViewDistance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaxViewDistance is emabled or not
           */
         virtual bool hasMaxViewDistance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaxViewDistance
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableMaxViewDistance(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaxViewDistance */
         virtual void clearMaxViewDistance()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EstimatedStartTime.
           *
           * @return The value of the simple primitive data type identified by EstimatedStartTime.
           */
         virtual uci::type::DateTimeTypeValue getEstimatedStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EstimatedStartTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEstimatedStartTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimatedStartTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimatedStartTime is emabled or not
           */
         virtual bool hasEstimatedStartTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimatedStartTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableEstimatedStartTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimatedStartTime */
         virtual void clearEstimatedStartTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentStatus.
           */
         virtual const uci::type::PO_ComponentStatusType& getComponentStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentStatus.
           */
         virtual uci::type::PO_ComponentStatusType& getComponentStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentStatus
           */
         virtual void setComponentStatus(const uci::type::PO_ComponentStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ComponentStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ComponentStatus is emabled or not
           */
         virtual bool hasComponentStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ComponentStatus
           *
           * @param type = uci::type::accessorType::pO_ComponentStatusType This Accessor's accessor type
           */
         virtual void enableComponentStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ComponentStatus */
         virtual void clearComponentStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CollectionConstraintsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraintsStatus.
           */
         virtual const uci::type::PO_CollectionConstraintsStatusType& getCollectionConstraintsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * CollectionConstraintsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionConstraintsStatus.
           */
         virtual uci::type::PO_CollectionConstraintsStatusType& getCollectionConstraintsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionConstraintsStatus to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionConstraintsStatus
           */
         virtual void setCollectionConstraintsStatus(const uci::type::PO_CollectionConstraintsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionConstraintsStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionConstraintsStatus is emabled or not
           */
         virtual bool hasCollectionConstraintsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionConstraintsStatus
           *
           * @param type = uci::type::accessorType::pO_CollectionConstraintsStatusType This Accessor's accessor type
           */
         virtual void enableCollectionConstraintsStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_CollectionConstraintsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionConstraintsStatus */
         virtual void clearCollectionConstraintsStatus()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual const uci::type::CommandRepetitionType& getRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Repetition.
           *
           * @return The acecssor that provides access to the complex content that is identified by Repetition.
           */
         virtual uci::type::CommandRepetitionType& getRepetition()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Repetition to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Repetition
           */
         virtual void setRepetition(const uci::type::CommandRepetitionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Repetition exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Repetition is emabled or not
           */
         virtual bool hasRepetition() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Repetition
           *
           * @param type = uci::type::accessorType::commandRepetitionType This Accessor's accessor type
           */
         virtual void enableRepetition(uci::base::accessorType::AccessorType type = uci::type::accessorType::commandRepetitionType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Repetition */
         virtual void clearRepetition()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ActivityType to copy from
           */
         PO_ActivityType(const PO_ActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ActivityType to the contents of the PO_ActivityType on the
           * right hand side (rhs) of the assignment operator.PO_ActivityType [only available to derived classes]
           *
           * @param rhs The PO_ActivityType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::PO_ActivityType
           * @return A constant reference to this PO_ActivityType.
           */
         const PO_ActivityType& operator=(const PO_ActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ActivityType_h

