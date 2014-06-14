/*
 * Copyright © 2014 Simple Entertainment Limited
 *
 * This file is part of Bob's Island.
 *
 * Bob's Island is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Bob's Island is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with Bob's Island. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <simplicity/API.h>
#include <simplicity/bullet/API.h>
//#include <simplicity/direct3d/API.h>
#include <simplicity/freeglut/API.h>
#include <simplicity/opengl/API.h>
//#include <simplicity/physx/API.h>
#include <simplicity/raknet/API.h>
#include <simplicity/rocket/API.h>
//#include <simplicity/winapi/API.h>

#include <the-island/API.h>

#include <bobs-island/bob/BobFactory.h>

#include "MeshLoader.h"
#include "SunMover.h"

using namespace bobsisland;
using namespace simplicity;
using namespace simplicity::bullet;
//using namespace simplicity::direct3d;
using namespace simplicity::freeglut;
using namespace simplicity::opengl;
//using namespace simplicity::physx;
using namespace simplicity::raknet;
using namespace simplicity::rocket;
//using namespace simplicity::winapi;
using namespace std;
using namespace theisland;

//#ifdef SIMPLE_WINDOWS
//void setupEngine(HINSTANCE instance, int commandShow);
//#else
void setupEngine();
//#endif
void setupScene();
void testing123(unsigned int radius);

//#ifdef SIMPLE_WINDOWS
//int CALLBACK WinMain(HINSTANCE instance, HINSTANCE /* previousInstance */, LPSTR /* commandLine */, int commandShow)
//#else
int main()
//#endif
{
	// Resources
	/////////////////////////
	unique_ptr<DataStore> consoleDataStore(new ConsoleDataStore);
	Resources::setDataStore(move(consoleDataStore), Category::CONSOLE);
	unique_ptr<DataStore> fileSystemDataStore(new FileSystemDataStore("."));
	Resources::setDataStore(move(fileSystemDataStore), Category::ALL_CATEGORIES);

	// Logging
	/////////////////////////
	Logs::setResource(Resources::get("out", Category::CONSOLE), Category::ALL_CATEGORIES);

	Logs::log(Category::INFO_LOG, "###########################");
	Logs::log(Category::INFO_LOG, "### BOB's Island Client ###");
	Logs::log(Category::INFO_LOG, "###########################");

	setRandomSeed(1234567);

	Logs::log(Category::INFO_LOG, "Setting up engine...");
	setupEngine();
	//setupEngine(instance, commandShow);
	Logs::log(Category::INFO_LOG, "Setting up scene...");
	setupScene();

	Logs::log(Category::INFO_LOG, "GO!!!");
	Simplicity::play();
}

