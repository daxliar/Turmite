
// 
// Turmite sandbox
//
// Usage: ./Turmite <config xml file> <number of steps>"
//
// Press 'q' to exit
// Press 's' to save current status in a PNG file
// Press '1', '2', '3' or '4' to add 1,10,100,1000 more steps
// 

#include <iostream>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "World.h"
#include "Turmite.h"
#include "lodepng.h"
#include "pugixml.hpp"

#define APP_NAME "Turmite"

//
// Global data used the the app
//

World*					g_world					= NULL;
StateTransitionTable*	g_stateTransitionTable	= NULL;
Turmite*				g_turmite				= NULL;
Colour*					g_colours				= NULL;
int						g_worldWidth			= 0;
int						g_worldHeight			= 0;
int						g_worldDefaultColour	= 0;

unsigned int			g_currentStep			= 0;
unsigned int			g_numberOfStepsToDo		= 0;

GLuint 					g_texture 				= 0;
unsigned char*			g_textureData			= NULL;

//
// Allocate the structures to run Turmite
//
bool Instance( const char* inFilename )
{
	int states = 0;
	int colours = 0;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(inFilename);
	if ( result )
	{
		// Get main node
		pugi::xml_node turmiteNode = doc.child("Turmite");

		// Get attributes for number of states and colours
		states					= turmiteNode.attribute("states").as_int();
		colours					= turmiteNode.attribute("colours").as_int();
		g_worldWidth			= turmiteNode.attribute("width").as_int();
		g_worldHeight			= turmiteNode.attribute("height").as_int();
		g_worldDefaultColour	= turmiteNode.attribute("defaultColour").as_int();

		// Default fixed boundaries
		eBoundayCondition boundaryCondition = eFixed;
		const char* boundaryAttribute = turmiteNode.attribute("boundary").as_string();
		if  ( boundaryAttribute )
		{
			if ( !strcmp(boundaryAttribute,"fixed") ){
				boundaryCondition = eFixed;
			}
			else if ( !strcmp(boundaryAttribute,"toroidal") ){
				boundaryCondition = eToroidal;
			}
			else if ( !strcmp(boundaryAttribute,"infinite") ){
				boundaryCondition = eInfinte;
			}
		}

		// Create the Turmite world
		if ( g_worldWidth > 0 && g_worldHeight > 0 )
		{
			g_world = new World( g_worldWidth, g_worldHeight, g_worldDefaultColour, boundaryCondition );
			assert( g_world );
			if ( !g_world )
			{
				return false;
			}
		}
		else
		{
			std::cerr << "Wrong number of colours!" << std::endl;
			return false;
		}

		// If there are states and colours
		if ( states > 0 && colours > 0 )
		{
			// Find colours node
			pugi::xml_node coloursNode = turmiteNode.child("Colours");
			if ( !coloursNode.empty() )
			{
				// Allocate colours 
				g_colours = new Colour[ colours ];
				assert(g_colours);
				if ( g_colours )
				{
					int foundColors = 0;

					// Read colours
					for (pugi::xml_node node = coloursNode.child("Colour"); node; node = node.next_sibling("Colour") )
					{
						if ( foundColors < colours )
						{
							g_colours[ foundColors ].R = static_cast<unsigned char>(node.attribute("R").as_int());
							g_colours[ foundColors ].G = static_cast<unsigned char>(node.attribute("G").as_int());
							g_colours[ foundColors ].B = static_cast<unsigned char>(node.attribute("B").as_int());
						}
						foundColors++;
					}
					assert( foundColors == colours );
					if ( foundColors != colours)
					{
						std::cerr << "Wrong number of colours!" << std::endl;
						return false;
					}
				}
			}
			else
			{
				std::cerr << "Can't find Colours node!" << std::endl;
				return false;
			}

			// Find states node
			pugi::xml_node statesNode = turmiteNode.child("States");
			if ( !statesNode.empty() )
			{
				// Allocate the state transition table
				g_stateTransitionTable = new StateTransitionTable( states, colours );
				assert(g_stateTransitionTable);
				if ( g_stateTransitionTable )
				{
					int currentState = 0;

					// Read states
					for (pugi::xml_node state = statesNode.child("State"); state; state = state.next_sibling("State") )
					{
						if ( currentState < states )
						{
							int currentOperation = 0;

							// For each state I need operations for each colour
							for (pugi::xml_node op = state.child("Operation"); op; op = op.next_sibling("Operation") )
							{
								if ( currentOperation < colours )
								{
									eMovement movement;
									Operation tmpOp;

									const char * turn = op.attribute("turn").as_string();
									if ( turn )
									{
										if ( !strcmp(turn,"R") ){
											movement = eRight;
										}
										else if ( !strcmp(turn,"L") ){
											movement = eLeft;
										}
										else if ( !strcmp(turn,"U") ){
											movement = eUturn;
										}
										else if ( !strcmp(turn,"N") ){
											movement = eNone;
										}
									}

									tmpOp.writeColour	= op.attribute("writeColour").as_int();
									tmpOp.turn			= movement;
									tmpOp.nextState		= op.attribute("nextState").as_int();

									g_stateTransitionTable->SetOperation( currentState, currentOperation, tmpOp );
								}
								else
								{
									std::cerr << "Skipping exceeding operations!" << std::endl;
								}
								currentOperation++;
							}
						}
						else
						{
							std::cerr << "Skipping exceeding states!" << std::endl;
						}
						currentState++;
					}
					assert( currentState == states );
					if ( currentState != states)
					{
						std::cerr << "Wrong number of states!" << std::endl;
						return false;
					}
				}
			}
		}
		else
		{
			std::cerr << "Not enough states or colours!" << std::endl;
			return false;
		}

		// Configuration is correct, I can allocate Turmite
		g_turmite = new Turmite( g_worldWidth / 2, g_worldHeight / 2, 0,  eWest, g_world, g_stateTransitionTable );
		assert( g_turmite );
	}

	return g_turmite != NULL;
}

