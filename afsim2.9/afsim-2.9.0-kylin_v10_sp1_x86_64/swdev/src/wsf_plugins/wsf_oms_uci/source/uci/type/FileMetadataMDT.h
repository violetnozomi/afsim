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
#ifndef Uci__Type__FileMetadataMDT_h
#define Uci__Type__FileMetadataMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FileMetadataID_Type_h)
# include "uci/type/FileMetadataID_Type.h"
#endif

#if !defined(Uci__Type__FileOutputType_h)
# include "uci/type/FileOutputType.h"
#endif

#if !defined(Uci__Type__FileNameType_h)
# include "uci/type/FileNameType.h"
#endif

#if !defined(Uci__Type__SourceFiltersType_h)
# include "uci/type/SourceFiltersType.h"
#endif

#if !defined(Uci__Type__CreationSourceEnum_h)
# include "uci/type/CreationSourceEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__SHA_2_256_HashType_h)
# include "uci/type/SHA_2_256_HashType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FileMetadataMDT sequence accessor class */
      class FileMetadataMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FileMetadataMDT()
         { }

         /** Returns this accessor's type constant, i.e. FileMetadataMDT
           *
           * @return This accessor's type constant, i.e. FileMetadataMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fileMetadataMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FileMetadataMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileMetadataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileMetadataID.
           */
         virtual const uci::type::FileMetadataID_Type& getFileMetadataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileMetadataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileMetadataID.
           */
         virtual uci::type::FileMetadataID_Type& getFileMetadataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileMetadataID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileMetadataID
           */
         virtual void setFileMetadataID(const uci::type::FileMetadataID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileDescription.
           */
         virtual const uci::type::FileOutputType& getFileDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileDescription.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileDescription.
           */
         virtual uci::type::FileOutputType& getFileDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileDescription to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileDescription
           */
         virtual void setFileDescription(const uci::type::FileOutputType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileName.
           *
           * @return The value of the string data type identified by FileName.
           */
         virtual const uci::type::FileNameType& getFileName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileName.
           *
           * @return The value of the string data type identified by FileName.
           */
         virtual uci::type::FileNameType& getFileName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileName to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileName(const uci::type::FileNameType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileSource.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileSource.
           */
         virtual const uci::type::SourceFiltersType& getFileSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileSource.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileSource.
           */
         virtual uci::type::SourceFiltersType& getFileSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileSource to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileSource
           */
         virtual void setFileSource(const uci::type::SourceFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileConsumer.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileConsumer.
           */
         virtual const uci::type::SourceFiltersType& getFileConsumer() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileConsumer.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileConsumer.
           */
         virtual uci::type::SourceFiltersType& getFileConsumer()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileConsumer to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileConsumer
           */
         virtual void setFileConsumer(const uci::type::SourceFiltersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileConsumer exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileConsumer is emabled or not
           */
         virtual bool hasFileConsumer() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileConsumer
           *
           * @param type = uci::type::accessorType::sourceFiltersType This Accessor's accessor type
           */
         virtual void enableFileConsumer(uci::base::accessorType::AccessorType type = uci::type::accessorType::sourceFiltersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileConsumer */
         virtual void clearFileConsumer()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CreationSource.
           *
           * @return The value of the enumeration identified by CreationSource.
           */
         virtual const uci::type::CreationSourceEnum& getCreationSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CreationSource.
           *
           * @return The value of the enumeration identified by CreationSource.
           */
         virtual uci::type::CreationSourceEnum& getCreationSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CreationSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCreationSource(const uci::type::CreationSourceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CreationSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCreationSource(uci::type::CreationSourceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UntrustedModification.
           *
           * @return The value of the simple primitive data type identified by UntrustedModification.
           */
         virtual xs::Boolean getUntrustedModification() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UntrustedModification.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUntrustedModification(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @return The value of the simple primitive data type identified by Timestamp.
           */
         virtual uci::type::DateTimeTypeValue getTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityInformation.
           */
         virtual const uci::type::SecurityInformationType& getSecurityInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SecurityInformation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SecurityInformation.
           */
         virtual uci::type::SecurityInformationType& getSecurityInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SecurityInformation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SecurityInformation
           */
         virtual void setSecurityInformation(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor to the SHA_2_Hash field contained in this message fragment
           *
           * @return The reference to the accessor to the SHA_2_Hash field.
           */
         virtual const uci::type::SHA_2_256_HashType& getSHA_2_Hash() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor to the SHA_2_Hash field contained in this message fragment
           *
           * @return The reference to the accessor to the SHA_2_Hash field.
           */
         virtual uci::type::SHA_2_256_HashType& getSHA_2_Hash()
            throw(uci::base::UCIException) = 0;


         /** Set the contents of the message fragment that this accesssor provides access to to the contents the specified
           * accessor provides access to. A deep copy will be used by this method in setting the contents
           *
           * @param value The accessor to use to set the contents of the message fragment that this accessor provides access to.
           */
         virtual void setSHA_2_Hash(const uci::type::SHA_2_256_HashType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SHA_2_Hashis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSHA_2_Hash() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSHA_2_Hash will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::sHA_2_256_HashType This Accessor's accessor type
           */
         virtual void enableSHA_2_Hash(uci::base::accessorType::AccessorType type = uci::type::accessorType::sHA_2_256_HashType)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSHA_2_Hash()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FileMetadataMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FileMetadataMDT to copy from
           */
         FileMetadataMDT(const FileMetadataMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FileMetadataMDT to the contents of the FileMetadataMDT on the
           * right hand side (rhs) of the assignment operator.FileMetadataMDT [only available to derived classes]
           *
           * @param rhs The FileMetadataMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::FileMetadataMDT
           * @return A constant reference to this FileMetadataMDT.
           */
         const FileMetadataMDT& operator=(const FileMetadataMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FileMetadataMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FileMetadataMDT_h

