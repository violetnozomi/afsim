// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ProductCollectionLocationType_h
#define Uci__Type__ProductCollectionLocationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__InertialStateType_h)
# include "uci/type/InertialStateType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProductCollectionPathType_h)
# include "uci/type/ProductCollectionPathType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ProductCollectionLocationType sequence accessor class */
      class ProductCollectionLocationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ProductCollectionLocationType()
         { }

         /** Returns this accessor's type constant, i.e. ProductCollectionLocationType
           *
           * @return This accessor's type constant, i.e. ProductCollectionLocationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::productCollectionLocationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ProductCollectionLocationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The collection path as traversed by the host platform. This allows the capture of an abbreviated time history of
           * platform motion. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductCollectionPathType, uci::type::accessorType::productCollectionPathType> CollectionPath;

         /** Returns the accessor that provides access to the complex content that is identified by the CollectionPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionPoint.
           */
         virtual const uci::type::InertialStateType& getCollectionPoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionPoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionPoint.
           */
         virtual uci::type::InertialStateType& getCollectionPoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionPoint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionPoint
           */
         virtual void setCollectionPoint(const uci::type::InertialStateType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CollectionPath.
           *
           * @return The bounded list identified by CollectionPath.
           */
         virtual const uci::type::ProductCollectionLocationType::CollectionPath& getCollectionPath() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CollectionPath.
           *
           * @return The bounded list identified by CollectionPath.
           */
         virtual uci::type::ProductCollectionLocationType::CollectionPath& getCollectionPath()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CollectionPath.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCollectionPath(const uci::type::ProductCollectionLocationType::CollectionPath& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ProductCollectionLocationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ProductCollectionLocationType to copy from
           */
         ProductCollectionLocationType(const ProductCollectionLocationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ProductCollectionLocationType to the contents of the
           * ProductCollectionLocationType on the right hand side (rhs) of the assignment operator.ProductCollectionLocationType
           * [only available to derived classes]
           *
           * @param rhs The ProductCollectionLocationType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ProductCollectionLocationType
           * @return A constant reference to this ProductCollectionLocationType.
           */
         const ProductCollectionLocationType& operator=(const ProductCollectionLocationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ProductCollectionLocationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ProductCollectionLocationType_h

