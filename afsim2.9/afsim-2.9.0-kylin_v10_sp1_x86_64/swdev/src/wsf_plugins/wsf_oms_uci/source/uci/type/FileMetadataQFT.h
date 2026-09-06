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
#ifndef Uci__Type__FileMetadataQFT_h
#define Uci__Type__FileMetadataQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FileMetadataID_Type_h)
# include "uci/type/FileMetadataID_Type.h"
#endif

#if !defined(Uci__Type__FileTypeEnum_h)
# include "uci/type/FileTypeEnum.h"
#endif

#if !defined(Uci__Type__FileFiltersType_h)
# include "uci/type/FileFiltersType.h"
#endif

#if !defined(Uci__Type__SourceFiltersType_h)
# include "uci/type/SourceFiltersType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FileMetadataQFT sequence accessor class */
      class FileMetadataQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~FileMetadataQFT()
         { }

         /** Returns this accessor's type constant, i.e. FileMetadataQFT
           *
           * @return This accessor's type constant, i.e. FileMetadataQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fileMetadataQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FileMetadataQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FileMetadataID_Type, uci::type::accessorType::fileMetadataID_Type> FileMetadataID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 30] */
         typedef uci::base::BoundedList<uci::type::FileTypeEnum, uci::type::accessorType::fileTypeEnum> FileType;

         /** A filter on the classification of the file. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SecurityInformationType, uci::type::accessorType::securityInformationType> FileSecurityInformation;

         /** Returns the value of the SimplePrimitive data type that is identified by the IDsOnly.
           *
           * @return The value of the simple primitive data type identified by IDsOnly.
           */
         virtual xs::Boolean getIDsOnly() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IDsOnly.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIDsOnly(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileMetadataID.
           *
           * @return The bounded list identified by FileMetadataID.
           */
         virtual const uci::type::FileMetadataQFT::FileMetadataID& getFileMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileMetadataID.
           *
           * @return The bounded list identified by FileMetadataID.
           */
         virtual uci::type::FileMetadataQFT::FileMetadataID& getFileMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FileMetadataID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFileMetadataID(const uci::type::FileMetadataQFT::FileMetadataID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileType.
           *
           * @return The bounded list identified by FileType.
           */
         virtual const uci::type::FileMetadataQFT::FileType& getFileType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileType.
           *
           * @return The bounded list identified by FileType.
           */
         virtual uci::type::FileMetadataQFT::FileType& getFileType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FileType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFileType(const uci::type::FileMetadataQFT::FileType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileFilters.
           */
         virtual const uci::type::FileFiltersType& getFileFilters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileFilters.
           */
         virtual uci::type::FileFiltersType& getFileFilters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileFilters to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileFilters
           */
         virtual void setFileFilters(const uci::type::FileFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileFilters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileFilters is emabled or not
           */
         virtual bool hasFileFilters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileFilters
           *
           * @param type = uci::type::accessorType::fileFiltersType This Accessor's accessor type
           */
         virtual void enableFileFilters(uci::base::accessorType::AccessorType type = uci::type::accessorType::fileFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileFilters */
         virtual void clearFileFilters()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceFilters.
           */
         virtual const uci::type::SourceFiltersType& getSourceFilters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SourceFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by SourceFilters.
           */
         virtual uci::type::SourceFiltersType& getSourceFilters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SourceFilters to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SourceFilters
           */
         virtual void setSourceFilters(const uci::type::SourceFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SourceFilters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SourceFilters is emabled or not
           */
         virtual bool hasSourceFilters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SourceFilters
           *
           * @param type = uci::type::accessorType::sourceFiltersType This Accessor's accessor type
           */
         virtual void enableSourceFilters(uci::base::accessorType::AccessorType type = uci::type::accessorType::sourceFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SourceFilters */
         virtual void clearSourceFilters()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConsumerFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConsumerFilters.
           */
         virtual const uci::type::SourceFiltersType& getConsumerFilters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ConsumerFilters.
           *
           * @return The acecssor that provides access to the complex content that is identified by ConsumerFilters.
           */
         virtual uci::type::SourceFiltersType& getConsumerFilters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ConsumerFilters to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ConsumerFilters
           */
         virtual void setConsumerFilters(const uci::type::SourceFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ConsumerFilters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ConsumerFilters is emabled or not
           */
         virtual bool hasConsumerFilters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ConsumerFilters
           *
           * @param type = uci::type::accessorType::sourceFiltersType This Accessor's accessor type
           */
         virtual void enableConsumerFilters(uci::base::accessorType::AccessorType type = uci::type::accessorType::sourceFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ConsumerFilters */
         virtual void clearConsumerFilters()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileSecurityInformation.
           *
           * @return The bounded list identified by FileSecurityInformation.
           */
         virtual const uci::type::FileMetadataQFT::FileSecurityInformation& getFileSecurityInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileSecurityInformation.
           *
           * @return The bounded list identified by FileSecurityInformation.
           */
         virtual uci::type::FileMetadataQFT::FileSecurityInformation& getFileSecurityInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FileSecurityInformation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFileSecurityInformation(const uci::type::FileMetadataQFT::FileSecurityInformation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FileMetadataQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FileMetadataQFT to copy from
           */
         FileMetadataQFT(const FileMetadataQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FileMetadataQFT to the contents of the FileMetadataQFT on the
           * right hand side (rhs) of the assignment operator.FileMetadataQFT [only available to derived classes]
           *
           * @param rhs The FileMetadataQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::FileMetadataQFT
           * @return A constant reference to this FileMetadataQFT.
           */
         const FileMetadataQFT& operator=(const FileMetadataQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FileMetadataQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FileMetadataQFT_h

