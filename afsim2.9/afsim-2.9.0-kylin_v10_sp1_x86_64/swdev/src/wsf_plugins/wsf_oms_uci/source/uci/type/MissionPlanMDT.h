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
#ifndef Uci__Type__MissionPlanMDT_h
#define Uci__Type__MissionPlanMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanCommandID_Type_h)
# include "uci/type/MissionPlanCommandID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanApprovalEnum_h)
# include "uci/type/MissionPlanApprovalEnum.h"
#endif

#if !defined(Uci__Type__MissionPlanDetailsType_h)
# include "uci/type/MissionPlanDetailsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanMDT sequence accessor class */
      class MissionPlanMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanMDT()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanMDT
           *
           * @return This accessor's type constant, i.e. MissionPlanMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanMDT& accessor)
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


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanCommandID.
           */
         virtual const uci::type::MissionPlanCommandID_Type& getMissionPlanCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanCommandID.
           */
         virtual uci::type::MissionPlanCommandID_Type& getMissionPlanCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanCommandID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanCommandID
           */
         virtual void setMissionPlanCommandID(const uci::type::MissionPlanCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionPlanCommandID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPlanCommandID is emabled or not
           */
         virtual bool hasMissionPlanCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPlanCommandID
           *
           * @param type = uci::type::accessorType::missionPlanCommandID_Type This Accessor's accessor type
           */
         virtual void enableMissionPlanCommandID(uci::base::accessorType::AccessorType type = uci::type::accessorType::missionPlanCommandID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPlanCommandID */
         virtual void clearMissionPlanCommandID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalState.
           *
           * @return The value of the enumeration identified by ApprovalState.
           */
         virtual const uci::type::MissionPlanApprovalEnum& getApprovalState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalState.
           *
           * @return The value of the enumeration identified by ApprovalState.
           */
         virtual uci::type::MissionPlanApprovalEnum& getApprovalState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalState(const uci::type::MissionPlanApprovalEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalState(uci::type::MissionPlanApprovalEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by Details.
           */
         virtual const uci::type::MissionPlanDetailsType& getDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by Details.
           */
         virtual uci::type::MissionPlanDetailsType& getDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Details to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Details
           */
         virtual void setDetails(const uci::type::MissionPlanDetailsType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanMDT to copy from
           */
         MissionPlanMDT(const MissionPlanMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanMDT to the contents of the MissionPlanMDT on the right
           * hand side (rhs) of the assignment operator.MissionPlanMDT [only available to derived classes]
           *
           * @param rhs The MissionPlanMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::MissionPlanMDT
           * @return A constant reference to this MissionPlanMDT.
           */
         const MissionPlanMDT& operator=(const MissionPlanMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanMDT_h

