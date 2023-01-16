#include "code_fragments.hpp"

void make_property_member_declarations(basic_builder& o, file_def const& parsed_file, relationship_object_def const& obj,
	property_def const& prop, bool add_prefix, std::string const& namesp, bool const_mode) {

	auto upresult = to_fat_index_type(parsed_file, prop.data_type, const_mode);

	o + substitute{ "type", upresult.has_value() ? *upresult : normalize_type(prop.data_type) };
	o + substitute{ "prop", prop.name };
	o + substitute{ "index_type", prop.array_index_type };
	o + substitute{ "namesp", namesp };
	o + substitute{ "vector_position", obj.is_expandable ? "ve::unaligned_contiguous_tags" : "ve::contiguous_tags" };

	if(add_prefix)
		o + heading{ "accessors for @obj@: @prop@" };

	if(prop.protection == protection_type::hidden && (add_prefix || o.declaration_mode)) {
		o + "private:";
	}

	if(prop.hook_get) { // HOOKED GETTERS
		switch(prop.type) {
			case property_type::bitfield:
				if(add_prefix) {
					o + "DCON_RELEASE_INLINE bool @obj@_get_@prop@(@obj@_id id) const;";
					if(prop.protection == protection_type::hidden) {
						o + "friend bool @obj@_fat_id::get_@prop@() cont;";
						o + "friend bool @obj@_const_fat_id::get_@prop@() const;";
					}

					o + "#ifndef DCON_NO_VE";
					o + "ve::vbitfield_type @obj@_get_@prop@(@vector_position@<@obj@_id> id) const" + block{
						o + "ve::vbitfield_type r; r.v = 0;";
						o + "for(int32_t i = 0; i < ve::vector_size; ++i)" + block{
							o + "tr.v |= (@obj@_get_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)))) << i;";
						};
						o + "return r;";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id) const" + block{
						o + "ve::vbitfield_type r; r.v = 0;";
						o + "for(int32_t i = 0; i < int32_t(id.subcount); ++i)" + block{
							o + "tr.v |= (@obj@_get_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)))) << i;";
						};
						o + "return r;";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@(ve::tagged_vector<@obj@ _id> id) const" + block{
						o + "ve::vbitfield_type r; r.v = 0;";
						o + "for(int32_t i = 0; i < ve::vector_size; ++i)" + block{
							o + "tr.v |= (@obj@_get_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)))) << i;";
						};
						o + "return r;";
					};
					o + "#endif";
				} else {
					o + "DCON_RELEASE_INLINE bool @namesp@get_@prop@() const" + block{
						o + "return container.@obj@_get_@prop@(id);";
					};
				}
				break;
			case property_type::vectorizable:
				if(add_prefix) {
					o + "@type@ @obj@_get_@prop@(@obj@_id id) const;";
					if(prop.protection == protection_type::hidden) {
						o + "friend @type@ @obj@_fat_id::get_@prop@() const;";
						o + "friend @type@ @obj@_const_fat_id::get_@prop@() const;";
					}

					o + "#ifndef DCON_NO_VE";
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@(@vector_position@<@obj@_id> id) const" + block{
						o + "return ve::apply([t = this](@obj@_id i){ return t->@obj@_get_@prop@(i); }, id);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id) const" + block{
						o + "return ve::apply([t = this](@obj@_id i){ return t->@obj@_get_@prop@(i); }, id);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::tagged_vector<@obj@_id> id) const" + block{
						o + "return ve::apply([t = this](@obj@_id i){ return t->@obj@_get_@prop@(i); }, id);";
					};
					o + "#endif";
				} else {
					if(upresult.has_value())
						o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@() const" + block{
							o + "return @type@(container, container.@obj@_get_@prop@(id));";
						};
					else
						o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@() const" + block{ 
							o + "return container.@obj@_get_@prop@(id);";
						};
					break;
				}
				break;
			case property_type::other:
				if(add_prefix) {
					o + "@type@ @obj@_get_@prop@(@obj@_id id) const;";
					if(prop.protection == protection_type::hidden) {
						o + "friend @type@ @obj@_fat_id::get_@prop@() const;";
						o + "friend @type@ @obj@_const_fat_id::get_@prop@() const;";
					}
				} else {
					o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@() const" + block{
						o + "return container.@obj@_get_@prop@(id);";
					};
				}
				break;
			case property_type::object:
				if(add_prefix) {
					o + "@type@ @obj@_get_@prop@(@obj@_id id) const;";
					if(prop.protection == protection_type::hidden) {
						o + "friend @type@ @obj@_fat_id::get_@prop@() const;";
						o + "friend @type@ @obj@_const_fat_id::get_@prop@() const;";
					}
				} else {
					o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@() const" + block {
						o + "return container.@obj@_get_@prop@(id);";
					};
				}
				break;
			case property_type::array_bitfield:
				if(add_prefix) {
					o + "bool @obj@_get_@prop@(@obj@_id id, @index_type@ i) const;";
					o + "uint32_t @obj@_get_@prop@_size() const;";

					if(prop.protection == protection_type::hidden) {
						o + "friend bool @obj@_fat_id::get_@prop@(@index_type@) const;";
						o + "friend bool @obj@_const_fat_id::get_@prop@(@index_type@) const;";
						o + "friend uint32_t @obj@_fat_id::get_@prop@_size() const;";
						o + "friend uint32_t @obj@_const_fat_id::get_@prop@_size() const;";
					}

					o + "#ifndef DCON_NO_VE";
					o + "ve::vbitfield_type @obj@_get_@prop@(@vector_position@<@obj@_id> id, @index_type@ n) const" + block{
						o + "ve::vbitfield_type r; r.v = 0;";
						o + "for(int32_t i = 0; i < ve::vector_size; ++i)" + block{
							o + "tr.v |= (@obj@_get_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)), n)) << i;";
						};
						o + "return r;";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id, @index_type@ n) const" + block{
						o + "ve::vbitfield_type r; r.v = 0;";
						o + "for(int32_t i = 0; i < int32_t(id.subcount); ++i)" + block{
							o + "tr.v |= (@obj@_get_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)), n)) << i;";
						};
						o + "return r;";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@(ve::tagged_vector<@obj@ _id> id, @index_type@ n) const" + block{
						o + "ve::vbitfield_type r; r.v = 0;";
						o + "for(int32_t i = 0; i < ve::vector_size; ++i)" + block{
							o + "tr.v |= (@obj@_get_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)), n)) << i;";
						};
						o + "return r;";
					};
					o + "#endif";
				} else {
					o + "DCON_RELEASE_INLINE bool @namesp@get_@prop@(@index_type@ i) const" + block {
						o + "return container.@obj@_get_@prop@(id, i);";
					};
					o + "DCON_RELEASE_INLINE uint32_t @namesp@get_@prop@_size() const noexcept" + block{
						o + "return container.@obj@_get_@prop@_size();";
					};
				}
				break;
			case property_type::array_other:
				if(add_prefix) {
					o + "@type@ @obj@_get_@prop@(@obj@_id id, @index_type@ n) const;";
					o + "uint32_t @obj@_get_@prop@_size() const;";

					if(prop.protection == protection_type::hidden) {
						o + "friend @type@ @obj@_fat_id::get_@prop@(@index_type@) const;";
						o + "friend @type@ @obj@_const_fat_id::get_@prop@(@index_type@) const;";
						o + "friend uint32_t @obj@_fat_id::get_@prop@_size() const;";
						o + "friend uint32_t @obj@_const_fat_id::get_@prop@_size() const;";
					}
				} else {
					o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@(@index_type@ i) const" + block {
						o + "return container.@obj@_get_@prop@(id, i);";
					};
					o + "DCON_RELEASE_INLINE uint32_t @namesp@get_@prop@_size() const noexcept" + block{
						o + "return container.@obj@_get_@prop@_size();";
					};
				}
				break;
			case property_type::array_vectorizable:
				if(add_prefix) {
					o + "@type@ @obj@_get_@prop@(@obj@_id id, @index_type@ n) const;";
					o + "uint32_t @obj@_get_@prop@_size() const;";

					if(prop.protection == protection_type::hidden) {
						o + "friend @type@ @obj@_fat_id::get_@prop@(@index_type@) const;";
						o + "friend @type@ @obj@_const_fat_id::get_@prop@(@index_type@) const;";
						o + "friend uint32_t @obj@_fat_id::get_@prop@_size() const;";
						o + "friend uint32_t @obj@_const_fat_id::get_@prop@_size() const;";
					}

					o + "#ifndef DCON_NO_VE";
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@(@vector_position@<@obj@_id> id, @index_type@ n) const" + block{
						o + "return ve::apply([t = this](@obj@_id i){ return t->@obj@_get_@prop@(i, n); }, id);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id, @index_type@ n) const" + block{
						o + "return ve::apply([t = this](@obj@_id i){ return t->@obj@_get_@prop@(i, n); }, id);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::tagged_vector<@obj@_id> id, @index_type@ n) const" + block{
						o + "return ve::apply([t = this](@obj@_id i){ return t->@obj@_get_@prop@(i, n); }, id);";
					};
					o + "#endif";
				} else {
					if(upresult.has_value())
						o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@(@index_type@ i) const" + block {
							o + "return @type@(container, container.@obj@_get_@prop@(id, i));"; 
						};
					else
						o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@(@index_type@ i) const" + block {
							o + "return container.@obj@_get_@prop@(id, i);";
						};
					o + "DCON_RELEASE_INLINE uint32_t @namesp@get_@prop@_size() const noexcept" + block{
						o + "return container.@obj@_get_@prop@_size();";
					};
				}
				break;
			default:
				std::abort(); // should never get here
				break;
		}
	} else if(!prop.is_derived) { // ORDINARY GETTERS
		switch(prop.type) {
			case property_type::bitfield:
				if(add_prefix) {
					o + "DCON_RELEASE_INLINE bool @obj@_get_@prop@(@obj@_id id) const noexcept" + block{
						o + "return dcon::bit_vector_test(@obj@.m_@prop@.vptr(), id.index());";
					};
					if(prop.protection == protection_type::hidden) {
						o + "friend bool @obj@_fat_id::get_@prop@() const noexcept;";
						o + "friend bool @obj@_const_fat_id::get_@prop@() const noexcept;";
					}

					o + "#ifndef DCON_NO_VE";
					o + "DCON_RELEASE_INLINE ve::vbitfield_type @obj@_get_@prop@(@vector_position@<@obj@_id> id) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr());";
					};
					o + "DCON_RELEASE_INLINE ve::vbitfield_type @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr());";
					};
					o + "DCON_RELEASE_INLINE ve::vbitfield_type @obj@_get_@prop@(ve::tagged_vector<@obj@_id> id) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr());";
					};
					o + "#endif";
				} else {
					o + "DCON_RELEASE_INLINE bool @namesp@get_@prop@() const noexcept" + block{
						o + "return container.@obj@_get_@prop@(id);";
					};
				}
				break;
			case property_type::vectorizable:
				if(add_prefix) {
					o + "DCON_RELEASE_INLINE @type@ const& @obj@_get_@prop@(@obj@_id id) const noexcept" + block{
						o + "return @obj@.m_@prop@.vptr()[id.index()];";
					};
					if(prop.protection != protection_type::read_only && !prop.hook_set) {
						o + "DCON_RELEASE_INLINE @type@& @obj@_get_@prop@(@obj@_id id) noexcept" + block{
							o + "return @obj@.m_@prop@.vptr()[id.index()];";
						};
					}

					if(prop.protection == protection_type::hidden) {
						if(upresult.has_value()) {
							o + "friend @type@ @obj@_fat_id::get_@prop@() const noexcept;";
							o + "friend @type@ @obj@_const_fat_id::get_@prop@() const noexcept;";
						} else {
							o + "friend @type@& @obj@_fat_id::get_@prop@() const noexcept;";
							o + "friend @type@ @obj@_const_fat_id::get_@prop@() const noexcept;";
						}
					}

					o + "#ifndef DCON_NO_VE";
					o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(@vector_position@<@obj@_id> id) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr());";
					};
					o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr());";
					};
					o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::tagged_vector<@obj@_id> id) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr());";
					};
					o + "#endif";
				} else {
					if(upresult.has_value())
						o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@() const noexcept" + block{
							o + "return @type@(container, container.@obj@_get_@prop@(id));";
						};
					else if(const_mode || prop.protection == protection_type::read_only || prop.hook_set)
						o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@() const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id);";
						};
					else
						o + "DCON_RELEASE_INLINE @type@& @namesp@get_@prop@() const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id);";
						};
				}
				break;
			case property_type::other:
				if(add_prefix) {
					o + "DCON_RELEASE_INLINE @type@ const& @obj@_get_@prop@(@obj@_id id) const noexcept" + block{
						o + "return @obj@.m_@prop@.vptr()[id.index()];";
					};
					if(prop.protection != protection_type::read_only && !prop.hook_set) {
						o + "DCON_RELEASE_INLINE @type@& @obj@_get_@prop@(@obj@_id id) noexcept" + block{
							o + "return @obj@.m_@prop@.vptr()[id.index()];";
						};
					}

					if(prop.protection == protection_type::hidden) {
						o + "friend @type@& @obj@_fat_id::get_@prop@() const noexcept;";
						o + "friend @type@ const& @obj@_const_fat_id::get_@prop@() const noexcept;";
					}

				} else {
					if(const_mode || prop.protection == protection_type::read_only || prop.hook_set) {
						o + "DCON_RELEASE_INLINE @type@ const& @namesp@get_@prop@() const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id);";
						};
					} else {
						o + "DCON_RELEASE_INLINE @type@& @namesp@get_@prop@() const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id);";
						};
					}
				}
				break;
			case property_type::object:
				if(add_prefix) {
					o + "DCON_RELEASE_INLINE @type@ const& @obj@_get_@prop@(@obj@_id id) const noexcept" + block{
						o + "return @obj@.m_@prop@.vptr()[id.index()];";
					};
					if(prop.protection != protection_type::read_only && !prop.hook_set) {
						o + "DCON_RELEASE_INLINE @type@& @obj@_get_@prop@(@obj@_id id) noexcept" + block{
							o + "return @obj@.m_@prop@.vptr()[id.index()];";
						};
					}
					if(prop.protection == protection_type::hidden) {
						o + "friend @obj@_fat_id::get_@prop@() const noexcept;";
						o + "friend @obj@_const_fat_id::get_@prop@() const noexcept;";
					}
				} else {
					if(const_mode || prop.protection == protection_type::read_only || prop.hook_set)
						o + "DCON_RELEASE_INLINE @type@ const& @namesp@get_@prop@() const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id);";
						};
					else
						o + "DCON_RELEASE_INLINE @type@& @namesp@get_@prop@() const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id);";
						};
				}
				break;
			case property_type::array_bitfield:
				if(add_prefix) {
					o + "DCON_RELEASE_INLINE bool @obj@_get_@prop@(@obj@_id id, @index_type@ n) const noexcept" + block{
						o + "return dcon::bit_vector_test(@obj@.m_@prop@.vptr(dcon::get_index(n)), id.index());";
					};
					o + "DCON_RELEASE_INLINE uint32_t @obj@_get_@prop@_size() const noexcept" + block{
						o + "return @obj@.m_@prop@.size;";
					};

					if(prop.protection == protection_type::hidden) {
						o + "friend bool @obj@_fat_id::get_@prop@( @index_type@ ) const noexcept;";
						o + "friend bool @obj@_const_fat_id::get_@prop@( @index_type@) const noexcept;";
						o + "friend uint32_t @obj@_fat_id::get_@prop@_size( ) const noexcept;";
						o + "friend uint32_t @obj@_const_fat_id::get_@prop@_size( ) const noexcept;";
					}

					o + "#ifndef DCON_NO_VE";
					o + "DCON_RELEASE_INLINE ve::vbitfield_type @obj@_get_@prop@(@vector_position@<@obj@_id> id, @index_type@ n) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr(dcon::get_index(n)));";
					};
					o + "DCON_RELEASE_INLINE ve::vbitfield_type @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id, @index_type@ n) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr(dcon::get_index(n)));";
					};
					o + "DCON_RELEASE_INLINE ve::vbitfield_type @obj@_get_@prop@(ve::tagged_vector<@obj@_id> id, @index_type@ n) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr(dcon::get_index(n)));";
					};
					o + "#endif";
				} else {
					o + "DCON_RELEASE_INLINE bool @namesp@get_@prop@(@index_type@ i) const noexcept" + block {
						o + "return container.@obj@_get_@prop@(id, i);";
					};
					o + "DCON_RELEASE_INLINE uint32_t @namesp@get_@prop@_size() const noexcept" + block {
						o + "return container.@obj@_get_@prop@_size();";
					};
				}
				break;
			case property_type::array_other:
				if(add_prefix) {
					o + "DCON_RELEASE_INLINE @type@ const& @obj@_get_@prop@(@obj@_id id, @index_type@ n) const noexcept" + block{
						o + "return @obj@.m_@prop@.vptr(dcon::get_index(n))[id.index()];";
					};
					if(prop.protection != protection_type::read_only && !prop.hook_set) {
						o + "DCON_RELEASE_INLINE @type@& @obj@_get_@prop@(@obj@_id id, @index_type@ n) noexcept" + block{
							o + "return @obj@.m_@prop@.vptr(dcon::get_index(n))[id.index()];";
						};
					}
					o + "DCON_RELEASE_INLINE uint32_t @obj@_get_@prop@_size() const noexcept" + block{
						o + "return @obj@.m_@prop@.size;";
					};

					if(prop.protection == protection_type::hidden) {
						o + "friend @type@& @obj@_fat_id::get_@prop@( @index_type@ ) const noexcept;";
						o + "friend @type@ const& @obj@_const_fat_id::get_@prop@( @index_type@) const noexcept;";
						o + "friend uint32_t @obj@_fat_id::get_@prop@_size( ) const noexcept;";
						o + "friend uint32_t @obj@_const_fat_id::get_@prop@_size( ) const noexcept;";
					}
				} else {
					if(const_mode || prop.protection == protection_type::read_only || prop.hook_set)
						o + "DCON_RELEASE_INLINE @type@ const& @namesp@get_@prop@(@index_type@ i) const noexcept" + block {
							o + "return container.@obj@_get_@prop@(id, i);";
						};
					else
						o + "DCON_RELEASE_INLINE @type@& @namesp@get_@prop@(@index_type@ i) const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id, i);";
						};
					o + "DCON_RELEASE_INLINE uint32_t @namesp@get_@prop@_size() const noexcept" + block {
						o + "return container.@obj@_get_@prop@_size();";
					};
				}
				break;
			case property_type::array_vectorizable:
				if(add_prefix) {
					o + "DCON_RELEASE_INLINE @type@ const& @obj@_get_@prop@(@obj@_id id, @index_type@ n) const noexcept" + block{
						o + "return @obj@.m_@prop@.vptr(dcon::get_index(n))[id.index()];";
					};
					if(prop.protection != protection_type::read_only && !prop.hook_set) {
						o + "DCON_RELEASE_INLINE @type@& @obj@_get_@prop@(@obj@_id id, @index_type@ n) noexcept" + block{
							o + "return @obj@.m_@prop@.vptr(dcon::get_index(n))[id.index()];";
						};
					}
					o + "DCON_RELEASE_INLINE uint32_t @obj@_get_@prop@_size() const noexcept" + block{
						o + "return @obj@.m_@prop@.size;";
					};

					if(prop.protection == protection_type::hidden) {
						if(upresult.has_value()) {
							o + "friend @type@ @obj@_fat_id::get_@prop@( @index_type@ ) const noexcept;";
							o + "friend @type@ @obj@_const_fat_id::get_@prop@( @index_type@) const noexcept;";
						} else {
							o + "friend @type@& @obj@_fat_id::get_@prop@( @index_type@ ) const noexcept;";
							o + "friend @type@ @obj@_const_fat_id::get_@prop@( @index_type@) const noexcept;";
						}
						o + "friend uint32_t @obj@_fat_id::get_@prop@_size( ) const noexcept;";
						o + "friend uint32_t @obj@_const_fat_id::get_@prop@_size( ) const noexcept;";
					}


					o + "#ifndef DCON_NO_VE";
					o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(@vector_position@<@obj@_id> id, @index_type@ n) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr(dcon::get_index(n)));";
					};
					o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id, @index_type@ n) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr(dcon::get_index(n)));";
					};
					o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::tagged_vector<@obj@_id> id, @index_type@ n) const noexcept" + block{
						o + "return ve::load(id, @obj@.m_@prop@.vptr(dcon::get_index(n)));";
					};
					o + "#endif";
				} else {
					if(upresult.has_value())
						o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@(@index_type@ i) const noexcept" + block{
							o + "return @type@(container, container.@obj@_get_@prop@(id, i));";
						};
					else if(const_mode || prop.protection == protection_type::read_only || prop.hook_set)
						o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@(@index_type@ i) const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id, i);";
						};
					else
						o + "DCON_RELEASE_INLINE @type@& @namesp@get_@prop@(@index_type@ i) const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id, i);";
						};

					o + "DCON_RELEASE_INLINE uint32_t @namesp@get_@prop@_size() const noexcept" + block {
						o + "return container.@obj@_get_@prop@_size();";
					};
				}
				break;
			case property_type::special_vector:
				if(add_prefix) {
					o + "DCON_RELEASE_INLINE dcon::dcon_vv_const_fat_id<@type@> @obj@_get_@prop@(@obj@_id id) const noexcept" + block{
						o + "return dcon::dcon_vv_const_fat_id<@type@>(@obj@.@prop@_storage, @obj@.m_@prop@.vptr()[id.index()]);";
					};
					if(prop.protection != protection_type::read_only && !prop.hook_set) {
						o + "DCON_RELEASE_INLINE dcon::dcon_vv_fat_id<@type@> @obj@_get_@prop@(@obj@_id id) noexcept" + block{
							o + "return dcon::dcon_vv_fat_id<@type@>(@obj@.@prop@_storage, @obj@.m_@prop@.vptr()[id.index()]);";
						};
					}

					if(prop.protection == protection_type::hidden) {
						o + "friend dcon::dcon_vv_fat_id<@type@> @obj@_fat_id::get_@prop@() const noexcept;";
						o + "friend dcon::dcon_vv_const_fat_id<@type@> @obj@_const_fat_id::get_@prop@() const noexcept;";
					}

				} else {
					if(const_mode || prop.protection == protection_type::read_only || prop.hook_set) {
						o + "DCON_RELEASE_INLINE dcon::dcon_vv_const_fat_id<@type@> @namesp@get_@prop@() const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id);";
						};
					} else {
						o + "DCON_RELEASE_INLINE dcon::dcon_vv_fat_id<@type@> @namesp@get_@prop@() const noexcept" + block{
							o + "return container.@obj@_get_@prop@(id);";
						};
					}
				}
				break;
			default:
				std::abort(); // should never get here
				break;
		}
	}

	if(prop.protection == protection_type::hidden && (add_prefix || o.declaration_mode)) {
		o + "public:";
	}

	if((add_prefix || o.declaration_mode) && (prop.protection == protection_type::read_only || prop.protection == protection_type::hidden)) {
		o + "private:";
	}

	if(!const_mode) {
		if(prop.hook_set) {
			switch(prop.type) {  // HOOKED SETTERS
				case property_type::bitfield:
					if(add_prefix) {
						o + "void @obj@_set_@prop@(@obj@_id id, bool value);";

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden)
							o + "friend void @obj@_fat_id::set_@prop@bool) const;";

						o + "#ifndef DCON_NO_VE";
						o + "void @obj@_set_@prop@(@vector_position@<@obj@_id> id, ve::vbitfield_type value)" + block{
							o + "for(int32_t i = 0; i < ve::vector_size; ++i)" + block{
								o + "@obj@_set_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)), (value.v & uint8_t(1 << i)) != 0);";
							};
						};
						o + "void @obj@_set_@prop@(ve::partial_contiguous_tags<@obj@_id> id, ve::vbitfield_type value)" + block{
							o + "for(int32_t i = 0; i < int32_t(id.subcount); ++i)" + block{
								o + "@obj@_set_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)), (value.v & uint8_t(1 << i)) != 0);";
							};
						};
						o + "void @obj@_set_@prop@(ve::tagged_vector<@obj@_id> id, ve::vbitfield_type value)" + block{
							o + "for(int32_t i = 0; i < ve::vector_size; ++i)" + block{
								o + "@obj@_set_@prop@(id[i], (value.v & uint8_t(1 << i)) != 0);";
							};
						};
						o + "#endif";
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(bool v) const" + block{
							o + "container.@obj@_set_@prop@(id, v);";
						};
					}
					break;
				case property_type::vectorizable:
					if(add_prefix) {
						o + "void @obj@_set_@prop@(@obj@_id id, @type@ value);";

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden)
							o + "friend void @obj@_fat_id::set_@prop@(@type@) const;";

						o + "#ifndef DCON_NO_VE";
						o + "void @obj@_set_@prop@(@vector_position@<@obj@_id> id, ve::value_to_vector_type<@type@> value)" + block{
							o + "ve::apply(value, [t = this](@obj@_id i, @type@ v){ t->@obj@_set_@prop@(i, v); }, id);";
						};
						o + "void @obj@_set_@prop@(ve::partial_contiguous_tags<@obj@_id> id, ve::value_to_vector_type<@type@> value)" + block{
							o + "ve::apply(value, [t = this](@obj@_id i, @type@ v){ t->@obj@_set_@prop@(i, v); }, id);";
						};
						o + "void @obj@_set_@prop@(ve::tagged_vector<@obj@_id> id, ve::value_to_vector_type<@type@> value)" + block{
							o + "ve::apply(value, [t = this](@obj@_id i, @type@ v){ t->@obj@_set_@prop@(i, v); }, id);";
						};
						o + "#endif";
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@type@ v) const" + block{
							o + "container.@obj@_set_@prop@(id, v);";
						};
					}
					break;
				case property_type::other:
					if(add_prefix) {
						o + "void @obj@_set_@prop@(@obj@_id id, @type@ const& value);";

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden)
							o + "friend void @obj@_fat_id::set_@prop@(@type@ const&) const;";

					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@type@ const& v) const" + block{
							o + "container.@obj@_set_@prop@(id, v);";
						};
					}
					break;
				case property_type::object:
					if(add_prefix) {
						o + "void @obj@_set_@prop@(@obj@_id id, @type@ const& value);";

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden)
							o + "friend void @obj@_fat_id::set_@prop@(@type@ const&) const;";
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@type@ const& v) const" + block{
							o + "container.@obj@_set_@prop@(id, v);";
						};
					}
					break;
				case property_type::array_bitfield:
					if(add_prefix) {
						o + "void @obj@_set_@prop@(@obj@_id id, @index_type@ n, bool value);";
						o + "void @namesp@resize_@prop@(uint32_t sz) const;";

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(@index_type@, bool) const;";
							o + "friend void @obj@_fat_id::resize_@prop@(uint32_t) const noexcept;";
						}

						o + "#ifndef DCON_NO_VE";
						o + "void @obj@_set_@prop@(@vector_position@<@obj@_id> id, @index_type@ n, ve::vbitfield_type value)" + block{
							o + "for(int32_t i = 0; i < ve::vector_size; ++i)" + block{
								o + "@obj@_set_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)), n, (value.v & uint8_t(1 << i)) != 0);";
							};
						};
						o + "void @obj@_set_@prop@(ve::partial_contiguous_tags<@obj@_id> id, @index_type@ n, ve::vbitfield_type value)" + block{
							o + "for(int32_t i = 0; i < int32_t(id.subcount); ++i)" + block{
								o + "@obj@_set_@prop@(@obj@_id(@obj@_id::value_base_t(id.value + i)), n, (value.v & uint8_t(1 << i)) != 0);";
							};
						};
						o + "void @obj@_set_@prop@(ve::tagged_vector<<@obj@_id> id, @index_type@ n, ve::vbitfield_type value)" + block{
							o + "for(int32_t i = 0; i < ve::vector_size; ++i)" + block{
								o + "@obj@_set_@prop@(id[i], n, (value.v & uint8_t(1 << i)) != 0);";
							};
						};
						o + "#endif";
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@index_type@ i, bool v) const" + block{
							"container.@obj@_set_@prop@(id, i, v);";
						};
						o + "DCON_RELEASE_INLINE void @namesp@resize_@prop@(uint32_t sz) const noexcept" + block{
							o + "container.@obj@_resize_@prop@(sz);";
						};
					}
					break;
				case property_type::array_other:
					if(add_prefix) {
						o + "void @obj@_set_@prop@(@obj@_id id, @index_type@ n, @type@ const& value);";
						o + "void @namesp@resize_@prop@(uint32_t sz) const;";

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(@index_type@, @type@ const&) const;";
							o + "friend void @obj@_fat_id::resize_@prop@(uint32_t) const noexcept;";
						}
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@index_type@ i, @type@ const& v) const" + block{
							o + "container.@obj@_set_@prop@(id, i, v);";
						};
						o + "DCON_RELEASE_INLINE void @namesp@resize_@prop@(uint32_t sz) const noexcept" + block{
							o + "container.@obj@_resize_@prop@(sz);";
						};
					}
					break;
				case property_type::array_vectorizable:
					if(add_prefix) {
						o + "void @obj@_set_@prop@(@obj@_id id, @index_type@ n, @type@ value);";
						o + "void @namesp@resize_@prop@(uint32_t sz) const;";

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(@index_type@, @type@) const;";
							o + "friend void @obj@_fat_id::resize_@prop@(uint32_t) const noexcept;";
						}

						o + "#ifndef DCON_NO_VE";
						o + "void @obj@_set_@prop@(@vector_position@<@obj@_id> id, @index_type@ n, ve::value_to_vector_type<@type@> value)" + block{
							o + "ve::apply(value, [t = this, n](@obj@_id i, @type@ v){ t->@obj@_set_@prop@(i, n, v); }, id);";
						};
						o + "void @obj@_set_@prop@(ve::partial_contiguous_tags<@obj@_id> id, @index_type@ n, ve::value_to_vector_type<@type@> value)" + block{
							o + "ve::apply(value, [t = this, n](@obj@_id i, @type@ v){ t->@obj@_set_@prop@(i, n, v); }, id);";
						};
						o + "void @obj@_set_@prop@(ve::tagged_vector<@obj@_id> id, @index_type@ n, ve::value_to_vector_type<@type@> value)" + block{
							o + "ve::apply(value, [t = this, n](@obj@_id i, @type@ v){ t->@obj@_set_@prop@(i, n, v); }, id);";
						};
						o + "#endif";
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@index_type@ i, @type@ v) const" + block{
							o + "container.@obj@_set_@prop@(id, i, v);";
						};
						o + "DCON_RELEASE_INLINE void @namesp@resize_@prop@(uint32_t sz) const noexcept" + block{
							o + "container.@obj@_resize_@prop@(sz);";
						};
					}
					break;
				default:
					std::abort(); // should never get here
					break;
			}
		} else if(!prop.is_derived) { // ORDINARY SETTERS
			switch(prop.type) {
				case property_type::bitfield:
					if(add_prefix) {
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@obj@_id id, bool value) noexcept" + block{
							o + "dcon::bit_vector_set(@obj@.m_@prop@.vptr(), id.index(), value);";
						};

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(bool) const noexcept;";
						}

						o + "#ifndef DCON_NO_VE";
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@vector_position@<@obj@_id> id, ve::vbitfield_type values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(), values);";
						};
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::partial_contiguous_tags<@obj@_id> id, ve::vbitfield_type values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(), values);";
						};
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::tagged_vector<@obj@_id> id, ve::vbitfield_type values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(), values);";
						};
						o + "#endif";
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(bool v) const noexcept" + block{
							o + "container.@obj@_set_@prop@(id, v);";
						};
					}
					break;
				case property_type::vectorizable:
					if(add_prefix) {
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
							o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
						};

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(@type@) const noexcept;";
						}

						o + "#ifndef DCON_NO_VE";
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@vector_position@<@obj@_id> id, ve::value_to_vector_type<@type@> values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(), values);";
						};
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::partial_contiguous_tags<@obj@_id> id, ve::value_to_vector_type<@type@> values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(), values);";
						};
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::tagged_vector<@obj@_id> id, ve::value_to_vector_type<@type@> values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(), values);";
						};
						o + "#endif";
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@type@ v) const noexcept" + block{
							o + "container.@obj@_set_@prop@(id, v);";
						};
					}
					break;
				case property_type::other:
					if(add_prefix) {
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@obj@_id id, @type@ const& value) noexcept" + block{
							o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
						};

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(@type@ const&) const noexcept;";
						}
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@type@ const& v) const noexcept" + block{
							o + "container.@obj@_set_@prop@(id, v);";
						};
					}
					break;
				case property_type::object:
					if(add_prefix) {
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@obj@_id id, @type@ const& value) noexcept" + block{
							o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
						};

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(@type@ const&) const noexcept;";
						}
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@type@ const& v) const noexcept" + block{
							o + "container.@obj@_set_@prop@(id, v);";
						};
					}
					break;
				case property_type::array_bitfield:
					if(add_prefix) {
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@obj@_id id, @index_type@ n, bool value) noexcept" + block{
							o + "dcon::bit_vector_set(@obj@.m_@prop@.vptr(dcon::get_index(n)), id.index(), value);";
						};
						o + "DCON_RELEASE_INLINE void @obj@_resize_@prop@(uint32_t size) noexcept" + block{
							o + "@obj@.m_@prop@.resize(size, @obj@.size_used);";
						};

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(@index_type@, bool) const noexcept;";
							o + "friend void @obj@_fat_id::@prop@_resize(uint32_t) const noexcept;";
						}

						o + "#ifndef DCON_NO_VE";
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@vector_position@<@obj@_id> id, @index_type@ n, ve::vbitfield_type values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(dcon::get_index(n)), values);";
						};
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::partial_contiguous_tags<@obj@_id> id, @index_type@ n, ve::vbitfield_type values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(dcon::get_index(n)), values);";
						};
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::tagged_vector<@obj@_id> id, @index_type@ n, ve::vbitfield_type values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(dcon::get_index(n)), values);";
						};
						o + "#endif";
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@index_type@ i, bool v) const noexcept" + block{
							o + "container.@obj@_set_@prop@(id, i, v);";
						};
						o + "DCON_RELEASE_INLINE void @namesp@resize_@prop@(uint32_t sz) const noexcept" + block {
							o + "container.@obj@_resize_@prop@(sz);";
						};
					}
					break;
				case property_type::array_other:
					if(add_prefix) {
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@obj@_id id, @index_type@ n, @type@ const& value) noexcept" + block{
							o + "@obj@.m_@prop@.vptr(dcon::get_index(n))[id.index()] = value;";
						};
						o + "DCON_RELEASE_INLINE void @obj@_resize_@prop@(uint32_t size) noexcept" + block{
							o + "return @obj@.m_@prop@.resize(size, @obj@.size_used);";
						};

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(@index_type@, @type@ const&) const noexcept;";
							o + "friend void @obj@_fat_id::@prop@_resize(uint32_t) const noexcept;";
						}
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@index_type@ i, @type@ const& v) const noexcept" + block {
							o + "container.@obj@_set_@prop@(id, i, v);"; 
						};
						o + "DCON_RELEASE_INLINE void @namesp@resize_@prop@(uint32_t sz) const noexcept" + block{
							o + "container.@obj@_resize_@prop@(sz);";
						};
					}
					break;
				case property_type::array_vectorizable:
					if(add_prefix) {
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@obj@_id id, @index_type@ n, @type@ value) noexcept" + block{
							o + "@obj@.m_@prop@.vptr(dcon::get_index(n))[id.index()] = value;";
						};
						o + "DCON_RELEASE_INLINE void @obj@_resize_@prop@(uint32_t size) noexcept" + block{
							o + "return @obj@.m_@prop@.resize(size, @obj@.size_used);";
						};

						if(prop.protection == protection_type::read_only || prop.protection == protection_type::hidden) {
							o + "friend void @obj@_fat_id::set_@prop@(@index_type@, @type@) const noexcept;";
							o + "friend void @obj@_fat_id::@prop@_resize(uint32_t) const noexcept;";
						}

						o + "#ifndef DCON_NO_VE";
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@vector_position@<@obj@_id> id, @index_type@ n, ve::value_to_vector_type<@type@> values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(dcon::get_index(n)), values);";
						};
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::partial_contiguous_tags<@obj@_id> id, @index_type@ n, ve::value_to_vector_type<@type@> values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(dcon::get_index(n)), values);";
						};
						o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::tagged_vector<@obj@_id> id, @index_type@ n, ve::value_to_vector_type<@type@> values) noexcept" + block{
							o + "ve::store(id, @obj@.m_@prop@.vptr(dcon::get_index(n)), values);";
						};
						o + "#endif";
					} else {
						o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@index_type@ i, @type@ v) const noexcept" + block{
							o + "container.@obj@_set_@prop@(id, i, v);"; 
						};
						o + "DCON_RELEASE_INLINE void @namesp@resize_@prop@(uint32_t sz) const noexcept" + block{
							o + "container.@obj@_resize_@prop@(sz);";
						};
					}
					break;
				case property_type::special_vector:
					
					break;
				default:
					std::abort(); // should never get here
					break;
			}
		}
	}
	if((add_prefix || o.declaration_mode) && (prop.protection == protection_type::read_only || prop.protection == protection_type::hidden)) {
		o + "public:";
	}
}

