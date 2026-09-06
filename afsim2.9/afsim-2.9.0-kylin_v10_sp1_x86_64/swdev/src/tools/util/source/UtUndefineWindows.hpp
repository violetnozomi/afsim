// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifdef _WIN32

#undef AccessCheckAndAuditAlarm
#undef AccessCheckByTypeAndAuditAlarm
#undef AccessCheckByTypeResultListAndAuditAlarm
#undef AccessCheckByTypeResultListAndAuditAlarmByHandle
#undef AddAtom
#undef AddFontResource
#undef AddFontResourceEx

#undef AnsiLower
#undef AnsiLowerBuff
#undef AnsiNext
#undef AnsiPrev
#undef AnsiToOem
#undef AnsiToOemBuff
#undef AnsiUpper
#undef AnsiUpperBuff

#undef AppendMenu

#undef BackupEventLog
#undef BadValue
#undef BeginUpdateResource
#undef BroadcastSystemMessage
#undef BroadcastSystemMessageEx
#undef BuildCommDCB
#undef BuildCommDCBAndTimeouts

#undef CallMsgFilter
#undef CallNamedPipe
#undef CallWindowProc
#undef CaptureStackBackTrace
#undef ChangeDisplaySettings
#undef ChangeDisplaySettingsEx
#undef ChangeMenu

#undef CharLower
#undef CharLowerBuff
#undef CharNext
#undef CharPrev
#undef CharToOem
#undef CharToOemBuff
#undef CharUpper
#undef CharUpperBuff

#undef CheckNameLegalDOS8Dot3
#undef ClearEventLog
#undef CommConfigDialog
#undef CopyAcceleratorTable
#undef CopyCursor
#undef CopyEnhMetaFile
#undef CopyFile
#undef CopyFileEx
#undef CopyFileTransacted
#undef CopyMetaFile

#undef CreateAcceleratorTable
#undef CreateActCtx
#undef CreateBoundaryDescriptor
#undef CreateColorSpace
#undef CreateDC
#undef CreateDesktop
#undef CreateDesktopEx

#undef CreateDialog
#undef CreateDialogA
#undef CreateDialogIndirect
#undef CreateDialogIndirectA
#undef CreateDialogIndirectParam
#undef CreateDialogIndirectW
#undef CreateDialogParam
#undef CreateDialogW

#undef CreateDirectoryEx
#undef CreateDirectoryTransacted
#undef CreateEnhMetaFile
#undef CreateFileMapping
#undef CreateFileMappingNuma
#undef CreateFileTransacted
#undef CreateFont
#undef CreateFontIndirect
#undef CreateFontIndirectEx
#undef CreateHardLink
#undef CreateHardLinkTransacted
#undef CreateIC
#undef CreateJobObject
#undef CreateMailslot
#undef CreateMDIWindow
#undef CreateMetaFile
#undef CreateNamedPipe
#undef CreatePrivateNamespace
#undef CreateScalableFontResource
#undef CreateSemaphore
#undef CreateSemaphoreEx
#undef CreateSymbolicLink
#undef CreateSymbolicLinkTransacted
#undef CreateWaitableTimer
#undef CreateWaitableTimerEx
#undef CreateWindow
#undef CreateWindowA
#undef CreateWindowEx
#undef CreateWindowA
#undef CreateWindowStation

#undef DecryptFile
#undef DefDlgProc
#undef DefFrameProc
#undef DefineDosDevice
#undef DefineHandleTable
#undef DefHookProc
#undef DefMDIChildProc
#undef DefWindowProc
#undef DeleteFileTransacted
#undef DeleteVolumeMountPoint
#undef DeviceCapabilities

#undef DialogBox
#undef DialogBoxA
#undef DialogBoxIndirect
#undef DialogBoxIndirectA
#undef DialogBoxIndirectParam
#undef DialogBoxIndirectW
#undef DialogBoxParam
#undef DialogBoxW

#undef DispatchMessage
#undef DlgDirList
#undef DlgDirListComboBox
#undef DlgDirSelectComboBoxEx
#undef DlgDirSelectEx
#undef DnsHostnameToComputerName
#undef DrawState
#undef DrawText
#undef DrawTextEx

#undef EncryptFile
#undef EndUpdateResource

