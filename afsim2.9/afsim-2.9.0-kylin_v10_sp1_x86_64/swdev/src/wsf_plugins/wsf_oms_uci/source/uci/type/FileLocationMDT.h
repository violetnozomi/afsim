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
#ifndef Uci__Type__FileLocationMDT_h
#define Uci__Type__FileLocationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FileLocationID_Type_h)
# include "uci/type/FileLocationID_Type.h"
#endif

#if !defined(Uci__Type__FileMetadataID_Type_h)
# include "uci/type/FileMetadataID_Type.h"
#endif

#if !defined(Uci__Type__MediaLocationStatusEnum_h)
# include "uci/type/MediaLocationStatusEnum.h"
#endif

#if !defined(Uci__Type__ProductLocationType_h)
# include "uci/type/ProductLocationType.h"
#endif

#if !defined(Uci__Type__ArchiveRequestType_h)
# include "uci/type/ArchiveRequestType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FileLocationMDT sequence accessor class */
      class FileLocationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FileLocationMDT()
         { }

         /** Returns this accessor's type constant, i.e. FileLocationMDT
           *
           * @return This accessor's type constant, i.e. FileLocationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fileLocationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FileLocationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileLocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileLocationID.
           */
         virtual const uci::type::FileLocationID_Type& getFileLocationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FileLocationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FileLocationID.
           */
         virtual uci::type::FileLocationID_Type& getFileLocationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FileLocationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FileLocationID
           */
         virtual void setFileLocationID(const uci::type::FileLocationID_Type& value)
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


         /** Returns the value of the enumeration that is identified by the FileLocationStatus.
           *
           * @return The value of the enumeration identified by FileLocationStatus.
           */
         virtual const uci::type::MediaLocationStatusEnum& getFileLocationStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FileLocationStatus.
           *
           * @return The value of the enumeration identified by FileLocationStatus.
           */
         virtual uci::type::MediaLocationStatusEnum& getFileLocationStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FileLocationStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFileLocationStatus(const uci::type::MediaLocationStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FileLocationStatus.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFileLocationStatus(uci::type::MediaLocationStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual const uci::type::ProductLocationType& getLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by Location.
           */
         virtual uci::type::ProductLocationType& getLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Location to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Location
           */
         virtual void setLocation(const uci::type::ProductLocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ArchiveConfiguration.
           *
           * @return The acecssor that provides access to the complex content that is identified by ArchiveConfiguration.
           */
         virtual const uci::type::ArchiveRequestType& getArchiveConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ArchiveConfiguration.
           *
           * @return The acecssor that provides access to the complex content that is identified by ArchiveConfiguration.
           */
         virtual uci::type::ArchiveRequestType& getArchiveConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ArchiveConfiguration to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ArchiveConfiguration
           */
         virtual void setArchiveConfiguration(const uci::type::ArchiveRequestType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ArchiveConfiguration exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ArchiveConfiguration is emabled or not
           */
         virtual bool hasArchiveConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ArchiveConfiguration
           *
           * @param type = uci::type::accessorType::archiveRequestType This Accessor's accessor type
           */
         virtual void enableArchiveConfiguration(uci::base::accessorType::AccessorType type = uci::type::accessorType::archiveRequestType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ArchiveConfiguration */
         virtual void clearArchiveConfiguration()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FileLocationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FileLocationMDT to copy from
           */
         FileLocationMDT(const FileLocationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FileLocationMDT to the contents of the FileLocationMDT on the
           * right hand side (rhs) of the assignment operator.FileLocationMDT [only available to derived classes]
           *
           * @param rhs The FileLocationMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::FileLocationMDT
           * @return A constant reference to this FileLocationMDT.
           */
         const FileLocationMDT& operator=(const FileLocationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FileLocationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FileLocationMDT_h

