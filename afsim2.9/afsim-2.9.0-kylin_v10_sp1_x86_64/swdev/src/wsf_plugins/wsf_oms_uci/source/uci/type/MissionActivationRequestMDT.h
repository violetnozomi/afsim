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
#ifndef Uci__Type__MissionActivationRequestMDT_h
#define Uci__Type__MissionActivationRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionActivationRequestID_Type_h)
# include "uci/type/MissionActivationRequestID_Type.h"
#endif

#if !defined(Uci__Type__MissionID_Type_h)
# include "uci/type/MissionID_Type.h"
#endif

#if !defined(Uci__Type__MissionStateEnum_h)
# include "uci/type/MissionStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionActivationRequestMDT sequence accessor class */
      class MissionActivationRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionActivationRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. MissionActivationRequestMDT
           *
           * @return This accessor's type constant, i.e. MissionActivationRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionActivationRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionActivationRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * MissionActivationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionActivationRequestID.
           */
         virtual const uci::type::MissionActivationRequestID_Type& getMissionActivationRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * MissionActivationRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionActivationRequestID.
           */
         virtual uci::type::MissionActivationRequestID_Type& getMissionActivationRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionActivationRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionActivationRequestID
           */
         virtual void setMissionActivationRequestID(const uci::type::MissionActivationRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual const uci::type::MissionID_Type& getMissionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual uci::type::MissionID_Type& getMissionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionID
           */
         virtual void setMissionID(const uci::type::MissionID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MissionState.
           *
           * @return The value of the enumeration identified by MissionState.
           */
         virtual const uci::type::MissionStateEnum& getMissionState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MissionState.
           *
           * @return The value of the enumeration identified by MissionState.
           */
         virtual uci::type::MissionStateEnum& getMissionState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MissionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMissionState(const uci::type::MissionStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MissionState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMissionState(uci::type::MissionStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionActivationRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionActivationRequestMDT to copy from
           */
         MissionActivationRequestMDT(const MissionActivationRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionActivationRequestMDT to the contents of the
           * MissionActivationRequestMDT on the right hand side (rhs) of the assignment operator.MissionActivationRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The MissionActivationRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::MissionActivationRequestMDT
           * @return A constant reference to this MissionActivationRequestMDT.
           */
         const MissionActivationRequestMDT& operator=(const MissionActivationRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionActivationRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionActivationRequestMDT_h

