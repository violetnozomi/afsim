// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Base__UUID_h
#define Uci__Base__UUID_h 1

#if !defined(SysLibStdint_h)
#  include <stdint.h>
//  @cond
#  define SysLibStdint_h 1
//  @endcond
#endif


#if !defined(StdLibVector)
#  include <vector>
//  @cond
#  define StdLibVector 1
//  @endcond
#endif

#if !defined(StdLibCstddef)
#  include <cstddef>
//  @cond
#  define StdLibCstddef 1
//  @endcond
#endif

#if !defined(StdLibAlgorithm)
#  include <algorithm>
//  @cond
#  define StdLibAlgorithm 1
//  @endcond
#endif

#if !defined(StdLibSstream)
#  include <sstream>
//  @cond
#  define StdLibSstream 1
//  @endcond
#endif


#if !defined(Uci__Base__UCIException_h)
# include "uci/base/UCIException.h"
#endif

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all base data types are declared
   namespace base {

      /** UUID is the base class of all UUID classes that are used to specify a unique ID.
        *
        * A UUID is an 128-bit identifier that can be used to uniquely identify anything. From the UUIDspecification (IETF
        * rsf4122.txt):
        *
        *    One of the main reasons for using UUIDs is that no centralized authority is required to administer them (although
        *    one format usesvIEEE 802 node identifiers, others do not). As a result, generationvon demand can be completely
        *    automated, and used for a variety of purposes. The UUID generation algorithm described here supports very high
        *    allocation rates of up to 10 million per second per machine ifvnecessary, so that they could even be used as
        *    transaction IDs.
        *
        *    UUIDs are of a fixed size (128 bits) which is reasonably small compared to other alternatives. This lends itself
        *    well to sorting,vordering, and hashing of all sorts, storing in databases, simple allocation, and ease of
        *    programming in general.
        *
        *    Since UUIDs are unique and persistent, they make excellent Uniform Resource Names. The unique ability to generate
        *    a new UUID without a registration process allows for UUIDs to be one of the URNs with the lowest minting cost.
        *
        * The UUID specification supports a number of different versions of the UUID. The two supported by this class include
        * the "random number" version and the "time-based" version.
        *
        * For the "time-based" UUID, the format of the stringified version of the uuid is as follows:
        *
        *
        *    UUID                   = time-low "-" time-mid "-" time-high-and-version "-" clock-seq-and-reserved clock-seq-low "-" node
        *    time-low               = 4hexOctet
        *    time-mid               = 2hexOctet
        *    time-high-and-version  = 2hexOctet
        *    clock-seq-and-reserved = hexOctet
        *    clock-seq-low          = hexOctet
        *    node                   = 6hexOctet
        *    hexOctet               = hexDigit hexDigit
        *    hexDigit               = "0" / "1" / "2" / "3" / "4" / "5" / "6" / "7" / "8" / "9"
        *                             "a" / "b" / "c" / "d" / "e" / "f"
        *                             "A" / "B" / "C" / "D" / "E" / "F"
        *
        * Note that the UUID does not provide any publicly visible constructors. The reason for this is that UUIDs must be
        * constructed by the derived UUID classes since they are responsible for managing all aspects of the UUID's existence
        * which goes beyond what is declared here. Applications can use the CAL's generateUUID() method to generate new UUIDs
        * as required.
        */
      class UUID  : public virtual uci::base::Accessor {
      public:



         // === Common Typedefs And Constants ===================================================================================

         /** The number of octets that are containted in a uuid. */
         static const size_t m_numberOfOctets = 16;

         /** The Octet is the data type used to hold two hexidecimal digits, each represented using 4 bits.The Octet is declared
           * to be of a single atomic type as oppposed to a container of types (array, vector, etc) so that it's usage better
           * aligns with the UUID specification. The most significant hexidecimal digit (4 bits) is stored in the most significant
           * 4 bits of the Octet while the least significant hexidecimal digit (4 bits) is stored in the least signficant 4 bits
           * of the Octet.
           */
         typedef uint8_t Octet;

         /** Octets is a vector containing a set of octets. This vector can be used to set and get the value of a uuid. The size
           * of a valid Octets vector shall be m_numberOfOctets. The octets are ordered in the vector in network ordering, i.e.
           * big endian ordering, such that the most significant octet is found in the first element of the vector, i.e.
           * octets[0], and the least significant octet is found in the last element of the vector, i.e. octets[m_numberOfOctets -
           * 1].
           */
         typedef std::vector<Octet> Octets;

         /** The node address is used to uniquely identify the processing node on a network of processing nodes that created the
           * uuid. In many cases, this is the MAC address of the device used by the processor that created this uuid. The size of
           * a valid Octets vector shall be m_sizeOfNodeAddress. The data is ordered in the vector in network ordering, i.e. big
           * endian ordering, such that the most significant unsigned 8-bit integer is found in the first element of the vector,
           * i.e. nodeAddress[0], and the least significant unsigned 8-bit integer is found in the last element of the vector,
           * i.e. nodeAddress[m_sizeOfNodeAddress - 1].
           */
         typedef std::vector<uint8_t> NodeAddress;

         /** The timestamp is used to record the current time. The bytes contained within the timestamp will be ordered in network
           * ordering, i.e. big endian ordering.
           */
         typedef uint64_t Timestamp;

         /** The clock sequence is the additional sequence that is combined with the current timestamp that is contained in
           * time-based uuid's. The bytes contained within the timestamp will be ordered in network ordering, i.e. big endian
           * ordering.
           */
         typedef uint16_t ClockSequence;


         /** The enumeration of the UUID's variants. A UUID's variant specifies the layout of the octets contains in the UUID's
           * data array.
           */
         enum Variant {
            /** NCS backward compatibility */
            variantNCS,

            /** Microsoft Corporation backward compatibility */
            variantMicrosoft,

            /** The currently supported variant type. All OMS UUIDs will follow this format. */
            variantCurrent,

            /** Future variant specification */
            variantFuture,

            /** This enumerated value is used to identify that the UUID is a variant of an unknown type, possibly indicating a
              * corruption of the UUID.
              */
            variantUnknown

         };


         /** The enumeration of the UUID versions. The version specifies how the UUID's bytes were generated. */
         enum Version {
            /** UUIDs of this version have the values generated following the specification laid out in the IETF UUID specification.
              * Basically the UUID is generated by combining the current time with the processor's ethernet hardware address, i.e.
              * it's MAC address.
              */
            versionTimeBased,

            /** UUIDs of this version have the values generated following the DCE securoty specification with embedded POSIX UIDs. */
            versionDceSecurity,

            /** UUIDs of this version have the values generated using hash values generated by the MD5 algorithm applied to some set
              * of names.
              */
            versionNameBasedMD5,

            /** UUIDs of this version have the values generated using some random or pseuod random generator. This version is also
              * specified in the IETF UUID specification.
              */
            versionRandomNumber,

            /** UUIDs of this version have the values generated using hash values generated by the SHA1 algorithm applied to some set
              * of names.
              */
            versionNameBasedSHA1,

            /** This enumerated value is used to identify that the UUID was generated by an unknown process, possibly indicating a
              * corruption of the UUID.
              */
            versionUnknown

         };


         /** The ValueUUID contains the value of a UUID expressed as two 64-bit values. Both values are stored in network
           * ordering, i.e. big endian ordering
           */
         struct ValueUUID {

            /** The most signficant bits of the uuid. This bytes in this value are stored in network ordering, i.e. big endian
              * ordering.
              */
            uint64_t mostSignificantBits;

            /** The least signficant bits of the uuid. This bytes in this value are stored in network ordering, i.e. big endian
              * ordering.
              */
            uint64_t leastSignificantBits;

         };





         // === Destructor ======================================================================================================

         /** The destructor */
         virtual ~UUID()
         { }

         /** Returns this accessor's type constant, i.e. uuid
           *
           * @return This accessor's type constant, i.e. uuid
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::base::accessorType::uuid;
         }




         // === SetValue methods ================================================================================================

         /** Sets the value of the universally unique identifier that is accessed by this UUID from a stringified representation
           * of a universally unique identifier. The stringified representation of a universally unique identifier follows the
           * format stated above.
           *
           * @param stringifiedUUID The stringified UUID used to set the universally unique identifier that is accessed by this
           *      UUID
           */
         virtual void setValue(const std::string& stringifiedUUID)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the universally unique identifier that is accessed by this UUID from a valueUUID.
           *
           * @param value The valueUUID used to set the universally unique identifier that is accessed by this UUID
           */
         virtual void setValue(const ValueUUID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the universally unique identifier that is accessed by this UUID from a vector of octets.
           *
           * @param octets The vector of octets that are used to initialize the universally unique identifier that is accessed by
           *      this UUID
           */
         virtual void setValue(const Octets& octets)
            throw(uci::base::UCIException) = 0;


         /** Resets the value of the universally unique identifier that is accessed by this UUID to nil, i. e. one whose value is
           * 0.
           */
         virtual void resetValue()
            throw(uci::base::UCIException) = 0;




         // === GetValue methods ================================================================================================

         /** Returns the value of the universal unique identifier that this UUID provides access to as a vector of octets.
           *
           * @return The vector of octets
           */
         virtual const Octets getValueAsOctets() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the universally unique identifier that is accessed by this UUID as a ValueUUID, i.e. 2 64-bit
           * values.
           *
           * @return The ValueUUID, i.e. 2 64-bit values
           */
         virtual const ValueUUID getValueAsValueUUID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the universally unique identifier that is accessed by this UUID as a string. The format of this
           * string is specified above.
           *
           * @return The stringified universally unique identifier
           */
         const std::string getValueAsString() const
            throw(uci::base::UCIException)
         {
            Octets octets = getValueAsOctets();
            std::string stringifiedUUID;
            int hexidecimalCount = 0;
            for (Octets::const_iterator iter = octets.begin(); iter < octets.end(); ++iter) {
               stringifiedUUID += toChar(((*iter) >> 4) & 0xF);
               stringifiedUUID += toChar((*iter) & 0xF);

               hexidecimalCount += 2;
               if ((hexidecimalCount == 8) || (hexidecimalCount == 12) || (hexidecimalCount == 16) || (hexidecimalCount == 20)) {
                  stringifiedUUID += '-';
               }


            }


            return stringifiedUUID;
         }




         // === Class Information ===============================================================================================

         /** Returns this UUIDs variant.
           *
           * @return This UUID's variant
           */
         Variant getVariant() const
            throw(uci::base::UCIException)
         {
            //  According to the specification, the variant is in high order bits of octet 8.
            Octet octet = getValueAsOctets()[8];

            //  Note: the specification states that the variant is stored in the most significant bits of octet 8.

            //  Test for NCS variant: variant = 0xxx
            if ((octet & 0x80) == 0x00) {
               return variantNCS;
            }

            //  Test for current variant: variant = 10xx
            if ((octet & 0xC0) == 0x80) {
               return variantCurrent;
            }

            //  Test for microsoft variant: variant = 110x
            if ((octet & 0xE0) == 0xC0) {
               return variantMicrosoft;
            }

            //  Test for future variant: variant = 111x
            if ((octet & 0xE0) == 0xE0) {
               return variantFuture;
            }

            return variantUnknown;;
         }


         /** Returns this UUIDs version.
           *
           * @return This UUID's version
           */
         Version getVersion() const
            throw(uci::base::UCIException)
         {
            //  According to the specification, the variant is in high order bits of octet 6.
            Octet octet = getValueAsOctets()[6];
            //  Test for time based version
            if ((octet & 0xF0) == 0x10) {
               return versionTimeBased;
            }

            //  Test for DCE security version
            if ((octet & 0xF0) == 0x20) {
               return versionDceSecurity;
            }

            //  Test for name based MD5 version
            if ((octet & 0xF0) == 0x30) {
               return versionNameBasedMD5;
            }

            //  Test for random number version
            if ((octet & 0xF0) == 0x40) {
               return versionRandomNumber;
            }

            //  Test for name based SHA1 version
            if ((octet & 0xF0) == 0x50) {
               return versionNameBasedSHA1;
            }

            return versionUnknown;;
         }


         /** Returns the the clock sequence of the uuid that is accessed by this UUID
           *
           * @return The clock sequence portion of this UUID's data
           */
         virtual ClockSequence getClockSequence() const
            throw(uci::base::UCIException) = 0;


         /** Returns the timestamp portion of this UUID's data. This method is only valid with time based UUIDs. Invocation of
           * this method with other versions of UUIDs will result in an exception being thrown. The value that is returned in
           * always in big endian ordering regardless of the CPU's ordering. When running on little endian machines, one should
           * swap bytes before using the returned value
           *
           * @return The timestamp portion of this UUID's data
           */
         virtual Timestamp getTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Returns the node address portion of this UUID's data. This method is only valid with time based UUIDs. Invocation of
           * this method with other versions of UUIDs will result in an exception being thrown.
           *
           * @return The timestamp portion of this UUID's data
           */
         virtual const NodeAddress getNodeAddress() const
            throw(uci::base::UCIException) = 0;




         // === Comparison Operators ============================================================================================

         /** Tests whether the value of this UUID is equal to the value of the specified UUID returning true if they are equal,
           * false otherwise.
           *
           * @param rhs The UUID on the right hand side of the == operator to compare against
           * @return True if this UUID equals the rhs UUID, false otherwise
           */
         bool operator==(const UUID& rhs) const
         {
            return (rhs.getValueAsValueUUID().mostSignificantBits == getValueAsValueUUID().mostSignificantBits) && (rhs.getValueAsValueUUID().leastSignificantBits == getValueAsValueUUID().leastSignificantBits);
         }


         /** Tests whether the value of this UUID is not equal to the value of the specified UUID returning true if they are not
           * equal, false otherwise.
           *
           * @param rhs The UUID on the right hand side of the != operator to compare against
           * @return True if this UUID is not equal to the rhs UUID, false otherwise
           */
         bool operator!=(const UUID& rhs) const
         {
            return (rhs.getValueAsValueUUID().mostSignificantBits != getValueAsValueUUID().mostSignificantBits) || (rhs.getValueAsValueUUID().leastSignificantBits != getValueAsValueUUID().leastSignificantBits);
         }


         /** Tests whether the value of the uuid that is accessed by this UUID is less than the value of the uuid that is accessed
           * by the specified UUID (rhs) returning true if the value of the uuid accessed by this UUID is less than the value of
           * the uuid accessed by the specified UUID (rhs), false otherwise. The comparison is performed on a hexidecimal digit by
           * hexidecimal digit basis starting with the most significant hexidecimal digit.
           *
           * @param rhs The UUID on the right hand side of the < operator to compare against
           * @return True if this UUID is less than the the rhs UUID, false otherwise
           */
         bool operator<(const UUID& rhs) const
         {
            Octets octets = getValueAsOctets();
            Octets rhsOctets = rhs.getValueAsOctets();
            return std::lexicographical_compare(octets.begin(), octets.end(), rhsOctets.begin(), rhsOctets.end());
         }


         /** Tests whether the value the uuid accessed by this UUID is greater than the value of the uuid accessed by the
           * specified UUID (rhs) returning true if the value of the uuid accessed by this UUID is greater than the value of the
           * uuid accessed by the specified UUID (rhs), false otherwise. The comparison is performed on a hexidecimal digit by
           * hexidecimal digit basis starting with the most significant hexidecimal digit.
           *
           * @param rhs The UUID on the right hand side of the > operator to compare against
           * @return True if this UUID is greater than the the rhs UUID, false otherwise
           */
         bool operator>(const UUID& rhs) const
         {
            Octets octets = getValueAsOctets();
            Octets rhsOctets = rhs.getValueAsOctets();
            return std::lexicographical_compare(rhsOctets.begin(), rhsOctets.end(), octets.begin(), octets.end());
         }


         /** Tests whether the value the uuid accessed by this UUID is less than or equal to the value of the uuid accessed by the
           * specified UUID (rhs) returning true if the value of the uuid accessed by this UUID is less than or equal to the value
           * of the uuid accessed by the specified UUID (rhs), false otherwise. The comparison is performed on a hexidecimal digit
           * by hexidecimal digit basis starting with the most significant hexidecimal digit.
           *
           * @param rhs The UUID on the right hand side of the <= operator to compare against
           * @return True if this UUID is less than or equal to the the rhs UUID, false otherwise
           */
         bool operator<=(const UUID& rhs) const
         {
            Octets octets = getValueAsOctets();
            Octets rhsOctets = rhs.getValueAsOctets();
            return !std::lexicographical_compare(rhsOctets.begin(), rhsOctets.end(), octets.begin(), octets.end());
         }


         /** Tests whether the value the uuid accessed by this UUID is greater than or equal to the value of the uuid accessed by
           * the specified UUID (rhs) returning true if the value of the uuid accessed by this UUID is greater than or equal to
           * the value of the uuid accessed by the specified UUID (rhs), false otherwise. The comparison is performed on a
           * hexidecimal digit by hexidecimal digit basis starting with the most significant hexidecimal digit.
           *
           * @param rhs The UUID on the right hand side of the >= operator to compare against
           * @return True if this UUID is greater than or equal to the the rhs UUID, false otherwise
           */
         bool operator>=(const UUID& rhs) const
         {
            Octets octets = getValueAsOctets();
            Octets rhsOctets = rhs.getValueAsOctets();
            return !std::lexicographical_compare(octets.begin(), octets.end(), rhsOctets.begin(), rhsOctets.end());
         }




         // === Helper Methods ==================================================================================================

         /** Returns whether this UUID is nil (true) or not (false). A UUID is considered NIL if every element in its data set is
           * 0
           *
           * @return True if the UUID is NIL, false otherwise
           */
         bool isNil() const
            throw(uci::base::UCIException)
         {
            return (getValueAsValueUUID().mostSignificantBits == 0) && (getValueAsValueUUID().leastSignificantBits == 0);
         }


         /** This method converts the specified variant into it stringified form that is useful for displaying information.
           *
           * @param variant The variant to convert
           * @return The stringified version of the specified variant
           */
         static const char* toString(Variant variant)
         {
            if (variant == variantNCS) {
               return "VariantNCS";
            }

            if (variant == variantMicrosoft) {
               return "VariantMicrosoft";
            }

            if (variant == variantCurrent) {
               return "VariantCurrent";
            }

            if (variant == variantFuture) {
               return "VariantFuture";
            }

            return "VariantUnknown";
         }


         /** This method converts the specified version into it stringified form that is useful for displaying information.
           *
           * @param version The version to convert
           * @return The stringified version of the specified version
           */
         static const char* toString(Version version)
         {
            if (version == versionTimeBased) {
               return "VersionTimeBased";
            }

            if (version == versionDceSecurity) {
               return "VersionDceSecurity";
            }

            if (version == versionNameBasedMD5) {
               return "VersionNameBasedMD5";
            }

            if (version == versionRandomNumber) {
               return "VersionRandomNumber";
            }

            if (version == versionNameBasedSHA1) {
               return "VersionNameBasedSHA1";
            }

            return "VersionUnknown";
         }



      protected:

         /** This method converts the specified version into it stringified form that is useful for displaying information.
           *
           * @param value The value to convert
           * @return The converted hexidecimal character
           */
         static char toChar(uint8_t value)
         {
            static char const chars[] = { '0', '1', '2', '3' , '4', '5', '6' , '7', '8' , '9', 'A', 'B', 'C', 'D', 'E', 'F' };
            return chars[value];
         }


         /** The constructor */
         UUID()
         { }

         /** The copy constructor
           *
           * @param rhs The UUID to copy from
           */
         UUID(const UUID& rhs)
         {
            (void)rhs;
         }


         /** The assignment operator. Sets the contents of this UUID to the contents of the UUID on the right hand side (rhs) of
           * the assignment operator.UUID
           *
           * @param rhs The UUID on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this UUID
           * @return A constant reference to this UUID.
           */
         const UUID& operator=(const UUID& rhs)
         {
            (void)rhs;
            return *this;
         }


      }; // UUID


   } // Namespace: base
} // Namespace: uci

#endif // Uci__Base__UUID_h

