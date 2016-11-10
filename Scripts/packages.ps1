# URL Configuration
$packageDir = $args[0] # Directory to save packages to
$release = $args[1] # Package release to download from

Write-Output "Scanning packages..."

# Enumerate the list of packages
foreach ($package in $args[2..$args.Length])
{
    $packagePath = $packageDir + $package

    # Check if we've already downloaded the package
    if (Test-Path $packagePath)
    {
        Write-Output ("Package {0} is up to date." -f $package)
        continue
    }

    $temp = [System.IO.Path]::GetTempFileName() + ".zip"

    # Download the package
    Write-Output ("Downloading package {0}..." -f $package)
    $url = "https://github.com/willcassella/SinGE/releases/download/{0}/{1}.zip" -f $release, $package
    $web = New-Object net.webclient
    $web.DownloadFile($url, $temp)

    # Extract the package
    Write-Output ("Extracting package {0}..." -f $package)
    Expand-Archive $temp -dest $packagePath
    Remove-Item $temp
}
