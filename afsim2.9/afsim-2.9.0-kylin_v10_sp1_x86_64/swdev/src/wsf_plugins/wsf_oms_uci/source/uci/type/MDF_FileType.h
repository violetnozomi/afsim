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
#ifndef Uci__Type__MDF_FileType_h
#define Uci__Type__MDF_FileType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MDF_SubCategoryFileID_Type_h)
# include "uci/type/MDF_SubCategoryFileID_Type.h"
#endif

#if !defined(Uci__Type__MDF_StatusEnum_h)
# include "uci/type/MDF_StatusEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MDF_FileType sequence accessor class */
      class MDF_FileType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MDF_FileType()
         { }

         /** Returns this accessor's type constant, i.e. MDF_FileType
           *
           * @return This accessor's type constant, i.e. MDF_FileType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mDF_FileType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MDF_FileType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileID.
           */
         virtual const uci::type::MDF_SubCategoryFileID_Type& getFileID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileID.
           */
         virtual uci::type::MDF_SubCategoryFileID_Type& getFileID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileID
           */
         virtual void setFileID(const uci::type::MDF_SubCategoryFileID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FileStatus.
           *
           * @return The value of the enumeration identified by FileStatus.
           */
         virtual const uci::type::MDF_StatusEnum& getFileStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FileStatus.
           *
           * @return The value of the enumeration identified by FileStatus.
           */
         virtual uci::type::MDF_StatusEnum& getFileStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FileStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFileStatus(const uci::type::MDF_StatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FileStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFileStatus(uci::type::MDF_StatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MDF_FileType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MDF_FileType to copy from
           */
         MDF_FileType(const MDF_FileType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MDF_FileType to the contents of the MDF_FileType on the right hand
           * side (rhs) of the assignment operator.MDF_FileType [only available to derived classes]
           *
           * @param rhs The MDF_FileType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::MDF_FileType
           * @return A constant reference to this MDF_FileType.
           */
         const MDF_FileType& operator=(const MDF_FileType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MDF_FileType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MDF_FileType_h

