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
#include <memory>

#include <simplicity/API.h>
//#include <simplicity/bullet/API.h>
#include <simplicity/freeglut/API.h>
#include <simplicity/opengl/API.h>

#include <the-island/API.h>

#include "BobControl.h"

using namespace bobsisland;
using namespace simplicity;
//using namespace simplicity::bullet;
using namespace simplicity::freeglut;
using namespace simplicity::opengl;
using namespace std;
using namespace theisland;

int main(int argc, char** argv)
{
	// Windowing
	/////////////////////////
	unique_ptr<Engine> windowingEngine(new FreeGLUTEngine("Bob's Island"));
	Simplicity::addEngine(move(windowingEngine));

	// World Representations
	/////////////////////////
	unique_ptr<Graph> world(new SimpleTree);
	Graph* rawWorld = world.get();
	Simplicity::addWorldRepresentation(move(world));

	// Models
	/////////////////////////
	unique_ptr<ModelFactory> modelFactory(new OpenGLModelFactory);
	ModelFactory::setInstance(move(modelFactory));

	// Bob!
	/////////////////////////
	unique_ptr<Entity> bob(new Entity);

	// Scripting
	/////////////////////////
	unique_ptr<Engine> scriptingEngine(new ScriptingEngine);
	Simplicity::addEngine(move(scriptingEngine));

	// Physics
	/////////////////////////
	/*unique_ptr<PhysicsFactory> physicsFactory(new BulletPhysicsFactory);
	PhysicsFactory::setInstance(move(physicsFactory));
	unique_ptr<Engine> physicsEngine(new BulletEngine(Vector3(0.0f, 0.0f, 0.0f)));
	Simplicity::addEngine(move(physicsEngine));*/

	// Rendering
	/////////////////////////
	unique_ptr<OpenGLRenderingEngine> renderingEngine(new OpenGLRenderingEngine);
	unique_ptr<Renderer> renderer(new SimpleOpenGLRenderer);

	// Shaders
	ifstream vertexShaderFile("src/main/glsl/my.vs");
	ifstream fragmentShaderFile("src/main/glsl/my.fs");
	unique_ptr<OpenGLVertexShader> vertexShader(new OpenGLVertexShader(vertexShaderFile));
	unique_ptr<OpenGLFragmentShader> fragmentShader(new OpenGLFragmentShader(fragmentShaderFile));
	vertexShaderFile.close();
	fragmentShaderFile.close();
	unique_ptr<Shader> shader(new OpenGLShader(move(vertexShader), move(fragmentShader)));
	renderer->setShader(move(shader));

	// Camera
	unique_ptr<Camera> camera(new OpenGLCamera);
	camera->setPerspective(60.0f, 4.0f / 3.0f);

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
	renderingEngine->setClearingColour(Vector4(0.0f, 0.5f, 0.75f, 1.0f));
	renderingEngine->setGraph(rawWorld);
	Simplicity::addEngine(move(renderingEngine));

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

	// Bob Components!
	/////////////////////////
	unique_ptr<Model> bobBody = ModelFactory::getInstance().createBoxMesh(Vector3(0.25f, 1.0f, 0.1f),
						Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	MathFunctions::translate(camera->getTransformation(), Vector3(0.0f, 1.11f, -0.21f));
	unique_ptr<BobControl> bobControl(new BobControl);
	bobControl->setEntity(bob.get());
	bob->addUniqueComponent(move(bobBody));
	bob->addUniqueComponent(move(bobControl));
	bob->addUniqueComponent(move(camera));
	MathFunctions::setTranslation(bob->getTransformation(), Vector3(0.0f, 0.0f, radius - 1.0f));

	// A Box!
	/////////////////////////
	/*unique_ptr<Entity> box(new Entity);
	MathFunctions::translate(box->getTransformation(), Vector4(0.0f, 50.0f, 0.0f, 1.0f));

	unique_ptr<Model> boxModel =
			ModelFactory::getInstance().createCubeMesh(5.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	Body::Material material;
	material.density = 0.0f;
	material.friction = 0.5f;
	material.restitution = 0.5f;
	unique_ptr<Body> boxBody = PhysicsFactory::getInstance().createBody(material, boxModel.get(), Matrix44(), true);
	boxBody->setEntity(box.get());
	//boxBody->setMass(0.0f);

	box->addUniqueComponent(move(boxModel));
	box->addUniqueComponent(move(boxBody));*/

	Simplicity::addEntity(move(bob));
	//Simplicity::addEntity(move(box));

	// GO!
	/////////////////////////
	Simplicity::play();
}
