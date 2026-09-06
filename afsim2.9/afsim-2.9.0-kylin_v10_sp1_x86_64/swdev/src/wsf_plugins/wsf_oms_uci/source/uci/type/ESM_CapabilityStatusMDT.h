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
#ifndef Uci__Type__ESM_CapabilityStatusMDT_h
#define Uci__Type__ESM_CapabilityStatusMDT_h 1

#if !defined(Uci__Type__CapabilityStatusBaseType_h)
# include "uci/type/CapabilityStatusBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityStatusType_h)
# include "uci/type/CapabilityStatusType.h"
#endif

#if !defined(Uci__Type__RF_ProfileType_h)
# include "uci/type/RF_ProfileType.h"
#endif

#if !defined(Uci__Type__AmbiguityOrderingEnum_h)
# include "uci/type/AmbiguityOrderingEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ESM_CapabilityStatusMDT sequence accessor class */
      class ESM_CapabilityStatusMDT : public virtual uci::type::CapabilityStatusBaseType {
      public:

         /** The destructor */
         virtual ~ESM_CapabilityStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. ESM_CapabilityStatusMDT
           *
           * @return This accessor's type constant, i.e. ESM_CapabilityStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eSM_CapabilityStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ESM_CapabilityStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the high level availability/state of a Capability instance. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityStatusType, uci::type::accessorType::capabilityStatusType> CapabilityStatus;

         /** Indicates a RF Emission Profile currently applied to the Capability. If omitted, there are no current profiles and
           * the Capability is operating per its defaults and other constraints. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RF_ProfileType, uci::type::accessorType::rF_ProfileType> RF_Profile;

         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual const uci::type::ESM_CapabilityStatusMDT::CapabilityStatus& getCapabilityStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityStatus.
           *
           * @return The bounded list identified by CapabilityStatus.
           */
         virtual uci::type::ESM_CapabilityStatusMDT::CapabilityStatus& getCapabilityStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityStatus(const uci::type::ESM_CapabilityStatusMDT::CapabilityStatus& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Profile.
           *
           * @return The bounded list identified by RF_Profile.
           */
         virtual const uci::type::ESM_CapabilityStatusMDT::RF_Profile& getRF_Profile() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Profile.
           *
           * @return The bounded list identified by RF_Profile.
           */
         virtual uci::type::ESM_CapabilityStatusMDT::RF_Profile& getRF_Profile()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_Profile.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_Profile(const uci::type::ESM_CapabilityStatusMDT::RF_Profile& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AmbiguityOrdering.
           *
           * @return The value of the enumeration identified by AmbiguityOrdering.
           */
         virtual const uci::type::AmbiguityOrderingEnum& getAmbiguityOrdering() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AmbiguityOrdering.
           *
           * @return The value of the enumeration identified by AmbiguityOrdering.
           */
         virtual uci::type::AmbiguityOrderingEnum& getAmbiguityOrdering()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AmbiguityOrdering.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAmbiguityOrdering(const uci::type::AmbiguityOrderingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AmbiguityOrdering.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAmbiguityOrdering(uci::type::AmbiguityOrderingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield AmbiguityOrderingis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasAmbiguityOrdering() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getAmbiguityOrdering will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::ambiguityOrderingEnum This Accessor's accessor type
           */
         virtual void enableAmbiguityOrdering(uci::base::accessorType::AccessorType type = uci::type::accessorType::ambiguityOrderingEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearAmbiguityOrdering()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RWR_AudioEnable.
           *
           * @return The value of the simple primitive data type identified by RWR_AudioEnable.
           */
         virtual xs::Boolean getRWR_AudioEnable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RWR_AudioEnable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRWR_AudioEnable(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RWR_AudioEnable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RWR_AudioEnable is emabled or not
           */
         virtual bool hasRWR_AudioEnable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RWR_AudioEnable
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableRWR_AudioEnable(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RWR_AudioEnable */
         virtual void clearRWR_AudioEnable()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SpectralDensityReporting.
           *
           * @return The value of the simple primitive data type identified by SpectralDensityReporting.
           */
         virtual xs::Boolean getSpectralDensityReporting() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SpectralDensityReporting.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSpectralDensityReporting(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SpectralDensityReporting exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpectralDensityReporting is emabled or not
           */
         virtual bool hasSpectralDensityReporting() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpectralDensityReporting
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableSpectralDensityReporting(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpectralDensityReporting */
         virtual void clearSpectralDensityReporting()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ESM_CapabilityStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ESM_CapabilityStatusMDT to copy from
           */
         ESM_CapabilityStatusMDT(const ESM_CapabilityStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ESM_CapabilityStatusMDT to the contents of the
           * ESM_CapabilityStatusMDT on the right hand side (rhs) of the assignment operator.ESM_CapabilityStatusMDT [only
           * available to derived classes]
           *
           * @param rhs The ESM_CapabilityStatusMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ESM_CapabilityStatusMDT
           * @return A constant reference to this ESM_CapabilityStatusMDT.
           */
         const ESM_CapabilityStatusMDT& operator=(const ESM_CapabilityStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ESM_CapabilityStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ESM_CapabilityStatusMDT_h

