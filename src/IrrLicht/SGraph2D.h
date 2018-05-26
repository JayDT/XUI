/*
Simple Graph
(c) Nicolaus Anderson
Created Jan 7, 2013

License: Same terms as irrlicht
*/

#ifndef __SIMPLEGRAPH2D_H__
#define __SIMPLEGRAPH2D_H__

#include "IGraph2D.h"
#include "Range.h"
#include "IncrementorT.h"

namespace irr
{
namespace gui
{

//! Class Graph 2D
/*
Purpose: To plot 2D math functions.
*/
class SGraph2D : public IGraph2D
{
		//! Graph Image
	/* The actual graph itself: a chain of points that need to be
	drawn. */
	irr::core::array<irr::video::S3DVertex> graphImage;

		//! Graph window
	/* This is the range of values that the graphed values are forced
	to fit into. It is a user-set range. */
	irr::core::rectf window;

		//! Markers (e.g. tick marks)
	bool UseMarkers;	// whether to use markers or not
	bool UseTicks;		// whether to use ticks or lines (if markers are in use)
	bool UseXTickLabels;		// whether to write labels for the x-axis tick marks
	bool UseYTickLabels;		// whether to write labels for the y-axis tick marks
	irr::f32 markXgap;	// spacing between markings on the x-axis
	irr::f32 markYgap;	// spacing between markings on the y-axis
	bool showXaxisMarks;	// whether the x-axis marks should be displayed
	bool showYaxisMarks;	// whether the y-axis marks should be displayed

		//! Axis displaying flags
	bool showXaxis;
	bool showYaxis;

		//! Clip points - i.e. Don't draw points outside the graph window
	bool clipPoints;

		// Polygon point attributes
	bool usePolyPts;		// Use polygon points
	irr::f32 polyRadius;	// Radius of the polygon points
	irr::s32 polyPts;		// Number of points in the polygon


		//! Colors

			// Background color
	irr::video::SColor background_color;
	bool hasBackground;

			// Axis colors
	irr::video::SColor xaxis_color;
	irr::video::SColor yaxis_color;

			// Point color
	irr::video::SColor point_color;

			// Tick marks / lines perpendicular to the axis
	irr::video::SColor xmark_color;
	irr::video::SColor ymark_color;


	// The video driver for drawing on screen
	irr::video::IVideoDriver* viddriver;

public:

		//! Constructor
	SGraph2D(
				// GUI element parameters
			IGUIEnvironment* envir,
			IGUIElement* parent,
			irr::s32 id,
			irr::core::recti& rectangle,
				// Graph parameters
			irr::core::rectf& graphWindow,
			bool marks = false,
			bool ticks = false,
			bool x_labels = true,
			bool y_labels = true
			)
		: IGraph2D( envir, parent, id, rectangle ),
		window( graphWindow ),
		UseMarkers( marks ),
		showXaxisMarks( marks ),
		showYaxisMarks( marks ),
		UseTicks( ticks ),
		UseXTickLabels( x_labels ),
		UseYTickLabels( y_labels ),
		markXgap(1.0f),
		markYgap(1.0f),
		showXaxis( true ),
		showYaxis( true ),
		clipPoints( true ),
		usePolyPts( false ),
		background_color( irr::video::SColor( 255, 0, 0, 0 ) ),
		hasBackground( true ),
		xaxis_color( irr::video::SColor( 255, 0, 125, 0 ) ),
		yaxis_color( irr::video::SColor( 255, 0, 125, 0 ) ),
		point_color( irr::video::SColor( 255, 255, 0, 0 ) ),
		xmark_color( irr::video::SColor( 255, 0, 125, 0 ) ),
		ymark_color( irr::video::SColor( 255, 0, 125, 0 ) )
	{
		viddriver = Environment->getVideoDriver();
	}

		//! Destructor
	~SGraph2D()
	{
	}

