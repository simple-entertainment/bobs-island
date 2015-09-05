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
#include <simplicity/terrain/API.h>

#include "bob/BobFactory.h"
#include "SunMover.h"

using namespace bobsisland;
using namespace simplicity;
using namespace simplicity::terrain;
using namespace std;

extern "C"
{
	void simplicity_setupEngine()
	{
		Resource* vertexShaderResource = Resources::get("glsl/vertexDefault.glsl");
		Resource* fragmentShaderResource = Resources::get("glsl/fragmentDefault.glsl");
		unique_ptr<Shader> vertexShader =
				RenderingFactory::getInstance()->createShader(Shader::Type::VERTEX, *vertexShaderResource);
		unique_ptr<Shader> fragmentShader =
				RenderingFactory::getInstance()->createShader(Shader::Type::FRAGMENT, *fragmentShaderResource);
		shared_ptr<Pipeline> pipeline =
				RenderingFactory::getInstance()->createPipeline(move(vertexShader), nullptr, move(fragmentShader));
		Simplicity::getEngine<RenderingEngine>()->setDefaultPipeline(pipeline);
	}

	void simplicity_setupScene()
	{
		RenderingEngine* renderingEngine = Simplicity::getEngine<RenderingEngine>();

		// Scene Graph
		/////////////////////////
		unique_ptr<Graph> sceneGraph(new QuadTree(1, Square(128.0f), QuadTree::Plane::XZ));
		renderingEngine->setGraph(sceneGraph.get());
		Simplicity::getScene()->addGraph(move(sceneGraph));

		// Bob
		/////////////////////////
		unique_ptr<Entity> bob = BobFactory::createBob(0);
		//rotate(bob->getTransform(), MathConstants::PI * 0.5f, Vector3(0.0f, 1.0f, 0.0f));
		setPosition(bob->getTransform(), Vector3(9.0f, 0.0f, 295.0f));

		// The Terrain
		/////////////////////////
		unique_ptr<Entity> terrain(new Entity(111));
		Resource* terrainFile = Resources::get("island.terrain", Resource::Type::ASSET, true);
		//unique_ptr<TerrainStreamer> terrainStreamer(
		//		new TerrainStreamer(*terrainFile, Vector2ui(1024, 1024), Vector2ui(100, 200), unitLength));
		unique_ptr<LODTerrainStreamer> terrainStreamer(
				new LODTerrainStreamer(*terrainFile, Vector2ui(1024, 1024),
									   {Vector2ui(64, 64), Vector2ui(256, 256), Vector2ui(1024, 1024)},
									   {1, 4, 16}));
		terrainStreamer->setTrackedEntity(*bob);
		terrain->addUniqueComponent(move(terrainStreamer));

		// The Sky
		/////////////////////////
		unique_ptr<Entity> sky(new Entity);
		rotate(sky->getTransform(), MathConstants::PI * -0.5f, Vector3(1.0f, 0.0f, 0.0f));

		unique_ptr<Mesh> skyMesh = ModelFactory::getInstance()->createHemisphereMesh(1100.0f, 20,
																					 shared_ptr<MeshBuffer>(),
																					 Vector4(0.0f, 0.5f, 0.75f, 1.0f),
																					 true);

		sky->addUniqueComponent(move(skyMesh));
		Simplicity::getScene()->addEntity(move(sky));

		// The Sun
		/////////////////////////
		unique_ptr<Entity> theSun(new Entity);

		unique_ptr<Light> sunLight(new Light("theSun"));
		sunLight->setAmbient(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
		sunLight->setDiffuse(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
		sunLight->setRange(1000.0f);
		sunLight->setSpecular(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
		sunLight->setStrength(32.0f);

		// Initially up in the sky directly above!
		Vector3 position(sin(0.0f), cos(0.0f), 0.0f);
		Vector3 direction = position;
		direction.negate();
		position *= 1000.0f;

		setPosition(theSun->getTransform(), position);
		sunLight->setDirection(direction);

		theSun->addUniqueComponent(move(sunLight));

		unique_ptr<Mesh> sunModel = ModelFactory::getInstance()->createSphereMesh(50.0f, 10, shared_ptr<MeshBuffer>(),
																				  Vector4(1.0f, 1.0f, 0.6f, 1.0f));
		MeshData& sunModelData = sunModel->getData(false);
		for (unsigned int index = 0; index < sunModelData.vertexCount; index++)
		{
			sunModelData.vertexData[index].normal.negate();
		}
		sunModel->releaseData();
		theSun->addUniqueComponent(move(sunModel));

		unique_ptr<Script> sunMover(new SunMover);
		theSun->addUniqueComponent(move(sunMover));

		renderingEngine->addLight(*theSun);

		// The Ocean
		/////////////////////////
		unique_ptr<Entity> ocean(new Entity);
		rotate(ocean->getTransform(), MathConstants::PI * -0.5f, Vector3(1.0f, 0.0f, 0.0f));

		unique_ptr<Mesh> oceanMesh =
				ModelFactory::getInstance()->createCylinderMesh(1200.0f, 500.0f, 20, shared_ptr<MeshBuffer>(),
																Vector4(0.0f, 0.4f, 0.6f, 1.0f), true);
		ocean->addUniqueComponent(move(oceanMesh));

		// Add everything!
		/////////////////////////
		Simplicity::getScene()->addEntity(move(bob));
		Simplicity::getScene()->addEntity(move(ocean));
		Simplicity::getScene()->addEntity(move(terrain));
		Simplicity::getScene()->addEntity(move(theSun));
	}
}
