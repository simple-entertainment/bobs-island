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
//#include <simplicity/physx/API.h>
#include <simplicity/raknet/API.h>

#include <the-island/API.h>

#include "ServerEngine.h"

using namespace bobsisland::server;
using namespace simplicity;
using namespace simplicity::bullet;
//using namespace simplicity::physx;
using namespace simplicity::raknet;
using namespace std;
using namespace theisland;

void setupEngine();
void setupScene();

int main()
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
	Logs::log(Category::INFO_LOG, "### BOB's Island Server ###");
	Logs::log(Category::INFO_LOG, "###########################");

	setRandomSeed(1234567);

	Logs::log(Category::INFO_LOG, "Setting up engine...");
	setupEngine();
	Logs::log(Category::INFO_LOG, "Setting up scene...");
	setupScene();

	Logs::log(Category::INFO_LOG, "GO!!!");
	Simplicity::play();
}

void setupEngine()
{
	// Messaging
	/////////////////////////
	unique_ptr<MessagingEngine> localMessagingEngine(new SimpleMessagingEngine);
	Messages::addEngine(localMessagingEngine.get());
	Simplicity::addEngine(move(localMessagingEngine));

	unique_ptr<MessagingEngine> remoteMessagingEngine(new RakNetMessagingEngine(55501, 10));
	Messages::addEngine(remoteMessagingEngine.get());
	Simplicity::addEngine(move(remoteMessagingEngine));

	// Models
	/////////////////////////
	unique_ptr<ModelFactory> modelFactory(new SimpleModelFactory);
	ModelFactory::setInstance(move(modelFactory));

	// Scripting
	/////////////////////////
	unique_ptr<Engine> scriptingEngine(new ScriptingEngine);
	Simplicity::addEngine(move(scriptingEngine));

	// Physics
	/////////////////////////
	unique_ptr<PhysicsFactory> physicsFactory(new BulletPhysicsFactory);
	PhysicsFactory::setInstance(move(physicsFactory));

	unique_ptr<Engine> physicsEngine(new BulletEngine(Vector3(0.0f, -10.0f, 0.0f)));
	Simplicity::addEngine(move(physicsEngine));

	// Server Logic
	/////////////////////////
	unique_ptr<Engine> serverEngine(new ServerEngine);
	Simplicity::addEngine(move(serverEngine));

	// The Scene
	/////////////////////////
	unique_ptr<Scene> theOnlyScene(new Scene);
	Simplicity::addScene("theOnly", move(theOnlyScene));

	// Scene Graph
	/////////////////////////
	unique_ptr<Graph> sceneGraph(new QuadTree(1, Square(128.0f), QuadTree::Plane::XZ));
	Simplicity::getScene()->addGraph(move(sceneGraph));
}

void setupScene()
{
	// The Island!
	/////////////////////////
	unsigned int radius = 128;
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
}
