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
#include <fstream>

#include <simplicity/API.h>
#include <simplicity/bullet/API.h>
#include <simplicity/freeglut/API.h>
#include <simplicity/opengl/API.h>

#include <the-island/API.h>

#include "BobControl.h"
#include "FlippingEngine.h"

using namespace bobsisland;
using namespace simplicity;
using namespace simplicity::bullet;
using namespace simplicity::freeglut;
using namespace simplicity::opengl;
using namespace std;
using namespace theisland;

int main(int argc, char** argv)
{
	// Windowing
	/////////////////////////
	unique_ptr<Engine> windowingEngine(new FreeGLUTEngine("Bob's Island"));

	// World Representations
	/////////////////////////
	unique_ptr<Graph> world(new QuadTree(1, Square(128.0f), QuadTree::Plane::XZ));

	// Models
	/////////////////////////
	unique_ptr<ModelFactory> modelFactory(new OpenGLModelFactory);
	ModelFactory::setInstance(move(modelFactory));

	// Bob!
	/////////////////////////
	unique_ptr<Entity> bob(new Entity);

	unique_ptr<Model> bobBody = ModelFactory::getInstance().createBoxMesh(Vector3(0.25f, 1.0f, 0.1f),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	unique_ptr<Model> bobGunArm = ModelFactory::getInstance().createCylinderMesh(0.05f, 0.75f, 10,
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	setPosition(bobGunArm->getTransform(), Vector3(0.25f, 0.95f, 0.0f));

	unique_ptr<BobControl> bobControl(new BobControl(*world.get()));
	bobControl->setEntity(bob.get());

	unique_ptr<Model> cameraBounds(new Square(32.0f));
	cameraBounds->setCategory(Categories::BOUNDS);

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
	unique_ptr<OpenGLRenderingEngine> renderingEngine(new OpenGLRenderingEngine);
	unique_ptr<Renderer> renderer(new OpenGLRenderer);

	// Shaders
	ifstream vertexShaderFile("src/main/glsl/my.vs");
	ifstream geometryShaderFile("src/main/glsl/my.gs");
	ifstream fragmentShaderFile("src/main/glsl/my.fs");
	unique_ptr<OpenGLVertexShader> vertexShader(new OpenGLVertexShader(vertexShaderFile));
	unique_ptr<OpenGLGeometryShader> geometryShader(new OpenGLGeometryShader(geometryShaderFile));
	unique_ptr<OpenGLFragmentShader> fragmentShader(new OpenGLFragmentShader(fragmentShaderFile));
	vertexShaderFile.close();
	fragmentShaderFile.close();
	unique_ptr<Shader> shader(new OpenGLShader(move(vertexShader), move(geometryShader), move(fragmentShader)));
	Shader* shaderRaw = shader.get();
	renderer->setShader(move(shader));

	// Camera
	unique_ptr<Camera> camera(new OpenGLCamera);
	camera->setPerspective(60.0f, 4.0f / 3.0f);
	// Position is relative to Bob.
	translate(camera->getTransform(), Vector3(0.0f, 1.11f, -0.21f));

	// Light
	unique_ptr<Light> light(new OpenGLLight("theOnly"));
	light->setAmbientComponent(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	light->setDiffuseComponent(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	light->setSpecularComponent(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
	light->setDirection(Vector3(0.0f, -1.0f, 0.0f));
	light->setStrength(32.0f);

	renderingEngine->addLight(move(light));
	renderingEngine->addRenderer(move(renderer));
	renderingEngine->setCamera(move(bob.get()));
	renderingEngine->setClearingColor(Vector4(0.0f, 0.5f, 0.75f, 1.0f));
	renderingEngine->setGraph(world.get());

	// Flipping triangles!
	/////////////////////////
	unique_ptr<Engine> flippingEngine(new FlippingEngine(*shaderRaw));

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
		profile.push_back(peakHeight * (pow(radius - index, 3) / pow(radius, 3)));
	}
	IslandFactory::createIsland(radius, profile);

	// Assemble Bob!
	/////////////////////////
	setPosition(bob->getTransform(), Vector3(0.0f, 0.0f, radius - 1.0f));
	bob->addUniqueComponent(move(bobBody));
	bob->addUniqueComponent(move(bobGunArm));
	bob->addUniqueComponent(move(bobControl));
	bob->addUniqueComponent(move(camera));
	bob->addUniqueComponent(move(cameraBounds)); // Yes, this is odd...

	// Testing 123
	/////////////////////////
	/*unique_ptr<FlyingCameraEngine> flyingCameraEngine(new FlyingCameraEngine(*bob.get()));
	Simplicity::addEngine(move(flyingCameraEngine));

	unique_ptr<Entity> test(new Entity);
	setPosition(test->getTransform(), Vector3(0.0f, 2.0f, radius - 3.0f));

	unique_ptr<Mesh> cube = ModelFactory::getInstance().createCubeMesh(1.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	Matrix44 relativeTransform;
	relativeTransform.setIdentity();
	setPosition(relativeTransform, Vector3(1.0f, 1.0f, 1.0f));
	unique_ptr<Model> cubeMinusCube = ModelFunctions::subtract(*cube, *cube, relativeTransform);

	unique_ptr<Mesh> cylinder = ModelFactory::getInstance().createCylinderMesh(0.25f, 10.0f, 5,
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	unique_ptr<Mesh> sphere = ModelFactory::getInstance().createSphereMesh(1.0f, 10, Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			false);

	unique_ptr<Mesh> triangle = ModelFactory::getInstance().createTriangleMesh(Vector3(0.0f, 1.0f, -5.0f),
			Vector3(-1.0f, -2.0f, 0.0f), Vector3(1.0f, -2.0f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	relativeTransform.setIdentity();
	unique_ptr<Model> triangleMinusCylinder = ModelFunctions::subtract(*triangle, *cylinder, relativeTransform);

	unique_ptr<Model> bounds(new Square(1.0f));
	bounds->setCategory(Categories::BOUNDS);

	//test->addUniqueComponent(move(cube));
	//test->addUniqueComponent(move(cubeMinusCube));
	//test->addUniqueComponent(move(cylinder));
	test->addUniqueComponent(move(sphere));
	//test->addUniqueComponent(move(triangle));
	//test->addUniqueComponent(move(triangleMinusCylinder));
	test->addUniqueComponent(move(bounds));
	Simplicity::addEntity(move(test));*/

	// Add everything!
	/////////////////////////
	Simplicity::addEngine(move(windowingEngine));
	Simplicity::addEngine(move(scriptingEngine));
	Simplicity::addEngine(move(physicsEngine));
	Simplicity::addEngine(move(renderingEngine));
	Simplicity::addEngine(move(flippingEngine));

	Simplicity::addWorldRepresentation(move(world));

	Simplicity::addEntity(move(bob));

	// GO!
	/////////////////////////
	Simplicity::play();
}
