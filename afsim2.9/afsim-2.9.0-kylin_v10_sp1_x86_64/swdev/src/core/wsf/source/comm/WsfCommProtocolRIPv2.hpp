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

#ifndef WSFCOMMPROTOCOLRIPV2_HPP
#define WSFCOMMPROTOCOLRIPV2_HPP

#include <cstdint>
#include <sstream>
#include <vector>

#include "UtInput.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfEvent.hpp"

namespace wsf
{
namespace comm
{
namespace router
{
namespace rip
{

class IPv4
{
public:
   constexpr IPv4() noexcept = default;

   constexpr IPv4(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d) noexcept
      : mData{{a, b, c, d}}
   {
   }

   explicit constexpr IPv4(std::uint32_t aVal) noexcept
      : mData{{static_cast<std::uint8_t>((aVal >> 24) & 0xFF),
               static_cast<std::uint8_t>((aVal >> 16) & 0xFF),
               static_cast<std::uint8_t>((aVal >> 8) & 0xFF),
               static_cast<std::uint8_t>(aVal & 0xFF)}}
   {
   }

   std::string to_string() const
   {
      std::stringstream str;
      str << std::to_string(mData[0]) << '.' << std::to_string(mData[1]) << '.' << std::to_string(mData[2]) << '.'
          << std::to_string(mData[3]);
      return str.str();
   }

   constexpr std::uint32_t to_uint32() const noexcept
   {
      return static_cast<std::uint32_t>(mData[0] << 24) | static_cast<std::uint32_t>(mData[1] << 16) |
             static_cast<std::uint32_t>(mData[2] << 8) | static_cast<std::uint32_t>(mData[3]);
   }

   bool operator==(const IPv4& rhs) const noexcept { return mData == rhs.mData; }

   bool operator!=(const IPv4& rhs) const noexcept { return !(*this == rhs); }

   constexpr bool operator<(const IPv4& rhs) const noexcept { return to_uint32() < rhs.to_uint32(); }

   constexpr bool operator>(const IPv4& rhs) const noexcept { return to_uint32() > rhs.to_uint32(); }

private:
   std::array<std::uint8_t, 4> mData{{0, 0, 0, 0}};
};

constexpr IPv4 operator|(const IPv4& lhs, const IPv4& rhs) noexcept
{
   return IPv4{lhs.to_uint32() | rhs.to_uint32()};
}

constexpr IPv4 operator&(const IPv4& lhs, const IPv4& rhs) noexcept
{
   return IPv4{lhs.to_uint32() & rhs.to_uint32()};
}

WSF_EXPORT std::ostream& operator<<(std::ostream& out, const IPv4& ip);

WSF_EXPORT std::istream& operator>>(std::istream& in, IPv4& ip);

constexpr size_t cINFINITY           = 16;
constexpr IPv4   cRIP_MULTICAST_ADDR = IPv4{224, 0, 0, 9};

struct WSF_EXPORT Entry
{
   std::uint16_t addressFamilyIdentifier{2};
   std::uint16_t routeTag{0};
   std::uint32_t address{0};
   std::uint32_t subnetMask{0};
   std::uint32_t nextHop{0};
   std::uint64_t metric{1};
};

enum class Command : std::int8_t
{
   Request  = 0x01,
   Response = 0x02
};

struct WSF_EXPORT Message
{
   Command            command{Command::Request};
   std::uint8_t       version{2};
   std::uint16_t      mustBeZero{0};
   std::vector<Entry> entries{};
};

Message WSF_EXPORT CreateFullTableRequest();
bool WSF_EXPORT    isFullTableRequest(const Message& aMsg);

class WSF_EXPORT RIPv2 : public ProtocolInterface
{
public:
   static constexpr const char* cRIP_DATAGRAM = "RIP_datagram";
   static constexpr const char* cRIP_MESSAGE  = "RIP_MESSAGE";

   RIPv2() = default;
   RIPv2(const RIPv2& aSrc);
   RIPv2(RIPv2&&) = default;
   RIPv2& operator=(RIPv2&&) = default;
   ~RIPv2() override         = default;

   WsfComponent* CloneComponent() const override { return Clone(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;

   RIPv2* Clone() const override { return new RIPv2(*this); }

   void TurnOn(double aSimTime, Router* aRouterPtr);

   bool          ShouldPropagateTruth() const override { return false; }
   graph::Graph* GetGraph() const override { return nullptr; }
   int           GetPriority() const override;

   std::vector<Address> Routing(double                    aSimTime,
                                const Address&            aSendingInterface,
                                size_t&                   aHopNumber,
                                double&                   aCost,
                                const wsf::comm::Message& aMessage) override;

   bool Send(double aSimTime, Router::SendData& aData) override;

   bool Receive(double aSimTime, const Address& aReceivingInterface, wsf::comm::Message& aMessage, bool& aOverrideForward) override;


private:
   void MessageReceived(double                      aSimTime,
                        const Address&              aSrcAddr,
                        const Address&              aReceivingInterface,
                        const router::rip::Message& aRIPMsg);
   void RequestReceived(double                      aSimTime,
                        const Address&              aSrcAddr,
                        const Address&              aReceivingInterface,
                        const router::rip::Message& aRIPMsg);
   void ResponseReceived(double                      aSimTime,
                         const Address&              aSrcAddr,
                         const Address&              aReceivingInterface,
                         const router::rip::Message& aRIPMsg);

   Message GenerateResponse(double aSimTime, const Address& aSrcAddr, bool aFullTable) const;
   Message GenerateFullTableResponse(double aSimTime, const Address& aSrcAddr) const;
   Message GenerateSpecificEntryResponse(double aSimTime, const Address& aSrcAddr, const router::rip::Message& aRIPMsg) const;

   void SendUpdate(double aSimTime, bool aTriggered);
   void SendExpiredNotification(double aSimTime, const IPv4& route) const;
   void SendRIPMessage(double aSimTime, const Address& aSource, const Address& aDest, const Message& aResponse) const;
   std::uint64_t GetCost(const IPv4& aAddress) const;
   bool          HasUpdates() const;
   void          ScheduleEvent(std::unique_ptr<WsfEvent> aEvent) const;
   double        GetRandomValue(double min, double max) const;

   // These items are for the type definition:
   double mUpdateTimeOut{30.0};
   double mInvalidationTimeout{180.0};
   double mGarbageCollectionTimeOut{120.0};
   bool   mPoisonedReverse{true};

   struct IPEntry
   {
      IPv4          subnetMask{255, 255, 255, 0};
      IPv4          gateway{0};
      std::uint64_t metric{router::rip::cINFINITY};
      double        lastSeen{0.0};
      bool          staticRoute{false};
      bool          valid{true};
      bool          changed{false};
   };

   // These items are for the instances of the protocol
   std::map<IPv4, IPEntry> mTable{};
   double                  mLastTimeOut{std::numeric_limits<double>::max()};
   bool                    mTriggeredUpdatedPending{false};
   std::shared_ptr<int>    mContext{nullptr};

   // Callback for router activation
   UtCallbackHolder mCallbacks{};
};

} // namespace rip
} // namespace router
} // namespace comm
} // namespace wsf

#endif
