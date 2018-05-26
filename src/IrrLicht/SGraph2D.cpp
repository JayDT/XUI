/*
Simple Graph
(c) Nicolaus Anderson
Created Jan 7, 2013

License: Same terms as irrlicht
*/

#include "SGraph2D.h"

#ifndef __SIMPLEGRAPH2D_CPP__
#define __SIMPLEGRAPH2D_CPP__

void irr::gui::SGraph2D::setGraphSize( irr::core::rectf& size )
{
	// Change the points so they can be drawn correctly on the new window
	changeGraphWindow( size );

	ReallocateGraphSpace();
}

void irr::gui::SGraph2D::setGraphSizeX( irr::f32 size, bool isMax )
{
	irr::core::rectf new_window(window);

	if ( isMax )
	{
		new_window.LowerRightCorner.X = size;
	} else {
		new_window.UpperLeftCorner.X = size;
	}

	new_window.repair();

	// Change the points so they can be drawn correctly on the new window
	changeGraphWindow( new_window );

	ReallocateGraphSpace();
}

void irr::gui::SGraph2D::setGraphSizeY( irr::f32 size, bool isMax )
{
	irr::core::rectf new_window(window);

	if ( isMax )
	{
		new_window.UpperLeftCorner.Y = size;
	} else {
		new_window.LowerRightCorner.Y = size;
	}

	new_window.repair();

	// Change the points so they can be drawn correctly on the new window
	changeGraphWindow( new_window );

	ReallocateGraphSpace();
}

void irr::gui::SGraph2D::setGraphScale( irr::core::rectf& scale )
{
	irr::core::rectf new_window(window);

	new_window.UpperLeftCorner.X *= scale.UpperLeftCorner.X;
	new_window.UpperLeftCorner.Y *= scale.UpperLeftCorner.Y;
	new_window.LowerRightCorner.X *= scale.LowerRightCorner.X;
	new_window.LowerRightCorner.Y *= scale.LowerRightCorner.Y;

	new_window.repair();

	// Change the points so they can be drawn correctly on the new window
	changeGraphWindow( new_window );

	ReallocateGraphSpace();
}

void irr::gui::SGraph2D::ReallocateGraphSpace()
{
	graphImage.reallocate(
		(irr::u32)AbsoluteRect.getWidth(),
		false
		);
}

void irr::gui::SGraph2D::changeGraphWindow( irr::core::rectf new_win )
{
	irr::core::position2df point;

	// Save the ratios - for convenience
	irr::f32 winARx = window.getWidth() / (f32)AbsoluteRect.getWidth();
	irr::f32 winARy = window.getHeight() / (f32)AbsoluteRect.getHeight();
	irr::f32 ARnwinx = (f32)AbsoluteRect.getWidth() / new_win.getWidth();
	irr::f32 ARnwiny = (f32)AbsoluteRect.getHeight() / new_win.getHeight();

	// Restore all points with new values in the new window
	for ( irr::u32 p=0; p < graphImage.size(); p++ )
	{
		point.X =
			(
				// shift from the old graph window (same as in first loop)
				graphImage[p].Pos.X * winARx
				+ window.UpperLeftCorner.X

				-new_win.UpperLeftCorner.X // shift to new origin
			)
			* ARnwinx; // change to irrlicht coordinate scale

		point.Y =
			(
				// shift from the old graph window (same as in first loop)
				graphImage[p].Pos.Y * winARy
				+ window.UpperLeftCorner.Y

				-new_win.UpperLeftCorner.Y // shift to new origin
			)
			* ARnwiny; // change to irrlicht coordinate scale

		// Overwrite the old point
		graphImage[p].Pos.X = point.X;
		graphImage[p].Pos.Y = point.Y;
	}

	// Overwrite the original window
	window = new_win;
}

irr::core::rectf irr::gui::SGraph2D::getGraphSize()
{
	return window;
}

Range<irr::f32> irr::gui::SGraph2D::getXAxisRange()
{
	return Range<irr::f32>(
				window.UpperLeftCorner.X,
				window.LowerRightCorner.X
				);
}

Range<irr::f32> irr::gui::SGraph2D::getYAxisRange()
{
	return Range<irr::f32>(
				window.UpperLeftCorner.Y,
				window.LowerRightCorner.Y
				);
}

