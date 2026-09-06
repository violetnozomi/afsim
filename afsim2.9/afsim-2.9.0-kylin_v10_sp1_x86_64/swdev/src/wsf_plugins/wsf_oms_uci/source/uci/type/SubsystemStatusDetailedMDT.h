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
#ifndef Uci__Type__SubsystemStatusDetailedMDT_h
#define Uci__Type__SubsystemStatusDetailedMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SubsystemStatusDetailedNameValuePairType_h)
# include "uci/type/SubsystemStatusDetailedNameValuePairType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This contains the current detailed status of the reporting Subsystem. This message is for reporting subsystem unique
        * data that cannot be reported with other messages.
        */
      class SubsystemStatusDetailedMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemStatusDetailedMDT()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemStatusDetailedMDT
           *
           * @return This accessor's type constant, i.e. SubsystemStatusDetailedMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemStatusDetailedMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemStatusDetailedMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This contains detailed status of the Subsystem. This field is used to provide status for subsystem unique attributes
           * that cannot be reported with other messages. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemStatusDetailedNameValuePairType, uci::type::accessorType::subsystemStatusDetailedNameValuePairType> Status;

         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Status.
           *
           * @return The bounded list identified by Status.
           */
         virtual const uci::type::SubsystemStatusDetailedMDT::Status& getStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Status.
           *
           * @return The bounded list identified by Status.
           */
         virtual uci::type::SubsystemStatusDetailedMDT::Status& getStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Status.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setStatus(const uci::type::SubsystemStatusDetailedMDT::Status& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemStatusDetailedMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemStatusDetailedMDT to copy from
           */
         SubsystemStatusDetailedMDT(const SubsystemStatusDetailedMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemStatusDetailedMDT to the contents of the
           * SubsystemStatusDetailedMDT on the right hand side (rhs) of the assignment operator.SubsystemStatusDetailedMDT [only
           * available to derived classes]
           *
           * @param rhs The SubsystemStatusDetailedMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SubsystemStatusDetailedMDT
           * @return A constant reference to this SubsystemStatusDetailedMDT.
           */
         const SubsystemStatusDetailedMDT& operator=(const SubsystemStatusDetailedMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemStatusDetailedMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemStatusDetailedMDT_h

