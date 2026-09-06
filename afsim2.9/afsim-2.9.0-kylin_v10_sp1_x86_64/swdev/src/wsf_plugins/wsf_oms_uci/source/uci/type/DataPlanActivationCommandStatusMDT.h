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
#ifndef Uci__Type__DataPlanActivationCommandStatusMDT_h
#define Uci__Type__DataPlanActivationCommandStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DataPlanActivationCommandID_Type_h)
# include "uci/type/DataPlanActivationCommandID_Type.h"
#endif

#if !defined(Uci__Type__DataPlanActivationCommandStateEnum_h)
# include "uci/type/DataPlanActivationCommandStateEnum.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** See the annotation in the associated message for an overall description of the message and this type. */
      class DataPlanActivationCommandStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DataPlanActivationCommandStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. DataPlanActivationCommandStatusMDT
           *
           * @return This accessor's type constant, i.e. DataPlanActivationCommandStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataPlanActivationCommandStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataPlanActivationCommandStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * DataPlanActivationCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataPlanActivationCommandID.
           */
         virtual const uci::type::DataPlanActivationCommandID_Type& getDataPlanActivationCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * DataPlanActivationCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DataPlanActivationCommandID.
           */
         virtual uci::type::DataPlanActivationCommandID_Type& getDataPlanActivationCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DataPlanActivationCommandID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DataPlanActivationCommandID
           */
         virtual void setDataPlanActivationCommandID(const uci::type::DataPlanActivationCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandStatus.
           *
           * @return The value of the enumeration identified by CommandStatus.
           */
         virtual const uci::type::DataPlanActivationCommandStateEnum& getCommandStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandStatus.
           *
           * @return The value of the enumeration identified by CommandStatus.
           */
         virtual uci::type::DataPlanActivationCommandStateEnum& getCommandStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandStatus(const uci::type::DataPlanActivationCommandStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandStatus(uci::type::DataPlanActivationCommandStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FailureReason.
           *
           * @return The value of the string data type identified by FailureReason.
           */
         virtual const uci::type::VisibleString1024Type& getFailureReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FailureReason.
           *
           * @return The value of the string data type identified by FailureReason.
           */
         virtual uci::type::VisibleString1024Type& getFailureReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FailureReason to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFailureReason(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FailureReason to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFailureReason(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FailureReason to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFailureReason(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FailureReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FailureReason is emabled or not
           */
         virtual bool hasFailureReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FailureReason
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableFailureReason(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FailureReason */
         virtual void clearFailureReason()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataPlanActivationCommandStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataPlanActivationCommandStatusMDT to copy from
           */
         DataPlanActivationCommandStatusMDT(const DataPlanActivationCommandStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataPlanActivationCommandStatusMDT to the contents of the
           * DataPlanActivationCommandStatusMDT on the right hand side (rhs) of the assignment
           * operator.DataPlanActivationCommandStatusMDT [only available to derived classes]
           *
           * @param rhs The DataPlanActivationCommandStatusMDT on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::DataPlanActivationCommandStatusMDT
           * @return A constant reference to this DataPlanActivationCommandStatusMDT.
           */
         const DataPlanActivationCommandStatusMDT& operator=(const DataPlanActivationCommandStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataPlanActivationCommandStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataPlanActivationCommandStatusMDT_h

