#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Texture2dArrayApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void Texture2dArrayApp::setup()
{
}

void Texture2dArrayApp::mouseDown( MouseEvent event )
{
}

void Texture2dArrayApp::update()
{
}

void Texture2dArrayApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( Texture2dArrayApp, RendererGl )