//#ifdef SIMPLE_WINDOWS
//void setupEngine(HINSTANCE instance, int commandShow)
//#else
void setupEngine()
//#endif
{
	// Windowing
	/////////////////////////
	unique_ptr<Engine> windowingEngine(new FreeGLUTEngine("Bob's Island"));
	//unique_ptr<WinAPIEngine> windowingEngine(new WinAPIEngine("Bob's Island", instance, commandShow));

	// Messaging
	/////////////////////////
	unique_ptr<MessagingEngine> localMessagingEngine(new SimpleMessagingEngine);
	Messages::addEngine(localMessagingEngine.get());
	unique_ptr<MessagingEngine> remoteMessagingEngine(new RakNetMessagingEngine("127.0.0.1", 55501));
	Messages::addEngine(remoteMessagingEngine.get());

	// Scene Graph
	/////////////////////////
	unique_ptr<Graph> sceneGraph(new QuadTree(1, Square(128.0f), QuadTree::Plane::XZ));

	// Models
	/////////////////////////
	unique_ptr<ModelFactory> modelFactory(new OpenGLModelFactory);
	//unique_ptr<ModelFactory> modelFactory(new Direct3DModelFactory);
	ModelFactory::setInstance(move(modelFactory));

	// Scripting
	/////////////////////////
	unique_ptr<Engine> scriptingEngine(new ScriptingEngine);

	// Physics
	/////////////////////////
	unique_ptr<PhysicsFactory> physicsFactory(new BulletPhysicsFactory);
	PhysicsFactory::setInstance(move(physicsFactory));

	unique_ptr<Engine> physicsEngine(new BulletEngine(Vector3(0.0f, -10.0f, 0.0f)));

	// Rendering
	/////////////////////////
	unique_ptr<RenderingFactory> renderingFactory(new OpenGLRenderingFactory);
	//unique_ptr<RenderingFactory> renderingFactory(new Direct3DRenderingFactory);
	RenderingFactory::setInstance(move(renderingFactory));

	unique_ptr<RenderingEngine> renderingEngine(new OpenGLRenderingEngine);
	unique_ptr<Renderer> renderer(new OpenGLRenderer);
	//unique_ptr<RenderingEngine> renderingEngine(new Direct3DRenderingEngine);
	//unique_ptr<Renderer> renderer(new Direct3DRenderer);

	// Shaders
	Resource* vertexShaderSource = Resources::get("src/main/glsl/my.vs", Category::UNCATEGORIZED);
	Resource* fragmentShaderSource = Resources::get("src/main/glsl/my.fs", Category::UNCATEGORIZED);
	unique_ptr<OpenGLShader> vertexShader(new OpenGLShader(Shader::Type::VERTEX, *vertexShaderSource));
	unique_ptr<OpenGLShader> fragmentShader(new OpenGLShader(Shader::Type::FRAGMENT, *fragmentShaderSource));
	unique_ptr<Pipeline> pipeline(new OpenGLPipeline(move(vertexShader), move(fragmentShader)));
	//Resource* vertexShaderSource = Resources::get("vertexDefault.cso", Category::UNCATEGORIZED, true);
	//Resource* fragmentShaderSource = Resources::get("fragmentDefault.cso", Category::UNCATEGORIZED, true);
	//unique_ptr<Pipeline> pipeline(new Direct3DPipeline(*vertexShaderSource, *fragmentShaderSource));
	renderer->setDefaultPipeline(move(pipeline));

	// UI
	/////////////////////////
	unique_ptr<Renderer> uiRenderer(new OpenGLRenderer);
	//unique_ptr<Renderer> uiRenderer(new Direct3DRenderer);
	uiRenderer->setClearColorBuffer(false);

	Resource* uiVertexShaderSource = Resources::get("src/main/glsl/rocket.vs", Category::UNCATEGORIZED);
	Resource* uiFragmentShaderSource = Resources::get("src/main/glsl/rocket.fs", Category::UNCATEGORIZED);
	unique_ptr<OpenGLShader> uiVertexShader(new OpenGLShader(Shader::Type::VERTEX, *uiVertexShaderSource));
	unique_ptr<OpenGLShader> uiFragmentShader(new OpenGLShader(Shader::Type::FRAGMENT, *uiFragmentShaderSource));
	unique_ptr<Pipeline> uiPipeline(new OpenGLPipeline(move(uiVertexShader), move(uiFragmentShader)));
	//Resource* uiVertexShaderSource = Resources::get("vertexRocket.cso", Category::UNCATEGORIZED, true);
	//Resource* uiFragmentShaderSource = Resources::get("fragmentRocket.cso", Category::UNCATEGORIZED, true);
	//unique_ptr<Pipeline> uiPipeline(new Direct3DPipeline(*uiVertexShaderSource, *uiFragmentShaderSource));
	uiRenderer->setDefaultPipeline(move(uiPipeline));

	unique_ptr<Engine> uiEngine(new RocketEngine(move(uiRenderer), Category::UNCATEGORIZED));

	// Assemble the rendering engine.
	/////////////////////////
	renderingEngine->addRenderer(move(renderer));
	renderingEngine->setGraph(sceneGraph.get());

	// Debugging
	/////////////////////////
	unique_ptr<DebugSerialCompositeEngine> debuggingEngine(new DebugSerialCompositeEngine);
	unique_ptr<Entity> debug(new Entity);
	Resource* consoleResource = Resources::get("src/main/rml/console.rml", Category::UNCATEGORIZED);
	unique_ptr<RocketDocument> console(new RocketConsole(*consoleResource, debuggingEngine.get()));
	Resource* consoleFontResource = Resources::get("src/main/resources/fonts/Ubuntu-Regular.ttf", Category::UNCATEGORIZED);
	unique_ptr<RocketFontFace> consoleFont(new RocketFontFace(*consoleFontResource));
	debug->addUniqueComponent(move(console));
	debug->addUniqueComponent(move(consoleFont));

	// Add everything!
	/////////////////////////
	Simplicity::setCompositeEngine(move(debuggingEngine));
	Simplicity::addEngine(move(windowingEngine));
	Simplicity::addEngine(move(localMessagingEngine));
	Simplicity::addEngine(move(remoteMessagingEngine));
	Simplicity::addEngine(move(scriptingEngine));
	Simplicity::addEngine(move(physicsEngine));
	Simplicity::addEngine(move(renderingEngine));
	Simplicity::addEngine(move(uiEngine));

	unique_ptr<Scene> theOnlyScene(new Scene);
	Simplicity::addScene("theOnly", move(theOnlyScene));

	Simplicity::getScene()->addGraph(move(sceneGraph));

	Simplicity::getScene()->addEntity(move(debug));
}

