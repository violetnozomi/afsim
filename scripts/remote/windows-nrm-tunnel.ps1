<#
.SYNOPSIS
Creates the SSH tunnel used by the AFSIM Network Resource Manager VNC desktop.
#>

param(
    [Parameter(Mandatory = $true)]
    [string]$Server,

    [string]$User = "pyh",

    [int]$SshPort = 22
)

$ErrorActionPreference = "Stop"

Write-Host "Forwarding Windows 127.0.0.1:5901 to $User@$Server via SSH."
Write-Host "Keep this window open, then connect the VNC Viewer to 127.0.0.1:5901."

ssh -p $SshPort -N -o ExitOnForwardFailure=yes `
    -L "5901:127.0.0.1:5901" "$User@$Server"
