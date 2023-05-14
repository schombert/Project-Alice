This is just a quick todo/note list for myself -breizh
GUI Todo:
- add "about me"s to the various tooltips where present
- Create Lists for Colonial Actions, Bankrupt Factories, Buildings be Constructed
- Factories are closed when their production scale is < 0.05
- Factories being built or upgraded = the nation has one or more state_building_construction

Abandon:
- top5 production and top3 import/export requires overlapping elements (see diplo war flags)

DONE:
- Add prototypes for monthly avg change for militancy, literacy, and conciousness   (they return monthly changes, we call them each time in the tooltips and divide ret'd value by 30
- add text::fp_four_places{} - Vic2 uses this many to represent TOPBAR_AVG_CHANGE
- Listing done for reforms, decisions

NOTE:
- I encourage you to do the simple thing when possible: write the function that you wish existed to make your life easier (like calculate_this_complicated_value) write the prototype, a mostly empty body with // TODO and a dummy return value, and then do some combination of telling me what you need it to do / write a comment (schombert)
- text::sequence_amp can have a substitution type of : std::string_view, dcon::text_key, dcon::province_id, dcon::state_instance_id, dcon::nation_id, dcon::national_identity_id, int64_t, fp_one_place, sys::date, std::monostate, fp_two_places, fp_three_places, fp_currency, pretty_integer, fp_percentage, int_percentage

Not Implemented but needs to be:
- Outliner  - Being done by leaf
- messagecat_* buttons

Finished:
- Mark anything 'placeholder' or 'fix me later' with a //TODO Comment
