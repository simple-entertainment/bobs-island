/*
 * Copyright © 2015 Simple Entertainment Limited
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
#include <libnoise/noise.h>

#include <simplicity/API.h>
#include <simplicity/terrain/API.h>

using namespace noise::module;
using namespace simplicity;
using namespace simplicity::terrain;
using namespace std;

// Island generation
float getHeight(int x, int y);
Perlin perlinNoise;
int mapSize = 1024;
int halfMapSize = mapSize / 2;
float peakHeight = 100.0f;
float peakAmplitude = 50.0f;
int perlinFrequency = 128;

float getHeight(int x, int y)
{
	float height = static_cast<float>(perlinNoise.GetValue(static_cast<double>(x) / perlinFrequency, 0.5,
														   static_cast<double>(y) / perlinFrequency)) + 1.0f;

	height /= 2.0f;
	float distanceFromOrigin = static_cast<float>(Vector2i(x - halfMapSize, y - halfMapSize).getMagnitude());
	float fractionFromOrigin = (1.0f - distanceFromOrigin / static_cast<float>(halfMapSize));
	height *= fractionFromOrigin * peakAmplitude;
	height += fractionFromOrigin * peakHeight - peakAmplitude;
	return height;
}

void createAK47()
{
	unique_ptr<Entity> ak47(new Entity);
	setPosition(ak47->getTransform(), Vector3(0.0f, 2.0f, 62.0f));
	ak47->addUniqueComponent(ModelFactory::getInstance()->loadObj(*Resources::get("models/ak47.obj")));
	Simplicity::getScene()->addEntity(move(ak47));
}

void createIslandTerrainFile()
{
	Resource* terrainFile = Resources::create("island.terrain", true);
	TerrainFactory::createFlatTerrain(*terrainFile, Vector2ui(mapSize, mapSize), getHeight, { 1, 4, 16 });

	// Testing 123
	/////////////////////////
	//unique_ptr<CameraController> flyingCameraEngine(new CameraController(*bob.get()));
	//Simplicity::addEngine(move(flyingCameraEngine));
	//testing123(radius);
}

void runModelMathTests(const Vector3& position)
{

	unique_ptr<Entity> test(new Entity);
	setPosition(test->getTransform(), position);

	unique_ptr<Mesh> cube = ModelFactory::getInstance()->createCubeMesh(1.0f, shared_ptr<MeshBuffer>(),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	Matrix44 relativeTransform;
	relativeTransform.setIdentity();
	setPosition(relativeTransform, Vector3(1.0f, 1.0f, 1.0f));
	unique_ptr<Model> cubeMinusCube = ModelFunctions::subtract(*cube, *cube, relativeTransform);

	unique_ptr<Mesh> cylinder = ModelFactory::getInstance()->createCylinderMesh(0.3f, 10.0f, 5,
			shared_ptr<MeshBuffer>(), Vector4(1.0f, 0.0f, 0.0f, 1.0f), false);

	unique_ptr<Mesh> hemisphere = ModelFactory::getInstance()->createHemisphereMesh(1.0f, 10, shared_ptr<MeshBuffer>(),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f), true);

	unique_ptr<Mesh> prism = ModelFactory::getInstance()->createPrismMesh(Vector3(0.5f, 0.5f, 0.5f),
			shared_ptr<MeshBuffer>(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	unique_ptr<Mesh> sphere = ModelFactory::getInstance()->createSphereMesh(1.0f, 10, shared_ptr<MeshBuffer>(),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f), true);

	unique_ptr<Mesh> triangle = ModelFactory::getInstance()->createTriangleMesh(Vector3(0.0f, 1.0f, 0.0f),
			Vector3(-1.0f, -2.0f, 0.0f), Vector3(1.0f, -2.0f, 0.0f), shared_ptr<MeshBuffer>(),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	setPosition(relativeTransform, Vector3(0.0f, 0.0f, 5.0f));
	unique_ptr<Model> triangleMinusCylinder = ModelFunctions::subtract(*triangle, *cylinder, relativeTransform);
	unique_ptr<Model> prismMinusCylinder = ModelFunctions::subtract(*prism, *cylinder, relativeTransform);

	unique_ptr<Model> bounds(new Square(1.0f));
	bounds->setCategory(Category::BOUNDS);

	//test->addUniqueComponent(move(cube));
	test->addUniqueComponent(move(cubeMinusCube));
	//test->addUniqueComponent(move(cylinder));
	//test->addUniqueComponent(move(hemisphere));
	//test->addUniqueComponent(move(prism));
	//test->addUniqueComponent(move(sphere));
	//test->addUniqueComponent(move(triangle));
	//test->addUniqueComponent(move(triangleMinusCylinder));
	//test->addUniqueComponent(move(prismMinusCylinder));
	test->addUniqueComponent(move(bounds));
	Simplicity::getScene()->addEntity(move(test));
}