#undef EnumDesktops
#undef EnumDisplayDevices
#undef EnumDisplaySettings
#undef EnumDisplaySettingsEx
#undef EnumFontFamilies
#undef EnumFontFamiliesEx
#undef EnumFonts
#undef EnumICMProfiles
#undef EnumProps
#undef EnumPropsEx
#undef EnumResourceLanguages
#undef EnumResourceNames
#undef EnumResourceTypes
#undef EnumTaskWindows
#undef EnumWindowStations

#undef ExitWindows
#undef ExtTextOut

#undef FileEncryptionStatus
#undef FindActCtxSectionString
#undef FindAtom
#undef FindFirstFileTransacted
#undef FindFirstVolume
#undef FindFirstVolumeMountPoint
#undef FindNextVolume
#undef FindNextVolumeMountPoint
#undef FindResource
#undef FindResourceEx
#undef FindWindow
#undef FindWindowEx
#undef FormatMessage
#undef FreeModule
#undef FreeProcInstance

#undef GetAltTabInfo
#undef GetAtomName
#undef GetBinaryType

#undef GetCharWidth
#undef GetCharWidth32
#undef GetCharABCWidths
#undef GetCharABCWidthsFloat
#undef GetCharacterPlacement
#undef GetCharWidthFloat

#undef GetClassInfo
#undef GetClassInfoEx
#undef GetClassLong
#undef GetClassLongPtr
#undef GetClassLongPtrA
#undef GetClassLongPtrW
#undef GetClassName

#undef GetClipboardFormatName
#undef GetCompressedFileSizeTransacted
#undef GetComputerName
#undef GetCurrentHwProfile
#undef GetCurrentTime
#undef GetDefaultCommConfig
#undef GetDlgItemText
#undef GetDllDirectory
#undef GetEnhMetaFile
#undef GetEnhMetaFileDescription
#undef GetFileAttributesTransacted
#undef GetFileSecurity
#undef GetFirmwareEnvironmentVariable
#undef GetFirmwareEnvironmentVariableEx
#undef GetFreeSpace
#undef GetFullPathNameTransacted
#undef GetGlyphIndices
#undef GetGlyphOutline
#undef GetICMProfile
#undef GetIconInfoEx
#undef GetKerningPairs
#undef GetKeyboardLayoutName
#undef GetKeyNameText
#undef GetLogColorSpace
#undef GetLogicalDriveStrings
#undef GetLongPathNameTransacted
#undef GetMenuItemInfo
#undef GetMenuString
#undef GetMetaFile
#undef GetMessage
#undef GetMonitorInfo
#undef GetNamedPipeClientComputerName
#undef GetNamedPipeHandleState
#undef GetNextWindow
#undef GetObject
#undef GetOutlineTextMetrics

#undef GetPrivateProfileInt
#undef GetPrivateProfileSection
#undef GetPrivateProfileSectionNames
#undef GetPrivateProfileString
#undef GetPrivateProfileStruct

#undef GetProfileInt
#undef GetProfileSection
#undef GetProfileString

#undef GetProp
#undef GetRawInputDeviceInfo
#undef GetShortPathName
#undef GetStartupInfo
#undef GetSysModalWindow
#undef GetTabbedTextExtent
#undef GetTextExtentExPoint
#undef GetTextExtentPoint
#undef GetTextExtentPoint32
#undef GetTextFace
#undef GetTextMetrics
#undef GetUserName
#undef GetUserObjectInformation
#undef GetVolumeNameForVolumeMountPoint
#undef GetVolumePathName
#undef GetVolumePathNamesForVolumeName

#undef GetWindowLong
#undef GetWindowLongPtr
#undef GetWindowLongPtrA
#undef GetWindowLongPtrW
#undef GetWindowModuleFileName
#undef GetWindowTask
#undef GetWindowText
#undef GetWindowTextLength

#undef GlobalAddAtom
#undef GlobalAddAtomEx
#undef GlobalDiscard
#undef GlobalFindAtom
#undef GlobalGetAtomName
#undef GlobalLRUNewest
#undef GlobalLRUOldest
#undef GrayString

#undef HasOverlappedIoCompleted

#undef InsertMenu
#undef InsertMenuItem
#undef IsBadStringPtr
#undef IsCharAlpha
#undef IsCharAlphaNumeric
#undef IsCharLower
#undef IsCharUpper
#undef IsDialogMessage

#undef KeyPress
#undef KeyRelease

