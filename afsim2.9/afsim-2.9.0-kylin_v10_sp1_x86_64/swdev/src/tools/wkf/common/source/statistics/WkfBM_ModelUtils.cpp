// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfBM_ModelUtils.hpp"

#include <algorithm>

#include <QFont>

#include "WkfBM_ModelBase.hpp"

// ========================================= //
//        wkf::bm::detail::Visibility        //
// ========================================= //

void wkf::bm::detail::Visibility::SetHiddenByProgram(bool aHidden) noexcept
{
   if (aHidden)
   {
      mFlags |= HiddenByProgram;
   }
   else
   {
      mFlags &= ~HiddenByProgram;
   }
}

void wkf::bm::detail::Visibility::SetHiddenByUser(bool aHidden) noexcept
{
   if (aHidden)
   {
      mFlags |= HiddenByUser;
   }
   else
   {
      mFlags &= ~HiddenByUser;
   }
}

void wkf::bm::detail::Visibility::SetExpanded(bool aExpanded) noexcept
{
   if (aExpanded)
   {
      mFlags |= Expanded;
   }
   else
   {
      mFlags &= ~Expanded;
   }
}

void wkf::bm::detail::Visibility::ToggleExpanded() noexcept
{
   mFlags ^= Expanded;
}

bool wkf::bm::detail::Visibility::IsHiddenByProgram() const noexcept
{
   return mFlags & HiddenByProgram;
}

bool wkf::bm::detail::Visibility::IsHiddenByUser() const noexcept
{
   return mFlags & HiddenByUser;
}

bool wkf::bm::detail::Visibility::IsVisible() const noexcept
{
   return !(mFlags & HiddenMask);
}

bool wkf::bm::detail::Visibility::IsSetExpanded() const noexcept
{
   return (mFlags & Expanded);
}

bool wkf::bm::detail::Visibility::IsExpanded() const noexcept
{
   return IsVisible() && IsSetExpanded();
}

// ========================================= //
//           wkf::bm::detail::Node           //
// ========================================= //

wkf::bm::detail::Node::Node(const Node* aParent, QString aTitle) noexcept
   : mParent(aParent)
   , mTitle(std::move(aTitle))
   , mColumn(aParent ? aParent->mColumn + 1 : -1)
{
   mVisibility.SetExpanded(true);
}

QStringList wkf::bm::detail::Node::GetNodeIndex() const noexcept
{
   QStringList         retval;
   const detail::Node* ptr = this;

   while (ptr->mColumn >= 0)
   {
      retval.prepend(ptr->mTitle);
      ptr = ptr->mParent;
   }

   return retval;
}

bool wkf::bm::detail::Node::IsDisplayed() const noexcept
{
   if (mParent)
   {
      return mVisibility.IsVisible() && mParent->mVisibility.IsSetExpanded() && mParent->IsDisplayed();
   }
   return mVisibility.IsVisible();
}

// ========================================= //
//            wkf::bm::NodeProxy             //
// ========================================= //

wkf::bm::NodeProxy::NodeProxy(detail::Node* aNode, ModelBase* aModel) noexcept
   : mNode(aNode)
   , mModel(aModel)
{
}

bool wkf::bm::NodeProxy::IsValid() const noexcept
{
   return mNode;
}

wkf::bm::NodeProxy::operator bool() const noexcept
{
   return IsValid();
}

bool wkf::bm::NodeProxy::operator!() const noexcept
{
   return !IsValid();
}

int wkf::bm::NodeProxy::GetColumn() const noexcept
{
   if (IsValid())
   {
      return mNode->mColumn;
   }
   return -1;
}

int wkf::bm::NodeProxy::GetCachedRow() const noexcept
{
   if (IsValid())
   {
      return mNode->mRow;
   }
   return -1;
}

QString wkf::bm::NodeProxy::GetTitle() const noexcept
{
   if (IsValid())
   {
      return mNode->mTitle;
   }
   return QString{};
}

QStringList wkf::bm::NodeProxy::GetNodeIndex() const noexcept
{
   if (IsValid())
   {
      return mNode->GetNodeIndex();
   }
   return QStringList{};
}

wkf::bm::ColorSetting wkf::bm::NodeProxy::GetColor() const noexcept
{
   if (IsValid())
   {
      return mNode->mColor;
   }
   return ut::nullopt;
}

bool wkf::bm::NodeProxy::IsHiddenByProgram() const noexcept
{
   return IsValid() && mNode->mVisibility.IsHiddenByProgram();
}

bool wkf::bm::NodeProxy::IsHiddenByUser() const noexcept
{
   return IsValid() && mNode->mVisibility.IsHiddenByUser();
}

