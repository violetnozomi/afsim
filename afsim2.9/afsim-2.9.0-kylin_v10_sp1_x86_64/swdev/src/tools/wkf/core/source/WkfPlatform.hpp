// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WkfPlatform_HPP
#define WkfPlatform_HPP

#include "wkf_export.h"

#include <algorithm>

#include <QList>

#include "WkfDefines.hpp"
#include "WkfEntity.hpp"

namespace wkf
{
	class WKF_EXPORT Platform : public Entity
	{
	public:
		Platform(const std::string& aName);
		~Platform() override = default;

		void               SetSide(const std::string& aSide) { mSide = aSide; }
		const std::string& GetSide() const { return mSide; }

		void               SetIcon(const std::string& aIcon) { mIcon = aIcon; }
		const std::string& GetIcon() const { return mIcon; }

		void         SetIndex(unsigned int aIndex) { mIndex = aIndex; }
		unsigned int GetIndex() const { return mIndex; }

		// These functions deal with the AFSIM platform type.
		//  Do not to confuse these functions with the VaObject object type which have similar names.
		void                            SetTypeList(const std::vector<std::string>& aTypeList) { mTypeList = aTypeList; }
		const std::vector<std::string>& GetTypeList() const { return mTypeList; }
		std::string                     GetTypeAFSIM() const;
		bool                            IsOfType(const std::string& aType) const
		{
			return std::find(mTypeList.begin(), mTypeList.end(), aType) != mTypeList.end();
		}

		bool IsAlive() const { return mBrokenTime == -1.0; }

		void SetBroken(double aBrokenTime);
		void SetDeleted(double aDeletionTime);

		SpatialDomain GetSpatialDomain() const { return mSpatialDomain; }
		void          SetSpatialDomain(const SpatialDomain& aDomain) { mSpatialDomain = aDomain; }

		std::string GetDescription() override;

		struct Subobject
		{
			std::string mIcon;
			double      mPositionECS[3];
			double      mOrientationECS[3];
		};

		void AddSubobject(const std::string& aName,
			const std::string& aIcon,
			const double       aPositionECS[3],
			const double       aOrientationECS[3]);
		void UpdateSubobject(const std::string& aName, const double aPositionECS[3], const double aOrientationECS[3]);
		void RemoveSubobject(const std::string& aName);
		// for management, eventually we may was GetSubobjectByUniqueId, too
		const std::map<std::string, Subobject>& GetSubobjectMap() const { return mSubobjectMap; }

		void SetBeingDragged(bool aDragging) { mIsBeingDragged = aDragging && GetDraggable(); }
		bool IsBeingDragged() const { return mIsBeingDragged; }

	private:
		std::string   mSide;
		std::string   mIcon;
		unsigned int  mIndex;
		SpatialDomain mSpatialDomain;

		std::vector<std::string>         mTypeList;
		std::map<std::string, Subobject> mSubobjectMap;

		double mBrokenTime;
		double mDeletionTime;

		bool mIsBeingDragged;
	};

	using PlatformList = QList<wkf::Platform*>;
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::Platform)
#endif // WkfPlatform_HPP
