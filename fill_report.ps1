$ErrorActionPreference = "Stop"
$base = "D:\HuaweiMoveData\Users\23022\Desktop\home\Sophomore_second_semester\oop\library"
$contentDir = Join-Path $base "report_content"
$docFile = Get-ChildItem -Path $base -Filter "*.doc" | Where-Object { $_.Name -notlike "*backup*" } | Select-Object -First 1
if ($null -eq $docFile) { throw "Reference doc not found" }
$docPath = $docFile.FullName
$backupPath = Join-Path $base ($docFile.BaseName + "_backup.doc")

if (-not (Test-Path $backupPath)) {
    Copy-Item $docPath $backupPath
} else {
    Copy-Item $backupPath $docPath -Force
}

function Read-Utf8($path) {
    return [System.IO.File]::ReadAllText($path, [System.Text.Encoding]::UTF8)
}

function To-WordCellText($text) {
    $t = $text.Trim()
    $t = $t -replace "`r`n", "`n"
    $t = $t -replace "`r", "`n"
    $parts = $t -split "`n"
    return ($parts | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne "" }) -join [char]11
}

function Set-CellText($cell, $text) {
    $cell.Range.Text = (To-WordCellText $text)
}

function Parse-EvalRows($raw) {
    $rows = @()
    $blocks = $raw -split "###ROW\d+###"
    foreach ($block in $blocks) {
        if ([string]::IsNullOrWhiteSpace($block)) { continue }
        if ($block -match "SOURCE\s*(.*?)\s*SCORE\s*(.*?)\s*REASON\s*(.*)" -or $true) {
            $m = [regex]::Match($block, "SOURCE\s*(.*?)\s*SCORE\s*(.*?)\s*REASON\s*(.*)", "Singleline")
            if ($m.Success) {
                $rows += [PSCustomObject]@{
                    Source = $m.Groups[1].Value.Trim()
                    Score  = $m.Groups[2].Value.Trim()
                    Reason = $m.Groups[3].Value.Trim()
                }
            }
        }
    }
    return $rows
}

function Insert-AfterHeading($doc, $heading, $content) {
    $find = $doc.Content.Find
    $find.ClearFormatting()
    $find.Text = $heading
    $find.Forward = $true
    $find.Wrap = 1
    if (-not $find.Execute()) { return }
    $para = $find.Parent.Paragraphs.Item(1)
    $nextPara = $para.Next()
    if ($null -ne $nextPara) {
        $nextPara.Range.Text = $content + "`r"
    } else {
        $para.Range.InsertParagraphAfter()
        $para.Next().Range.Text = $content + "`r"
    }
}

$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0
$doc = $null

try {
    $doc = $word.Documents.Open($docPath)

    $doc.Paragraphs.Item(8).Range.Text = (Read-Utf8 (Join-Path $contentDir "cover_name.txt")).Trim() + "`r"
    $doc.Paragraphs.Item(18).Range.Text = (Read-Utf8 (Join-Path $contentDir "cover_date.txt")).Trim() + "`r"

    $evalRows = Parse-EvalRows (Read-Utf8 (Join-Path $contentDir "eval.txt"))
    $table = $doc.Tables.Item(1)
    for ($i = 0; $i -lt $evalRows.Count; $i++) {
        $row = $i + 2
        Set-CellText $table.Cell($row, 3) $evalRows[$i].Source
        Set-CellText $table.Cell($row, 4) $evalRows[$i].Score
        Set-CellText $table.Cell($row, 5) $evalRows[$i].Reason
    }

    $deleteTexts = (Read-Utf8 (Join-Path $contentDir "delete_lines.txt")) -split "`r`n|`n"
    foreach ($t in $deleteTexts) {
        if ([string]::IsNullOrWhiteSpace($t)) { continue }
        $find = $doc.Content.Find
        $find.ClearFormatting()
        $find.Text = $t.Trim()
        $find.Forward = $true
        $find.Wrap = 1
        if ($find.Execute()) {
            $find.Parent.Delete()
        }
    }

    $headingLines = (Read-Utf8 (Join-Path $contentDir "headings.tsv")) -split "`r`n|`n"
    foreach ($line in $headingLines) {
        if ([string]::IsNullOrWhiteSpace($line)) { continue }
        $parts = $line -split "`t", 2
        if ($parts.Count -lt 2) { continue }
        $sectionPath = Join-Path $contentDir $parts[1].Trim()
        Insert-AfterHeading $doc $parts[0].Trim() (Read-Utf8 $sectionPath)
    }

    $doc.Save()
    Write-Output "SUCCESS rows=$($evalRows.Count)"
}
finally {
    if ($null -ne $doc) { $doc.Close($true) }
    $word.Quit()
    if ($null -ne $doc) { [void][System.Runtime.Interopservices.Marshal]::ReleaseComObject($doc) }
    [void][System.Runtime.Interopservices.Marshal]::ReleaseComObject($word)
    [GC]::Collect()
    [GC]::WaitForPendingFinalizers()
}
