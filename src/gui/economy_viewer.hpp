#pragma once

namespace sys {

struct state;

}

namespace economy_viewer {

struct economy_viewer_state;
void update(sys::state& state);
void render(sys::state& state);
}
