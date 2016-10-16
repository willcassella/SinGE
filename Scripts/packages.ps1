# URL Configuration
$project = $args[0]
$release = "dependencies-v0.1"
$url = "https://github.com/willcassella/SinGE/releases/download/" + $release + "/" + $project + ".zip"

# Check if we've alredy downloaded packages
if (Test-Path "packages")
{
    echo "Packages already up to date"
    exit
}

echo "Downloading packages..."

# Download the packages file
wget $url -outfile "packages.zip"
Expand-Archive "packages.zip" -dest "packages"
Remove-Item "packages.zip"

echo "Package download complete"