//
// Get the current worold and save it in a PNG file
// 
void SaveWorldPng()
{
	if ( !( g_world && g_turmite && g_colours ) )
	{
		return;
	}

	// Get boundaries
	int minX,maxX,minY,maxY;
	g_world->GetBoundaries(minX,maxX,minY,maxY);

	// Size
	int width = maxX - minX + 1;
	int height = maxY - minY + 1;

	unsigned int bpp = 4;
	std::vector<unsigned char> png;
	std::vector<unsigned char> bmp( width * height * bpp, 255 );

	// Writing PNG out
	int currentPosX(0);
	int currentPosY(0);

	g_turmite->GetPosistion( currentPosX, currentPosY );

	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++ )
		{
			unsigned int newpos = 4 * y * width + 4 * x;

			int worldPosX = minX + x;
			int worldPosY = minY + y;

			ColourIndex index = g_world->GetColourIndex( worldPosX, worldPosY );

			if ( currentPosX == worldPosX && currentPosY == worldPosY )
			{
				bmp[newpos + 0] = 255;	//R
				bmp[newpos + 1] = 0;	//G
				bmp[newpos + 2] = 0;	//B
				bmp[newpos + 3] = 255;	//A
			}
			else
			{
				bmp[newpos + 0] = g_colours[ index ].R;	//R
				bmp[newpos + 1] = g_colours[ index ].G;	//G
				bmp[newpos + 2] = g_colours[ index ].B;	//B
				bmp[newpos + 3] = 255;	//A
			}
		}
	}	

	unsigned int error = lodepng::encode(png, bmp, width, height);

	if(!error)
	{
		char filename[256];
		sprintf( filename, "turmite_step_%d.png", g_currentStep );
		lodepng::save_file(png, filename );

		std::cout << "Saved " << filename << std::endl;
	}
	else
	{
		std::cerr << "Can't encode file!" << std::endl;
	}
}

//
// Deallocate memory on app exit
//
void OnCloseCB()
{
	std::cout << "Quitting..." << std::endl;

	// Free memory
	if ( g_world )
	{
		delete g_world;
	}
	if ( g_stateTransitionTable )
	{
		delete g_stateTransitionTable;
	}
	if ( g_turmite )
	{
		delete g_turmite;
	}
	if ( g_colours )
	{
		delete [] g_colours;
	}
	if ( g_textureData )
	{
		delete [] g_textureData;
	}
}

//
// Do steps until you reach the required number ot steps
//
void Evolve()
{
	if ( g_turmite )
	{
		// Let's evolve the world up to the number of required steps
		while ( g_currentStep < g_numberOfStepsToDo )
		{
			// Let's move inside the world
			g_currentStep = g_turmite->Step();
		}
	}
}

//
// Render a simple quad with the texture of the world
//
void RenderSceneCB()
{
    // Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    // Render quad
    glBegin( GL_QUADS );
        glTexCoord2f(0.0f, 0.0f); glVertex2f( -1.0f,  1.0f );
        glTexCoord2f(1.0f, 0.0f); glVertex2f(  1.0f,  1.0f );
        glTexCoord2f(1.0f, 1.0f); glVertex2f(  1.0f, -1.0f );
        glTexCoord2f(0.0f, 1.0f); glVertex2f( -1.0f, -1.0f );
    glEnd();

    // Update screen
    glutSwapBuffers();
}

