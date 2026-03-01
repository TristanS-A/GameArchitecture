#pragma once
#include <unordered_map>
#include <string>
#include <Trackable.h>

class GraphicsBuffer;

typedef std::string BufferKey;

class GraphicsBufferManager : public Trackable
{
public:
	GraphicsBufferManager();
	~GraphicsBufferManager();
	void init() {};
	void cleanup();

	GraphicsBuffer* createAndAddGraphicsBuffer(const BufferKey& key, const std::string& bufferFilename);
	void addGraphicsBuffer(const BufferKey& key, GraphicsBuffer* bufferToAdd);
	void deleteGraphicsBuffer(const BufferKey &key);
	void deleteGraphicsBuffer(GraphicsBuffer* bufferToDelete);
	GraphicsBuffer* getGraphicsBuffer(const BufferKey &key) const;
	BufferKey getGraphicsBufferKey(GraphicsBuffer* bufferToFind);
private:
	std::unordered_map<BufferKey, GraphicsBuffer*> mBufferMap;
};