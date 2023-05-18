# .GUI File Format Observed File Specifications

---

This is a simple in-progress documentation of the .gui file format used by Vic2, EU3, and Hoi3.
Everyone is welcome to contribute to this file as I (breizh) am not a professional when it comes to file formats let alone Vic2s.
It is written from the perspective of Vic2 files, and as a result may not be fully accurate for other Games, be that EU3 or Hoi3.

---

Positions and Sizes are relative to the parent (thanks leaf!)

---

# Types Used within the File
- string = a string is the rvalue (right hand value) of the key, this can be any ASCII String, it is surronded by a " on both sides
- bool = a boolean value, with its value being either `no` or `yes`
- int = can be any (presumably signed) integer
- float = can be any (signed) float
- vec2 = surronded by { ... } with space delimiated values within it, for example `position = { x= 2 y = 3}` as seen in interface/alerts.gui
- direction = is not surronded by a " and can be of a value of, `left`, `centre`, `right`
- section = this is simply a placeholder value the document uses to represent that a variable/property is in fact a section and not a actual property as described above, this starts with { and ends with } when the section ends
- action = appears it can represent some action can be substituted here, example = 'close_window

# Files judged
- airbuilder.gui        (empty)
- brigadebuilder.gui    (empty)
- divisiondesigner.gui  (empty)
- election.gui          (empty)
- shipbuilder.gui       (empty)
- alerts.gui
- rightclickmenu.gui
- tutorial_control.gui
- event_country_window.gui


# Entry Format
- For the sake of simplicity we list the sections and then their properties
- for each property a "tag" is placed after it on the right hand side, for example `(o|v|h|e)` with each of the letters representing one of the following:
- o = optional
- v = vic2        (this means it was *observed* in the vic2 files and does __not__ say its vic2 exclusive)
- h = hoi3        (similar to above)
- e = eu3         (similar to above)

## guiTypes section properties:
Appears to be able to contain:
- windowType = section
- positionType = section
- guiButtonType = section                   (o|v)

## windowType section properties:
Appears to be able to contain:
- name = string
- backGround = string                       (o|v)
- position = vec2
- size = vec2
- moveable = int (presumably to be 0 for unmoveable and 1 for moveable?)
- dontRender = string                       (o|v)
- horizontalBorder = string                 (o|v)
- verticalBorder = string                   (o|v)
- fullScreen = bool                         (o|v)
- upsound = string                          (o|v)
- downsound = string                        (o|v)
- orientation = string                      (o|v)       - Appears it may be Orientation aswell (tutorial_control.gui)
### Sections used in windowType
- iconType = section                        (o|v)
- listboxType = section                     (o|v)
- editBoxType = section                     (o|v)
- guiButtonType = section                   (o|v)
- instantTextBoxType = section              (o|v)
- OverlappingElementsBoxType = section      (o|v)
- windowType = section                      (o|v)

## iconType section properties:
Appears to be able to contain:
- name = string
- spriteType = string
- position = vec2
- Orientation = string
- rotation = float                          (o|v)

## positionType section properties:
Appears to be able to contain:
- name = string
- position = vec2

## listboxType section properties:
Appears to be able to contain:
- name = string
- position = vec2
- backGround = string
- size = vec2
- spacing = int
- scrollbartype = string
- borderSize = vec2
- Orientation = string

## editBoxType section properties:
Appears to be able to contain:
- position = vec2
- name = section
- textureFile = string
- font = string
- borderSize = vec2
- size = vec2
- text = string
- orientation = string

## guiButtonType section properties:
Appears to be able to contain:
- name = string
- position = vec2                           (o|v)
- quadTextureSprite = string
- tooltip = string                          (o|v)
- tooltipText = string                      (o|v)
- delayedTooltipText = string               (o|v)
- buttonText = string                       (o|v)
- buttonFont = string                       (o|v)
- Orientation = direction                   (o|v)
- clicksound = action                       (o|v)

## instantTextBoxType section properties:
Appears to be able to contain:
- name = string
- position = vec2
- format = direction
- font = string
- maxWidth = int
- maxHeight = int
- fixedsize = bool                          (o|v)
- Orientation = string                      (o|v)
- textureFile = string                      (o|v)
- borderSize = vec2                         (o|v)
- text = string                             (o|v)

## OverlappingElementsBoxType section properties:
Appears to be able to contain:
- name = string
- position = vec2
- size = vec2
- format = direction
- spacing = int
- Orientation = string

