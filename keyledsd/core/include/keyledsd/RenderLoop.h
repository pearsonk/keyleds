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
#ifndef KEYLEDS_RENDER_LOOP_H_D7E4709F
#define KEYLEDS_RENDER_LOOP_H_D7E4709F

#include <mutex>
#include <vector>
#include "keyledsd/Device.h"
#include "keyledsd/RenderTarget.h"
#include "tools/AnimationLoop.h"

namespace keyleds {

/****************************************************************************/

/** Device render loop
 *
 * An AnimationLoop that runs a set of Renderers and sends the resulting
 * RenderTarget state to a Device. It assumes entire control of the device.
 * That is, no other thread is allowed to call Device's manipulation methods
 * while a RenderLoop for it exists.
 */
class RenderLoop final : public tools::AnimationLoop
{
    using renderer_list = std::vector<Renderer *>;
public:
                        RenderLoop(Device &, unsigned fps);
                        ~RenderLoop() override;

    /// Returns a lock that bars the render loop from using renderers while it is held
    /// Holding it is mandatory for modifying any renderer or the list itself
    std::unique_lock<std::mutex>    lock();

    /// Renderer list accessor. When using it to modify renderers, a lock must be held.
    /// The list only holds pointers, which must be valid as long as they remain
    /// in the list. RenderLoop will not destroy them or interact in any way but
    /// calling their render method.
    renderer_list &     renderers() { return m_renderers; }

    /// Creates a new render target matching the layout of given device
    static RenderTarget renderTargetFor(const Device &);

private:
    bool                render(unsigned long) override;
    void                run() override;

    /// Reads current device led state into the render target
    void                getDeviceState(RenderTarget & state);

private:
    Device &            m_device;               ///< The device to render to
    renderer_list       m_renderers;            ///< Current list of renderers (unowned)
    std::mutex          m_mRenderers;           ///< Controls access to m_renderers

    RenderTarget        m_state;                ///< Current state of the device
    RenderTarget        m_buffer;               ///< Buffer to render into, avoids re-creating it
                                                ///  on every render
    std::vector<Device::ColorDirective> m_directives;   ///< Buffer of directives, avoids new/delete on
                                                        ///< every render
};

/****************************************************************************/

} // namespace keyleds

#endif
