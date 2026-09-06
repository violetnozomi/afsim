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
#ifndef Uci__Type__EA_TaskProtectedAssetsType_h
#define Uci__Type__EA_TaskProtectedAssetsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ProtectedAssetType_h)
# include "uci/type/ProtectedAssetType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_TaskProtectedAssetsType sequence accessor class */
      class EA_TaskProtectedAssetsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_TaskProtectedAssetsType()
         { }

         /** Returns this accessor's type constant, i.e. EA_TaskProtectedAssetsType
           *
           * @return This accessor's type constant, i.e. EA_TaskProtectedAssetsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_TaskProtectedAssetsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_TaskProtectedAssetsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the identity and priority of protected assets [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProtectedAssetType, uci::type::accessorType::protectedAssetType> ProtectedAsset;

         /** Returns the value of the SimplePrimitive data type that is identified by the ProtectAll.
           *
           * @return The value of the simple primitive data type identified by ProtectAll.
           */
         virtual xs::Boolean getProtectAll() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ProtectAll.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setProtectAll(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SelfProtect.
           *
           * @return The value of the simple primitive data type identified by SelfProtect.
           */
         virtual xs::Boolean getSelfProtect() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SelfProtect.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSelfProtect(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProtectedAsset.
           *
           * @return The bounded list identified by ProtectedAsset.
           */
         virtual const uci::type::EA_TaskProtectedAssetsType::ProtectedAsset& getProtectedAsset() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProtectedAsset.
           *
           * @return The bounded list identified by ProtectedAsset.
           */
         virtual uci::type::EA_TaskProtectedAssetsType::ProtectedAsset& getProtectedAsset()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProtectedAsset.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProtectedAsset(const uci::type::EA_TaskProtectedAssetsType::ProtectedAsset& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_TaskProtectedAssetsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_TaskProtectedAssetsType to copy from
           */
         EA_TaskProtectedAssetsType(const EA_TaskProtectedAssetsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_TaskProtectedAssetsType to the contents of the
           * EA_TaskProtectedAssetsType on the right hand side (rhs) of the assignment operator.EA_TaskProtectedAssetsType [only
           * available to derived classes]
           *
           * @param rhs The EA_TaskProtectedAssetsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EA_TaskProtectedAssetsType
           * @return A constant reference to this EA_TaskProtectedAssetsType.
           */
         const EA_TaskProtectedAssetsType& operator=(const EA_TaskProtectedAssetsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_TaskProtectedAssetsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_TaskProtectedAssetsType_h

