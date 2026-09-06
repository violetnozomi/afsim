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
#ifndef Uci__Type__ConstrainedOpPointType_h
#define Uci__Type__ConstrainedOpPointType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OpPointCategoryEnum_h)
# include "uci/type/OpPointCategoryEnum.h"
#endif

#if !defined(Uci__Type__OpPointID_Type_h)
# include "uci/type/OpPointID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ConstrainedOpPointType sequence accessor class */
      class ConstrainedOpPointType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ConstrainedOpPointType()
         { }

         /** Returns this accessor's type constant, i.e. ConstrainedOpPointType
           *
           * @return This accessor's type constant, i.e. ConstrainedOpPointType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::constrainedOpPointType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ConstrainedOpPointType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element indicates the type of operational point. An operational point has to contain at least one type, but can
           * contain more than one type. [Maximum occurrences: 14]
           */
         typedef uci::base::BoundedList<uci::type::OpPointCategoryEnum, uci::type::accessorType::opPointCategoryEnum> Category;

         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual const uci::type::ConstrainedOpPointType::Category& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual uci::type::ConstrainedOpPointType::Category& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Category.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCategory(const uci::type::ConstrainedOpPointType::Category& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpPointID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpPointID.
           */
         virtual const uci::type::OpPointID_Type& getOpPointID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpPointID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpPointID.
           */
         virtual uci::type::OpPointID_Type& getOpPointID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpPointID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpPointID
           */
         virtual void setOpPointID(const uci::type::OpPointID_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ConstrainedOpPointType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ConstrainedOpPointType to copy from
           */
         ConstrainedOpPointType(const ConstrainedOpPointType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ConstrainedOpPointType to the contents of the
           * ConstrainedOpPointType on the right hand side (rhs) of the assignment operator.ConstrainedOpPointType [only available
           * to derived classes]
           *
           * @param rhs The ConstrainedOpPointType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ConstrainedOpPointType
           * @return A constant reference to this ConstrainedOpPointType.
           */
         const ConstrainedOpPointType& operator=(const ConstrainedOpPointType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ConstrainedOpPointType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ConstrainedOpPointType_h

