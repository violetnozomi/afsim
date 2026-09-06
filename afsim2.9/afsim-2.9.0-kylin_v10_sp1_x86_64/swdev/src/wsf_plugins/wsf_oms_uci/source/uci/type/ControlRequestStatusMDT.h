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
#ifndef Uci__Type__ControlRequestStatusMDT_h
#define Uci__Type__ControlRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ControlRequestID_Type_h)
# include "uci/type/ControlRequestID_Type.h"
#endif

#if !defined(Uci__Type__ApprovalEnum_h)
# include "uci/type/ApprovalEnum.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ControlRequestStatusMDT sequence accessor class */
      class ControlRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ControlRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. ControlRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. ControlRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::controlRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ControlRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControlRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControlRequestID.
           */
         virtual const uci::type::ControlRequestID_Type& getControlRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControlRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControlRequestID.
           */
         virtual uci::type::ControlRequestID_Type& getControlRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ControlRequestID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ControlRequestID
           */
         virtual void setControlRequestID(const uci::type::ControlRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalType.
           *
           * @return The value of the enumeration identified by ApprovalType.
           */
         virtual const uci::type::ApprovalEnum& getApprovalType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ApprovalType.
           *
           * @return The value of the enumeration identified by ApprovalType.
           */
         virtual uci::type::ApprovalEnum& getApprovalType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalType(const uci::type::ApprovalEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ApprovalType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setApprovalType(uci::type::ApprovalEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Remarks.
           *
           * @return The value of the string data type identified by Remarks.
           */
         virtual const uci::type::VisibleString1024Type& getRemarks() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Remarks.
           *
           * @return The value of the string data type identified by Remarks.
           */
         virtual uci::type::VisibleString1024Type& getRemarks()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRemarks(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarks(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarks(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Remarks exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Remarks is emabled or not
           */
         virtual bool hasRemarks() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Remarks
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableRemarks(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Remarks */
         virtual void clearRemarks()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlRequestStatusMDT to copy from
           */
         ControlRequestStatusMDT(const ControlRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlRequestStatusMDT to the contents of the
           * ControlRequestStatusMDT on the right hand side (rhs) of the assignment operator.ControlRequestStatusMDT [only
           * available to derived classes]
           *
           * @param rhs The ControlRequestStatusMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ControlRequestStatusMDT
           * @return A constant reference to this ControlRequestStatusMDT.
           */
         const ControlRequestStatusMDT& operator=(const ControlRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ControlRequestStatusMDT_h

