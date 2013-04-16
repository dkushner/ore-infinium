/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#ifndef PACKET_H
#define PACKET_H

#include "src/packet.pb.h"

#include <enet/enet.h>

#include <iostream>

namespace google {
namespace protobuf {
namespace io {
class CodedOutputStream;
class StringOutputStream;
}
}
}

class Packet
{
public:

    /**
     * Packet command/contents sent *from* client to a server
     */
    enum FromClientPacketContents {
        InvalidFromClientPacket = 0,
        /// to send stuff like the name of the player, version of game client, etc.
        InitialConnectionDataFromClientPacket,
        /// only sending the chat message itself, not player name. server handles that magic
        ChatMessageFromClientPacket,
        /// contains version, player name and other things that the server needs to OK for us to not get booted right away.
        PlayerInitialDataFromClientPacket,
        //FIXME: not sure wtf to do with that one..i don't think we want a huge generic thing, it also is unused presently
        PlayerStateFromClientPacket,
        /// movement attempted left/right/up/down
        PlayerMoveFromClientPacket,
        /// mouse position as well as button
        PlayerMouseStateFromClient,

        QuickBarInventorySelectSlotRequestFromClient
    };

    /**
     * Packet command/contents sent *from server* to (a) client(s).
     */
    enum FromServerPacketContents {
        InvalidFromServerPacket = 0,
        ChatMessageFromServerPacket,
        InitialPlayerDataFromServerPacket,

        /// sent when the server has sent all currently connected players, to a newly-connected client, meaning all future "initial player data" will be considered "new" players
        InitialPlayerDataFinishedFromServerPacket,

        QuickBarInventoryItemFromServerPacket,

        /// when the item count of the quickbar inventory has changed
        /// e.g. someone placed or dropped or consumed an item, and now the mentioned client needs
        /// to know the new count.
        QuickBarInventoryItemCountChangedFromServerPacket,

        PlayerDisconnectedFromServerPacket,
        PlayerMoveFromServerPacket,
        ChunkFromServerPacket,

        /// when a new item has been spawned in the world (dropped or placed)
        ItemSpawnedFromServerPacket
    };

    enum ConnectionEventType {
        None = 0,
        //client version does not match server version, forceful disconnect.
        DisconnectedVersionMismatch,
        DisconnectedInvalidPlayerName
    };

    enum class PacketCompression {
        CompressedPacket,
        UncompressedPacket
    };

    /**
     * Serializes the @p message into a string, along with the packet header and @p packetType which specifies
     * what kind of data is inside it.
     *
     */
    static std::string serialize(google::protobuf::Message* message, uint32_t packetType, PacketCompression compressed);
    static void serializeStreamHeader(google::protobuf::io::StringOutputStream* stringOut, uint32_t packetType, PacketCompression compressed);
    static void serializeStreamContents(google::protobuf::io::StringOutputStream* stringOut, google::protobuf::Message* message, uint32_t packetType, PacketCompression compressed);

    /**
     * Deserializes the provided stringstream, @p in and puts the data into @p message
     * Seeks the stream pointer back to zero when done.
     */
    static void deserialize(const std::string& packetToDeserialize, google::protobuf::Message* message);

    /**
     * Returns the packettype, representing an enum of either @sa FromClientPacketContents
     * or @sa FromServerPacketContents
     * Seeks the stream pointer back to zero when done.
     */
    static uint32_t deserializePacketType(const std::string& packet);

    //FIXME unused
    static std::string compress(std::stringstream* in);
    static std::string decompress(std::stringstream* in);

    /**
     * Send a packet containing the message @p message to the peer @p peer with the packet type (e.g. InvalidFromClientPacket, InvalidFromServerPacket, etc.)
     * @p enetPacketType e.g. whether ENET_PACKET_FLAG_RELIABLE or something else..
     */
    static void sendPacket(ENetPeer* peer, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType);

    static void sendPacketBroadcast(ENetHost* host, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType);
    static void sendCompressedPacketBroadcast(ENetHost* host, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType);

private:
    Packet();
    ~Packet();
};

#endif
