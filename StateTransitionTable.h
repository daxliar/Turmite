#pragma once

// State for each cell of the world
typedef int State;

// Define an invalid colour
#define INVALID_STATE ((State)-1)

// I'll store an index of the color only for the grid
typedef int ColourIndex;

// Define an invalid colour
#define INVALID_COLOUR ((ColourIndex)-1)

// Possible movements
enum eMovement
{
	eNone,		// Don't turn
	eLeft,		// Turn	90 degree left
	eRight,		// Turn	90 degree right
	eUturn,		// Turn 180 degree

	eMovement_Max
};

// Color
struct Colour
{
	Colour() : R(0), G(0), B(0) {}

	unsigned char R;
	unsigned char G;
	unsigned char B;
};

// Operation for each colour
struct Operation
{
	Operation() : writeColour(INVALID_COLOUR), turn(eNone), nextState(INVALID_STATE) {}

	bool IsValid() const { return writeColour != INVALID_COLOUR && nextState != INVALID_STATE; }

	ColourIndex		writeColour;
	eMovement		turn;
	State			nextState;
};

// 
class StateTransitionTable
{
public:
	StateTransitionTable(const int numStates, const int inNumColours );
	virtual ~StateTransitionTable(void);

	const Operation& GetOperation(const State inState, const ColourIndex inColour ) const;
	bool SetOperation(const State inState, const ColourIndex inColour, const Operation& inOperation );

private:

	// Number of colour and states;
	int m_numState;
	int m_numColour;

	// Table with the operations. Coloums are operations for each colour, rows are the states;
	Operation**	m_operations;

	// Invalid operation in case of error
	Operation m_invalidOperation;
};