Inc<irr::f32> irr::gui::SGraph2D::getIterableXRange()
{
	Inc<irr::f32> inc(CYC_REPEAT);

	inc.setRange( getXAxisRange() );
	inc.setStep( window.getWidth() / ((irr::f32)AbsoluteRect.getWidth()) );
	inc.restart();

	return inc;
}

Inc<irr::f32> irr::gui::SGraph2D::getIterableYRange()
{
	Inc<irr::f32> inc(CYC_REPEAT);

	inc.setRange( getYAxisRange() );
	inc.setStep( window.getHeight() / ((irr::f32)AbsoluteRect.getHeight()) );
	inc.restart();

	return inc;
}

void irr::gui::SGraph2D::markerSpacingX( irr::f32 gap )
{
	markXgap = gap;
}

void irr::gui::SGraph2D::markerSpacingY( irr::f32 gap )
{
	markYgap = gap;
}

void irr::gui::SGraph2D::draw()
{
	// Don't bother doing anything if this isn't visible
	if ( !IsVisible || AbsoluteRect.getArea() == 0 || window.getArea() == 0.0f )
		return;

	// variables...

	irr::core::vector2di point; /* point to be drawn on screen representing
								a point from the MathFunc */

		/* Marker offset
		- used to ensure the markers are drawn from the center outward. */
	Inc<irr::f32> markerIter;	/* no wrapping because we want
								out-of-bounds checking */
		// Marker line - drawn on the GUI graph screen
	irr::core::line2df marker;

		// For marker labels (drawn if desired)
	irr::core::stringw label;
	irr::core::position2df label_pos;


	// operations...

	// Draw the background if there is one
	if ( hasBackground )
	{
		viddriver->draw2DRectangle(
							background_color,
							AbsoluteRect,
							&AbsoluteClippingRect
							);
	}

	// Draw the axis if desired

	if ( showXaxis )
	{
		drawToGUI(
			irr::core::line2df(
				window.UpperLeftCorner.X,
				0.0f,
				window.LowerRightCorner.X,
				0.0f
				),
			xaxis_color
			);
	}

	if ( showYaxis )
	{
		drawToGUI(
			irr::core::line2df(
				0.0f,
				window.UpperLeftCorner.Y,
				0.0f,
				window.LowerRightCorner.Y
				),
			yaxis_color
			);
	}

	// Draw the markers/lines if desired
	if ( UseMarkers )
	{
			// Y-axis
		if ( showYaxisMarks )
		{
			/* Set the offset for making the lines appear to be drawn
			from the center outward */
			markerIter.setMin( window.UpperLeftCorner.Y );
			markerIter.setMax( window.LowerRightCorner.Y );
			markerIter.setStep( markYgap );
			markerIter.setVal( 0.0f ); // start in the center

			// Set up the marker line

				// Left side of the line
			if ( UseTicks )
				marker.start.X = -window.getWidth() / 40.0f; // for 5% window width
			else
				marker.start.X = window.UpperLeftCorner.X;

				// Right side of the line
			if ( UseTicks )
				marker.end.X = window.getWidth() / 40.0f; // for 5% window width
			else
				marker.end.X = window.LowerRightCorner.X;

			// Draw each marker
			while ( !++markerIter ) // go until past the max
			{
				// Assign the position to the line to draw
				marker.start.Y = marker.end.Y = markerIter.Val();

				// Draw the marker to the GUI
				drawToGUI( marker, ymark_color );

				// Write the axis value if desired
				if ( UseYTickLabels )
				{
					label = irr::core::stringw( (irr::s32)markerIter.Val() );

					label_pos.X = marker.end.X + window.getWidth()/50;
					label_pos.Y = markerIter.Val();

					drawToGUI( label, label_pos, ymark_color, false, true );
				}
			}

			// Restart at the center
			markerIter = 0.0f;

			// Draw each marker
			while ( !--markerIter ) // go until past the min
			{
				// Assign the position to the line to draw
				marker.start.Y = markerIter.Val();
				marker.end.Y = markerIter.Val();

				// Draw the marker to the GUI
				drawToGUI( marker, ymark_color );

				// Write the axis value if desired
				if ( UseYTickLabels )
				{
					label = irr::core::stringw( (irr::s32)markerIter.Val() );

					label_pos.X = marker.end.X + window.getWidth()/50;
					label_pos.Y = markerIter.Val();

					drawToGUI( label, label_pos, ymark_color, false, true );
				}
			}
		}

			// X-axis
		if ( showXaxisMarks )
		{
			/* Set the offset for making the lines appear to be drawn
			from the center outward */
			markerIter.setMin( window.UpperLeftCorner.X );
			markerIter.setMax( window.LowerRightCorner.X );
			markerIter.setStep( markXgap );
			markerIter.setVal( 0.0f ); // start in the center

			// Set up the marker line

				// Top of the line - Below the x-axis in terms of irrlicht drawing
			if ( UseTicks )
				marker.start.Y = window.getHeight() / 40.0f; // for 5% window height
			else
				marker.start.Y = window.UpperLeftCorner.Y;

				// Bottom of the line - Above the x-axis in terms of irrlicht drawing
			if ( UseTicks )
				marker.end.Y = -window.getHeight() / 40.0f; // for 5% window height
			else
				marker.end.Y = window.LowerRightCorner.Y;

			// Draw each marker
			while ( !++markerIter ) // go until past the max
			{
				// Assign the position to the line to draw
				marker.start.X = marker.end.X = markerIter.Val();

				// Draw the marker to the GUI
				drawToGUI( marker, xmark_color );

				// Write the axis value if desired
				if ( UseXTickLabels )
				{
					label = irr::core::stringw( (irr::s32)markerIter.Val() );

					label_pos.X = markerIter.Val();
					label_pos.Y = marker.start.Y + window.getHeight()/50;

					drawToGUI( label, label_pos, xmark_color, false, true );
				}
			}

			// Restart at the center
			markerIter = 0.0f;

			// Draw each marker
			while ( !--markerIter ) // go until past the min
			{
				// Assign the position to the line to draw
				marker.start.X = markerIter.Val();
				marker.end.X = markerIter.Val();

				// Draw the marker to the GUI
				drawToGUI( marker, xmark_color );

				// Write the axis value if desired
				if ( UseXTickLabels )
				{
					label = irr::core::stringw( (irr::s32)markerIter.Val() );

					label_pos.X = markerIter.Val();
					label_pos.Y = marker.start.Y + window.getHeight()/50;

					drawToGUI( label, label_pos, xmark_color, false, true );
				}
			}
		}
	}

	// Display each points on screen
	for (
		irr::s32 pt = 0;
		pt < (irr::s32)graphImage.size();
		pt++
		)
	{
		point.X = (irr::s32)graphImage[pt].Pos.X;
		point.Y = (irr::s32)graphImage[pt].Pos.Y;

		drawToGUI( point, graphImage[pt].Color );
	}
}

