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
#ifndef Uci__Type__ServiceErrorReportMDT_h
#define Uci__Type__ServiceErrorReportMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ServiceErrorReportID_Type_h)
# include "uci/type/ServiceErrorReportID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AssociatedMessageType_h)
# include "uci/type/AssociatedMessageType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ServiceErrorReportMDT sequence accessor class */
      class ServiceErrorReportMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ServiceErrorReportMDT()
         { }

         /** Returns this accessor's type constant, i.e. ServiceErrorReportMDT
           *
           * @return This accessor's type constant, i.e. ServiceErrorReportMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::serviceErrorReportMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ServiceErrorReportMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an input message that is associated with or the cause of the service error. This element should be omitted
           * if an input message was not the cause of the error. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AssociatedMessageType, uci::type::accessorType::associatedMessageType> AssociatedMessage;

         /** Returns the accessor that provides access to the complex content that is identified by the ServiceErrorReportID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceErrorReportID.
           */
         virtual const uci::type::ServiceErrorReportID_Type& getServiceErrorReportID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceErrorReportID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceErrorReportID.
           */
         virtual uci::type::ServiceErrorReportID_Type& getServiceErrorReportID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ServiceErrorReportID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ServiceErrorReportID
           */
         virtual void setServiceErrorReportID(const uci::type::ServiceErrorReportID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual const uci::type::ServiceID_Type& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual uci::type::ServiceID_Type& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ServiceID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ServiceID
           */
         virtual void setServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ErrorDetail.
           *
           * @return The value of the string data type identified by ErrorDetail.
           */
         virtual const uci::type::VisibleString1024Type& getErrorDetail() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ErrorDetail.
           *
           * @return The value of the string data type identified by ErrorDetail.
           */
         virtual uci::type::VisibleString1024Type& getErrorDetail()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ErrorDetail to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setErrorDetail(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ErrorDetail to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setErrorDetail(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ErrorDetail to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setErrorDetail(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SoftwareExceptionDetail.
           *
           * @return The value of the string data type identified by SoftwareExceptionDetail.
           */
         virtual const uci::type::VisibleString1024Type& getSoftwareExceptionDetail() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SoftwareExceptionDetail.
           *
           * @return The value of the string data type identified by SoftwareExceptionDetail.
           */
         virtual uci::type::VisibleString1024Type& getSoftwareExceptionDetail()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SoftwareExceptionDetail to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSoftwareExceptionDetail(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SoftwareExceptionDetail to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSoftwareExceptionDetail(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SoftwareExceptionDetail to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSoftwareExceptionDetail(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SoftwareExceptionDetail exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SoftwareExceptionDetail is emabled or not
           */
         virtual bool hasSoftwareExceptionDetail() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SoftwareExceptionDetail
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableSoftwareExceptionDetail(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SoftwareExceptionDetail */
         virtual void clearSoftwareExceptionDetail()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedMessage.
           *
           * @return The bounded list identified by AssociatedMessage.
           */
         virtual const uci::type::ServiceErrorReportMDT::AssociatedMessage& getAssociatedMessage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociatedMessage.
           *
           * @return The bounded list identified by AssociatedMessage.
           */
         virtual uci::type::ServiceErrorReportMDT::AssociatedMessage& getAssociatedMessage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociatedMessage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociatedMessage(const uci::type::ServiceErrorReportMDT::AssociatedMessage& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ServiceErrorReportMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ServiceErrorReportMDT to copy from
           */
         ServiceErrorReportMDT(const ServiceErrorReportMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ServiceErrorReportMDT to the contents of the ServiceErrorReportMDT
           * on the right hand side (rhs) of the assignment operator.ServiceErrorReportMDT [only available to derived classes]
           *
           * @param rhs The ServiceErrorReportMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ServiceErrorReportMDT
           * @return A constant reference to this ServiceErrorReportMDT.
           */
         const ServiceErrorReportMDT& operator=(const ServiceErrorReportMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ServiceErrorReportMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ServiceErrorReportMDT_h

