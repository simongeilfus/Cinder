#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#define CATCH_CONFIG_NOSTDOUT
namespace Catch {
	std::ostream &cout() { return ci::app::console(); }
	std::ostream &cerr() { return ci::app::console(); }
}

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

//____________________________________________________________________________________________________________
TEST_CASE( "1. Default Color Texture / No Depth" ) {
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().disableDepth() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}

	
//____________________________________________________________________________________________________________
TEST_CASE( "2. Default Color Texture / Default Depth Buffer" ) {
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "3. Default Color Texture / Default Depth Texture" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture() );

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "4. Default Color Texture / Custom Depth Buffer" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer( GL_DEPTH_COMPONENT24 ) );

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "5. Default Color Texture / Custom Depth Texture" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture( gl::Texture2d::Format().internalFormat( GL_DEPTH_COMPONENT24 ) ) );

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->getDepthTexture()->getInternalFormat() == GL_DEPTH_COMPONENT24 );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "6. Default Color Texture / Default Depth Buffer / Default stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer().stencilBuffer() );

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "7. Default Color Texture / Custom Depth Buffer / Default stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture()
								.depthBuffer( GL_DEPTH_COMPONENT24 ).stencilBuffer() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "8. Default Color Texture / Default Depth Texture / Default stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture().stencilBuffer() );

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "9. Custom Color Texture 2D / No Depth / No stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( 512, 512 ) )
								.disableDepth() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "10. 3D Fbo: Custom Color Texture 3D / No Depth / No stencil" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 256 ) )
								.disableDepth() );
				
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "11. Layered Fbo: Custom Color Texture 2D Array / No Depth / No stencil" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 16, gl::Texture3d::Format().target( GL_TEXTURE_2D_ARRAY ) ) )
								.disableDepth() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "12. Custom Color Texture 2D / Default Depth Buffer / No stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( 512, 512 ) ) );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "13. Custom Color Texture 2D / Default Depth Texture / No stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( 512, 512 ) )
								.depthTexture() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "14. 3D Fbo: Custom Color Texture 3D / Default Depth Buffer (Skip because Tex3D doesn't support Depth) / No stencil" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 256 ) ) );
				
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() ); // texture3d can't have a stencil buffer
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "15. Layered Fbo: Custom Color Texture 2D Array / Default Depth Buffer / No stencil" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 16, gl::Texture3d::Format().target( GL_TEXTURE_2D_ARRAY ) ) ) );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_DEPTH_ATTACHMENT ) ) );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "16. Layered Fbo: Custom Color Texture 2D Array / Default Depth Texture / No stencil" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 16, gl::Texture3d::Format().target( GL_TEXTURE_2D_ARRAY ) ) )
								.depthTexture() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_DEPTH_ATTACHMENT ) ) );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "17. Layered Fbo: Custom Color Texture 2D Array / Default Depth Buffer / Default stencil Buffer" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 16, gl::Texture3d::Format().target( GL_TEXTURE_2D_ARRAY ) ) )
								.stencilBuffer() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "18. Layered Fbo: Custom Color Texture 2D Array / Default Depth Texture / Default stencil Buffer" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 16, gl::Texture3d::Format().target( GL_TEXTURE_2D_ARRAY ) ) )
								.depthTexture()
								.stencilBuffer() 
	);
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_DEPTH_STENCIL_ATTACHMENT ) ) );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "19. Stencil Fbo: Default Color Texture / No Depth / Default stencil Buffer" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format().stencilBuffer().disableDepth() );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "20. Stencil Fbo: Custom Color Texture / No Depth / Default stencil Buffer" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format().stencilBuffer().disableDepth()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( 256, 256 ) ) );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
}
	
//____________________________________________________________________________________________________________
// I don't see a point of a texture stencil attachment but specs say that it is allowed 
// https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
TEST_CASE( "21. Stencil 3d Fbo: Custom 3d Texture / No Depth / Default stencil Buffer" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format().stencilBuffer().disableDepth()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 64 ) ) );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() ); // texture3d can't have a stencil buffer
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_COLOR_ATTACHMENT0 ) ) );
}
	
//____________________________________________________________________________________________________________
// I don't see a point of a texture stencil attachment but specs say that it is allowed 
// https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
TEST_CASE( "22. Stencil Layered Fbo: Custom 2d Texture Array / No Depth / Default stencil Buffer" ) { 
	auto fbo = gl::Fbo::create( 256, 256, gl::Fbo::Format().stencilBuffer().disableDepth()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture3d::create( 256, 256, 64, gl::Texture3d::Format().target( GL_TEXTURE_2D_ARRAY ) ) ) );
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( ! fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
	REQUIRE( dynamic_pointer_cast<gl::Texture3d>( fbo->getTextureBase( GL_STENCIL_ATTACHMENT ) ) );
}

