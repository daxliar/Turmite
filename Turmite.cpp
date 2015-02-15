#include <assert.h>

#include "Turmite.h"
#include "World.h"

Turmite::Turmite( const int inPosX, const int inPosY, const State inState, const eOrientation inOrientation, World* inWorld, StateTransitionTable* inStateTransitionTable  )
	: m_posX(inPosX)
	, m_posY(inPosY)
	, m_state(inState)
	, m_orientation(inOrientation)
	, m_currentStep(0)
	, m_world(inWorld)
	, m_stateTransitionTable(inStateTransitionTable)
{
	assert(m_world);
	assert(m_stateTransitionTable);
}

Turmite::~Turmite(void)
{
}

unsigned int Turmite::Step()
{
	assert(m_world);
	assert(m_stateTransitionTable);

	if ( m_world  && m_stateTransitionTable )
	{
		// Current colour index
		ColourIndex colourIndex = m_world->GetColourIndex( m_posX, m_posY );

		// Get the operation to perform
		const Operation& operation = m_stateTransitionTable->GetOperation( m_state, colourIndex );
		if ( operation.IsValid() )
		{
			// Change orientation
			Rotate( operation.turn );

			// Change the colour on the grid
			m_world->SetColourIndex( operation.writeColour, m_posX, m_posY );

			// Move forward following the new orientation
			m_world->MoveForward( m_orientation, m_posX, m_posY );

			// Change current state
			m_state = operation.nextState;
		}
	}
	return ++m_currentStep;
}

void Turmite::Rotate( const eMovement inMovement )
{
	int rot(0);

	switch( inMovement )
	{
	case eRight:	rot =  1;	break;
	case eLeft:		rot =  3;	break;
	case eUturn:	rot =  2;	break;
	}

	m_orientation = static_cast<eOrientation>( (static_cast<int>(m_orientation) + rot) % eOrientation_Max );
}
