#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class EmptyFboApp : public App {
  public:
	EmptyFboApp();
	void draw() override;
	void mouseDown( MouseEvent event ) override;

	gl::Texture2dRef mTextureR, mTextureG, mTextureB;
};

EmptyFboApp::EmptyFboApp()
{
	// create 3 test textures
	mTextureR = gl::Texture2d::create( getWindowWidth() / 3, getWindowHeight() );
	mTextureG = gl::Texture2d::create( getWindowWidth() / 3, getWindowHeight() );
	mTextureB = gl::Texture2d::create( getWindowWidth() / 3, getWindowHeight() );

	// create an empty fbo and bind it
	auto fbo = gl::Fbo::create();
	gl::ScopedFramebuffer scopedFbo( fbo );

	// attach the first texture and clear it with red
	gl::frameBufferTexture( mTextureR, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
	
	fbo->checkStatus();

	gl::clear( Color( 1.0f, 0.0f, 0.0f ) );
	
	// replace the first attachement by the second texture and clear it with green
	gl::frameBufferTexture( mTextureG, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
	
	fbo->checkStatus();

	gl::clear( Color( 0.0f, 1.0f, 0.0f ) );
	
	// replace the first attachement by the third texture and clear it with blue
	gl::frameBufferTexture( mTextureB, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
	
	fbo->checkStatus();

	gl::clear( Color( 0.0f, 0.0f, 1.0f ) );

	// check for erros
	CI_CHECK_GL();
}

void EmptyFboApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	
	gl::draw( mTextureR );
	gl::draw( mTextureG, vec2( getWindowWidth() / 3, 0.0f ) );
	gl::draw( mTextureB, vec2( getWindowWidth() / 3 * 2, 0.0f ) );
}

void EmptyFboApp::mouseDown( MouseEvent event )
{
	// pick the texture under the mouse
	gl::Texture2dRef tex;
	if( event.getPos().x < getWindowWidth() / 3 ) tex = mTextureR;
	else if( event.getPos().x > getWindowWidth() / 3 * 2 ) tex = mTextureB;
	else tex = mTextureG;
	
	// create an empty fbo, attach one of the textures and clear it with a random color
	auto fbo = gl::Fbo::create();
	gl::ScopedFramebuffer scopedFbo( fbo );
	gl::frameBufferTexture( tex, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
	fbo->checkStatus();
	gl::clear( Color( randFloat(), randFloat(), randFloat() ) );

}

CINDER_APP( EmptyFboApp, RendererGl )
