#pragma once

#include "StateTransitionTable.h"

// Forward declaration on world
class World;

// Current orientation of the Turmite
enum eOrientation
{
	eNorth = 0,
	eEast,
	eSouth,
	eWest,

	eOrientation_Max
};

class Turmite
{
public:

	Turmite( const int inPosX, 
			 const int inPosY, 
			 const State inState, 
			 const eOrientation inOrientation, 
			 World* inWorld, 
			 StateTransitionTable* inStateTransitionTable );

	~Turmite(void);

	unsigned int Step();

	void GetPosistion( int& outX, int& outY ) const { outX = m_posX; outY = m_posY; }

private:

	// Rotate using the 
	void Rotate( const eMovement inMovement );

	// Turmite position, state and orientation
	int						m_posX;
	int						m_posY;
	State					m_state;
	eOrientation			m_orientation;

	// State transition table with the rules 
	StateTransitionTable*	m_stateTransitionTable;

	// Number of iterations done
	unsigned int			m_currentStep;

	// Every step investigate the world
	World*					m_world;

	// private default constructor/copy constructor/assignment operator
	Turmite();
	Turmite( const Turmite& );
	const Turmite& operator=( const Turmite& );
};

