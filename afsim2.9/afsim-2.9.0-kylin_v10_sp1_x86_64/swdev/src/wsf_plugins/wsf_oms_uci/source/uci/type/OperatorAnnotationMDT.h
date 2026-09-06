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
#ifndef Uci__Type__OperatorAnnotationMDT_h
#define Uci__Type__OperatorAnnotationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OperatorAnnotationID_Type_h)
# include "uci/type/OperatorAnnotationID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ID_Type_h)
# include "uci/type/ID_Type.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OperatorAnnotationMDT sequence accessor class */
      class OperatorAnnotationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OperatorAnnotationMDT()
         { }

         /** Returns this accessor's type constant, i.e. OperatorAnnotationMDT
           *
           * @return This accessor's type constant, i.e. OperatorAnnotationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::operatorAnnotationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OperatorAnnotationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The specific object instance(s) with which the annotation is associated. For Entity message types, this field would
           * be populated with entity IDs. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ID_Type, uci::type::accessorType::iD_Type> ObjectID;

         /** Annotation notes and text [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::VisibleString1024Type, uci::type::accessorType::visibleString1024Type> AnnotationText;

         /** Returns the accessor that provides access to the complex content that is identified by the AnnotationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AnnotationID.
           */
         virtual const uci::type::OperatorAnnotationID_Type& getAnnotationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AnnotationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AnnotationID.
           */
         virtual uci::type::OperatorAnnotationID_Type& getAnnotationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AnnotationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AnnotationID
           */
         virtual void setAnnotationID(const uci::type::OperatorAnnotationID_Type& value)
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


         /** Returns the value of the SimplePrimitive data type that is identified by the TimestampForAnnotation.
           *
           * @return The value of the simple primitive data type identified by TimestampForAnnotation.
           */
         virtual uci::type::DateTimeTypeValue getTimestampForAnnotation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimestampForAnnotation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimestampForAnnotation(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimestampForAnnotation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimestampForAnnotation is emabled or not
           */
         virtual bool hasTimestampForAnnotation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimestampForAnnotation
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableTimestampForAnnotation(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimestampForAnnotation */
         virtual void clearTimestampForAnnotation()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ObjectID.
           *
           * @return The bounded list identified by ObjectID.
           */
         virtual const uci::type::OperatorAnnotationMDT::ObjectID& getObjectID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ObjectID.
           *
           * @return The bounded list identified by ObjectID.
           */
         virtual uci::type::OperatorAnnotationMDT::ObjectID& getObjectID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ObjectID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setObjectID(const uci::type::OperatorAnnotationMDT::ObjectID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AnnotationText.
           *
           * @return The bounded list identified by AnnotationText.
           */
         virtual const uci::type::OperatorAnnotationMDT::AnnotationText& getAnnotationText() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AnnotationText.
           *
           * @return The bounded list identified by AnnotationText.
           */
         virtual uci::type::OperatorAnnotationMDT::AnnotationText& getAnnotationText()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AnnotationText.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAnnotationText(const uci::type::OperatorAnnotationMDT::AnnotationText& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OperatorAnnotationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OperatorAnnotationMDT to copy from
           */
         OperatorAnnotationMDT(const OperatorAnnotationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OperatorAnnotationMDT to the contents of the OperatorAnnotationMDT
           * on the right hand side (rhs) of the assignment operator.OperatorAnnotationMDT [only available to derived classes]
           *
           * @param rhs The OperatorAnnotationMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::OperatorAnnotationMDT
           * @return A constant reference to this OperatorAnnotationMDT.
           */
         const OperatorAnnotationMDT& operator=(const OperatorAnnotationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OperatorAnnotationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OperatorAnnotationMDT_h