void irr::gui::SGraph2D::clearGraph()
{
	graphImage.clear();
}

void irr::gui::SGraph2D::autoAdjust()
{
	/* Since all of the data points were stored in irrlicht coordinates
	system SCALE (in order to draw them faster) (NOT including offset)
	they must be converted back to the graph window coordinate system
	for the graph to be rescaled to fit them. This can be a costly operation,
	(depending on the number of points saved to the graph) so this function
	should be called only once - after all of the points have been added. */

	/* Operations require that both windows - GUI and graph - exist,
	otherwise we get a 1/0 errore
	Furthermore, there must be points to graph. */
	if ( window.getArea() == 0 || AbsoluteRect.getArea() == 0
		|| graphImage.size() == 0 )
		return;

	// Temporary vertex for passing data
	irr::core::position2df point;

	// Temporary rectangle - for creating the new window
	irr::core::rectf new_window;


	// Save the ratios - for convenience
	irr::f32 winARx = window.getWidth() / (f32)AbsoluteRect.getWidth();
	irr::f32 winARy = window.getHeight() / (f32)AbsoluteRect.getHeight();

	// First point determines start
	/* This is done because the points may not actually reside anywhere
	near the origin. */
	new_window.UpperLeftCorner.X =
			graphImage[0].Pos.X * winARx // change scale
			+ window.UpperLeftCorner.X; // shift from corner to origin
	new_window.LowerRightCorner.X =
			graphImage[0].Pos.X * winARx // change scale
			+ window.UpperLeftCorner.X; // shift from corner to origin
	new_window.UpperLeftCorner.Y =
			graphImage[0].Pos.Y * winARy // change scale
			+ window.UpperLeftCorner.Y; // shift from corner to origin
	new_window.LowerRightCorner.Y =
			graphImage[0].Pos.Y * winARy // change scale
			+ window.UpperLeftCorner.Y; // shift from corner to origin

	// Generate the new window
	for ( irr::u32 p=1; p < graphImage.size(); p++ )
	{
		point.X =
			graphImage[p].Pos.X * winARx // change scale
			+ window.UpperLeftCorner.X; // shift from corner to origin
		point.Y =
			graphImage[p].Pos.Y * winARy // change scale
			+ window.UpperLeftCorner.Y; // shift from corner to origin

		new_window.addInternalPoint( point );
	}

	changeGraphWindow( new_window );
}

