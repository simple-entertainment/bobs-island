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

#include <simplicity/API.h>
#include <simplicity/live555/API.h>
#include <simplicity/raknet/API.h>
#include <simplicity/rocket/API.h>
#include <simplicity/terrain/API.h>

#ifdef DIRECT3D
#include <simplicity/direct3d/API.h>
#include <simplicity/winapi/API.h>
#else
#include <simplicity/opengl/API.h>
#include <simplicity/glfw/API.h>
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
using namespace simplicity;
using namespace simplicity::live555;
using namespace simplicity::raknet;
using namespace simplicity::rocket;
using namespace simplicity::terrain;
using namespace std;

#ifdef DIRECT3D
using namespace simplicity::direct3d;
using namespace simplicity::winapi;
#else
using namespace simplicity::glfw;
using namespace simplicity::opengl;
#endif

#ifdef PHYSX
using namespace simplicity::simphysx;
#else
using namespace simplicity::bullet;
#endif

extern "C" void setupEngine()
{
	// Windowing
	/////////////////////////
#ifdef DIRECT3D
	unique_ptr<WinAPIEngine> windowingEngine(new WinAPIEngine("Bob's Island", instance, commandShow));
#else
	unique_ptr<Engine> windowingEngine(new GLFWEngine("Bob's Island"));
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
	Resource* vertexShaderResource = Resources::get("vertexDefault.cso", Category::UNCATEGORIZED, true);
	Resource* fragmentShaderResource = Resources::get("fragmentDefault.cso", Category::UNCATEGORIZED, true);
#else
	Resource* vertexShaderResource = Resources::get("src/main/glsl/vertexDefault.glsl");
	Resource* fragmentShaderResource = Resources::get("src/main/glsl/fragmentDefault.glsl");
#endif
	unique_ptr<Shader> vertexShader =
			RenderingFactory::getInstance()->createShader(Shader::Type::VERTEX, *vertexShaderResource);
	unique_ptr<Shader> fragmentShader =
			RenderingFactory::getInstance()->createShader(Shader::Type::FRAGMENT, *fragmentShaderResource);
	shared_ptr<Pipeline> pipeline =
			RenderingFactory::getInstance()->createPipeline(move(vertexShader), nullptr, move(fragmentShader));
	renderingEngine->setDefaultPipeline(pipeline);

#ifndef BOB_AS_CLIENT
	// Streaming
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
	Simplicity::addEngine(move(renderingEngine));
}

extern "C" void setupScene()
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

	// Assemble the Sun!
	/////////////////////////
	unique_ptr<Script> sunMover(new SunMover);
	theSun->addUniqueComponent(move(sunMover));

	// Add everything!
	/////////////////////////
	Simplicity::getScene()->addEntity(move(startingCamera));
	Simplicity::getScene()->addEntity(move(theSun));
}
