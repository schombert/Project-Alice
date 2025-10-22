## This is a list of the diffrent datacontainer serialization tags, and an short explanation of their purposes

`save` : Properties&objects with this tag will be saved to the save file, and sent over the network to a new client in MP.

`scenario` : Properties&objects with this tag will be saved to the scenario file. This data should be contant during a game session and not change.

`local_save` : Properties&objects with this tag should only be locally accessible and not be saved either in the save file or being sent to a client in MP. This dosent currently work properly due to a incorrect "load_save" construct in dcon atm.

`keep_after_state_reload` : Properties&objects with this tag is excluded from being reset when a state reload is called (from a save reload in MP for example). This is typcially local or ui stuff, which dosent have any effect on the gamestate itself.

`mp_checksum_excluded` : Properties&objects with this tag is excluded from being included in the mp_state checksum, which is used for determining when a client is oos in MP. This is also typically local or ui stuff, but is not always the same properties as previous tag.

`mp_data` : Properties&objects with this tag is serialized and sent to the client ahead of the save when a new client joins, or to OOS'd client when a resync is initiated. This should be data which is important to send to the client, but dosen't make sense to store in the savegame proper.

Properties or objects without any tag will be included in the mp_state checksum, and will be reset when a state reset is called, but won't be saved to either a scenario or save file. Typically the fill_unsaved_values() function will fill these in after a state reload.
