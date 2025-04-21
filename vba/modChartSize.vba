Option Explicit

Function PlotArea_WidthHeight(sheetName As String, chartName As String) As Variant
	Dim ch As Chart
	Dim ans(1 To 2) As Double
	Set ch = ThisWorkbook.Sheets(sheetName).ChartObjects(chartName).Chart
	ans(1) = ch.PlotArea.Width
	ans(2) = ch.PlotArea.Height
	PlotArea_WidthHeight = ans
End Function  ' PlotArea_WidthHeight()
