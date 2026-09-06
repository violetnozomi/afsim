// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__DMPI_StatusMDT_h
#define Uci__Type__DMPI_StatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DMPI_StoreStatusType_h)
# include "uci/type/DMPI_StoreStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_StatusMDT sequence accessor class */
      class DMPI_StatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DMPI_StatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_StatusMDT
           *
           * @return This accessor's type constant, i.e. DMPI_StatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_StatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_StatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the DMPI status in terms of its associated store(s). The number of instances of this element varies
           * depending on the store quantity of the associated DMPI, how the associated Strike Task(s) are allocated, etc. See the
           * annotations for the DMPI_Status message for further insight. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DMPI_StoreStatusType, uci::type::accessorType::dMPI_StoreStatusType> StoreStatus;

         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_ID.
           */
         virtual const uci::type::DMPI_ID_Type& getDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_ID.
           */
         virtual uci::type::DMPI_ID_Type& getDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DMPI_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DMPI_ID
           */
         virtual void setDMPI_ID(const uci::type::DMPI_ID_Type& value)
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


         /** Returns whether the Element that is identified by MissionPlanID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPlanID is emabled or not
           */
         virtual bool hasMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPlanID
           *
           * @param type = uci::type::accessorType::missionPlanID_Type This Accessor's accessor type
           */
         virtual void enableMissionPlanID(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionPlanID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPlanID */
         virtual void clearMissionPlanID()
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


         /** Returns whether the Element that is identified by SystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SystemID is emabled or not
           */
         virtual bool hasSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SystemID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SystemID */
         virtual void clearSystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual const uci::type::TaskID_Type& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual uci::type::TaskID_Type& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskID
           */
         virtual void setTaskID(const uci::type::TaskID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TaskID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TaskID is emabled or not
           */
         virtual bool hasTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TaskID
           *
           * @param type = uci::type::accessorType::taskID_Type This Accessor's accessor type
           */
         virtual void enableTaskID(uci::base::accessorType::AccessorType type = uci::type::accessorType::taskID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TaskID */
         virtual void clearTaskID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the StoreStatus.
           *
           * @return The bounded list identified by StoreStatus.
           */
         virtual const uci::type::DMPI_StatusMDT::StoreStatus& getStoreStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the StoreStatus.
           *
           * @return The bounded list identified by StoreStatus.
           */
         virtual uci::type::DMPI_StatusMDT::StoreStatus& getStoreStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the StoreStatus.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStoreStatus(const uci::type::DMPI_StatusMDT::StoreStatus& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_StatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_StatusMDT to copy from
           */
         DMPI_StatusMDT(const DMPI_StatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_StatusMDT to the contents of the DMPI_StatusMDT on the right
           * hand side (rhs) of the assignment operator.DMPI_StatusMDT [only available to derived classes]
           *
           * @param rhs The DMPI_StatusMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::DMPI_StatusMDT
           * @return A constant reference to this DMPI_StatusMDT.
           */
         const DMPI_StatusMDT& operator=(const DMPI_StatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_StatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_StatusMDT_h

