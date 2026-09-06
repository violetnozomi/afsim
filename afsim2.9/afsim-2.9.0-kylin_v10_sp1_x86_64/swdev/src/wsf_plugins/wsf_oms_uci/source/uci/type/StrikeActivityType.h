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
#ifndef Uci__Type__StrikeActivityType_h
#define Uci__Type__StrikeActivityType_h 1

#if !defined(Uci__Type__ActivityBaseType_h)
# include "uci/type/ActivityBaseType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

#if !defined(Uci__Type__AO_CodeType_h)
# include "uci/type/AO_CodeType.h"
#endif

#if !defined(Uci__Type__LAR_ID_Type_h)
# include "uci/type/LAR_ID_Type.h"
#endif

#if !defined(Uci__Type__ApprovalEnum_h)
# include "uci/type/ApprovalEnum.h"
#endif

#if !defined(Uci__Type__EnduranceFootprintType_h)
# include "uci/type/EnduranceFootprintType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the StrikeActivityType sequence accessor class */
      class StrikeActivityType : public virtual uci::type::ActivityBaseType {
      public:

         /** The destructor */
         virtual ~StrikeActivityType()
         { }

         /** Returns this accessor's type constant, i.e. StrikeActivityType
           *
           * @return This accessor's type constant, i.e. StrikeActivityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::strikeActivityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StrikeActivityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SelectedForKeyLoad.
           *
           * @return The value of the simple primitive data type identified by SelectedForKeyLoad.
           */
         virtual xs::Boolean getSelectedForKeyLoad() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SelectedForKeyLoad.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSelectedForKeyLoad(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the WeaponArmed.
           *
           * @return The value of the simple primitive data type identified by WeaponArmed.
           */
         virtual xs::Boolean getWeaponArmed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the WeaponArmed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setWeaponArmed(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ReadyForRelease.
           *
           * @return The value of the simple primitive data type identified by ReadyForRelease.
           */
         virtual xs::Boolean getReadyForRelease() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReadyForRelease.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReadyForRelease(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SelectedForRelease.
           *
           * @return The value of the simple primitive data type identified by SelectedForRelease.
           */
         virtual xs::Boolean getSelectedForRelease() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SelectedForRelease.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSelectedForRelease(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SelectedForJettison.
           *
           * @return The value of the simple primitive data type identified by SelectedForJettison.
           */
         virtual xs::Boolean getSelectedForJettison() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SelectedForJettison.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSelectedForJettison(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssignedDMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssignedDMPI_ID.
           */
         virtual const uci::type::DMPI_ID_Type& getAssignedDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AssignedDMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AssignedDMPI_ID.
           */
         virtual uci::type::DMPI_ID_Type& getAssignedDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AssignedDMPI_ID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AssignedDMPI_ID
           */
         virtual void setAssignedDMPI_ID(const uci::type::DMPI_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AssignedDMPI_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AssignedDMPI_ID is emabled or not
           */
         virtual bool hasAssignedDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AssignedDMPI_ID
           *
           * @param type = uci::type::accessorType::dMPI_ID_Type This Accessor's accessor type
           */
         virtual void enableAssignedDMPI_ID(uci::base::accessorType::AccessorType type = uci::type::accessorType::dMPI_ID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AssignedDMPI_ID */
         virtual void clearAssignedDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AO_Code.
           *
           * @return The acecssor that provides access to the complex content that is identified by AO_Code.
           */
         virtual const uci::type::AO_CodeType& getAO_Code() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AO_Code.
           *
           * @return The acecssor that provides access to the complex content that is identified by AO_Code.
           */
         virtual uci::type::AO_CodeType& getAO_Code()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AO_Code to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AO_Code
           */
         virtual void setAO_Code(const uci::type::AO_CodeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AO_Code exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AO_Code is emabled or not
           */
         virtual bool hasAO_Code() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AO_Code
           *
           * @param type = uci::type::accessorType::aO_CodeType This Accessor's accessor type
           */
         virtual void enableAO_Code(uci::base::accessorType::AccessorType type = uci::type::accessorType::aO_CodeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AO_Code */
         virtual void clearAO_Code()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_ID.
           */
         virtual const uci::type::LAR_ID_Type& getLAR_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_ID.
           */
         virtual uci::type::LAR_ID_Type& getLAR_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LAR_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LAR_ID
           */
         virtual void setLAR_ID(const uci::type::LAR_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LAR_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LAR_ID is emabled or not
           */
         virtual bool hasLAR_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LAR_ID
           *
           * @param type = uci::type::accessorType::lAR_ID_Type This Accessor's accessor type
           */
         virtual void enableLAR_ID(uci::base::accessorType::AccessorType type = uci::type::accessorType::lAR_ID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LAR_ID */
         virtual void clearLAR_ID()
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
         virtual const uci::type::ApprovalEnum& getConsentState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConsentState.
           *
           * @return The value of the enumeration identified by ConsentState.
           */
         virtual uci::type::ApprovalEnum& getConsentState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConsentState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConsentState(const uci::type::ApprovalEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConsentState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConsentState(uci::type::ApprovalEnum::EnumerationItem value)
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
           * @param type = uci::type::accessorType::approvalEnum This Accessor's accessor type
           */
         virtual void enableConsentState(uci::base::accessorType::AccessorType type = uci::type::accessorType::approvalEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearConsentState()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KineticFootprint.
           *
           * @return The acecssor that provides access to the complex content that is identified by KineticFootprint.
           */
         virtual const uci::type::EnduranceFootprintType& getKineticFootprint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the KineticFootprint.
           *
           * @return The acecssor that provides access to the complex content that is identified by KineticFootprint.
           */
         virtual uci::type::EnduranceFootprintType& getKineticFootprint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the KineticFootprint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by KineticFootprint
           */
         virtual void setKineticFootprint(const uci::type::EnduranceFootprintType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by KineticFootprint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by KineticFootprint is emabled or not
           */
         virtual bool hasKineticFootprint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by KineticFootprint
           *
           * @param type = uci::type::accessorType::enduranceFootprintType This Accessor's accessor type
           */
         virtual void enableKineticFootprint(uci::base::accessorType::AccessorType type = uci::type::accessorType::enduranceFootprintType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by KineticFootprint */
         virtual void clearKineticFootprint()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StrikeActivityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StrikeActivityType to copy from
           */
         StrikeActivityType(const StrikeActivityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StrikeActivityType to the contents of the StrikeActivityType on
           * the right hand side (rhs) of the assignment operator.StrikeActivityType [only available to derived classes]
           *
           * @param rhs The StrikeActivityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::StrikeActivityType
           * @return A constant reference to this StrikeActivityType.
           */
         const StrikeActivityType& operator=(const StrikeActivityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StrikeActivityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StrikeActivityType_h