		//! Set graph size
	/* NOT the same as scaling.
	This function changes the ranges of values in the graph without changing
	the size of the graph as a GUI element.
	*/
	virtual void setGraphSize( irr::core::rectf& size );

		//! Set graph size along one dimension
	/* NOT the same as scaling.
	This function changes the ranges of values of ONE AXIS in the graph without
	changing the size of the graph as a GUI element.
	\param size - New axis min or max
	\param isMax - If the value given is for the maximum
	*/
	virtual void setGraphSizeX( irr::f32 size, bool isMax=true );
	virtual void setGraphSizeY( irr::f32 size, bool isMax=true );

		//! Set scale
	/* Changes the scale of the ranges of values in the graph without changing
	the size of the graph itself.
	NOTE: Since this is a scaling, the rectangle being passed in should be the
	percentage change (i.e. multiplier for the current values). */
	virtual void setGraphScale( irr::core::rectf& scale );

		//! Reallocate Memory for Graph
	/* In order to allow the graph to draw points quickly, this function is
	available to reallocate storage space for the data points.
	Old data points will not be destroyed and thus must be overwritten. */
	void ReallocateGraphSpace();

protected:

		//! Change graph window
	/* Corrects the graph points to be drawn correctly in the new graph
	window. */
	void changeGraphWindow( irr::core::rectf new_win );

public:

		//! Get graph size
	/* Returns the size of the graph. */
	virtual irr::core::rectf getGraphSize();

	// Graphing region

		//! Get Graph X-Axis Range
	/* Returns the range of values that will be visible when the graph is
	drawn. */
	Range<irr::f32> getXAxisRange();

		//! Get Graph Y-Axis Range
	/* Returns the range of values that will be visible when the graph is
	drawn. */
	Range<irr::f32> getYAxisRange();

		//! Get Iterable Range for X-Axis
	/* Returns an incrementor whose range spans the visible x-axis of the
	graph, whose step yields one point per pixel acroos the screen, and whose
	starting position is the one necessary for all points drawn with this as
	a guide to cross the entire graph window. */
	Inc<irr::f32> getIterableXRange();

		//! Get Iterable Range for Y-Axis
	/* Returns an incrementor whose range spans the visible y-axis of the
	graph, whose step yields one point per pixel acroos the screen, and whose
	starting position is the one necessary for all points drawn with this as
	a guide to cross the entire graph window. */
	Inc<irr::f32> getIterableYRange();


	// Other ----

		//! Set step size
	/* Sets the spacing between tick marks (if any). */
	void markerSpacingX( irr::f32 gap=1.0f );
	void markerSpacingY( irr::f32 gap=1.0f );

		//! Draw
	/* Draws the GUI element. */
	virtual void draw();

		//! Clear graph
	/* Erases everything in the graph. */
	virtual void clearGraph();

		//! Auto-adjust
	/* Corrects the graph window so that all of the points will fit. */
	void autoAdjust();

		//! Clip points
	/* Sets whether points outside the graphing area should be drawn. */
	void setPointClipping( bool yes=true);


	//-------------------------------
	// Drawing area functions / Spaz

		//! Set if there is a background
	void useBackground( bool use=true );

		//! Set background color
	/* Sets the color to be displayed behind the graph. */
	void setBackgroundColor( irr::video::SColor color );

		//! Set point color
	/* Sets the color of the points to be displayed. */
	virtual void setPointColor( irr::video::SColor color );

		//! Set x-axis color
	/* Sets the color that will be used in drawing the line representing
	the x-axis. */
	virtual void setXAxisColor( irr::video::SColor color );

		//! Set y-axis color
	/* Sets the color that will be used in drawing the line representing
	the y-axis. */
	virtual void setYAxisColor( irr::video::SColor color );

		//! Show x-axis
	/* Shows the x-xais if desired. */
	void setShowXAxis( bool show=true );

