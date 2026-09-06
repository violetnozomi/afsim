// This file was generated  on 10/18/2017 at 5:35:32 AM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* DISTRIBUTION STATEMENT D.  Distribution authorized to the Department 
*    of Defense and U.S. DoD contractors only; Critical Technology; 26 Mar 13.
*    Other requests shall be referred to the Air Force Rapid Capabilities 
*    Office, 202-404-5497 or 202-404-5498.
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
#ifndef Uci__Type__EOB_TimestampsType_h
#define Uci__Type__EOB_TimestampsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EOB_TimestampsType sequence accessor class */
      class EOB_TimestampsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EOB_TimestampsType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_TimestampsType
           *
           * @return This accessor's type constant, i.e. EOB_TimestampsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_TimestampsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_TimestampsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FirstObservation.
           *
           * @return The value of the simple primitive data type identified by FirstObservation.
           */
         virtual uci::type::DateTimeTypeValue getFirstObservation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FirstObservation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFirstObservation(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FirstObservation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FirstObservation is emabled or not
           */
         virtual bool hasFirstObservation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FirstObservation
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableFirstObservation(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FirstObservation */
         virtual void clearFirstObservation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LastObservation.
           *
           * @return The value of the simple primitive data type identified by LastObservation.
           */
         virtual uci::type::DateTimeTypeValue getLastObservation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LastObservation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLastObservation(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LastObservation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LastObservation is emabled or not
           */
         virtual bool hasLastObservation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LastObservation
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableLastObservation(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LastObservation */
         virtual void clearLastObservation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TimeCreated.
           *
           * @return The value of the simple primitive data type identified by TimeCreated.
           */
         virtual uci::type::DateTimeTypeValue getTimeCreated() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimeCreated.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimeCreated(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeCreated exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeCreated is emabled or not
           */
         virtual bool hasTimeCreated() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeCreated
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableTimeCreated(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeCreated */
         virtual void clearTimeCreated()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_TimestampsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_TimestampsType to copy from
           */
         EOB_TimestampsType(const EOB_TimestampsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_TimestampsType to the contents of the EOB_TimestampsType on
           * the right hand side (rhs) of the assignment operator.EOB_TimestampsType [only available to derived classes]
           *
           * @param rhs The EOB_TimestampsType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EOB_TimestampsType
           * @return A constant reference to this EOB_TimestampsType.
           */
         const EOB_TimestampsType& operator=(const EOB_TimestampsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_TimestampsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_TimestampsType_h

