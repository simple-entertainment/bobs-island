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
	unique_ptr<Graph> world(new QuadTree(1, Square(64.0f), QuadTree::Plane::XZ));

	// Models
	/////////////////////////
	unique_ptr<ModelFactory> modelFactory(new OpenGLModelFactory);
	ModelFactory::setInstance(move(modelFactory));

	// Bob!
	/////////////////////////
	unique_ptr<Entity> bob(new Entity);

	unique_ptr<Model> bobBody = ModelFactory::getInstance().createBoxMesh(Vector3(0.25f, 1.0f, 0.1f),
						Vector4(1.0f, 0.0f, 0.0f, 1.0f));

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
	// Position is relative to Bob.
	MathFunctions::translate(camera->getTransformation(), Vector3(0.0f, 1.11f, -0.21f));

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
	renderingEngine->setGraph(world.get());

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
	MathFunctions::setTranslation(bob->getTransformation(), Vector3(0.0f, 0.0f, radius - 1.0f));
	bob->addUniqueComponent(move(bobBody));
	bob->addUniqueComponent(move(bobControl));
	bob->addUniqueComponent(move(camera));
	bob->addUniqueComponent(move(cameraBounds)); // Yes, this is odd...

	// Add everything!
	/////////////////////////
	Simplicity::addEngine(move(windowingEngine));
	Simplicity::addEngine(move(scriptingEngine));
	Simplicity::addEngine(move(physicsEngine));
	Simplicity::addEngine(move(renderingEngine));

	Simplicity::addWorldRepresentation(move(world));

	Simplicity::addEntity(move(bob));

	// GO!
	/////////////////////////
	Simplicity::play();
}