		//! Show y-axis
	/* Shows the y-axis if desired. */
	void setShowYAxis( bool show=true );

		//! Draw with polygons
	/* Draw the points with polygons. */
	void usePolygons( irr::f32 radius=1.5f, irr::s32 corners=4 );
	
		//! Stop drawing with polygons
	/* Stops drawing using polygons to draw the points.
	Note that you will need to reset the radius if you do this. */
	void useNoPolygons();

		//! Show X-axis labels
	/* Writes the values of the tick marks on the x-axis next to them.
	Note: The marks for that axis must actually be visible for the numbers
	to appear. */
	void showXAxisLabels( bool yes=true );

		//! Show Y-axis labels
	/* Writes the values of the tick marks on the x-axis next to them.
	Note: The marks for that axis must actually be visible for the numbers
	to appear. */
	void showYAxisLabels( bool yes=true );


	//----- Tick marks vs lines
	/* Either tick marks (three pixels crossing the line) or perpendicular
	lines can be used to show the step size. */

		//! Set to use tick marks instead of lines
	void useTicks();

		//! Set to use perpendicular lines instead of tick marks
	void useLines();

		//! Set to use no markers
	void useNoMarkers();

		//! Set to show the x-axis markers
	void showXAxisMarkers( bool show=true );

		//! Set to show the y-axis markers
	void showYAxisMarkers( bool show=true );

		//! Set x-axis tick color / perpendicular line color
	void setXAxisMarkerColor( irr::video::SColor color );

		//! Set y-axis tick color / perpendicular line color
	void setYAxisMarkerColor( irr::video::SColor color );

	//-----------------------------
	
		//! Serialize attributes
	virtual void serializeAttributes(
		irr::io::IAttributes* out,
		irr::io::SAttributeReadWriteOptions* options=0
		) const;

		//! Deserialize attributes
	virtual void deserializeAttributes(
		irr::io::IAttributes* in,
		irr::io::SAttributeReadWriteOptions* options=0
		);


	// ------------ Drawing functions ------------

		//! Draw to the graph
	/* Draws on the graph a single point whose coordinates are preppared
	according to the graph's coordinate system.
	You should use this function for standard operations. */
	void drawOnGraph(
		irr::core::vector2df point,
		irr::video::SColor color=irr::video::SColor(255,0,0,0)
		);

	void drawOnGraph(
		irr::f32 x,
		irr::f32 y,
		irr::video::SColor color=irr::video::SColor(255,0,0,0)
		);

		//! Draw on the graph - Polar coord
	/* Draws on the graph a single point whose coordinates are preppared
	according to the graph's coordinate system but are polar corrdinates.
	\param point - A radius (X value) and an angle (Y value).
	You should use this function for standard operations. */
	void drawOnGraphPolar(
		irr::core::vector2df point,
		irr::video::SColor color=irr::video::SColor(255,0,0,0)
		);

	void drawOnGraphPolar(
		irr::f32 radius,
		irr::f32 angle,
		irr::video::SColor color=irr::video::SColor(255,0,0,0)
		);

		//! Draw to the graph
	/* Draws on the graph a single point whose coordinates are preppared for
	the AbsoluteRect and need merely be shifted. */
	void drawRawPoint(
		irr::core::vector2df point,
		irr::video::SColor color=irr::video::SColor(255,0,0,0)
		);

protected:

		//! Draw directly to the GUI element
	/* Draws to the GUI element by handling the relative offset
	of the GUI element from the upperleft corner of the screen. */
	void drawToGUI( irr::core::vector2di point, irr::video::SColor color );
	void drawToGUI( irr::core::line2df line, irr::video::SColor color );
	void drawToGUI(
		irr::core::stringw text,
		irr::core::position2df pos,
		irr::video::SColor color,
		bool horiz_centering,
		bool vert_centering
		);
};

} // end namespace gui
} // end namespace irr

#endif // define __SIMPLEGRAPH2D_H__