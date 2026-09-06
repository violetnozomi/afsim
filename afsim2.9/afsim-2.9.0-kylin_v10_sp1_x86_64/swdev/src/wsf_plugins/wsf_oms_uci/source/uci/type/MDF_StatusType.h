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
#ifndef Uci__Type__MDF_StatusType_h
#define Uci__Type__MDF_StatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MDF_CommandType_h)
# include "uci/type/MDF_CommandType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MDF_StatusType sequence accessor class */
      class MDF_StatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MDF_StatusType()
         { }

         /** Returns this accessor's type constant, i.e. MDF_StatusType
           *
           * @return This accessor's type constant, i.e. MDF_StatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mDF_StatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MDF_StatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the currently active Mission Data File for this Capability. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MDF_CommandType, uci::type::accessorType::mDF_CommandType> CurrentMDF;

         /** Indicates the MDF that has been commanded for activation for this Capability. Upon completion of activation, this MDF
           * becomes the CurrentMDF. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MDF_CommandType, uci::type::accessorType::mDF_CommandType> CommandedMDF;

         /** Indicates the MDF that most recently failed activation for this Capability. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MDF_CommandType, uci::type::accessorType::mDF_CommandType> FailedMDF;

         /** Returns the bounded list that is identified by the CurrentMDF.
           *
           * @return The bounded list identified by CurrentMDF.
           */
         virtual const uci::type::MDF_StatusType::CurrentMDF& getCurrentMDF() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CurrentMDF.
           *
           * @return The bounded list identified by CurrentMDF.
           */
         virtual uci::type::MDF_StatusType::CurrentMDF& getCurrentMDF()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CurrentMDF.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCurrentMDF(const uci::type::MDF_StatusType::CurrentMDF& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommandedMDF.
           *
           * @return The bounded list identified by CommandedMDF.
           */
         virtual const uci::type::MDF_StatusType::CommandedMDF& getCommandedMDF() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CommandedMDF.
           *
           * @return The bounded list identified by CommandedMDF.
           */
         virtual uci::type::MDF_StatusType::CommandedMDF& getCommandedMDF()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CommandedMDF.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommandedMDF(const uci::type::MDF_StatusType::CommandedMDF& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FailedMDF.
           *
           * @return The bounded list identified by FailedMDF.
           */
         virtual const uci::type::MDF_StatusType::FailedMDF& getFailedMDF() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FailedMDF.
           *
           * @return The bounded list identified by FailedMDF.
           */
         virtual uci::type::MDF_StatusType::FailedMDF& getFailedMDF()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FailedMDF.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFailedMDF(const uci::type::MDF_StatusType::FailedMDF& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MDF_StatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MDF_StatusType to copy from
           */
         MDF_StatusType(const MDF_StatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MDF_StatusType to the contents of the MDF_StatusType on the right
           * hand side (rhs) of the assignment operator.MDF_StatusType [only available to derived classes]
           *
           * @param rhs The MDF_StatusType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::MDF_StatusType
           * @return A constant reference to this MDF_StatusType.
           */
         const MDF_StatusType& operator=(const MDF_StatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MDF_StatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MDF_StatusType_h