void make_delete_current_composite_keys_text(basic_builder& o, std::string const& target_id, relationship_object_def const& obj, related_object const& i) {
	o + substitute{ "target_id", target_id };
	if(i.is_covered_by_composite_key) {
		for(auto& cc : obj.composite_indexes) {
			std::string params;
			bool needs_adjust = false;

			for(auto& idx : cc.component_indexes) {
				if(idx.property_name == i.property_name) {
					needs_adjust = true;
				}
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == obj.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(" + target_id + ".index()))";
				else
					params += obj.name + ".m_" + idx.property_name + ".vptr()[" + target_id + ".index()]";
			}
			if(needs_adjust) {
				o + substitute{ "params", params } +substitute{ "ckname", cc.name };
				o + "@obj@.hashm_@ckname@.erase( @obj@.to_@ckname@_keydata(@params@) );";
			}
		}
	}
}

void make_try_current_composite_keys_text(basic_builder& o, std::string const& target_id, relationship_object_def const& obj, related_object const& i) {
	o + substitute{ "target_id", target_id };
	if(i.is_covered_by_composite_key) {
		for(auto& cc : obj.composite_indexes) {
			std::string params;
			bool needs_adjust = false;

			for(auto& idx : cc.component_indexes) {
				if(idx.property_name == i.property_name) {
					needs_adjust = true;
				}
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == obj.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(" + target_id + ".index()))";
				else if(idx.property_name == i.property_name)
					params += "value";
				else
					params += obj.name + ".m_" + idx.property_name + ".vptr()[" + target_id + ".index()]";
			}
			if(needs_adjust) {
				o + substitute{ "params", params } +substitute{ "ckname", cc.name };
				o + "if(@obj@.hashm_@ckname@.find( @obj@.to_@ckname@_keydata(@params@) ) != @obj@.hashm_@ckname@.end()) return false;";
			}
		}
	}
}

