Option Explicit

' To go in Module 'modChartAxis'
' Source: https://github.com/jdaw1/chart_markers
' Or: https://github.com/jdaw1/penrose_tiling/vba


' Excel's built-in chart scaling is terrible, it being far too keen to include 0.
' Compute your own Min Max Major Minor, and this will do the work.
' Two strings specify chart; two Booleans specify axis; four numbers are the values.
' To make Min and Max use Excel's automatic, set minScale >= maxScale.
' To make Major and Minor use Excel's automatic, set minorStep > majorStep; or set either to <= 0.
Function Axis_Scale( _
	tabName As String, chartName As String, _
	xAxis As Boolean, primaryAxis As Boolean, _
	minScale As Double, maxScale As Double, majorStep As Double, minorStep As Double _
) As Double
	Dim ch As Chart
	If Len(tabName) > 0 Then
		Set ch = ThisWorkbook.Sheets(tabName).ChartObjects(chartName).Chart
	Else
		Set ch = ThisWorkbook.Charts(chartName)
	End If  ' Len(tabName) > 0
	With IIf(xAxis, _
		ch.Axes(xlCategory, IIf(primaryAxis, xlPrimary, xlSecondary)), _
		ch.Axes(xlValue, IIf(primaryAxis, xlPrimary, xlSecondary)) _
	)
		.MinimumScaleIsAuto = True
		.MaximumScaleIsAuto = True  ' In case of awkwardnesses such as New Min > Old Max
		.MajorUnitIsAuto = True
		.MinorUnitIsAuto = True  ' In case of awkwardnesses such as New Major < Old Minor
		If minScale < maxScale Then
			.MinimumScaleIsAuto = False: .MinimumScale = minScale
			.MaximumScaleIsAuto = False: .MaximumScale = maxScale
		End If  ' minScale < maxScale
		If minorStep <= majorStep Then
			If majorStep > 0 Then .MajorUnitIsAuto = False: .MajorUnit = majorStep
			If minorStep > 0 Then .MinorUnitIsAuto = False: .MinorUnit = minorStep
		End If  ' minorStep <= majorStep
	End With  ' Axis

	Axis_Scale = Now
End Function  ' Axis_Scale