void setupScene()
{
	// Mesh Loader! (TODO review)
	/////////////////////////
	unique_ptr<Entity> meshLoader(new Entity);
	unique_ptr<MeshLoader> meshLoaderComponent(new MeshLoader);
	meshLoader->addUniqueComponent(move(meshLoaderComponent));

	// Bob!
	/////////////////////////
	unique_ptr<Entity> bob = BobFactory::createBob();
	Entity* rawBob = bob.get();

	// Camera
	/////////////////////////
	unique_ptr<Camera> camera(new Camera);
	camera->setFarClippingDistance(2000.0f);
	camera->setPerspective(60.0f, 4.0f / 3.0f);
	// Position is relative to Bob.
	translate(camera->getTransform(), Vector3(0.0f, 1.11f, -0.21f));

	unique_ptr<Model> cameraBounds(new Square(32.0f));
	setPosition(cameraBounds->getTransform(), Vector3(0.0f, 0.0f, -32.0f));
	cameraBounds->setCategory(Category::BOUNDS);

	// The Sun
	/////////////////////////
	unique_ptr<Entity> theSun(new Entity);

	unique_ptr<Light> sunLight(new Light("theSun"));
	sunLight->setAmbient(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	sunLight->setDiffuse(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	sunLight->setRange(1000.0f);
	sunLight->setSpecular(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	sunLight->setStrength(32.0f);

	unique_ptr<Mesh> sunModel = ModelFactory::getInstance().createSphereMesh(50.0f, 10, Vector4(1.0f, 1.0f, 0.6f));
	for (unsigned int index = 0; index < sunModel->getVertexCount(); index++)
	{
		sunModel->getVertices()[index].normal.negate();
	}

	theSun->addUniqueComponent(move(sunLight));
	theSun->addUniqueComponent(move(sunModel));

	// The flashlight
	/////////////////////////
	unique_ptr<Entity> flash(new Entity);

	unique_ptr<Light> flashLight(new Light("flash"));
	flashLight->setAmbient(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	flashLight->setAttenuation(Vector3(0.5f, 0.05f, 0.0f));
	flashLight->setDiffuse(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	flashLight->setDirection(Vector3(0.0f, 0.0f, -1.0f));
	flashLight->setRange(50.0f);
	flashLight->setSpecular(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	flashLight->setStrength(32.0f);

	// Update the rendering engine.
	/////////////////////////
	RenderingEngine* renderingEngine = Simplicity::getEngine<RenderingEngine>();
	renderingEngine->addLight(*theSun);
	//renderingEngine->addLight(*flash);
	renderingEngine->setCamera(rawBob);

	// The Island!
	/////////////////////////
	unsigned int radius = 64;
	vector<float> profile;
	profile.reserve(radius * 2);
	float peakHeight = 32.0f;
	for (unsigned int index = 0; index < radius * 2; index++)
	{
		// Drop below sea level outside of the radius.
		if (index > radius)
		{
			profile.push_back((float) radius - index);
			continue;
		}

		// The cone.
		//profile.push_back(peakHeight - index * (peakHeight / radius));

		// The bagel.
		//profile.push_back(peakHeight * ((sinf(index / 5.5f) + 1.0f) / 4.0f));

		// Mountains and beaches.
		profile.push_back(peakHeight * static_cast<float>(pow(radius - index, 3) / pow(radius, 3)));
	}
	IslandFactory::createIsland(radius, profile);

	// Assemble Bob!
	/////////////////////////
	rotate(bob->getTransform(), MathConstants::PI * 0.5f, Vector3(0.0f, 1.0f, 0.0f));
	setPosition(bob->getTransform(), Vector3(0.0f, 0.0f, radius - 1.0f));
	bob->addUniqueComponent(move(camera));
	bob->addUniqueComponent(move(cameraBounds)); // Yes, this is odd...

	// Assemble the Sun!
	/////////////////////////
	unique_ptr<Script> sunMover(new SunMover(*flashLight));
	theSun->addUniqueComponent(move(sunMover));

	// Assemble the Flash Light!
	/////////////////////////
	flash->addUniqueComponent(move(flashLight));

	// Testing 123
	/////////////////////////
	//unique_ptr<FlyingCameraEngine> flyingCameraEngine(new FlyingCameraEngine(*bob.get()));
	//Simplicity::addEngine(move(flyingCameraEngine));
	//testing123(radius);

	// Add everything!
	/////////////////////////
	Simplicity::getScene()->addEntity(move(bob));
	Simplicity::getScene()->addEntity(move(meshLoader));
	Simplicity::getScene()->addEntity(move(theSun));
	Simplicity::getScene()->addEntity(move(flash), *rawBob);
}

void testing123(unsigned int radius)
{

	unique_ptr<Entity> test(new Entity);
	setPosition(test->getTransform(), Vector3(0.0f, 2.0f, radius - 5.0f));

	unique_ptr<Mesh> cube = ModelFactory::getInstance().createCubeMesh(1.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	Matrix44 relativeTransform;
	relativeTransform.setIdentity();
	setPosition(relativeTransform, Vector3(1.0f, 1.0f, 1.0f));
	unique_ptr<Model> cubeMinusCube = ModelFunctions::subtract(*cube, *cube, relativeTransform);

	unique_ptr<Mesh> cylinder = ModelFactory::getInstance().createCylinderMesh(0.3f, 10.0f, 5,
			Vector4(1.0f, 0.0f, 0.0f, 1.0f), false);

	unique_ptr<Mesh> hemisphere = ModelFactory::getInstance().createHemisphereMesh(1.0f, 10,
			Vector4(1.0f, 0.0f, 0.0f, 1.0f), true);

	unique_ptr<Mesh> prism = ModelFactory::getInstance().createPrismMesh(Vector3(0.5f, 0.5f, 0.5f),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	unique_ptr<Mesh> sphere = ModelFactory::getInstance().createSphereMesh(1.0f, 10, Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			true);

	unique_ptr<Mesh> triangle = ModelFactory::getInstance().createTriangleMesh(Vector3(0.0f, 1.0f, 0.0f),
			Vector3(-1.0f, -2.0f, 0.0f), Vector3(1.0f, -2.0f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	setPosition(relativeTransform, Vector3(0.0f, 0.0f, 5.0f));
	unique_ptr<Model> triangleMinusCylinder = ModelFunctions::subtract(*triangle, *cylinder, relativeTransform);
	unique_ptr<Model> prismMinusCylinder = ModelFunctions::subtract(*prism, *cylinder, relativeTransform);

	unique_ptr<Model> bounds(new Square(1.0f));
	bounds->setCategory(Category::BOUNDS);

	//test->addUniqueComponent(move(cube));
	test->addUniqueComponent(move(cubeMinusCube));
	//test->addUniqueComponent(move(cylinder));
	//test->addUniqueComponent(move(hemisphere));
	//test->addUniqueComponent(move(prism));
	//test->addUniqueComponent(move(sphere));
	//test->addUniqueComponent(move(triangle));
	//test->addUniqueComponent(move(triangleMinusCylinder));
	//test->addUniqueComponent(move(prismMinusCylinder));
	test->addUniqueComponent(move(bounds));
	Simplicity::getScene()->addEntity(move(test));
}