//
// Application update
//
void UpdateCB(int value)
{
	RenderSceneCB();

	glutTimerFunc( 200, UpdateCB, 0 );
}

//
// Update the texture using world's data
//
void UpdateTextureDataFromWorld()
{
	if ( g_textureData )
	{
		// Writing texture
		int currentPosX(0);
		int currentPosY(0);
		for ( int y = 0; y < g_worldHeight; y++ )
		{
			for ( int x = 0; x < g_worldWidth; x++ )
			{
				unsigned int newpos = 3 * y * g_worldWidth + 3 * x;

				ColourIndex index = g_world->GetColourIndex( x, y );

				g_turmite->GetPosistion( currentPosX, currentPosY );

				if ( currentPosX == x && currentPosY == y )
				{
					g_textureData[newpos + 0] = 255;	//R
					g_textureData[newpos + 1] = 0;		//G
					g_textureData[newpos + 2] = 0;		//B
				}
				else
				{
					g_textureData[newpos + 0] = g_colours[ index ].R;	//R
					g_textureData[newpos + 1] = g_colours[ index ].G;	//G
					g_textureData[newpos + 2] = g_colours[ index ].B;	//B
				}
			}
		}
	}
}

//
// Update the texture and assign it again
//
void UpdateTexture()
{
	UpdateTextureDataFromWorld();

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, g_worldWidth, g_worldHeight, GL_RGB, GL_UNSIGNED_BYTE, g_textureData );
}

//
// Create the texture and update it
//
void InitializeWorldTexture()
{
	// Allocate texure 
	g_textureData = new unsigned char[ g_worldWidth * g_worldHeight * 3 ];

	// Write texure pixels from world data
	UpdateTextureDataFromWorld();

    // Create Texture	
    glGenTextures(1, &g_texture);
    glBindTexture(GL_TEXTURE_2D, g_texture );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // scale linearly when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, g_worldWidth, g_worldHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, g_textureData );
}

//
// Keyboard input to quit, save or increase the number of steps
//
void InputCB(unsigned char key, int x, int y )
{
    switch( key )
    {
    case 'q':
        glutLeaveMainLoop();
        break;
    case 's':
    	// Save
		SaveWorldPng();
		break;
    case '1':
        // increase step 1
    	g_numberOfStepsToDo += 1;
    	Evolve();
    	UpdateTexture();
        break;
    case '2':
        // increase step 10
    	g_numberOfStepsToDo += 10;
    	Evolve();
    	UpdateTexture();
        break;
    case '3':
        // increase step 100
    	g_numberOfStepsToDo += 100;
    	Evolve();
    	UpdateTexture();
        break;
    case '4':
        // increase step 1000
    	g_numberOfStepsToDo += 1000;
    	Evolve();
    	UpdateTexture();
        break;
    }
}

//
// Set the required callbacks for glut
//
void InitializeGlutCallbacks()
{
	glutTimerFunc( 200, UpdateCB, 0 );	
    glutDisplayFunc(RenderSceneCB);
    glutKeyboardFunc(InputCB);
    glutCloseFunc(OnCloseCB);
}

//
// Init OpenGL window
//
void InitGL()
{
	char *myargv [1];
    int myargc=1;
    myargv [0] = strdup(APP_NAME);

    // Init glut
    glutInit(&myargc, myargv);

    // Set display mode
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);

    // Window size is the size of the world
    glutInitWindowSize(g_worldWidth, g_worldHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(APP_NAME);

    // Initialize basi callbacks
    InitializeGlutCallbacks();

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
    	std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
		exit(1);
    }

    // Enable texturing
    glEnable(GL_TEXTURE_2D);

    // Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    // Create texture using world data
    InitializeWorldTexture();

    // Start main loop
    glutMainLoop();
}

//
// Application main
//
int main( int argc, char** argv )
{
	if ( argc != 3 )
	{
		std::cerr << "Usage: " << argv[0] << " <config xml file> <number of steps>" << std::endl;
		return 1;
	}
	else
	{
		// Get the number of steps to achieve
		g_numberOfStepsToDo = atoi( argv[2] );

		// Instance Turmite using XML configuration file
		if ( Instance( argv[1] ) )
		{
			// Reach the number of steps to do
			Evolve();

			// Create GL window
			InitGL();

			return 0;
		}
	}
	return 1;
}