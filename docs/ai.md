## AI logic

### Estimations

- `static inline float estimate_strength(sys::state& state, dcon::nation_id n)` -- Exclusively estimate the power of this nation, plus subjects and vassals (but not allies).
- `static inline float estimate_defensive_strength(sys::state& state, dcon::nation_id n)` -- Estimate the defensive strength we would encounter if we were to battle against them (combined forces of the nation plus the allies).
- `static inline float estimate_additional_offensive_strength(sys::state& state, dcon::nation_id n, dcon::nation_id target)` -- Estimate the additional offensive strength in comparison to the target.
