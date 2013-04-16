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
    std::string stringPacketHeader;
    google::protobuf::io::StringOutputStream stringStreamPacketHeader(&stringPacketHeader);

    serializeStreamHeader(&stringStreamPacketHeader, packetType, compressed);

    std::string stringPacketContents;
    google::protobuf::io::StringOutputStream stringStreamPacketContents(&stringPacketContents);

    serializeStreamContents(&stringStreamPacketContents, message, packetType, compressed);

    assert(stringPacketContents.size() > 0);
    //Debug::log(Debug::StartupArea) << "HEADER coded out stringstream, post-serialized: " << stringPacketHeader.size();
    //Debug::log(Debug::StartupArea) << "CONTENTS coded out stringstream, post-serialized: " << stringPacketContents.size();

    std::stringstream ss(std::stringstream::out | std::stringstream::binary);
    ss << stringPacketHeader;
    ss << stringPacketContents;
    //Debug::log(Debug::StartupArea) << "SS post-serialized: " << ss.str().size();

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

void Packet::serializeStreamContents(google::protobuf::io::StringOutputStream* stringOut, google::protobuf::Message* message, uint32_t packetType, PacketCompression compressed)
{
    google::protobuf::io::CodedOutputStream coded_out(stringOut);

    std::string contentsString;
    // write actual contents
    message->SerializeToString(&contentsString);

    coded_out.WriteVarint32(contentsString.size());
    coded_out.WriteString(contentsString);
}

std::string Packet::compress(std::stringstream* in)
{
    /*
   /////////////////////////////////////////////////////////////////////////////////////////////
   boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
   std::stringstream compressedStream;

   boost::iostreams::zlib_params params;
   params.level = boost::iostreams::zlib::best_compression;

   out.push(boost::iostreams::zlib_compressor(params));
   out.push(*in);

   boost::iostreams::copy(out, compressedStream);
   /////////////////////////////////////////////////////////////////////////////////////////////

   return compressedStream.str();
   */
}

std::string Packet::decompress(std::stringstream* in)
{
    /*
   /////////////////////////////////////////////////////////////////////////////////////////////
   boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;

   std::stringstream decompressedStream;

   boost::iostreams::zlib_params params;
   params.level = boost::iostreams::zlib::best_compression;

   inbuf.push(boost::iostreams::zlib_decompressor(params));
   inbuf.push(*in);

   std::stringstream compressed;
   boost::iostreams::copy(inbuf, decompressedStream);

   /////////////////////////////////////////////////////////////////////////////////////////////

   return decompressedStream.str();
   */
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

  //  Debug::log(Debug::StartupArea) << "DESERIALIZE SS POINTER POSITION BEFORE ALL READS: " <<  coded_in.CurrentPosition();

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

 //   Debug::log(Debug::StartupArea) << "DESERIALIZE SS POINTER POSITION after header READS: " <<  coded_in.CurrentPosition();

    if (compressed == false) {
        //packet contents
        coded_in.ReadVarint32(&msgSize);

        if (coded_in.ReadString(&s, msgSize)) {
            message->ParseFromString(s);
        } else {
            assert(0);
        }
    } else {
        // we need to decompress the packet contents before giving it to protobuf to deserialize
        std::string rawContents;

        //seek to the end of the header so everything after is the contents
        ss.seekp(coded_in.CurrentPosition());
        ss >> rawContents;

        std::stringstream compressedStream(rawContents);
        std::stringstream decompressedStream(decompress(&compressedStream));

        google::protobuf::io::IstreamInputStream raw_in(&decompressedStream);
        google::protobuf::io::CodedInputStream coded_in(&raw_in);

        coded_in.ReadVarint32(&msgSize);

        if (coded_in.ReadString(&s, msgSize)) {
            message->ParseFromString(s);
        } else {
            assert(0);
        }
    }
//    Debug::log(Debug::StartupArea) << "DESERIALIZE SS POINTER POSITION after contents READS: " << coded_in.CurrentPosition();
}

void Packet::sendPacket(ENetPeer* peer, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType)
{
    assert(peer && message);

    std::string packetContents = Packet::serialize(message, packetType, PacketCompression::UncompressedPacket);

    ENetPacket *packet = enet_packet_create(packetContents.data(), packetContents.size(), enetPacketType);
    assert(packet);

//Debug::log() << "SENDING PACKET";
    enet_peer_send(peer, 0, packet);
}

/*
boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
boost::iostreams::zlib_params params;
params.level = boost::iostreams::zlib::best_compression;

out.push(boost::iostreams::zlib_compressor(params));
out.push(ss);

std::stringstream compressed;
boost::iostreams::copy(out, compressed);

//  out.push(file);
//   char data[5] = {'a', 'b', 'c', 'd', 'e'};
//    boost::iostreams::copy(boost::iostreams::basic_array_source<char>(data, sizeof(data)), out);

std::ofstream file("TESTWORLDDATA", std::ios::binary);
file << compressed.str();
file.close();
*/
void Packet::sendCompressedPacketBroadcast(ENetHost* host, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType)
{
    assert(host && message);

    std::string packetContents = Packet::serialize(message, packetType, PacketCompression::UncompressedPacket);

    ENetPacket *packet = enet_packet_create(packetContents.data(), packetContents.size(), enetPacketType);
    assert(packet);

    enet_host_broadcast(host, 0, packet);
}

void Packet::sendPacketBroadcast(ENetHost* host, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType)
{
    assert(host && message);
//Debug::log() << "SENDING PACKET BROAD";

    std::string packetContents = Packet::serialize(message, packetType, PacketCompression::UncompressedPacket);

    ENetPacket *packet = enet_packet_create(packetContents.data(), packetContents.size(), enetPacketType);
    assert(packet);

    enet_host_broadcast(host, 0, packet);
}
