/*
   Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,
   2011, 2012 Her Majesty the Queen in Right of Canada (Communications
   Research Center Canada)

   Copyright (C) 2020
   Matthias P. Braendli, matthias.braendli@mpb.li

    http://www.opendigitalradio.org
   */
/*
   This file is part of the ODR-mmbTools.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <iostream>
#include <iterator>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include "ThreadsafeQueue.h"
#include "edioutput/TagItems.h"
#include "edioutput/TagPacket.h"
#include "edioutput/Transport.h"
#include "edi/common.hpp"


struct tagpacket_t {
    uint16_t dlfc;
    std::vector<uint8_t> tagpacket;
    EdiDecoder::frame_timestamp_t timestamp;
    std::chrono::steady_clock::time_point received_at;
    EdiDecoder::seq_info_t seq;
};

class EDISender {
    public:
        EDISender() = default;
        EDISender(const EDISender& other) = delete;
        EDISender& operator=(const EDISender& other) = delete;
        ~EDISender();
        void start(const edi::configuration_t& conf,
                int delay_ms, bool drop_late_packets);
        void push_tagpacket(tagpacket_t&& tagpacket);
        void print_configuration(void);

    private:
        void send_tagpacket(tagpacket_t& frame);
        void process(void);

        int tist_delay_ms;
        bool drop_late;
        std::atomic<bool> running;
        std::thread process_thread;
        edi::configuration_t edi_conf;
        ThreadsafeQueue<tagpacket_t> tagpackets;

        std::shared_ptr<edi::Sender> edi_sender;

        struct buffering_stat_t {
            // Time between when we received the packets and when we transmit packets, in microseconds
            double buffering_time_us = 0.0;
            bool late = false;
        };
        std::vector<buffering_stat_t> buffering_stats;

};
