// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMNETWORK_HPP
#define WSFCOMMNETWORK_HPP

#include <string>
#include <unordered_map>

class UtInput;
#include "UtScriptClassDefine.hpp"
#include "WsfCommAddress.hpp"
#include "WsfObject.hpp"
class WsfSimulation;
#include "wsf_export.h"

//! Developer note:
//! There may be future benefit to providing a script defined network
//! in which the user specifies the logic for addition and removal of
//! network members or links, as well as the update method. In addition,
//! this class would likely benefit and be streamlined by the usage
//! of callbacks for advanced protocol interactions in the future.

namespace wsf
{
namespace comm
{

//! @name Network
//! A container that is a general abstraction of a network.
//! Network object representations only exist in AFSIM as a container
//! to associate individual comms and their addresses together for
//! ease of lookup via the NetworkManager. In addition, this object
//! is extensible and can be instantiated via user input to aid in
//! automatic linking of objects within the network at the time of
//! of simulation instantiation for various topologies or other
//! behaviors that require additional setup.
//! @note This class SHOULD be abstract, but since we use it as a
//! generic type in the script interface, we're forced to allow
//! it to be instantiated. This class should never be used directly,
//! and only as a base for other implementations.
class WSF_EXPORT Network : public WsfObject
{
public:
   struct NamedComm
   {
      WsfStringId mPlatformName{};
      WsfStringId mCommName{};

      NamedComm() = default;
      NamedComm(WsfStringId aPlatformName, WsfStringId aCommName)
         : mPlatformName(aPlatformName)
         , mCommName(aCommName){};

      bool operator==(const NamedComm& aComm) const
      {
         return ((mPlatformName == aComm.mPlatformName) && (mCommName == aComm.mCommName));
      }

      bool operator!=(const NamedComm& aComm) const { return !(*this == aComm); }
   };

   using AddressList     = std::vector<Address>;
   using AddressLinkList = std::vector<std::pair<Address, Address>>;
   using NamedList       = std::vector<NamedComm>;
   using NamedLinkList   = std::vector<std::pair<NamedComm, NamedComm>>;

   //! Need to provide a default constructor for instantiation prior
   //! to address provisioning. Setters need to be called on Network objects
   //! created this way prior to usage.
   Network()           = default;
   ~Network() override = default;
   Network(const std::string& aNetworkName, const Address& aAddress);

   Network(const Network& aSrc) = default;
   Network& operator=(const Network& aRhs) = default;
   Network(Network&& aSrc)                 = default;

   //! @name InitializeLinkage
   //! Required method for creating links for this particular network topology.
   //! This class only converts named comms to addresses. Implementations should
   //! use the protected version for implementing their behavior.
   bool InitializeLinkage(WsfSimulation& aSimulation);

   bool         ProcessInput(UtInput& aInput) override { return false; }
   Network*     Clone() const override { return nullptr; }
   const char*  GetScriptClassName() const override;
   virtual bool Initialize(WsfSimulation& aSim) { return true; }

   //! @name Remove/Add Public Methods
   //! These methods are the interface accessible methods for adding/removing
   //! to the network. These base methods only add/remove to the applicable
   //! container depending on the implementation specific call of the same name.
   //! The optional address parameter is included for passage of an additional
   //! address via the Network interface that may or may not be used on
   //! by various implementations.
   //@{
   bool AddMember(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address());
   bool RemoveMember(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address());
   bool AddLink(const Address& aSourceAddress,
                const Address& aDestinationAddress,
                WsfSimulation& aSimulation,
                const Address& aOptionalAddress = Address());
   bool RemoveLink(const Address& aSourceAddress,
                   const Address& aDestinationAddress,
                   WsfSimulation& aSimulation,
                   const Address& aOptionalAddress = Address());
   //@}

   //! @name Update method
   //! This method is called when a re-evaluation of linkage should occur
   //! within the network topology. This is useful on special network
   //! implementations where linkage isn't solely defined on membership
   //! or edge definitions, i.e. add-hoc networking. The derived member
   //! implementations should define the logic for establishment/removal of
   //! linkage via the protected version of this method.

   void SetAddress(const Address& aAddress) { mAddress = aAddress; }
   void SetIsManaged(bool aManaged) { mManaged = aManaged; }

   const Address& GetAddress() const { return mAddress; }
   size_t         GetHostsMax() const { return mAddress.GetNumHostAddresses(); }
   bool           IsManaged() const { return mManaged; }

   //! @name GetAddressFromNamedComm
   //! Helper method for obtaining an address from a named comm.
   //! This is provided as a public member for conversion of
   //! named platform/comms to the address interface used by the
   //! the network object.
   static Address GetAddressFromNamedComm(WsfStringId aPlatformName, WsfStringId aCommName, WsfSimulation& aSimulation);

   //! @name GetNamedMemberList
   //! This method returns the named member list for this network, as provided
   //! by user input with the network type definition. These members must be
   //! available for external inspection (namely by the network manager) so
   //! that any associated comm interface can have its definition modified to
   //! specify inclusion into this network, otherwise it may be incorrectly
   //! assigned elsewhere (default network). This isn't required for other
   //! member types, as they are static and indicate network membership
   //! directly with the address itself.
   const NamedList& GetNamedMemberList() const { return mNamedList; }

   //! @name HasMember
   //! Checks for existence of address in network. Should only be used
   //! during runtime/after comm framework is initialized.
   bool HasMember(const Address& aAddress) const;

protected:
   virtual bool InitializeLinkageP(WsfSimulation& aSimulation) { return true; }

