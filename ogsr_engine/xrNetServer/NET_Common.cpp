#include "stdafx.h"
#include "NET_Common.h"

#pragma pack(push)
#pragma pack(1)
struct MultipacketHeader
{
    u8 tag;
    u16 unpacked_size;
};
#pragma pack(pop)

static const unsigned MaxMultipacketSize = 32768;

MultipacketSender::MultipacketSender() {}

void MultipacketSender::SendPacket(const void* packet_data, u32 packet_sz, u32 flags, u32 timeout)
{
    _buf_cs.Enter();

    Buffer* buf = &_buf;

    u32 old_flags = (buf->last_flags) & (~DPNSEND_IMMEDIATELLY);
    u32 new_flags = flags & (~DPNSEND_IMMEDIATELLY);

    if ((buf->buffer.B.count + packet_sz + sizeof(u16) >= NET_PacketSizeLimit) || (old_flags != new_flags) || (flags & DPNSEND_IMMEDIATELLY))
    {
        _FlushSendBuffer(timeout, buf);
    }

    buf->buffer.w_u16((u16)packet_sz);
    buf->buffer.w(packet_data, packet_sz);

    if (flags & DPNSEND_IMMEDIATELLY)
        _FlushSendBuffer(timeout, buf);

    buf->last_flags = flags;
    _buf_cs.Leave();
}

void MultipacketSender::FlushSendBuffer(u32 timeout)
{
    _buf_cs.Enter();

    _FlushSendBuffer(timeout, &_buf);
    _FlushSendBuffer(timeout, &_gbuf);

    _buf_cs.Leave();
}

void MultipacketSender::_FlushSendBuffer(u32 timeout, Buffer* buf)
{
    // expected to be called between '_buf_cs' enter/leave

    if (buf->buffer.B.count)
    {
        // compress data

        u8 packet_data[MaxMultipacketSize];
        MultipacketHeader* header = (MultipacketHeader*)packet_data;

        std::memcpy(packet_data + sizeof(MultipacketHeader), buf->buffer.B.data, buf->buffer.B.count);

        header->tag = NET_TAG_MERGED;
        header->unpacked_size = (u16)buf->buffer.B.count;

        _SendTo_LL(packet_data, buf->buffer.B.count + sizeof(MultipacketHeader), buf->last_flags, timeout);
        buf->buffer.B.count = 0;
    } // if buffer not empty
}

//------------------------------------------------------------------------------
#include "NET_Messages.h"
void MultipacketReciever::RecievePacket(const void* packet_data, u32 packet_sz, u32 param)
{
#pragma todo("Delete this shit!!")
}