/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITYCORE_IP_ADDRESS_H
#define TRINITYCORE_IP_ADDRESS_H

#include "Define.h"
#include <boost/asio/ip/address.hpp>

namespace Trinity::Net
{
    using boost::asio::ip::make_address;
    using boost::asio::ip::make_address_v4;
    using boost::asio::ip::make_address_v6;
    using boost::asio::ip::v4_mapped_t::v4_mapped;
    inline uint32 address_to_uint(boost::asio::ip::address_v4 const& address) { return address.to_uint(); }
}

#endif // TRINITYCORE_IP_ADDRESS_H
