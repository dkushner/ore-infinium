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

class Packet
{
    Packet();
    ~Packet();

    /**
     * Packet command/contents sent *from* client to a server
     */
    enum FromClientPacketContents {
       InvalidFromClientPacket = 0,
       /// to send stuff like the name of the player, version of game client, etc.
      ClientInitialConnectionDataFromClientPacket,
      ChatMessageFromClientPacket,
      PlayerInitialDataFromClientPacket,
      PlayerStateFromClientPacket,
    };

    /**
     * Packet command/contents sent *from server* to (a) client(s).
     */
    enum FromServerPacketContents {
        InvalidFromServerPacket = 0,
        ChatMessageFromServerPacket
    };

    unsigned short packetContents = 0;
    void* data;
};

#endif
