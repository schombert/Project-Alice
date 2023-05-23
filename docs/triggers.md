# Trigger bytecode

The triggers are stored into a bytecode which can then be optimized to reduce internal bytecode size.

All of the bytecode is stored in 16-bit cells. It's important to know the starting position of the first scope/instruction because the bytecode is position sensitive.

For example, take the following sequence of trigger bytecode:

| Position | Data |
|---|---|
| 0 | `trigger::generic_scope` |
| 1 | `1` |

This describes a scope with a payload size 1. The payload includes everything within the scope + the size of the payload itself.

| Position | Data |
|---|---|
| 0 | `trigger::is_disjunctive_scope | trigger::generic_scope` |
| 1 | `8` |
| 2 | `trigger::association_ne | trigger::owns` |
| 3 | `100` |
| 4 | `trigger::x_core_scope_nation` |
| 5 | `4` |
| 6 | `trigger::association_lt | trigger::blockade` |
| 7 | `trigger::payload(float(2.0))` |

This is roughly equivalent to the following code:
```sh
OR = { #Disjuntive scope
    owns != 100 #Does not own province 100
    all_core = { #x_core_scope_nation
        blockade < 1 #blockade is < 1
    }
}
```
The `trigger::is_disjunctive_scope` tells us we're dealing with a scope where we have to choose only one path (as opposed to AND where it chooses all paths). `trigger::generic_scope` marks this 16-bit cell as pertaining to a scope. This means the following cell contains the payload size. In this case, 8 cells (including the payload size itself) which is correct. Now every child within the trigger scope will need to be parsed individually - this makes the bytecode very fragile so it's important to double check that it's being read from a valid position. 

`trigger::owns` is a question of "do we own the following?" then `trigger::association_ne` negates the conditional question into "do we NOT own the following?". With `100` being the Id of the province. To obtain the fixed size of non-scope bytecode instructions a lookup table is used.

Afterwards, we see a `trigger::x_core_scope_nation` which corresponds to `all_core = { ... }` (`any_core` has an aditional OR'ed `trigger::is_existence_scope`). Next, the payload size, which is 4, this means the total data (plus the payload) has a size of 4. Then we obtain the first conditional, which is a `trigger::association_lt` "this is less than" OR'ed with a `trigger::blockade`
 "is this blockaded?", to form the question: "is the blockade value, less than x?", where `x` corresponds to the first cell, which is encoded from a float into an uint16_t by the means of `trigger::payload(float(2.0))`.
