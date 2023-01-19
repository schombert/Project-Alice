## Other miscellaneous parsing tools

All of these functions are declared in `parsers.hpp`

### Converting text to values

The following functions are provided to turn text into the appropriate kinds of values. They are fairly self explanatory, except that I will note that they all expect to receive just the text containing the value, and not any surrounding whitespace (see the next section for that).

```
float parse_float(std::string_view content, int32_t line, error_handler& err);
double parse_double(std::string_view content, int32_t line, error_handler& err);
bool parse_bool(std::string_view content, int32_t line, error_handler& err);
int32_t parse_int(std::string_view content, int32_t line, error_handler& err);
uint32_t parse_uint(std::string_view content, int32_t line, error_handler& err);
association_type parse_association_type(std::string_view content, int32_t line, error_handler& err);
```

### Other utility functions

```
std::string_view remove_surrounding_whitespace(std::string_view txt);
```
This function strips the common forms of ASCII whitespace (` `, `\t`, `\r`, `\n`) off of a string (well, technically it just moves the view around, but it works out to the same thing.

### Reading text from a "csv"

Paradox shipped Victoria 2 with some data in a pseudo-csv format. Unlike your ordinary csv files it may contain comments (I don't think ordinary csv files have comments, right?), and ignores the csv rules for quotation marks. When you open up a csv file to begin parsing data from it, the first thing you want to do is write the following lines:
```
if(sz != 0 && cpos[0] == '#')
	cpos = parsers::csv_advance_to_next_line(cpos, file_data + sz);
```
where `cpos` is your current position into the file data, `file_data` is the beginning of the file (so probably equal to `cpos` when you are doing this), and `sz` is the number of bytes in the file. This will advance the position past the first comment. You will never have to do this again manually, and all subsequent comments will be skipped for you.

To read data out of the csv file, you will use one of the following two functions:
```
template<size_t count_values, typename T>
char const* parse_fixed_amount_csv_values(char const* start, char const* end, char seperator, T&& function)
```
or
```
template<typename T>
char const* parse_first_and_nth_csv_values(uint32_t nth, char const* start, char const* end, char seperator, T&& function)
```

The first function, `parse_fixed_amount_csv_values`, must be supplied with an explicit integer template parameter for `count_values`, then it will call `function` (probably a lambda you write) once for each non-comment line of the file  with an array of `std::string_view` of size `count_values` containing the first `count_values` many items in that line of the csv file.

The second function, `parse_first_and_nth_csv_values`, works in basically the same way. It calls `function` once per line of the file with two parameters: a `string_view` containing the first item on the line and a `string_view` containing the `nth` item on the line. This is useful when reading text out of the files that contain the translations, as for each you need they key, the first item, and then some nth item corresponding to the language you want.

The `separator` parameter for both functions determines the character that is used to mark where one item of the csv file ends and the next begins. For us, I believe that this will always be `;`.

**Note** The items you get out of a csv file may include leading and/or trailing whitespace. Use `remove_surrounding_whitespace` first if you want to get a value out of them.

### Parsing the definitions lua-like file

I was thinking of properly defining some functions for this, but since it is one file that we will be parsing once, and there are no other places for any such functions to be relevant, I plan on simply doing something ad-hoc. If you really want an explanation of how I will parse the file, let me know.