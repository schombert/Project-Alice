## The Parser Generator: How it works

There is one important thing to keep in mind when discussing the parser generator, and that is the distinction between the generator, which parses a file describing the output we want from it, and the parsers that it generates, which can parse most of the data files that ship with the game and turn them into useful data. So when we are talking about "the parser" be sure to keep in mind whether we are talking about the parser inside the generator or the parsers that it generates.

### A brief history lesson

There are a number of weird and confusing things going on in the generator and its file format, and that is because of its relatively complicated history. Eventually it would be nice to turn it and the file it consumes into something nice and clean, but what we have right now was taken from the open v2 project with only minor adjustments. Originally parsing in open v2 was handled by template black magic that turned a type that was itself structured as a tree of types into a parser by using information at each node of that tree at compile time. This sounded extremely clever at the time, but was a real nightmare and would sometimes crash the compiler. As a minor improvement, parsers were then made with the aid of a number of macros, which were still a pain to compile, but at least were a little easier to work with. Eventually I realized there were some obvious performance improvements that I could get while greatly reducing compile times, and so I started to move to something like the generator we have here. However, that transition was not entirely completed in open v2, and most of the macro definitions still exist there.

During the development of open v2, the way that the parsers were used changed both as I tackled more and more of the files, which exposed me to various edge cases, and as I simply experimented with different ways of approaching things to try to find better solutions. The result of this is that the macro system grew to incorporate a number of different ways of doing things, and to make compatibility with that system easier the parser generator was built to be able to do basically everything. I hope that eventually we will be able to simplify things, because as I intend to more or less recreate the parser definitions from scratch I will try to stick to one way of doing things, allowing someone (someone else I hope) to go back, strip out the unused bits, and leave us with something nice and clean. (And I know of an additional, unimplemented, optimization for the generated parsers that I would like to put in too, one of these days.)

As a very rough approximation, we can pretend the generator is meant to support two different approaches to parsing. In the first approach, we parse a particular "group" in the file (something that looks like `key = { ... }`) by creating an object that stores any information contained in the group and then returning it. This is a very natural way of doing things because it creates a hierarchy of C++ objects that reflects the contents of the file, which you can then easily process after you are finished with the file. The second approach to parsing is to place data in its final destination immediately as you read the file. To do this, as you make your way through nested "groups" in the file you construct a "context" -- a representation of where you are in the file, roughly speaking, as well as references to the external objects you need to store the data -- which gets passed to the parser and which it in turn passes on to any functions it calls to handle the data. In the first approach, the parser stores values it encounters primarily by assigning them to member variables of the class it is associated with (more on this below); in the second approach it primarily calls functions on that data as it encounters it.

### The contents of the paradox-format files to ultimately be parsed

There is a bit of an idiosyncratic terminology that I have developed accidentally while writing these various parser iterations, and now I guess you have to learn it to make understanding the parser generator a little easier. Sorry.

At the most general level, a paradox-format file is a list of items, and each item can be one of four things.

First an item can be a `value` or `association` (sorry, both terms are used in places), which is typically in the form of `yyy = zzzz`. `yyy` and `zzzz` can be any sequence of characters not including spaces, tabs, newlines, or brackets, or they can be a sequence of characters containing any of those except newlines, so long as it is surrounded by quotation marks. The equal sign itself is called its `association type`. For any normal paradox-format file it is always `=`. However, to make modding easier, we also support `<`, `>`, `>=`, `<=`, `!=`, `==`, and `<>` as association types. (This isn't a new feature; it was already in open v2 and it would take more effort to take it out at this point.)

The next most common item is the `group`, which is in the form `yyy = { ... }` where `...` is itself a list of zero or more items. Strictly speaking, any of the parsers generated parses a single group (we treat the entirely of the contents of the file as an implicit group). When a parser encounters a group, it hands the contents of that group off to another parser and then does something with the return value it gets back.

Then we have the much rarer items. Occasionally we find a `free value`, i.e. something looking like just `yyy` but not followed by a supported association type (the most common of these are numbers). All of these are sent to the same free value handler. And then there is the rarest of all, the `free set` which looks like `{ ... }`, i.e. a list of items but without the label you expect to find on a group. The contents of each list are parsed by the free set handler.

Note that comments don't need to be accounted for; they are filtered out before the parser would see them. (That's right, there is a tiny preprocessor.)

### Defining a parser for the contents of a group

Keep in mind that each file is an implicit group, so the parser for each type of file begins with one of these which then calls into other group parsers.

