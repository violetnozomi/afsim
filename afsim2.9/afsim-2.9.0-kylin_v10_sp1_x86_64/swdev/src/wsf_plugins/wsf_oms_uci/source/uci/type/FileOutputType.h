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
#ifndef Uci__Type__FileOutputType_h
#define Uci__Type__FileOutputType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FileTypeEnum_h)
# include "uci/type/FileTypeEnum.h"
#endif

#if !defined(Uci__Type__FileFormatType_h)
# include "uci/type/FileFormatType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FileOutputType sequence accessor class */
      class FileOutputType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FileOutputType()
         { }

         /** Returns this accessor's type constant, i.e. FileOutputType
           *
           * @return This accessor's type constant, i.e. FileOutputType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fileOutputType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FileOutputType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the content of the File. Multiple instances are allowed to support Files with multiple types of content.
           * See enumeration annotations for further details. [Maximum occurrences: 30]
           */
         typedef uci::base::BoundedList<uci::type::FileTypeEnum, uci::type::accessorType::fileTypeEnum> FileType;

         /** Returns the bounded list that is identified by the FileType.
           *
           * @return The bounded list identified by FileType.
           */
         virtual const uci::type::FileOutputType::FileType& getFileType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileType.
           *
           * @return The bounded list identified by FileType.
           */
         virtual uci::type::FileOutputType::FileType& getFileType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FileType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFileType(const uci::type::FileOutputType::FileType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileFormat.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileFormat.
           */
         virtual const uci::type::FileFormatType& getFileFormat() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileFormat.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileFormat.
           */
         virtual uci::type::FileFormatType& getFileFormat()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileFormat to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileFormat
           */
         virtual void setFileFormat(const uci::type::FileFormatType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FileOutputType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FileOutputType to copy from
           */
         FileOutputType(const FileOutputType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FileOutputType to the contents of the FileOutputType on the right
           * hand side (rhs) of the assignment operator.FileOutputType [only available to derived classes]
           *
           * @param rhs The FileOutputType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::FileOutputType
           * @return A constant reference to this FileOutputType.
           */
         const FileOutputType& operator=(const FileOutputType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FileOutputType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FileOutputType_h