void make_try_current_composite_keys_text_with_multiplicity(basic_builder& o, std::string const& target_id, relationship_object_def const& obj, related_object const& i) {
	o + substitute{ "target_id", target_id };
	if(i.is_covered_by_composite_key) {
		for(auto& cc : obj.composite_indexes) {
			std::string params;
			bool needs_adjust = false;

			for(auto& idx : cc.component_indexes) {
				if(idx.property_name == i.property_name) {
					needs_adjust = true;
				}
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == obj.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(" + target_id + ".index()))";
				else if(idx.property_name == i.property_name)
					params += "temp";
				else
					params += obj.name + ".m_" + idx.property_name + ".vptr()[" + target_id + ".index()]";
			}
			if(needs_adjust) {
				o + substitute{ "params", params } +substitute{ "ckname", cc.name };
				o + "auto temp = @obj@.m_@prop@.vptr()[id.index()];";
				o + "temp[i] = value;";
				o + "std::sort(temp.begin(), temp.end(), [](@type@ a, @type@ b){ return a.value < b.value; });";
				o + "if(@obj@.hashm_@ckname@.find( @obj@.to_@ckname@_keydata(@params@) ) != @obj@.hashm_@ckname@.end())" + block{
					o + "return false;";
				};
			}
		}
	}
}

void make_remove_add_current_composite_keys_text(basic_builder& o, std::string const& target_id, relationship_object_def const& obj, related_object const& i) {
	o + substitute{ "target_id", target_id };
	if(i.is_covered_by_composite_key) {
		for(auto& cc : obj.composite_indexes) {
			std::string params;
			bool needs_adjust = false;

			for(auto& idx : cc.component_indexes) {
				if(idx.property_name == i.property_name) {
					needs_adjust = true;
				}
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == obj.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(" + target_id + ".index()))";
				else
					params += obj.name + ".m_" + idx.property_name + ".vptr()[" + target_id + ".index()]";
			}
			if(needs_adjust) {
				o + substitute{ "params", params } +substitute{ "ckname", cc.name };
				o + "if(auto it = @obj@.hashm_@ckname@.find( @obj@.to_@ckname@_keydata(@params@) ); it != @obj@.hashm_@ckname@.end())" + block{
					o + "delete_@obj@(it->second);";
				};
				o + "@obj@.hashm_@ckname@.insert_or_assign(@obj@.to_@ckname@_keydata(@params@), @target_id@);";
			}
		}
	}
}

void make_force_add_current_composite_keys_text(basic_builder& o, std::string const& target_id, relationship_object_def const& obj, related_object const& i) {
	o + substitute{ "target_id", target_id };
	if(i.is_covered_by_composite_key) {
		for(auto& cc : obj.composite_indexes) {
			std::string params;
			bool needs_adjust = false;

			for(auto& idx : cc.component_indexes) {
				if(idx.property_name == i.property_name) {
					needs_adjust = true;
				}
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == obj.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(" + target_id +".index()))";
				else
					params += obj.name + ".m_" + idx.property_name + ".vptr()[" + target_id + ".index()]";
			}
			if(needs_adjust) {
				o + substitute{ "params", params } +substitute{ "ckname", cc.name };
				o + "@obj@.hashm_@ckname@.insert_or_assign(@obj@.to_@ckname@_keydata(@params@), @target_id@);";
			}
		}
	}
}