void irr::gui::SGraph2D::setPointClipping( bool yes )
{
	clipPoints = yes;
}

void irr::gui::SGraph2D::useBackground( bool use )
{
	hasBackground = use;
}

void irr::gui::SGraph2D::setBackgroundColor( irr::video::SColor color )
{
	background_color = color;
}

void irr::gui::SGraph2D::setPointColor( irr::video::SColor color )
{
	point_color = color;
}

void irr::gui::SGraph2D::setXAxisColor( irr::video::SColor color )
{
	xaxis_color = color;
}

void irr::gui::SGraph2D::setYAxisColor( irr::video::SColor color )
{
	yaxis_color = color;
}

void irr::gui::SGraph2D::setShowXAxis( bool show )
{
	showXaxis = show;
}

void irr::gui::SGraph2D::setShowYAxis( bool show )
{
	showYaxis = show;
}

void irr::gui::SGraph2D::usePolygons( irr::f32 radius, irr::s32 corners )
{
	usePolyPts = true;
	polyRadius = radius;
	polyPts = corners;
}

void irr::gui::SGraph2D::useNoPolygons()
{
	usePolyPts = false;
}

void irr::gui::SGraph2D::showXAxisLabels( bool yes )
{
	UseXTickLabels = yes;
}

void irr::gui::SGraph2D::showYAxisLabels( bool yes )
{
	UseYTickLabels = yes;
}

void irr::gui::SGraph2D::useTicks()
{
	UseMarkers = true;
	UseTicks = true;

	showXaxisMarks = true;
	showYaxisMarks = true;
}

void irr::gui::SGraph2D::useLines()
{
	UseMarkers = true;
	UseTicks = false;

	showXaxisMarks = true;
	showYaxisMarks = true;
}

void irr::gui::SGraph2D::useNoMarkers()
{
	UseMarkers = false;
}

void irr::gui::SGraph2D::showXAxisMarkers( bool show )
{
	showXaxisMarks = show;
	if (show) UseMarkers = true;
}

void irr::gui::SGraph2D::showYAxisMarkers( bool show )
{
	showYaxisMarks = show;
	if (show) UseMarkers = true;
}

void irr::gui::SGraph2D::setXAxisMarkerColor( irr::video::SColor color )
{
	xmark_color = color;
}

void irr::gui::SGraph2D::setYAxisMarkerColor( irr::video::SColor color )
{
	ymark_color = color;
}

void irr::gui::SGraph2D::serializeAttributes(
	irr::io::IAttributes *out,
	irr::io::SAttributeReadWriteOptions *options
	) const
{
	IGUIElement::serializeAttributes( out, options );

	irr::core::recti win;
	win.UpperLeftCorner.X = (irr::s32)(window.UpperLeftCorner.X);
	win.UpperLeftCorner.Y = (irr::s32)(window.UpperLeftCorner.Y);
	win.LowerRightCorner.X = (irr::s32)(window.LowerRightCorner.X);
	win.LowerRightCorner.Y = (irr::s32)(window.LowerRightCorner.Y);

	out->addRect( "Window", win );

	out->addBool( "FillBackground", hasBackground );
	out->addColor( "BGColor", background_color );
	out->addColor( "XAxisColor", xaxis_color );
	out->addColor( "YAxisColor", yaxis_color );
	out->addColor( "PointColor", point_color );
	out->addColor( "XAxisTickColor", xmark_color );
	out->addColor( "YAxisTickColor", ymark_color );

	out->addBool( "UseMarkers", UseMarkers );
	out->addBool( "UseTicks", UseTicks );
	out->addFloat( "MarkerXSpacing", markXgap );
	out->addFloat( "MarkerYSpacing", markYgap );

	out->addBool( "ShowXAxis", showXaxis );
	out->addBool( "ShowYAxis", showYaxis );

	out->addBool( "ShowXAxisLabels", UseXTickLabels );
	out->addBool( "ShowYAxisLabels", UseYTickLabels );

	out->addBool( "ClipDrawingRegion", clipPoints );

	out->addBool( "UsePolygons", usePolyPts );
	out->addFloat( "PolygonRadius", polyRadius );
	out->addInt( "PolygonVertices", polyPts );
}

