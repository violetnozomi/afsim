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
#ifndef Uci__Type__StoreManagementCommandType_h
#define Uci__Type__StoreManagementCommandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__StoreType_h)
# include "uci/type/StoreType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ReleaseConsentType_h)
# include "uci/type/ReleaseConsentType.h"
#endif

#if !defined(Uci__Type__Velocity2D_Type_h)
# include "uci/type/Velocity2D_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the StoreManagementCommandType choice accessor class */
      class StoreManagementCommandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~StoreManagementCommandType()
         { }

         /** Returns this accessor's type constant, i.e. StoreManagementCommandType
           *
           * @return This accessor's type constant, i.e. StoreManagementCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::storeManagementCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StoreManagementCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum StoreManagementCommandTypeChoice {
            STOREMANAGEMENTCOMMANDTYPE_CHOICE_NONE,
            STOREMANAGEMENTCOMMANDTYPE_CHOICE_NEXTSTORESTATION,
            STOREMANAGEMENTCOMMANDTYPE_CHOICE_NEXTSTORETYPE,
            STOREMANAGEMENTCOMMANDTYPE_CHOICE_OVERRIDELAR,
            STOREMANAGEMENTCOMMANDTYPE_CHOICE_OVERRIDEATTITUDECONSTRAINTS,
            STOREMANAGEMENTCOMMANDTYPE_CHOICE_MASTERARM,
            STOREMANAGEMENTCOMMANDTYPE_CHOICE_RELEASECONSENT,
            STOREMANAGEMENTCOMMANDTYPE_CHOICE_LAR_CALCULATIONWINDHOLD,
            STOREMANAGEMENTCOMMANDTYPE_CHOICE_LAR_CALCULATIONWINDOVERRIDE
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual StoreManagementCommandTypeChoice getStoreManagementCommandTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getStoreManagementCommandTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition,
           * the access methods associated with the chosen element will be enabled and will provide access to the chosen element.
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
         virtual void setStoreManagementCommandTypeChoiceOrdinal(StoreManagementCommandTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NextStoreStation.
           *
           * @return The acecssor that provides access to the complex content that is identified by NextStoreStation.
           */
         virtual const uci::type::ForeignKeyType& getNextStoreStation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NextStoreStation.
           *
           * @return The acecssor that provides access to the complex content that is identified by NextStoreStation.
           */
         virtual uci::type::ForeignKeyType& getNextStoreStation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NextStoreStation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NextStoreStation
           */
         virtual void setNextStoreStation(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NextStoreType.
           *
           * @return The acecssor that provides access to the complex content that is identified by NextStoreType.
           */
         virtual const uci::type::StoreType& getNextStoreType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NextStoreType.
           *
           * @return The acecssor that provides access to the complex content that is identified by NextStoreType.
           */
         virtual uci::type::StoreType& getNextStoreType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NextStoreType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NextStoreType
           */
         virtual void setNextStoreType(const uci::type::StoreType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OverrideLAR.
           *
           * @return The value of the simple primitive data type identified by OverrideLAR.
           */
         virtual xs::Boolean getOverrideLAR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OverrideLAR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOverrideLAR(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OverrideAttitudeConstraints.
           *
           * @return The value of the simple primitive data type identified by OverrideAttitudeConstraints.
           */
         virtual xs::Boolean getOverrideAttitudeConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OverrideAttitudeConstraints.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOverrideAttitudeConstraints(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MasterArm.
           *
           * @return The value of the simple primitive data type identified by MasterArm.
           */
         virtual xs::Boolean getMasterArm() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MasterArm.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMasterArm(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReleaseConsent.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleaseConsent.
           */
         virtual const uci::type::ReleaseConsentType& getReleaseConsent() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReleaseConsent.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleaseConsent.
           */
         virtual uci::type::ReleaseConsentType& getReleaseConsent()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReleaseConsent to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReleaseConsent
           */
         virtual void setReleaseConsent(const uci::type::ReleaseConsentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LAR_CalculationWindHold.
           *
           * @return The value of the simple primitive data type identified by LAR_CalculationWindHold.
           */
         virtual xs::Boolean getLAR_CalculationWindHold() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LAR_CalculationWindHold.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLAR_CalculationWindHold(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * LAR_CalculationWindOverride.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_CalculationWindOverride.
           */
         virtual const uci::type::Velocity2D_Type& getLAR_CalculationWindOverride() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * LAR_CalculationWindOverride.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_CalculationWindOverride.
           */
         virtual uci::type::Velocity2D_Type& getLAR_CalculationWindOverride()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LAR_CalculationWindOverride to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LAR_CalculationWindOverride
           */
         virtual void setLAR_CalculationWindOverride(const uci::type::Velocity2D_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StoreManagementCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StoreManagementCommandType to copy from
           */
         StoreManagementCommandType(const StoreManagementCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StoreManagementCommandType to the contents of the
           * StoreManagementCommandType on the right hand side (rhs) of the assignment operator.StoreManagementCommandType [only
           * available to derived classes]
           *
           * @param rhs The StoreManagementCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::StoreManagementCommandType
           * @return A constant reference to this StoreManagementCommandType.
           */
         const StoreManagementCommandType& operator=(const StoreManagementCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StoreManagementCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StoreManagementCommandType_h