void make_link_member_declarations(basic_builder& o, file_def const& parsed_file, relationship_object_def const& obj,
	related_object const& i, bool add_prefix, std::string const& namesp, bool const_mode) {

	if(!add_prefix) {
		o + substitute{ "rtype", i.type_name + (const_mode ? "_const_fat_id"  : "_fat_id") }  +substitute{ "prop", i.property_name };
		o + substitute{ "prtype", i.type_name + "_id" };
		o + substitute{ "namesp", namesp };
		

		if(o.declaration_mode && i.protection == protection_type::hidden)
			o + "private:";

		if(i.multiplicity == 1) {
			o + "DCON_RELEASE_INLINE @rtype@ @namesp@get_@prop@() const noexcept" + block{
					o + "return @rtype@(container, container.@obj@_get_@prop@(id));";
			};
		} else {
			o + "DCON_RELEASE_INLINE @rtype@ @namesp@get_@prop@(int32_t i) const noexcept" + block{
					o + "return @rtype@(container, container.@obj@_get_@prop@(id, i));";
			};
			o + "DCON_RELEASE_INLINE bool @namesp@has_@prop@(@prtype@ val) const noexcept" + block{
					o + "return container.@obj@_has_@prop@(id, val);";
			};
		}
		if(o.declaration_mode && i.protection == protection_type::hidden)
			o + "public:";

		if(!const_mode) {
			if((o.declaration_mode) &&
				(i.protection == protection_type::read_only || i.protection == protection_type::hidden))
				o + "private:";

			if(i.multiplicity == 1) {
				o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(@prtype@ val) const noexcept" + block{
					o + "container.@obj@_set_@prop@(id, val);";
				};

				o + "DCON_RELEASE_INLINE bool @namesp@try_set_@prop@(@prtype@ val) const noexcept" + block{
					o + "return container.@obj@_try_set_@prop@(id, val);";
				};
			} else {
				o + "DCON_RELEASE_INLINE void @namesp@set_@prop@(int32_t i, @prtype@ val) const noexcept" + block{
					o + "container.@obj@_set_@prop@(id, i, val);";
				};
				o + "DCON_RELEASE_INLINE bool @namesp@try_set_@prop@(int32_t i, @prtype@ val) const noexcept" + block{
					o + "return container.@obj@_try_set_@prop@(id, i, val);";
				};
				o + "DCON_RELEASE_INLINE void @namesp@replace_@prop@(@prtype@ newval, @prtype@ oldval) const noexcept" + block{
					o + "container.@obj@_replace_@prop@(id, newval, oldval);";
				};
				o + "DCON_RELEASE_INLINE bool @namesp@try_replace_@prop@(@prtype@ newval, @prtype@ oldval) const noexcept" + block{
					o + "return container.@obj@_try_replace_@prop@(id, newval, oldval);";
				};
			}
			

			if((o.declaration_mode) && 
				(i.protection == protection_type::read_only || i.protection == protection_type::hidden))
				o + "public:";
		}
	} else {
		if(i.index == index_type::at_most_one && obj.primary_key == i) {
			o + substitute{ "rel", obj.name } +substitute{ "prop", i.property_name } +substitute{ "prop_type", i.type_name } +
				substitute{ "vector_position", obj.is_expandable ? "ve::unaligned_contiguous_tags" : "ve::contiguous_tags" };

			if(i.protection == protection_type::hidden) {
				o + "private:";
				o + "friend @rtype@ @obj@_fat_id::get_@prop@() const noexcept;";
				o + "friend @rtype@ @obj@_const_fat_id::get_@prop@() const noexcept;";
			}

			o + "DCON_RELEASE_INLINE @prop_type@_id @rel@_get_@prop@(@rel@_id id) const noexcept" + block{
				o + "return @prop_type@_id(@prop_type@_id::value_base_t(id.index()));";
			};

			o + "#ifndef DCON_NO_VE";
			o + "DCON_RELEASE_INLINE @vector_position@<@prop_type@_id> @rel@_get_@prop@(@vector_position@<@rel@_id> id) const noexcept" + block{
				o + "return @vector_position@<@prop_type@_id>(id.value);";
			};

			o + "DCON_RELEASE_INLINE ve::partial_contiguous_tags<@prop_type@_id> @rel@_get_@prop@(ve::partial_contiguous_tags<@rel@_id> id) const noexcept" + block{
				o + "return ve::partial_contiguous_tags<@prop_type@_id>(id.value, id.subcount);";
			};

			o + "DCON_RELEASE_INLINE ve::tagged_vector<@prop_type@_id> @rel@_get_@prop@(ve::tagged_vector<@rel@_id> id) const noexcept" + block{
				o + "return ve::tagged_vector<@prop_type@_id>(id, std::true_type{});";
			};
			o + "#endif";
			

			if(i.protection == protection_type::hidden)
				o + "public:";

			o + "private:";
			o + "void internal_@rel@_set_@prop@(@rel@_id id, @prop_type@_id value) noexcept" + block{
				o + "if(bool(value))" + block{
					o + "delete_@rel@( @rel@_id(@rel@_id::value_base_t(value.index())) );";
					o + "internal_move_relationship_@rel@(id, @rel@_id(@rel@_id::value_base_t(value.index())) );";
				};
			};
			o + "public:";

			if(i.protection == protection_type::read_only || i.protection == protection_type::hidden) {
				o + "private:";
				o + "friend void @obj@_fat_id::set_@prop@( @prop_type@_id) const noexcept;";
			}

			o + "void @rel@_set_@prop@(@rel@_id id, @prop_type@_id value) noexcept" + block{
				o + "if(bool(value))" + block{
					o + "delete_@rel@( @rel@_id(@rel@_id::value_base_t(value.index())) );";
					o + "internal_move_relationship_@rel@(id, @rel@_id(@rel@_id::value_base_t(value.index())) );";
				} +append{"else"} +block{
					o + "delete_@rel@(id);";
				};
			};

			o + "bool @rel@_try_set_@prop@(@rel@_id id, @prop_type@_id value) noexcept" + block{
				o + "if(bool(value))" + block{
					o + "if(@rel@_is_valid( @rel@_id(@rel@_id::value_base_t(value.index())) )) return false;";
					o + "internal_move_relationship_@rel@(id, @rel@_id(@rel@_id::value_base_t(value.index())) );";
					o + "return true;";
				} +append{"else"} +block{
					o + "return false;";
				};
			};

			if(i.protection == protection_type::read_only || i.protection == protection_type::hidden)
				o + "public:";

		} else if(i.index == index_type::at_most_one) {

			o + substitute{ "obj", obj.name } +substitute{ "prop", i.property_name } +substitute{ "type", i.type_name + "_id" };
			o + substitute{ "vector_position", obj.is_expandable ? "ve::unaligned_contiguous_tags" : "ve::contiguous_tags" };
			
			if(i.protection == protection_type::hidden) {
				o + "private:";
				if(i.multiplicity == 1) {
					o + "friend @rtype@ @obj@_fat_id::get_@prop@() const noexcept;";
					o + "friend @rtype@ @obj@_const_fat_id::get_@prop@() const noexcept;";
				} else {
					o + "friend @rtype@ @obj@_fat_id::get_@prop@(int32_t) const noexcept;";
					o + "friend @rtype@ @obj@_const_fat_id::get_@prop@(int32_t) const noexcept;";
					o + "friend bool @obj@_fat_id::has_@prop@(@prop_type@_i) const noexcept;";
					o + "friend bool @obj@_const_fat_id::has_@prop@(@prop_type@_i) const noexcept;";
				}
			}

			if(i.multiplicity == 1) {
				o + "DCON_RELEASE_INLINE @type@ @obj@_get_@prop@(@obj@_id id) const noexcept" + block{
					o + "return @obj@.m_@prop@.vptr()[id.index()];";
				};

				o + "#ifndef DCON_NO_VE";
				o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(@vector_position@<@obj@_id> id) const noexcept" + block{
					o + "return ve::load(id, @obj@.m_@prop@.vptr());";
				};
				o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id) const noexcept" + block{
					o + "return ve::load(id, @obj@.m_@prop@.vptr());";
				};
				o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::tagged_vector<@obj@_id> id) const noexcept" + block{
					o + "return ve::load(id, @obj@.m_@prop@.vptr());";
				};
				o + "#endif";
			} else {
				o + "DCON_RELEASE_INLINE @type@ @obj@_get_@prop@(@obj@_id id, int32_t i) const noexcept" + block{
					o + "return @obj@.m_@prop@.vptr()[id.index()][i];";
				};
				o + "DCON_RELEASE_INLINE bool @obj@_has_@prop@(@obj@_id id, @type@ v) const noexcept" + block{
					for(int32_t j = 0; j < i.multiplicity; ++j) {
						o + substitute{"i", std::to_string(j)};
						o + "if(@obj@.m_@prop@.vptr()[id.index()][@i@] == v) return true;";
					}
					o + "return false;";
				};
			}

			if(i.protection == protection_type::hidden) {
				o + "public:";
			}

			o + "private:";

			if(i.multiplicity == 1) {
				o + "void internal_@obj@_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
					o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()]; bool(old_value))" + block{
						o + "@obj@.m_link_back_@prop@.vptr()[old_value.index()] = @obj@_id();";
					};
					o + "if(bool(value))" + block{
						if(!i.is_optional) {
							o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
								o + "delete_@obj@(old_rel);";
							};
						} else {
							o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
								o + "@obj@.m_@prop@.vptr()[old_rel.index()] = @type@();";
							};
						}
						o + "@obj@.m_link_back_@prop@.vptr()[value.index()] = id;";
					};
					o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
				};
			} else {
				o + "void internal_@obj@_set_@prop@(@obj@_id id, int32_t i, @type@ value) noexcept" + block{
					o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()][i]; bool(old_value))" + block{
						o + "@obj@.m_link_back_@prop@.vptr()[old_value.index()] = @obj@_id();";
					};
					o + "if(bool(value))" + block{
						if(!i.is_optional) {
							o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
								o + "delete_@obj@(old_rel);";
							};
						} else {
							o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
								o + "@obj@.m_@prop@.vptr()[old_rel.index()] = @type@();";
							};
						}
						o + "@obj@.m_link_back_@prop@.vptr()[value.index()] = id;";
					};
					o + "@obj@.m_@prop@.vptr()[id.index()][i] = value;";
				};
			}
			o + "public:";

			if(i.protection == protection_type::read_only || i.protection == protection_type::hidden) {
				o + "private:";
				if(i.multiplicity == 1) {
					o + "friend void @obj@_fat_id::set_@prop@( @prop_type@_id) const noexcept;";
					o + "friend bool @obj@_fat_id::try_set_@prop@( @prop_type@_id) const noexcept;";
				} else {
					o + "friend void @obj@_fat_id::set_@prop@(int32_t, @prop_type@_id) const noexcept;";
					o + "friend bool @obj@_fat_id::try_set_@prop@(int32_t, @prop_type@_id) const noexcept;";
					o + "friend void @obj@_fat_id::replace_@prop@(@prop_type@_id, @prop_type@_id) const noexcept;";
					o + "friend bool @obj@_fat_id::try_replace_@prop@(@prop_type@_id, @prop_type@_id) const noexcept;";
				}
			}

			if(i.multiplicity == 1) {
				o + "void @obj@_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
					o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()]; bool(old_value))" + block{
						o + "@obj@.m_link_back_@prop@.vptr()[old_value.index()] = @obj@_id();";
					};
					o + "if(bool(value))" + block{
						if(!i.is_optional) {
							o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
								o + "delete_@obj@(old_rel);";
							};
						} else {
							o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
								o + "@obj@.m_@prop@.vptr()[old_rel.index()] = @type@();";
							};
						}
						o + "@obj@.m_link_back_@prop@.vptr()[value.index()] = id;";

						make_delete_current_composite_keys_text(o, "id", obj, i);
						o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
						make_remove_add_current_composite_keys_text(o, "id", obj, i);
					} +append{ "else" } +block{
						if(!i.is_optional) {
							o + "delete_@obj@(id);";
						} else {
							make_delete_current_composite_keys_text(o, "id", obj, i);
							o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
							make_remove_add_current_composite_keys_text(o, "id", obj, i);
						}
					};
				};
			} else {
				o + "void @obj@_set_@prop@(@obj@_id id, int32_t i, @type@ value) noexcept" + block{
					o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()][i]; bool(old_value))" + block{
						o + "@obj@.m_link_back_@prop@.vptr()[old_value.index()] = @obj@_id();";
					};
					if(i.is_distinct) {
						o + "if(@obj@_has_@prop@(id, value)) value = @type@();";
					}
					o + "if(bool(value))" + block{
						if(!i.is_optional) {
							o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
								o + "delete_@obj@(old_rel);";
							};
						} else {
							o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
								o + "@obj@.m_@prop@.vptr()[old_rel.index()] = @type@();";
							};
						}
						o + "@obj@.m_link_back_@prop@.vptr()[value.index()] = id;";

						make_delete_current_composite_keys_text(o, "id", obj, i);
						o + "@obj@.m_@prop@.vptr()[id.index()][i] = value;";
						if(i.is_covered_by_composite_key) {
							o + "std::sort(@obj@.m_@prop@.vptr()[id.index()].begin(), @obj@.m_@prop@.vptr()[id.index()].end(), "
								"[](@type@ a, @type@ b){ return a.value < b.value; });";
						}
						make_remove_add_current_composite_keys_text(o, "id", obj, i);
					} +append{ "else" } +block{
						if(!i.is_optional) {
							o + "delete_@obj@(id);";
						} else {
							make_delete_current_composite_keys_text(o, "id", obj, i);
							o + "@obj@.m_@prop@.vptr()[id.index()][i] = value;";
							if(i.is_covered_by_composite_key) {
								o + "std::sort(@obj@.m_@prop@.vptr()[id.index()].begin(), @obj@.m_@prop@.vptr()[id.index()].end(), "
									"[](@type@ a, @type@ b){ return a.value < b.value; });";
							}
							make_remove_add_current_composite_keys_text(o, "id", obj, i);
						}
					};
				};

				o + "void @obj@_replace_@prop@(@obj@_id id, @type@ newvalue, @type@ oldvalue) noexcept" + block{
					for(int32_t j = 0; j < i.multiplicity; ++j) {
						o + substitute{"i", std::to_string(j)};
						o + "if(@obj@.m_@prop@.vptr()[id.index()][@i@] == oldvalue)" + block{
							o + "@obj@_set_@prop@(id, @i@, newvalue);";
							o + "return;";
						};
					}
				};
			}

			if(i.multiplicity == 1) {
				o + "bool @obj@_try_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
					if(!i.is_optional) {
						o + "if(!bool(value)) return false;";
					}
					make_try_current_composite_keys_text(o, "id", obj, i);

					o + "if(bool(value))" + block{
						o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
							o + "return false;";
						};
						o + "@obj@.m_link_back_@prop@.vptr()[value.index()] = id;";
					};
					o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()]; bool(old_value))" + block{
						o + "@obj@.m_link_back_@prop@.vptr()[old_value.index()] = @obj@_id();";
					};
					o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
					make_force_add_current_composite_keys_text(o, "id", obj, i);
					o + "return true;";
				};
			} else {
				o + "bool @obj@_try_set_@prop@(@obj@_id id, int32_t i, @type@ value) noexcept" + block{
					if(!i.is_optional) {
						o + "if(!bool(value)) return false;";
					}
					if(i.is_distinct) {
						o + "if(@obj@_has_@prop@(id, value)) return false;";
					}
					make_try_current_composite_keys_text_with_multiplicity(o, "id", obj, i);

					o + "if(bool(value))" + block{
						o + "if(auto old_rel = @obj@.m_link_back_@prop@.vptr()[value.index()]; bool(old_rel))" + block{
							o + "return false;";
						};
						o + "@obj@.m_link_back_@prop@.vptr()[value.index()] = id;";
					};
					o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()][i]; bool(old_value))" + block{
						o + "@obj@.m_link_back_@prop@.vptr()[old_value.index()] = @obj@_id();";
					};
					o + "@obj@.m_@prop@.vptr()[id.index()][i] = value;";
					if(i.is_covered_by_composite_key) {
						o + "std::sort(@obj@.m_@prop@.vptr()[id.index()].begin(), @obj@.m_@prop@.vptr()[id.index()].end(), "
							"[](@type@ a, @type@ b){ return a.value < b.value; });";
					}
					make_force_add_current_composite_keys_text(o, "id", obj, i);
					o + "return true;";
				};

				o + "bool @obj@_try_replace_@prop@(@obj@_id id, @type@ newvalue, @type@ oldvalue) noexcept" + block{
					for(int32_t j = 0; j < i.multiplicity; ++j) {
						o + substitute{"i", std::to_string(j)};
						o + "if(@obj@.m_@prop@.vptr()[id.index()][@i@] == oldvalue)" + block{
							o + "return @obj@_try_set_@prop@(id, @i@, newvalue);";
						};
					}
					o + "return false;";
				};
			}

			if(i.protection == protection_type::read_only || i.protection == protection_type::hidden)
				o + "public:";

		} else if(i.index == index_type::many) {
			o + substitute{ "obj", obj.name } +substitute{ "prop", i.property_name } +substitute{ "type", i.type_name + "_id" };
			o + substitute{ "vector_position", obj.is_expandable ? "ve::unaligned_contiguous_tags" : "ve::contiguous_tags" };

			if(i.protection == protection_type::hidden) {
				o + "private:";
				if(i.multiplicity == 1) {
					o + "friend @rtype@ @obj@_fat_id::get_@prop@() const noexcept;";
					o + "friend @rtype@ @obj@_const_fat_id::get_@prop@() const noexcepot;";
				} else {
					o + "friend @rtype@ @obj@_fat_id::get_@prop@(int32_t) const noexcept;";
					o + "friend @rtype@ @obj@_const_fat_id::get_@prop@(int32_t) const noexcept;";
					o + "friend bool @obj@_fat_id::has_@prop@(@prop_type@_i) const noexcept;";
					o + "friend bool @obj@_const_fat_id::has_@prop@(@prop_type@_i) const noexcept;";
				}
			}

			if(i.multiplicity == 1) {
				o + "DCON_RELEASE_INLINE @type@ @obj@_get_@prop@(@obj@_id id) const noexcept" + block{
					o + "return @obj@.m_@prop@.vptr()[id.index()];";
				};

				o + "#ifndef DCON_NO_VE";
				o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(@vector_position@<@obj@_id> id) const noexcept" + block{
					o + "return ve::load(id, @obj@.m_@prop@.vptr());";
				};
				o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id) const noexcept" + block{
					o + "return ve::load(id, @obj@.m_@prop@.vptr());";
				};
				o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::tagged_vector<@obj@_id> id) const noexcept" + block{
					o + "return ve::load(id, @obj@.m_@prop@.vptr());";
				};
				o + "#endif";
			} else {
				o + "DCON_RELEASE_INLINE @type@ @obj@_get_@prop@(@obj@_id id, int32_t i) const noexcept" + block{
					o + "return @obj@.m_@prop@.vptr()[id.index()][i];";
				};
				o + "DCON_RELEASE_INLINE bool @obj@_has_@prop@(@obj@_id id, @type@ v) const noexcept" + block{
					for(int32_t j = 0; j < i.multiplicity; ++j) {
						o + substitute{"i", std::to_string(j)};
						o + "if(@obj@.m_@prop@.vptr()[id.index()][@i@] == v) return true;";
					}
					o + "return false;";
				};
			}

			if(i.protection == protection_type::hidden) {
				o + "public:";
			}

			o + "private:";

			if(i.multiplicity == 1) {
				o + "void internal_@obj@_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
					if(i.ltype == list_type::list) {
						o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()]; bool(old_value))" + block{
							o + "if(auto old_left = @obj@.m_link_@prop@.vptr()[id.index()].left; bool(old_left))" + block{
								o + "@obj@.m_link_@prop@.vptr()[old_left.index()].right = @obj@.m_link_@prop@.vptr()[id.index()].right;";
							} +append{"else"} +block{ // else: was the current head of the existing list
								o + "@obj@.m_head_back_@prop@.vptr()[old_value.index()] = @obj@.m_link_@prop@.vptr()[id.index()].right;";
							};
							o + "if(auto old_right = @obj@.m_link_@prop@.vptr()[id.index()].right; bool(old_right))" + block{
								o + "@obj@.m_link_@prop@.vptr()[old_right.index()].left = @obj@.m_link_@prop@.vptr()[id.index()].left;";
							};
						};
						o + "if(bool(value))" + block{
							o + "if(auto existing_list = @obj@.m_head_back_@prop@.vptr()[value.index()]; bool(existing_list))" + block{
								o + "@obj@.m_link_@prop@.vptr()[id.index()].left = existing_list;";
								o + "if(auto r = @obj@.m_link_@prop@.vptr()[existing_list.index()].right; bool(r))" + block{
									o + "@obj@.m_link_@prop@.vptr()[id.index()].right = r;";
									o + "@obj@.m_link_@prop@.vptr()[r.index()].left = id;";
								} +append{ "else" } +block{
									o + "@obj@.m_link_@prop@.vptr()[id.index()].right = @obj@_id();";
								};
								o + "@obj@.m_link_@prop@.vptr()[existing_list.index()].right = id;";
								o + "@obj@.m_head_back_@prop@.vptr()[value.index()] = existing_list;";
							} +append{"else"} +block{
								o + "@obj@.m_head_back_@prop@.vptr()[value.index()] = id;";
								o + "@obj@.m_link_@prop@.vptr()[id.index()].right = @obj@_id();";
								o + "@obj@.m_link_@prop@.vptr()[id.index()].left = @obj@_id();";
							};
						} +append{ "else" } +block{
							o + "@obj@.m_link_@prop@.vptr()[id.index()].right = @obj@_id();";
							o + "@obj@.m_link_@prop@.vptr()[id.index()].left = @obj@_id();";
						};
						o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
					} else if(i.ltype == list_type::array) {
						o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()]; bool(old_value))" + block{
							o + "auto& vref = @obj@.m_array_@prop@.vptr()[old_value.index()];";
							o + "dcon::remove_unique_item(@obj@.@prop@_storage, vref, id);";
						};
						o + "if(bool(value))" + block{
							o + "dcon::push_back(@obj@.@prop@_storage, @obj@.m_array_@prop@.vptr()[value.index()], id);";
						};
						o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
					} else if(i.ltype == list_type::std_vector) {
						o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()]; bool(old_value))" + block{
							o + "auto& vref = @obj@.m_array_@prop@.vptr()[old_value.index()];";
							o + "if(auto it = std::find(vref.begin(), vref.end(), id); it != vref.end())" + block{
								o + "*it = vref.back();";
								o + "vref.pop_back();";
							};
						};
						o + "if(bool(value))" + block{
							o + "@obj@.m_array_@prop@.vptr()[value.index()].push_back(id);";
						};
						o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
					}
				};
			} else {
				o + "void internal_@obj@_set_@prop@(@obj@_id id, int32_t i, @type@ value) noexcept" + block{
					if(i.ltype == list_type::array) {
						o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()][i]; bool(old_value))" + block{
							o + "auto& vref = @obj@.m_array_@prop@.vptr()[old_value.index()];";
							if(i.is_distinct)
								o + "dcon::remove_unique_item(@obj@.@prop@_storage, vref, id);";
							else
								o + "dcon::remove_all_items(@obj@.@prop@_storage, vref, id);";
						};
						o + "if(bool(value))" + block{
							o + "dcon::push_back(@obj@.@prop@_storage, @obj@.m_array_@prop@.vptr()[value.index()], id);";
						};
						o + "@obj@.m_@prop@.vptr()[id.index()][i] = value;";
					} else if(i.ltype == list_type::std_vector) {
						o + "if(auto old_value = @obj@.m_@prop@.vptr()[id.index()][i]; bool(old_value))" + block{
							if(i.is_distinct) {
								o + "auto& vref = @obj@.m_array_@prop@.vptr()[old_value.index()];";
								o + "if(auto it = std::find(vref.begin(), vref.end(), id); it != vref.end())" + block{
									o + "*it = vref.back();";
									o + "vref.pop_back();";
								};
							} else {
								o + "auto dat = @obj@.m_array_@prop@.vptr()[old_value.index()].data();";
								o + "auto sz = @obj@.m_array_@prop@.vptr()[old_value.index()].size();";
								o + "for(auto j = sz; j > 0; --j)" + block{
									o + "if(*(dat + j - 1) == id)" + block{
										o + "*(dat + j - 1) = *(dat + sz - 1);";
										o + "--sz;";
									};
									o + "vref.resize(sz);";
								};
							}
						};
						o + "if(bool(value))" + block{
							o + "@obj@.m_array_@prop@.vptr()[value.index()].push_back(id);";
						};
						o + "@obj@.m_@prop@.vptr()[id.index()][i] = value;";
					}
				};
			}
			o + "public:";

			if(i.protection == protection_type::read_only || i.protection == protection_type::hidden) {
				o + "private:";
				if(i.multiplicity == 1) {
					o + "friend void @obj@_fat_id::set_@prop@( @prop_type@_id) const noexcept;";
					o + "friend bool @obj@_fat_id::try_set_@prop@( @prop_type@_id) const noexcept;";
				} else {
					o + "friend void @obj@_fat_id::set_@prop@(int32_t, @prop_type@_id) const noexcept;";
					o + "friend bool @obj@_fat_id::try_set_@prop@(int32_t, @prop_type@_id) const noexcept;";
					o + "friend void @obj@_fat_id::replace_@prop@(@prop_type@_id, @prop_type@_id) const noexcept;";
					o + "friend bool @obj@_fat_id::try_replace_@prop@(@prop_type@_id, @prop_type@_id) const noexcept;";
				}
			}

			if(i.multiplicity == 1) {
				o + "void @obj@_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
					if(!i.is_optional) {
						o + "if(!bool(value))" + block{
							o + "delete_@obj@(id);";
							o + "return;";
						};
					}
					make_delete_current_composite_keys_text(o, "id", obj, i);
					o + "internal_@obj@_set_@prop@(id, value);";
					make_remove_add_current_composite_keys_text(o, "id", obj, i);
				};

				o + "bool @obj@_try_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
					if(!i.is_optional) {
						o + "if(!bool(value))" + block{
							o + "return false;";
						};
					}

					make_try_current_composite_keys_text(o, "id", obj, i);
					o + "internal_@obj@_set_@prop@(id, value);";

					make_force_add_current_composite_keys_text(o, "id", obj, i);
					o + "return true;";
				};
			} else {
				o + "void @obj@_set_@prop@(@obj@_id id, int32_t i, @type@ value) noexcept" + block{
					if(i.is_distinct) {
						o + "if(@obj@_has_@prop@(id, value)) value = @type@();";
					}
					if(!i.is_optional) {
						o + "if(!bool(value))" + block{
							o + "delete_@obj@(id);";
							o + "return;";
						};
					}
					make_delete_current_composite_keys_text(o, "id", obj, i);
					o + "internal_@obj@_set_@prop@(id, i, value);";
					if(i.is_covered_by_composite_key) {
						o + "std::sort(@obj@.m_@prop@.vptr()[id.index()].begin(), @obj@.m_@prop@.vptr()[id.index()].end(), "
							"[](@type@ a, @type@ b){ return a.value < b.value; });";
					}
					make_remove_add_current_composite_keys_text(o, "id", obj, i);
				};

				o + "bool @obj@_try_set_@prop@(@obj@_id id, int32_t i, @type@ value) noexcept" + block{
					if(!i.is_optional) {
						o + "if(!bool(value))" + block{
							o + "return false;";
						};
					}
					if(i.is_distinct) {
						o + "if(@obj@_has_@prop@(id, value)) return false;";
					}
					make_try_current_composite_keys_text_with_multiplicity(o, "id", obj, i);
					o + "internal_@obj@_set_@prop@(id, i, value);";
					if(i.is_covered_by_composite_key) {
						o + "std::sort(@obj@.m_@prop@.vptr()[id.index()].begin(), @obj@.m_@prop@.vptr()[id.index()].end(), "
							"[](@type@ a, @type@ b){ return a.value < b.value; });";
					}
					make_force_add_current_composite_keys_text(o, "id", obj, i);
					o + "return true;";
				};

				o + "void @obj@_replace_@prop@(@obj@_id id, @type@ newvalue, @type@ oldvalue) noexcept" + block{
					for(int32_t j = 0; j < i.multiplicity; ++j) {
						o + substitute{"i", std::to_string(j)};
						o + "if(@obj@.m_@prop@.vptr()[id.index()][@i@] == oldvalue)" + block{
							o + "@obj@_set_@prop@(id, @i@, newvalue);";
							o + "return;";
						};
					}
				};

				o + "bool @obj@_try_replace_@prop@(@obj@_id id, @type@ newvalue, @type@ oldvalue) noexcept" + block{
					for(int32_t j = 0; j < i.multiplicity; ++j) {
						o + substitute{"i", std::to_string(j)};
						o + "if(@obj@.m_@prop@.vptr()[id.index()][@i@] == oldvalue)" + block{
							o + "return @obj@_try_set_@prop@(id, @i@, newvalue);";
						};
					}
					o + "return false;";
				};
			}
			if(i.protection == protection_type::read_only || i.protection == protection_type::hidden)
				o + "public:";
		} else { // unindexed
			o + substitute{ "obj", obj.name } +substitute{ "prop", i.property_name } +substitute{ "type", i.type_name + "_id" };
			o + substitute{ "vector_position", obj.is_expandable ? "ve::unaligned_contiguous_tags" : "ve::contiguous_tags" };

			if(i.protection == protection_type::hidden) {
				o + "private:";
				if(i.multiplicity == 1) {
					o + "friend @rtype@ @obj@_fat_id::get_@prop@() const noexcept;";
					o + "friend @rtype@ @obj@_const_fat_id::get_@prop@() const noexcpet;";
				} else {
					o + "friend @rtype@ @obj@_fat_id::get_@prop@(int32_t) const noexcept;";
					o + "friend @rtype@ @obj@_const_fat_id::get_@prop@(int32_t) const noexcept;";
					o + "friend bool @obj@_fat_id::has_@prop@(@prop_type@_i) const noexcept;";
					o + "friend bool @obj@_const_fat_id::has_@prop@(@prop_type@_i) const noexcept;";
				}
			}

			if(i.multiplicity == 1) {
				o + "DCON_RELEASE_INLINE @type@ @obj@_get_@prop@(@obj@_id id) const noexcept" + block{
					o + "return @obj@.m_@prop@.vptr()[id.index()];";
				};

				o + "#ifndef DCON_NO_VE";
				o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(@vector_position@<@obj@_id> id) const noexcept" + block{
					o + "return ve::load(id, @obj@.m_@prop@.vptr());";
				};
				o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::partial_contiguous_tags<@obj@_id> id) const noexcept" + block{
					o + "return ve::load(id, @obj@.m_@prop@.vptr());";
				};
				o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@type@> @obj@_get_@prop@(ve::tagged_vector<@obj@_id> id) const noexcept" + block{
					o + "return ve::load(id, @obj@.m_@prop@.vptr());";
				};
				o + "#endif";
			} else {
				o + "DCON_RELEASE_INLINE @type@ @obj@_get_@prop@(@obj@_id id, int32_t i) const noexcept" + block{
					o + "return @obj@.m_@prop@.vptr()[id.index()][i];";
				};
				o + "DCON_RELEASE_INLINE bool @obj@_has_@prop@(@obj@_id id, @type@ v) const noexcept" + block{
					for(int32_t j = 0; j < i.multiplicity; ++j) {
						o + substitute{"i", std::to_string(j)};
						o + "if(@obj@.m_@prop@.vptr()[id.index()][@i@] == v) return true;";
					}
					o + "return false;";
				};
			}

			if(i.protection == protection_type::hidden) {
				o + "public:";
			}

			o + "private:";
			if(i.multiplicity == 1) {
				o + "DCON_RELEASE_INLINE void internal_@obj@_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
					o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
				};
			} else {
				o + "DCON_RELEASE_INLINE void internal_@obj@_set_@prop@(@obj@_id id, int32_t i, @type@ value) noexcept" + block{
					o + "@obj@.m_@prop@.vptr()[id.index()][i] = value;";
				};
			}
			o + "public:";

			if(i.protection == protection_type::read_only || i.protection == protection_type::hidden) {
				o + "private:";
				if(i.multiplicity == 1) {
					o + "friend void @obj@_fat_id::set_@prop@( @prop_type@_id) const noexcept;";
					o + "friend bool @obj@_fat_id::try_set_@prop@( @prop_type@_id) const noexcept;";
				} else {
					o + "friend void @obj@_fat_id::set_@prop@(int32_t, @prop_type@_id) const noexcept;";
					o + "friend bool @obj@_fat_id::try_set_@prop@(int32_t, @prop_type@_id) const noexcept;";
					o + "friend void @obj@_fat_id::replace_@prop@(@prop_type@_id, @prop_type@_id) const noexcept;";
					o + "friend bool @obj@_fat_id::try_replace_@prop@(@prop_type@_id, @prop_type@_id) const noexcept;";
				}
			}

			if(i.multiplicity == 1) {
				o + "void @obj@_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
					if(!i.is_optional) {
						o + "if(!bool(value))" + block{
							o + "delete_@obj@(id);";
							o + "return;";
						};
					}
					make_delete_current_composite_keys_text(o, "id", obj, i);
					o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
					make_remove_add_current_composite_keys_text(o, "id", obj, i);
				};
				o + "bool @obj@_try_set_@prop@(@obj@_id id, @type@ value) noexcept" + block{
					if(!i.is_optional) {
						o + "if(!bool(value)) return false;";
					}
					make_try_current_composite_keys_text(o, "id", obj, i);
					o + "@obj@.m_@prop@.vptr()[id.index()] = value;";
					make_force_add_current_composite_keys_text(o, "id", obj, i);
					o + "return true;";
				};
			} else {
				o + "void @obj@_set_@prop@(@obj@_id id, int32_t i, @type@ value) noexcept" + block{
					if(i.is_distinct) {
						o + "if(@obj@_has_@prop@(id, value)) value = @type@();";
					}
					if(!i.is_optional) {
						o + "if(!bool(value))" + block{
							o + "delete_@obj@(id);";
							o + "return;";
						};
					}
					make_delete_current_composite_keys_text(o, "id", obj, i);
					o + "internal_@obj@_set_@prop@(id, i, value);";
					if(i.is_covered_by_composite_key) {
						o + "std::sort(@obj@.m_@prop@.vptr()[id.index()].begin(), @obj@.m_@prop@.vptr()[id.index()].end(), "
							"[](@type@ a, @type@ b){ return a.value < b.value; });";
					}
					make_remove_add_current_composite_keys_text(o, "id", obj, i);
				};

				o + "bool @obj@_try_set_@prop@(@obj@_id id, int32_t i, @type@ value) noexcept" + block{
					if(!i.is_optional) {
						o + "if(!bool(value))" + block{
							o + "return false;";
						};
					}
					if(i.is_distinct) {
						o + "if(@obj@_has_@prop@(id, value)) return false;";
					}
					make_try_current_composite_keys_text_with_multiplicity(o, "id", obj, i);
					o + "internal_@obj@_set_@prop@(id, i, value);";
					if(i.is_covered_by_composite_key) {
						o + "std::sort(@obj@.m_@prop@.vptr()[id.index()].begin(), @obj@.m_@prop@.vptr()[id.index()].end(), "
							"[](@type@ a, @type@ b){ return a.value < b.value; });";
					}
					make_force_add_current_composite_keys_text(o, "id", obj, i);
					o + "return true;";
				};

				o + "void @obj@_replace_@prop@(@obj@_id id, @type@ newvalue, @type@ oldvalue) noexcept" + block{
					for(int32_t j = 0; j < i.multiplicity; ++j) {
						o + substitute{"i", std::to_string(j)};
						o + "if(@obj@.m_@prop@.vptr()[id.index()][@i@] == oldvalue)" + block{
							o + "@obj@_set_@prop@(id, @i@, newvalue);";
							o + "return;";
						};
					}
				};

				o + "bool @obj@_try_replace_@prop@(@obj@_id id, @type@ newvalue, @type@ oldvalue) noexcept" + block{
					for(int32_t j = 0; j < i.multiplicity; ++j) {
						o + substitute{"i", std::to_string(j)};
						o + "if(@obj@.m_@prop@.vptr()[id.index()][@i@] == oldvalue)" + block{
							o + "return @obj@_try_set_@prop@(id, @i@, newvalue);";
						};
					}
					o + "return false;";
				};
			}

			if(!i.is_covered_by_composite_key && i.multiplicity == 1) {
				o + "#ifndef DCON_NO_VE";
				o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(@vector_position@<@obj@_id> id, ve::value_to_vector_type<@type@> values) noexcept" + block{
					o + "ve::store(id, @obj@.m_@prop@.vptr(), values);";
				};
				o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::partial_contiguous_tags<@obj@_id> id, ve::value_to_vector_type<@type@> values) noexcept" + block{
					o + "ve::store(id, @obj@.m_@prop@.vptr(), values);";
				};
				o + "DCON_RELEASE_INLINE void @obj@_set_@prop@(ve::tagged_vector<@obj@_id> id, ve::value_to_vector_type<@type@> values) noexcept" + block{
					o + "ve::store(id, @obj@.m_@prop@.vptr(), values);";
				};
				o + "#endif";
			}

			if(i.protection == protection_type::read_only || i.protection == protection_type::hidden)
				o + "public:";
		}
	}

}

