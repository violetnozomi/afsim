// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AO_ActivityType_h
#define Uci__Type__AO_ActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Type__SensorConeType_h)
# include "uci/type/SensorConeType.h"
#endif

#if !defined(Uci__Type__PolygonType_h)
# include "uci/type/PolygonType.h"
#endif

#if !defined(Uci__Type__TargetType_h)
# include "uci/type/TargetType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AO_ComponentSettingsType_h)
# include "uci/type/AO_ComponentSettingsType.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ConsentEnum_h)
# include "uci/type/ConsentEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_ActivityType sequence accessor class */
      class AO_ActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~AO_ActivityType()
         { }

         /** Returns this accessor's type constant, i.e. AO_ActivityType
           *
           * @return This accessor's type constant, i.e. AO_ActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_ActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_ActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This component is optional. Indicates specific component settings for various elements for the AO sensor [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AO_ComponentSettingsType, uci::type::accessorType::aO_ComponentSettingsType> ComponentSettings;

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


         /** Returns the accessor that provides access to the complex content that is identified by the EmissionFootPrint2D.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmissionFootPrint2D.
           */
         virtual const uci::type::PolygonType& getEmissionFootPrint2D() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmissionFootPrint2D.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmissionFootPrint2D.
           */
         virtual uci::type::PolygonType& getEmissionFootPrint2D()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmissionFootPrint2D to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmissionFootPrint2D
           */
         virtual void setEmissionFootPrint2D(const uci::type::PolygonType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmissionFootPrint2D exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmissionFootPrint2D is emabled or not
           */
         virtual bool hasEmissionFootPrint2D() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmissionFootPrint2D
           *
           * @param type = uci::type::accessorType::polygonType This Accessor's accessor type
           */
         virtual void enableEmissionFootPrint2D(uci::base::accessorType::AccessorType type = uci::type::accessorType::polygonType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmissionFootPrint2D */
         virtual void clearEmissionFootPrint2D()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual const uci::type::TargetType& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual uci::type::TargetType& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Target to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Target
           */
         virtual void setTarget(const uci::type::TargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Target exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Target is emabled or not
           */
         virtual bool hasTarget() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Target
           *
           * @param type = uci::type::accessorType::targetType This Accessor's accessor type
           */
         virtual void enableTarget(uci::base::accessorType::AccessorType type = uci::type::accessorType::targetType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Target */
         virtual void clearTarget()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentSettings.
           *
           * @return The bounded list identified by ComponentSettings.
           */
         virtual const uci::type::AO_ActivityType::ComponentSettings& getComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ComponentSettings.
           *
           * @return The bounded list identified by ComponentSettings.
           */
         virtual uci::type::AO_ActivityType::ComponentSettings& getComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ComponentSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setComponentSettings(const uci::type::AO_ActivityType::ComponentSettings& value)
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


         /** Returns the value of the SimplePrimitive data type that is identified by the ConsentRequired.
           *
           * @return The value of the simple primitive data type identified by ConsentRequired.
           */
         virtual xs::Boolean getConsentRequired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ConsentRequired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setConsentRequired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ConsentRequired exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ConsentRequired is emabled or not
           */
         virtual bool hasConsentRequired() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ConsentRequired
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableConsentRequired(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ConsentRequired */
         virtual void clearConsentRequired()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConsentState.
           *
           * @return The value of the enumeration identified by ConsentState.
           */
         virtual const uci::type::ConsentEnum& getConsentState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConsentState.
           *
           * @return The value of the enumeration identified by ConsentState.
           */
         virtual uci::type::ConsentEnum& getConsentState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConsentState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConsentState(const uci::type::ConsentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConsentState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConsentState(uci::type::ConsentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ConsentStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasConsentState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getConsentState will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::consentEnum This Accessor's accessor type
           */
         virtual void enableConsentState(uci::base::accessorType::AccessorType type = uci::type::accessorType::consentEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearConsentState()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AO_ActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_ActivityType to copy from
           */
         AO_ActivityType(const AO_ActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_ActivityType to the contents of the AO_ActivityType on the
           * right hand side (rhs) of the assignment operator.AO_ActivityType [only available to derived classes]
           *
           * @param rhs The AO_ActivityType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::AO_ActivityType
           * @return A constant reference to this AO_ActivityType.
           */
         const AO_ActivityType& operator=(const AO_ActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_ActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_ActivityType_h

