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

#include <simplicity/API.h>
#include <simplicity/freeglut/API.h>
#include <simplicity/live555/API.h>
#include <simplicity/opengl/API.h>
#include <simplicity/raknet/API.h>

using namespace simplicity;
using namespace simplicity::freeglut;
using namespace simplicity::live555;
using namespace simplicity::raknet;
using namespace simplicity::opengl;
using namespace std;

int main()
{
	Logs::info("bobs-island-client", "###########################");
	Logs::info("bobs-island-client", "### BOB's Island Client ###");
	Logs::info("bobs-island-client", "###########################");

	Logs::info("bobs-island-client", "Setting up engine...");

	// Windowing
	/////////////////////////
	unique_ptr<WindowEngine> windowingEngine(new FreeGLUTEngine("Bob's Island (Thin)"));
	Simplicity::addEngine(move(windowingEngine));

	// Messaging
	/////////////////////////
	unique_ptr<MessagingEngine> localMessagingEngine(new SimpleMessagingEngine);
	Messages::addEngine(localMessagingEngine.get());
	Simplicity::addEngine(move(localMessagingEngine));

	unique_ptr<MessagingEngine> remoteMessagingEngine(new RakNetMessagingEngine("127.0.0.1", 55501));
	Messages::addEngine(remoteMessagingEngine.get());
	Simplicity::addEngine(move(remoteMessagingEngine));

	Messages::registerRecipient(Subject::KEYBOARD_BUTTON, RecipientCategory::SERVER);
	Messages::registerRecipient(Subject::MOUSE_BUTTON, RecipientCategory::SERVER);
	Messages::registerRecipient(Subject::MOUSE_MOVE, RecipientCategory::SERVER);

	unique_ptr<Codec> keyboardButtonCodec(new SimpleCodec<KeyboardButtonEvent>);
	Messages::setCodec(Subject::KEYBOARD_BUTTON, move(keyboardButtonCodec));
	unique_ptr<Codec> mouseButtonCodec(new SimpleCodec<MouseButtonEvent>);
	Messages::setCodec(Subject::MOUSE_BUTTON, move(mouseButtonCodec));
	unique_ptr<Codec> mouseMoveCodec(new SimpleCodec<MouseMoveEvent>);
	Messages::setCodec(Subject::MOUSE_MOVE, move(mouseMoveCodec));

	// Models
	/////////////////////////
	unique_ptr<ModelFactory> modelFactory(new OpenGLModelFactory);
	ModelFactory::setInstance(move(modelFactory));

	// Rendering
	/////////////////////////
	unique_ptr<RenderingFactory> renderingFactory(new OpenGLRenderingFactory);
	RenderingFactory::setInstance(move(renderingFactory));
	unique_ptr<RenderingEngine> renderingEngine(new OpenGLRenderingEngine);
	Simplicity::addEngine(move(renderingEngine));

	// Sreaming
	/////////////////////////
	//unique_ptr<Live555ClientEngine> streamingEngine(new Live555ClientEngine("rtsp://127.0.0.1:8554/h264ESVideoTest"));
	unique_ptr<Live555ClientEngine> streamingEngine(new Live555ClientEngine("rtsp://127.0.0.1:8554/simplicity::live555"));

	// Video Surface (with dummy data)
	unique_ptr<Model> mesh = ModelFactory::getInstance()->createSquareMesh(1.0f);
	//unique_ptr<Texture> texture = RenderingFactory::getInstance()->createTexture(new char[320 * 240 * 3], 320, 240,
	//		PixelFormat::RGB);
	unique_ptr<Texture> texture = RenderingFactory::getInstance()->createTexture(new char[1024 * 768 * 3], 1024, 768,
			PixelFormat::RGB);
	mesh->setTexture(texture.get());

	// Sink
	//unique_ptr<Sink> sink(new ResourceSink(Resources::create("test.264", Category::UNCATEGORIZED, true)));
	unique_ptr<Sink> sink(new TextureSink(*texture.get()));
	streamingEngine->setSink(move(sink));

	// Video Surface Entity
	unique_ptr<Entity> videoSurfaceEntity(new Entity());
	videoSurfaceEntity->addUniqueComponent(move(mesh));
	Simplicity::getScene()->addEntity(move(videoSurfaceEntity));

	// Finalise
	Simplicity::addEngine(move(streamingEngine));

	// GO!!!
	/////////////////////////
	Logs::info("bobs-island-client", "GO!!!");

	//Simplicity::setMaxFrameRate(30);
	Simplicity::play();
}

