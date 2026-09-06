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
#ifndef Uci__Type__MissionPlanStatusMDT_h
#define Uci__Type__MissionPlanStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__SystemSourceEnum_h)
# include "uci/type/SystemSourceEnum.h"
#endif

#if !defined(Uci__Type__VehicleStatusType_h)
# include "uci/type/VehicleStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanStatusMDT sequence accessor class */
      class MissionPlanStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanStatusMDT
           *
           * @return This accessor's type constant, i.e. MissionPlanStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanID
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Source.
           *
           * @return The value of the enumeration identified by Source.
           */
         virtual const uci::type::SystemSourceEnum& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Source.
           *
           * @return The value of the enumeration identified by Source.
           */
         virtual uci::type::SystemSourceEnum& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Source.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSource(const uci::type::SystemSourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Source.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSource(uci::type::SystemSourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleStatus.
           */
         virtual const uci::type::VehicleStatusType& getVehicleStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the VehicleStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by VehicleStatus.
           */
         virtual uci::type::VehicleStatusType& getVehicleStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the VehicleStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by VehicleStatus
           */
         virtual void setVehicleStatus(const uci::type::VehicleStatusType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanStatusMDT to copy from
           */
         MissionPlanStatusMDT(const MissionPlanStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanStatusMDT to the contents of the MissionPlanStatusMDT
           * on the right hand side (rhs) of the assignment operator.MissionPlanStatusMDT [only available to derived classes]
           *
           * @param rhs The MissionPlanStatusMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::MissionPlanStatusMDT
           * @return A constant reference to this MissionPlanStatusMDT.
           */
         const MissionPlanStatusMDT& operator=(const MissionPlanStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanStatusMDT_h

