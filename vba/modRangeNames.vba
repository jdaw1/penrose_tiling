Option Explicit


' To go in module 'modRangeNames'
' Source: https://github.com/jdaw1/chart_markers
' or: https://github.com/jdaw1/penrose_tiling/vba



Sub RangeNames_DeleteRef()
	Dim n As Name, s As String, i As Integer
	i = 0
	For Each n In ActiveWorkbook.Names
		If Left(n.RefersTo, 6) = "=#REF!" Or Right(n.RefersTo, 6) = "!#REF!" Then s = n.Name: i = i + 1: n.Delete
	Next n
	If i = 0 Then MsgBox "All " & ActiveWorkbook.Names.Count & " names are good: none deleted."
	If i = 1 Then MsgBox "One name deleted, that being " & s & " (" & ActiveWorkbook.Names.Count & " names remain)."
	If i > 1 Then MsgBox i & " names deleted, including " & s & " (" & ActiveWorkbook.Names.Count & " names remain)."
End Sub  ' RangeNames_DeleteRef


Sub RangeNames_DeleteBeginningWith()
	Dim n As Name, nameStart As String, s As String, i As Integer

	nameStart = InputBox( _
		prompt:="Delete all range names beginning with what string? ", _
		Title:="Starting characters of range names to be deleted", Default:="")

	If Len(nameStart) = 0 Then Exit Sub

	i = 0
	For Each n In ActiveWorkbook.Names
		If 0 = StrComp(Left(n.Name, Len(nameStart)), nameStart, 0) Then s = n.Name: i = i + 1: n.Delete
	Next n
	If i = 0 Then MsgBox "No names deleted."
	If i = 1 Then MsgBox "One name deleted, that being " & s & "."
	If i > 1 Then MsgBox i & " names deleted, including " & s & "."
End Sub  ' RangeNames_DeleteBeginningWith


Sub RangeNames_DeleteContaining()
	Dim n As Name, NameContains As String, s As String, i As Integer

	NameContains = InputBox( _
		prompt:="Delete all range names beginning with what string? ", _
		Title:="Starting characters of range names to be deleted", Default:="")

	If Len(NameContains) = 0 Then Exit Sub

	i = 0
	For Each n In ActiveWorkbook.Names
		If InStr(1, n.Name, NameContains, vbBinaryCompare) > 0 Then s = n.Name: i = i + 1: n.Delete
	Next n
	If i = 0 Then MsgBox "No names deleted."
	If i = 1 Then MsgBox "One name deleted, that being " & s & "."
	If i > 1 Then MsgBox i & " names deleted, including " & s & "."
End Sub  ' RangeNames_DeleteContaining



Sub RangeNames_DeDuplicate()
	Dim nm As Name, nms As Variant, exists As Variant, i As Integer, j As Integer, str As String, ans As Integer
	Set nms = ActiveWorkbook.Names
	For Each nm In ActiveWorkbook.Names
		If Left(nm.RefersTo, 6) = "=#REF!" _
		Or Right(nm.RefersTo, 6) = "!#REF!" _
		Or Right(nm.RefersTo, 7) = "!#REF!#" Then
			nm.Delete
		End If  ' #REF!
	Next nm

	ReDim nms(1 To ActiveWorkbook.Names.Count) As Name, exists(1 To ActiveWorkbook.Names.Count) As Boolean
	For i = 1 To UBound(exists)
		Set nms(i) = ActiveWorkbook.Names(i)
		exists(i) = True
	Next i

	For i = 2 To UBound(nms)
		If exists(i) Then
			If nms(i).Visible Then
				For j = 1 To i - 1
					If exists(j) Then
						If nms(j).Visible Then
							If nms(i) = nms(j) Then
								On Error Resume Next
								nms(i).RefersToRange.Parent.Activate
								nms(i).RefersToRange.Select
								On Error GoTo 0
								str = "Yes = delete " & nms(i).Name & ";" _
									& Chr(10) & "No = delete " & nms(j).Name & ";" _
									& Chr(10) & "Cancel = keep both."
								ans = MsgBox(str, 3, "Duplicate name. Delete either?")
								If ans = 6 Then  ' Yes
									nms(i).Delete: exists(i) = False: Exit For
								ElseIf ans = 7 Then  ' No
									nms(j).Delete: exists(j) = False
								End If  ' ans = 6
							End If  ' Duplicate?
						End If  ' nms(j).Visible
					End If  ' exists(j)
				Next j
			End If  ' nms(i).Visible
		End If  ' exists(i)
	Next i
End Sub  ' RangeNames_DeDuplicate



Sub RangeNamesPenrose_Create()
	Dim rowNum As Long, rowNumMax As Long
	Dim thisStr As String
	Dim underscorePosn As Integer
	Dim wks As Worksheet

	Set wks = ThisWorkbook.ActiveSheet

	rowNumMax = wks.UsedRange.Cells(wks.UsedRange.Rows.Count, 1).Row
	rowNum = 1
	Do
		Do  ' Allows 'Exit Do' to be the equivalent of C-style 'continue'.
			If Application.WorksheetFunction.IsNonText(wks.Cells(rowNum, 1)) Then Exit Do
			thisStr = wks.Cells(rowNum, 1).Value

			If "#" = Left(thisStr, 1) Then
				With wks.Cells(rowNum, 1).EntireRow.Interior
					.Pattern = xlSolid
					.Color = vbCyan
					.TintAndShade = 0
					.PatternTintAndShade = 0
				End With
				Exit Do
			End If  ' #

			underscorePosn = InStr(1, thisStr, "_")
			If underscorePosn <= 1 Then Exit Do
			If Len(thisStr) < underscorePosn + 4 Then Exit Do
			If Mid(thisStr, 1 + underscorePosn, 1) < "0" Then Exit Do
			If Mid(thisStr, 1 + underscorePosn, 1) > "9" Then Exit Do
			If Mid(thisStr, 2 + underscorePosn, 1) < "0" Then Exit Do
			If Mid(thisStr, 2 + underscorePosn, 1) > "9" Then Exit Do
			If Mid(thisStr, 3 + underscorePosn, 1) <> "." Then Exit Do

			wks.Cells(rowNum, 1).Select
			Range(Selection, Selection.End(xlToRight)).Select
			With Selection.Borders(xlEdgeBottom)
				.Color = vbBlack
				.Weight = xlMedium
				.LineStyle = xlContinuous
				.TintAndShade = 0
			End With  ' Selection.Borders(xlEdgeBottom)

			wks.Cells(rowNum, 1).Select
			Range(Selection, Selection.End(xlDown)).Select
			Range(Selection, Selection.End(xlToRight)).Select
			Selection.CreateNames Top:=True, Left:=False, bottom:=False, Right:=False

			rowNum = Selection.Rows.Count + rowNum
		Loop While False  ' Allows 'Exit Do' to be the equivalent of C-style 'continue'.
		rowNum = 1 + rowNum
	Loop While rowNum <= rowNumMax
	wks.Cells(1, 1).Activate: wks.Cells(1, 1).Select
End Sub  ' RangeNamesPenrose_Create()
