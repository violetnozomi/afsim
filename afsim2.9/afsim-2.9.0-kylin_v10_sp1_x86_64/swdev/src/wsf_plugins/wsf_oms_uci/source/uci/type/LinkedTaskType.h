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
#ifndef Uci__Type__LinkedTaskType_h
#define Uci__Type__LinkedTaskType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LinkedTaskType sequence accessor class */
      class LinkedTaskType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LinkedTaskType()
         { }

         /** Returns this accessor's type constant, i.e. LinkedTaskType
           *
           * @return This accessor's type constant, i.e. LinkedTaskType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::linkedTaskType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LinkedTaskType& accessor)
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


         /** Returns the value of the SimplePrimitive data type that is identified by the Required.
           *
           * @return The value of the simple primitive data type identified by Required.
           */
         virtual xs::Boolean getRequired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Required.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRequired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LinkedTaskType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LinkedTaskType to copy from
           */
         LinkedTaskType(const LinkedTaskType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LinkedTaskType to the contents of the LinkedTaskType on the right
           * hand side (rhs) of the assignment operator.LinkedTaskType [only available to derived classes]
           *
           * @param rhs The LinkedTaskType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::LinkedTaskType
           * @return A constant reference to this LinkedTaskType.
           */
         const LinkedTaskType& operator=(const LinkedTaskType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LinkedTaskType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LinkedTaskType_h

