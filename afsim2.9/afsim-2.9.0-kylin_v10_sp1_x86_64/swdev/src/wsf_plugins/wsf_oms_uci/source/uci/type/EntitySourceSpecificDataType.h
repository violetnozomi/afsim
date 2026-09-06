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
#ifndef Uci__Type__EntitySourceSpecificDataType_h
#define Uci__Type__EntitySourceSpecificDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityFusionEligibilityEnum_h)
# include "uci/type/EntityFusionEligibilityEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CorrelatedEntityID_Type_h)
# include "uci/type/CorrelatedEntityID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

#if !defined(Uci__Type__PassiveActiveEnum_h)
# include "uci/type/PassiveActiveEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntitySourceSpecificDataType sequence accessor class */
      class EntitySourceSpecificDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntitySourceSpecificDataType()
         { }

         /** Returns this accessor's type constant, i.e. EntitySourceSpecificDataType
           *
           * @return This accessor's type constant, i.e. EntitySourceSpecificDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entitySourceSpecificDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntitySourceSpecificDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the unique ID of an Entity which is eligible for or excluded from future automated fusion operations with
           * the Entity represented by this Entity message. This element is required when the sibling FusionEligibility element
           * indicates SELECTIVE_EXCLUSION or SELECTIVE_INCLUSION and should be omitted otherwise. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CorrelatedEntityID_Type, uci::type::accessorType::correlatedEntityID_Type> CorrelatedEntityID;

         /** Indicates the unique ID of an Entity that for at least a singular point in time was kinematically uncertain with this
           * Entity. An example of this is when two vehicles come to a common intersection, stop, and then depart together.
           * Tracking algorithms have to make a decision on which track goes with which target and are not guaranteed to make the
           * correct decision unless there are outside sources (FBCB2, High Range Resolution Profiles, etc.) that allow the
           * tracker to either confirm the decision, or determine that a swap is necessary at a later time. At the time of the
           * intersection, the uncertainties of the tracks make them kinematically ambiguous so that information should be carried
           * on with the fused Entity. If the tracker makes the correct decision, than the other Entity will not be a
           * ContributorID, since it will have never contributed to the overall fused picture. Likewise, the Identity of the fused
           * Entity will not be modified since the other Entity never made a contribution. If the tracker makes an incorrect
           * decision than the other Entity will be a ContributorID and will possibly contribute to the Identity of the Fused
           * Entity. Regardless of the decision, the Ambiguous ID must be updated to reflect the ambiguous state that occurred.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EntityID_Type, uci::type::accessorType::entityID_Type> AmbiguousEntityID;

         /** [Minimum inclusive: 0] [Maximum inclusive: 15] */
         typedef uci::base::UnsignedIntAccessor TrackQuality;
         //  The type of primitive data accessed by TrackQuality
         typedef xs::UnsignedInt TrackQualityValue;

         /** Returns the value of the enumeration that is identified by the FusionEligibility.
           *
           * @return The value of the enumeration identified by FusionEligibility.
           */
         virtual const uci::type::EntityFusionEligibilityEnum& getFusionEligibility() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FusionEligibility.
           *
           * @return The value of the enumeration identified by FusionEligibility.
           */
         virtual uci::type::EntityFusionEligibilityEnum& getFusionEligibility()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFusionEligibility(const uci::type::EntityFusionEligibilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FusionEligibility.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFusionEligibility(uci::type::EntityFusionEligibilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FusionEligibilityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFusionEligibility() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFusionEligibility will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::entityFusionEligibilityEnum This Accessor's accessor type
           */
         virtual void enableFusionEligibility(uci::base::accessorType::AccessorType type = uci::type::accessorType::entityFusionEligibilityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFusionEligibility()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelatedEntityID.
           *
           * @return The bounded list identified by CorrelatedEntityID.
           */
         virtual const uci::type::EntitySourceSpecificDataType::CorrelatedEntityID& getCorrelatedEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CorrelatedEntityID.
           *
           * @return The bounded list identified by CorrelatedEntityID.
           */
         virtual uci::type::EntitySourceSpecificDataType::CorrelatedEntityID& getCorrelatedEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CorrelatedEntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCorrelatedEntityID(const uci::type::EntitySourceSpecificDataType::CorrelatedEntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DropProtectState.
           *
           * @return The value of the simple primitive data type identified by DropProtectState.
           */
         virtual xs::Boolean getDropProtectState() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DropProtectState.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDropProtectState(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DropProtectState exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DropProtectState is emabled or not
           */
         virtual bool hasDropProtectState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DropProtectState
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableDropProtectState(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DropProtectState */
         virtual void clearDropProtectState()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AmbiguousEntityID.
           *
           * @return The bounded list identified by AmbiguousEntityID.
           */
         virtual const uci::type::EntitySourceSpecificDataType::AmbiguousEntityID& getAmbiguousEntityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AmbiguousEntityID.
           *
           * @return The bounded list identified by AmbiguousEntityID.
           */
         virtual uci::type::EntitySourceSpecificDataType::AmbiguousEntityID& getAmbiguousEntityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AmbiguousEntityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAmbiguousEntityID(const uci::type::EntitySourceSpecificDataType::AmbiguousEntityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TrackQuality.
           *
           * @return The value of the simple primitive data type identified by TrackQuality.
           */
         virtual uci::type::EntitySourceSpecificDataType::TrackQualityValue getTrackQuality() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TrackQuality.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTrackQuality(uci::type::EntitySourceSpecificDataType::TrackQualityValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TrackQuality exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TrackQuality is emabled or not
           */
         virtual bool hasTrackQuality() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TrackQuality
           *
           * @param type = uci::type::entitySourceSpecificDataType::accessorType::trackQuality This Accessor's accessor type
           */
         virtual void enableTrackQuality(uci::base::accessorType::AccessorType type = uci::type::entitySourceSpecificDataType::accessorType::trackQuality)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TrackQuality */
         virtual void clearTrackQuality()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Priority.
           *
           * @return The acecssor that provides access to the complex content that is identified by Priority.
           */
         virtual const uci::type::RankingType& getPriority() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Priority.
           *
           * @return The acecssor that provides access to the complex content that is identified by Priority.
           */
         virtual uci::type::RankingType& getPriority()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Priority to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Priority
           */
         virtual void setPriority(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Priority exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Priority is emabled or not
           */
         virtual bool hasPriority() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Priority
           *
           * @param type = uci::type::accessorType::rankingType This Accessor's accessor type
           */
         virtual void enablePriority(uci::base::accessorType::AccessorType type = uci::type::accessorType::rankingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Priority */
         virtual void clearPriority()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PassiveActiveIndicator.
           *
           * @return The value of the enumeration identified by PassiveActiveIndicator.
           */
         virtual const uci::type::PassiveActiveEnum& getPassiveActiveIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PassiveActiveIndicator.
           *
           * @return The value of the enumeration identified by PassiveActiveIndicator.
           */
         virtual uci::type::PassiveActiveEnum& getPassiveActiveIndicator()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PassiveActiveIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPassiveActiveIndicator(const uci::type::PassiveActiveEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PassiveActiveIndicator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPassiveActiveIndicator(uci::type::PassiveActiveEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PassiveActiveIndicatoris available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPassiveActiveIndicator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPassiveActiveIndicator will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::passiveActiveEnum This Accessor's accessor type
           */
         virtual void enablePassiveActiveIndicator(uci::base::accessorType::AccessorType type = uci::type::accessorType::passiveActiveEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPassiveActiveIndicator()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntitySourceSpecificDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntitySourceSpecificDataType to copy from
           */
         EntitySourceSpecificDataType(const EntitySourceSpecificDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntitySourceSpecificDataType to the contents of the
           * EntitySourceSpecificDataType on the right hand side (rhs) of the assignment operator.EntitySourceSpecificDataType
           * [only available to derived classes]
           *
           * @param rhs The EntitySourceSpecificDataType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::EntitySourceSpecificDataType
           * @return A constant reference to this EntitySourceSpecificDataType.
           */
         const EntitySourceSpecificDataType& operator=(const EntitySourceSpecificDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntitySourceSpecificDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntitySourceSpecificDataType_h

