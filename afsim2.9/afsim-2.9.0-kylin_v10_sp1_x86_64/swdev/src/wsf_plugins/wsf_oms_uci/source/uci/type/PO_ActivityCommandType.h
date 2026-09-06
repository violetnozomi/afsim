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
#ifndef Uci__Type__PO_ActivityCommandType_h
#define Uci__Type__PO_ActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsType_h)
# include "uci/type/PO_ComponentSettingsType.h"
#endif

#if !defined(Uci__Type__PO_CollectionConstraintsSettingsType_h)
# include "uci/type/PO_CollectionConstraintsSettingsType.h"
#endif

#if !defined(Uci__Type__PO_TargetType_h)
# include "uci/type/PO_TargetType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ActivityCommandType sequence accessor class */
      class PO_ActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~PO_ActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ActivityCommandType
           *
           * @return This accessor's type constant, i.e. PO_ActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ActivityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeComponentSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeComponentSettings.
           */
         virtual const uci::type::PO_ComponentSettingsType& getChangeComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeComponentSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeComponentSettings.
           */
         virtual uci::type::PO_ComponentSettingsType& getChangeComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeComponentSettings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeComponentSettings
           */
         virtual void setChangeComponentSettings(const uci::type::PO_ComponentSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeComponentSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeComponentSettings is emabled or not
           */
         virtual bool hasChangeComponentSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeComponentSettings
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsType This Accessor's accessor type
           */
         virtual void enableChangeComponentSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeComponentSettings */
         virtual void clearChangeComponentSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ChangeCollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeCollectionConstraints.
           */
         virtual const uci::type::PO_CollectionConstraintsSettingsType& getChangeCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ChangeCollectionConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeCollectionConstraints.
           */
         virtual uci::type::PO_CollectionConstraintsSettingsType& getChangeCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeCollectionConstraints to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeCollectionConstraints
           */
         virtual void setChangeCollectionConstraints(const uci::type::PO_CollectionConstraintsSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeCollectionConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeCollectionConstraints is emabled or not
           */
         virtual bool hasChangeCollectionConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeCollectionConstraints
           *
           * @param type = uci::type::accessorType::pO_CollectionConstraintsSettingsType This Accessor's accessor type
           */
         virtual void enableChangeCollectionConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_CollectionConstraintsSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeCollectionConstraints */
         virtual void clearChangeCollectionConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTarget.
           */
         virtual const uci::type::PO_TargetType& getChangeTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTarget.
           */
         virtual uci::type::PO_TargetType& getChangeTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeTarget to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeTarget
           */
         virtual void setChangeTarget(const uci::type::PO_TargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeTarget exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeTarget is emabled or not
           */
         virtual bool hasChangeTarget() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeTarget
           *
           * @param type = uci::type::accessorType::pO_TargetType This Accessor's accessor type
           */
         virtual void enableChangeTarget(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_TargetType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeTarget */
         virtual void clearChangeTarget()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ActivityCommandType to copy from
           */
         PO_ActivityCommandType(const PO_ActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ActivityCommandType to the contents of the
           * PO_ActivityCommandType on the right hand side (rhs) of the assignment operator.PO_ActivityCommandType [only available
           * to derived classes]
           *
           * @param rhs The PO_ActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::PO_ActivityCommandType
           * @return A constant reference to this PO_ActivityCommandType.
           */
         const PO_ActivityCommandType& operator=(const PO_ActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ActivityCommandType_h

