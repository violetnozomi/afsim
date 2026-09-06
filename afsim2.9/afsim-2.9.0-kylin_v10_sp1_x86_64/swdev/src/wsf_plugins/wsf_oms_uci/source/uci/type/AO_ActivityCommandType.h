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
#ifndef Uci__Type__AO_ActivityCommandType_h
#define Uci__Type__AO_ActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Type__AO_ComponentSettingsType_h)
# include "uci/type/AO_ComponentSettingsType.h"
#endif

#if !defined(Uci__Type__TargetType_h)
# include "uci/type/TargetType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_ActivityCommandType sequence accessor class */
      class AO_ActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~AO_ActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. AO_ActivityCommandType
           *
           * @return This accessor's type constant, i.e. AO_ActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_ActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_ActivityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeComponentSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeComponentSettings.
           */
         virtual const uci::type::AO_ComponentSettingsType& getChangeComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeComponentSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeComponentSettings.
           */
         virtual uci::type::AO_ComponentSettingsType& getChangeComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeComponentSettings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeComponentSettings
           */
         virtual void setChangeComponentSettings(const uci::type::AO_ComponentSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeComponentSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeComponentSettings is emabled or not
           */
         virtual bool hasChangeComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeComponentSettings
           *
           * @param type = uci::type::accessorType::aO_ComponentSettingsType This Accessor's accessor type
           */
         virtual void enableChangeComponentSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::aO_ComponentSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeComponentSettings */
         virtual void clearChangeComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTarget.
           */
         virtual const uci::type::TargetType& getChangeTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTarget.
           */
         virtual uci::type::TargetType& getChangeTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeTarget to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeTarget
           */
         virtual void setChangeTarget(const uci::type::TargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeTarget exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeTarget is emabled or not
           */
         virtual bool hasChangeTarget() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeTarget
           *
           * @param type = uci::type::accessorType::targetType This Accessor's accessor type
           */
         virtual void enableChangeTarget(uci::base::accessorType::AccessorType type = uci::type::accessorType::targetType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeTarget */
         virtual void clearChangeTarget()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AO_ActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_ActivityCommandType to copy from
           */
         AO_ActivityCommandType(const AO_ActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_ActivityCommandType to the contents of the
           * AO_ActivityCommandType on the right hand side (rhs) of the assignment operator.AO_ActivityCommandType [only available
           * to derived classes]
           *
           * @param rhs The AO_ActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::AO_ActivityCommandType
           * @return A constant reference to this AO_ActivityCommandType.
           */
         const AO_ActivityCommandType& operator=(const AO_ActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_ActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_ActivityCommandType_h

