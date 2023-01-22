## Miscellaneous stuff that lives in the `sys::state` object

There are some little things that are worth documenting, but which don't merit a documentation file on their own. This is where their documentation lives instead.

### The string pool

The state object contains a `std::vector<char>` named `text_data` containing win1250 codepage character values. This contains an amalgamation of all the text we may need, as extracted from the game files. **Do not** alter the contents of `text_data` while the game is running; only add data to it when making a new scenario or loading an existing one. Data inside the string pool should be accessed via the `text_tag` struct, which functions essentially as a smaller string view (it can be copied around freely and is 8 bytes in size). There are three convenience functions to help working with the string pool:

- `std::string_view to_string_view(text_tag tag) const` -- this function takes a `text_tag` and turns it into a conventional string_view.
`text_tag add_to_pool(std::string_view text)` -- this function takes the text within the string_view, appends it to the end of the `text_data` vector, and then returns a `text_tag` that represents the stored text.
`text_tag add_unique_to_pool(std::string_view text)` -- this function acts as the one above, except that it first searches the existing stored data to see if it can reuse some sequence from it. If such a sequence is found, no new data will be appended to `text_data`. Otherwise, it acts like the function above. This function is useful if you know that there is a good chance that the text you want to store is already somewhere in `text_data`. However, scanning the entirety of the stored text is not without a cost, especially as the amount of text grows, so do not use this function as the default way to add text.
