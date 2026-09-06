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
#ifndef Uci__Type__AO_CapabilityCommandType_h
#define Uci__Type__AO_CapabilityCommandType_h 1

#if !defined(Uci__Type__CapabilityCommandBaseType_h)
# include "uci/type/CapabilityCommandBaseType.h"
#endif

#if !defined(Uci__Type__TargetType_h)
# include "uci/type/TargetType.h"
#endif

#if !defined(Uci__Type__AO_ComponentSettingsType_h)
# include "uci/type/AO_ComponentSettingsType.h"
#endif

#if !defined(Uci__Type__OpticalCollectionConstraintsType_h)
# include "uci/type/OpticalCollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__CommandRepetitionType_h)
# include "uci/type/CommandRepetitionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_CapabilityCommandType sequence accessor class */
      class AO_CapabilityCommandType : public virtual uci::type::CapabilityCommandBaseType {
      public:

         /** The destructor */
         virtual ~AO_CapabilityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. AO_CapabilityCommandType
           *
           * @return This accessor's type constant, i.e. AO_CapabilityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_CapabilityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_CapabilityCommandType& accessor)
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


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentSettings.
           */
         virtual const uci::type::AO_ComponentSettingsType& getComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentSettings.
           */
         virtual uci::type::AO_ComponentSettingsType& getComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentSettings
           */
         virtual void setComponentSettings(const uci::type::AO_ComponentSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ComponentSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ComponentSettings is emabled or not
           */
         virtual bool hasComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ComponentSettings
           *
           * @param type = uci::type::accessorType::aO_ComponentSettingsType This Accessor's accessor type
           */
         virtual void enableComponentSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::aO_ComponentSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ComponentSettings */
         virtual void clearComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmissionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmissionConstraints.
           */
         virtual const uci::type::OpticalCollectionConstraintsType& getEmissionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmissionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmissionConstraints.
           */
         virtual uci::type::OpticalCollectionConstraintsType& getEmissionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmissionConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmissionConstraints
           */
         virtual void setEmissionConstraints(const uci::type::OpticalCollectionConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmissionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmissionConstraints is emabled or not
           */
         virtual bool hasEmissionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmissionConstraints
           *
           * @param type = uci::type::accessorType::opticalCollectionConstraintsType This Accessor's accessor type
           */
         virtual void enableEmissionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::opticalCollectionConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmissionConstraints */
         virtual void clearEmissionConstraints()
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
         AO_CapabilityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_CapabilityCommandType to copy from
           */
         AO_CapabilityCommandType(const AO_CapabilityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_CapabilityCommandType to the contents of the
           * AO_CapabilityCommandType on the right hand side (rhs) of the assignment operator.AO_CapabilityCommandType [only
           * available to derived classes]
           *
           * @param rhs The AO_CapabilityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::AO_CapabilityCommandType
           * @return A constant reference to this AO_CapabilityCommandType.
           */
         const AO_CapabilityCommandType& operator=(const AO_CapabilityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_CapabilityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_CapabilityCommandType_h

