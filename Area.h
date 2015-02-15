#pragma once

#include "StateTransitionTable.h"

// Area to store part of the world
class Area
{
public:

	Area(	const int inLeft, 
			const int inTop, 
			const int inWidth,
			const int inHeight,
			const ColourIndex inDefaultColour );

	virtual ~Area(void);

	ColourIndex GetColour( const int X, const int Y ) const;
	bool SetColour( const ColourIndex inColourIndex, const int X, const int Y );

	void Clone( const Area* inArea, const int inStartX, const int inStartY  );

private:

	// Top left position
	int m_left;
	int m_top;

	// size of the area
	int m_width;
	int m_height;

	// Colour indexes are allocate in a linear array
	ColourIndex* m_grid;

	// private default constructor/copy constructor/assignment operator
	Area();
	Area( const Area& );
	const Area& operator=( const Area& );
};
