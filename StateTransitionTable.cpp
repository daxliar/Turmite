#include "StateTransitionTable.h"

#include <assert.h>

StateTransitionTable::StateTransitionTable( const int inNumStates, const int inNumColours )
	: m_numState(0)
	, m_numColour(0)
	, m_operations(0)
{
	// Check input params
	assert( inNumStates > 0 && inNumColours > 0 );
	if ( inNumStates > 0 && inNumColours > 0 )
	{
		// Set number of states and colours
		m_numState = inNumStates;
		m_numColour = inNumColours;

		// Allocate table. For each state a list of Operation for each colour index
		m_operations = new Operation*[ m_numState ];
		assert( m_operations );
		if ( m_operations )
		{
			for ( int state = 0; state < m_numState; state++ )
			{
				m_operations[ state ] = new Operation[ m_numColour ];
				assert( m_operations[ state ] );
			}
		}
	}
}

StateTransitionTable::~StateTransitionTable(void)
{
	if ( m_operations )
	{
		for ( int state = 0; state < m_numState; state++ )
		{
			delete [] m_operations[ state ];
		}
		delete [] m_operations;
	}
}

const Operation& StateTransitionTable::GetOperation(const int inState, const int inColour ) const
{
	assert( m_operations );
	if ( m_operations )
	{
		assert( inState >= 0 && inState < m_numState );
		assert( inColour >= 0 && inColour < m_numColour );
		if ( inState >= 0 && inState < m_numState && inColour >= 0 && inColour < m_numColour )
		{
			return m_operations[ inState ][ inColour ];
		}
	}
	return m_invalidOperation;
}

bool StateTransitionTable::SetOperation( const State inState, const ColourIndex inColour, const Operation& inOperation )
{
	assert( m_operations );
	if ( m_operations )
	{
		assert( inState >= 0 && inState < m_numState );
		assert( inColour >= 0 && inColour < m_numColour );
		if ( inState >= 0 && inState < m_numState && inColour >= 0 && inColour < m_numColour )
		{
			m_operations[ inState ][ inColour ] = inOperation;
			return true;
		}
	}
	return false;
}
