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
#include <simplicity/freeglut/API.h>
#include <simplicity/opengl/API.h>
#include <simplicity/rocket/API.h>

#include <the-island/API.h>

#include "BobControl.h"
#include "FlippingEngine.h"
#include "SunEngine.h"

using namespace bobsisland;
using namespace simplicity;
using namespace simplicity::bullet;
using namespace simplicity::freeglut;
using namespace simplicity::opengl;
using namespace simplicity::rocket;
using namespace std;
using namespace theisland;

void onKeyboardButton(const void* message);
void setupEngine();
void setupScene();

int main()
{
	/*OpenCL::init();
	ifstream clFile("src/main/cl/matrix.cl");
	OpenCL::addProgram(clFile, "matrix");
	clFile.close();*/

	setRandomSeed(1234567);

	setupEngine();
	setupScene();

	//Messages::registerRecipient(Events::KEYBOARD_BUTTON, onKeyboardButton);

	Simplicity::play();
}

void onKeyboardButton(const void* message)
{
	const KeyboardButtonEvent* event = static_cast<const KeyboardButtonEvent*>(message);

	if (event->button == Keyboard::Button::ESCAPE && event->buttonState == Button::State::UP)
	{
		Simplicity::stop();
	}
}

void setupEngine()
{
	// Windowing
	/////////////////////////
	unique_ptr<Engine> windowingEngine(new FreeGLUTEngine("Bob's Island"));

	// Resources
	/////////////////////////
	unique_ptr<DataStore> consoleDataStore(new ConsoleDataStore);
	Resources::setDataStore(move(consoleDataStore), Category::CONSOLE);
	unique_ptr<DataStore> fileSystemDataStore(new FileSystemDataStore("."));
	Resources::setDataStore(move(fileSystemDataStore), Category::ALL_CATEGORIES);

	// Logging
	/////////////////////////
	Logs::setResource(Resources::get("out", Category::CONSOLE), Category::ALL_CATEGORIES);

	// Scene Graphs
	/////////////////////////
	unique_ptr<Graph> sceneGraph0(new SimpleGraph);
	unique_ptr<Graph> sceneGraph1(new QuadTree(1, Square(128.0f), QuadTree::Plane::XZ));

	// Models
	/////////////////////////
	unique_ptr<ModelFactory> modelFactory(new OpenGLModelFactory);
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
	RenderingFactory::setInstance(move(renderingFactory));

	unique_ptr<OpenGLRenderingEngine> renderingEngine(new OpenGLRenderingEngine);
	unique_ptr<Renderer> renderer(new OpenGLRenderer);

	// Shaders
	Resource* vertexShaderSource = Resources::get("src/main/glsl/my.vs", Category::UNCATEGORIZED);
	Resource* geometryShaderSource = Resources::get("src/main/glsl/my.gs", Category::UNCATEGORIZED);
	Resource* fragmentShaderSource = Resources::get("src/main/glsl/my.fs", Category::UNCATEGORIZED);
	unique_ptr<OpenGLVertexShader> vertexShader(new OpenGLVertexShader(*vertexShaderSource));
	unique_ptr<OpenGLGeometryShader> geometryShader(new OpenGLGeometryShader(*geometryShaderSource));
	unique_ptr<OpenGLFragmentShader> fragmentShader(new OpenGLFragmentShader(*fragmentShaderSource));
	unique_ptr<Shader> shader(new OpenGLShader(move(vertexShader), move(geometryShader), move(fragmentShader)));
	Shader* shaderRaw = shader.get();
	renderer->setShader(move(shader));

	// Flipping triangles!
	/////////////////////////
	unique_ptr<Engine> flippingEngine(new FlippingEngine(*shaderRaw));

	// UI
	/////////////////////////
	unique_ptr<Renderer> uiRenderer(new OpenGLRenderer);
	uiRenderer->setClearColorBuffer(false);

	Resource* uiVertexShaderSource = Resources::get("src/main/glsl/rocket.vs", Category::UNCATEGORIZED);
	Resource* uiFragmentShaderSource = Resources::get("src/main/glsl/rocket.fs", Category::UNCATEGORIZED);
	unique_ptr<OpenGLVertexShader> uiVertexShader(new OpenGLVertexShader(*uiVertexShaderSource));
	unique_ptr<OpenGLFragmentShader> uiFragmentShader(new OpenGLFragmentShader(*uiFragmentShaderSource));
	unique_ptr<Shader> uiShader(new OpenGLShader(move(uiVertexShader), move(uiFragmentShader)));
	uiRenderer->setShader(move(uiShader));

	unique_ptr<Engine> uiEngine(new RocketEngine(move(uiRenderer), Category::UNCATEGORIZED));

	// Assemble the rendering engine.
	/////////////////////////
	renderingEngine->addRenderer(move(renderer));
	renderingEngine->setGraph(sceneGraph1.get());

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
	Simplicity::addEngine(move(scriptingEngine));
	Simplicity::addEngine(move(physicsEngine));
	Simplicity::addEngine(move(renderingEngine));
	Simplicity::addEngine(move(flippingEngine));
	Simplicity::addEngine(move(uiEngine));

	unique_ptr<Scene> theOnlyScene(new Scene);
	Simplicity::addScene("theOnly", move(theOnlyScene));

	//Simplicity::getScene()->addGraph(move(sceneGraph0));
	Simplicity::getScene()->addGraph(move(sceneGraph1));

	Simplicity::getScene()->addEntity(move(debug));
}

