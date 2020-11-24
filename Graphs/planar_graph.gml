Creator "ogdf::GraphIO::writeGML"
graph
[
	directed	1
	node
	[
		id	0
		label	"0"
		graphics
		[
			x	75.0000000000
			y	40.0000000000
			w	20.0000000000
			h	20.0000000000
			fill	"#FFFF00"
			fillbg	"#000000"
			outline	"#000000"
			pattern	"Solid"
			stipple	"Solid"
			lineWidth	1.0000000000
			type	"Rect"
		]
	]
	node
	[
		id	1
		label	"1"
		graphics
		[
			x	45.0000000000
			y	140.0000000000
			w	20.0000000000
			h	20.0000000000
			fill	"#FFFF00"
			fillbg	"#000000"
			outline	"#000000"
			pattern	"Solid"
			stipple	"Solid"
			lineWidth	1.0000000000
			type	"Rect"
		]
	]
	node
	[
		id	2
		label	"2"
		graphics
		[
			x	90.0000000000
			y	140.0000000000
			w	20.0000000000
			h	20.0000000000
			fill	"#FFFF00"
			fillbg	"#000000"
			outline	"#000000"
			pattern	"Solid"
			stipple	"Solid"
			lineWidth	1.0000000000
			type	"Rect"
		]
	]
	node
	[
		id	3
		label	"3"
		graphics
		[
			x	60.0000000000
			y	90.0000000000
			w	20.0000000000
			h	20.0000000000
			fill	"#FFFF00"
			fillbg	"#000000"
			outline	"#000000"
			pattern	"Solid"
			stipple	"Solid"
			lineWidth	1.0000000000
			type	"Rect"
		]
	]
	node
	[
		id	4
		label	"4"
		graphics
		[
			x	120.0000000000
			y	90.0000000000
			w	20.0000000000
			h	20.0000000000
			fill	"#FFFF00"
			fillbg	"#000000"
			outline	"#000000"
			pattern	"Solid"
			stipple	"Solid"
			lineWidth	1.0000000000
			type	"Rect"
		]
	]
	edge
	[
		source	1
		target	0
		generalization	0
		graphics
		[
			type	"line"
			arrow	"none"
			stipple	"Solid"
			lineWidth	1.0000000000
			Line [
				point [ x 45.0000000000 y 140.0000000000 ]
				point [ x 30.0000000000 y 90.0000000000 ]
				point [ x 75.0000000000 y 40.0000000000 ]
			]
			fill "#0000FF"
		]
	]
	edge
	[
		source	2
		target	0
		generalization	0
		graphics
		[
			type	"line"
			arrow	"none"
			stipple	"Solid"
			lineWidth	1.0000000000
			Line [
				point [ x 90.0000000000 y 140.0000000000 ]
				point [ x 90.0000000000 y 90.0000000000 ]
				point [ x 75.0000000000 y 40.0000000000 ]
			]
			fill "#0000FF"
		]
	]
	edge
	[
		source	3
		target	2
		generalization	0
		graphics
		[
			type	"line"
			arrow	"none"
			stipple	"Solid"
			lineWidth	1.0000000000
			fill "#0000FF"
		]
	]
	edge
	[
		source	4
		target	2
		generalization	0
		graphics
		[
			type	"line"
			arrow	"none"
			stipple	"Solid"
			lineWidth	1.0000000000
			fill "#0000FF"
		]
	]
	edge
	[
		source	0
		target	3
		generalization	0
		graphics
		[
			type	"line"
			arrow	"none"
			stipple	"Solid"
			lineWidth	1.0000000000
			fill "#0000FF"
		]
	]
	edge
	[
		source	3
		target	1
		generalization	0
		graphics
		[
			type	"line"
			arrow	"none"
			stipple	"Solid"
			lineWidth	1.0000000000
			fill "#0000FF"
		]
	]
	edge
	[
		source	0
		target	4
		generalization	0
		graphics
		[
			type	"line"
			arrow	"none"
			stipple	"Solid"
			lineWidth	1.0000000000
			fill "#0000FF"
		]
	]
]
