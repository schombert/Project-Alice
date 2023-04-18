## Miscellaneous stuff that lives in the `sys` namespace

### Dates

The few simple types required to work with dates can be found in the `date_interface.hpp` header. There, there are two types of dates defined. First we have the `absolute_time_points`. Generally speaking there should only ever be two of these, namely the
```
absolute_time_point start_date;
absolute_time_point end_date;
```
members of the `sys::state` object. The two store the full dates that are required to turn the compact dates we work with internally into ordinary-looking dates for the ui. Generally, unless the ui (or reading dates out of a text file) is involved in some way, there should be no need to use these member variables either in most places. You can construct an `absolute_time_point` with the help of the `year_month_day` struct as `sys::absolute_time_point{sys::year_month_day{Y, M, D}}`, which will produce an an absolute time point representing the given year (Y), month (M), and day (D).

Most dates that the program will use are encoded by the `date` struct, which internally stores the date as a number of days after the `start_date`. Dates can be operated on directly in these terms. You can construct a date from an integer, representing the number of days after the start date, and you can add integers to a date, thereby adjusting it that many days (this is how durations should typically be calculated, by advancing the date 30 days for a month or 365 days for a year, even if this doesn't advance the user-friendly date by exactly one month or one year). You can also construct dates with the help of the `year_month_day` and the absolute starting date. For example `sys::date{sys::year_month_day{Y, M, D}, start_date}` will result in a `date` representing the given year (Y), month (M), and day (D).

Finally, the convenience function `bool is_playable_date(date d, absolute_time_point start, absolute_time_point end)` is a quick way of testing whether a `date` falls within the playable range of the game.

### Events

- ```c++
void fire_event(sys::state& state, const std::variant<
    dcon::national_event_id,
    dcon::free_national_event_id,
    dcon::provincial_event_id,
    dcon::free_provincial_event_id
> event_id);``` : Fires up an event window; first checking if there is any available event windows on the pool, otherwise create a new window and add it to the pool. This allows us to reuse previously created windows.

Once the user selects an option on the event window, it will be hidden from view and pass the option choosen to the game state. All event windows are placed on a pool, distinguished by 3 types: major, national and provincial. Once a window is hidden, it is available to be grabbed by another upcoming event, otherwise a new window will need to be created and added to the pool.
