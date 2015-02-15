#include <assert.h>
#include <complex>
#include <algorithm>

#include "Area.h"

Area::Area( const int inLeft, const int inTop, const int inWidth, const int inHeight, const ColourIndex inDefaultColour )
	: m_left(inLeft)
	, m_top(inTop)
	, m_height(inHeight)
	, m_width(inWidth)
	, m_grid(0)
{
	assert( m_width > 0 && m_height > 0 );

	if ( m_width > 0 && m_height > 0 )
	{
		int numberOfCells = m_width * m_height;

		m_grid = new ColourIndex[ numberOfCells ];
		assert( m_grid );

		if ( m_grid )
		{
			for ( int index = 0; index < numberOfCells; index++ )
			{
				m_grid[ index ] = inDefaultColour;
			}
		}
	}
}

Area::~Area( void )
{
	if ( m_grid )
	{
		delete [] m_grid;
	}
}

ColourIndex Area::GetColour( const int X, const int Y ) const
{
	if ( m_grid )
	{
		if ( X >= m_left && X < (m_left + m_width) && 
			 Y >= m_top && Y < (m_top + m_height))
		{
			int gridX = std::abs(m_left - X);
			int gridY = std::abs(m_top - Y);

			return m_grid[ (m_width * gridY) + gridX ];
		}
	}
	return INVALID_COLOUR;
}


bool Area::SetColour( const ColourIndex inColourIndex, const int X, const int Y )
{
	if ( m_grid )
	{
		if ( X >= m_left && X < (m_left + m_width) && 
			 Y >= m_top && Y < (m_top + m_height))
		{
			int gridX = std::abs(m_left - X);
			int gridY = std::abs(m_top - Y);

			m_grid[ (m_width * gridY) + gridX ] = inColourIndex;

			return true;
		}
	}
	return false;
}

void Area::Clone( const Area* inArea, const int inStartX, const int inStartY )
{
	if ( inArea && m_grid )
	{
		for ( int x = 0; x < inArea->m_width; x++ )
		{
			for ( int y = 0; y < inArea->m_height; y++ )
			{
				const ColourIndex index = inArea->GetColour( inArea->m_left + x, inArea->m_top + y );

				const int newX = inStartX + x;
				const int newY = inStartY + y;

				SetColour( index, newX, newY );
			}
		}
	}
}