void irr::gui::SGraph2D::deserializeAttributes(
	irr::io::IAttributes *in,
	irr::io::SAttributeReadWriteOptions *options
	)
{
	IGUIElement::deserializeAttributes( in, options );

	irr::core::recti win = in->getAttributeAsRect( "Window" );
	window.UpperLeftCorner.X = (irr::f32)(win.UpperLeftCorner.X);
	window.UpperLeftCorner.Y = (irr::f32)(win.UpperLeftCorner.Y);
	window.LowerRightCorner.X = (irr::f32)(win.LowerRightCorner.X);
	window.LowerRightCorner.Y = (irr::f32)(win.LowerRightCorner.Y);

	hasBackground		=	in->getAttributeAsBool( "FillBackground" );
	background_color	=	in->getAttributeAsColor( "BGColor" );
	xaxis_color			=	in->getAttributeAsColor( "XAxisColor" );
	yaxis_color			=	in->getAttributeAsColor( "YAxisColor" );
	point_color			=	in->getAttributeAsColor( "PointColor" );
	xmark_color			=	in->getAttributeAsColor( "XAxisTickColor" );
	ymark_color			=	in->getAttributeAsColor( "YAxisTickColor" );

	UseMarkers			=	in->getAttributeAsBool( "UseMarkers" );
	UseTicks			=	in->getAttributeAsBool( "UseTicks" );
	markXgap			=	in->getAttributeAsFloat( "MarkerXSpacing" );
	markYgap			=	in->getAttributeAsFloat( "MarkerYSpacing" );

	showXaxis			=	in->getAttributeAsBool( "ShowXAxis" );
	showYaxis			=	in->getAttributeAsBool( "ShowYAxis" );

	UseXTickLabels		=	in->getAttributeAsBool( "ShowXAxisLabels" );
	UseYTickLabels		=	in->getAttributeAsBool( "ShowYAxisLabels" );

	clipPoints			=	in->getAttributeAsBool( "ClipDrawingRegion" );

	usePolyPts			=	in->getAttributeAsBool( "UsePolygons" );
	polyRadius			=	in->getAttributeAsFloat( "PolygonRadius" );
	polyPts				=	in->getAttributeAsInt( "PolygonVertices" );
}

void irr::gui::SGraph2D::drawOnGraph(
	irr::core::vector2df point, irr::video::SColor color
	)
{
	// Cannot save if the window does not exist
	if ( window.getArea() == 0 )
		return;

	// Shift the origin to the corner
	point.X -= window.UpperLeftCorner.X;
	point.Y -= window.UpperLeftCorner.Y;

	// Convert to the actual GUI window's coordinate system
	point.X *= ((irr::f32)AbsoluteRect.getWidth()) / window.getWidth();
	point.Y *= ((irr::f32)AbsoluteRect.getHeight()) / window.getHeight();

	// Save
	graphImage.push_back(
		irr::video::S3DVertex( point.X, point.Y, 0,0,0,0, color, 0,0)
		);
}

void irr::gui::SGraph2D::drawOnGraph(
		irr::f32 x,
		irr::f32 y,
		irr::video::SColor color
		)
{
	drawOnGraph( irr::core::vector2df(x,y), color );
}

void irr::gui::SGraph2D::drawOnGraphPolar(
		irr::core::vector2df point,
		irr::video::SColor color
		)
{
	// cartesian coordinates
	irr::core::vector2df cartesian;

	cartesian.X = point.X // radius
		* cos( point.Y * irr::core::DEGTORAD ); // cos(angle)

	cartesian.Y = point.X // radius
		* sin( point.Y * irr::core::DEGTORAD ); // cos(angle)

	drawOnGraph( cartesian, color );
}

void irr::gui::SGraph2D::drawOnGraphPolar(
		irr::f32 radius,
		irr::f32 angle,
		irr::video::SColor color
		)
{
	drawOnGraphPolar( irr::core::vector2df( radius, angle ), color );
}

void irr::gui::SGraph2D::drawRawPoint(
	irr::core::vector2df point,
	irr::video::SColor color
	)
{
	// Shift the origin to the corner
	point.X -= window.UpperLeftCorner.X
		 // conversion of shift to GUI element size
		* ((irr::f32)AbsoluteRect.getWidth()) / window.getWidth();

	point.Y -= window.UpperLeftCorner.Y
		// conversion of shift to GUI element size
		* ((irr::f32)AbsoluteRect.getHeight()) / window.getHeight();

	// Save
	graphImage.push_back(
		irr::video::S3DVertex( point.X, point.Y, 0,0,0,0, color, 0,0)
		);
}

