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
		// Scene Graph
		/////////////////////////
		unique_ptr<Graph> sceneGraph(new QuadTree(1, Square(128.0f), QuadTree::Plane::XZ));
		Simplicity::getEngine<RenderingEngine>()->setGraph(sceneGraph.get());
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

		// The Sun
		/////////////////////////
		Entity* sun = Simplicity::getScene()->getEntity("Sun");
		Mesh* sunModel = sun->getComponent<Mesh>();
		MeshData& sunModelData = sunModel->getData(false);
		for (unsigned int index = 0; index < sunModelData.vertexCount; index++)
		{
			sunModelData.vertexData[index].normal.negate();
		}
		sunModel->releaseData();
		Simplicity::getEngine<RenderingEngine>()->addLight(*sun);

		// Add everything!
		/////////////////////////
		Simplicity::getScene()->addEntity(move(bob));
		Simplicity::getScene()->addEntity(move(terrain));
	}
}