bool wkf::bm::NodeProxy::IsVisible() const noexcept
{
   return IsValid() && mNode->mVisibility.IsVisible();
}

bool wkf::bm::NodeProxy::IsExpanded() const noexcept
{
   return IsValid() && mNode->mVisibility.IsExpanded();
}

bool wkf::bm::NodeProxy::IsDisplayed() const noexcept
{
   return IsValid() && mNode->IsDisplayed();
}

void wkf::bm::NodeProxy::SetColor(ColorSetting aColor) noexcept
{
   if (IsValid() && mNode->mColor != aColor)
   {
      mNode->mColor = std::move(aColor);
      if (mModel && IsDisplayed())
      {
         mModel->EmitFixedColumnColorChanged(mNode->mRow, mNode->mColumn);
      }
   }
}

void wkf::bm::NodeProxy::SetHiddenByProgram(bool aHidden) noexcept
{
   if (IsValid() && IsHiddenByProgram() != aHidden)
   {
      const bool pre = mNode->IsDisplayed();
      mNode->mVisibility.SetHiddenByProgram(aHidden);
      const bool post = mNode->IsDisplayed();

      if (pre != post)
      {
         VisibilityChanged();
      }
   }
}

void wkf::bm::NodeProxy::SetHiddenByUser(bool aHidden) noexcept
{
   if (IsValid() && IsHiddenByUser() != aHidden)
   {
      const bool pre = mNode->IsDisplayed();
      mNode->mVisibility.SetHiddenByUser(aHidden);
      const bool post = mNode->IsDisplayed();

      if (pre != post)
      {
         VisibilityChanged();
      }
   }
}

void wkf::bm::NodeProxy::SetExpanded(bool aExpanded) noexcept
{
   if (IsValid() && (IsExpanded() != aExpanded))
   {
      mNode->mVisibility.SetExpanded(aExpanded);
      if (mNode->IsDisplayed())
      {
         VisibilityChanged();
      }
   }
}

void wkf::bm::NodeProxy::ToggleExpanded() noexcept
{
   if (IsValid())
   {
      mNode->mVisibility.ToggleExpanded();
      if (mNode->IsDisplayed())
      {
         VisibilityChanged();
      }
   }
}

QList<wkf::bm::NodeProxy> wkf::bm::NodeProxy::GetChildren() noexcept
{
   if (IsValid())
   {
      QList<wkf::bm::NodeProxy> retval;

      for (auto& child : mNode->mChildren)
      {
         retval += NodeProxy{&child, mModel};
      }

      return retval;
   }

   return {};
}

wkf::bm::detail::Node* wkf::bm::NodeProxy::GetPointer() noexcept
{
   return mNode;
}

const wkf::bm::detail::Node* wkf::bm::NodeProxy::GetPointer() const noexcept
{
   return mNode;
}

void wkf::bm::NodeProxy::VisibilityChanged()
{
   if (mModel)
   {
      mModel->UpdateCache();
   }
}

// ========================================= //
//            wkf::bm::LeafProxy             //
// ========================================= //

wkf::bm::LeafProxy::LeafProxy(detail::Node* aNode, ModelBase* aModel, const QStringList& aDataNames) noexcept
   : NodeProxy(aNode, aModel)
   , mDataNames(aDataNames)
{
}

bool wkf::bm::LeafProxy::SetData(int aDataIndex, QVariant aValue, ColorSetting aColor)
{
   if (IsValid() && aDataIndex >= 0 && aDataIndex < static_cast<int>(mNode->mData.size()))
   {
      SetDataP(aDataIndex, std::move(aValue), aColor);
      return true;
   }
   return false;
}

QVariant wkf::bm::LeafProxy::GetData(int aDataIndex) const
{
   if (IsValid() && aDataIndex >= 0 && static_cast<int>(mNode->mData.size()))
   {
      return mNode->mData[aDataIndex].mValue;
   }
   return QVariant{};
}

QStringList wkf::bm::LeafProxy::GetDataNames() const noexcept
{
   return mDataNames;
}

void wkf::bm::LeafProxy::SetDataP(int aDataIndex, QVariant aValue, ColorSetting aColor)
{
   auto& data = mNode->mData[aDataIndex];

   if (data.mValue != aValue || data.mColor != aColor)
   {
      data.mValue = std::move(aValue);
      data.mColor = aColor;

      if (mModel && IsDisplayed())
      {
         auto index = mModel->index(mNode->mRow, mModel->FixedColumnDisplayCount() + aDataIndex);
         emit mModel->dataChanged(index, index, {Qt::DisplayRole, Qt::BackgroundRole});
      }
   }
}
