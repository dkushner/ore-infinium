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

#include "packet.h"

#include "src/packet.pb.h"

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "src/debug.h"

#include <iostream>
#include <fstream>

std::string Packet::serialize(google::protobuf::Message* message, uint32_t packetType, PacketCompression compressed)
{

    std::string stringPacketContents;
    google::protobuf::io::StringOutputStream stringStreamPacketContents(&stringPacketContents);

    PacketCompression actualCompression = serializeStreamContents(&stringStreamPacketContents, message, packetType, compressed);

    std::string stringPacketHeader;
    google::protobuf::io::StringOutputStream stringStreamPacketHeader(&stringPacketHeader);

    serializeStreamHeader(&stringStreamPacketHeader, packetType, actualCompression);

    assert(stringPacketContents.size() > 0);

    std::stringstream ss(std::stringstream::out | std::stringstream::binary);
    ss << stringPacketHeader;
    ss << stringPacketContents;

    return ss.str();
}

void Packet::serializeStreamHeader(google::protobuf::io::StringOutputStream* stringOut, uint32_t packetType, PacketCompression compressed)
{
    google::protobuf::io::CodedOutputStream coded_out(stringOut);

    std::string headerString;

    // write packet header, containing type of message we're sending
    PacketBuf::Packet p;
    p.set_type(packetType);
    bool isCompressed = (compressed == PacketCompression::CompressedPacket);
    p.set_compressed(isCompressed);
    p.SerializeToString(&headerString);

    // write the size of the serialized packet header and the contents itself
    coded_out.WriteVarint32(headerString.size());
    coded_out.WriteRaw(headerString.data(), headerString.size());
}

Packet::PacketCompression Packet::serializeStreamContents(google::protobuf::io::StringOutputStream* stringOut, google::protobuf::Message* message, uint32_t packetType, PacketCompression compressed)
{
    google::protobuf::io::CodedOutputStream coded_out(stringOut);

    std::string contentsString;
    // write actual contents
    message->SerializeToString(&contentsString);

    if ((compressed == PacketCompression::CompressedPacket) && (contentsString.size() <= 60)) {
        Debug::log(Debug::StartupArea) << "packet serialization, forgoing packet compression, packet size is too small to likely have a positive yield.";
        compressed = PacketCompression::UncompressedPacket;
    }

    //FIXME: SECURITY: packet exception is not caught if the packet claims it's compressed but boost's zlib fails
    switch (compressed) {
        case PacketCompression::CompressedPacket: {
        std::stringstream uncompressedStream(contentsString);
        std::string compressedString = compress(&uncompressedStream);

        coded_out.WriteVarint32(contentsString.size());
        coded_out.WriteString(compressedString);
        break;
        }

        case PacketCompression::UncompressedPacket: {
        coded_out.WriteVarint32(contentsString.size());
        coded_out.WriteString(contentsString);
        break;
        }
    }

    return compressed;
}

std::string Packet::compress(std::stringstream* in)
{
    Debug::log(Debug::StartupArea) << "compressing packet..precompressed size: " << in->str().size();
   /////////////////////////////////////////////////////////////////////////////////////////////
   boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
   std::stringstream compressedStream;

   boost::iostreams::zlib_params params;
   params.level = boost::iostreams::zlib::best_compression;

   out.push(boost::iostreams::zlib_compressor(params));
   out.push(*in);

   boost::iostreams::copy(out, compressedStream);
   /////////////////////////////////////////////////////////////////////////////////////////////
    Debug::log(Debug::StartupArea) << "compressing packet..compressed size: " << compressedStream.str().size();

   return compressedStream.str();
}

std::string Packet::decompress(std::stringstream* in)
{
   /////////////////////////////////////////////////////////////////////////////////////////////
   boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;

   std::stringstream decompressedStream;

   boost::iostreams::zlib_params params;

   inbuf.push(boost::iostreams::zlib_decompressor(params));
   inbuf.push(*in);

   std::stringstream compressed;
   boost::iostreams::copy(inbuf, decompressedStream);

   /////////////////////////////////////////////////////////////////////////////////////////////

   return decompressedStream.str();
}

