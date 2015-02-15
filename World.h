#pragma once

#include <vector>

#include "Turmite.h"

// Forward declaration of the Area
class Area;

// World boundaries conditions
enum eBoundayCondition
{
	eFixed = 0,		// Bounce against borders
	eToroidal,		// Wrap around the world
	eInfinte,		// Expand beyound the edge of the world

	eBoundayCondition_Max
};

// Defines the world which is made up of a list of areas
class World
{
public:

	World( const int inWidth, const int inHeight, const ColourIndex inDefaultColour, const eBoundayCondition inBoundaries );
	virtual ~World(void);

	// Setter/Getter for colours
	ColourIndex GetColourIndex( int inPosX, int inPosY );
	void SetColourIndex( const ColourIndex inColourIndex, int inPosX, int inPosY );

	// Move forward one cell following orientation
	void MoveForward( const eOrientation inOrientation, int& outPosX, int& outPosY );

	// Get min/max size
	void GetBoundaries( int& outMinX, int& outMaxX, int& outMinY, int& outMaxY ) const;

private:

	// Initialize the world with the params
	void Init();

	// Store world size and initial params
	int m_worldTop;
	int m_worldLeft;
	int m_worldWidth;
	int m_worldHeight;
	int m_worldInitialWidth;
	int m_worldInitialHeight;

	// Store min max values set
	int m_minX;
	int m_maxX;
	int m_minY;
	int m_maxY;

	// Conditions when the turmite hits the edge of the world
	eBoundayCondition m_boundaryCondition;

	// Default colour when instantiating new areas
	ColourIndex m_defaultColourIndex;

	// Current area
	Area* m_area;

	// private default constructor/copy constructor/assignment operator
	World(void);
	World( const World& );
	const World& operator=( const World& );
};
