#include <stdlib.h>
#include <assert.h>

#include "World.h"
#include "Area.h"

World::World( const int inWidth, const int inHeight, const ColourIndex inDefaultColour, const eBoundayCondition inBoundaries )
	: m_worldTop(0)
	, m_worldLeft(0)
	, m_worldWidth(inWidth)
	, m_worldHeight(inHeight)
	, m_defaultColourIndex(inDefaultColour)
	, m_worldInitialWidth(inWidth)
	, m_worldInitialHeight(inHeight)
	, m_boundaryCondition(inBoundaries)
	, m_area(NULL)
	, m_minX(0)
	, m_maxX(inWidth-1)
	, m_minY(0)
	, m_maxY(inHeight-1)
{
	// Allocate area
	m_area = new Area( m_worldLeft, m_worldTop, m_worldWidth, m_worldHeight, m_defaultColourIndex );
	assert( m_area );
}

World::~World(void)
{
	if ( m_area )
	{
		delete m_area;
	}
}

ColourIndex World::GetColourIndex( int inPosX, int inPosY )
{
	if ( m_area )
	{
		return m_area->GetColour( inPosX, inPosY );
	}
	return INVALID_COLOUR;
}

void World::SetColourIndex( const ColourIndex inColourIndex, int inPosX, int inPosY )
{
	if ( m_area )
	{
		// Update mix/max coordinates
		if ( inPosX < m_minX )
			m_minX = inPosX;
		if ( inPosX > m_maxX )
			m_maxX = inPosX;
		if ( inPosY < m_minY )
			m_minY = inPosY;
		if ( inPosY > m_maxY )
			m_maxY = inPosY;

		// Set colour in the grid
		m_area->SetColour( inColourIndex, inPosX, inPosY );
	}
}

void World::GetBoundaries( int& outMinX, int& outMaxX, int& outMinY, int& outMaxY ) const
{
	outMinX = m_minX;
	outMaxX = m_maxX;
	outMinY = m_minY;
	outMaxY = m_maxY;
}

void World::MoveForward( const eOrientation inOrientation, int& outPosX, int& outPosY ) 
{
	assert( inOrientation < eOrientation_Max );

	// Move forward
	switch( inOrientation )
	{
	case eNorth:	outPosY--;	break;
	case eEast:		outPosX++;	break;
	case eSouth:	outPosY++;	break;
	case eWest:		outPosX--;	break;
	}

	// Apply boundary condition
	switch( m_boundaryCondition )
	{
	case eFixed:
		{
			// Bounce back into the world
			if ( outPosX >= m_worldWidth )
				outPosX -= 2;
			if ( outPosX < m_worldLeft )
				outPosX += 2;
			if ( outPosY >= m_worldHeight )
				outPosY -= 2;
			if ( outPosY < m_worldTop )
				outPosY += 2;
		}
		break;
	case eToroidal:
		{
			outPosX = (((outPosX % m_worldWidth) + m_worldWidth) % m_worldWidth);
			outPosY = (((outPosY % m_worldHeight) + m_worldHeight) % m_worldHeight);
		}
		break;
	case eInfinte:
		{
			Area * newArea(NULL);

			// Reallocate a larger area
			if ( outPosX >= m_worldWidth )
			{
				// Make the world double
				m_worldWidth += m_worldInitialWidth;

				newArea = new Area( m_worldLeft, m_worldTop, m_worldWidth, m_worldHeight, m_defaultColourIndex );
			}
			if ( outPosX < m_worldLeft )
			{
				// Extend the world
				m_worldLeft -= m_worldInitialWidth;
				m_worldWidth += m_worldInitialWidth;

				newArea = new Area( m_worldLeft, m_worldTop, m_worldWidth, m_worldHeight, m_defaultColourIndex );
			}
			if ( outPosY >= m_worldHeight )
			{
				// Extend the world
				m_worldHeight += m_worldInitialHeight;

				newArea = new Area( m_worldLeft, m_worldTop, m_worldWidth, m_worldHeight, m_defaultColourIndex );
			}
			if ( outPosY < m_worldTop )
			{
				// Extend the world
				m_worldTop -= m_worldInitialHeight;
				m_worldHeight += m_worldInitialHeight;

				newArea = new Area( m_worldLeft, m_worldTop, m_worldWidth, m_worldHeight, m_defaultColourIndex );
			}

			if ( newArea )
			{
				// Clone the new area
				newArea->Clone( m_area, m_worldLeft, m_worldTop );

				// Delete the old one and assign the new one
				delete m_area;
				m_area = newArea;
			}
		}
		break;
	}
}