// Multisampling
//____________________________________________________________________________________________________________

//____________________________________________________________________________________________________________
TEST_CASE( "23. Multisampling: Default Color Texture / No Depth" ) {
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().disableDepth().samples( 4 ) );
	fbo->markAsDirty();
	fbo->resolveTextures();

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
	REQUIRE( fbo->getMultisampleId() );
}

	
//____________________________________________________________________________________________________________
TEST_CASE( "24. Multisampling: Default Color Texture / Default Depth Buffer" ) {
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer().samples( 4 ) );
	fbo->markAsDirty();
	fbo->resolveTextures();
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
	REQUIRE( fbo->getMultisampleId() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "25. Multisampling: Default Color Texture / Default Depth Texture" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture().samples( 4 ) );
	fbo->markAsDirty();
	fbo->resolveTextures();

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
	REQUIRE( fbo->getMultisampleId() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "26. Multisampling: Default Color Texture / Custom Depth Buffer" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer( GL_DEPTH_COMPONENT24 ).samples( 4 ) );
	fbo->markAsDirty();
	fbo->resolveTextures();

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasStencilAttachment() );
	REQUIRE( fbo->getMultisampleId() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "27. Multisampling: Default Color Texture / Custom Depth Texture" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture( gl::Texture2d::Format().internalFormat( GL_DEPTH_COMPONENT24 ) ).samples( 4 ) );
	fbo->markAsDirty();
	fbo->resolveTextures();

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->getDepthTexture()->getInternalFormat() == GL_DEPTH_COMPONENT24 );
	REQUIRE( ! fbo->hasStencilAttachment() );
	REQUIRE( fbo->getMultisampleId() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "28. Multisampling: Default Color Texture / Default Depth Buffer / Default stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthBuffer().stencilBuffer().samples( 4 ) );
	fbo->markAsDirty();
	fbo->resolveTextures();

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
	REQUIRE( fbo->getMultisampleId() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "29. Multisampling: Default Color Texture / Custom Depth Buffer / Default stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture()
								.depthBuffer( GL_DEPTH_COMPONENT24 ).stencilBuffer().samples( 4 ) );
	fbo->markAsDirty();
	fbo->resolveTextures();
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
	REQUIRE( fbo->getMultisampleId() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "30. Multisampling: Default Color Texture / Default Depth Texture / Default stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format().colorTexture().depthTexture().stencilBuffer().samples( 4 ) );
	fbo->markAsDirty();
	fbo->resolveTextures();

	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( fbo->getDepthTexture() );
	REQUIRE( fbo->hasDepthAttachment() );
	REQUIRE( fbo->hasStencilAttachment() );
	REQUIRE( fbo->getMultisampleId() );
}
	
//____________________________________________________________________________________________________________
TEST_CASE( "31. Multisampling: Custom Color Texture 2D / No Depth / No stencil" ) { 
	auto fbo = gl::Fbo::create( 512, 512, gl::Fbo::Format()
								.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( 512, 512 ) )
								.disableDepth().samples( 4 ) );
	fbo->markAsDirty();
	fbo->resolveTextures();
		
	REQUIRE( gl::getError() == GL_NO_ERROR );
	REQUIRE( gl::Fbo::checkStatus() );
	REQUIRE( fbo->getColorTexture() );
	REQUIRE( ! fbo->getDepthTexture() );
	REQUIRE( ! fbo->hasDepthAttachment() );
	REQUIRE( ! fbo->hasStencilAttachment() );
	REQUIRE( fbo->getMultisampleId() );
}

class FboTestSuite : public App {
public:
	void setup()
	{
		Catch::Session session;
		//session.configData().showSuccessfulTests = true;
		session.configData().useColour = Catch::UseColour::Yes;
		session.run();
	}
};

CINDER_APP( FboTestSuite, RendererGl( RendererGl::Options().debugBreak( GL_DEBUG_SEVERITY_LOW ).debugLog( GL_DEBUG_SEVERITY_LOW ) ), []( App::Settings *settings ) {
	settings->setConsoleWindowEnabled();
	settings->setWindowSize( ivec2( 0, 0 ) );
} )