   //! @name ProcessInput helper methods
   //! These methods parse input based on the particular container
   //! they are applicable to. These are provided for individual usage
   //! as indicated by implementation.
   //@{
   virtual bool ProcessAddressListInput(UtInput& aInput);
   virtual bool ProcessAddressLinkListInput(UtInput& aInput);
   virtual bool ProcessNamedListInput(UtInput& aInput);
   virtual bool ProcessNamedLinkListInput(UtInput& aInput);
   //@}

   //! @name Add/Remove Implementation Behavior Methods
   //! These methods allow implementations to add functionality to the
   //! base add/remove methods.
   //! @note These methods are called after container actions have
   //! been completed (add/remove), in an assumptive "true" state
   //! in the public method call. Thus, the implementation may
   //! undo these changes and return a different value. In addition,
   //! these methods are required for correct establishment of linkage,
   //! as they should notify the network manager to create the necessary
   //! linkage so that implementation details are local.
   //@{
   virtual bool AddMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address())
   {
      return true;
   }
   virtual bool RemoveMemberP(const Address& aAddress,
                              WsfSimulation& aSimulation,
                              const Address& aOptionalAddress = Address())
   {
      return true;
   }
   virtual bool AddLinkP(const Address& aSourceAddress,
                         const Address& aDestinationAddress,
                         WsfSimulation& aSimulation,
                         const Address& aOptionalAddress = Address())
   {
      return true;
   }
   virtual bool RemoveLinkP(const Address& aSourceAddress,
                            const Address& aDestinationAddress,
                            WsfSimulation& aSimulation,
                            const Address& aOptionalAddress = Address())
   {
      return true;
   }
   //@}

   Address     mAddress{};
   AddressList mAddressList{};

   //! Because we use the "mAddressList" member as a container for all address input, we have to use an
   //! additional container to distinguish between addresses converted during object processing,
   //! and those set via user input. This object is only for addresses provided directly by the
   //! user via ProcessInput, or via direct access.
   AddressList     mInputAddressList{};
   AddressLinkList mAddressLinkList{};

   //! Same case as "mInputAddressList" above, except with link definitions.
   AddressLinkList mInputAddressLinkList{};

   //! These member variables only exist to handle ProcessInput parsing of
   //! comms defined by names, since these comms are not instantiated yet,
   //! and do not have addresses.
   NamedList     mNamedList{};
   NamedLinkList mNamedLinkList{};

   //! True if this network is being managed by the simulation.
   bool mManaged{false};
};

//! A base script class with the required interface for
//! network script objects. The script interface prevents from using this class as
//! an abstract base.
class WSF_EXPORT ScriptNetworkClass : public UtScriptClass
{
public:
   ScriptNetworkClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   std::string ToString(void* aObjectPtr) const override;

   UT_DECLARE_SCRIPT_METHOD(GetAddress);
   UT_DECLARE_SCRIPT_METHOD(GetName);
   UT_DECLARE_SCRIPT_METHOD(SetName);
   UT_DECLARE_SCRIPT_METHOD(SetAddress);

   UT_DECLARE_SCRIPT_METHOD(AddMember_1);
   UT_DECLARE_SCRIPT_METHOD(AddMember_2);
   UT_DECLARE_SCRIPT_METHOD(RemoveMember_1);
   UT_DECLARE_SCRIPT_METHOD(RemoveMember_2);
   UT_DECLARE_SCRIPT_METHOD(AddLink_1);
   UT_DECLARE_SCRIPT_METHOD(AddLink_2);
   UT_DECLARE_SCRIPT_METHOD(RemoveLink_1);
   UT_DECLARE_SCRIPT_METHOD(RemoveLink_2);
   UT_DECLARE_SCRIPT_METHOD(GetMembers);

   UT_DECLARE_SCRIPT_METHOD(AddNetwork);
   UT_DECLARE_SCRIPT_METHOD(RemoveNetwork);
   UT_DECLARE_SCRIPT_METHOD(GetNetwork_1);
   UT_DECLARE_SCRIPT_METHOD(GetNetwork_2);
   UT_DECLARE_SCRIPT_METHOD(GetManagedNetworks);

protected:
   //! Typically, network objects are expected to be added to the simulation's
   //! network manager for handling any allocated memory. However, this may
   //! not be ultimately the case, and as such, we would have a memory leak.
   //! The DestroyHelper method will validate the network object has not been
   //! transferred ownership to the network manager before deleting for correct
   //! management of allocated memory of network objects.
   template<typename T>
   static void DestroyHelper(void* aNetworkPtr)
   {
      static_assert(std::is_base_of<wsf::comm::Network, T>::value, "Template type must derive from wsf::comm::Network");
      auto network = static_cast<T*>(aNetworkPtr);
      if (!network->IsManaged())
      {
         delete network;
      }
   }
};

} // namespace comm
} // namespace wsf

UT_MAP_CLASS_TO_SCRIPT_NAME(wsf::comm::Network, "WsfNetwork")

//! Template specialization for wsf::comm::Network::NamedComm. Allows usage of NamedComm in std::unordered_map/set.
namespace std
{
template<>
struct hash<wsf::comm::Network::NamedComm>
{
   size_t operator()(const wsf::comm::Network::NamedComm& aComm) const
   {
      return std::hash<WsfStringId>{}(aComm.mPlatformName) * 3 ^ std::hash<WsfStringId>{}(aComm.mCommName);
   }
};
} // namespace std

#endif
