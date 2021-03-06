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
#include <simplicity/opengl/API.h>
#include <simplicity/terrain/API.h>

#include "WaterAnimator.h"

using namespace simplicity;
using namespace simplicity::opengl;
using namespace simplicity::terrain;
using namespace std;

extern "C"
{
	void simplicity_setupEngine()
	{
		// Pipeline
		Simplicity::getEngine<RenderingEngine>()->setDefaultPipeline(RenderingFactory::createPipeline(
				RenderingFactory::createShader(Shader::Type::VERTEX, *Resources::get("glsl/vertexDefault.glsl")),
				nullptr,
				RenderingFactory::createShader(Shader::Type::FRAGMENT, *Resources::get("glsl/fragmentDefault.glsl"))
		));

		Simplicity::getEngine<OpenGLRenderingEngine>()->setFrameBuffer(RenderingFactory::createFrameBuffer(
				{
						RenderingFactory::createTexture(nullptr, 800, 600, PixelFormat::RGB_HDR),
						RenderingFactory::createTexture(nullptr, 800, 600, PixelFormat::RGB_HDR)
				},
				true
		));

		unique_ptr<PostProcessor> postProcessor(new BloomPostProcessor);
		Simplicity::getEngine<OpenGLRenderingEngine>()->setPostProcessor(move(postProcessor));
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
		Model* sunModel = sun->getComponent<Model>();
		sunModel->getMesh()->getBuffer()->setPipeline(RenderingFactory::createPipeline(
				RenderingFactory::createShader(Shader::Type::VERTEX, *Resources::get("glsl/vertexDefault.glsl")),
				nullptr,
				RenderingFactory::createShader(Shader::Type::FRAGMENT, *Resources::get("glsl/fragmentSun.glsl"))
		));
		// Invert normals so that light from the center of the sun illuminates it.
		MeshData& sunModelData = sunModel->getMesh()->getData(false);
		for (unsigned int index = 0; index < sunModelData.vertexCount; index++)
		{
			sunModelData.vertexData[index].normal.negate();
		}
		sunModel->getMesh()->releaseData();
		Simplicity::getEngine<RenderingEngine>()->addLight(*sun);

		// Sky
		unique_ptr<Entity> sky(new Entity(Category::UNCATEGORIZED, "Sky"));
		ModelFactory::Recipe skyRecipe;
		skyRecipe.shape = ModelFactory::Recipe::Shape::BOX;
		skyRecipe.inwardFaces = true;
		skyRecipe.outwardFaces = false;
		skyRecipe.dimensions[0] = 2000.0f;
		skyRecipe.dimensions[1] = 2000.0f;
		skyRecipe.dimensions[2] = 2000.0f;
		std::shared_ptr<Mesh> skyMesh(ModelFactory::cookMesh(skyRecipe));
		MeshData& meshData = skyMesh->getData(false);

		meshData.vertexData[0].texCoord.X() = 0.2501f;//1.0f / 4.0f;
		meshData.vertexData[0].texCoord.Y() = 0.9999f;//1.0f;
		meshData.vertexData[1].texCoord.X() = 0.2501f;//1.0f / 4.0f;
		meshData.vertexData[1].texCoord.Y() = 0.6667f;//2.0f / 3.0f;
		meshData.vertexData[2].texCoord.X() = 0.4999f;//2.0f / 4.0f;
		meshData.vertexData[2].texCoord.Y() = 0.6667f;//2.0f / 3.0f;
		meshData.vertexData[3].texCoord.X() = 0.4999f;//2.0f / 4.0f;
		meshData.vertexData[3].texCoord.Y() = 0.9999f;//1.0f;

		meshData.vertexData[4].texCoord.X() = 0.2501f;//1.0f / 4.0f;
		meshData.vertexData[4].texCoord.Y() = 0.3332f;//1.0f / 3.0f;
		meshData.vertexData[5].texCoord.X() = 0.2501f;//1.0f / 4.0f;
		meshData.vertexData[5].texCoord.Y() = 0.0001f;//0.0f;
		meshData.vertexData[6].texCoord.X() = 0.4999f;//2.0f / 4.0f;
		meshData.vertexData[6].texCoord.Y() = 0.0001f;//0.0f;
		meshData.vertexData[7].texCoord.X() = 0.4999f;//2.0f / 4.0f;
		meshData.vertexData[7].texCoord.Y() = 0.3332f;//1.0f / 3.0f;

		meshData.vertexData[8].texCoord.X() = 0.7501f;//3.0f / 4.0f;
		meshData.vertexData[8].texCoord.Y() = 0.6665f;//2.0f / 3.0f;
		meshData.vertexData[9].texCoord.X() = 0.7501f;//3.0f / 4.0f;
		meshData.vertexData[9].texCoord.Y() = 0.3334f;//1.0f / 3.0f;
		meshData.vertexData[10].texCoord.X() = 0.9999f;//1.0f;
		meshData.vertexData[10].texCoord.Y() = 0.3334f;//1.0f / 3.0f;
		meshData.vertexData[11].texCoord.X() = 0.9999f;//1.0f;
		meshData.vertexData[11].texCoord.Y() = 0.6665f;//2.0f / 3.0f;

		meshData.vertexData[12].texCoord.X() = 0.5001f;//2.0f / 4.0f;
		meshData.vertexData[12].texCoord.Y() = 0.6665f;//2.0f / 3.0f;
		meshData.vertexData[13].texCoord.X() = 0.5001f;//2.0f / 4.0f;
		meshData.vertexData[13].texCoord.Y() = 0.3334f;//1.0f / 3.0f;
		meshData.vertexData[14].texCoord.X() = 0.7499f;//3.0f / 4.0f;
		meshData.vertexData[14].texCoord.Y() = 0.3334f;//1.0f / 3.0f;
		meshData.vertexData[15].texCoord.X() = 0.7499f;//3.0f / 4.0f;
		meshData.vertexData[15].texCoord.Y() = 0.6665f;//2.0f / 3.0f;

		meshData.vertexData[16].texCoord.X() = 0.2501f;//1.0f / 4.0f;
		meshData.vertexData[16].texCoord.Y() = 0.6665f;//2.0f / 3.0f;
		meshData.vertexData[17].texCoord.X() = 0.2501f;//1.0f / 4.0f;
		meshData.vertexData[17].texCoord.Y() = 0.3334f;//1.0f / 3.0f;
		meshData.vertexData[18].texCoord.X() = 0.4999f;//2.0f / 4.0f;
		meshData.vertexData[18].texCoord.Y() = 0.3334f;//1.0f / 3.0f;
		meshData.vertexData[19].texCoord.X() = 0.4999f;//2.0f / 4.0f;
		meshData.vertexData[19].texCoord.Y() = 0.6665f;//2.0f / 3.0f;

		meshData.vertexData[20].texCoord.X() = 0.0001f;//0.0f;
		meshData.vertexData[20].texCoord.Y() = 0.6665f;//2.0f / 3.0f;
		meshData.vertexData[21].texCoord.X() = 0.0001f;//0.0f;
		meshData.vertexData[21].texCoord.Y() = 0.3334f;//1.0f / 3.0f;
		meshData.vertexData[22].texCoord.X() = 0.2499f;//1.0f / 4.0f;
		meshData.vertexData[22].texCoord.Y() = 0.3334f;//1.0f / 3.0f;
		meshData.vertexData[23].texCoord.X() = 0.2499f;//1.0f / 4.0f;
		meshData.vertexData[23].texCoord.Y() = 0.6665f;//2.0f / 3.0f;

		skyMesh->releaseData();
		skyMesh->getBuffer()->setPipeline(RenderingFactory::createPipeline("simple"));
		std::unique_ptr<Model> skyModel(new Model);
		skyModel->setMesh(skyMesh);
		skyModel->setTexture(RenderingFactory::createTexture(*Resources::get("images/skybox.jpg"),
															 PixelFormat::BGR));
		sky->addComponent(std::move(skyModel));
		Simplicity::getScene()->addEntity(std::move(sky));

		// Terrain
		unique_ptr<Entity> terrain(new Entity(111));

		Vector2ui mapSize(1024, 1024);
		Resource* terrainFile = Resources::get("island.terrain", Resource::Type::ASSET, true);
		vector<LevelOfDetail> levelsOfDetail(3);
		levelsOfDetail[0].layerCount = 2;
		levelsOfDetail[0].sampleFrequency = 1;
		levelsOfDetail[1].layerCount = 2;
		levelsOfDetail[1].sampleFrequency = 4;
		levelsOfDetail[2].layerCount = 2;
		levelsOfDetail[2].sampleFrequency = 16;

		unique_ptr<TerrainSource> terrainSource(new ResourceTerrainSource(mapSize, *terrainFile, levelsOfDetail));
		unique_ptr<TerrainStreamer> terrainStreamer(new TerrainStreamer(move(terrainSource), mapSize, 64, levelsOfDetail));

		terrainStreamer->setTarget(*bob);
		terrain->addComponent(move(terrainStreamer));

		Simplicity::getScene()->addEntity(move(terrain));

		// Ocean
		Entity* ocean = Simplicity::getScene()->getEntity("Ocean");
		Model* oceanModel = ocean->getComponent<Model>();

		oceanModel->getMesh()->getBuffer()->setPipeline(RenderingFactory::createPipeline(
				RenderingFactory::createShader(Shader::Type::VERTEX, *Resources::get("glsl/vertexWater.glsl")),
				nullptr,
				RenderingFactory::createShader(Shader::Type::FRAGMENT, *Resources::get("glsl/fragmentWater.glsl"))
		));

		unique_ptr<Component> oceanAnimator(new bobsisland::WaterAnimator);
		ocean->addComponent(move(oceanAnimator));
	}
}
