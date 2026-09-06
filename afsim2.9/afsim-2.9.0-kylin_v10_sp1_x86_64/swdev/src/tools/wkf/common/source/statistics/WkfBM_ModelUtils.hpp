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

#ifndef WKF_BM_MODEL_UTILS_HPP
#define WKF_BM_MODEL_UTILS_HPP

#include <QColor>
#include <QModelIndex>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "UtCompilerAttributes.hpp"
#include "UtOptional.hpp"

namespace wkf
{
namespace bm
{
class ModelBase;
using ColorSetting = ut::optional<QColor>;

namespace detail
{
struct Visibility
{
   //! Enum represents collection of bits.
   enum : char
   {
      HiddenByProgram = 0x01,
      HiddenByUser    = 0x02,
      Expanded        = 0x04,

      HiddenMask = 0x03
   };

   char mFlags = Expanded;

   void SetHiddenByProgram(bool aHidden) noexcept;
   void SetHiddenByUser(bool aHidden) noexcept;

   void SetExpanded(bool aExpanded) noexcept;
   void ToggleExpanded() noexcept;

   bool IsHiddenByProgram() const noexcept;
   bool IsHiddenByUser() const noexcept;

   //! Requires not hidden by program or user.
   bool IsVisible() const noexcept;

   //! Returns if the node is expanded.
   bool IsSetExpanded() const noexcept;

   //! Returns if the node is visible and expanded.
   bool IsExpanded() const noexcept;
};

struct DataNode
{
   QVariant     mValue;
   ColorSetting mColor;
};

struct Node
{
   //! If aParent is nullptr, this is the root node.
   Node(const Node* aParent, QString aTitle) noexcept;
   QStringList GetNodeIndex() const noexcept;
   bool        IsDisplayed() const noexcept; //!< Requires visible and all ancestors expanded.

   std::list<Node> mChildren;   //!< Only used for non-leaf nodes
                                //!< Using list instead of vector to maintain pointer stability
   std::vector<DataNode> mData; //!< Only used for leaf nodes

   const Node* const mParent = nullptr;
   const QString     mTitle;
   const int         mColumn;

   Visibility   mVisibility;
   ColorSetting mColor;
   int          mRow = -1;
};
} // namespace detail

//! A proxy for a detail::Node.
//! It is lightweight and intended to be passed by value.
//! It is NOT intended to be used polymorphically.
class NodeProxy
{
public:
   //! Constructs an invalid NodeProxy.
   NodeProxy() noexcept = default;

   //! Creates a proxy to aNode.
   //! If aModel is set, it will be automatically updated whenever changes are made.
   //! Otherwise, the user is responsible for updating the model's cache.
   NodeProxy(detail::Node* aNode, ModelBase* aModel) noexcept;

   //! Returns true if proxying a valid node.
   bool IsValid() const noexcept;
   //! Same as @see IsValid().
   operator bool() const noexcept;
   //! Returns the opposite of @see IsValid().
   bool operator!() const noexcept;

   //! If IsValid(), returns the column of the node.
   //! Else, returns -1.
   int GetColumn() const noexcept;

   //! If IsValid() and mModel is set, returns the cached row of the node.
   //! * Note that this may not be the node's actual row.
   //! Else, returns -1.
   int GetCachedRow() const noexcept;

   //! If IsValid(), returns the title of the node.
   //! Else, returns an empty string.
   QString GetTitle() const noexcept;

   //! If IsValid(), returns the node index.
   //! Else, returns an empty list.
   QStringList GetNodeIndex() const noexcept;

   //! If IsValid(), returns the color of the node.
   //! Else, returns a default ColorSetting.
   ColorSetting GetColor() const noexcept;

   //! If IsValid(), returns the node's visibility.
   //! Else, returns false.
   //! @{
   bool IsHiddenByProgram() const noexcept;
   bool IsHiddenByUser() const noexcept;

   //! @note Only returns whether the current node is set to be visible.
   //!       Does not take into account parent nodes.
   bool IsVisible() const noexcept;
   bool IsExpanded() const noexcept;

   bool IsDisplayed() const noexcept;
   //! @}

   //! If IsValid(), sets the color of the node.
   void SetColor(ColorSetting aColor) noexcept;

   //! If IsValid(), adjusts the node's visibility.
   //! @{
   void SetHiddenByProgram(bool aHidden) noexcept;
   void SetHiddenByUser(bool aHidden) noexcept;

   void SetExpanded(bool aExpanded) noexcept;
   void ToggleExpanded() noexcept;
   //! @}

   //! If IsValid(), return a list of proxies to the node's children.
   //! Else, returns an empty list.
   QList<NodeProxy> GetChildren() noexcept;

   //! If IsValid(), returns the internal pointer.
   //! Else, returns nullptr.
   //! @{
   detail::Node*       GetPointer() noexcept;
   const detail::Node* GetPointer() const noexcept;
   //! @}

protected:
   void VisibilityChanged();

   detail::Node* mNode  = nullptr;
   ModelBase*    mModel = nullptr;
};

//! A proxy for a leaf detail::Node.
//! It is lightweight and intended to be passed by value.
class LeafProxy final : public NodeProxy
{
public:
   //! Constructs an invalid LeafProxy().
   LeafProxy() noexcept = default;

   //! Used by @see ModelBase to construct LeafProxy's.
   //! Should not be used by anyone else.
   LeafProxy(detail::Node* aNode, ModelBase* aModel, const QStringList& aDataNames) noexcept;

   //! Sets a data field in the proxied node.
   //! Returns true on success, false on failure.
   //! The operation may fail if the Leaf is not valid or aDataIndex is out of bounds.
   bool SetData(int aDataIndex, QVariant aValue, ColorSetting aColor);

   //! Returns a data field in the proxied node.
   QVariant GetData(int aDataIndex) const;

   //! Returns the data names passed to the constructor.
   QStringList GetDataNames() const noexcept;

private:
   QStringList mDataNames; //!< The names of the data fields.

   //! Assumes that IsValid() and aDataIndex is valid.
   void SetDataP(int aDataIndex, QVariant aValue, ColorSetting aColor);
};
} // namespace bm
} // namespace wkf

#endif
