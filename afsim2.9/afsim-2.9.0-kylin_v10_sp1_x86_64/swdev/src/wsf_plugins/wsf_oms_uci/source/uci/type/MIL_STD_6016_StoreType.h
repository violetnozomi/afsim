// This file was generated  on 10/18/2017 at 5:35:34 AM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__MIL_STD_6016_StoreType_h
#define Uci__Type__MIL_STD_6016_StoreType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__MIL_STD_6016_StoreCategoryEnum_h)
# include "uci/type/MIL_STD_6016_StoreCategoryEnum.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MIL_STD_6016_StoreType sequence accessor class */
      class MIL_STD_6016_StoreType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MIL_STD_6016_StoreType()
         { }

         /** Returns this accessor's type constant, i.e. MIL_STD_6016_StoreType
           *
           * @return This accessor's type constant, i.e. MIL_STD_6016_StoreType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mIL_STD_6016_StoreType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MIL_STD_6016_StoreType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StoreType.
           *
           * @return The value of the simple primitive data type identified by StoreType.
           */
         virtual xs::UnsignedInt getStoreType() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StoreType.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStoreType(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the StoreCategory.
           *
           * @return The value of the enumeration identified by StoreCategory.
           */
         virtual const uci::type::MIL_STD_6016_StoreCategoryEnum& getStoreCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the StoreCategory.
           *
           * @return The value of the enumeration identified by StoreCategory.
           */
         virtual uci::type::MIL_STD_6016_StoreCategoryEnum& getStoreCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the StoreCategory.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setStoreCategory(const uci::type::MIL_STD_6016_StoreCategoryEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the StoreCategory.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setStoreCategory(uci::type::MIL_STD_6016_StoreCategoryEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StoreTypeVariant.
           *
           * @return The acecssor that provides access to the complex content that is identified by StoreTypeVariant.
           */
         virtual const uci::type::ForeignKeyType& getStoreTypeVariant() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StoreTypeVariant.
           *
           * @return The acecssor that provides access to the complex content that is identified by StoreTypeVariant.
           */
         virtual uci::type::ForeignKeyType& getStoreTypeVariant()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the StoreTypeVariant to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by StoreTypeVariant
           */
         virtual void setStoreTypeVariant(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StoreTypeVariant exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StoreTypeVariant is emabled or not
           */
         virtual bool hasStoreTypeVariant() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StoreTypeVariant
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableStoreTypeVariant(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StoreTypeVariant */
         virtual void clearStoreTypeVariant()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MIL_STD_6016_StoreType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MIL_STD_6016_StoreType to copy from
           */
         MIL_STD_6016_StoreType(const MIL_STD_6016_StoreType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MIL_STD_6016_StoreType to the contents of the
           * MIL_STD_6016_StoreType on the right hand side (rhs) of the assignment operator.MIL_STD_6016_StoreType [only available
           * to derived classes]
           *
           * @param rhs The MIL_STD_6016_StoreType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::MIL_STD_6016_StoreType
           * @return A constant reference to this MIL_STD_6016_StoreType.
           */
         const MIL_STD_6016_StoreType& operator=(const MIL_STD_6016_StoreType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MIL_STD_6016_StoreType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MIL_STD_6016_StoreType_h

