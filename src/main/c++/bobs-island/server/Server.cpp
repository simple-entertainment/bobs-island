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

// Use alternate APIs:
//#define DIRECT3D
//#define PHYSX

// Use alternate Renderers:
//#define MULTI_DRAW

#include <libnoise/noise.h>

#include <simplicity/API.h>
#include <simplicity/editor/API.h>
#include <simplicity/live555/API.h>
#include <simplicity/raknet/API.h>
#include <simplicity/rocket/API.h>
#include <simplicity/terrain/API.h>

#ifdef DIRECT3D
#include <simplicity/direct3d/API.h>
#include <simplicity/winapi/API.h>
#else
#include <simplicity/freeglut/API.h>
#include <simplicity/opengl/API.h>
#endif

#ifdef PHYSX
#include <simplicity/physx/API.h>
#else
#include <simplicity/bullet/API.h>
#endif

#include "bob/BobFactory.h"
#include "ServerEngine.h"
#include "SunMover.h"

using namespace bobsisland;
using namespace bobsisland::server;
using namespace noise::module;
using namespace simplicity;
using namespace simplicity::editor;
using namespace simplicity::live555;
using namespace simplicity::raknet;
using namespace simplicity::rocket;
using namespace simplicity::terrain;
using namespace std;

#ifdef DIRECT3D
using namespace simplicity::direct3d;
using namespace simplicity::winapi;
#else
using namespace simplicity::freeglut;
using namespace simplicity::opengl;
#endif

#ifdef PHYSX
using namespace simplicity::simphysx;
#else
using namespace simplicity::bullet;
#endif

float getHeight(int x, int y);
void setupEngine();
void setupScene();
void start();
void testing123(unsigned int radius);

// Island generation
Perlin perlinNoise;
int mapSize = 1024;
int halfMapSize = mapSize / 2;
float peakHeight = 100.0f;
float peakAmplitude = 50.0f;
int perlinFrequency = 128;

#ifdef DIRECT3D
int commandShow = 0;
HINSTANCE instance = nullptr;

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE /* previousInstance */, LPSTR /* commandLine */, int commandShow)
{
	::commandShow = commandShow;
	::instance = instance;

	start();
}
#else

float getHeight(int x, int y)
{
	float height = static_cast<float>(perlinNoise.GetValue(static_cast<double>(x) / perlinFrequency, 0.5,
														   static_cast<double>(y) / perlinFrequency)) + 1.0f;

	height /= 2.0f;
	float distanceFromOrigin = static_cast<float>(Vector2i(x - halfMapSize, y - halfMapSize).getMagnitude());
	float fractionFromOrigin = (1.0f - distanceFromOrigin / static_cast<float>(halfMapSize));
	height *= fractionFromOrigin * peakAmplitude;
	height += fractionFromOrigin * peakHeight - peakAmplitude;
	return height;
}

int main()
{
	start();

	/*unique_ptr<MessagingEngine> remoteMessagingEngine(new RakNetMessagingEngine(55501, 16));
	Messages::addEngine(remoteMessagingEngine.get());
	remoteMessagingEngine->onPlay();

	while (true)
	{
		remoteMessagingEngine->advance();
		this_thread::sleep_for(chrono::milliseconds(100));
	}*/
}
#endif

