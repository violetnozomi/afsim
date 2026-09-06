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
#ifndef Uci__Type__MDF_SubCategoryType_h
#define Uci__Type__MDF_SubCategoryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MDF_SubCategoryID_Type_h)
# include "uci/type/MDF_SubCategoryID_Type.h"
#endif

#if !defined(Uci__Type__MDF_StatusEnum_h)
# include "uci/type/MDF_StatusEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MDF_FileType_h)
# include "uci/type/MDF_FileType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MDF_SubCategoryType sequence accessor class */
      class MDF_SubCategoryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MDF_SubCategoryType()
         { }

         /** Returns this accessor's type constant, i.e. MDF_SubCategoryType
           *
           * @return This accessor's type constant, i.e. MDF_SubCategoryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mDF_SubCategoryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MDF_SubCategoryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the structure to define the file ID and status of the file(s) associated with the specified MDF Subcategory
           * file or fileset. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::MDF_FileType, uci::type::accessorType::mDF_FileType> File;

         /** Returns the accessor that provides access to the complex content that is identified by the SubCategoryID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCategoryID.
           */
         virtual const uci::type::MDF_SubCategoryID_Type& getSubCategoryID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubCategoryID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubCategoryID.
           */
         virtual uci::type::MDF_SubCategoryID_Type& getSubCategoryID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubCategoryID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubCategoryID
           */
         virtual void setSubCategoryID(const uci::type::MDF_SubCategoryID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubCategoryStatus.
           *
           * @return The value of the enumeration identified by SubCategoryStatus.
           */
         virtual const uci::type::MDF_StatusEnum& getSubCategoryStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubCategoryStatus.
           *
           * @return The value of the enumeration identified by SubCategoryStatus.
           */
         virtual uci::type::MDF_StatusEnum& getSubCategoryStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubCategoryStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubCategoryStatus(const uci::type::MDF_StatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubCategoryStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubCategoryStatus(uci::type::MDF_StatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the File.
           *
           * @return The bounded list identified by File.
           */
         virtual const uci::type::MDF_SubCategoryType::File& getFile() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the File.
           *
           * @return The bounded list identified by File.
           */
         virtual uci::type::MDF_SubCategoryType::File& getFile()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the File.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFile(const uci::type::MDF_SubCategoryType::File& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MDF_SubCategoryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MDF_SubCategoryType to copy from
           */
         MDF_SubCategoryType(const MDF_SubCategoryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MDF_SubCategoryType to the contents of the MDF_SubCategoryType on
           * the right hand side (rhs) of the assignment operator.MDF_SubCategoryType [only available to derived classes]
           *
           * @param rhs The MDF_SubCategoryType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::MDF_SubCategoryType
           * @return A constant reference to this MDF_SubCategoryType.
           */
         const MDF_SubCategoryType& operator=(const MDF_SubCategoryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MDF_SubCategoryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MDF_SubCategoryType_h