For each parser we wish to define we must also define a unique class. The contents of the class will either be used to store the parsed data or to provide functions to process it with. There are two restrictions on these classes. First, they must be default constructible. Secondly they must support a `finalize` function that takes as a single parameter the context fed into the parser (this function is called when the parser is finished reading in the group, which can be useful when you want to do something with the data immediately, but can't until you have all of the contents of the group). If your parser doesn't use a context, I would just put `int32_t` for the type of this parameter (maybe one day we will get regular void).

We begin the definition for each parser by placing the associated type name on a single line by itself without any preceding spaces or tabs. Then each line following that one that start with a single tab will be used to define that parser. The meta-parser is, unfortunately, not designed for robustness, so be sure to use a single tab to start each line, use spaces and not tabs elsewhere in the line for justification, and do not insert additional blank lines within the definition for a particular parser.

For example, visually a parser definition could look like this:
```
type_name
	line 1
	line 2
	line 3
	...
```

#### Defining how values are handled

This section first describes your options for parsing a value of the form `yyy = zzz` when you know what the value of `yyy` is known beforehand. Note that `,` will be treated as whitespace, and thus can be freely used for formatting purposes. To parse such a value add either a line such as the following to the parser definition:
```
	yyy, value, type, handler_type
```
or a line such as
```
	yyy, value, type, handler_type (destination_name)
```
They keyword `value` must be present to tell the generator that we are parsing a value named `yyy` and not a group. `type` can be one of the following: `float`, `double`, `int` (for `int32_t`), `uint` (for `uint32_t`), `bool`, `text`, or `date` (`date` is not currently supported at the time of this document is being written because dates have not yet been put into Project Alice). This is simply a convenient way of doing common transformations on `zzz`. If you specify `text` you will get the text of `zzz` except with any surrounding quotation marks removed, if present. `handler_type` must be one of `discard`, `member`, `member_fn`, or `function` (the effects of this are described below). Finally, `(destination_name)` if present, must always be surrounded by parenthesis. When handling `zzz`, the key, `yyy` is also used as the name of the member, member function, or free function, unless you override this choice by specifying a destination name (you can use this to send multiple values to the same handler for example).

For each line you add to handle a value, you need to have something prepared to accept it, unless you want to discard it, which is what specifying `discard` for `handler_type` will do. If you specify `member` for `handler_type`, then the class associated with the parser must have a member with the correct destination name, and which is of a type with an `operator=` that can accept the type of the parsed `zzz`. If `handler_type` is `member_fn`, then the class must have a member with the destination name and which can accept the following parameters: an enum of type `association_type`, the type of the parsed `zzz`, an object of class `error_handler` by reference, an `int32_t` containing the line number that the left hand of the value was read from, and the context for the parser, either by value or reference (when the context is not being used, the convention is to make it type int32_t). The `association_type` parameter allows you to inspect the association type that was present for the value, and you can use the `error_handler`, and the line number, to record if the value cannot be accepted for any reason. Finally `function` works the same way as `member_fn`, except that it calls a free function with the destination and passes it as an additional first parameter the object used for parsing, by reference. `function` doesn't really provide much extra flexibility, and I will be trying to avoid it.

In addition to handling values where the left-hand side is a known quantity that is fixed in advance, you can also send both the left and the right hand sides of a value to a routine of your choice (assuming that it does not match an explicitly specific left-hand side). Doing this requires a line such as the following:
```
	#any, value, type, handler_type (destination_name)
```
and as usual, the content in parenthesis may also be omitted completely. And as with the case where you explicitly specify the left-hand side, the text `value` is necessary to indicate that this line applies to values. `type` and `handler_type` also take on the same values with essentially the same meanings as before. `discard` as a handler type works as it did before, and can be useful if you want to throw out all other values you haven't explicitly matched for without generating any errors. `member` is mostly useless in this context, as it does not communicate any information about what the left hand side of the value expression was.  `member_fn` and `function` are modified in that they will receive an additional `string_view` as their first or second parameters, respectively, which holds the left-hand side of the value expression.

#### Defining how groups are handled

Specifying how a group should be parsed is done in much the same way as it is for values, but with a few more options, as you can write the necessary line as either
```
	yyy, group, type, handler_type (destination_name)
```
or
```
	yyy, extern, function_name, handler_type (destination_name)
```
with `(destination_name)` being optional as before. Both of these lines will instruct the generated parser what to do with a group item that looks like `yyy = { ... }`. The difference between `group` and `extern` is that `group` will handle the content of the group (i.e. the items that make up `...`) by invoking another generated parser (by calling the parser that was associated with the class `type` with the content of that group and the current context. `extern`, on the other hand, will call the function with the given name with three parameters: an object of type `token_generator` by reference, an object of class `error_handler` by reference, and the context for the parser, either by value or reference. This is useful when you are actually using the context. The typical pattern with `extern` is to simply use it as a place to create a context with more information before invoking another generated parser to handle it.

`handler_type` and `destination_name` work essentially as they do for values, except that instead of handling the parsed right-hand side of the value, they handle the return value of the parser or other function, with the only difference being that there is no `association_type` parameter, since that is not meaningful for groups.

**NOTE** Another minor difference between `group` and `extern` is that even if you specify handling `extern` with `discard`, it will still be run on the contents of the group, which is useful if you are learning towards processing the data immediately and don't have anything useful to return. Yes, this makes reading the definition a bit misleading.

As with values, you may replace `yyy` with `#any` to parse groups where the token on the left of the `=` is not fixed in advance. And as with values, this adds an additional `string_view` parameter to the handler. In addition, a function called because of `extern` will also be passed a `string_view` containing the text to the left of the `=` as an additional first parameter (but not for automatically generated parsers called because of `group`, as they cannot take extra parameters).

#### Defining how free values are handled

#### Defining how free sets are handled