void irr::gui::SGraph2D::drawToGUI(
						irr::core::vector2di point,
						irr::video::SColor color
						)
{
	// Flip for drawing on the GUI
	point.Y = AbsoluteRect.getHeight() - point.Y;

	/* This function has been passed a value that is already prepared to
	be drawn to screen and simply needs to be offset. */
	point.X += AbsoluteRect.UpperLeftCorner.X;
	point.Y += AbsoluteRect.UpperLeftCorner.Y;

	// Don't draw points outside the window
	if ( clipPoints && !AbsoluteClippingRect.isPointInside( point ) )
		return;

	// Do nothing if the window isn't even visible
	if ( window.getArea() == 0.0f )
		return;

	// Draw the point as a pixel - might be very small and hard to see
	/* - Drawn faster than a polygon and may create a smooth line (if MathFunc
	returns values of a smooth function) */
	viddriver->drawPixel( point.X, point.Y, color );

	// Attempt to draw the point as a polygon (to make it more visible)
	if ( usePolyPts && polyRadius >= 1.0f )
		viddriver->draw2DPolygon(
						point,			// position
						polyRadius,		// radius
						color,			// color
						polyPts			// roundness of the point
						);

}

void irr::gui::SGraph2D::drawToGUI(
						irr::core::line2df line,
						irr::video::SColor color
						)
{
	// Do nothing if the window isn't even visible
	// - saves time and prevents divide-by-zero errors
	if ( window.getArea() == 0.0f )
		return;

	/* This function has been passed a value that needs to be prepared
	to fit in the window in addition to it being offset. */
	
	// Line that will be drawn - initialized to begin within the GUI element
	irr::core::line2di drawline(
		AbsoluteRect.UpperLeftCorner.X,
		AbsoluteRect.UpperLeftCorner.Y,
		AbsoluteRect.UpperLeftCorner.X,
		AbsoluteRect.UpperLeftCorner.Y
		);

	// Tranform the line from graph coordinates.
	line.start.X -= window.UpperLeftCorner.X;
	line.end.X -= window.UpperLeftCorner.X;

		// Flip the y-axis for drawing in irrlicht coordinates
	line.start.Y = window.LowerRightCorner.Y - line.start.Y;
	line.end.Y = window.LowerRightCorner.Y - line.end.Y;

	// starting x
	drawline.start.X += (irr::s32) ( line.start.X
		* AbsoluteRect.getWidth() / window.getWidth() // coordinate system conversion
		);

	// starting y
	drawline.start.Y += (irr::s32) ( line.start.Y
		* AbsoluteRect.getHeight() / window.getHeight() // coordinate system conversion
		);

	// ending x
	drawline.end.X += (irr::s32) ( line.end.X
		* AbsoluteRect.getWidth() / window.getWidth() // coordinate system conversion
		);

	// ending y
	drawline.end.Y += (irr::s32) ( line.end.Y
		* AbsoluteRect.getHeight() / window.getHeight() // coordinate system conversion
		);


	// Draw the line
	viddriver->draw2DLine( drawline.start, drawline.end, color );
}


void irr::gui::SGraph2D::drawToGUI(
		irr::core::stringw text,
		irr::core::position2df pos,
		irr::video::SColor color,
		bool horiz_centering,
		bool vert_centering
		)
{
	irr::core::recti fin_pos(AbsoluteRect);
	fin_pos.LowerRightCorner = fin_pos.UpperLeftCorner;

	// Shift center
	pos.X -= window.UpperLeftCorner.X;
	pos.Y = window.UpperLeftCorner.Y - pos.Y + window.getHeight(); // Flip the y-axis

	// coordinate system conversion
	pos.X *= (irr::f32)AbsoluteRect.getWidth() / window.getWidth();
	pos.Y *= (irr::f32)AbsoluteRect.getHeight() / window.getHeight();

	// Move onto the graph (in irrlicht)
	fin_pos += irr::core::position2di( (irr::s32)pos.X, (irr::s32)pos.Y );

	// Write the text
	Environment->getSkin()->getFont()->draw(
						text,
						fin_pos,
						color,
						horiz_centering,
						vert_centering,
						&AbsoluteClippingRect
						);
}


#endif // define __SIMPLEGRAPH2D_CPP__