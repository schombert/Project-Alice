#pragma once

namespace logistics {


constexpr float construction_route_transport_leeway = 0.001f; // The minimum % of the total cost a route will attempt to consume and transport per day as extra leeway. This makes it so that constructions arent "stuck" for abit due to supply loss. Should be low enough that it isn't noticable.

}