void setupEngine()
{
	// Windowing
	/////////////////////////
#ifdef DIRECT3D
	unique_ptr<WinAPIEngine> windowingEngine(new WinAPIEngine("Bob's Island", instance, commandShow));
#else
	unique_ptr<Engine> windowingEngine(new FreeGLUTEngine("Bob's Island"));
#endif
	Simplicity::addEngine(move(windowingEngine));

	// Messaging
	/////////////////////////
	unique_ptr<MessagingEngine> localMessagingEngine(new SimpleMessagingEngine);
	Messages::addEngine(localMessagingEngine.get());
	Simplicity::addEngine(move(localMessagingEngine));

	unique_ptr<MessagingEngine> remoteMessagingEngine(new RakNetMessagingEngine(55501, 16));
	Messages::addEngine(remoteMessagingEngine.get());
	Simplicity::addEngine(move(remoteMessagingEngine));

	unique_ptr<Codec> keyboardButtonCodec(new SimpleCodec<KeyboardButtonEvent>);
	Messages::setCodec(Subject::KEYBOARD_BUTTON, move(keyboardButtonCodec));
	unique_ptr<Codec> mouseButtonCodec(new SimpleCodec<MouseButtonEvent>);
	Messages::setCodec(Subject::MOUSE_BUTTON, move(mouseButtonCodec));
	unique_ptr<Codec> mouseMoveCodec(new SimpleCodec<MouseMoveEvent>);
	Messages::setCodec(Subject::MOUSE_MOVE, move(mouseMoveCodec));

	// Models
	/////////////////////////
#ifdef DIRECT3D
	unique_ptr<ModelFactory> modelFactory(new Direct3DModelFactory);
#else
	unique_ptr<ModelFactory> modelFactory(new OpenGLModelFactory);
#endif
	ModelFactory::setInstance(move(modelFactory));

	// Scripting
	/////////////////////////
	unique_ptr<Engine> scriptingEngine(new ScriptingEngine);
	Simplicity::addEngine(move(scriptingEngine));

	// Physics
	/////////////////////////
#ifdef PHYSX
	unique_ptr<PhysXEngine> physicsEngine(new PhysXEngine(Vector3(0.0f, -10.0f, 0.0f)));
	unique_ptr<PhysXPhysicsFactory> physicsFactory(new PhysXPhysicsFactory(physicsEngine->getPhysics(),
			physicsEngine->getCooking()));
#else
	unique_ptr<PhysicsFactory> physicsFactory(new BulletPhysicsFactory);
	unique_ptr<Engine> physicsEngine(new BulletEngine(Vector3(0.0f, -10.0f, 0.0f)));
#endif
	PhysicsFactory::setInstance(move(physicsFactory));
	Simplicity::addEngine(move(physicsEngine));

	// Game Logic
	/////////////////////////
	unique_ptr<Engine> serverEngine(new ServerEngine);
	Simplicity::addEngine(move(serverEngine));

	// Rendering
	/////////////////////////
#ifdef DIRECT3D
	unique_ptr<RenderingFactory> renderingFactory(new Direct3DRenderingFactory);
	unique_ptr<RenderingEngine> renderingEngine(new Direct3DRenderingEngine);
	unique_ptr<Renderer> renderer(new Direct3DRenderer);
#else
	unique_ptr<RenderingFactory> renderingFactory(new OpenGLRenderingFactory);
	unique_ptr<OpenGLRenderingEngine> renderingEngine(new OpenGLRenderingEngine);
#  ifdef MULTI_DRAW
	unique_ptr<Renderer> renderer(new MultiDrawOpenGLRenderer);
#  else
	unique_ptr<Renderer> renderer(new SimpleOpenGLRenderer);
#  endif
#endif
	RenderingFactory::setInstance(move(renderingFactory));

#ifndef BOB_AS_CLIENT
	// Frame buffer
	unique_ptr<OpenGLTexture> renderTexture(new OpenGLTexture(new char[1024 * 768 * 3], 1024, 768, PixelFormat::RGB));
	Texture* rawRenderTexture = renderTexture.get();
	unique_ptr<OpenGLFrameBuffer> frameBuffer(new OpenGLFrameBuffer(move(renderTexture)));
	renderingEngine->setFrameBuffer(move(frameBuffer));
#endif

	// Shaders
#ifdef DIRECT3D
	Resource* vertexShaderSource = Resources::get("vertexDefault.cso", Category::UNCATEGORIZED, true);
	Resource* fragmentShaderSource = Resources::get("fragmentDefault.cso", Category::UNCATEGORIZED, true);
	unique_ptr<Pipeline> pipeline(new Direct3DPipeline(*vertexShaderSource, *fragmentShaderSource));
#else
#  ifdef MULTI_DRAW
	Resource* vertexShaderSource = Resources::get("src/main/glsl/vertexMultiDraw.glsl", Category::UNCATEGORIZED);
#  else
	Resource* vertexShaderSource = Resources::get("src/main/glsl/vertexDefault.glsl", Category::UNCATEGORIZED);
#  endif
	Resource* fragmentShaderSource = Resources::get("src/main/glsl/fragmentDefault.glsl", Category::UNCATEGORIZED);
	unique_ptr<OpenGLShader> vertexShader(new OpenGLShader(Shader::Type::VERTEX, *vertexShaderSource));
	unique_ptr<OpenGLShader> fragmentShader(new OpenGLShader(Shader::Type::FRAGMENT, *fragmentShaderSource));
	unique_ptr<Pipeline> pipeline(new OpenGLPipeline(move(vertexShader), move(fragmentShader)));
#endif
	renderer->setDefaultPipeline(move(pipeline));

#ifndef BOB_AS_CLIENT
	// Sreaming
	/////////////////////////
	unique_ptr<Live555ServerEngine> streamingEngine(new Live555ServerEngine);
	unique_ptr<Source> textureSource(new TextureSource(*rawRenderTexture));
	streamingEngine->setSource(move(textureSource));
	Simplicity::addEngine(move(streamingEngine));
#endif

	// Scene Graph
	/////////////////////////
	unique_ptr<Graph> sceneGraph(new QuadTree(1, Square(128.0f), QuadTree::Plane::XZ));
	renderingEngine->setGraph(sceneGraph.get());
	Simplicity::getScene()->addGraph(move(sceneGraph));

	// Assemble the rendering engine.
	/////////////////////////
	renderingEngine->addRenderer(move(renderer));
	Simplicity::addEngine(move(renderingEngine));
}

