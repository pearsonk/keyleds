/* Keyleds -- Gaming keyboard tool
 * Copyright (C) 2017 Julien Hartmann, juli1.hartmann@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "keyledsd/RenderTarget.h"

#include <cstddef>
#include <type_traits>
#include <utility>

static_assert(std::is_pod<keyleds::RGBAColor>::value, "RGBAColor must be a POD type");
static_assert(sizeof(keyleds::RGBAColor) == 4, "RGBAColor must be tightly packed");

static constexpr std::size_t alignBytes = 32;  // 16 is minimum for SSE2, 32 for AVX2
static constexpr std::size_t alignColors = alignBytes / sizeof(keyleds::RGBAColor);

using keyleds::RenderTarget;

/// Returns the given value, aligned to upper bound of given aligment
static constexpr std::size_t align(std::size_t value, std::size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

/****************************************************************************/

RenderTarget::RenderTarget(size_type size)
 : m_colors(nullptr),
   m_size(size),                            // m_size tracks actual number of keys
   m_capacity(align(size, alignColors))    // m_capacity tracks actual buffer size
{
    if (::posix_memalign(reinterpret_cast<void**>(&m_colors), alignBytes,
                         m_capacity * sizeof(m_colors[0])) != 0) {
        throw std::bad_alloc();
    }
}

RenderTarget & RenderTarget::operator=(RenderTarget && other) noexcept
{
    free(m_colors);
    m_colors = nullptr;
    m_size = 0u;
    m_capacity = 0u;

    using std::swap;
    swap(*this, other);
    return *this;
}

RenderTarget::~RenderTarget()
{
    free(m_colors);
}
