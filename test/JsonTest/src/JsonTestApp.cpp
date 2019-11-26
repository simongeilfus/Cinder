#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Json.h"

using namespace ci;
using namespace ci::app;
using namespace ci::json;
using namespace std;

void basicTests();
void customTypeTests();

class JsonTestApp : public App {
public:
	JsonTestApp()
	{
		basicTests();
		customTypeTests();

		quit();
	}
};

void basicTests()
{
	// Basic cinder types test
	{
		Json j;
		j["position"] = vec2( 1, 2 );
		j["bounds"] = Rectf( vec2( 100 ), vec2( 200 ) );
		j["color"] = ColorA( 1, 0, 0, 1 );

		console() << j.dump( 1, '\t' ) << endl << endl;
	}

	// Basic I/O tests
	{
		fs::path jsonPath = getAppPath() / "test.json";

		Json outputJson = {
			{ "position", vec2( 1, 2 ) },
		{ "bounds", Rectf( vec2( 100 ), vec2( 200 ) ) },
		{ "color", ColorA( 1, 0, 0, 1 ) }
		};
		writeJson( jsonPath, outputJson, 1, '\t', true );

		Json inputJson = loadJson( loadFile( jsonPath ) );
		vec2 position = inputJson["position"];
		Rectf bounds = inputJson["bounds"];
		ColorA color = inputJson["color"];

		console() << "Parsing json from file" << endl;
		console() << "position\t" << position << endl;
		console() << "bounds\t\t" << bounds << endl;
		console() << "color\t\t" << color << endl << endl;
	}

	// Parsing from string with comments
	{
		Json j = parseJson( R"(
				/* Comments will be stripped if "stripComments" is set to true 
				Otherwise nlohmann::json::parse will throw as this is not supported
				*/

				{
					// some position
					"position": [ 1.0, 2.0 ],
					// some Rectf
					"bounds": [ 100.0, 100.0, 200.0, 200.0 ],
					// some color
					"color": [ 1.0, 0.0, 0.0, 1.0 ]
				}
			)", /*stripComments=*/ true );

		vec2 position = j["position"];
		Rectf bounds = j["bounds"];
		ColorA color = j["color"];

		console() << "Parsing json from string" << endl;
		console() << "position\t" << position << endl;
		console() << "bounds\t\t" << bounds << endl;
		console() << "color\t\t" << color << endl << endl;
	}
}

// dummy custom type
struct Object {
	std::string name;
	ci::vec3 position;
	ci::ColorA color;
	bool operator==( const Object &rhs ) const { return name == rhs.name && position == rhs.position && color == rhs.color; }
};

// serialization functions need to be in the same namespace as the type, for third-party types see "adl_serializer" in the official documentation
void to_json( ci::json::Json &j, const Object &o ) { j = ci::json::Json { { "name", o.name }, { "position", o.position }, { "color", o.color } }; }
void from_json( const ci::json::Json &j, Object &o ) { o = { j["name"], j["position"], j["color"] };	}

void customTypeTests()
{
	// Custom types and std::vector support
	{
		vector<Object> objects;
		for( size_t i = 0; i < 5; ++i ) {
			Object o;
			o.name = "object_" + to_string( i );
			o.position = vec3( 0, 0, i * 10.0f );
			o.color = ColorA::white();
			objects.push_back( o );
		}

		Json j;
		j["objects"] = objects;

		console() << "Custom type: " << j.dump() << endl;

		vector<Object> objectsCopy = j["objects"];
		console() << "Custom type deserialization: " << std::boolalpha << ( objects == objectsCopy ) << endl << endl;
	}

	// shared_ptr test
	{
		using ObjectRef = shared_ptr<Object>;
		vector<ObjectRef> objects;
		for( size_t i = 0; i < 5; ++i ) {
			ObjectRef o = make_shared<Object>();
			o->name = "object_" + to_string( i );
			o->position = vec3( 0, 0, i * 10.0f );
			o->color = ColorA::white();
			objects.push_back( o );
		}

		Json j;
		j["objects"] = objects;
		vector<ObjectRef> objectsCopy = j["objects"];
	}
}

CINDER_APP( JsonTestApp, RendererGl )