void primary_key_getter_setter_text(basic_builder & o, bool as_multiple) {
	o + "DCON_RELEASE_INLINE @rel@_id @prop_type@_get_@rel@@as_suffix@(@prop_type@_id id) const noexcept" + block{
		o + "return (id.value <= @rel@.size_used) ? @rel@_id(@rel@_id::value_base_t(id.index())) : @rel@_id();";
	};

	o + "#ifndef DCON_NO_VE";
	o + "DCON_RELEASE_INLINE @vector_position@<@rel@_id> @prop_type@_get_@rel@@as_suffix@"
		"(@vector_position@<@prop_type@_id> id) const noexcept" + block{
		o + "return @vector_position@<@rel@_id>(id.value);";
	};
	o + "DCON_RELEASE_INLINE ve::partial_contiguous_tags<@rel@_id> @prop_type@_get_@rel@@as_suffix@"
		"(ve::partial_contiguous_tags<@prop_type@_id> id) const noexcept" + block{
		o + "return ve::partial_contiguous_tags<@rel@_id>(id.value, id.subcount);";
	};

	o + "DCON_RELEASE_INLINE ve::tagged_vector<@rel@_id> @prop_type@_get_@rel@@as_suffix@"
		"(ve::tagged_vector<@prop_type@_id> id) const noexcept" + block{
		o + "return ve::tagged_vector<@rel@_id>(id, std::true_type{});";
	};
	o + "#endif";

	o + "DCON_RELEASE_INLINE void @prop_type@_remove_@rel@@as_suffix@(@prop_type@_id id) noexcept" + block{
		o + "if(@rel@_is_valid(@rel@_id(@rel@_id::value_base_t(id.index()))))" + block{
			if(as_multiple)
				o + "@rel@_replace_@rel_prop@(@rel@_id(@rel@_id::value_base_t(id.index())), @obj@_id(), id);";
			else
				o + "@rel@_set_@rel_prop@(@rel@_id(@rel@_id::value_base_t(id.index())), @obj@_id());";
		};
	};
}

