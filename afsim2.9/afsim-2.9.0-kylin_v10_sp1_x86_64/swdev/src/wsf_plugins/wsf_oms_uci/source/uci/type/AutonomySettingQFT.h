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
#ifndef Uci__Type__AutonomySettingQFT_h
#define Uci__Type__AutonomySettingQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AutonomySettingID_Type_h)
# include "uci/type/AutonomySettingID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AutonomySettingQFT sequence accessor class */
      class AutonomySettingQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~AutonomySettingQFT()
         { }

         /** Returns this accessor's type constant, i.e. AutonomySettingQFT
           *
           * @return This accessor's type constant, i.e. AutonomySettingQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::autonomySettingQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AutonomySettingQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::AutonomySettingID_Type, uci::type::accessorType::autonomySettingID_Type> AutonomySettingID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SystemID_Type, uci::type::accessorType::systemID_Type> ApplicableSystemID;

         /** Returns the bounded list that is identified by the AutonomySettingID.
           *
           * @return The bounded list identified by AutonomySettingID.
           */
         virtual const uci::type::AutonomySettingQFT::AutonomySettingID& getAutonomySettingID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AutonomySettingID.
           *
           * @return The bounded list identified by AutonomySettingID.
           */
         virtual uci::type::AutonomySettingQFT::AutonomySettingID& getAutonomySettingID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AutonomySettingID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAutonomySettingID(const uci::type::AutonomySettingQFT::AutonomySettingID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual const uci::type::AutonomySettingQFT::ApplicableSystemID& getApplicableSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ApplicableSystemID.
           *
           * @return The bounded list identified by ApplicableSystemID.
           */
         virtual uci::type::AutonomySettingQFT::ApplicableSystemID& getApplicableSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ApplicableSystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setApplicableSystemID(const uci::type::AutonomySettingQFT::ApplicableSystemID& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AutonomySettingQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AutonomySettingQFT to copy from
           */
         AutonomySettingQFT(const AutonomySettingQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AutonomySettingQFT to the contents of the AutonomySettingQFT on
           * the right hand side (rhs) of the assignment operator.AutonomySettingQFT [only available to derived classes]
           *
           * @param rhs The AutonomySettingQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AutonomySettingQFT
           * @return A constant reference to this AutonomySettingQFT.
           */
         const AutonomySettingQFT& operator=(const AutonomySettingQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AutonomySettingQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AutonomySettingQFT_h