void setupScene()
{
	// Bob!
	/////////////////////////
	unique_ptr<Entity> bob(new Entity);
	Entity* rawBob = bob.get();

	unique_ptr<Model> bobBody = ModelFactory::getInstance().createBoxMesh(Vector3(0.25f, 1.0f, 0.1f),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	unique_ptr<Model> bobGunArm = ModelFactory::getInstance().createCylinderMesh(0.05f, 0.75f, 10,
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	setPosition(bobGunArm->getTransform(), Vector3(0.25f, 0.95f, 0.0f));

	Graph* quadTree = Simplicity::getScene()->getGraph<QuadTree>();
	unique_ptr<BobControl> bobControl(new BobControl(*quadTree));

	unique_ptr<Model> cameraBounds(new Square(32.0f));
	setPosition(cameraBounds->getTransform(), Vector3(0.0f, 0.0f, -32.0f));
	cameraBounds->setCategory(Category::BOUNDS);

	// Camera
	/////////////////////////
	unique_ptr<Camera> camera(new OpenGLCamera);
	camera->setFarClippingDistance(2000.0f);
	camera->setPerspective(60.0f, 4.0f / 3.0f);
	// Position is relative to Bob.
	translate(camera->getTransform(), Vector3(0.0f, 1.11f, -0.21f));

	// The Sun
	/////////////////////////
	unique_ptr<Entity> theSun(new Entity);

	unique_ptr<Light> sunLight(new OpenGLLight("theSun"));
	sunLight->setAmbient(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	sunLight->setDiffuse(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	sunLight->setRange(1000.0f);
	sunLight->setSpecular(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	sunLight->setStrength(32.0f);

	unique_ptr<Mesh> sunModel = ModelFactory::getInstance().createSphereMesh(50.0f, 10, Vector4(1.0f, 1.0f, 0.6f));
	for (unsigned int index = 0; index < sunModel->getVertices().size(); index++)
	{
		sunModel->getVertices()[index].normal.negate();
	}

	theSun->addUniqueComponent(move(sunLight));
	theSun->addUniqueComponent(move(sunModel));

	// The flashlight
	/////////////////////////
	unique_ptr<Entity> flash(new Entity);

	unique_ptr<Light> flashLight(new OpenGLLight("flash"));
	flashLight->setAmbient(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	flashLight->setAttenuation(Vector3(0.5f, 0.05f, 0.0f));
	flashLight->setDiffuse(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	flashLight->setDirection(Vector3(0.0f, 0.0f, -1.0f));
	flashLight->setRange(50.0f);
	flashLight->setSpecular(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	flashLight->setStrength(32.0f);

	// The Sun engine
	/////////////////////////
	unique_ptr<Engine> sunEngine(new SunEngine(*theSun, *flashLight));
	flash->addUniqueComponent(move(flashLight));

	// Update the rendering engine.
	/////////////////////////
	RenderingEngine* renderingEngine = Simplicity::getEngine<RenderingEngine>();
	renderingEngine->addLight(*theSun);
	//renderingEngine->addLight(*flash);
	renderingEngine->setCamera(bob.get());

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
	bob->addUniqueComponent(move(bobBody));
	bob->addUniqueComponent(move(bobGunArm));
	bob->addUniqueComponent(move(bobControl));
	bob->addUniqueComponent(move(camera));
	bob->addUniqueComponent(move(cameraBounds)); // Yes, this is odd...

	// Testing 123
	/////////////////////////
	unique_ptr<FlyingCameraEngine> flyingCameraEngine(new FlyingCameraEngine(*bob.get()));
	//Simplicity::addEngine(move(flyingCameraEngine));

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

	// Add everything!
	/////////////////////////
	Simplicity::addEngine(move(sunEngine));

	Simplicity::getScene()->addEntity(move(bob));
	Simplicity::getScene()->addEntity(move(theSun));
	Simplicity::getScene()->addEntity(move(flash), *rawBob);
}
