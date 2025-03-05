## Super Console

### Player's Guide

In version 1.1.3 the in-game console (the window that pops up when you press `~`) has been completely reworked. Instead of supporting a fixed list of commands, the console now supports a complete programming language that can let you inspect and modify nearly any part of the game. That is probably a bit much for the average cheater, so this part of the guide will walk you through making the new console do the same things as the old one. Note that the new console is sensitive to capitalization, spaces, and the exact format of values (10.0 gets you a floating point value while 10 gets you an integer value, and commands that expect one won't accept the other). If your console input worked you will see a green `ok.` appear. Otherwise, you will get an error. If you encounter an error, carefully recheck your spelling and spacing and try again.

#### Changing infamy, prestige, and other values

Here is how you can use the console to change some common values:

- `10.0 TAG plurality !` : set the plurality of the three letter tag to 10.0 (note, the decimal place is required)
- `10.0 TAG prestige !` : set the prestige of the three letter tag to 10.0 (note that this may not be reflected in the displayed prestige exactly because of permanent prestige modifiers)
- `10.0 TAG infamy !` : set the infamy of the three letter tag to 10.0
- `10.0 TAG 0 >commodity_id stockpiles !` : set the treasury of the three letter tag to 10.0
- `10.0 TAG research_points !` : set the research points of the three letter tag to 10.0
- `10.0 TAG diplomatic_points !` : set the diplo points (what you pay for diplomatic actions) of the three letter tag to 10.0
- `10.0 TAG leadership_points !` : set the leadership points (what you pay for generals and admirals) of the three letter tag to 10.0

Command to display all commodities IDs into console:
`commodity-size while 1 - dup 0 >= loop dup dup >commodity_id @ name @ localize swap end-while drop`

#### Special console functions

And here is a list of some more specialized console functions that you can use

- `clear` : removes old text form the console window
- `spectate` : switches the game to spectator mode (use `change-tag` to resume playing)
- `true fps` : turns the visible FPS counter on. A value of `false` will instead turn it off
- `true provid`: Turns on and off display of content province ids in tooltips
- `false set-auto-choice` : turns off all existing auto event choices
- `TAG change-tag` : changes who you are playing as to TAG
- `TAG true set-westernized` : changes the civilized/uncivilized status of TAG
- `TAG 2030 fire-event` : Fires event 2030 for the TAG. (Note that this number must correspond to a national event that can occur randomly.)
- `make-crisis` : forces some crisis to start if one currently isn't going on
- `end-game` : switches to the end-game screen
- `TAG 10.0 set-mil` : sets the militancy of all pops in TAG to 10.0
- `TAG 10.0 set-con` : sets the consciousness of all pops in TAG to 10.0
- `TAG TAG make-allied` : creates an alliance between the two tags
- `true cheat-wargoals` : allows you to always add wargoals while it is enabled
- `true cheat-reforms` : allows you to change to any political reform while it is enabled
- `true cheat-diplomacy` : while it is enabled the AI will say yes to anything that you propose
- `true cheat-decisions` : you can ignore the `allow` requirements of decisions while it is enabled
- `true cheat-decision-potential` : you can ignore the `potential` requirements of decisions while it is enabled
- `true cheat-army` : your land units build instantly while it is enabled
- `true cheat-navy` : your naval units build instantly while it is enabled
- `true cheat-factories` : your factories build instantly while it is enabled
- `TAG true instant-research` : research for the TAG will start completing instantly
- `TAG complete-construction` : instantly completes the construction of all province buildings in TAG
- `TAG TAG conquer` : the first TAG immediately conquers all provinces of the second TAG
- `PROV123 TAG make-core` : adds a core of TAG to the province
- `PROV123 TAG remove-core` : removes any core of TAG in the province
- `PROV123 TAG set-owner` : makes TAG the owner of the province
- `PROV123 TAG set-controller` : makes TAG the controller of the province
- `365 add-days` : adds 365 days to the current date
- `dump-oos` : makes an oos dump
- `true daily-oos-check` : makes the OOS check daily instead of monthly
- `dump-econ` : puts some economic data in the console and starts econ dumping
- `vanilla save-map` : makes an image of the map. `vanilla` can also be replaced by one of the following to alter its appearance: `no-sea-line`, `no-blend`, `no-sea-line-2`,  and `blend-no-sea`
- `load-file ...` : loads the file named `...` (relative to your documents\Project Alice directory). This isn't very useful unless you have created a set of common functions (see the documentation below) that you want to save in a file to reuse.
	
#### `player @` and province names

Wherever you see a `TAG` in the above commands you can also substitute `player @` (note the space) which will act as the tag of whatever nation you are currently playing. Wherever you see `PROV123` in the above commands you can also substitute `selected-prov @` which will act as the province that is currently selected in the main user interface. Also, a quick note on province names. Being able to refer to `PROV123` relies on that being the province's name at the time the console was first opened. Initially, all provinces are named following that pattern (PROV plus their id from the game files). However, this may change because of renaming effects. If you load a game where a province has been given a new name, you will no longer be able to refer to it by its original `PROV123` name.
 

### Super User's and Developer's Guide

The new console has a complete programming language embedded within it. I am not going to try to document that language in its entirety here. Instead, this section will provide an overview of what you need to know to get access everything stored in the internal data container and to write your own simple functions. For a developer, this is probably not good enough to be a replacement for the proper use of the debugger. However, it can make some of your debugging tasks a bit easier, such as looking up particular property values associated with a given id or determining what the text corresponding to a particular key is.

#### The basics: a stack language

The language used in the console is a statically typed stack-oriented language. Almost everything that the language does involves a manipulation of the *data stack*. We will illustrate the contents of the data stack in this document as a horizontal series of boxes. The box on the left is the bottom of the stack, while the box on the right is the top of the stack. Items are always added and removed from the top of the stack.

```
┌─────┬─────┬─────┐
│  A  │  B  │  C  │
└─────┴─────┴─────┘
```

For example, the above is a picture of a stack with three items on it, A, B, and C where A is on the bottom of the stack and C is on the top.

A line in the console is divided into what are known as *words*. Words are separated from one another by white-space characters. Otherwise, there are no restrictions on the characters that can compose a word (so it is important to remember that, unlike languages in the `C` family, `a+b` would be a single word, not three words). These words are also case sensitive, so `PROV123` is a different word than `prov123`. Each word in the line is executed, from left to right. And that's all there is to the core syntax of the language. Numbers (both integers and decimal values) push their value to the stack. Integers push an `i32` type value (signed 32 bit integer) to the stack and decimals push a `f32` value (32 bit floating point value, equivalent to a `C` float). Thus the line `-3 4.5 100` would produce the following stack:

```
┌───────────┬────────────┬────────────┐
│  i32: -3  │  f32: 4.5  │  i32: 100  │
└───────────┴────────────┴────────────┘
```

If you type that line in the console and hit enter, you will see each of the three values printed below it followed by `ok.`. This is because, after a line on the console is executed, whatever is left on the stack is then printed back to the console, followed by `ok.` to indicate that it has executed without any errors. If your command leaves nothing on the stack, you will just see `ok.`.

#### The basics: data types

As mentioned above, the language is statically typed, although type inference takes care of most things. However, you do need to be aware that the language does no implicit type casts, so if you want to add values of different types together, for example, you will need to explicitly cast them to the same type. (Note that the command will not run with mismatched types, so you won't get unexpected behavior if you lose track of the types.) Common numerical types you may encounter are `i8`, `i16`, `i32`, `i64` (signed integers of various bit counts), `u8`, `u16`, `u32`, `u64` (unsigned integers of various bit counts), `f32`, and `f64` (floating point types corresponding to `float` and `double` from `C`). The numerical types can all be converted into each other, with conversions generally following the `C` rules. Other types you may see are `bool` (modeled as an unsigned 1 bit integer) and `ptr(...)` which is a pointer to the `...` type. For example `ptr(i32)` is a pointer to a 32 bit integer. `ptr(nil)` is the equivalent of `void*` from `C`. These are the most common types, but they are not all the types in the language, and new types can be added by users. Our data container interface automatically generates types for the various ids and other indexes, for example, which we will come back to later.

Changing a value from one numerical type to another is done by a family of `>...` functions where `...` is the name of the destination type. For example `>i32` converts a value to a 32 bit signed integer. In a more conventional programming language you would say that each of these functions has a single argument, the value to be converted, and returns a single value, the result. However, we have a stack-oriented language. So instead of thinking of the function as having a single parameter, we instead model it as consuming a single value from the top of the stack. And instead of thinking of it as returning a single value, we think of it as adding a single value to the top of the stack. Thus, we model the execution of `-3 4.5 >i32` as doing the following:

`-3 4.5` pushes two values to the stack
```
┌───────────┬────────────┐
│  i32: -3  │  f32: 4.5  │
└───────────┴────────────┘
```
then, `>i32` consumes a single value from the stack
```
┌───────────┐
│  i32: -3  │
└───────────┘
```
finally, `>i32` pushes its result back to the stack
```
┌───────────┬──────────┐
│  i32: -3  │  i32: 4  │
└───────────┴──────────┘
```

#### The basics: mathematical operations

The language supports the basic math operations `+`, `-`, `*`, `/`, `mod` and comparisons `=`, `<>`, `<=`, `<`, `>`, `>=` (note that, unlike `C`, equality comparison is `=` not `==` and inequality is `<>` not `!=`) between pairs of values of the same numerical type. Binary operations like this consume both of their stack inputs and place their result back on the stack. This result will be of the same type for the first five operations and of type `bool` for the comparisons. The value on the top of the stack is the value conventionally thought of as the "right hand" side of the operator and the value below it is the value on the "left hand" side. (You can think of the operator as being placed between the last two items in one of the stack illustrations, if that helps.

For example `-3 4 -` does the following:

`-3 4` pushes two values to the stack
```
┌───────────┬──────────┐
│  i32: -3  │  i32: 4  │
└───────────┴──────────┘
```
`-` removes both items to compute -3 - 4.
```
╷
│
╵
```
`-` places the result back on the stack
```
┌───────────┐
│  i32: -7  │
└───────────┘
```

#### The basics: stack manipulation

Sometimes you will end up with values on the stack that aren't quite in the order that you need them. The preferred solution is to rewrite the expression so that you leave values on the stack in the order that they need to be consumed. When that isn't possible, well, then we have the stack manipulation words. The most basic of the manipulation words are `dup`, `drop`, and `swap`.

`dup` makes a copy of the item on the top of the stack an pushes it to the stack, turning, for example,
```
┌───────────┬────────────┐
│  i32: -3  │  f32: 4.5  │
└───────────┴────────────┘
```
into 
```
┌───────────┬────────────┬────────────┐
│  i32: -3  │  f32: 4.5  │  f32: 4.5  │
└───────────┴────────────┴────────────┘
```

`drop` does the opposite of `dup`: it removes the top item of the stack without doing anything with it, turning 
```
┌───────────┬────────────┐
│  i32: -3  │  f32: 4.5  │
└───────────┴────────────┘
```
into
```
┌───────────┐
│  i32: -3  │
└───────────┘
```

Finally, `swap` exchanges the two top items on the stack, turning
```
┌───────────┬────────────┐
│  i32: -3  │  f32: 4.5  │
└───────────┴────────────┘
```
into 
```
┌────────────┬───────────┐
│  f32: 4.5  │  i32: -3  │
└────────────┴───────────┘
```

Sometimes you need to dig down deeper into the stack. For that I will briefly introduce `>r`, `r>`, and `r@` without saying too much about them. In addition to the data stack, the language also contains a second stack, called the *return stack*. This was traditionally used mostly for control flow, and few words directly interact with it. `>r` removes the top item from the data stack and pushes it to the top of the return stack. `r>` does the reverse: it removes the top item of the return stack and pushes it to the top of the data stack. Finally `r@` makes a copy of the top of the return stack (without altering it) and pushes the copy to the top of the data stack.

You should always strive to take everything off of the return stack that you place there. Items left on the return stack will not be displayed in the console (although they will be safely discarded, so they won't interfere with any subsequent work). But, it is perfectly safe to put items there temporarily. For example, you may encounter a sequence such as the following `>r >r ...some work done here... r> r>`. This takes two items from the top of the stack and stores them on the return stack temporarily so that the work can be executed as if it were two stack items lower. Then, when the work is done, the top two items are restored to their original position (and order).


#### The basics: pointers

The last of the language basics to be aware of before we get into examining and manipulating the state of the game is pointers. Often (although not always) the values that will immediately returned by functions operating on the state of the game will be pointers to enable both reading the value and setting it with a single function. Reading the contents of a pointer is done with the word `@`. `@` takes the pointer value from the top of the data stack, dereferences it, and then pushes the contents of the pointer to the stack. You can also think of `@` as replacing a pointer with the value to which it points. The word `!` is used to store a value to the location pointed to. `!` consumes both a pointer from the top of the stack and a value of the corresponding type under it. The stack picture before you use `!` should look like the following:
```
┌┄┄┄┄┄┬─────────────────┬─────────────────┐
│ ... │     T: value    │ ptr(T): pointer │
└┄┄┄┄┄┴─────────────────┴─────────────────┘
```
This may mean that you end up writing a line such as the following `value ...expression generating pointer... !` where the use of `!` ends up at some distance from the value that will ultimately be stored.

If you need to change the type of a pointer for some reason, you can use the powerful `ptr-cast` word. The word `ptr-cast` must be followed by the name of a pointer type (for example: `ptr-cast ptr(i32)`). The pointer cast function then takes the pointer on the top of the stack and changes it to the specified type. Be aware that writing to a pointer that is changed to another type can easily result in data corruption. It is almost always better to change the type of the data that you are storing to match the type of the pointer, rather than the other way around.

#### Interacting with the data container: ids

Types are automatically generated for each of the data container keys with the same names (although, with no need for a namespace qualification). For example `dcon::text_key` is available as type `text_key` from within the console. Any such key can be generated directly from an i32 value using `>` followed by the name of the key as a single word. For example `0 >text_key` will result in a stack containing a single `text_key` type for the 0th indexed key. The word `>index` performs the inverse operation, removing an id from the stack and replacing it with its index value.

In addition to using an index to generate the id you want, you can push certain ids to the stack using their individual names. Specifically, for most objects with a `name` property in the data container you can use the the text key that is stored there (the unlocalized key, not its display value) to get the id for that particular object. For example `french` pushes the `culture_id` for the French culture to the stack. The three letter TAG identifiers also work basically in this way, but instead of pushing the national_identity_id, which might seem like the logical choice, they push the `nation_id` of the nation currently holding that tag, since that is more convenient for the average user.

***Important Note:*** developers should keep in mind that the internal value stored inside a C++ data container id is *not* the same as its index. Instead, an internal value of 0 is the invalid id, an internal value of 1 is index zero, and so on. So, if you see an id storing a value of 15 in the debugger, that would be the key produced by the command `14 >id_type`.

The word `valid?` takes an id value from the stack and pushes a boolean that is true if the id contains a valid index or false if it is the representation of the invalid key. If for some reason you need the invalid key from within the console, the sequence `make key_name` will push an instance to the top of the stack.

For most ids, if you leave them on the stack you will simply see the word `#struct` appear for them in the console, with no further details. However, there is an exception for the `text_key` type. If you leave that type on the stack, the console will display the unlocalized contents of the corresponding key value. You can also use the word `localize` on `text_key` values. This will remove the id from the stack and replace it with a new value representing the localized contents of the key. If that value is left on the stack, the console will then display that localized content.

#### Interacting with the data container: accessing values

The properties of a data container object are all available as words from the console (non-developers: there is a description of these objects and their properties that should be mostly understandable by non-programmers located in the repository's `src/gamestate/dcon_generated.txt` file). Each of these words will consume the corresponding id type from the top of the stack and push a pointer the value of that property for that object instance. For example, `0 >national_identity_id adjective` will return a pointer to the `text_key` storing the default adjective for the 0th national identity object. If you aren't planning on modifying that value, this will almost always be followed by a dereference operation, so your console command might look like. `0 >national_identity_id adjective @` or even `0 >national_identity_id adjective @ localize`. In general, the data types of properties correspond to the data types described in the previous sections in ways that I hope are obvious.

That is really all you need to know for most properties, but there are some trickier cases. First, there are array type properties, such as
```
property{
	name{ government_adjective }
	type{ array{government_type_id}{text_key} }
}
```
For such a property you need both the id for the object instance and an index into the array (in this case, a `government_type_id`). The index into the array should be the top item of the stack, with the id of the object instance immediately below it. For our example here, we might write something like `0 >national_identity_id 1> government_type_id government_adjective @`.

Retrieving information from pooled vector type properties also has additional complications. For a property (from the leader_images object) such as
```
property{
	name{ generals }
	type{vector_pool{2000}{gfx_object_id}}
}
```
the word `generals` will yield a proxy type representing the individual array stored for a particular object instance. The word `size` will then pop that proxy off the stack and put in its place an `i32` containing the size of that particular array. Or, if an `i32` value is pushed to the stack on top of the proxy object, the word `index` will return a pointer to the value stored in that index of the array. For example `0 >leader_images generals size` will yield the number of general images stored for the 0th leader images object instance. `0 >leader_images generals 2 index @` will return the value (a `gfx_object_id`) for the 3rd general image stored for the 0th leader images object instance.

As with an object's properties, it is possible to traverse the relationships that an object is involved in. Unfortunately, things may be a little less intuitive here. The C++ interface automatically generates a number of helper functions that are missing from the console interface, making things slightly more verbose.

To get from an object instance to a relationship it is involved in a word is generated from the name of the relationship + `-` + the name of the link in the relationship. For example, `culture_group_membership-member` is the word provided to convert a `culture_id` into a `culture_group_membership_id`. However, what this word yields depends on the nature of the link. An ordinary single-item link will yield a pointer to the id. But if that link is also the primary key of the relationship then it will just produce the id itself as a value, and not as a pointer (because the id linked to is computed, not stored, and so there is no memory to point to). Finally, if this is a many-to-one link, then the word will yield an array proxy as described for the vector pool type properties above. This is what you will get if you use the `culture_group_membership-group` word on a `culture_group_id`, namely an array proxy that stores all the `culture_group_membership_id`s for relations involving the group.

Frankly, it will probably take some experimenting to become familiar with how to extract the data from particular relationships. As a guide, you can also look at the contents of the `fif_dcon_generated.hpp` file which contains the definitions that make all of the words for accessing properties. Although you may not understand exactly what is going on in those definitions, I do think that you can, with the information contained in this document, work out enough to tell if a particular word is performing an index calculation to return a value, or if it is generating an array proxy object, or if it is returning a pointer to a particular value.

Converting the id of a relationship to that of an object instance involved in it is less verbose: the name of the link by itself serves as the word for doing that.

#### Interacting with the data container: more complex functions

As the previous section has shown, traversing a relationship to move from one id to another can become quite verbose, and certainly not the sort of thing that you want to enter into the console repeatedly. Now that I have enabled ctrl+v for the console, one solution would be to create a document with the commands you have created. However, you can also declare new functions in the console to package commands you write into a single word (although, note that this will not survive beyond a given session). The syntax for doing that is:
```
: word-name .... contents .... ;
```
The word immediately after the `:` will be the word that invokes your definition in the future. The remaining words prior to the `;` are the commands that will be executed whenever your new word is used. Let's walk through an example of how you might use this. Suppose, following on from the previous section, you want to be able to list the names of the individual cultures belonging to a particular culture group. To do that for one name you might enter `0 >culture_group_id culture_group_membership-group 2 index @ member name @ localize` (this will get the localized name of the third culture). Obviously, doing that is quite the typing exercise, and you wouldn't want to enter it repeatedly. So, first we need to factor out the "variables". Let us suppose that you want to be able to change the `culture_group_id` and the index of the culture you are looking for. And let us also say that you want to use your word as `0 >culture_group_id 2 member-name`. In that case the index would be on the top of the stack and the id would be below it when your word runs. We start by cutting off the hard-coded culture group id from the command, producing `culture_group_membership-group 2 index @ member name @ localize`. The first word there expects the group id to be on the top of the stack, not the index. So we will start by getting the index out of the way with `>r` giving us `>r culture_group_membership-group 2 index @ member name @ localize` As we proceed through these commands, we eventually get to `2`. But we don't want it to be always 2, we want the index that we put on the return stack earlier, and so we replace 2 with `r>` This give us `>r culture_group_membership-group r> index @ member name @ localize`. We turn that into a definition like so `: member-name >r culture_group_membership-group r> index @ member name @ localize ;`. Once we hit enter the console tells us `ok.` and we can then use our new word. Note that words are "compiled" when they are first used, not when they are defined with `:`, so any mistakes will be revealed when you first try to use it. I suggest trying out your commands before turning them into words to find any bugs first.
