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
#ifndef Uci__Type__ConstrainedOpLineType_h
#define Uci__Type__ConstrainedOpLineType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OpLineCategoryEnum_h)
# include "uci/type/OpLineCategoryEnum.h"
#endif

#if !defined(Uci__Type__OpLineID_Type_h)
# include "uci/type/OpLineID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ConstrainedOpLineType sequence accessor class */
      class ConstrainedOpLineType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ConstrainedOpLineType()
         { }

         /** Returns this accessor's type constant, i.e. ConstrainedOpLineType
           *
           * @return This accessor's type constant, i.e. ConstrainedOpLineType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::constrainedOpLineType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ConstrainedOpLineType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element indicates the type of operational line. An operational line has to contain at least one type, but can
           * contain more than one type. Includes amplification field for LINK 16 J3.0 Reference: MIL-STD-6016 [Maximum
           * occurrences: 11]
           */
         typedef uci::base::BoundedList<uci::type::OpLineCategoryEnum, uci::type::accessorType::opLineCategoryEnum> Category;

         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual const uci::type::ConstrainedOpLineType::Category& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual uci::type::ConstrainedOpLineType::Category& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Category.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCategory(const uci::type::ConstrainedOpLineType::Category& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpLineID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpLineID.
           */
         virtual const uci::type::OpLineID_Type& getOpLineID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpLineID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpLineID.
           */
         virtual uci::type::OpLineID_Type& getOpLineID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpLineID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpLineID
           */
         virtual void setOpLineID(const uci::type::OpLineID_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ConstrainedOpLineType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ConstrainedOpLineType to copy from
           */
         ConstrainedOpLineType(const ConstrainedOpLineType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ConstrainedOpLineType to the contents of the ConstrainedOpLineType
           * on the right hand side (rhs) of the assignment operator.ConstrainedOpLineType [only available to derived classes]
           *
           * @param rhs The ConstrainedOpLineType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::ConstrainedOpLineType
           * @return A constant reference to this ConstrainedOpLineType.
           */
         const ConstrainedOpLineType& operator=(const ConstrainedOpLineType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ConstrainedOpLineType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ConstrainedOpLineType_h

