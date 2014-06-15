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

#include <bobs-island/bob/BobController.h>
#include <bobs-island/bob/BobFactory.h>

using namespace bobsisland;
using namespace simplicity;
using namespace simplicity::bullet;
//using namespace simplicity::physx;
using namespace simplicity::raknet;
using namespace std;
using namespace theisland;

void setupEngine();
void setupScene();

void jumpEcho(const void* message);
void lookEcho(const void* message);
void moveEcho(const void* message);
void shootEcho(const void* message);

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

	unique_ptr<MessagingEngine> remoteMessagingEngine(new RakNetMessagingEngine(55501, 1));
	Messages::addEngine(remoteMessagingEngine.get());
	Simplicity::addEngine(move(remoteMessagingEngine));

	Logs::log(Category::INFO_LOG, "Setting up echo!");
	Messages::registerRecipient(Action::JUMP, jumpEcho);
	Messages::registerRecipient(Action::LOOK, lookEcho);
	Messages::registerRecipient(Action::MOVE, moveEcho);
	Messages::registerRecipient(Action::SHOOT, shootEcho);

	Messages::registerRecipient(Action::JUMP2, RecipientCategory::CLIENT);
	Messages::registerRecipient(Action::LOOK2, RecipientCategory::CLIENT);
	Messages::registerRecipient(Action::MOVE2, RecipientCategory::CLIENT);
	Messages::registerRecipient(Action::SHOOT2, RecipientCategory::CLIENT);

	unique_ptr<Codec> jumpCodec(new EmptyCodec);
	Messages::setCodec(Action::JUMP, move(jumpCodec));
	unique_ptr<Codec> jump2Codec(new EmptyCodec);
	Messages::setCodec(Action::JUMP2, move(jump2Codec));
	unique_ptr<Codec> lookCodec(new SimpleCodec<Vector<int, 2>>);
	Messages::setCodec(Action::LOOK, move(lookCodec));
	unique_ptr<Codec> look2Codec(new SimpleCodec<Vector<int, 2>>);
	Messages::setCodec(Action::LOOK2, move(look2Codec));
	unique_ptr<Codec> moveCodec(new SimpleCodec<BobController::Direction>);
	Messages::setCodec(Action::MOVE, move(moveCodec));
	unique_ptr<Codec> move2Codec(new SimpleCodec<BobController::Direction>);
	Messages::setCodec(Action::MOVE2, move(move2Codec));
	unique_ptr<Codec> shootCodec(new EmptyCodec);
	Messages::setCodec(Action::SHOOT, move(shootCodec));
	unique_ptr<Codec> shoot2Codec(new EmptyCodec);
	Messages::setCodec(Action::SHOOT2, move(shoot2Codec));

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

	// Bob!
	/////////////////////////
	unique_ptr<Entity> bob = BobFactory::createBob();
	rotate(bob->getTransform(), MathConstants::PI * 0.5f, Vector3(0.0f, 1.0f, 0.0f));
	setPosition(bob->getTransform(), Vector3(0.0f, 0.0f, radius - 1.0f));
	Simplicity::getScene()->addEntity(move(bob));
}

void jumpEcho(const void* message)
{
	Messages::send(Action::JUMP2, message);
}

void lookEcho(const void* message)
{
	Messages::send(Action::LOOK2, message);
}

void moveEcho(const void* message)
{
	Messages::send(Action::MOVE2, message);
}

void shootEcho(const void* message)
{
	Messages::send(Action::SHOOT2, message);
}