void unique_getter_setter_text(basic_builder & o, bool as_multiple) {
	o + "DCON_RELEASE_INLINE @rel@_id @prop_type@_get_@rel@@as_suffix@(@prop_type@_id id) const noexcept" + block{
		o + "return @rel@.m_link_back_@rel_prop@.vptr()[id.index()];";
	};

	o + "#ifndef DCON_NO_VE";
	o + "DCON_RELEASE_INLINE ve::tagged_vector<@rel@_id> @prop_type@_get_@rel@@as_suffix@"
		"(@vector_position@<@prop_type@_id> id) const noexcept" + block{
		o + "return ve::load(id, @rel@.m_link_back_@rel_prop@.vptr());";
	};
	o + "DCON_RELEASE_INLINE ve::tagged_vector<@rel@_id> @prop_type@_get_@rel@@as_suffix@"
		"(ve::partial_contiguous_tags<@prop_type@_id> id) const noexcept" + block{
		o + "return ve::load(id, @rel@.m_link_back_@rel_prop@.vptr());";
	};
	o + "DCON_RELEASE_INLINE ve::tagged_vector<@rel@_id> @prop_type@_get_@rel@@as_suffix@"
		"(ve::tagged_vector<@prop_type@_id> id) const noexcept" + block{
		o + "return ve::load(id, @rel@.m_link_back_@rel_prop@.vptr());";
	};
	o + "#endif";

	o + "DCON_RELEASE_INLINE void @prop_type@_remove_@rel@@as_suffix@(@prop_type@_id id) noexcept" + block{
		o + "if(auto backid = @rel@.m_link_back_@rel_prop@.vptr()[id.index()]; bool(backid))" + block{
			if(as_multiple)
				o + "@rel@_replace_@rel_prop@(backid, @obj@_id(), id);";
			else
				o + "@rel@_set_@rel_prop@(backid, @obj@_id());";
		};
	};
}

void join_setter_text(basic_builder& o, property_def const& ip, bool add_prefix, bool as_primary_key) {
	switch(ip.type) {
		case property_type::bitfield:
			if(add_prefix) {
				if(as_primary_key) {
					o + "DCON_RELEASE_INLINE void @obj@_set_@prop@_from_@rel@(@obj@_id ref_id, bool val)" + block{
						o + "@rel@_set_@prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())), val);";
					};
				} else {
					o + "void @obj@_set_@prop@_from_@rel@(@obj@_id id, bool val)" + block{
						o + "if(auto ref_id = @rel@.m_link_back_@as_name@.vptr()[id.index()]; bool(ref_id))" + block{
							o + "@rel@_set_@prop@(ref_id, val);";
						};
					};
				}
			} else {
				o + "DCON_RELEASE_INLINE void @namesp@set_@prop@_from_@rel@( bool v) const noexcept" + block{
					o + "container.@obj@_set_@prop@_from_@rel@(id, v);";
				};
			}
				break;
		case property_type::vectorizable:
			if(add_prefix) {
				if(as_primary_key) {
					o + "void @obj@_set_@prop@_from_@rel@(@obj@_id ref_id, @type@ val)" + block{
						o + "@rel@_set_@prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())), val);";
					};
				} else {
					o + "void @obj@_set_@prop@_from_@rel@(@obj@_id id, @type@ val)" + block{
						o + "if(auto ref_id = @rel@.m_link_back_@as_name@.vptr()[id.index()]; bool(ref_id))" + block{
							o + "@rel@_set_@prop@(ref_id, val);";
						};
					};
				}
			} else {
				o + "DCON_RELEASE_INLINE void @namesp@set_@prop@_from_@rel@(@type@ v) const noexcept" + block{
					o + "container.@obj@_set_@prop@_from_@rel@(id, v);";
				};
			}
				break;
		case property_type::other:
			break;
		case property_type::array_bitfield:
			if(add_prefix) {
				if(as_primary_key) {
					o + "void @obj@_set_@prop@_from_@rel@(@obj@_id ref_id, @i_type@ i, bool val)" + block{
						o + "@rel@_set_@prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())), i, val);";
					};
				} else {
					o + "void @obj@_set_@prop@_from_@rel@(@obj@_id id, @i_type@ i, bool val)" + block{
						o + "if(auto ref_id = @rel@.m_link_back_@as_name@.vptr()[id.index()]; bool(ref_id))" + block{
							o + "@rel@_set_@prop@(ref_id, i, val);";
						};
					};
				}
			} else {
				o + "DCON_RELEASE_INLINE void @namesp@set_@prop@_from_@rel@(@i_type@ i, bool v) const noexcept" + block{
					"container.@obj@_set_@prop@_from_@rel@(id, i, v);";
				};
			}
			break;
		case property_type::array_other:
			break;
		case property_type::array_vectorizable:
			if(add_prefix) {
				if(as_primary_key) {
					o + "void @obj@_set_@prop@_from_@rel@(@obj@_id ref_id, @i_type@ i, @type@ val)" + block{
						o + "@rel@_set_@prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())), i, val);";
					};
				} else {
					o + "void @obj@_set_@prop@_from_@rel@(@obj@_id id, @i_type@ i, @type@ val)" + block{
						o + "if(auto ref_id = @rel@.m_link_back_@as_name@.vptr()[id.index()]; bool(ref_id))" + block{
							o + "@rel@_set_@prop@(ref_id, i, val);";
						};
					};
				}
			} else {
				o + "DCON_RELEASE_INLINE void @namesp@set_@prop@_from_@rel@(@i_type@ i, @type@ v) const noexcept" + block{
					"container.@obj@_set_@prop@_from_@rel@(id, i, v);";
				};
			}
				break;
		case property_type::special_vector:
			break;
	}
}

