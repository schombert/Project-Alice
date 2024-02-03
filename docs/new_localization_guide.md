## Text and Localization Guide

This document is a working draft. The final version will be located in the repository for the ui system.

### Overview

Those of you familiar with Victoria 2 modding will have encountered the localization .csv files before. Each line of those files contains a key followed by entries in a number of different languages. When the game needs to display text in the user interface, it uses a key to look up that text (and chooses the appropriate language at that point).

The new ui system will use something that is conceptually similar, although with a different implementation. Our system will also rely on a collection of files that define text for various keys. However, for us each file will contain only a single language, and switching the displayed language will instead mean switching to a different set of files.

### Conceptual Point: Attributes

Before we can discuss the details of the file format, we have to first cover a conceptual point. In our system, each piece of text can have between zero and eight *attributes*. An attribute, in the most general terms, conveys extra syntactical information about the text. Originally, these attributes grew out of the need to correctly handle plural forms. In English, we write "1 apple" and "2 apples", and this means that we cannot simply substitute a number into a blank such as `___ apples` because it would sometimes produce an ungrammatical result. The solution to this problem is to not write a single "apples", but to instead write both "apple" and "apples" and to select which to use based on the *attribute* of the text that fills in the blank (in this case, whether it is a singular value or not.

There are a number of attributes that are based on the categories needed to form cardinal and ordinal forms [found here](https://www.unicode.org/cldr/charts/44/supplemental/language_plural_rules.html). (A cardinal form is like "apple" vs. "apples", while an ordinal form is the information needed to know whether to write "st", "nd", "rd", or "th" after a number.)

For convenience, the following attributes are built in for numbers: `zero`, `one`, `two`, `few`, `many`, and `other` for the cardinal categories and `ord_zero`, `ord_one`, `ord_two`, `ord_few`, `ord_many`, and `ord_other` for the ordinal categories. Most numbers will come with one of each. Note that, despite their names, the meaning of these attributes depends entirely on the rules described in the previous link, and varies from language to language. Finally, there is `z` (i.e. true zero), which is applied to text that denotes zero (not to be confused with the `zero` attribute mentioned earlier, which does a different job). `z` is provided so that you can produce text such as "no apples" instead of "0 apples" if you want.

Note that up to 255 total attributes can be defined. Any attribute that you attempt to use will function, as unknown attributes are automatically defined as they are encountered. You can use these attributes to substitute adjectives of the correct grammatical gender for the noun that they modify, use the correct "a" or "an" article, and so on.

### Keys and Content

Each file consists of a number of *keys* and their accompanying text *content*. All text in the file must be utf8 encoded. A key is separated from its content by one or more white-space characters, and content is separated from the next key by one or more newlines. A key consists of a key name followed, optionally, by one or more attributes, each separated from the key name and each other by `.` characters. For example, `apple` is a key consisting of just the key name `apple`. `apple.many` is the key name `apple` plus the attribute `many`. And `apple.many.def` is the key name apple plus two attributes: `many` and `def`. A key name may appear more than once in the file. All the keys for a single key name are grouped together, and it is possible to select one of the particular contents associated with that key name by specifying attributes to match against, as described below. Note that a key name may not contain the following characters: `.`, `?`, `{`, '}', or '\'.

### Special Content Sequences

Unless otherwise specified, the content for a key will end up reproduced verbatim in the program. The exceptions have to do with the characters `\`, `{`, and `}`. In general, you can think of the `\` character as starting a special command that will make the displayed text do something that you could not otherwise literally express.

#### Simple replacements

There are three commands that are `\` followed by a single character: `\\`, `\n`, and `\t`. When encountering one of these, it is replaced by a single character. `\\` turns into a single `\` character, `\n` into a line break, and `\t` into a tabular space. Note that if these commands are not immediately followed by another command, such as `\n\t`, that they must be separated by a space from the text that follows them. For example: `Line 1\n Line 2`. The space following `\n` will not appear in the text; there would be no space before `Line 2` in the previous example when the text was displayed.

#### Formatting commands

The following commands are all written followed by a pair of braces (`{ ... }`), and they apply to the text between those braces.
- `\it{ ... }`: the text within the braces will be displayed in italics if possible.
- `\b{ ... }`: the text within the braces will be displayed in bold if possible.
- `\sc{ ... }` the text within the braces will be displayed using small capitals if possible.
- `\os{ ... }` numbers appearing withing the text between the braces will be displayed in old style numerals if possible.
- `\tab{ ... }` numbers appearing withing the text between the braces will be displayed in tabular/lining numerals if possible.
- `\sup{ ... }` the text within the braces will be displayed as a superscript if possible.
- `\sub{ ... }` the text within the braces will be displayed as a subscript if possible.

#### Special characters

The `\c{ ... }` command is used to insert special characters that may be hard to type otherwise. The `...` can be one of the following:
- `left-brace`: inserts the `{` character
- `right-brace`: inserts the `}` character
- `em-space`: inserts an em space
- `en-space`: inserts and en space
- `3rd-em`: inserts a third-of-an-em space
- `4th-em`: inserts a fourth-of-an-em space
- `6th-em`: inserts a sixth-of-an-em space
- `thin-space`: inserts a thin space
- `hair-space`: inserts a hair space
- `figure-space`: inserts a figure space (i.e. the width of one lining numeral if the font supports it)
- `ideo-space`: inserts an ideographic space (suitable for CJK glyphs)
- `hyphen`: inserts a hyphen
- `figure-dash`: inserts a figure dash (i.e. the width of one lining numeral if the font supports it)
- `en-dash`: inserts an en dash
- `em-dash`: inserts an em dash
- `minus`: inserts the mathematical minus sign
- `####` (any decimal number): inserts the unicode codepoint with that value

#### Other Formatting

- `\o{ ### }{ ... }`: this is the command for changing the color of the text. The text within the second pair of braces will be rendered using the palette entry specified by the number within the first pair of braces.
- `i{ ### }`: this command inserts the icon designated by the number between the braces at its location in the text. Note that the icon corresponding to a given number depends on the context in which the text appears (you cannot depend on `i{0}` always producing the same icon). If the nature of the icon that will appear matters to the translation, you will need to ask one of the programmers about it.
- `\a{ name }`: this command attaches an additional attribute to the text (it will have no visible effect on the displayed text, and it does not matter where the command appears, so I suggest placing all such commands at the end). This is one way of getting information out of a variable substitution (discussed below). For example, if our file contained `apple.many apples\a{vowel}`, then we could match against the provided attribute (`vowel`) to detect, say, whether the term that the key name resolved to began with a vowel. (Note that I do not anticipate this being needed much in practice).

#### Variable substitution

Much of the in-game text is generated not just by grabbing text directly from the content of a key, but by using it as a template into which various details, such as names and numbers, is inserted. To make this work, the content exposes places where the information should be inserted, referred to as *variables*. Because more than one piece of information may need to be inserted at the same time, each variable has a unique name. The names of these variables are determined by the code that uses the text. Thus, the names of the variables themselves must not be translated or altered.

Functionally, a place for a variable replacement is denoted simply by `{ ... }`, where `...` is the name of the variable. For example `{number} percent complete` can be used with the variable `number` to create text such as `5 percent complete`. You may also append one or more attributes to the variable by following it with `.`s. For example `{value.a.b}` is a variable substitution for the variable `value` with additional attributes `a` and `b`. If the text being substituted into the variable comes from a key elsewhere in the file (which, for almost everything except numbers, it will), then these attributes will be uses to select, from that family of keys, the one that best matches the attributes. If the file has `apple apple` and `apple.many apples`, then passing the key `apple` to `{value.many}` will result in `apples` appearing instead of `apple`. This could also be used to match the grammatical gender of an adjective to the noun it modifies, for example.

#### Alternative selection

Finally, we come to the most complicated part of the localization system: selecting alternate text sequences based on attributes. On an abstract level, what this mechanism is for is allowing us to generate text that is, for example, either "apple" or "apples" appropriately within a longer piece of content based on the attributes of the variables. This is done, syntactically, with the `\match` command. Each `\match` command is followed by any number of braced pairs `{ ... }{ ... }`. The first member of the pair contains the *conditions* for the match and the second member of the pair contains the *content* of the match. When a `\match` command is encountered, the conditions of each pair are checked, in order, and the content of the first pair where the conditions are satisfied is substituted into the text (the content of all other pairs is ignored).

##### Match conditions

The conditions for a pair are 0 or more tests for attributes. Each test is expressed as a variable (just as you would write it for a variable substitution, as described above), followed by a `?` character, and then followed by up to 8 attributes, each separated by `.`s from each other. For each part of the condition, the variable will be looked up *as if* a variable substitution was occurring (and thus `name.attr?...` can be used to pick a particular member of a key set). Then, it will be checked to see if it possesses each attribute listed after the `?`. If it does, then its part of the condition is a match. If all parts of the condition match, then the content for that condition will be substituted into the text, and the remainder of the match pairs will be skipped. If some part of the condition is not a match, then the process will instead repeat with the next pair. Note that an empty pair of braces, `{}`, will always be a match, and so it can be used as the last condition in a pair to ensure that some content from the `\match` command will always be used.

##### Match content

All commands, except for further `\match` commands and `\a` commands may appear in the match content.

##### An example

Consider the content `I have \\match{number?one}{ an apple }{number?z}{ no apples} }{}{ {number} apples }.`. Assuming that the variable `number` is the number `0`, or `1`, or `3` with the appropriate attributes, this content will produce, for each of these possibilities:
```
I have an apple.
I have no apples.
I have 3 apples.
```
Now, let us consider something a bit more complex: the content `I have \\match{number?one}{ {obj.ind} }{number?z}{ no {obj.many} } }{}{ {number} {obj.many} }.` And assume also that the following keys and content have been defined:
```
apple apple
apple.ind an apple
apple.many apples
lemon lemon
lemon.ind a lemon
lemon.many lemons
```
As before, let us consider what happens when the variable `number` is the number `0`, or `1`, or `3` with the appropriate attributes *and* under the assumption that the variable `obj` has been assigned to either the key `apple` or the key `lemon`. In those cases, the following text will be produced:
```
I have an apple.
I have no apples.
I have 3 apples.
I have a lemon.
I have no lemons.
I have 3 lemons.
```