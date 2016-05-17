#pragma once

class MapMaster;
class ObjectFactory;
class Chat;
class TextPainter;
class SyncRandom;
class Names;

class IGame
{
public:
    virtual MapMaster& GetMap() = 0;
    virtual const MapMaster& GetMap() const = 0;
    virtual ObjectFactory& GetFactory() = 0;
    virtual Chat& GetChat() = 0;
    virtual TextPainter& GetTexts() = 0;
    virtual SyncRandom& GetRandom() = 0;
    virtual Names& GetNames() = 0;
};