void join_getter_text(basic_builder& o, property_def const& ip, bool add_prefix, bool as_primary_key, std::optional<std::string> upresult) {
	switch(ip.type) {
		case property_type::bitfield:
			if(add_prefix) {
				if(as_primary_key) {
					o + "bool @obj@_get_@prop@_from_@rel@(@obj@_id ref_id) const" + block{
						o + "return @rel@_get_@prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())));";
					};
					o + "#ifndef DCON_NO_VE";
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(@vector_position@<@obj@_id> ref_id) const" + block{
						o + "return @rel@_get_@prop@(@vector_position@<@rel@_id>(ref_id.value));";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> ref_id) const" + block{
						o + "return @rel@_get_@prop@(ve::partial_contiguous_tags<@rel@_id>(ref_id.value, ref_id.subcount));";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(ve::tagged_vector<@obj@_id> ref_id) const" + block{
						o + "return @rel@_get_@prop@(ve::tagged_vector<@rel@_id>(ref_id, std::true_type{}));";
					};
					o + "#endif";
				} else {
					o + "bool @obj@_get_@prop@_from_@rel@(@obj@_id id) const" + block{
						o + "return @rel@_get_@prop@(@rel@.m_link_back_@as_name@.vptr()[id.index()]);";
					};

					o + "#ifndef DCON_NO_VE";
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(@vector_position@<@obj@_id> id) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id);";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> id) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id);";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(ve::tagged_vector<@obj@_id> id) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id);";
					};
					o + "#endif";
				}
			} else {
				o + "DCON_RELEASE_INLINE bool @namesp@get_@prop@_from_@rel@() const noexcept" + block{
					o + "return container.@obj@_get_@prop@_from_@rel@(id);";
				};
			}
			break;
		case property_type::vectorizable:
			if(add_prefix) {
				if(as_primary_key) {
					o + "@type@ @obj@_get_@prop@_from_@rel@(@obj@_id ref_id) const" + block{
						o + "return @rel@_get_@prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())));";
					};
					o + "#ifndef DCON_NO_VE";
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(@vector_position@<@obj@_id> ref_id) const" + block{
						o + "return @rel@_get_@prop@(@vector_position@<@rel@_id>(ref_id.value));";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> ref_id) const" + block{
						o + "return @rel@_get_@prop@(ve::partial_contiguous_tags<@rel@_id>(ref_id.value, ref_id.subcount));";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(ve::tagged_vector<@obj@_id> ref_id) const" + block{
						o + "return @rel@_get_@prop@(ve::tagged_vector<@rel@_id>(ref_id, std::true_type{}));";
					};
					o + "#endif";
				} else {
					o + "@type@ @obj@_get_@prop@_from_@rel@(@obj@_id id) const" + block{
						o + "return @rel@_get_@prop@(@rel@.m_link_back_@as_name@.vptr()[id.index()]);";
					};

					o + "#ifndef DCON_NO_VE";
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(@vector_position@<@obj@_id> id) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> id) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(ve::tagged_vector<@obj@_id> id) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id);";
					};
					o + "#endif";
				}
			} else {
				if(upresult.has_value())
					o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@_from_@rel@() const noexcept" + block{
						o + "return @type@(container, container.@obj@_get_@prop@_from_@rel@(id));";
				};
				else
					o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@_from_@rel@() const noexcept" + block{
						o + "return container.@obj@_get_@prop@_from_@rel@(id);";
				};
			}
			break;
		case property_type::other:
			break;
		case property_type::array_bitfield:
			if(add_prefix) {
				if(as_primary_key) {
					o + "bool @obj@_get_@prop@_from_@rel@(@obj@_id ref_id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())), i);";
					};
					o + "#ifndef DCON_NO_VE";
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(@vector_position@<@obj@_id> ref_id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(@vector_position@<@rel@_id>(ref_id.value), i);";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> ref_id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(ve::partial_contiguous_tags<@rel@_id>(ref_id.value, ref_id.subcount), i);";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(ve::tagged_vector<@obj@_id> ref_id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(ve::tagged_vector<@rel@_id>(ref_id, std::true_type{}), i);";
					};
					o + "#endif";
				} else {
					o + "bool @obj@_get_@prop@_from_@rel@(@obj@_id id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(@rel@.m_link_back_@as_name@.vptr()[id.index()], i);";
					};

					o + "#ifndef DCON_NO_VE";
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(@vector_position@<@obj@_id> id, @i_type@ i) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id, i);";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> id, @i_type@ i) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id, i);";
					};
					o + "ve::vbitfield_type @obj@_get_@prop@_from_@rel@(ve::tagged_vector<@obj@_id> id, @i_type@ i) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id, i);";
					};
					o + "#endif";
				}
			} else {
				o + "DCON_RELEASE_INLINE bool @namesp@get_@prop@_from_@rel@(@i_type@ i) const noexcept" + block{
					o + "return container.@obj@_get_@prop@_from_@rel@(id, i);";
				};
			}
			break;
		case property_type::array_other:
			break;
		case property_type::array_vectorizable:
			if(add_prefix) {
				if(as_primary_key) {
					o + "@type@ @obj@_get_@prop@_from_@rel@(@obj@_id ref_id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())), i);";
					};
					o + "#ifndef DCON_NO_VE";
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(@vector_position@<@obj@_id> ref_id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(@vector_position@<@rel@_id>(ref_id.value), i);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> ref_id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(ve::partial_contiguous_tags<@rel@_id>(ref_id.value, ref_id.subcount), i);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(ve::tagged_vector<@obj@_id> ref_id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(ve::tagged_vector<@rel@_id>(ref_id, std::true_type{}), i);";
					};
					o + "#endif";
				} else {
					o + "@type@ @obj@_get_@prop@_from_@rel@(@obj@_id id, @i_type@ i) const" + block{
						o + "return @rel@_get_@prop@(@rel@.m_link_back_@as_name@.vptr()[id.index()], i);";
					};

					o + "#ifndef DCON_NO_VE";
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(@vector_position@<@obj@_id> id, @i_type@ i) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id, i);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> id, @i_type@ i) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id, i);";
					};
					o + "ve::value_to_vector_type<@type@> @obj@_get_@prop@_from_@rel@(ve::tagged_vector<@obj@_id> id, @i_type@ i) const" + block{
						o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
						o + "return @rel@_get_@prop@(ref_id, i);";
					};
					o + "#endif";
				}
			} else {
				if(upresult.has_value())
					o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@_from_@rel@(@i_type@ i) const noexcept" + block{
						o + "return @type@(container, container.@obj@_get_@prop@_from_@rel@(id, i));";
				};
				else
					o + "DCON_RELEASE_INLINE @type@ @namesp@get_@prop@_from_@rel@(@i_type@ i) const noexcept" + block{
						o + "return container.@obj@_get_@prop@_from_@rel@(id, i);";
				};
			}
			break;
		case property_type::special_vector:
			break;
	}
}

void many_getter_setter_text(basic_builder & o, list_type ltype, bool remove_all_needs_sort, bool has_multiplicity) {

	o + "DCON_RELEASE_INLINE internal::const_iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator @obj@_get_@rel@@as_suffix@(@obj@_id id) const" + block{
		o + "return internal::const_iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator(*this, id);";
	};
	o + "DCON_RELEASE_INLINE internal::iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator @obj@_get_@rel@@as_suffix@(@obj@_id id)" + block{
		o + "return internal::iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator(*this, id);";
	};

	o + "template<typename T>";
	o + "DCON_RELEASE_INLINE void @obj@_for_each_@rel@@as_suffix@(@obj@_id id, T&& func) const" + block{
		o + "if(bool(id))" + block{
			if(ltype == list_type::list) {
				o + "for(auto list_pos = @rel@.m_head_back_@rel_prop@.vptr()[id.index()]; "
					"bool(list_pos); list_pos = @rel@.m_link_@rel_prop@.vptr()[list_pos.index()].right)" + block{
					o + "func(list_pos);";
				};
			} else if(ltype == list_type::array) {
				o + "auto vrange = dcon::get_range(@rel@.@rel_prop@_storage, @rel@.m_array_@rel_prop@.vptr()[id.index()]);";
				o + "std::for_each(vrange.first, vrange.second, func);";
			} else if(ltype == list_type::std_vector) {
				o + "auto& vref = @rel@.m_array_@rel_prop@.vptr()[id.index()];";
				o + "std::for_each(vref.begin(), vref.end(), func);";
			}
		};
	};

	if(ltype == list_type::array) {
		o + "DCON_RELEASE_INLINE std::pair<@rel@_id const*, @rel@_id const*> @obj@_range_of_@rel@@as_suffix@"
			"(@obj@_id id) const" + block{
			o + "if(bool(id))" + block{
				o + "auto vrange = dcon::get_range(@rel@.@rel_prop@_storage, @rel@.m_array_@rel_prop@.vptr()[id.index()]);";
				o + "return std::pair<@rel@_id const*, @rel@_id const*>(vrange.first, vrange.second);";
			} +append{"else"} +block{
				o + "return std::pair<@rel@_id const*, @rel@_id const*>(nullptr, nullptr);";
			};
		};
	} else if(ltype == list_type::std_vector) {
		o + "DCON_RELEASE_INLINE std::pair<@rel@_id const*, @rel@_id const*> @obj@_range_of_@rel@@as_suffix@"
			"(@obj@_id id) const" + block{
			o + "if(bool(id))" + block{
				o + "auto& vref = @rel@.m_array_@rel_prop@.vptr()[id.index()];";
				o + "return std::pair<@rel@_id const*, @rel@_id const*>(vref.data(), vref.data() + vref.size());";
			} +append{"else"} +block{
				o + "return std::pair<@rel@_id const*, @rel@_id const*>(nullptr, nullptr);";
			};
		};
	}

	o + "void @obj@_remove_all_@rel@@as_suffix@(@obj@_id id) noexcept" + block{
		if(ltype == list_type::array || ltype == list_type::std_vector) {
			o + "auto rng = @obj@_range_of_@rel@_as_@rel_prop@(id);";
			o + "dcon::local_vector<@rel@_id> temp(rng.first, rng.second);";
		} else {
			o + "dcon::local_vector<@rel@_id> temp;";
			o + "@obj@_for_each_@rel@_as_@rel_prop@(id, [&](@rel@_id j) { temp.push_back(j); });";
		}
		if(remove_all_needs_sort)
			o + "std::sort(temp.begin(), temp.end(), [](@rel@_id l, @rel@_id r){ return l.value > r.value; });";
		if(has_multiplicity)
			o + "std::for_each(temp.begin(), temp.end(), [t = this, id](@rel@_id i) { t->@rel@_replace_@rel_prop@(i, @obj@_id(), id); });";
		else
			o + "std::for_each(temp.begin(), temp.end(), [t = this](@rel@_id i) { t->@rel@_set_@rel_prop@(i, @obj@_id()); });";
	};
}

void relation_many_join_getters_setters_text(basic_builder& o, list_type indexed_as, bool property_is_pk) {

	o + "template<typename T>";
	o + "void @obj@_for_each_@prop@_from_@rel@(@obj@_id id, T&& func) const" + block{
		o + "@obj@_for_each_@rel@_as_@as_name@(id, [&](@rel@_id i)" + block{
			o + "func(@rel@_get_@prop@(i));";
		} +append{");"};
	};

		

	o + "bool @obj@_has_@prop@_from_@rel@(@obj@_id id, @p_type@@ref@ target) const" + block{
		if(indexed_as == list_type::list) {
			o + "for(auto list_pos = @rel@.m_head_back_@as_name@.vptr()[id.index()]; "
				"bool(list_pos); list_pos = @rel@.m_link_@as_name@.vptr()[list_pos.index()].right)" + block{
				if(!property_is_pk) {
					o + "if(@rel@.m_@prop@.vptr()[list_pos.index()] == target) return true;";
				} else {
					o + "if(list_pos.index() == target.index()) return true;";
				}
			};
			o + "return false;";
		} else if(indexed_as == list_type::array) {
			o + "auto vrange = dcon::get_range(@rel@.@as_name@_storage, @rel@.m_array_@as_name@.vptr()[id.index()]);";
			o + "for(auto pos = vrange.first; pos != vrange.second; ++pos)" + block{
				if(!property_is_pk) {
					o + "if(@rel@.m_@prop@.vptr()[pos->index()] == target) return true;";
				} else {
					o + "if(pos->index() == target.index()) return true;";
				}
			};
			o + "return false;";
		} else if(indexed_as == list_type::std_vector) {
			o + "auto& vref = @rel@.m_array_@as_name@.vptr()[id.index()];";
			o + "for(auto pos = vref.begin(); pos != vref.end(); ++pos)" + block{
				if(!property_is_pk) {
					o + "if(@rel@.m_@prop@.vptr()[pos->index()] == target) return true;";
				} else {
					o + "if(pos->index() == target.index()) return true;";
				}
			};
			o + "return false;";
		}
	};
	
}

