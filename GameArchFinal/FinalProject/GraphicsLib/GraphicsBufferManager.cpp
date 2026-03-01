#include "GraphicsBufferManager.h"
#include "GraphicsBufferManager.h"
#include "GraphicsBuffer.h"

GraphicsBufferManager::GraphicsBufferManager()
{
}

GraphicsBufferManager::~GraphicsBufferManager()
{
	cleanup();
}

void GraphicsBufferManager::cleanup()
{
	for (auto iterator : mBufferMap) 
	{
		GraphicsBuffer* buffer = iterator.second;
		delete buffer;
		buffer = nullptr;
	}

	mBufferMap.clear();
}

GraphicsBuffer* GraphicsBufferManager::createAndAddGraphicsBuffer(const BufferKey& key, const std::string& bufferFilename)
{
	GraphicsBuffer* newBuffer = nullptr;
	auto iterator = mBufferMap.find(key);

	if (iterator == mBufferMap.end())
	{
		newBuffer = new GraphicsBuffer(bufferFilename);
		mBufferMap[key] = newBuffer;
	}

	return newBuffer;
}

void GraphicsBufferManager::addGraphicsBuffer(const BufferKey& key, GraphicsBuffer* bufferToAdd)
{
	auto iterator = mBufferMap.find(key);

	if (iterator == mBufferMap.end())
	{
		mBufferMap[key] = bufferToAdd;
	}
}

void GraphicsBufferManager::deleteGraphicsBuffer(const BufferKey &key)
{
	auto iterator = mBufferMap.find(key);

	if (iterator != mBufferMap.end())
	{
		delete iterator->second;
		mBufferMap.erase(iterator);
	}
}

void GraphicsBufferManager::deleteGraphicsBuffer(GraphicsBuffer* bufferToDelete)
{
	for (auto iterator : mBufferMap)
	{
		if (iterator.second == bufferToDelete)
		{
			delete iterator.second;
			mBufferMap.erase(iterator.first);
			return;
		}
	}
}

GraphicsBuffer* GraphicsBufferManager::getGraphicsBuffer(const BufferKey& key) const
{
	auto iterator = mBufferMap.find(key);

	if (iterator != mBufferMap.end())
	{
		return iterator->second;
	}

	return nullptr;
}

BufferKey GraphicsBufferManager::getGraphicsBufferKey(GraphicsBuffer* bufferToFind)
{
	for (auto iterator : mBufferMap)
	{
		if (iterator.second == bufferToFind)
		{
			return iterator.first;
		}
	}

	return "";
}