uint32_t Packet::deserializePacketType(const std::string& packet)
{
    std::stringstream ss(packet);

    google::protobuf::io::IstreamInputStream raw_in(&ss);
    google::protobuf::io::CodedInputStream coded_in(&raw_in);

    std::string s;

    assert(ss.str().size() > 0);

    //packet header
    uint32_t msgSize;

    bool result = coded_in.ReadVarint32(&msgSize);
    assert(result);

    assert(msgSize > 0);

    if (coded_in.ReadString(&s, msgSize)) {
        PacketBuf::Packet p;
        p.ParseFromString(s);

        return p.type();
    } else {
        assert(0);
    }
}

void Packet::deserialize(const std::string& packetToDeserialize, google::protobuf::Message* message)
{
    std::stringstream ss(packetToDeserialize);

    google::protobuf::io::IstreamInputStream raw_in(&ss);
    google::protobuf::io::CodedInputStream coded_in(&raw_in);

    std::string s;

    assert(ss.str().size() > 0);

    //packet header
    uint32_t msgSize;
    coded_in.ReadVarint32(&msgSize);
    assert(msgSize > 0);

    bool compressed = false;
    if (coded_in.ReadString(&s, msgSize)) {
        PacketBuf::Packet p;
        p.ParseFromString(s);

        compressed = p.compressed();
    } else {
        assert(0);
    }

    // retrieve the size of the uncompressed message..this size is part of the uncompressed data
    coded_in.ReadVarint32(&msgSize);

    if (compressed == false) {
        //packet contents
        if (coded_in.ReadString(&s, msgSize)) {
            message->ParseFromString(s);
        } else {
            assert(0);
        }
    } else {
        // we need to decompress the packet contents before giving it to protobuf to deserialize
        //seek to the end of the header so everything after is the contents
        std::string rawContentsRemaining = packetToDeserialize.substr(coded_in.CurrentPosition(), packetToDeserialize.size() );

        std::stringstream compressedStream(rawContentsRemaining);

        //Debug::log(Debug::StartupArea) << "COMPRESSED STREAM: " << compressedStream.str().size();

        std::string decompressedString = decompress(&compressedStream);

        //Debug::log(Debug::StartupArea) << "DECOMPRESSED STR: " << decompressedString.size();

        std::stringstream decompressedStream(decompressedString);

        //Debug::log(Debug::StartupArea) << " DECOMPRESSED STREAM STR: " << decompressedStream.str().size();

        google::protobuf::io::IstreamInputStream decompressedRaw(&decompressedStream);
        google::protobuf::io::CodedInputStream decompressedCoded(&decompressedRaw);

        if (decompressedCoded.ReadString(&s, msgSize)) {
            message->ParseFromString(s);
        } else {
            assert(0);
        }
    }
}

void Packet::sendPacket(ENetPeer* peer, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType)
{
    assert(peer && message);

    std::string packetContents = Packet::serialize(message, packetType, PacketCompression::UncompressedPacket);

    ENetPacket *packet = enet_packet_create(packetContents.data(), packetContents.size(), enetPacketType);
    assert(packet);

    enet_peer_send(peer, 0, packet);
}

void Packet::sendPacketCompressed(ENetPeer* peer, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType)
{
    assert(peer && message);

    std::string packetContents = Packet::serialize(message, packetType, PacketCompression::CompressedPacket);

    ENetPacket *packet = enet_packet_create(packetContents.data(), packetContents.size(), enetPacketType);
    assert(packet);

    enet_peer_send(peer, 0, packet);
}

void Packet::sendPacketCompressedBroadcast(ENetHost* host, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType)
{
    assert(host && message);

    std::string packetContents = Packet::serialize(message, packetType, PacketCompression::CompressedPacket);

    ENetPacket *packet = enet_packet_create(packetContents.data(), packetContents.size(), enetPacketType);
    assert(packet);

    enet_host_broadcast(host, 0, packet);
}

void Packet::sendPacketBroadcast(ENetHost* host, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType)
{
    assert(host && message);

    std::string packetContents = Packet::serialize(message, packetType, PacketCompression::UncompressedPacket);

    ENetPacket *packet = enet_packet_create(packetContents.data(), packetContents.size(), enetPacketType);
    assert(packet);

    enet_host_broadcast(host, 0, packet);
}
