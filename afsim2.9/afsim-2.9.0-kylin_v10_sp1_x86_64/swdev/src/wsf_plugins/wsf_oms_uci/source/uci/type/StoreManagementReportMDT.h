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
#ifndef Uci__Type__StoreManagementReportMDT_h
#define Uci__Type__StoreManagementReportMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SupportCapabilityID_Type_h)
# include "uci/type/SupportCapabilityID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__StoreType_h)
# include "uci/type/StoreType.h"
#endif

#if !defined(Uci__Type__LAR_CalculationWindType_h)
# include "uci/type/LAR_CalculationWindType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This element specifies the dynamic status of the store management supporting Capability of a System. A stores
        * management supporting Capability is associated with other primary Capabilities that it controls such as Strike
        * Capabilities.
        */
      class StoreManagementReportMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~StoreManagementReportMDT()
         { }

         /** Returns this accessor's type constant, i.e. StoreManagementReportMDT
           *
           * @return This accessor's type constant, i.e. StoreManagementReportMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::storeManagementReportMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StoreManagementReportMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportCapabilityID.
           */
         virtual const uci::type::SupportCapabilityID_Type& getSupportCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SupportCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SupportCapabilityID.
           */
         virtual uci::type::SupportCapabilityID_Type& getSupportCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SupportCapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SupportCapabilityID
           */
         virtual void setSupportCapabilityID(const uci::type::SupportCapabilityID_Type& value)
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


         /** Returns the value of the SimplePrimitive data type that is identified by the ReleaseConsent.
           *
           * @return The value of the simple primitive data type identified by ReleaseConsent.
           */
         virtual xs::Boolean getReleaseConsent() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReleaseConsent.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReleaseConsent(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LAR_ConstraintsEnforced.
           *
           * @return The value of the simple primitive data type identified by LAR_ConstraintsEnforced.
           */
         virtual xs::Boolean getLAR_ConstraintsEnforced() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LAR_ConstraintsEnforced.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLAR_ConstraintsEnforced(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AttitudeConstraintsEnforced.
           *
           * @return The value of the simple primitive data type identified by AttitudeConstraintsEnforced.
           */
         virtual xs::Boolean getAttitudeConstraintsEnforced() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AttitudeConstraintsEnforced.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAttitudeConstraintsEnforced(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentStoreStation.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentStoreStation.
           */
         virtual const uci::type::ForeignKeyType& getCurrentStoreStation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CurrentStoreStation.
           *
           * @return The acecssor that provides access to the complex content that is identified by CurrentStoreStation.
           */
         virtual uci::type::ForeignKeyType& getCurrentStoreStation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CurrentStoreStation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CurrentStoreStation
           */
         virtual void setCurrentStoreStation(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CurrentStoreStation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CurrentStoreStation is emabled or not
           */
         virtual bool hasCurrentStoreStation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CurrentStoreStation
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableCurrentStoreStation(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CurrentStoreStation */
         virtual void clearCurrentStoreStation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SelectedStoreType.
           *
           * @return The acecssor that provides access to the complex content that is identified by SelectedStoreType.
           */
         virtual const uci::type::StoreType& getSelectedStoreType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SelectedStoreType.
           *
           * @return The acecssor that provides access to the complex content that is identified by SelectedStoreType.
           */
         virtual uci::type::StoreType& getSelectedStoreType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SelectedStoreType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SelectedStoreType
           */
         virtual void setSelectedStoreType(const uci::type::StoreType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SelectedStoreType exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SelectedStoreType is emabled or not
           */
         virtual bool hasSelectedStoreType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SelectedStoreType
           *
           * @param type = uci::type::accessorType::storeType This Accessor's accessor type
           */
         virtual void enableSelectedStoreType(uci::base::accessorType::AccessorType type = uci::type::accessorType::storeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SelectedStoreType */
         virtual void clearSelectedStoreType()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_CalculationWind.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_CalculationWind.
           */
         virtual const uci::type::LAR_CalculationWindType& getLAR_CalculationWind() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_CalculationWind.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_CalculationWind.
           */
         virtual uci::type::LAR_CalculationWindType& getLAR_CalculationWind()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LAR_CalculationWind to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LAR_CalculationWind
           */
         virtual void setLAR_CalculationWind(const uci::type::LAR_CalculationWindType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LAR_CalculationWind exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LAR_CalculationWind is emabled or not
           */
         virtual bool hasLAR_CalculationWind() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LAR_CalculationWind
           *
           * @param type = uci::type::accessorType::lAR_CalculationWindType This Accessor's accessor type
           */
         virtual void enableLAR_CalculationWind(uci::base::accessorType::AccessorType type = uci::type::accessorType::lAR_CalculationWindType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LAR_CalculationWind */
         virtual void clearLAR_CalculationWind()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StoreManagementReportMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StoreManagementReportMDT to copy from
           */
         StoreManagementReportMDT(const StoreManagementReportMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StoreManagementReportMDT to the contents of the
           * StoreManagementReportMDT on the right hand side (rhs) of the assignment operator.StoreManagementReportMDT [only
           * available to derived classes]
           *
           * @param rhs The StoreManagementReportMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::StoreManagementReportMDT
           * @return A constant reference to this StoreManagementReportMDT.
           */
         const StoreManagementReportMDT& operator=(const StoreManagementReportMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StoreManagementReportMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StoreManagementReportMDT_h

