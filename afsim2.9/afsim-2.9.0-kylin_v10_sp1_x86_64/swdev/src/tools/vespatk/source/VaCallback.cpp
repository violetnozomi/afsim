// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "VaCallback.hpp"

#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>

#include "UtCalendar.hpp"
#include "UtLog.hpp"
#include "UtWallClock.hpp"

namespace vespa
{
VaCallbackGroupId VaCallback::sCurrentGroupId;

VaCallback::VaCallback()
   : VaUniqueId()
   , mCallbackLinkPtr(nullptr)
   , mGroupId()
   , mIsBlocked(false)
   , mDisconnectFlag(false)
   , mDeleteFlag(false)
{
}

VaCallback::VaCallback(const VaCallback& aSrc)
   : VaUniqueId(aSrc, true)
   , mCallbackLinkPtr(nullptr)
   , mGroupId()
   , mIsBlocked(false)
   , mDisconnectFlag(false)
   , mDeleteFlag(false)
{
}

VaCallback::~VaCallback()
{
   mDeleteFlag = false;
   Disconnect();
}

void VaCallback::Disconnect()
{
   if (mCallbackLinkPtr != nullptr)
   {
      mCallbackLinkPtr->Disconnect(this);
   }
   else if (mDeleteFlag)
   {
      delete this;
   }
}

const int                         VaCallbackList::cMAX_RECURSION = 16;
int                               VaCallbackList::sInstanceCount = 0;
bool                              VaCallbackList::sEnableProfile = false;
VaCallbackList::ProfileQueueStack VaCallbackList::sProfileStack;
VaCallbackList::ProfileNode*      VaCallbackList::sProfileTreePtr  = nullptr;
std::string                       VaCallbackList::sProfileFileName = "vespa-profile-{date}.xml";
std::string                       VaCallbackList::sProfileDateTime = "";

VaCallbackList::VaCallbackList()
   : VaObject()
   , mCallbackList()
   , mCallbackGroupId(0)
   , mCallbackDepth(0)
{
   ++sInstanceCount;
}

VaCallbackList::VaCallbackList(const std::string& aType, const std::string& aName)
   : VaObject()
   , mCallbackList()
   , mCallbackGroupId(0)
   , mCallbackDepth(0)
{
   ++sInstanceCount;

   SetCallbackType(aType);
   SetName(aName);
}

VaCallbackList::VaCallbackList(const VaCallbackList& aSrc)
   : VaObject(aSrc)
   , mCallbackList()
   , mCallbackGroupId(0)
   , mCallbackDepth(0)
{
}

VaCallbackList::~VaCallbackList()
{
   --sInstanceCount;
   if (sInstanceCount <= 0)
   {
      CleanUpProfile();
   }
   DisconnectAll();
}

VaCallbackList& VaCallbackList::operator[](VaCallbackGroupId aCallbackGroupId)
{
   mCallbackGroupId = aCallbackGroupId;
   return *this;
}

//! Disconnect the indicated callback from the callback list.
//! Note: The callback object (pointed to by aCallbackPtr) is NOT deleted. It can be
//! reattached to another callback list.
//! @param aCallbackPtr The callback to be disconnected.
void VaCallbackList::Disconnect(VaCallback* aCallbackPtr)
{
   if (mCallbackDepth == 0)
   {
      bool found = (EraseCallback(mCallbackList, aCallbackPtr) || EraseCallback(mBlockedCallbackList, aCallbackPtr));

      if (found)
      {
         // the subscriber it is no longer connected.
         delete aCallbackPtr->mCallbackLinkPtr;
         aCallbackPtr->mCallbackLinkPtr = nullptr;
      }

      // If we specified that the callback should be deleted, delete it.
      // The destructor sets the delete flag to false to avoid a recursive delete.
      if (aCallbackPtr->GetDeleteFlag())
      {
         delete aCallbackPtr;
      }
   }
   else
   {
      aCallbackPtr->SetDisconnectFlag(true);
   }
}

//! Disconnect all callbacks.
//! @note The callback objects are not deleted.
void VaCallbackList::DisconnectAll()
{
   // Tell each connected subscriber that they have been disconnected.
   MapType::iterator it;
   for (it = mCallbackList.begin(); it != mCallbackList.end(); ++it)
   {
      std::set<VaCallback*>::iterator jt;
      for (jt = it->second.begin(); jt != it->second.end(); ++jt)
      {
         delete (*jt)->mCallbackLinkPtr;
         (*jt)->mCallbackLinkPtr = nullptr;
      }
      it->second.clear();
   }
   mCallbackList.clear();
   for (it = mBlockedCallbackList.begin(); it != mBlockedCallbackList.end(); ++it)
   {
      std::set<VaCallback*>::iterator jt;
      for (jt = it->second.begin(); jt != it->second.end(); ++jt)
      {
         delete (*jt)->mCallbackLinkPtr;
         (*jt)->mCallbackLinkPtr = nullptr;
      }
      it->second.clear();
   }
   mBlockedCallbackList.clear();
}

bool VaCallbackList::EraseCallback(MapType& aList, VaCallback* aCallbackPtr)
{
   bool              retval = false;
   MapType::iterator it     = aList.find(aCallbackPtr->GetGroupId());
   if (it != aList.end())
   {
      std::set<VaCallback*>::iterator jt = it->second.find(aCallbackPtr);
      if (jt != it->second.end())
      {
         it->second.erase(jt);
         retval = true;
      }
      if (it->second.empty())
      {
         aList.erase(it);
      }
   }
   return retval;
}

void VaCallbackList::BeginProfile()
{
   if (sEnableProfile)
   {
      bool         found = true;
      ProfileNode* pn    = sProfileStack.empty() ? nullptr : sProfileStack.top()->FindChild(GetUniqueId());
      if (pn == nullptr)
      {
         pn    = new ProfileNode(GetUniqueId(), GetCallbackType(), GetName());
         found = false;
      }

      // Add the new node as a child.
      if (!found)
      {
         sProfileStack.top()->AddChild(pn);
      }

      // Add the new node to the stack.
      sProfileStack.push(pn);

      pn->mNumberCallbacks = 0;
      pn->mTimerPtr->ResetClock();
   }
}

void VaCallbackList::EndProfile()
{
   if (sEnableProfile)
   {
      ProfileNode* pn = sProfileStack.top();
      sProfileStack.pop();
      ++(pn->mNumberCalls);
      pn->mTotalTime += pn->mTimerPtr->GetClock();

      // Update the root node's total time.
      GetProfileTree()->mTotalTime = GetProfileTree()->mTimerPtr->GetClock();
   }
}

void VaCallbackList::BeginCallbackProfile()
{
   if (sEnableProfile)
   {
      ProfileNode* pn = sProfileStack.top();
      ++(pn->mNumberCallbacks);
   }
}

void VaCallbackList::EndCallbackProfile() {}

void VaCallbackList::SetEnableProfile(bool aEnableProfile)
{
   if (sEnableProfile != aEnableProfile)
   {
      // Clean up the old profile data.
      CleanUpProfile();

      // This needs to occur after CleanUpProfile.
      sEnableProfile = aEnableProfile;

      // Generate a new root profile node.
      if (sEnableProfile)
      {
         GetProfileTree();
      }
   }
}

bool VaCallbackList::GetEnableProfile()
{
   return sEnableProfile;
}

void VaCallbackList::PrintProfile(const std::string& aFilename)
{
   std::string filename = aFilename;
   if (aFilename.empty())
   {
      filename = sProfileFileName;
   }

   // If we find a '%date%' substitution, fill it with the current date.
   static const std::string datePattern = "{date}";
   size_t                   pos         = filename.find(datePattern);
   if (pos != std::string::npos)
   {
      UtCalendar calendar;
      calendar.SetCurrentDateAndTime();

      std::ostringstream oss1;
      // clang-format off
      oss1 << calendar.GetYear()
         << std::setw(2) << std::setfill('0') << calendar.GetMonth()
         << std::setw(2) << std::setfill('0') << calendar.GetDay() << "-"
         << std::setw(2) << std::setfill('0') << calendar.GetHour()
         << std::setw(2) << std::setfill('0') << calendar.GetMinute()
         << std::setw(2) << std::setfill('0') << calendar.GetSecond();
      // clang-format on
      std::string date = oss1.str().c_str();
      filename.replace(pos, datePattern.length(), date);

      std::ostringstream oss2;
      // clang-format off
      oss2 << calendar.GetYear() << "-"
         << std::setw(2) << std::setfill('0') << calendar.GetMonth() << "-"
         << std::setw(2) << std::setfill('0') << calendar.GetDay() << "T"
         << std::setw(2) << std::setfill('0') << calendar.GetHour() << ":"
         << std::setw(2) << std::setfill('0') << calendar.GetMinute() << ":"
         << std::setw(2) << std::setfill('0') << calendar.GetSecond();
      // clang-format on
      sProfileDateTime = oss2.str().c_str();
   }

   std::ofstream fout(filename);
   if (fout)
   {
      auto out = ut::log::info() << "Writing profile log.";
      out.AddNote() << "File: " << filename;
      PrintProfile(fout);
   }
}

void VaCallbackList::PrintProfile(std::ostream& aOut)
{
   aOut << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << '\n';
   aOut << "<vespa_performance_data>" << '\n';
   aOut << "  <date>" << sProfileDateTime << "</date>" << '\n';
   GetProfileTree()->Print("", aOut);
   aOut << "</vespa_performance_data>" << '\n';

   // Reset the date-time string.
   sProfileDateTime = "";
}

void VaCallbackList::CleanUpProfile()
{
   // Clean up the old profile data.
   if (sProfileTreePtr != nullptr)
   {
      ProfileNode* rootPtr = GetProfileTree();
      delete rootPtr;
      sProfileTreePtr = nullptr;
   }
   sEnableProfile = false;

   // Empty the stack.
   if (!sProfileStack.empty())
   {
      sProfileStack.pop();
   }
}

void VaCallbackList::ConnectP(VaCallback* aCallbackPtr)
{
   // If the callback is currently connected to a list then disconnect it from that list.
   aCallbackPtr->Disconnect();

   // Connect the callback if not already in the list.
   aCallbackPtr->mCallbackLinkPtr = new CallbackLink(this);
   if (aCallbackPtr->IsBlocked())
   {
      mBlockedCallbackList[aCallbackPtr->GetGroupId()].insert(aCallbackPtr);
   }
   else
   {
      mCallbackList[aCallbackPtr->GetGroupId()].insert(aCallbackPtr);
      if (mCallbackDepth != 0)
      {
         mTempCallbackList[aCallbackPtr->GetGroupId()].insert(aCallbackPtr);
      }
   }
}

VaCallbackList::ProfileNode::ProfileNode()
   : mCallbackUniqueId(0)
   , mCallbackType("")
   , mCallbackName("")
   , mNumberCalls(0)
   , mNumberCallbacks(0)
   , mTotalTime(0.0)
   , mTimerPtr(new UtWallClock())
{
}

VaCallbackList::ProfileNode::ProfileNode(int                aCallbackUniqueId,
                                         const std::string& aCallbackType,
                                         const std::string& aCallbackName)
   : mCallbackUniqueId(aCallbackUniqueId)
   , mCallbackType(aCallbackType)
   , mCallbackName(aCallbackName)
   , mNumberCalls(0)
   , mNumberCallbacks(0)
   , mTotalTime(0.0)
   , mTimerPtr(new UtWallClock())
{
}

VaCallbackList::ProfileNode::~ProfileNode()
{
   for (Children::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      delete *it;
   }
   mChildren.clear();

   delete mTimerPtr;
}


VaCallbackList::ProfileNode* VaCallbackList::GetProfileTree()
{
   if (sProfileTreePtr == nullptr)
   {
      sProfileTreePtr = new ProfileNode(0, "Root", "Root");
      sProfileStack.push(sProfileTreePtr);
   }
   return sProfileTreePtr;
}

VaCallbackList::ProfileNode* VaCallbackList::ProfileNode::AddChild(ProfileNode* aProfileNodePtr)
{
   mChildren.push_back(aProfileNodePtr);
   return aProfileNodePtr;
}

VaCallbackList::ProfileNode* VaCallbackList::ProfileNode::FindChild(int aCallbackUniqueId)
{
   ProfileNode* childPtr = nullptr;
   for (Children::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      if ((*it)->mCallbackUniqueId == aCallbackUniqueId)
      {
         childPtr = *it;
         break;
      }
   }
   return childPtr;
}

double VaCallbackList::ProfileNode::CalculateSelfTime() const
{
   double childTime = 0;
   for (Children::const_iterator it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      childTime += (*it)->mTotalTime;
   }
   return mTotalTime - childTime;
}

void VaCallbackList::ProfileNode::Print(const std::string& aIndent, std::ostream& aOut) const
{
   // clang-format off
   aOut << aIndent << "<performance_data " << aIndent 
      << "uid=\"" << mCallbackUniqueId << "\""
      << " type=\"" << mCallbackType << "\""
      << " name=\"" << mCallbackName << "\""
      << " callbacks=\"" << mNumberCallbacks << "\""
      << " calls=\"" << mNumberCalls << "\""
      << " self-time=\"" << CalculateSelfTime() << "\""
      << " total-time=\"" << mTotalTime << "\""
      << ">" << '\n';
   // clang-format on

   for (Children::const_iterator it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      (*it)->Print(aIndent + "  ", aOut);
   }
   aOut << aIndent << "</performance_data>" << '\n';
}
} // end namespace vespa
