param(
    [Parameter(Mandatory=$true)][string]$Source,
    [Parameter(Mandatory=$true)][string]$Destination
)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
$null = New-Item -ItemType Directory -Path $Destination -Force
$iconPath = Join-Path $Destination 'YaoEngine-mechanical.ico'
$pngPath = Join-Path $Destination 'YaoEngine-mechanical.png'
$previewPath = Join-Path $Destination 'YaoEngine-mechanical-256.png'
foreach ($path in @($iconPath, $pngPath, $previewPath)) {
    if (Test-Path -LiteralPath $path) { throw "Output already exists: $path" }
}
Copy-Item -LiteralPath $Source -Destination $pngPath
$sourceBitmap = [System.Drawing.Bitmap]::FromFile($Source)
$sizes = @(16,20,24,32,48,64,128,256)
$frames = [System.Collections.Generic.List[byte[]]]::new()
try {
    foreach ($size in $sizes) {
        $bitmap = [System.Drawing.Bitmap]::new($size, $size, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
        $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
        $attributes = [System.Drawing.Imaging.ImageAttributes]::new()
        $stream = [System.IO.MemoryStream]::new()
        try {
            $graphics.Clear([System.Drawing.Color]::Transparent)
            $graphics.CompositingMode = [System.Drawing.Drawing2D.CompositingMode]::SourceCopy
            $graphics.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
            $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
            $graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
            $attributes.SetWrapMode([System.Drawing.Drawing2D.WrapMode]::TileFlipXY)
            $rectangle = [System.Drawing.Rectangle]::new(0,0,$size,$size)
            $graphics.DrawImage($sourceBitmap,$rectangle,0,0,$sourceBitmap.Width,$sourceBitmap.Height,[System.Drawing.GraphicsUnit]::Pixel,$attributes)
            $bitmap.Save($stream,[System.Drawing.Imaging.ImageFormat]::Png)
            $frames.Add($stream.ToArray())
            if ($size -eq 256) { [System.IO.File]::WriteAllBytes($previewPath,$stream.ToArray()) }
        } finally {
            $stream.Dispose()
            $attributes.Dispose()
            $graphics.Dispose()
            $bitmap.Dispose()
        }
    }
} finally { $sourceBitmap.Dispose() }
$fileStream = [System.IO.File]::Create($iconPath)
$writer = [System.IO.BinaryWriter]::new($fileStream)
try {
    $writer.Write([uint16]0)
    $writer.Write([uint16]1)
    $writer.Write([uint16]$sizes.Count)
    $offset = 6 + 16 * $sizes.Count
    for ($index = 0; $index -lt $sizes.Count; $index++) {
        $dimension = if ($sizes[$index] -eq 256) { 0 } else { $sizes[$index] }
        $writer.Write([byte]$dimension)
        $writer.Write([byte]$dimension)
        $writer.Write([byte]0)
        $writer.Write([byte]0)
        $writer.Write([uint16]1)
        $writer.Write([uint16]32)
        $writer.Write([uint32]$frames[$index].Length)
        $writer.Write([uint32]$offset)
        $offset += $frames[$index].Length
    }
    foreach ($frame in $frames) { $writer.Write([byte[]]$frame) }
} finally { $writer.Dispose(); $fileStream.Dispose() }
$bytes = [System.IO.File]::ReadAllBytes($iconPath)
if ([BitConverter]::ToUInt16($bytes,0) -ne 0 -or [BitConverter]::ToUInt16($bytes,2) -ne 1 -or [BitConverter]::ToUInt16($bytes,4) -ne $sizes.Count) {
    throw 'ICO header is invalid.'
}
for ($index = 0; $index -lt $sizes.Count; $index++) {
    $entry = 6 + 16 * $index
    $length = [BitConverter]::ToUInt32($bytes,$entry+8)
    $offset = [BitConverter]::ToUInt32($bytes,$entry+12)
    if (($offset + $length) -gt $bytes.Length) { throw 'ICO frame is out of bounds.' }
    $stream = [System.IO.MemoryStream]::new($bytes,[int]$offset,[int]$length)
    $bitmap = [System.Drawing.Bitmap]::new($stream)
    try {
        if ($bitmap.Width -ne $sizes[$index] -or $bitmap.Height -ne $sizes[$index]) { throw 'ICO frame dimension mismatch.' }
        if ($bitmap.GetPixel(0,0).A -ne 0) { throw 'ICO frame lost transparency.' }
    } finally { $bitmap.Dispose(); $stream.Dispose() }
}
$windowsIcon = [System.Drawing.Icon]::new($iconPath,256,256)
try {
    [pscustomobject]@{Icon=$iconPath;Sizes=($sizes -join ', ');Bytes=$bytes.Length;WindowsDecodedSize=$windowsIcon.Size;Transparency='Verified in all 8 frames'} | Format-List
} finally { $windowsIcon.Dispose() }
