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
#ifndef Uci__Type__MDF_ReferenceType_h
#define Uci__Type__MDF_ReferenceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MDF_ID_Type_h)
# include "uci/type/MDF_ID_Type.h"
#endif

#if !defined(Uci__Type__MDF_SubCategoryFileID_Type_h)
# include "uci/type/MDF_SubCategoryFileID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type is used to reference an MDF and optionally an MDF subcategory file and/or specific items within the file or
        * MDF.
        */
      class MDF_ReferenceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MDF_ReferenceType()
         { }

         /** Returns this accessor's type constant, i.e. MDF_ReferenceType
           *
           * @return This accessor's type constant, i.e. MDF_ReferenceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mDF_ReferenceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MDF_ReferenceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the index of a specific item within the referenced MDF or MDF subcategory file. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> MDF_ItemIndex;

         /** Returns the accessor that provides access to the complex content that is identified by the MDF_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MDF_ID.
           */
         virtual const uci::type::MDF_ID_Type& getMDF_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MDF_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MDF_ID.
           */
         virtual uci::type::MDF_ID_Type& getMDF_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MDF_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MDF_ID
           */
         virtual void setMDF_ID(const uci::type::MDF_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MDF_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MDF_ID is emabled or not
           */
         virtual bool hasMDF_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MDF_ID
           *
           * @param type = uci::type::accessorType::mDF_ID_Type This Accessor's accessor type
           */
         virtual void enableMDF_ID(uci::base::accessorType::AccessorType type = uci::type::accessorType::mDF_ID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MDF_ID */
         virtual void clearMDF_ID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MDF_SubCategoryFileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MDF_SubCategoryFileID.
           */
         virtual const uci::type::MDF_SubCategoryFileID_Type& getMDF_SubCategoryFileID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MDF_SubCategoryFileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MDF_SubCategoryFileID.
           */
         virtual uci::type::MDF_SubCategoryFileID_Type& getMDF_SubCategoryFileID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MDF_SubCategoryFileID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MDF_SubCategoryFileID
           */
         virtual void setMDF_SubCategoryFileID(const uci::type::MDF_SubCategoryFileID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MDF_SubCategoryFileID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MDF_SubCategoryFileID is emabled or not
           */
         virtual bool hasMDF_SubCategoryFileID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MDF_SubCategoryFileID
           *
           * @param type = uci::type::accessorType::mDF_SubCategoryFileID_Type This Accessor's accessor type
           */
         virtual void enableMDF_SubCategoryFileID(uci::base::accessorType::AccessorType type = uci::type::accessorType::mDF_SubCategoryFileID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MDF_SubCategoryFileID */
         virtual void clearMDF_SubCategoryFileID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MDF_ItemIndex.
           *
           * @return The bounded list identified by MDF_ItemIndex.
           */
         virtual const uci::type::MDF_ReferenceType::MDF_ItemIndex& getMDF_ItemIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MDF_ItemIndex.
           *
           * @return The bounded list identified by MDF_ItemIndex.
           */
         virtual uci::type::MDF_ReferenceType::MDF_ItemIndex& getMDF_ItemIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MDF_ItemIndex.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMDF_ItemIndex(const uci::type::MDF_ReferenceType::MDF_ItemIndex& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MDF_ReferenceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MDF_ReferenceType to copy from
           */
         MDF_ReferenceType(const MDF_ReferenceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MDF_ReferenceType to the contents of the MDF_ReferenceType on the
           * right hand side (rhs) of the assignment operator.MDF_ReferenceType [only available to derived classes]
           *
           * @param rhs The MDF_ReferenceType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::MDF_ReferenceType
           * @return A constant reference to this MDF_ReferenceType.
           */
         const MDF_ReferenceType& operator=(const MDF_ReferenceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MDF_ReferenceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MDF_ReferenceType_h

