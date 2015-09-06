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

using namespace simplicity;
using namespace simplicity::terrain;
using namespace std;

extern "C"
{
	void simplicity_setupEngine()
	{
		// Pipeline
		unique_ptr<Shader> vertexShader =
				RenderingFactory::createShader(Shader::Type::VERTEX, *Resources::get("glsl/vertexDefault.glsl"));
		unique_ptr<Shader> fragmentShader =
				RenderingFactory::createShader(Shader::Type::FRAGMENT, *Resources::get("glsl/fragmentDefault.glsl"));
		shared_ptr<Pipeline> pipeline =
				RenderingFactory::createPipeline(move(vertexShader), nullptr, move(fragmentShader));
		Simplicity::getEngine<RenderingEngine>()->setDefaultPipeline(pipeline);
	}

	void simplicity_setupScene()
	{
		// Scene Graph
		unique_ptr<Graph> sceneGraph(new QuadTree(1, Square(128.0f), QuadTree::Plane::XZ));
		Simplicity::getEngine<RenderingEngine>()->setGraph(sceneGraph.get());
		Simplicity::getScene()->addGraph(move(sceneGraph));

		// Bob
		Entity* bob = Simplicity::getScene()->getEntity("Bob");
		// Setup the camera
		Camera* camera = bob->getComponent<Camera>();
		camera->setPerspective(60.0f, 4.0f / 3.0f);
		// Yes, this is odd... Bob's bounds are used as the camera bounds... for now...
		unique_ptr<Model> cameraBounds(new Square(32.0f));
		setPosition(cameraBounds->getTransform(), Vector3(0.0f, 0.0f, -32.0f));
		cameraBounds->setCategory(Category::BOUNDS);
		bob->addUniqueComponent(move(cameraBounds));
		// Switch to Bob's camera
		Simplicity::getEngine<RenderingEngine>()->setCamera(bob);

		// Sun
		Entity* sun = Simplicity::getScene()->getEntity("Sun");
		// Invert normals so that light from the center of the sun illuminates it.
		Mesh* sunModel = sun->getComponent<Mesh>();
		MeshData& sunModelData = sunModel->getData(false);
		for (unsigned int index = 0; index < sunModelData.vertexCount; index++)
		{
			sunModelData.vertexData[index].normal.negate();
		}
		sunModel->releaseData();
		Simplicity::getEngine<RenderingEngine>()->addLight(*sun);

		// Terrain
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
		Simplicity::getScene()->addEntity(move(terrain));
	}
}