void setupScene()
{
	// Starting Camera
	/////////////////////////
	unique_ptr<Entity> startingCamera(new Entity);
	translate(startingCamera->getTransform(), Vector3(0.0f, 10.0f, 128.0f));
	unique_ptr<Camera> cameraComponent(new Camera);
	cameraComponent->setFarClippingDistance(2000.0f);
	cameraComponent->setPerspective(60.0f, 4.0f / 3.0f);
	startingCamera->addUniqueComponent(move(cameraComponent));

	unique_ptr<Model> cameraBounds(new Square(32.0f));
	setPosition(cameraBounds->getTransform(), Vector3(0.0f, 0.0f, -32.0f));
	cameraBounds->setCategory(Category::BOUNDS);
	startingCamera->addUniqueComponent(move(cameraBounds));

	// The Sun
	/////////////////////////
	unique_ptr<Entity> theSun(new Entity);

	unique_ptr<Light> sunLight(new Light("theSun"));
	sunLight->setAmbient(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	sunLight->setDiffuse(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	sunLight->setRange(1000.0f);
	sunLight->setSpecular(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	sunLight->setStrength(32.0f);

	unique_ptr<Mesh> sunModel = ModelFactory::getInstance()->createSphereMesh(50.0f, 10, shared_ptr<MeshBuffer>(),
			Vector4(1.0f, 1.0f, 0.6f, 1.0f));
	MeshData& sunModelData = sunModel->getData(false);
	for (unsigned int index = 0; index < sunModelData.vertexCount; index++)
	{
		sunModelData.vertexData[index].normal.negate();
	}
	sunModel->releaseData();

	// Initially up in the sky directly above!
	Vector3 position(sin(0.0f), cos(0.0f), 0.0f);
	Vector3 direction = position;
	direction.negate();
	position *= 1000.0f;

	setPosition(theSun->getTransform(), position);
	sunLight->setDirection(direction);

	theSun->addUniqueComponent(move(sunLight));
	theSun->addUniqueComponent(move(sunModel));

	// Update the rendering engine.
	/////////////////////////
	RenderingEngine* renderingEngine = Simplicity::getEngine<RenderingEngine>();
	renderingEngine->addLight(*theSun);
	renderingEngine->setCamera(startingCamera.get());

	// The Island!
	/////////////////////////
	//Resource* terrainFile = Resources::create("island.terrain", Category::UNCATEGORIZED, true);
	//TerrainFactory::createFlatTerrain(*terrainFile, Vector2ui(mapSize, mapSize), getHeight, { 1, 4, 16 });

	// Assemble the Sun!
	/////////////////////////
	unique_ptr<Script> sunMover(new SunMover);
	theSun->addUniqueComponent(move(sunMover));

	// Testing 123
	/////////////////////////
	//unique_ptr<GodCameraController> flyingCameraEngine(new GodCameraController(*bob.get()));
	//Simplicity::addEngine(move(flyingCameraEngine));
	//testing123(radius);

	// Add everything!
	/////////////////////////
	Simplicity::getScene()->addEntity(move(startingCamera));
	Simplicity::getScene()->addEntity(move(theSun));

	// AK!!!
	/////////////////////////
	/*unique_ptr<Entity> ak47(new Entity);
	setPosition(ak47->getTransform(), Vector3(0.0f, 2.0f, 62.0f));
	ak47->addUniqueComponent(ModelFactory::getInstance()->loadObj(*Resources::get("res/ak47.obj",
			Category::UNCATEGORIZED)));
	Simplicity::getScene()->addEntity(move(ak47));*/
}

void start()
{
	Logs::info("bobs-island-server", "###########################");
	Logs::info("bobs-island-server", "### BOB's Island Server ###");
	Logs::info("bobs-island-server", "###########################");

	setRandomSeed(1234567);

	Editor::setup();

	Logs::info("bobs-island-server", "Setting up engine...");
	setupEngine();
	Logs::info("bobs-island-server", "Setting up scene...");
	setupScene();

	Logs::info("bobs-island-server", "GO!!!");
#ifdef DIRECT3D
	unique_ptr<Renderer> editorRenderer(new Direct3DRenderer);
#else
	unique_ptr<Renderer> editorRenderer(new SimpleOpenGLRenderer);
#endif
	editorRenderer->setClearColorBuffer(false);

#ifdef DIRECT3D
	Resource* uiVertexShaderSource = Resources::get("vertexRocket.cso", Category::UNCATEGORIZED, true);
	Resource* uiFragmentShaderSource = Resources::get("fragmentRocket.cso", Category::UNCATEGORIZED, true);
	unique_ptr<Pipeline> editorPipeline(new Direct3DPipeline(*uiVertexShaderSource, *uiFragmentShaderSource));
#  else
	Resource* vertexShaderSource = Resources::get("src/main/glsl/vertexRocket.glsl", Category::UNCATEGORIZED);
	Resource* fragmentShaderSource = Resources::get("src/main/glsl/fragmentRocket.glsl", Category::UNCATEGORIZED);
	unique_ptr<OpenGLShader> vertexShader(new OpenGLShader(Shader::Type::VERTEX, *vertexShaderSource));
	unique_ptr<OpenGLShader> fragmentShader(new OpenGLShader(Shader::Type::FRAGMENT, *fragmentShaderSource));
	unique_ptr<Pipeline> editorPipeline(new OpenGLPipeline(move(vertexShader), move(fragmentShader)));
#endif
	editorRenderer->setDefaultPipeline(move(editorPipeline));

	Editor::run(move(editorRenderer));
}

void testing123(unsigned int radius)
{

	unique_ptr<Entity> test(new Entity);
	setPosition(test->getTransform(), Vector3(0.0f, 2.0f, radius - 5.0f));

	unique_ptr<Mesh> cube = ModelFactory::getInstance()->createCubeMesh(1.0f, shared_ptr<MeshBuffer>(),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	Matrix44 relativeTransform;
	relativeTransform.setIdentity();
	setPosition(relativeTransform, Vector3(1.0f, 1.0f, 1.0f));
	unique_ptr<Model> cubeMinusCube = ModelFunctions::subtract(*cube, *cube, relativeTransform);

	unique_ptr<Mesh> cylinder = ModelFactory::getInstance()->createCylinderMesh(0.3f, 10.0f, 5,
			shared_ptr<MeshBuffer>(), Vector4(1.0f, 0.0f, 0.0f, 1.0f), false);

	unique_ptr<Mesh> hemisphere = ModelFactory::getInstance()->createHemisphereMesh(1.0f, 10, shared_ptr<MeshBuffer>(),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f), true);

	unique_ptr<Mesh> prism = ModelFactory::getInstance()->createPrismMesh(Vector3(0.5f, 0.5f, 0.5f),
			shared_ptr<MeshBuffer>(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	unique_ptr<Mesh> sphere = ModelFactory::getInstance()->createSphereMesh(1.0f, 10, shared_ptr<MeshBuffer>(),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f), true);

	unique_ptr<Mesh> triangle = ModelFactory::getInstance()->createTriangleMesh(Vector3(0.0f, 1.0f, 0.0f),
			Vector3(-1.0f, -2.0f, 0.0f), Vector3(1.0f, -2.0f, 0.0f), shared_ptr<MeshBuffer>(),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));

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
