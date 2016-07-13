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
	void keyDown( KeyEvent event ) override;

	gl::Texture2dRef mTextureRed, mTextureGreen, mTextureBlue;
};

EmptyFboApp::EmptyFboApp()
{
	// create 3 test textures
	mTextureRed		= gl::Texture2d::create( getWindowWidth() / 3, getWindowHeight() );
	mTextureGreen	= gl::Texture2d::create( getWindowWidth() / 3, getWindowHeight() );
	mTextureBlue	= gl::Texture2d::create( getWindowWidth() / 3, getWindowHeight() );

	// create an empty fbo and bind it
	auto fbo = gl::Fbo::createEmpty();
	gl::ScopedFramebuffer scopedFbo( fbo );

	// attach the first texture and clear it with red
	gl::frameBufferTexture( mTextureRed, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
	gl::clear( Color( 1.0f, 0.0f, 0.0f ) );
	
	// replace the first attachement by the second texture and clear it with green
	gl::frameBufferTexture( mTextureGreen, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
	gl::clear( Color( 0.0f, 1.0f, 0.0f ) );
	
	// replace the first attachement by the third texture and clear it with blue
	gl::frameBufferTexture( mTextureBlue, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
	gl::clear( Color( 0.0f, 0.0f, 1.0f ) );

	// just to make sure let's check if there's an issue with our fbo	
	gl::Fbo::checkStatus();

	// check for erros
	CI_CHECK_GL();

	// need more work here:	
	console() << "fbo->getAspectRatio()\t" << fbo->getAspectRatio() << endl;
	console() << "fbo->getBounds()\t" << fbo->getBounds() << endl;
	console() << "fbo->getColorTexture()\t" << fbo->getColorTexture() << endl;
	console() << "fbo->getDepthTexture()\t" << fbo->getDepthTexture() << endl;
	console() << "fbo->getFormat()\t" << fbo->getFormat().getLabel() << endl;
	console() << "fbo->getHeight()\t" << fbo->getHeight() << endl;
	console() << "fbo->getLabel()\t" << fbo->getLabel() << endl;
	console() << "fbo->getMultisampleId()\t" << fbo->getMultisampleId() << endl;
	console() << "fbo->getResolveId()\t" << fbo->getResolveId() << endl;
	console() << "fbo->getSize()\t" << fbo->getSize() << endl;
	console() << "fbo->getTexture2d()\t" << fbo->getTexture2d( GL_COLOR_ATTACHMENT0 ) << endl;
	console() << "fbo->getTextureBase()\t" << fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) << endl;
	console() << "fbo->getWidth()\t" << fbo->getWidth() << endl;
}

void EmptyFboApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	
	gl::ScopedViewport viewport( getWindowSize() );
	gl::setMatricesWindow( getWindowSize() );
	gl::draw( mTextureRed );
	gl::draw( mTextureGreen, vec2( getWindowWidth() / 3, 0.0f ) );
	gl::draw( mTextureBlue, vec2( getWindowWidth() / 3 * 2, 0.0f ) );
}

void EmptyFboApp::mouseDown( MouseEvent event )
{
	// pick the texture under the mouse
	gl::Texture2dRef tex;
	if( event.getPos().x < getWindowWidth() / 3 ) tex = mTextureRed;
	else if( event.getPos().x > getWindowWidth() / 3 * 2 ) tex = mTextureBlue;
	else tex = mTextureGreen;
	
	// create an empty fbo and bind it
	auto fbo = gl::Fbo::createEmpty();
	gl::ScopedFramebuffer scopedFbo( fbo );
	if( event.isLeft() ) {
		// attach the current texture and clear it with a random color
		gl::frameBufferTexture( tex, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
		gl::Fbo::checkStatus();

		gl::clear( Color( randFloat(), randFloat(), randFloat() ) );
	}
	else if( event.isRight() && event.isShiftDown() ) {
		// attach the current texture and draw a teapot without depth testing
		gl::frameBufferTexture( tex, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
		gl::Fbo::checkStatus();
		
		gl::ScopedDepth scopedDepth( true ); // has no effect because we don't have a depth buffer
		gl::ScopedViewport viewport( tex->getSize() );
		gl::setMatrices( CameraPersp( tex->getWidth(), tex->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
		auto teapot = gl::Batch::create( geom::Teapot(), gl::getStockShader( gl::ShaderDef().lambert() ) );
		teapot->draw();
	}
	else if( event.isRight() && event.isAltDown() ) {
		// create a depth texture
		auto depthTexture = gl::Texture2d::create( tex->getWidth(), tex->getHeight(), gl::Texture2d::Format().internalFormat( GL_DEPTH_COMPONENT24 ) );

		// attach the current texture and depth texture and draw a teapot with proper depth testing
		gl::frameBufferTexture( tex, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
		gl::frameBufferTexture( depthTexture, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0 );
		gl::Fbo::checkStatus();
		
		gl::clear( GL_DEPTH_BUFFER_BIT );

		gl::ScopedDepth scopedDepth( true );
		gl::ScopedViewport viewport( tex->getSize() );
		gl::setMatrices( CameraPersp( tex->getWidth(), tex->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
		auto teapot = gl::Batch::create( geom::Teapot(), gl::getStockShader( gl::ShaderDef().lambert() ) );
		teapot->draw();
	}
	else if( event.isRight() ) {
		// create a depth buffer
		auto depthBuffer = gl::Renderbuffer::create( tex->getWidth(), tex->getHeight(), GL_DEPTH_COMPONENT24 );

		// attach the current texture and depth buffer and draw a teapot with proper depth testing
		gl::frameBufferTexture( tex, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
		gl::framebufferRenderbuffer( depthBuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT );
		gl::Fbo::checkStatus();
		
		gl::clear( GL_DEPTH_BUFFER_BIT );

		gl::ScopedDepth scopedDepth( true );
		gl::ScopedViewport viewport( tex->getSize() );
		gl::setMatrices( CameraPersp( tex->getWidth(), tex->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
		auto teapot = gl::Batch::create( geom::Teapot(), gl::getStockShader( gl::ShaderDef().lambert() ) );
		teapot->draw();
	}

	// check for erros
	CI_CHECK_GL();
}
void EmptyFboApp::keyDown( KeyEvent event ) 
{
	if( event.getCode() == KeyEvent::KEY_1 ) {
		// create an empty fbo, attach the 3 textures and clear them with black
		auto fbo = gl::Fbo::createEmpty();
		gl::ScopedFramebuffer scopedFbo( fbo );
		gl::frameBufferTexture( mTextureRed, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
		gl::frameBufferTexture( mTextureGreen, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0 );
		gl::frameBufferTexture( mTextureBlue, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0 );
		gl::Fbo::checkStatus();
		
		gl::ScopedDrawBuffers drawBuffers( { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 } );

		gl::clear( Color::black() );
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
	}
	else if( event.getCode() == KeyEvent::KEY_2 ) {
		// create two empty fbo2, attach renderbuffers to the first and the red texture to the other.
		// render cubes in the first and then blit it to the second
		auto colorBuffer = gl::Renderbuffer::create( mTextureRed->getWidth(), mTextureRed->getHeight() );
		auto depthBuffer = gl::Renderbuffer::create( mTextureRed->getWidth(), mTextureRed->getHeight(), GL_DEPTH_COMPONENT24 );
		auto fbo0 = gl::Fbo::createEmpty();
		{
			gl::ScopedFramebuffer scopedFbo( fbo0 );
			gl::framebufferRenderbuffer( colorBuffer, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 );
			gl::framebufferRenderbuffer( depthBuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT );
			gl::Fbo::checkStatus();
		
			gl::clear();

			gl::ScopedDepth scopedDepth( true );
			gl::ScopedViewport viewport( mTextureRed->getSize() );
			gl::setMatrices( CameraPersp( mTextureRed->getWidth(), mTextureRed->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
			auto colorCube = gl::Batch::create( geom::Cube().colors(), gl::getStockShader( gl::ShaderDef().color() ) );
			for( int i = 0; i < 150; ++i ) {
				gl::ScopedModelMatrix scopedMat;
				gl::multModelMatrix( glm::scale( vec3( randFloat() ) ) * glm::rotate( randFloat(), randVec3() ) * glm::translate( randVec3() * randFloat( 0.0f, 10.0f ) ) );
				colorCube->draw();
			}
		}
	
		// create the second fbo and attach its texture
		auto fbo1 = gl::Fbo::createEmpty();
		gl::ScopedFramebuffer scopedFbo( fbo1 );
		gl::frameBufferTexture( mTextureRed, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );

		// blit fbo0 to fbo1
		fbo0->blitTo( fbo1, mTextureRed->getBounds(), mTextureRed->getBounds() );
	}
	else if( event.getCode() == KeyEvent::KEY_3 ) {
		// Multisampling test
		// create two empty fbo2, attach renderbuffers to the first and the red texture to the other.
		// render cubes in the first and then blit it to the second
		auto colorBuffer = gl::Renderbuffer::create( mTextureRed->getWidth(), mTextureGreen->getHeight(), GL_RGBA8, gl::Fbo::getMaxSamples() );
		auto depthBuffer = gl::Renderbuffer::create( mTextureRed->getWidth(), mTextureGreen->getHeight(), GL_DEPTH_COMPONENT24, gl::Fbo::getMaxSamples() );
		auto fbo0 = gl::Fbo::createEmpty();
		{
			gl::ScopedFramebuffer scopedFbo( fbo0 );
			gl::framebufferRenderbuffer( colorBuffer, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 );
			gl::framebufferRenderbuffer( depthBuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT );
			gl::Fbo::checkStatus();
		
			gl::clear();

			gl::ScopedDepth scopedDepth( true );
			gl::ScopedViewport viewport( mTextureGreen->getSize() );
			gl::setMatrices( CameraPersp( mTextureGreen->getWidth(), mTextureGreen->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
			auto colorCube = gl::Batch::create( geom::Cube().colors(), gl::getStockShader( gl::ShaderDef().color() ) );
			for( int i = 0; i < 150; ++i ) {
				gl::ScopedModelMatrix scopedMat;
				gl::multModelMatrix( glm::scale( vec3( randFloat() ) ) * glm::rotate( randFloat(), randVec3() ) * glm::translate( randVec3() * randFloat( 0.0f, 10.0f ) ) );
				colorCube->draw();
			}
		}
	
		// create the second fbo and attach its texture
		auto fbo1 = gl::Fbo::createEmpty();
		gl::ScopedFramebuffer scopedFbo( fbo1 );
		gl::frameBufferTexture( mTextureGreen, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );

		// resolve the multisample anti-aliasing by blitting the multisample fbo to the regular one
		fbo0->blitTo( fbo1, mTextureGreen->getBounds(), mTextureGreen->getBounds() ); 
	}
	else if( event.getCode() == KeyEvent::KEY_4 ) {
		// Multisampling test
		// create two empty fbo2, attach renderbuffers to the first and the red texture to the other.
		// render cubes in the first and then blit it to the second
		auto immutable = true;
		auto colorBuffer = gl::Texture2d::create( mTextureRed->getWidth(), mTextureGreen->getHeight(), gl::Texture2d::Format().internalFormat( GL_RGBA8 ).samples( gl::Fbo::getMaxSamples() ).immutableStorage( immutable ) );
		auto depthBuffer = gl::Texture2d::create( mTextureRed->getWidth(), mTextureGreen->getHeight(), gl::Texture2d::Format().internalFormat( GL_DEPTH_COMPONENT24 ).samples( gl::Fbo::getMaxSamples() ).immutableStorage( immutable ) );
		auto fbo0 = gl::Fbo::createEmpty();
		{
			gl::ScopedFramebuffer scopedFbo( fbo0 );
			gl::frameBufferTexture( colorBuffer, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );
			gl::frameBufferTexture( depthBuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0 );
			if( ! gl::Fbo::checkStatus() ) console() << "INCOMPLETE!!!" << endl;
		
			gl::clear();

			gl::ScopedDepth scopedDepth( true );
			gl::ScopedViewport viewport( mTextureGreen->getSize() );
			gl::setMatrices( CameraPersp( mTextureGreen->getWidth(), mTextureGreen->getHeight(), 50.0f, 0.1f, 100.0f ).calcFraming( Sphere( vec3( 0.0f ), 2.0f ) ) );
		
			auto colorCube = gl::Batch::create( geom::Cube().colors(), gl::getStockShader( gl::ShaderDef().color() ) );
			for( int i = 0; i < 150; ++i ) {
				gl::ScopedModelMatrix scopedMat;
				gl::multModelMatrix( glm::scale( vec3( randFloat() ) ) * glm::rotate( randFloat(), randVec3() ) * glm::translate( randVec3() * randFloat( 0.0f, 10.0f ) ) );
				colorCube->draw();
			}
		}
	
		// create the second fbo and attach its texture
		auto fbo1 = gl::Fbo::createEmpty();
		gl::ScopedFramebuffer scopedFbo( fbo1 );
		gl::frameBufferTexture( mTextureBlue, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0 );

		// resolve the multisample anti-aliasing by blitting the multisample fbo to the regular one
		fbo0->blitTo( fbo1, mTextureGreen->getBounds(), mTextureGreen->getBounds() ); 
	}

	// check for errors
	CI_CHECK_GL();
}

CINDER_APP( EmptyFboApp, RendererGl( RendererGl::Options().debug() ) )
