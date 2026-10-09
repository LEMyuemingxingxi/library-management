$ErrorActionPreference = "Stop"
$base = "D:\HuaweiMoveData\Users\23022\Desktop\home\Sophomore_second_semester\oop\library"
$contentDir = Join-Path $base "report_content"
$docFile = Get-ChildItem -Path $base -Filter "*.doc" | Where-Object { $_.Name -notlike "*backup*" } | Select-Object -First 1
if ($null -eq $docFile) { throw "doc not found" }

function Read-Utf8($path) {
    return [System.IO.File]::ReadAllText($path, [System.Text.Encoding]::UTF8)
}

function To-WordCellText($text) {
    $t = $text.Trim() -replace "`r`n", "`n" -replace "`r", "`n"
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
        $m = [regex]::Match($block, "SOURCE\s*(.*?)\s*SCORE\s*(.*?)\s*REASON\s*(.*)", "Singleline")
        if ($m.Success) {
            $rows += [PSCustomObject]@{
                Source = $m.Groups[1].Value.Trim()
                Score  = $m.Groups[2].Value.Trim()
                Reason = $m.Groups[3].Value.Trim()
            }
        }
    }
    return $rows
}

$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0
$doc = $null

try {
    $doc = $word.Documents.Open($docFile.FullName, $false, $false)

    $evalRows = Parse-EvalRows (Read-Utf8 (Join-Path $contentDir "eval.txt"))
    $table = $doc.Tables.Item(1)
    for ($i = 0; $i -lt $evalRows.Count; $i++) {
        $row = $i + 2
        Set-CellText $table.Cell($row, 3) $evalRows[$i].Source
        Set-CellText $table.Cell($row, 4) $evalRows[$i].Score
        Set-CellText $table.Cell($row, 5) $evalRows[$i].Reason
    }

    $refContent = (Read-Utf8 (Join-Path $contentDir "section5.txt")).Trim()
    $oldRef = "[1] 李际军. 2026年现代C++面向对象综合编程题（8-1 图书管理系统）[Z]. 浙江大学, 2026."
    $find = $doc.Content.Find
    $find.ClearFormatting()
    $find.Text = $oldRef
    $find.Forward = $true
    $find.Wrap = 1
    if ($find.Execute()) {
        $start = $find.Parent.Start
        $endPara = $find.Parent
        while ($null -ne $endPara.Next()) {
            $next = $endPara.Next()
            $txt = ($next.Range.Text -replace "`r|`a", "").Trim()
            if ($txt -match "^\[") { $endPara = $next } else { break }
        }
        $rng = $doc.Range($start, $endPara.Range.End)
        $rng.Text = ($refContent -replace "`n", "`r") + "`r"
    } else {
        $find2 = $doc.Content.Find
        $find2.ClearFormatting()
        $find2.Text = "5. 参考文献资料"
        $find2.Forward = $true
        $find2.Wrap = 1
        if ($find2.Execute()) {
            $para = $find2.Parent.Paragraphs.Item(1)
            $para.Range.InsertParagraphAfter()
            $para.Next().Range.Text = ($refContent -replace "`n", "`r") + "`r"
        }
    }

    $doc.Save()
    Write-Output "PATCH_OK rows=$($evalRows.Count)"
}
finally {
    if ($null -ne $doc) { $doc.Close($true) }
    $word.Quit()
    if ($null -ne $doc) { [void][System.Runtime.Interopservices.Marshal]::ReleaseComObject($doc) }
    [void][System.Runtime.Interopservices.Marshal]::ReleaseComObject($word)
    [GC]::Collect()
    [GC]::WaitForPendingFinalizers()
}
