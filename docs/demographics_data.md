## Demographics Data

Each pop stores a number of values about its preferred issues and ideologies. These are often referred to as the demographic data for the pop. These values, along with information about the prevalence of various cultures, religions, pop types, and employment are then aggregated on the province, state, and national level for easy reference.

Each source of demographics data is stored in the `demographics` property of the corresponding object. The demographics property is itself a floating point array, with each index into it corresponding to a different piece of demographics information. The functions for manipulating this information are part fof the `economy` namespace, mainly for lack of a better place to put them.
