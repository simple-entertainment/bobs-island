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
		unique_ptr<SceneGraph> sceneGraph(new QuadTree(1, Square(128.0f), QuadTree::Plane::XZ));
		Simplicity::getEngine<RenderingEngine>()->setGraph(sceneGraph.get());
		Simplicity::getScene()->addState(move(sceneGraph));

		// Bob
		Entity* bob = Simplicity::getScene()->getEntity("Bob");
		// Setup the camera
		Camera* camera = bob->getComponent<Camera>();
		camera->setPerspective(60.0f, 4.0f / 3.0f);
		unique_ptr<Shape> cameraBounds(new Square(32.0f));
		camera->setBounds(move(cameraBounds));
		// Switch to Bob's camera
		Simplicity::getEngine<RenderingEngine>()->setCamera(bob);

		// Sun
		Entity* sun = Simplicity::getScene()->getEntity("Sun");
		// Invert normals so that light from the center of the sun illuminates it.
		Model* sunModel = sun->getComponent<Model>();
		MeshData& sunModelData = sunModel->getMesh()->getData(false);
		for (unsigned int index = 0; index < sunModelData.vertexCount; index++)
		{
			sunModelData.vertexData[index].normal.negate();
		}
		sunModel->getMesh()->releaseData();
		Simplicity::getEngine<RenderingEngine>()->addLight(*sun);

		// Terrain
		unique_ptr<Entity> terrain(new Entity(111));
		Resource* terrainFile = Resources::get("island.terrain", Resource::Type::ASSET, true);

		unique_ptr<vector<TerrainStreamer::LevelOfDetail>> levelsOfDetail(new vector<TerrainStreamer::LevelOfDetail>(3));

		levelsOfDetail->at(0).layerCount = 2;
		levelsOfDetail->at(0).source =
				unique_ptr<TerrainSource>(new ResourceTerrainSource(Vector2ui(1024, 1024), *terrainFile));
		levelsOfDetail->at(0).sampleFrequency = 1;

		unsigned int stride = 4;
		unsigned int resourceOffset = 1025 * 1025 * stride * sizeof(float);
		levelsOfDetail->at(1).layerCount = 2;
		levelsOfDetail->at(1).source =
				unique_ptr<TerrainSource>(new ResourceTerrainSource(Vector2ui(256, 256), *terrainFile, resourceOffset));
		levelsOfDetail->at(1).sampleFrequency = 4;

		resourceOffset += 257 * 257 * stride * sizeof(float);
		levelsOfDetail->at(2).layerCount = 2;
		levelsOfDetail->at(2).source =
			unique_ptr<TerrainSource>(new ResourceTerrainSource(Vector2ui(64, 64), *terrainFile, resourceOffset));
		levelsOfDetail->at(2).sampleFrequency = 16;

		unique_ptr<TerrainStreamer> terrainStreamer(
				new TerrainStreamer(move(levelsOfDetail), Vector2ui(1024, 1024), 64));

		terrainStreamer->setTarget(*bob);
		terrain->addComponent(move(terrainStreamer));

		Simplicity::getScene()->addEntity(move(terrain));
	}
}