#undef LimitEmsPages
#undef LoadAccelerators
#undef LoadBitmap
#undef LoadCursor
#undef LoadCursorFromFile
#undef LoadIcon
#undef LoadImage
#undef LoadKeyboardLayout
#undef LoadMenu
#undef LoadMenuIndirect
#undef LockSegment
#undef LogonUser
#undef LogonUserEx
#undef LookupAccountName
#undef LookupAccountNameLocal
#undef LookupAccountSid
#undef LookupAccountSidLocal
#undef LookupPrivilegeDisplayName
#undef LookupPrivilegeName
#undef LookupPrivilegeValue
#undef lstrcat
#undef lstrcmp
#undef lstrcmpi
#undef lstrcpy
#undef lstrcpyn
#undef lstrlen

#undef MakeProcInstance
#undef MessageBox
#undef MessageBoxEx
#undef MessageBoxIndirect
#undef min
#undef MapVirtualKey
#undef MapVirtualKeyEx
#undef max
#undef ModifyMenu
#undef MoveFile
#undef MoveFileEx
#undef MoveFileTransacted
#undef MoveFileWithProgress

#undef ObjectCloseAuditAlarm
#undef ObjectDeleteAuditAlarm
#undef ObjectOpenAuditAlarm
#undef ObjectPrivilegeAuditAlarm
#undef OemToAnsi
#undef OemToAnsiBuff
#undef OemToChar
#undef OemToCharBuff
#undef OpenBackupEventLog
#undef OpenDesktop
#undef OpenEncryptedFileRaw
#undef OpenEventLog
#undef OpenFileMapping
#undef OpenJobObject
#undef OpenMutex
#undef OpenPrivateNamespace
#undef OpenSemaphore
#undef OpenWaitableTimer
#undef OpenWindowStation

#undef PeekMessage
#undef PolyTextOut
#undef PostAppMessage
#undef PostAppMessageA
#undef PostAppMessageW
#undef PostMessage
#undef PostThreadMessage
#undef PrivateExtractIcons
#undef PrivilegedServiceAuditAlarm
#undef ProcThreadAttributeValue

#undef QueryDosDevice
#undef QueryFullProcessImageName

#undef RealGetWindowClass
#undef ReadEventLog
#undef RegisterClass
#undef RegisterClassEx
#undef RegisterClipboardFormat
#undef RegisterDeviceNotification
#undef RegisterEventSource
#undef RegisterWindowMessage
#undef RemoveDirectoryTransacted
#undef RemoveFontResource
#undef RemoveProp
#undef ReplaceFile
#undef ReportEvent
#undef ResetDC

#undef SecureZeroMemory
#undef SendDlgItemMessage
#undef SendMessage
#undef SendMessageCallback
#undef SendMessageTimeout
#undef SendNotifyMessage

#undef SetClassLong
#undef SetClassLongPtr
#undef SetClassLongPtrA
#undef SetClassLongPtrW
#undef SetDefaultCommConfig
#undef SetDlgItemText
#undef SetDllDirectory
#undef SetEnvironmentStrings
#undef SetFileAttributesTransacted
#undef SetFileSecurity
#undef SetFileShortName
#undef SetFirmwareEnvironmentVariable
#undef SetFirmwareEnvironmentVariableEx
#undef SetICMProfile
#undef SetMenuItemInfo
#undef SetVolumeMountPoint
#undef SetPort
#undef SetProp
#undef SetSwapAreaSize
#undef SetSysModalWindow
#undef SetUserObjectInformation
#undef SetVolumeLabel
#undef SetWindowLong
#undef SetWindowLongPtr
#undef SetWindowLongPtrA
#undef SetWindowLongPtrW
#undef SetWindowsHook
#undef SetWindowsHookEx
#undef SetWindowText

#undef StartDoc
#undef SystemParametersInfo

#undef TabbedTextOut
#undef TextOut
#undef TranslateAccelerator

#undef UnlockSegment
#undef UnregisterClass
#undef UpdateICMRegKey
#undef UpdateResource

#undef VerifyVersionInfo
#undef VkKeyScan
#undef VkKeyScanEx

#undef WaitNamedPipe
#undef wglUseFontBitmaps
#undef wglUseFontOutlines
#undef WinHelp
#undef WritePrivateProfileSection
#undef WritePrivateProfileString
#undef WritePrivateProfileStruct
#undef WriteProfileSection
#undef WriteProfileString
#undef wsprintf
#undef wvsprintf

#undef Yield

#endif /*_WIN32*/

