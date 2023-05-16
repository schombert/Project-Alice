## Text data in the game

The text data for the game primarily comes from the `.csv` files in the `localisation` directory, although various other bits of character data end up here too, for lack of a better home. Each row in the `.csv` file consists first of a label that identifies the text, and then a number of columns, with each column corresponding to a translation in a different language (although many of them are empty). There are also lines that begin with `#` to denote comments. Be aware that this is not a standards-compliant `.csv` file. Quotation marks (`"`) have a special meaning in the `.csv` format, while here they are just quotation marks (and they don't always come in proper pairs, which is why following the `.csv` standard will break your parsing of these files. Within the text data there are a few peculiarities. First, you may encounter a character with value `0xA7` followed by a capital letter. This indicates that any following text must be displayed in a different color. There are also matched pairs of `$` characters that enclose variables that will need to be replaced before displaying the text. And, finally, the sequence `\n` is used to stand for a line break. To capture these peculiarities, the text for a particular key is not stored as-is. Instead it is broken up into a run of components. Each component is either: a sequence of plain text, a color change, a variable to be replaced, or a newline.

On some occassions, the `0xA7` escape might be a 3-byte escape sequence, of `0xEF`, `0xBF` and 0xBD` characters in that order, followed by a colour code. There is also a special handle case for question marks `?` characters; as they can be followed by a colour code, if they are indeed followed by a colour code - then they will be treated as a escape sequence.

Colour codes may be one of the following:
- `W` White
- `R` Red
- `G` Green
- `Y` Yellow
- `b` Black
- `!` Reset colour (aliased with White)
Any other colour code is undefined.

Specifically, the description of these text runs is stored in the `text_sequences` member of `sys::state`. This is a container indexed by `dcon::text_sequence_id`. In it are `text::text_sequence` objects, each of which contains a `starting_component` index and a `component_count`. These values can be used to retrieve the run of components from the `text_components` member of `sys::state`. Each of these components is a variant, namely `std::variant<line_break, text_color, variable_type, dcon::text_key>`. A `line_break` is just an empty struct signifying a line break, and a `dcon::text_key` is an index into the string pool (see more below) that functions as a handle to a sequence of text. `text_color` and `variable_type` are both enumerations that correspond to the possible text color changes and variable types. (Encountering a `text_color` in a sequence means that you should switch to rendering subsequent text in that color.)

Finally, you can look up these text runs by their key *in all lowercase* using the `key_to_text_sequence` map. Internally, this map stores `dcon::text_key` as its representation of its keys, but you can call `find` with either a `std::string` or a `std::string_view` to look up a text run.

### The string pool

The state object contains a `std::vector<char>` named `text_data` containing win1250 codepage character values. This contains an amalgamation of all the text we may need, as extracted from the game files. **Do not** alter the contents of `text_data` while the game is running; only add data to it when making a new scenario or loading an existing one. Data inside the string pool should be accessed via the `dcon::text_key` struct, which functions essentially as a smaller string view (it can be copied around freely and is 4 bytes in size). There are three convenience functions to help working with the string pool:

- `std::string_view to_string_view(dcon::text_key tag) const` -- this function takes a `text_tag` and turns it into a conventional string_view. If `dcon::text_key` is the special invalid tag, you will just get back an empty string view.
- `dcon::text_key add_to_pool(std::string_view text)` -- this function takes the text within the string_view, appends it to the end of the `text_data` vector, and then returns a `text_tag` that represents the stored text.
- `dcon::text_key add_to_pool(std::string const& text)` -- this function takes the text within the string_view, appends it to the end of the `text_data` vector, and then returns a `text_tag` that represents the stored text. If you have a non-null terminated string that you want to add, use the previous function, but if it is already in a `std::string`, use this one.
- `dcon::text_key add_unique_to_pool(std::string const& text)` -- this function acts as the one above, except that it first searches the existing stored data to see if the string is already stored in it. If the string is found, no new data will be appended to `text_data`. Otherwise, it acts like the function above. This function is useful if you know that there is a good chance that the text you want to store is already somewhere in `text_data`. However, scanning the entirety of the stored text is not without a cost, especially as the amount of text grows, so do not use this function as the default way to add text.

Implementation note: I go back and forth a bit on whether to store the size of the string in its identifying tag (currently `dcon::text_key`) or whether to store null characters in `text_data` to determine where one string ends and another begins. Currently I am storing null characters.