void make_related_member_declarations(basic_builder& o, file_def const& parsed_file, relationship_object_def const& obj,
	in_relation_information const& in_rel, bool add_prefix, std::string const& namesp, bool const_mode) {

	o + substitute{ "rel", in_rel.relation_name } +substitute{ "rel_prop", in_rel.linked_as->property_name };
	o + substitute{ "namesp", namesp };
	o + substitute{ "vector_position", in_rel.rel_ptr->is_expandable ? "ve::unaligned_contiguous_tags" : "ve::contiguous_tags" };
	o + substitute{ "prop_type", obj.name };
	o + substitute{ "obj", obj.name };
	o + substitute{ "id" , const_mode ? "_const_fat_id" : "_fat_id" };

	if(in_rel.linked_as->index == index_type::at_most_one) {
		if(add_prefix) {
			if(in_rel.linked_as->is_primary_key) {
				o + substitute{ "as_suffix", std::string("_as_") + in_rel.linked_as->property_name };
				primary_key_getter_setter_text(o, in_rel.linked_as->multiplicity > 1);
			} else {
				o + substitute{ "as_suffix", std::string("_as_") + in_rel.linked_as->property_name };
				unique_getter_setter_text(o, in_rel.linked_as->multiplicity > 1);
			}
		} else {
			o + "DCON_RELEASE_INLINE @rel@@id@ @namesp@get_@rel@_as_@rel_prop@() const noexcept" + block{
				o + "return @rel@@id@(container, container.@obj@_get_@rel@_as_@rel_prop@(id));";
			};

			if(!const_mode) {
				o + "DCON_RELEASE_INLINE void @namesp@remove_@rel@_as_@rel_prop@() const noexcept" + block{
					o + "container.@obj@_remove_@rel@_as_@rel_prop@(id);";
				};
			}
		}

		bool is_only_of_type = true;
		for(auto& ir : in_rel.rel_ptr->indexed_objects) {
			if(ir.type_name == obj.name && ir.property_name != in_rel.linked_as->property_name)
				is_only_of_type = false;
		}
		if(is_only_of_type) {

			if(add_prefix) {
				if(in_rel.linked_as->is_primary_key) {
					o + substitute{ "as_suffix", "" };
					primary_key_getter_setter_text(o, in_rel.linked_as->multiplicity > 1);
				} else {
					o + substitute{ "as_suffix", "" };
					unique_getter_setter_text(o, in_rel.linked_as->multiplicity > 1);
				}
			} else {
				o + "DCON_RELEASE_INLINE @rel@@id@ @namesp@get_@rel@() const noexcept" + block{
					o + "return @rel@@id@(container, container.@obj@_get_@rel@(id));";
				};
				if(!const_mode) {
					o + "DCON_RELEASE_INLINE void @namesp@remove_@rel@() const noexcept" + block{
						o + "container.@obj@_remove_@rel@(id);";
					};
				}
			}

			o + substitute{ "as_name", in_rel.linked_as->property_name };

			for(auto& ir : in_rel.rel_ptr->indexed_objects) {
				if(ir.related_to != &obj) {
					o + substitute{ "r_prop", ir.property_name };
					o + substitute{ "r_type", ir.type_name };

					if(ir.protection != protection_type::hidden) {

						
						if(add_prefix) {
							if(in_rel.linked_as->is_primary_key) {
								o + "DCON_RELEASE_INLINE @r_type@_id @obj@_get_@r_prop@_from_@rel@(@obj@_id ref_id) const" + block{
									o + "return @rel@_get_@r_prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())));";
								};
								o + "#ifndef DCON_NO_VE";
								o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@r_type@_id> @obj@_get_@r_prop@_from_@rel@(@vector_position@<@obj@_id> ref_id) const" + block{
									o + "return @rel@_get_@r_prop@(@vector_position@<@rel@_id>(ref_id.value));";
								};
								o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@r_type@_id> @obj@_get_@r_prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> ref_id) const" + block{
									o + "return @rel@_get_@r_prop@(ve::partial_contiguous_tags<@rel@_id>(ref_id.value, ref_id.subcount));";
								};
								o + "DCON_RELEASE_INLINE ve::value_to_vector_type<@r_type@_id> @obj@_get_@r_prop@_from_@rel@(ve::tagged_vector<@obj@_id> ref_id) const" + block{
									o + "return @rel@_get_@r_prop@(ve::tagged_vector<@rel@_id>(ref_id, std::true_type{}));";
								};
								o + "#endif";
							} else {
								o + "@r_type@_id @obj@_get_@r_prop@_from_@rel@(@obj@_id id) const" + block{
									o + "return @rel@_get_@r_prop@(@rel@.m_link_back_@as_name@.vptr()[id.index()]);";
								};

								o + "#ifndef DCON_NO_VE";
								o + "ve::value_to_vector_type<@r_type@_id> @obj@_get_@r_prop@_from_@rel@(@vector_position@<@obj@_id> id) const" + block{
									o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
									o + "return @rel@_get_@r_prop@(ref_id);";
								};
								o + "ve::value_to_vector_type<@r_type@_id> @obj@_get_@r_prop@_from_@rel@(ve::partial_contiguous_tags<@obj@_id> id) const" + block{
									o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
									o + "return @rel@_get_@r_prop@(ref_id);";
								};
								o + "ve::value_to_vector_type<@r_type@_id> @obj@_get_@r_prop@_from_@rel@(ve::tagged_vector<@obj@_id> id) const" + block{
									o + "auto ref_id = ve::load(id, @rel@.m_link_back_@as_name@.vptr());";
									o + "return @rel@_get_@r_prop@(ref_id);";
								};
								o + "#endif";
							}
						} else {
							o + "DCON_RELEASE_INLINE @r_type@@id@ @namesp@get_@r_prop@_from_@rel@() const noexcept" + block{
								o + "return @r_type@@id@(container, container.@obj@_get_@r_prop@_from_@rel@(id));";
							};
						}
					} // end protection hidden check

					if(ir.protection != protection_type::read_only && ir.protection != protection_type::hidden) {
						if(add_prefix) {
							if(in_rel.linked_as->is_primary_key) {
								o + "void @obj@_set_@r_prop@_from_@rel@(@obj@_id ref_id, @r_type@_id val)" + block{
									o + "@rel@_set_@r_prop@(@rel@_id(@rel@_id::value_base_t(ref_id.index())), val);";
								};
							} else {
								o + "void @obj@_set_@r_prop@_from_@rel@(@obj@_id id, @r_type@_id val)" + block{
									o + "if(auto ref_id = @rel@.m_link_back_@as_name@.vptr()[id.index()]; bool(ref_id))" + block{
										o + "@rel@_set_@r_prop@(ref_id, val);";
									};
								};
							}
						} else if(!const_mode) {
							o + "DCON_RELEASE_INLINE void @namesp@set_@r_prop@_from_@rel@(@r_type@_id v) const noexcept" + block{
								o + "container.@obj@_set_@r_prop@_from_@rel@(id, v);";
							};
						}
					}
				}
			}
			for(auto& ip : in_rel.rel_ptr->properties) {
				auto upresult = to_fat_index_type(parsed_file, ip.data_type, const_mode);
				o + substitute{ "type", upresult.has_value() ? *upresult : ip.data_type } +substitute{ "prop", ip.name };
				o + substitute{ "i_type", ip.array_index_type };

				if(ip.protection != protection_type::hidden && ip.protection != protection_type::read_only) {
					if(!const_mode && (!ip.is_derived || ip.hook_set)) {
						join_setter_text(o, ip, add_prefix, in_rel.linked_as->is_primary_key);
					}
				}

				if(ip.protection != protection_type::hidden) {
					if(!ip.is_derived || ip.hook_get) {
						join_getter_text(o, ip, add_prefix, in_rel.linked_as->is_primary_key, upresult);
					}
				}
			} // end loop over properties in joined
		} // end: is only of type

	} else if(in_rel.linked_as->index == index_type::many) {
		if(add_prefix) {
			o + substitute{ "as_suffix", std::string("_as_") + in_rel.linked_as->property_name };
			many_getter_setter_text(o, in_rel.linked_as->ltype, !in_rel.linked_as->is_optional && in_rel.rel_ptr->store_type == storage_type::compactable, in_rel.linked_as->multiplicity > 1);
		} else {
			o + "template<typename T>";
			o + "DCON_RELEASE_INLINE void @namesp@for_each_@rel@_as_@rel_prop@(T&& func) const" + block{
				o + "container.@obj@_for_each_@rel@_as_@rel_prop@(id, [&, t = this](@rel@_id i){func(fatten(t->container, i));});";
			};
			if(in_rel.linked_as->ltype == list_type::array || in_rel.linked_as->ltype == list_type::std_vector) {
				o + "DCON_RELEASE_INLINE std::pair<@rel@_id const*, @rel@_id const*> @namesp@range_of_@rel@_as_@rel_prop@() const" + block{
					o + "return container.@obj@_range_of_@rel@_as_@rel_prop@(id);";
				};
			}
			if(!const_mode) {
				o + "DCON_RELEASE_INLINE void @namesp@remove_all_@rel@_as_@rel_prop@() const noexcept" + block{
					o + "container.@obj@_remove_all_@rel@_as_@rel_prop@(id);";
				};
				o + "DCON_RELEASE_INLINE internal::iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator @namesp@get_@rel@_as_@rel_prop@() const" + block{
					o + "return internal::iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator(container, id);";
				};
			} else {
				o + "DCON_RELEASE_INLINE internal::const_iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator @namesp@get_@rel@_as_@rel_prop@() const" + block{
					o + "return internal::const_iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator(container, id);";
				};
			}
		}
		bool is_only_of_type = true;
		for(auto& ir : in_rel.rel_ptr->indexed_objects) {
			if(ir.type_name == obj.name && ir.property_name != in_rel.linked_as->property_name)
				is_only_of_type = false;
		}
		if(is_only_of_type) {
			if(add_prefix) {
				o + substitute{ "as_suffix", "" };
				many_getter_setter_text(o, in_rel.linked_as->ltype, !in_rel.linked_as->is_optional && in_rel.rel_ptr->store_type == storage_type::compactable, in_rel.linked_as->multiplicity > 1);
			} else {
				o + "template<typename T>";
				o + "DCON_RELEASE_INLINE void @namesp@for_each_@rel@(T&& func) const" + block{
					o + "container.@obj@_for_each_@rel@(id, [&, t = this](@rel@_id i){func(fatten(t->container, i));});";
				};
				if(in_rel.linked_as->ltype == list_type::array || in_rel.linked_as->ltype == list_type::std_vector) {
					o + "DCON_RELEASE_INLINE std::pair<@rel@_id const*, @rel@_id const*> @namesp@range_of_@rel@() const" + block{
						o + "return container.@obj@_range_of_@rel@(id);";
					};
				}
				if(!const_mode) {
					o + "DCON_RELEASE_INLINE void @namesp@remove_all_@rel@() const noexcept" + block{
						o + "container.@obj@_remove_all_@rel@(id);";
					};
					o + "DCON_RELEASE_INLINE internal::iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator @namesp@get_@rel@() const" + block{
						o + "return internal::iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator(container, id);";
					};
				} else {
					o + "DCON_RELEASE_INLINE internal::const_iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator @namesp@get_@rel@() const" + block{
						o + "return internal::const_iterator_@obj@_foreach_@rel@_as_@rel_prop@_generator(container, id);";
					};
				}
			}

			o + substitute{ "as_name", in_rel.linked_as->property_name };

			for(auto& ir : in_rel.rel_ptr->indexed_objects) {
				if(ir.related_to != &obj) {
					o + substitute{ "r_prop", ir.property_name };
					o + substitute{ "prop", ir.property_name };
					o + substitute{ "p_type", ir.type_name + "_id" };

					if(ir.protection != protection_type::hidden) {
						if(add_prefix) {
							o + substitute{ "ref", "" };
							relation_many_join_getters_setters_text(o, in_rel.linked_as->ltype, in_rel.rel_ptr->primary_key == ir);
						} else {
							o + "template<typename T>";
							o + "DCON_RELEASE_INLINE void @namesp@for_each_@r_prop@_from_@rel@(T&& func) const" + block{
								o + "container.@obj@_for_each_@r_prop@_from_@rel@(id, [&, t = this](@p_type@ i){func(fatten(t->container, i));});";
							};
							o + "DCON_RELEASE_INLINE bool @namesp@has_@r_prop@_from_@rel@(@p_type@ target) const" + block{
								o + "return container.@obj@_has_@r_prop@_from_@rel@(id, target);";
							};
						}
					}
				}
			}
			for(auto& ip : in_rel.rel_ptr->properties) {
				if(!ip.is_derived || ip.hook_get) {
					o + substitute{ "p_type", ip.data_type };
					o + substitute{ "prop", ip.name };

					auto upresult = to_fat_index_type(parsed_file, ip.data_type, const_mode);

					if(ip.protection != protection_type::hidden) {
						if(add_prefix) {
							o + substitute{ "ref", is_common_type(ip.data_type) ? "" : " const&" };
							relation_many_join_getters_setters_text(o, in_rel.linked_as->ltype, false);
						} else {
							if(upresult.has_value()) {
								o + "template<typename T>";
								o + "DCON_RELEASE_INLINE void @namesp@for_each_@prop@_from_@rel@(T&& func) const" + block{
									o + "container.@obj@_for_each_@prop@_from_@rel@(id, [&, t = this](@p_type@ i){func(fatten(t->container, i));});";
								};
							} else {
								o + "template<typename T>";
								o + "DCON_RELEASE_INLINE void @namesp@for_each_@prop@_from_@rel@(T&& func) const" + block{
									o + "container.@obj@_for_each_@prop@_from_@rel@(id, func);";
								};
							}

							o + substitute{ "ref", is_vectorizable_type(parsed_file, ip.data_type) ? "" : " const&" };

							o + "DCON_RELEASE_INLINE bool @namesp@has_@prop@_from_@rel@(@p_type@@ref@ target) const" + block{
								o + "return container.@obj@_has_@prop@_from_@rel@(id, target);";
							};
						}
					}
				}
			}
		}
	}

}

basic_builder& make_object_member_declarations(basic_builder& o, file_def const& parsed_file, relationship_object_def const& obj,
	bool add_prefix, bool declaration_mode, std::string const& namesp, bool const_mode) {

	o + substitute("obj", obj.name);
	o + substitute{ "namesp", namesp };
	o.declaration_mode = declaration_mode;

	if(add_prefix)
		o + heading{"Functions for @obj@:"};

	for(auto& prop : obj.properties) {
		make_property_member_declarations(o, parsed_file, obj,
			prop, add_prefix, namesp, const_mode);
	}
	for(auto& indexed : obj.indexed_objects) {
		make_link_member_declarations(o, parsed_file, obj,
			indexed, add_prefix, namesp, const_mode);
	}
	for(auto& involved_in : obj.relationships_involved_in) {
		make_related_member_declarations(o, parsed_file, obj,
			involved_in, add_prefix, namesp, const_mode);
	}

	// object globals
	
	if(add_prefix) {
		if(!obj.is_relationship) {
			if(obj.store_type != storage_type::erasable) {
				o + "DCON_RELEASE_INLINE bool @obj@_is_valid(@obj@_id id) const noexcept" + block{
					o + "return bool(id) && uint32_t(id.index()) < @obj@.size_used;";
				};
			} else {
				o + "DCON_RELEASE_INLINE bool @obj@_is_valid(@obj@_id id) const noexcept" + block{
					o + "return bool(id) && uint32_t(id.index()) < @obj@.size_used && @obj@.m__index.vptr()[id.index()] == id;";
				};
			}
		} else if(obj.primary_key.points_to) { // primary key relationship
			std::string temp = std::string("return bool(id) && uint32_t(id.index()) < @obj@.size_used && ") +
				"" + obj.primary_key.points_to->name + "_is_valid(" + obj.primary_key.points_to->name +
				"_id(" + obj.primary_key.points_to->name + "_id::value_base_t(id.index()))) && (";
			for(auto& iob : obj.indexed_objects) {
				if(obj.primary_key != iob) {
					if(iob.multiplicity == 1) {
						temp += "bool(@obj@.m_" + iob.property_name + ".vptr()[id.index()]) || ";
					} else {
						for(int32_t i = 0; i < iob.multiplicity; ++i) {
							o + substitute{ "mul", std::to_string(i) };
							temp += "bool(@obj@.m_" + iob.property_name + ".vptr()[id.index()][@mul@]) || ";
						}
					}
				}
			}
			temp += "false);";

			o + "DCON_RELEASE_INLINE bool @obj@_is_valid(@obj@_id id) const noexcept" + block{
					o + temp;
			};
		} else {
			if(obj.store_type == storage_type::contiguous || obj.store_type == storage_type::compactable) {
				o + "DCON_RELEASE_INLINE bool @obj@_is_valid(@obj@_id id) const noexcept" + block{
					o + "return bool(id) && uint32_t(id.index()) < @obj@.size_used;;";
				};
			} else if(obj.store_type == storage_type::erasable) {
				o + "DCON_RELEASE_INLINE bool @obj@_is_valid(@obj@_id id) const noexcept" + block{
					o + "return bool(id) && uint32_t(id.index()) < @obj@.size_used && @obj@.m__index.vptr()[id.index()] == id;";
				};
			}
		}
	} else {
		o + "DCON_RELEASE_INLINE bool @namesp@is_valid() const noexcept" + block{
			o + "return container.@obj@_is_valid(id);";
		};

		for(auto m : obj.member_functions) {
			if(m.is_const && !const_mode) {
				std::string all_params;
				for(auto& p : m.parameter_names) {
					if(all_params.length() > 0)
						all_params += ", ";
					all_params += p;
				}
				// pass through
				o + substitute{ "params", all_params };
				o + substitute{ "fn", m.name };

				auto name_pos = m.signature.find(m.name);
				std::string in_namsepace = "inline " + m.signature.substr(0, name_pos) + "@namesp@"
					+ m.signature.substr(name_pos);

				o + in_namsepace + block{
					o + "return @obj@_const_fat_id(container, id).@fn@(@params@);";
				};
			} else if(m.is_const == const_mode) {
				if(declaration_mode) {
					o + m.signature;
					o + append{ ";" };
				}
			}

		}
	}

	o + line_break();
	return o;
}