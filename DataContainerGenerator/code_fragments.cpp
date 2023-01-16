#include "code_fragments.hpp"
 
basic_builder& make_load_record(basic_builder& o, file_def const & fd) {
	o + "struct load_record" + class_block{
		for(auto& ro : fd.relationship_objects) {
			o + substitute("object", ro.name);
			o + "bool @object@ : 1;";
			if(ro.store_type == storage_type::erasable)
				o + "bool @object@__index : 1;";
			for(auto& io : ro.indexed_objects) {
				o + substitute("prop_name", io.property_name);
				o + "bool @object@_@prop_name@ : 1;";
			}
			for(auto& io : ro.properties) {
				o + substitute("prop_name", io.name);
				o + "bool @object@_@prop_name@ : 1;";
			}
		}
		o + "load_record()" + block{
			for(auto& ro : fd.relationship_objects) {
				o + substitute("object", ro.name);
				o + "@object@ = false;";
				if(ro.store_type == storage_type::erasable)
					o + "@object@__index = false;";
				for(auto& io : ro.indexed_objects) {
					o + substitute("prop_name", io.property_name);
					o + "@object@_@prop_name@ = false;";
				}
				for(auto& io : ro.properties) {
					o + substitute("prop_name", io.name);
					o + "@object@_@prop_name@ = false;";
				}
			}
		};
	};
	return o;
}

basic_builder& make_id_definition(basic_builder& o, std::string const& type_name, std::string const& underlying_type) {
	o + substitute("name", type_name ) + substitute("type", underlying_type);
	o + heading{"definition of strongly typed index for @name@"};
	o + "class @name@" + class_block{ o
		+ "public:"
		+ "using value_base_t = @type@;"
		+ "using zero_is_null_t = std::true_type;"
		+ line_break{}
		+"@type@ value = 0;"
		+ line_break{}
		+"constexpr @name@() noexcept = default;"
		+"explicit constexpr @name@(@type@ v) noexcept : value(v + 1) {}"
		+ "constexpr @name@(@name@ const& v) noexcept = default;"
		+ "constexpr @name@(@name@&& v) noexcept = default;"
		+ line_break{}
		+"@name@& operator=(@name@ const& v) noexcept = default;"
		+"@name@& operator=(@name@&& v) noexcept = default;"
		+"constexpr bool operator==(@name@ v) const noexcept { return value == v.value; }"
		+ "constexpr bool operator!=(@name@ v) const noexcept { return value != v.value; }"
		+ "explicit constexpr operator bool() const noexcept { return value != @type@(0); }"
		+ "constexpr DCON_RELEASE_INLINE int32_t index() const noexcept" + block{
			o + "return int32_t(value) - 1;";
		};
	} + line_break{}
	+"class @name@_pair" + class_block{ o
		+ "public:"
		+ "@name@ left;"
		+ "@name@ right;";
	} + line_break{}
	+ "DCON_RELEASE_INLINE bool is_valid_index(@name@ id) { return bool(id); }" + line_break{};

	return o;
}

basic_builder& make_value_to_vector_type(basic_builder& o, std::string const& qualified_name) {
	o + substitute("type", qualified_name)
		+ "template<>"
		+ "struct value_to_vector_type_s<@type@>" + class_block {
			o + "using type = tagged_vector<@type@>;";
		} + line_break{};
	return o;
}

basic_builder& make_member_container(basic_builder& o,
	std::string const& member_name, std::string const& type_name, std::string const& size,
	struct_padding pad, bool is_expandable, std::optional<std::string> const& special_fill, int32_t multiplicity) {

	o + substitute("type", type_name) + substitute("name", member_name) + substitute{"size", size}
	+ substitute{ "align", pad != struct_padding::none ? "alignas(64) " : "" }
	+ substitute{"fill", special_fill.has_value() ? *special_fill : std::string("")};
	o + substitute{ "mval", std::to_string(multiplicity) };

	o + heading{"storage space for @name@ of type @type@"};
	o + "struct @align@dtype_@name@" + block{
		if(is_expandable) {
			if(multiplicity == 1) {
				o + "std::vector<@type@> values;";
			} else {
				o + "std::vector<std::array<@type@, @mval@>> values;";
			}
			o + "DCON_RELEASE_INLINE auto vptr() const { return values.data() + 1; }";
			o + "DCON_RELEASE_INLINE auto vptr() { return values.data() + 1; }";
		} else {
			if(pad == struct_padding::fixed)
				o + "uint8_t padding[(63 + sizeof(@type@)) & ~uint64_t(63)];";
			if(multiplicity == 1) {
				o + "@type@ values[@size@];";
			} else {
				o + "std::array<@type@, @mval@> values[@size@];";
			}
			o + "DCON_RELEASE_INLINE auto vptr() const { return values; }";
			o +"DCON_RELEASE_INLINE auto vptr() { return values; }";;
		}
		
		if(!special_fill.has_value()) {
			if(is_expandable)
				o + "dtype_@name@() { values.emplace_back(); }";
			else if(pad == struct_padding::none)
				o + "dtype_@name@() { std::uninitialized_value_construct_n(values, @size@); }";
			else
				o + "dtype_@name@() { std::uninitialized_value_construct_n(values - 1, 1 + @size@); }";
		} else {
			if(is_expandable)
				o + "dtype_@name@() { values.push_back( @fill@ ); }";
			else if(pad == struct_padding::none)
				o + "dtype_@name@() { std::uninitialized_fill_n(values, @size@, @fill@); }";
			else
				o + "dtype_@name@() { std::uninitialized_fill_n(values - 1, 1 + @size@, @fill@); }";
		}
	} +"m_@name@;" + line_break{};

	return o;
}

basic_builder& make_array_member_container(basic_builder& o, 
	std::string const& member_name, std::string const& type_name, size_t raw_size,
	bool is_expandable, bool is_bitfield) {

	std::string size = is_bitfield ?
		std::string("( (") + std::to_string(raw_size) + " + 7) / 8" +
		" + uint64_t(64) - (( (" + std::to_string(raw_size) + " + 7) / 8) & uint64_t(63))"
		" )"
		:
		std::string("(sizeof(") + type_name + ") * " + std::to_string(raw_size) +
		 " + uint64_t(64) - ((sizeof(" + type_name + ") * " + std::to_string(raw_size) + ") & uint64_t(63))"
		" + ((sizeof(" + type_name + ") + uint64_t(63)) & ~uint64_t(63)))";
	// = size of all values in bytes + number of bytes to get the end to a multiple of 64 + 64 * number of cachelines
	// required to fit the type

	o + substitute("type", type_name) + substitute("name", member_name) + substitute{ "size", size };
	o + substitute{ "pad_value", std::string("((uint64_t(63) + sizeof(") + type_name + ")) & ~uint64_t(63))" };

	o + heading{ "storage space for @name@ of type array of @type@" };
	o + "struct dtype_@name@" + block{
		if(is_expandable) {
			o + "std::vector<std::vector<@type@>> values;";
			o + "uint32_t size = 0;";

			o + "DCON_RELEASE_INLINE auto vptr(int32_t i) const { return values[i].data() + 1; }";
			o + "DCON_RELEASE_INLINE auto vptr(int32_t i) { return values[i].data() + 1; }";
			o + "DCON_RELEASE_INLINE void resize(uint32_t sz, uint32_t container_size)" + block{
				o + "values.resize(sz);";
				if(!is_bitfield)
					o + "for(uint32_t i = size; i < sz; ++i) values[i].resize(container_size + 1);";
				else
					o + "for(uint32_t i = size; i < sz; ++i) values[i].resize((container_size + 7) / 8 + 1);";
				o + "size = sz;";
			};
			if(!is_bitfield) {
				o + "DCON_RELEASE_INLINE void emplace_back_all(uint32_t)" + block{
					o + "for(auto& v : values) v.emplace_back();";
				};
				o + "DCON_RELEASE_INLINE void copy_value(int32_t dest, int32_t source)" + block{
					o + "for(auto& v : values)" + block{
						o + "v[1 + dest] = v[1 + source];";
					};
				};
				o + "DCON_RELEASE_INLINE void pop_back_all(uint32_t)" + block{
					o + "for(auto& v : values)" + block{
						o + "v.pop_back();";
					};
				};
				o + "DCON_RELEASE_INLINE void zero_at(int32_t dest)" + block{
					o + "for(auto& v : values)" + block{
						o + "v[1 + dest] = @type@{};";
					};
				};
			} else {
				o + "DCON_RELEASE_INLINE void emplace_back_all(uint32_t newsz)" + block{
					o + "for(auto& v : values)" + block{
						o + "v.resize(1 + (newsz + 7) / 8);";
					};
				};
				o + "DCON_RELEASE_INLINE void copy_value(int32_t dest, int32_t source)" + block{
					o + "for(int32_t i = 0; i < int32_t(size); ++i)" + block{
						o + "dcon::bit_vector_set(vptr(i), dest, dcon::bit_vector_test(vptr(i), source));";
					};
				};
				o + "DCON_RELEASE_INLINE void pop_back_all(uint32_t vsize)" + block{
					o + "for(auto& v : values)" + block{
						o + "v.resize(1 + (vsize + 6) / 8);";
					};
				};
				o + "DCON_RELEASE_INLINE void zero_at(int32_t dest)" + block{
					o + "for(int32_t i = 0; i < int32_t(size); ++i)" + block{
						o + "dcon::bit_vector_set(vptr(i), dest, false);";
					};
				};
			}
		} else {
			o + "std::byte* values = nullptr;";
			o + "uint32_t size = 0;";

			o + "DCON_RELEASE_INLINE auto vptr(int32_t i) const" + block{
				o + "return reinterpret_cast<@type@ const*>(values + @pad_value@ + i * @size@);";
			};
			o + "DCON_RELEASE_INLINE auto vptr(int32_t i)" + block{
				o + "return reinterpret_cast<@type@*>(values + @pad_value@ + i * @size@);";
			};
			o + +"DCON_RELEASE_INLINE void resize(uint32_t sz, uint32_t)" + block{
				o + "std::byte* temp = (std::byte*)(::operator new(@pad_value@ + sz * @size@, std::align_val_t{ 64 }));";
				o + "if(sz > size)" + block {
					o + "if(values)" + block {
						o + "std::memcpy(temp, values, @pad_value@ + size * @size@);";
						o + "std::memset(temp + @pad_value@ + size * @size@, 0, (sz - size) * @size@);";
					} +append{"else"} + block{
						o + "std::memset(temp, 0, @pad_value@ + sz * @size@);";
					};
				} +append{ "else" } +block{
					o + "std::memcpy(temp, values, @pad_value@ + sz * @size@);";
				};
				o + "::operator delete(values, std::align_val_t{ 64 });";
				o + "values = temp;";
				o + "size = sz;";
			};
			o + "~dtype_@name@() { ::operator delete(values, std::align_val_t{ 64 }); }";
			if(!is_bitfield) {
				o + "DCON_RELEASE_INLINE void copy_value(int32_t dest, int32_t source)" + block{
					o + "for(int32_t i = 0; i < int32_t(size); ++i)" + block{
						o + "vptr(i)[dest] = vptr(i)[source];";
					};
				};
				o + "DCON_RELEASE_INLINE void zero_at(int32_t dest)" + block{
					o + "for(int32_t i = 0; i < int32_t(size); ++i)" + block{
						o + "vptr(i)[dest] = @type@{};";
					};
				};
			} else {
				o + "DCON_RELEASE_INLINE void copy_value(int32_t dest, int32_t source)" + block{
					o + "for(int32_t i = 0; i < int32_t(size); ++i)" + block{
						o + "dcon::bit_vector_set(vptr(i), dest, dcon::bit_vector_test(vptr(i), source));";
					};
				};
				o + "DCON_RELEASE_INLINE void zero_at(int32_t dest)" + block{
					o + "for(int32_t i = 0; i < int32_t(size); ++i)" + block{
						o + "dcon::bit_vector_set(vptr(i), dest, false);";
					};
				};
			}
		}
		
	} +"m_@name@;" + line_break{};

	return o;
}

std::string expand_size_to_fill_cacheline_calculation(std::string const& member_type, size_t base_size) {
	return std::string("(sizeof(") + member_type + ") <= 64 ? ("
		"uint32_t(" + std::to_string(base_size) + ") + (uint32_t(64) / uint32_t(sizeof(" + member_type + "))) - uint32_t(1)) "
		"& ~(uint32_t(64) / uint32_t(sizeof(" + member_type + ")) - uint32_t(1)) : uint32_t(" + std::to_string(base_size) + "))";
}

basic_builder& make_erasable_object_constructor(basic_builder& o, std::string const& name, size_t size) {
	o + substitute{ "name", name } +substitute{ "size", std::to_string(size) } +substitute{"u_type", size_to_tag_type(size) }
	+"@name@_class()" + block{
		o + "for(int32_t i = @size@ - 1; i >= 0; --i)" + block{
			o + "m__index.vptr()[i] = first_free;";
			o + "first_free = @name@_id(@u_type@(i));";
		};
	};
	return o;
}


basic_builder& clear_value(basic_builder& o, std::string const& object_name, std::string const& property_name,
	std::string const& property_type_val, property_type type, std::string const& at, bool is_multiple) {

	o + substitute{ "t_obj", object_name } + substitute{ "t_prop", property_name } + substitute{ "t_at", at }
		+substitute{ "t_type", property_type_val };

	if(is_multiple) {
		o + "@t_obj@.m_@t_prop@.vptr()[@t_at@.index()].fill(@t_type@{});";
	} else if(type == property_type::special_vector) {
		o + "@t_obj@.@t_prop@_storage.release(@t_obj@.m_@t_prop@.vptr()[@t_at@.index()]);";
	} else if(type == property_type::bitfield) {
		o + "dcon::bit_vector_set(@t_obj@.m_@t_prop@.vptr(), @t_at@.index(), false);";
	} else if(type == property_type::array_other || type == property_type::array_vectorizable || type == property_type::array_bitfield) {
		o + "@t_obj@.m_@t_prop@.zero_at(@t_at@.index());";
	} else {
		o + "@t_obj@.m_@t_prop@.vptr()[@t_at@.index()] = @t_type@{};";
	}

	return o;
}

basic_builder& pop_value(basic_builder& o, std::string const& object_name, std::string const& property_name,
	property_type type, std::string const& at, bool array_name = false) {

	o + substitute{ "t_obj", object_name } +substitute{ "t_prop", property_name } +substitute{ "t_at", at };

	if(type == property_type::special_vector) {
		if(array_name) {
			o + "@t_obj@.@t_prop@_storage.release(@t_obj@.m_array_@t_prop@.vptr()[@t_at@.index()]);";
			o + "@t_obj@.m_array_@t_prop@.values.pop_back();";
		} else {
			o + "@t_obj@.@t_prop@_storage.release(@t_obj@.m_@t_prop@.vptr()[@t_at@.index()]);";
			o + "@t_obj@.m_@t_prop@.values.pop_back();";
		}
	} else if(type == property_type::bitfield) {
		o + "dcon::bit_vector_set(@t_obj@.m_@t_prop@.vptr(), @t_at@.index(), false);";
		o + "@t_obj@.m_@t_prop@.values.resize(1 + (@t_obj@.size_used + 6) / 8);";
	} else if(type == property_type::array_other || type == property_type::array_vectorizable || type == property_type::array_bitfield) {
		o + "@t_obj@.m_@t_prop@.pop_back_all(@t_obj@.size_used);";
	} else {
		o + "@t_obj@.m_@t_prop@.values.pop_back();";
	}

	return o;
}

basic_builder& make_pop_back(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name };

	o + heading{ "container pop_back for @obj@" };

	o + "void pop_back_@obj@()" + block{
		o + "if(@obj@.size_used == 0) return;";
		o + "@obj@_id id_removed(@obj@_id::value_base_t(@obj@.size_used - 1));";

		if(cob.hook_delete)
			o + "on_delete_@obj@(id_removed);";

		for(auto& ck : cob.composite_indexes) {
			std::string params;
			for(auto& idx : ck.component_indexes) {
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == cob.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(id_removed.index()))";
				else
					params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
			}
			o + substitute{ "params", params } +substitute{ "ckname", ck.name };
			o + "@obj@.hashm_@ckname@.erase( @obj@.to_@ckname@_keydata(@params@) );";
		}

		for(auto& iob : cob.indexed_objects) {
			o + substitute{ "i_prop_name", iob.property_name } +substitute{ "i_type_name", iob.type_name };
			if(cob.primary_key != iob) {
				if(iob.multiplicity == 1) {
					o + "internal_@obj@_set_@i_prop_name@(id_removed, @i_type_name@_id());";
					if(cob.is_expandable) {
						o + "@obj@.m_@i_prop_name@.values.pop_back();";
						if(iob.ltype == list_type::list)
							o + "@obj@.m_link_@i_prop_name@.values.pop_back();";
					}
				} else {
					for(int32_t i = 0; i < iob.multiplicity; ++i) {
						o + substitute{ "i", std::to_string(i)};

						o + "internal_@obj@_set_@i_prop_name@(id_removed, @i@, @i_type_name@_id());";
						if(cob.is_expandable) {
							o + "@obj@.m_@i_prop_name@.values.pop_back();";
							if(iob.ltype == list_type::list)
								o + "@obj@.m_link_@i_prop_name@.values.pop_back();";
						}
					}
				}
			}
		}

		for(auto& cr : cob.relationships_involved_in) {
			o + substitute{ "rel_name", cr.relation_name } +substitute{"rel_prop_name", cr.linked_as->property_name };
			if(cr.linked_as->is_primary_key) {
				if(cob.is_expandable) {
					o + "pop_back_@rel_name@();";
				} else {
					o + "delete_@rel_name@(@rel_name@_id(@rel_name@_id::value_base_t(id_removed.index())));";
					o + "@rel_name@.size_used = @obj@.size_used - 1;";
				}
			} else if(cr.linked_as->index == index_type::at_most_one) {
				o + "@obj@_remove_@rel_name@_as_@rel_prop_name@(id_removed);";
				if(cob.is_expandable) {
					pop_value(o, cr.relation_name, std::string("link_back_") + cr.linked_as->property_name,
						property_type::other, "id_removed");
				}
			} else if(cr.linked_as->index == index_type::many) {
				o + "@obj@_remove_all_@rel_name@_as_@rel_prop_name@(id_removed);";
				if(cob.is_expandable) {
					if(cr.linked_as->ltype == list_type::list) {
						pop_value(o, cr.relation_name, std::string("head_back_") + cr.linked_as->property_name,
							property_type::other, "id_removed");
					} else if(cr.linked_as->ltype == list_type::array) {
						pop_value(o, cr.relation_name, cr.linked_as->property_name,
							property_type::special_vector, "id_removed", true);
					} else if(cr.linked_as->ltype == list_type::std_vector) {
						pop_value(o, cr.relation_name, std::string("array_") + cr.linked_as->property_name,
							property_type::other, "id_removed");
					}
				}
			}
		}
		if(cob.is_expandable) {
			for(auto& cp : cob.properties) {
				if(!cp.is_derived) {
					pop_value(o, cob.name, cp.name, cp.type, "id_removed");
				}
			}
		} else {
			for(auto& cp : cob.properties) {
				if(!cp.is_derived) {
					clear_value(o, cob.name, cp.name, cp.data_type, cp.type, "id_removed", false);
				}
			}
		}
		o + "--@obj@.size_used;";
	};


	o + line_break{};
	return o;
}

basic_builder& clear_value_range(basic_builder& o, std::string const& object_name, std::string const& property_name,
	std::string const& property_type_val, property_type type, std::string const& start, std::string const& count,
	bool array_name = false) {

	o + substitute{ "t_obj", object_name } +substitute{ "t_prop", property_name } +substitute{ "begin", start }
	+substitute{ "t_type", property_type_val } +substitute{ "count", count };

	if(type == property_type::special_vector) {
		if(array_name) {
			o + "std::for_each(@t_obj@.m_array_@t_prop@.vptr() + @begin@, @t_obj@.m_array_@t_prop@.vptr() + @begin@ + @count@, "
				"[t = this](dcon::stable_mk_2_tag& i){ t->@t_obj@.@t_prop@_storage.release(i); });";
		} else {
			o + "std::for_each(@t_obj@.m_@t_prop@.vptr() + @begin@, @t_obj@.m_@t_prop@.vptr() + @begin@ + @count@, "
				"[t = this](dcon::stable_mk_2_tag& i){ t->@t_obj@.@t_prop@_storage.release(i); });";
		}
	} else if(type == property_type::bitfield) {
		o + "for(uint32_t i = @begin@; i < 8 * (((@begin@ + 7) / 8)); ++i)" + block{
			o + "dcon::bit_vector_set(@t_obj@.m_@t_prop@.vptr(), i, false);";
		};
		o + "std::fill_n(@t_obj@.m_@t_prop@.vptr() + (@begin@ + 7) / 8, (@begin@ + @count@ + 7) / 8 - (@begin@ + 7) / 8, dcon::bitfield_type{0});";
	} else if(type == property_type::object) {
		o + "std::destroy_n(@t_obj@.m_@t_prop@.vptr() + @begin@, @count@);";
		o + "std::uninitialized_default_construct_n(@t_obj@.m_@t_prop@.vptr() + @begin@, @count@);";
	} else if(type == property_type::array_other || type == property_type::array_vectorizable) {
		o + "for(int32_t s = 0; s < int32_t(@t_obj@.m_@t_prop@.size); ++s)" + block{
			o + "std::fill_n(@t_obj@.m_@t_prop@.vptr(s) + @begin@, @count@, @t_type@{});";
		};
	} else if(type == property_type::array_bitfield) {
		o + "for(int32_t s = 0; s < int32_t(@t_obj@.m_@t_prop@.size); ++s)" + block{
			o + "for(uint32_t i = @begin@; i < 8 * (((@begin@ + 7) / 8)); ++i)" + block{
				o + "dcon::bit_vector_set(@t_obj@.m_@t_prop@.vptr(s), i, false);";
			};
			o + "std::fill_n(@t_obj@.m_@t_prop@.vptr(s) + (@begin@ + 7) / 8, (@begin@ + @count@ + 7) / 8 - (@begin@ + 7) / 8, dcon::bitfield_type{0});";
		};
	} else {
		o + "std::fill_n(@t_obj@.m_@t_prop@.vptr() + @begin@, @count@, @t_type@{});";
	}
	return o;
}
basic_builder& shrink_value_range(basic_builder& o, std::string const& object_name, std::string const& property_name,
	property_type type, std::string const& start, std::string const& count, bool array_name = false) {

	o + substitute{ "t_obj", object_name } +substitute{ "t_prop", property_name } +substitute{ "begin", start }
	+substitute{ "count", count };

	if(type == property_type::special_vector) {
		if(array_name) {
			o + "std::for_each(@t_obj@.m_array_@t_prop@.vptr() + @begin@, @t_obj@.m_array_@t_prop@.vptr() + @begin@ + @count@, "
				"[t = this](dcon::stable_mk_2_tag& i){ t->@t_obj@.@t_prop@_storage.release(i); });";
			o + "@t_obj@.m_array_@t_prop@.values.resize(1 + @begin@);";
		} else {
			o + "std::for_each(@t_obj@.m_@t_prop@.vptr() + @begin@, @t_obj@.m_@t_prop@.vptr() + @begin@ + @count@, "
				"[t = this](dcon::stable_mk_2_tag& i){ t->@t_obj@.@t_prop@_storage.release(i); });";
			o + "@t_obj@.m_@t_prop@.values.resize(1 + @begin@);";
		}
	} else if(type == property_type::bitfield) {
		o + "for(uint32_t i = @begin@; i < 8 * (((@begin@ + 7) / 8)); ++i)" + block{
			o + "dcon::bit_vector_set(@t_obj@.m_@t_prop@.vptr(), i, false);";
		};
		o + "@t_obj@.m_@t_prop@.values.resize(1 + (@begin@ + 7) / 8);";
	} else if(type == property_type::array_other || type == property_type::array_vectorizable) {
		o + "for(int32_t s = 0; s < int32_t(@t_obj@.m_@t_prop@.size); ++s)" + block{
			o + "@t_obj@.m_@t_prop@.values[s].resize(1 + @begin@);";
		};
	} else if(type == property_type::array_bitfield) {
		o + "for(int32_t s = 0; s < int32_t(@t_obj@.m_@t_prop@.size); ++s)" + block{
			o + "for(uint32_t i = @begin@; i < 8 * (((@begin@ + 7) / 8)); ++i)" + block{
				o + "dcon::bit_vector_set(@t_obj@.m_@t_prop@.vptr(s), i, false);";
			};
			o + "@t_obj@.m_@t_prop@.values[s].resize(1 + (@begin@ + 7) / 8);";
		};
	}  else {
		o + "@t_obj@.m_@t_prop@.values.resize(1 + @begin@);";
	}
	return o;
}

basic_builder& grow_value_range(basic_builder& o, std::string const& object_name, std::string const& property_name,
	property_type type, std::string const& size, bool array_name = false) {

	o + substitute{ "t_obj", object_name } +substitute{ "t_prop", property_name } +substitute{ "size", size };

	if(type == property_type::special_vector) {
		if(array_name) {
			o + "@t_obj@.m_array_@t_prop@.values.resize(1 + @size@, std::numeric_limits<dcon::stable_mk_2_tag>::max());";
		} else {
			o + "@t_obj@.m_@t_prop@.values.resize(1 + @size@, std::numeric_limits<dcon::stable_mk_2_tag>::max());";
		}
	} else if(type == property_type::bitfield) {
		o + "@t_obj@.m_@t_prop@.values.resize(1 + (@size@ + 7) / 8);";
	} else if(type == property_type::array_other || type == property_type::array_vectorizable) {
		o + "for(int32_t s = 0; s < int32_t(@t_obj@.m_@t_prop@.size); ++s)" + block{
			o + "@t_obj@.m_@t_prop@.values[s].resize(1 + @size@);";
		};
	} else if(type == property_type::array_bitfield) {
		o + "for(int32_t s = 0; s < int32_t(@t_obj@.m_@t_prop@.size); ++s)" + block{
			o + "@t_obj@.m_@t_prop@.values[s].resize(1 + (@size@ + 7) / 8);";
		};
	} else {
		o + "@t_obj@.m_@t_prop@.values.resize(1 + @size@);";
	}
	return o;
}

basic_builder& make_object_resize(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name } +substitute{ "obj_sz", std::to_string(cob.size) };
	o + heading{ "container resize for @obj@" };

	o + "void @obj@_resize(uint32_t new_size)" + block{
		if(!cob.is_expandable) {
			o + "#ifndef DCON_USE_EXCEPTIONS";
			o + "if(new_size > @obj_sz@) std::abort();";
			o + "#else";
			o + "if(new_size > @obj_sz@) throw dcon::out_of_space{};";
			o + "#endif";
		}
		o + "const uint32_t old_size = @obj@.size_used;";
		o + "if(new_size < old_size)" + block{ // contracting
			if(cob.store_type == storage_type::erasable) {
				o + "@obj@.first_free = @obj@_id();";
				if(cob.is_expandable) {
					o + "@obj@.m__index.values.resize(new_size + 1);";
					o + "int32_t i = int32_t(new_size - 1);";
				} else {
					o + "int32_t i = int32_t(@obj_sz@ - 1);";
					o + "for(; i >= int32_t(new_size); --i)" + block{
						o + "@obj@.m__index.vptr()[i] = @obj@.first_free;";
						o + "@obj@.first_free = @obj@_id(@obj@_id::value_base_t(i));";
					};
				}
				o + "for(; i >= 0; --i)" + block{
					o + "if(@obj@.m__index.vptr()[i] != @obj@_id(@obj@_id::value_base_t(i)))" + block{
						o + "@obj@.m__index.vptr()[i] = @obj@.first_free;";
						o + "@obj@.first_free = @obj@_id(@obj@_id::value_base_t(i));";
					};
				};
			}
			for(auto& ck : cob.composite_indexes) {
				o + substitute{ "ckname", ck.name };
				o + "@obj@.hashm_@ckname@.clear();";
			}
			for(auto& io : cob.indexed_objects) {
				if(cob.primary_key != io) {
					if(!cob.is_expandable) {
						if(io.multiplicity == 1) {
							clear_value_range(o, cob.name, io.property_name, io.type_name + "_id", property_type::other,
								"0", "old_size");
						} else {
							clear_value_range(o, cob.name, io.property_name, io.type_name + "_id", property_type::object,
								"0", "old_size");
						}
					} else {
						if(io.multiplicity == 1) {
							shrink_value_range(o, cob.name, io.property_name, property_type::other, "new_size", "old_size - new_size");
							clear_value_range(o, cob.name, io.property_name, io.type_name + "_id", property_type::other,
								"0", "new_size");
						} else {
							shrink_value_range(o, cob.name, io.property_name, property_type::object, "new_size", "old_size - new_size");
							clear_value_range(o, cob.name, io.property_name, io.type_name + "_id", property_type::object,
								"0", "new_size");
						}
					}

					if(io.index == index_type::at_most_one) {
						clear_value_range(o, cob.name, std::string("link_back_") + io.property_name, cob.name + "_id",
							property_type::other, "0", io.type_name + ".size_used");
					} else if(io.index == index_type::many) {
						if(io.ltype == list_type::list) {
							if(!cob.is_expandable) {
								clear_value_range(o, cob.name, std::string("link_") + io.property_name,
									cob.name + "_id_pair", property_type::other, "0", "old_size");
							} else {
								shrink_value_range(o, cob.name, std::string("link_") + io.property_name,
									property_type::other, "new_size", "old_size - new_size");
								clear_value_range(o, cob.name, std::string("link_") + io.property_name,
									cob.name + "_id_pair", property_type::other, "0", "new_size");
							}
							clear_value_range(o, cob.name, std::string("head_back_") + io.property_name, cob.name + "_id",
								property_type::other, "0", io.type_name + ".size_used");
						} else if(io.ltype == list_type::array) {
							clear_value_range(o, cob.name, io.property_name, "",
								property_type::special_vector, "0", io.type_name + ".size_used", true);
						} else if(io.ltype == list_type::std_vector) {
							clear_value_range(o, cob.name, std::string("array_") + io.property_name, std::string("std::vector<") + cob.name + "_id>",
								property_type::other, "0", io.type_name + ".size_used");
						}
					}
				}
			}
			for(auto& cp : cob.properties) {
				if(!cp.is_derived) {
					if(!cob.is_expandable)
						clear_value_range(o, cob.name, cp.name, cp.data_type, cp.type, "new_size", "old_size - new_size");
					else
						shrink_value_range(o, cob.name, cp.name, cp.type, "new_size", "old_size - new_size");
				}
			}
			for(auto& cr : cob.relationships_involved_in) {
				if(cr.linked_as->is_primary_key) {
					o + (cr.relation_name + "_resize(std::min(new_size, " + cr.relation_name + ".size_used));");
				} else if(cr.linked_as->index == index_type::at_most_one) {
					o + (cr.relation_name + "_resize(0);");
				} else if(cr.linked_as->index == index_type::many) {
					o + (cr.relation_name + "_resize(0);");
				}
			}
		} + append{ "else if(new_size > old_size)" } + block{ // expanding
			if(cob.store_type == storage_type::erasable) {
				o + "@obj@.first_free = @obj@_id();";
				if(cob.is_expandable) {
					o + "@obj@.m__index.values.resize(new_size + 1);";
					o + "int32_t i = int32_t(new_size - 1);";
				} else {
					o + "int32_t i = int32_t(@obj_sz@ - 1);";
					o + "for(; i >= int32_t(old_size); --i)" + block{
						o + "@obj@.m__index.vptr()[i] = @obj@.first_free;";
						o + "@obj@.first_free = @obj@_id(@obj@_id::value_base_t(i));";
					};
				}
				o + "for(; i >= 0; --i)" + block{
					o + "if(@obj@.m__index.vptr()[i] != @obj@_id(@obj@_id::value_base_t(i)))" + block{
						o + "@obj@.m__index.vptr()[i] = @obj@.first_free;";
						o + "@obj@.first_free = @obj@_id(@obj@_id::value_base_t(i));";
					};
				};
			}

			if(cob.is_expandable) {
				for(auto& cp : cob.properties) {
					if(!cp.is_derived) {
						grow_value_range(o, cob.name, cp.name, cp.type, "new_size");
					}
				}
				for(auto& io : cob.indexed_objects) {
					if(cob.primary_key != io) {
						grow_value_range(o, cob.name, io.property_name, property_type::other, "new_size");
					}
					if(io.index == index_type::many && io.ltype == list_type::list) {
						grow_value_range(o, cob.name, std::string("link_") + io.property_name, property_type::other, "new_size");
					}
				}

				for(auto& cr : cob.relationships_involved_in) {
					if(cr.linked_as->is_primary_key) {

					} else if(cr.linked_as->index == index_type::at_most_one) {
						grow_value_range(o, cr.relation_name, std::string("link_back_") + cr.linked_as->property_name,
							property_type::other, "new_size");
					} else if(cr.linked_as->index == index_type::many) {
						if(cr.linked_as->ltype == list_type::list) {
							grow_value_range(o, cr.relation_name, std::string("head_back_") + cr.linked_as->property_name,
								property_type::other, "new_size");
						} else if(cr.linked_as->ltype == list_type::array) {
							grow_value_range(o, cr.relation_name, cr.linked_as->property_name,
								property_type::special_vector, "new_size", true);
						} else if(cr.linked_as->ltype == list_type::std_vector) {
							grow_value_range(o, cr.relation_name, std::string("array_") + cr.linked_as->property_name,
								property_type::object, "new_size");
						}
					}
				}
			}
			
		}; // end expanding

		o + "@obj@.size_used = new_size;";
	};
	o + line_break{};
	return o;
}

basic_builder& move_value_from_back(basic_builder& o, std::string const& object_name, std::string const& property_name,
	std::string const& source, std::string const& destination, std::string const& null_value, bool is_expandable, int32_t multiplicity) {

	o + substitute{ "t_obj", object_name } +substitute{ "t_prop", property_name }
		+substitute{ "t_source", source } +substitute{ "t_dest", destination }
	+substitute{ "t_null", null_value };

	if(multiplicity > 1) {
		for(int32_t i = 0; i < multiplicity; ++i) {
			o + substitute{ "i", std::to_string(i) };
			o + "@t_obj@.m_@t_prop@.vptr()[@t_dest@.index()][@i@] = @t_obj@.m_@t_prop@.vptr()[@t_source@.index()][@i@];";
		}
	} else {
		o + "@t_obj@.m_@t_prop@.vptr()[@t_dest@.index()] = std::move(@t_obj@.m_@t_prop@.vptr()[@t_source@.index()]);";
	}
	if(is_expandable) {
		o + "@t_obj@.m_@t_prop@.values.pop_back();";
	} else {
		o + "@t_obj@.m_@t_prop@.vptr()[@t_source@.index()] = @t_null@;";
	}

	return o;
}

basic_builder& move_array_value_from_back(basic_builder& o, std::string const& object_name, std::string const& property_name,
	std::string const& source, std::string const& destination, bool is_expandable) {

	o + substitute{ "t_obj", object_name } +substitute{ "t_prop", property_name }
	+substitute{ "t_source", source } +substitute{ "t_dest", destination };

	o + "@t_obj@.m_@t_prop@.copy_value(@t_dest@.index(), @t_source@.index());";
	if(is_expandable) {
		o + "@t_obj@.m_@t_prop@.pop_back_all(@t_obj@.size_used);";
	} else {
		o + "@t_obj@.m_@t_prop@.zero_at(@t_source@.index());";
	}

	return o;
}

basic_builder& move_value(basic_builder& o, std::string const& object_name, std::string const& property_name,
	std::string const& source, std::string const& destination, std::string const& null_value, int32_t multiplicity) {

	o + substitute{ "t_obj", object_name } +substitute{ "t_prop", property_name }
		+substitute{ "t_source", source } +substitute{ "t_dest", destination }
	+substitute{ "t_null", null_value };

	if(multiplicity == 1) {
		o + "@t_obj@.m_@t_prop@.vptr()[@t_dest@.index()] = std::move(@t_obj@.m_@t_prop@.vptr()[@t_source@.index()]);";
		o + "@t_obj@.m_@t_prop@.vptr()[@t_source@.index()] = @t_null@;";
	} else {
		for(int32_t i = 0; i < multiplicity; ++i) {
			o + substitute{ "i", std::to_string(i) };
			o + "@t_obj@.m_@t_prop@.vptr()[@t_dest@.index()][@i@] = @t_obj@.m_@t_prop@.vptr()[@t_source@.index()][@i@];";
			o + "@t_obj@.m_@t_prop@.vptr()[@t_source@.index()][@i@] = @t_null@;";
		}
	}
	
	return o;
}

basic_builder& move_array_value(basic_builder& o, std::string const& object_name, std::string const& property_name,
	std::string const& source, std::string const& destination) {

	o + substitute{ "t_obj", object_name } +substitute{ "t_prop", property_name }
	+substitute{ "t_source", source } +substitute{ "t_dest", destination };

	o + "@t_obj@.m_@t_prop@.copy_value(@t_dest@.index(), @t_source@.index());";
	o + "@t_obj@.m_@t_prop@.zero_at(@t_source@.index());";

	return o;
}


void make_composite_key_internal_assign(basic_builder& o, relationship_object_def const& in_obj, related_object const& link_to_update, std::string const& target_id) {
	if(link_to_update.is_covered_by_composite_key) {
		for(auto& cc : in_obj.composite_indexes) {
			std::string params;
			bool needs_adjust = false;

			for(auto& idx : cc.component_indexes) {
				if(idx.property_name == link_to_update.property_name) {
					needs_adjust = true;
				}
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == in_obj.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(" + target_id + ".index()))";
				else
					params += in_obj.name + ".m_" + idx.property_name + ".vptr()[" + target_id + ".index()]";
			}
			if(needs_adjust) {
				o + substitute{ "params", params } +substitute{ "ckname", cc.name };
				o + substitute{ "target_id", target_id };
				o + substitute{ "index_in", in_obj.name };
				o + "@index_in@.hashm_@ckname@.insert_or_assign(@index_in@.to_@ckname@_keydata(@params@), @target_id@);";
			}
		}
	}
}

void make_composite_key_internal_delete(basic_builder& o, relationship_object_def const& in_obj, related_object const& link_to_update, std::string const& target_id) {
	if(link_to_update.is_covered_by_composite_key) {
		for(auto& cc : in_obj.composite_indexes) {
			std::string params;
			bool needs_adjust = false;

			for(auto& idx : cc.component_indexes) {
				if(idx.property_name == link_to_update.property_name) {
					needs_adjust = true;
				}
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == in_obj.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(" + target_id + ".index()))";
				else
					params += in_obj.name + ".m_" + idx.property_name + ".vptr()[" + target_id + ".index()]";
			}
			if(needs_adjust) {
				o + substitute{ "params", params } +substitute{ "ckname", cc.name };
				o + substitute{ "target_id", target_id };
				o + substitute{ "index_in", in_obj.name };
				o + "@index_in@.hashm_@ckname@.erase(@index_in@.to_@ckname@_keydata(@params@));";
			}
		}
	}
}

basic_builder& make_compactable_delete(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name };
	o + heading{ "container compactable delete for @obj@" };

	o + "void delete_@obj@(@obj@_id id)" + block{ 
		o + "@obj@_id id_removed = id;";
		o + "@obj@_id last_id(@obj@_id::value_base_t(@obj@.size_used - 1));";
		o + "if(id_removed == last_id) { pop_back_@obj@(); return; }";
		if(cob.hook_delete)
			o + "on_delete_@obj@(id_removed);";

		for(auto& ck : cob.composite_indexes) {
			std::string params;
			for(auto& idx : ck.component_indexes) {
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == cob.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(id_removed.index()))";
				else
					params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
			}
			o + substitute{ "params", params } +substitute{ "ckname", ck.name };
			o + "@obj@.hashm_@ckname@.erase( @obj@.to_@ckname@_keydata(@params@) );";
		}

		for(auto& iob : cob.indexed_objects) {
			o + substitute{ "i_prop", iob.property_name } +substitute{ "i_type_name", iob.type_name };

			if(iob.multiplicity == 1) {
				o + "internal_@obj@_set_@i_prop@(id_removed, @i_type_name@_id());";
			} else {
				for(int32_t i = 0; i < iob.multiplicity; ++i) {
					o + substitute{ "i", std::to_string(i) };
					o + "internal_@obj@_set_@i_prop@(id_removed, @i@, @i_type_name@_id());";
				}
			}

			if(iob.index == index_type::at_most_one) {
				if(iob.multiplicity == 1) {
					o + "if(auto related = @obj@.m_@i_prop@.vptr()[last_id.index()]; bool(related))" + block{
						o + "@obj@.m_link_back_@i_prop@.vptr()[related.index()] = id_removed;";
					};
				} else {
					for(int32_t i = 0; i < iob.multiplicity; ++i) {
						o + substitute{ "i", std::to_string(i) };
						o + "if(auto related = @obj@.m_@i_prop@.vptr()[last_id.index()][@i@]; bool(related))" + block{
							o + "@obj@.m_link_back_@i_prop@.vptr()[related.index()] = id_removed;";
						};
					}
				}
			} else if(iob.index == index_type::many) {
				if(iob.ltype == list_type::list) {
					o + "if(bool(@obj@.m_@i_prop@.vptr()[last_id.index()]))" + block{
						o + "auto tmp = @obj@.m_link_@i_prop@.vptr()[last_id.index()];";
						o + "if(bool(tmp.left))" + block{
							o + "@obj@.m_link_@i_prop@.vptr()[tmp.left.index()].right = id_removed;";
						} +append{ "else" } +block{
							o + "for(auto lpos = last_id; bool(lpos); lpos = @obj@.m_link_@i_prop@.vptr()[lpos.index()].right)" + block{
								o + "@obj@.m_head_back_@i_prop@.vptr()[@obj@.m_@i_prop@.vptr()[lpos.index()].index()] = id_removed;";
							};
						};
						o + "if(bool(tmp.right))" + block{
							o + "@obj@.m_link_@i_prop@.vptr()[tmp.right.index()].left = id_removed;";
						};
					};
					move_value_from_back(o, cob.name, std::string("link_") + iob.property_name,
						"last_id", "id_removed", cob.name + "_id_pair()", cob.is_expandable, 1);
				} else if(iob.ltype == list_type::std_vector) {
					if(iob.multiplicity == 1) {
						o + "if(auto tmp = @obj@.m_@i_prop@.vptr()[last_id.index()]; bool(tmp))" + block{
							o + "auto& vref = @obj@.m_array_@i_prop@.vptr()[tmp.index()];";
							o + "if(auto pos = std::find(vref.begin(), vref.end(), last_id); pos != range.second)" + block {
								o + "*pos = id_removed;";
							};
						};
					} else {
						for(int32_t i = 0; i < iob.multiplicity; ++i) {
							o + substitute{ "i", std::to_string(i) };
							o + "if(auto tmp = @obj@.m_@i_prop@.vptr()[last_id.index()][@i@]; bool(tmp))" + block{
								o + "auto& vref = @obj@.m_array_@i_prop@.vptr()[tmp.index()];";
								if(iob.is_distinct) {
									o + "if(auto pos = std::find(vref.begin(), vref.end(), last_id); pos != range.second)" + block {
										o + "*pos = id_removed;";
									};
								} else {
									o + "for(auto pos = std::find(vref.begin(), vref.end(), last_id); pos != range.second; pos = std::find(pos + 1, vref.end(), last_id))" + block{
										o + "*pos = id_removed;";
									};
								}
							};
						}
					}
				} else if(iob.ltype == list_type::array) {
					if(iob.multiplicity == 1) {
						o + "if(auto tmp = @obj@.m_@i_prop@.vptr()[last_id.index()]; bool(tmp))" + block{
							o + "dcon::replace_unique_item(@obj@.@i_prop@_storage, @obj@.m_array_@i_prop@.vptr()[tmp.index()],"
								" last_id, id_removed);";
						};
					} else {
						for(int32_t i = 0; i < iob.multiplicity; ++i) {
							o + substitute{ "i", std::to_string(i) };
							o + "if(auto tmp = @obj@.m_@i_prop@.vptr()[last_id.index()][@i@]; bool(tmp))" + block{
								if(iob.is_distinct) {
									o + "dcon::replace_unique_item(@obj@.@i_prop@_storage, @obj@.m_array_@i_prop@.vptr()[tmp.index()],"
										" last_id, id_removed);";
								} else {
									o + "dcon::replace_all_items(@obj@.@i_prop@_storage, @obj@.m_array_@i_prop@.vptr()[tmp.index()],"
										" last_id, id_removed);";
								}
							};
						}
					}
				}
			}

			move_value_from_back(o, cob.name, iob.property_name,
				"last_id", "id_removed", iob.type_name + "_id()", cob.is_expandable, iob.multiplicity);
		}
		for(auto& cr : cob.relationships_involved_in) {
			o + substitute{ "rel", cr.relation_name } +substitute{"r_prop", cr.linked_as->property_name };
			if(cr.linked_as->is_primary_key) {
				o + "delete_@rel@(@rel@_id(@rel@_id::value_base_t(id_removed.index())));";
				o + "internal_move_relationship_@rel@(@rel@_id(@rel@_id::value_base_t(last_id.index())), @rel@_id(@rel@_id::value_base_t(id_removed.index())));";
				if(cob.is_expandable) {
					o + "pop_back_@rel@();";
				} else {
					o + "@rel@.size_used = @obj@.size_used - 1;";
				}
			} else if(cr.linked_as->index == index_type::at_most_one) {
				o + "@obj@_remove_@rel@_as_@r_prop@(id_removed);";
				o + "if(auto bk = @rel@.m_link_back_@r_prop@.vptr()[last_id.index()]; bool(bk))" + block{
					if(cr.linked_as->is_covered_by_composite_key)
						make_composite_key_internal_delete(o, *cr.rel_ptr, *cr.linked_as, "bk");
					if(cr.linked_as->multiplicity > 1) {
						for(int32_t j = 0; j < cr.linked_as->multiplicity; ++j) {
							o + substitute{ "i", std::to_string(j) };
							o + "if(@rel@.m_@r_prop@.vptr()[bk.index()][@i@] == last_id) @rel@.m_@r_prop@.vptr()[bk.index()][@i@] = id_removed;";
						}
						if(cr.linked_as->is_covered_by_composite_key) {
							o + "std::sort(@rel@.m_@r_prop@.vptr()[bk.index()].begin(), @rel@.m_@r_prop@.vptr()[bk.index()].end(), [](@obj@_id l, @obj@_id r){ return l.value < r.value; });";
						}
					} else {
						o + "@rel@.m_@r_prop@.vptr()[bk.index()] = id_removed;";
					}
					if(cr.linked_as->is_covered_by_composite_key)
						make_composite_key_internal_assign(o, *cr.rel_ptr, *cr.linked_as, "bk");
				};
				move_value_from_back(o, cr.relation_name, std::string("link_back_") + cr.linked_as->property_name,
					"last_id", "id_removed", cr.relation_name + "_id()", cob.is_expandable, 1);
			} else if(cr.linked_as->index == index_type::many) {
				o + "@obj@_remove_all_@rel@_as_@r_prop@(id_removed);";
				o + "@obj@_for_each_@rel@_as_@r_prop@(last_id, "
					"[this, id_removed, last_id](@rel@_id i)" + block{
						if(cr.linked_as->is_covered_by_composite_key)
							make_composite_key_internal_delete(o, *cr.rel_ptr, *cr.linked_as, "i");
						if(cr.linked_as->multiplicity > 1) {
							for(int32_t j = 0; j < cr.linked_as->multiplicity; ++j) {
								o + substitute{ "i", std::to_string(j) };
								o + "if(@rel@.m_@r_prop@.vptr()[i.index()][@i@] == last_id) @rel@.m_@r_prop@.vptr()[i.index()][@i@] = id_removed;";
							}
							if(cr.linked_as->is_covered_by_composite_key) {
								o + "std::sort(@rel@.m_@r_prop@.vptr()[i.index()].begin(), @rel@.m_@r_prop@.vptr()[i.index()].end(), [](@obj@_id l, @obj@_id r){ return l.value < r.value; });";
							}
						} else {
							o + "@rel@.m_@r_prop@.vptr()[i.index()] = id_removed;";
						}
						if(cr.linked_as->is_covered_by_composite_key)
							make_composite_key_internal_assign(o, *cr.rel_ptr, *cr.linked_as, "i");
				} +append{ ");" };

				if(cr.linked_as->ltype == list_type::list) {
					move_value_from_back(o, cr.relation_name, std::string("head_back_") + cr.linked_as->property_name,
						"last_id", "id_removed", cr.relation_name + "_id()", cob.is_expandable, 1);
				} else if(cr.linked_as->ltype == list_type::array) {
					o + "@rel@.@r_prop@_storage.release(@rel@.m_array_@r_prop@.vptr()[id_removed.index()]);";
					move_value_from_back(o, cr.relation_name, std::string("array_") + cr.linked_as->property_name,
						"last_id", "id_removed", "std::numeric_limits<dcon::stable_mk_2_tag>::max()", cob.is_expandable, 1);
				} else if(cr.linked_as->ltype == list_type::std_vector) {
					move_value_from_back(o, cr.relation_name, std::string("array_") + cr.linked_as->property_name,
						"last_id", "id_removed", std::string("std::vector<") + cr.relation_name + "_id>{}", cob.is_expandable, 1);
				}
			}
		}
		for(auto& cp : cob.properties) {
			o + substitute{ "prop", cp.name };
			if(cp.is_derived) {
			} else if(cp.type == property_type::special_vector) {
				o + "@obj@.@prop@_storage.release(@obj@.m_@prop@.vptr()[id_removed.index()]);";
				move_value_from_back(o, cob.name, cp.name,
					"last_id", "id_removed", "std::numeric_limits<dcon::stable_mk_2_tag>::max()", cob.is_expandable, 1);
			} else if(cp.type == property_type::bitfield) {
				o + "dcon::bit_vector_set(@obj@.m_@prop@.vptr(), id_removed.index(), "
					"dcon::bit_vector_test(@obj@.m_@prop@.vptr(), last_id.index()));";
				o + "dcon::bit_vector_set(@obj@.m_@prop@.vptr(), last_id.index(), false);";
				if(cob.is_expandable)
					o + "@obj@.m_@prop@.values.resize(1 + (@obj@.size_used + 6) / 8);";
			} else if(cp.type == property_type::array_bitfield || cp.type == property_type::array_other || cp.type == property_type::array_vectorizable) {
				move_array_value_from_back(o, cob.name, cp.name,
					"last_id", "id_removed", cob.is_expandable);
			} else {
				move_value_from_back(o, cob.name, cp.name,
					"last_id", "id_removed", cp.data_type + "{}", cob.is_expandable, 1);
			}
		}

		for(auto& ck : cob.composite_indexes) {
			if(ck.involves_primary_key) {
				{
					std::string params;
					for(auto& idx : ck.component_indexes) {
						if(params.length() > 0)
							params += ", ";
						if(idx.property_name == cob.primary_key.property_name)
							params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(last_id.index()))";
						else
							params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
					}
					o + substitute{ "params", params } +substitute{ "ckname", ck.name };
					o + "@obj@.hashm_@ckname@.erase(@obj@.to_@ckname@_keydata(@params@));";
				}
				{
					std::string params;
					for(auto& idx : ck.component_indexes) {
						if(params.length() > 0)
							params += ", ";
						if(idx.property_name == cob.primary_key.property_name)
							params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(id_removed.index()))";
						else
							params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
					}
					o + substitute{ "params", params } +substitute{ "ckname", ck.name };
					o + "@obj@.hashm_@ckname@.insert_or_assign(@obj@.to_@ckname@_keydata(@params@), id_removed);";
				}
			} else {
				std::string params;
				for(auto& idx : ck.component_indexes) {
					if(params.length() > 0)
						params += ", ";
					if(idx.property_name == cob.primary_key.property_name)
						params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(last_id.index()))";
					else
						params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
				}
				o + substitute{ "params", params } +substitute{ "ckname", ck.name };
				o + "@obj@.hashm_@ckname@.insert_or_assign(@obj@.to_@ckname@_keydata(@params@), id_removed);";
			}
			
		}

		o + "--@obj@.size_used;";
		if(cob.hook_move)
			o + "on_move_@obj@(id_removed, last_id);";
	};
	o + line_break{};
	return o;
}

basic_builder& expandable_push_back(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name };

	for(auto& cr : cob.relationships_involved_in) {
		o + substitute{ "rel", cr.relation_name } +substitute{ "in_rel_as", cr.linked_as->property_name };
		if(cr.linked_as->is_primary_key) {
			o + "@rel@.size_used = @obj@.size_used + 1;";
			for(auto& rp : cr.rel_ptr->properties) {
				o + substitute{ "r_prop", rp.name };
				if(rp.is_derived) {
				} if(rp.type == property_type::bitfield) {
					o + "@rel@.m_@r_prop@.values.resize(1 + (@obj@.size_used + 8) / 8);";
				} else if(rp.type == property_type::special_vector) {
					o + "@rel@.m_@r+prop@.values.push_back(std::numeric_limits<dcon::stable_mk_2_tag>::max());";
				} else {
					o + "@rel@.m_@r_prop@.values.emplace_back();";
				}
			}
			for(auto& ri : cr.rel_ptr->indexed_objects) {
				o + substitute{ "r_prop", ri.property_name };
				if(cr.rel_ptr->primary_key == ri) {
				} else {
					o + "@rel@.m_@r_prop@.values.emplace_back();";
				}
				if(ri.ltype == list_type::list && ri.index == index_type::many) {
					o + "@rel@.m_link_@r_propr@.values.emplace_back();";
				}
			}
		} else if(cr.linked_as->index == index_type::at_most_one) {
			o + "@rel@.m_link_back_@in_rel_as@.values.emplace_back();";
		} else if(cr.linked_as->index == index_type::many) {
			if(cr.linked_as->ltype == list_type::list) {
				o + "@rel@.m_head_back_@in_rel_as@.values.emplace_back();";
			} else if(cr.linked_as->ltype == list_type::array) {
				o + "@rel@.m_array_@in_rel_as@.values.push_back(std::numeric_limits<dcon::stable_mk_2_tag>::max());";
			} else if(cr.linked_as->ltype == list_type::std_vector) {
				o + "@rel@.m_array_@in_rel_as@.values.emplace_back();";
			}
		}
	}
	for(auto& io : cob.indexed_objects) {
		o + substitute{ "prop", io.property_name };
		if(io.index == index_type::at_most_one && cob.primary_key == io) {

		} else if(io.index == index_type::at_most_one) {
			o + "@obj@.m_@prop@.values.emplace_back();";
		} else if(io.index == index_type::many) {
			o + "@obj@.m_@prop@.values.emplace_back();";
			if(io.ltype == list_type::list) {
				o + "@obj@.m_link_@prop@.values.emplace_back();";
			}
		} else if(io.index == index_type::none) {
			o + "@obj@.m_@prop@.values.emplace_back();";
		}
	}
	for(auto& cp : cob.properties) {
		o + substitute{ "prop", cp.name };
		if(cp.is_derived) {
		} else if(cp.type == property_type::bitfield) {
			o + "@obj@.m_@prop@.values.resize(1 + (@obj@.size_used + 8) / 8);";
		} else if(cp.type == property_type::special_vector) {
			o + "@obj@.m_@prop@.values.push_back(std::numeric_limits<dcon::stable_mk_2_tag>::max());";
		} else if(cp.type == property_type::array_bitfield || cp.type == property_type::array_other || cp.type == property_type::array_vectorizable) {
			o + "@obj@.m_@prop@.emplace_back_all(@obj@.size_used + 1);";
		} else {
			o + "@obj@.m_@prop@.values.emplace_back();";
		}
	}
	o + "++@obj@.size_used;";

	return o;
}

basic_builder& increase_size(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name };

	for(auto& cr : cob.relationships_involved_in) {
		o + substitute{ "rel", cr.relation_name } +substitute{ "in_rel_as", cr.linked_as->property_name };
		if(cr.linked_as->is_primary_key) {
			o + "@rel@.size_used = @obj@.size_used + 1;";
		}
	}
	o + "++@obj@.size_used;";

	return o;
}

basic_builder& make_non_erasable_create(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name } + substitute{ "size", std::to_string(cob.size) };
	o + heading{ "container create for @obj@" };

	o + "@obj@_id create_@obj@()" + block{

		o + "@obj@_id new_id(@obj@_id::value_base_t(@obj@.size_used));";

		if(cob.is_expandable) {
			expandable_push_back(o, cob);
		} else {
			o + "#ifndef DCON_USE_EXCEPTIONS";
			o + "if(@obj@.size_used >= @size@) std::abort();";
			o + "#else";
			o + "if(@obj@.size_used >= @size@) throw dcon::out_of_space{};";
			o + "#endif";
			increase_size(o, cob);
		}
		if(cob.hook_create)
			o + "on_create_@obj@(new_id);";
		o + "return new_id;";
	};

	o + line_break{};
	return o;
}

basic_builder& make_clearing_delete(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name } +substitute{ "size", std::to_string(cob.size) };
	o + heading{ "container delete for @obj@" };

	o + "void delete_@obj@(@obj@_id id_removed)" + block{
		if(cob.hook_delete)
			o + "on_delete_@obj@(id_removed);";

		for(auto& ck : cob.composite_indexes) {
			std::string params;
			for(auto& idx : ck.component_indexes) {
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == cob.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(id_removed.index()))";
				else
					params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
			}
			o + substitute{ "params", params } +substitute{ "ckname", ck.name };
			o + "@obj@.hashm_@ckname@.erase( @obj@.to_@ckname@_keydata(@params@) );";
		}

		for(auto& io : cob.indexed_objects) {
			if(cob.primary_key != io) {
				o + substitute{ "i_prop", io.property_name } +substitute{ "i_type", io.type_name };
				if(io.multiplicity == 1) {
					o + "internal_@obj@_set_@i_prop@(id_removed, @i_type@_id());";
				} else {
					for(int32_t i = 0; i < io.multiplicity; ++i) {
						o + substitute{ "i", std::to_string(i) };
						o + "internal_@obj@_set_@i_prop@(id_removed, @i@, @i_type@_id());";
					}
				}
			}
		}
		for(auto& cr : cob.relationships_involved_in) {
			o + substitute{ "rel", cr.relation_name } +substitute{ "r_prop", cr.linked_as->property_name };
			if(cr.linked_as->is_primary_key) {
				o + "delete_@rel@(@rel@_id(@rel@_id::value_base_t(id_removed.index())));";
			} else if(cr.linked_as->index == index_type::at_most_one) {
				o + "@obj@_remove_@rel@_as_@r_prop@(id_removed));";
			} else if(cr.linked_as->index == index_type::many) {
				o + "@obj@_remove_all_@rel@_as_@r_prop@(id_removed));";
			}
		}
		for(auto& cp : cob.properties) {
			if(!cp.is_derived) {
				clear_value(o, cob.name, cp.name, cp.data_type, cp.type, "id_removed", false);
			}
		}
	};

	o + line_break{};
	return o;
}

basic_builder& make_erasable_delete(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name } +substitute{ "size", std::to_string(cob.size) };
	o + heading{ "container delete for @obj@" };

	o + "void delete_@obj@(@obj@_id id_removed)" + block{
		o + "if(!@obj@_is_valid(id_removed)) return;";
		if(cob.hook_delete)
			o + "on_delete_@obj@(id_removed);";

		o + "@obj@.m__index.vptr()[id_removed.index()] = @obj@.first_free;"; 
		o + "@obj@.first_free = id_removed;";
		o + "if(int32_t(@obj@.size_used) - 1 == id_removed.index())" + block{
			o + "for( ; @obj@.size_used > 0 && "
			"@obj@.m__index.vptr()[@obj@.size_used - 1] != @obj@_id(@obj@_id::value_base_t(@obj@.size_used - 1));  "
			"--@obj@.size_used) ;";
		};

		for(auto& ck : cob.composite_indexes) {
			std::string params;
			for(auto& idx : ck.component_indexes) {
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == cob.primary_key.property_name)
					params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(id_removed.index()))";
				else
					params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
			}
			o + substitute{ "params", params } +substitute{ "ckname", ck.name };
			o + "@obj@.hashm_@ckname@.erase( @obj@.to_@ckname@_keydata(@params@) );";
		}

		for(auto& io : cob.indexed_objects) {
			if(cob.primary_key != io) {
				o + substitute{ "i_prop", io.property_name } +substitute{ "i_type", io.type_name };
				if(io.multiplicity == 1) {
					o + "internal_@obj@_set_@i_prop@(id_removed, @i_type@_id());";
				} else {
					for(int32_t i = 0; i < io.multiplicity; ++i) {
						o + substitute{ "i", std::to_string(i) };
						o + "internal_@obj@_set_@i_prop@(id_removed, @i@, @i_type@_id());";
					}
				}
			}
		}
		for(auto& cr : cob.relationships_involved_in) {
			o + substitute{ "rel", cr.relation_name } +substitute{ "r_prop", cr.linked_as->property_name };
			if(cr.linked_as->is_primary_key) {
				o + "delete_@rel@(@rel@_id(@rel@_id::value_base_t(id_removed.index())));";
				o + "@rel@.size_used = @obj@.size_used;";
			} else if(cr.linked_as->index == index_type::at_most_one) {
				o + "@obj@_remove_@rel@_as_@r_prop@(id_removed);";
			} else if(cr.linked_as->index == index_type::many) {
				o + "@obj@_remove_all_@rel@_as_@r_prop@(id_removed);";
			}
		}
		for(auto& cp : cob.properties) {
			if(!cp.is_derived) {
				clear_value(o, cob.name, cp.name, cp.data_type, cp.type, "id_removed", false);
			}
		}
	};

	o + line_break{};
	return o;
}
basic_builder& erasable_set_new_id(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "t_obj", cob.name };
	if(!cob.is_expandable) {
		o + "#ifndef DCON_USE_EXCEPTIONS";
		o + "if(!bool(@t_obj@.first_free)) std::abort();";
		o + "#else";
		o + "if(!bool(@t_obj@.first_free)) throw dcon::out_of_space{};";
		o + "#endif";
		o + "@t_obj@_id new_id = @t_obj@.first_free;";
		o + "@t_obj@.first_free = @t_obj@.m__index.vptr()[@t_obj@.first_free.index()];";
		o + "@t_obj@.m__index.vptr()[new_id.index()] = new_id;";
		o + "@t_obj@.size_used = std::max(@t_obj@.size_used, uint32_t(new_id.index() + 1));";
	} else {
		o + "@t_obj@_id new_id = @t_obj@.first_free;";
		o + "bool expanded = !bool(@t_obj@.first_free );";
		o + "if(expanded)" + block{
			o + "new_id = @t_obj@_id(@t_obj@_id::value_base_t(@t_obj@.size_used));";
			o + "@t_obj@.m__index.values.push_back(new_id);";
			expandable_push_back(o, cob);
		} +append{ "else" } +block{
			o + "@t_obj@.first_free = @t_obj@.m__index.vptr()[@t_obj@.first_free.index()];";
			o + "@t_obj@.m__index.vptr()[new_id.index()] = new_id;";
			o + "@t_obj@.size_used = std::max(@t_obj@.size_used, uint32_t(new_id.index() + 1));";
		};
	}
	
	for(auto& cr : cob.relationships_involved_in) {
		if(cr.linked_as->is_primary_key) {
			o + substitute{ "rel", cr.relation_name };
			o + "@rel@.size_used = @t_obj@.size_used;";
		}
	}
	

	return o;
}


basic_builder& make_erasable_create(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name } +substitute{ "size", std::to_string(cob.size) };
	o + heading{ "container create for @obj@" };

	o + "@obj@_id create_@obj@()" + block{
		
		erasable_set_new_id(o, cob);

		if(cob.hook_create)
			o + "on_create_@obj@(new_id);";
		o + "return new_id;";
	};

	o + line_break{};
	return o;
}

basic_builder& make_internal_move_relationship(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name } +substitute{ "size", std::to_string(cob.size) };
	o + heading{ "container move relationship for @obj@" };

	o + "void internal_move_relationship_@obj@(@obj@_id last_id, @obj@_id id_removed)" + block{
		{
			std::string params;
			for(auto& ck : cob.composite_indexes) {
				for(auto& idx : ck.component_indexes) {
					if(params.length() > 0)
						params += ", ";
					if(idx.property_name == cob.primary_key.property_name)
						params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(last_id.index()))";
					else
						params += cob.name + ".m_" + idx.property_name + ".vptr()[last_id.index()]";
				}
				o + substitute{ "params", params } + substitute{ "ckname", ck.name };
				o + "@obj@.hashm_@ckname@.erase(@obj@.to_@ckname@_keydata(@params@));";
			}
		}
		

		for(auto& io : cob.indexed_objects) {
			if(cob.primary_key != io) {
				o + substitute{ "i_prop", io.property_name } +substitute{ "i_type", io.type_name };
				if(io.multiplicity == 1) {
					o + "internal_@obj@_set_@i_prop@(id_removed, @i_type@_id());";
				} else {
					for(int32_t i = 0; i < io.multiplicity; ++i) {
						o + substitute{ "i", std::to_string(i) };
						o + "internal_@obj@_set_@i_prop@(id_removed, @i@, @i_type@_id());";
					}
				}

				if(io.index == index_type::at_most_one) {
					if(io.multiplicity == 1) {
						o + "if(auto related = @obj@.m_@i_prop@.vptr()[last_id.index()]; bool(related))" + block{
							o + "@obj@.m_link_back_@i_prop@.vptr()[related.index()] = id_removed;";
						};
					} else {
						for(int32_t i = 0; i < io.multiplicity; ++i) {
							o + substitute{ "i", std::to_string(i) };
							o + "if(auto related = @obj@.m_@i_prop@.vptr()[last_id.index()][@i@]; bool(related))" + block{
								o + "@obj@.m_link_back_@i_prop@.vptr()[related.index()] = id_removed;";
							};
						}
					}
				} else if(io.index == index_type::many) {
					if(io.ltype == list_type::list) {
						o + "if(bool(@obj@.m_@i_prop@.vptr()[last_id.index()]))" + block{
							o + "auto tmp = @obj@.m_link_@i_prop@.vptr()[last_id.index()];";
							o + "if(bool(tmp.left))" + block{
								o + "@obj@.m_link_@i_prop@.vptr()[tmp.left.index()].right = id_removed;";
							} +append{ "else" } +block{
								o + "for(auto lpos = last_id; bool(lpos); lpos = @obj@.m_link_@i_prop@.vptr()[lpos.index()].right)" + block{
									o + "@obj@.m_head_back_@i_prop@.vptr()[@obj@.m_@i_prop@.vptr()[lpos.index()].index()] = id_removed;";
								};
							};
							o + "if(bool(tmp.right))" + block{
								o + "@obj@.m_link_@i_prop@.vptr()[tmp.right.index()].left = id_removed;";
							};
						};
						move_value(o, cob.name, std::string("link_") + io.property_name,
							"last_id", "id_removed", cob.name + "_id_pair()", 1);
					} else if(io.ltype == list_type::std_vector) {
						if(io.multiplicity == 1) {
							o + "if(auto tmp = @obj@.m_@i_prop@.vptr()[last_id.index()]; bool(tmp))" + block{
								o + "auto& vref = @obj@.m_array_@i_prop@.vptr()[tmp.index()];";
								o + "if(auto pos = std::find(vref.begin(), vref.end(), last_id); pos != vref.end())" + block {
									o + "*pos = id_removed;";
								};
							};
						} else {
							for(int32_t i = 0; i < io.multiplicity; ++i) {
								o + substitute{ "i", std::to_string(i) };
								o + "if(auto tmp = @obj@.m_@i_prop@.vptr()[last_id.index()][@i@]; bool(tmp))" + block{
									o + "auto& vref = @obj@.m_array_@i_prop@.vptr()[tmp.index()];";
									if(io.is_distinct) {
										o + "if(auto pos = std::find(vref.begin(), vref.end(), last_id); pos != vref.end())" + block{
											o + "*pos = id_removed;";
										};
									} else {
										o + "for(auto pos = std::find(vref.begin(), vref.end(), last_id); pos != vref.end(); pos = std::find(pos + 1, vref.end(), last_id))" + block{
											o + "*pos = id_removed;";
										};
									}
								};
							}
						}
					} else if(io.ltype == list_type::array) {
						if(io.multiplicity == 1) {
							o + "if(auto tmp = @obj@.m_@i_prop@.vptr()[last_id.index()]; bool(tmp))" + block{
								o + "dcon::replace_unique_item(@obj@.@i_prop@_storage, @obj@.m_array_@i_prop@.vptr()[tmp.index()],"
									" last_id, id_removed);";
							};
						} else {
							for(int32_t i = 0; i < io.multiplicity; ++i) {
								o + substitute{ "i", std::to_string(i) };
								o + "if(auto tmp = @obj@.m_@i_prop@.vptr()[last_id.index()][@i@]; bool(tmp))" + block{
									if(io.is_distinct) {
										o + "dcon::replace_unique_item(@obj@.@i_prop@_storage, @obj@.m_array_@i_prop@.vptr()[tmp.index()],"
											" last_id, id_removed);";
									} else {
										o + "dcon::replace_all_items(@obj@.@i_prop@_storage, @obj@.m_array_@i_prop@.vptr()[tmp.index()],"
											" last_id, id_removed);";
									}
								};
							}
						}
					}
				}

				move_value(o, cob.name, io.property_name,
					"last_id", "id_removed", io.type_name + "_id()", io.multiplicity);
			}
		}
		for(auto& cp : cob.properties) {
			o + substitute{ "prop", cp.name };
			if(cp.is_derived) {
			} else if(cp.type == property_type::special_vector) {
				o + "@obj@.@prop@_storage.release(@obj@.m_@prop@.vptr()[id_removed.index()]);";
				move_value(o, cob.name, cp.name,
					"last_id", "id_removed", "std::numeric_limits<dcon::stable_mk_2_tag>::max()", 1);
			} else if(cp.type == property_type::bitfield) {
				o + "dcon::bit_vector_set(@obj@.m_@prop@.vptr(), id_removed.index(), "
					"dcon::bit_vector_test(@obj@.m_@prop@.vptr(), last_id.index()));";
				o + "dcon::bit_vector_set(@obj@.m_@prop@.vptr(), last_id.index(), false);";
			} else if(cp.type == property_type::array_bitfield || cp.type == property_type::array_other || cp.type == property_type::array_vectorizable) {
				move_array_value(o, cob.name, cp.name,
					"last_id", "id_removed");
			} else {
				move_value(o, cob.name, cp.name,
					"last_id", "id_removed", cp.data_type + "{}", 1);
			}
		}

		for(auto& ck : cob.composite_indexes) {
			if(ck.involves_primary_key) {
				{
					std::string params;
					for(auto& idx : ck.component_indexes) {
						if(params.length() > 0)
							params += ", ";
						if(idx.property_name == cob.primary_key.property_name)
							params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(last_id.index()))";
						else
							params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
					}
					o + substitute{ "params", params } +substitute{ "ckname", ck.name };
					o + "@obj@.hashm_@ckname@.erase(@obj@.to_@ckname@_keydata(@params@));";
				}
				{
					std::string params;
					for(auto& idx : ck.component_indexes) {
						if(params.length() > 0)
							params += ", ";
						if(idx.property_name == cob.primary_key.property_name)
							params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(id_removed.index()))";
						else
							params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
					}
					o + substitute{ "params", params } +substitute{ "ckname", ck.name };
					o + "@obj@.hashm_@ckname@.insert_or_assign(@obj@.to_@ckname@_keydata(@params@), id_removed);";
				}
			} else {
				std::string params;
				for(auto& idx : ck.component_indexes) {
					if(params.length() > 0)
						params += ", ";
					if(idx.property_name == cob.primary_key.property_name)
						params += idx.object_type + "_id(" + idx.object_type + "_id::value_base_t(last_id.index()))";
					else
						params += cob.name + ".m_" + idx.property_name + ".vptr()[id_removed.index()]";
				}
				o + substitute{ "params", params } +substitute{ "ckname", ck.name };
				o + "@obj@.hashm_@ckname@.insert_or_assign(@obj@.to_@ckname@_keydata(@params@), id_removed);";
			}

		}
	};

	o + line_break{};
	return o;
}

basic_builder& make_relation_try_create(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name } +substitute{ "size", std::to_string(cob.size) }
	+substitute{ "params", make_relationship_parameters(cob) };
	o + heading{ "container try create relationship for @obj@" };

	o + "@obj@_id try_create_@obj@(@params@)" + block{
		for(auto& iob : cob.indexed_objects) {
			o + substitute{ "prop", iob.property_name };
			if(iob.multiplicity == 1) {
				if(!iob.is_optional) {
					o + "if(!bool(@prop@_p)) return @obj@_id();";
				}
				if(cob.primary_key != iob) {
					if(iob.index == index_type::at_most_one) {
						o + "if(bool(@prop@_p) && bool(@obj@.m_link_back_@prop@.vptr()[@prop@_p.index()])) return @obj@_id();";
					}
				} else {
					o + "if(@obj@_is_valid(@obj@_id(@obj@_id::value_base_t(@prop@_p.index())))) return @obj@_id();";
				}
			} else {
				for(int32_t i = 0; i < iob.multiplicity; ++i) {
					o + substitute{ "i", std::to_string(i) };
					if(!iob.is_optional) {
						o + "if(!bool(@prop@_p@i@)) return @obj@_id();";
					}
					if(iob.index == index_type::at_most_one) {
						o + "if(bool(@prop@_p@i@) && bool(@obj@.m_link_back_@prop@.vptr()[@prop@_p@i@.index()])) return @obj@_id();";
					}
					if(iob.is_distinct) {
						for(int32_t j = i + 1; j < iob.multiplicity; ++j) {
							o + substitute{ "j", std::to_string(j) };
							o + "if(bool(@prop@_p@i@) && @prop@_p@i@ == @prop@_p@j@) return @obj@_id();";
						}
					}
				}

				o + substitute{ "mult", std::to_string(iob.multiplicity) };
				o + substitute{ "id_type", iob.type_name + "_id" };

				if(iob.is_covered_by_composite_key) {
					o + "std::array<@id_type@, @mult@> @prop@_p =" + class_block{
						for(int32_t i = 0; i < iob.multiplicity; ++i) {
							o + substitute{ "i", std::to_string(i) };
							o + "@prop@_p@i@,";
						}
					};
					o + "std::sort(@prop@_p.begin(), @prop@_p.end(), [](@id_type@ a, @id_type@ b){ return a.value < b.value; });";
				}
			}
		}



		for(auto& ck : cob.composite_indexes) {
			if(!ck.involves_primary_key) {
				std::string params;
				for(auto& idx : ck.component_indexes) {
					if(params.length() > 0)
						params += ", ";
					params += idx.property_name + "_p";
				}
				o + substitute{ "params", params } +substitute{ "ckname", ck.name };
				o + "if(@obj@.hashm_@ckname@.contains(@obj@.to_@ckname@_keydata(@params@))) return @obj@_id();";
			}
		}

		if(cob.primary_key.points_to != nullptr) {
			for(auto& iob : cob.indexed_objects) {
				if(cob.primary_key == iob) {
					o + substitute{ "pkey", iob.property_name };
				}
			}
			o + "@obj@_id new_id(@obj@_id::value_base_t(@pkey@_p.index()));";
			o + "if(@obj@.size_used < uint32_t(@pkey@_p.value)) @obj@_resize(uint32_t(@pkey@_p.value));";
		} else if(cob.store_type != storage_type::erasable) {
			o + "@obj@_id new_id = @obj@_id(@obj@_id::value_base_t(@obj@.size_used));";

			if(!cob.is_expandable) {
				o + "#ifndef DCON_USE_EXCEPTIONS";
				o + "if(@obj@.size_used >= @size@) std::abort();";
				o + "#else";
				o + "if(@obj@.size_used >= @size@) throw dcon::out_of_space{};";
				o + "#endif";
				increase_size(o, cob);
			} else {
				expandable_push_back(o, cob);
			}
		} else {
			erasable_set_new_id(o, cob);
		}
		

		for(auto& iob : cob.indexed_objects) {
			o + substitute{ "prop", iob.property_name };
			if( cob.primary_key != iob)  {
				if(iob.multiplicity == 1) {
					o + "internal_@obj@_set_@prop@(new_id, @prop@_p);";
				} else {
					for(int32_t i = 0; i < iob.multiplicity; ++i) {
						o + substitute{ "i", std::to_string(i) };
						if(iob.is_covered_by_composite_key) {
							o + "internal_@obj@_set_@prop@(new_id, @i@, @prop@_p[@i@]);";
						} else {
							o + "internal_@obj@_set_@prop@(new_id, @i@, @prop@_p@i@);";
						}
					}
				}
			}
		}

		for(auto& ck : cob.composite_indexes) {
			std::string params;
			for(auto& idx : ck.component_indexes) {
				if(params.length() > 0)
					params += ", ";
				params += idx.property_name + "_p";
			}
			o + substitute{ "params", params } +substitute{ "ckname", ck.name };
			o + "@obj@.hashm_@ckname@.insert_or_assign(@obj@.to_@ckname@_keydata(@params@), new_id);";
		}

		if(cob.hook_create)
			o + "on_create_@obj@(new_id);";

		o + "return new_id;";
	};

	o + line_break{};
	return o;
}
basic_builder& make_relation_force_create(basic_builder& o, relationship_object_def const& cob) {
	o + substitute{ "obj", cob.name } +substitute{ "size", std::to_string(cob.size) }
	+substitute{ "params", make_relationship_parameters(cob) };
	o + heading{ "container force create relationship for @obj@" };

	o + "@obj@_id force_create_@obj@(@params@)" + block{
		for(auto& iob : cob.indexed_objects) {
			if(iob.multiplicity > 1) {
				o + substitute{ "mult", std::to_string(iob.multiplicity) };
				o + substitute{ "id_type", iob.type_name + "_id" };
				o + substitute{ "prop", iob.property_name };

				if(iob.is_covered_by_composite_key) {
					o + "std::array<@id_type@, @mult@> @prop@_p =" + class_block{
						for(int32_t i = 0; i < iob.multiplicity; ++i) {
							o + substitute{ "i", std::to_string(i) };
							o + "@prop@_p@i@,";
						}
					};
					o + "std::sort(@prop@_p.begin(), @prop@_p.end(), [](@id_type@ a, @id_type@ b){ return a.value < b.value; });";
				}
			}
		}

		if(cob.primary_key.points_to != nullptr) {
			for(auto& iob : cob.indexed_objects) {
				if(cob.primary_key == iob) {
					o + substitute{ "pkey", iob.property_name };
				}
			}
			o + "@obj@_id new_id(@obj@_id::value_base_t(@pkey@_p.index()));";
			o + "if(@obj@.size_used < uint32_t(@pkey@_p.value)) @obj@_resize(uint32_t(@pkey@_p.value));";
		} else if(cob.store_type != storage_type::erasable) {
			o + "@obj@_id new_id = @obj@_id(@obj@_id::value_base_t(@obj@.size_used));";

			if(!cob.is_expandable) {
				o + "#ifndef DCON_USE_EXCEPTIONS";
				o + "if(@obj@.size_used >= @size@) std::abort();";
				o + "#else";
				o + "if(@obj@.size_used >= @size@) throw dcon::out_of_space{};";
				o + "#endif";
				increase_size(o, cob);
			} else {
				expandable_push_back(o, cob);
			}
		} else {
			erasable_set_new_id(o, cob);
		}

		for(auto& ck : cob.composite_indexes) {
			std::string params;
			for(auto& idx : ck.component_indexes) {
				if(params.length() > 0)
					params += ", ";
				params += idx.property_name + "_p";
			}
			o + substitute{ "params", params } +substitute{ "ckname", ck.name };
			o + inline_block{
				o + "auto key_dat = @obj@.to_@ckname@_keydata(@params@);";
				o + "if(auto it = @obj@.hashm_@ckname@.find(key_dat); it !=  @obj@.hashm_@ckname@.end())" + block{
					o + "delete_@obj@(it->second);";
				};
				o + "@obj@.hashm_@ckname@.insert_or_assign(key_dat, new_id);";
			};
		}


		for(auto& iob : cob.indexed_objects) {
			o + substitute{ "prop", iob.property_name };
			if( cob.primary_key != iob) {
				if(iob.multiplicity == 1) {
					o + "internal_@obj@_set_@prop@(new_id, @prop@_p);";
				} else {
					for(int32_t i = 0; i < iob.multiplicity; ++i) {
						o + substitute{ "i", std::to_string(i) };
						if(iob.is_covered_by_composite_key) {
							o + "internal_@obj@_set_@prop@(new_id, @i@, @prop@_p[@i@]);";
						} else {
							o + "internal_@obj@_set_@prop@(new_id, @i@, @prop@_p@i@);";
						}
					}
				}
			}
		}

		if(cob.hook_create)
			o + "on_create_@obj@(new_id);";

		o + "return new_id;";
	};

	o + line_break{};
	return o;
}

basic_builder& relation_iterator_foreach_as_generator(basic_builder& o, relationship_object_def const& obj, relationship_object_def const& rel, related_object const& l) {

	o + substitute{ "obj", obj.name };
	o + substitute{ "rel", rel.name };
	o + substitute{ "rel_prop", l.property_name };
	o + substitute{ "it_name", std::string("iterator_") + obj.name + "_foreach_" + rel.name + "_as_" + l.property_name };

	o + "struct @it_name@_generator" + class_block{
		o + "data_container& container;";
		o + "@obj@_id ob;";

		o + "@it_name@_generator(data_container& c, @obj@_id o) : container(c), ob(o) {}";
		o + "DCON_RELEASE_INLINE @it_name@ begin() const noexcept" + block{
			o + "return @it_name@(container, ob);";
		};
		o + "DCON_RELEASE_INLINE @it_name@ end() const noexcept" + block{
			if(l.ltype == list_type::list) {
				o + "return @it_name@(container);";
			} else {
				o + "return @it_name@(container, ob, 0);";
			}
		};
	};

	o + substitute{ "it_name", std::string("const_iterator_") + obj.name + "_foreach_" + rel.name + "_as_" + l.property_name };

	o + "struct @it_name@_generator" + class_block{
		o + "data_container const& container;";
		o + "@obj@_id ob;";

		o + "@it_name@_generator(data_container const& c, @obj@_id o) : container(c), ob(o) {}";
		o + "DCON_RELEASE_INLINE @it_name@ begin() const noexcept" + block{
			o + "return @it_name@(container, ob);";
		};
		o + "DCON_RELEASE_INLINE @it_name@ end() const noexcept" + block{
			if(l.ltype == list_type::list) {
				o + "return @it_name@(container);";
			} else {
				o + "return @it_name@(container, ob, 0);";
			}
		};
	};

	o + line_break{};
	return o;
}

basic_builder& relation_iterator_foreach_as_declaration(basic_builder& o, relationship_object_def const& obj, relationship_object_def const& rel, related_object const& l) {
	o + substitute{ "obj", obj.name };
	o + substitute{ "rel", rel.name };
	o + substitute{ "rel_prop", l.property_name };
	o + substitute{ "it_name", std::string("iterator_") + obj.name + "_foreach_" + rel.name + "_as_" + l.property_name };
	o + "class @it_name@" + class_block{
		o + "private:";
		o + "data_container& container;";
		if(l.ltype == list_type::list) {
			o + "@rel@_id list_pos;";
		} else {
			o + "@rel@_id const* ptr = nullptr;";
		}

		o + "public:";
		o + "@it_name@(data_container& c, @obj@_id fr) noexcept;";
		if(l.ltype == list_type::list) {
			o + "@it_name@(data_container& c, @rel@_id r) noexcept : container(c), list_pos(r) {}";
			o + "@it_name@(data_container& c) noexcept : container(c) {}";
		} else {
			o + "@it_name@(data_container& c, @rel@_id const* r) noexcept : container(c), ptr(r) {}";
			o + "@it_name@(data_container& c, @obj@_id fr, int) noexcept;";
		}

		o + "DCON_RELEASE_INLINE @it_name@& operator++() noexcept;";
		o + "DCON_RELEASE_INLINE @it_name@& operator--() noexcept;";

		o + "DCON_RELEASE_INLINE bool operator==(@it_name@ const& o) const noexcept" + block{
			if(l.ltype == list_type::list) {
				o + "return list_pos == o.list_pos;";
			} else {
				o + "return ptr == o.ptr;";
			}
		};
		o + "DCON_RELEASE_INLINE bool operator!=(@it_name@ const& o) const noexcept" + block{
			o + "return !(*this == o);";
		};
		o + "DCON_RELEASE_INLINE @rel@_fat_id operator*() const noexcept" + block{
			if(l.ltype == list_type::list) {
				o + "return @rel@_fat_id(container, list_pos);";
			} else {
				o + "return @rel@_fat_id(container, *ptr);";
			}
		};

		if(l.ltype != list_type::list) {
			o + "DCON_RELEASE_INLINE @it_name@& operator+=(ptrdiff_t n) noexcept" + block{
				o + "ptr += n;";
				o + "return *this;";
			};
			o + "DCON_RELEASE_INLINE @it_name@& operator-=(ptrdiff_t n) noexcept" + block{
				o + "ptr -= n;";
				o + "return *this;";
			};
			o + "DCON_RELEASE_INLINE @it_name@ operator+(ptrdiff_t n) const noexcept" + block{
				o + "return @it_name@(container, ptr + n);";
			};
			o + "DCON_RELEASE_INLINE @it_name@ operator-(ptrdiff_t n) const noexcept" + block{
				o + "return @it_name@(container, ptr - n);";
			};
			o + "DCON_RELEASE_INLINE ptrdiff_t operator-(@it_name@ const& o) const noexcept" + block{
				o + "return ptr - o.ptr;";
			};
			o + "DCON_RELEASE_INLINE bool operator>(@it_name@ const& o) const noexcept" + block{
				o + "return ptr > o.ptr;";
			};
			o + "DCON_RELEASE_INLINE bool operator>=(@it_name@ const& o) const noexcept" + block{
				o + "return ptr >= o.ptr;";
			};
			o + "DCON_RELEASE_INLINE bool operator<(@it_name@ const& o) const noexcept" + block{
				o + "return ptr < o.ptr;";
			};
			o + "DCON_RELEASE_INLINE bool operator<=(@it_name@ const& o) const noexcept" + block{
				o + "return ptr <= o.ptr;";
			};
			o + "DCON_RELEASE_INLINE @rel@_fat_id operator[](ptrdiff_t n) const noexcept" + block{
				o + "return @rel@_fat_id(container, *(ptr + n));";
			};
		}
	};

	o + substitute{ "it_name", std::string("const_iterator_") + obj.name + "_foreach_" + rel.name + "_as_" + l.property_name };
	o + "class @it_name@" + class_block{
		o + "private:";
		o + "data_container const& container;";
		if(l.ltype == list_type::list) {
			o + "@rel@_id list_pos;";
		} else {
			o + "@rel@_id const* ptr = nullptr;";
		}

		o + "public:";
		o + "@it_name@(data_container const& c, @obj@_id fr) noexcept;";
		if(l.ltype == list_type::list) {
			o + "@it_name@(data_container const& c, @rel@_id r) noexcept : container(c), list_pos(r) {}";
			o + "@it_name@(data_container const& c) noexcept : container(c) {}";
		} else {
			o + "@it_name@(data_container const& c, @rel@_id const* r) noexcept : container(c), ptr(r) {}";
			o + "@it_name@(data_container const& c, @obj@_id fr, int) noexcept;";
		}

		o + "DCON_RELEASE_INLINE @it_name@& operator++() noexcept;";
		o + "DCON_RELEASE_INLINE @it_name@& operator--() noexcept;";

		o + "DCON_RELEASE_INLINE bool operator==(@it_name@ const& o) const noexcept" + block{
			if(l.ltype == list_type::list) {
				o + "return list_pos == o.list_pos;";
			} else {
				o + "return ptr == o.ptr;";
			}
		};
		o + "DCON_RELEASE_INLINE bool operator!=(@it_name@ const& o) const noexcept" + block{
			o + "return !(*this == o);";
		};
		o + "DCON_RELEASE_INLINE @rel@_const_fat_id operator*() const noexcept" + block{
			if(l.ltype == list_type::list) {
				o + "return @rel@_const_fat_id(container, list_pos);";
			} else {
				o + "return @rel@_const_fat_id(container, *ptr);";
			}
		};

		if(l.ltype != list_type::list) {
			o + "DCON_RELEASE_INLINE @it_name@& operator+=(ptrdiff_t n) noexcept" + block{
				o + "ptr += n;";
				o + "return *this;";
			};
			o + "DCON_RELEASE_INLINE @it_name@& operator-=(ptrdiff_t n) noexcept" + block{
				o + "ptr -= n;";
				o + "return *this;";
			};
			o + "DCON_RELEASE_INLINE @it_name@ operator+(ptrdiff_t n) const noexcept" + block{
				o + "return @it_name@(container, ptr + n);";
			};
			o + "DCON_RELEASE_INLINE @it_name@ operator-(ptrdiff_t n) const noexcept" + block{
				o + "return @it_name@(container, ptr - n);";
			};
			o + "DCON_RELEASE_INLINE ptrdiff_t operator-(@it_name@ const& o) const noexcept" + block{
				o + "return ptr - o.ptr;";
			};
			o + "DCON_RELEASE_INLINE bool operator>(@it_name@ const& o) const noexcept" + block{
				o + "return ptr > o.ptr;";
			};
			o + "DCON_RELEASE_INLINE bool operator>=(@it_name@ const& o) const noexcept" + block{
				o + "return ptr >= o.ptr;";
			};
			o + "DCON_RELEASE_INLINE bool operator<(@it_name@ const& o) const noexcept" + block{
				o + "return ptr < o.ptr;";
			};
			o + "DCON_RELEASE_INLINE bool operator<=(@it_name@ const& o) const noexcept" + block{
				o + "return ptr <= o.ptr;";
			};
			o + "DCON_RELEASE_INLINE @rel@_const_fat_id operator[](ptrdiff_t n) const noexcept" + block{
				o + "return @rel@_const_fat_id(container, *(ptr + n));";
			};
		}
	};

	o + line_break{};
	return o;
}

basic_builder& relation_iterator_foreach_as_implementation(basic_builder& o, relationship_object_def const& obj, relationship_object_def const& rel, related_object const& l) {

	o + substitute{ "obj", obj.name };
	o + substitute{ "rel", rel.name };
	o + substitute{ "rel_prop", l.property_name };

	o + substitute{ "it_name", std::string("iterator_") + obj.name + "_foreach_" + rel.name + "_as_" + l.property_name };

	o + "DCON_RELEASE_INLINE @it_name@::@it_name@(data_container& c,  @obj@_id fr) noexcept : container(c)" + block{
		if(l.ltype == list_type::list) {
			o + "list_pos = container.@rel@.m_head_back_@rel_prop@.vptr()[fr.index()];";
		} else if(l.ltype == list_type::array) {
			o + "ptr = dcon::get_range(container.@rel@.@rel_prop@_storage, container.@rel@.m_array_@rel_prop@.vptr()[fr.index()]).first;";
		} else if(l.ltype == list_type::std_vector) {
			o + "ptr = container.@rel@.m_array_@rel_prop@.vptr()[fr.index()].data();";
		}
	};

	if(l.ltype != list_type::list) {
		o + "DCON_RELEASE_INLINE @it_name@::@it_name@(data_container& c, @obj@_id fr, int) noexcept : container(c)" + block{
			if(l.ltype == list_type::array) {
				o + "ptr = dcon::get_range(container.@rel@.@rel_prop@_storage, container.@rel@.m_array_@rel_prop@.vptr()[fr.index()]).second;";
			} else if(l.ltype == list_type::std_vector) {
				o + "auto& vref = container.@rel@.m_array_@rel_prop@.vptr()[fr.index()];";
				o + "ptr = vref.data() + vref.size();";
			}
		};
	}

	o + "DCON_RELEASE_INLINE @it_name@& @it_name@::operator++() noexcept" + block{
		if(l.ltype == list_type::list) {
			o + "list_pos = container.@rel@.m_link_@rel_prop@.vptr()[list_pos.index()].right;";
		} else {
			o + "++ptr;";
		}
		o + "return *this;";
	};
	o + "DCON_RELEASE_INLINE @it_name@& @it_name@::operator--() noexcept" + block{
		if(l.ltype == list_type::list) {
			o + "list_pos = container.@rel@.m_link_@rel_prop@.vptr()[list_pos.index()].left;";
		} else {
			o + "--ptr;";
		}
		o + "return *this;";
	};

	o + substitute{ "it_name", std::string("const_iterator_") + obj.name + "_foreach_" + rel.name + "_as_" + l.property_name };
	o + "DCON_RELEASE_INLINE @it_name@::@it_name@(data_container const& c,  @obj@_id fr) noexcept : container(c)" + block{
		if(l.ltype == list_type::list) {
			o + "list_pos = container.@rel@.m_head_back_@rel_prop@.vptr()[fr.index()];";
		} else if(l.ltype == list_type::array) {
			o + "ptr = dcon::get_range(container.@rel@.@rel_prop@_storage, container.@rel@.m_array_@rel_prop@.vptr()[fr.index()]).first;";
		} else if(l.ltype == list_type::std_vector) {
			o + "ptr = container.@rel@.m_array_@rel_prop@.vptr()[fr.index()].data();";
		}
	};
	if(l.ltype != list_type::list) {
		o + "DCON_RELEASE_INLINE @it_name@::@it_name@(data_container const& c, @obj@_id fr, int) noexcept : container(c)" + block{
			if(l.ltype == list_type::array) {
				o + "ptr = dcon::get_range(container.@rel@.@rel_prop@_storage, container.@rel@.m_array_@rel_prop@.vptr()[fr.index()]).second;";
			} else if(l.ltype == list_type::std_vector) {
				o + "auto& vref = container.@rel@.m_array_@rel_prop@.vptr()[fr.index()];";
				o + "ptr = vref.data() + vref.size();";
			}
		};
	}
	o + "DCON_RELEASE_INLINE @it_name@& @it_name@::operator++() noexcept" + block{
		if(l.ltype == list_type::list) {
			o + "list_pos = container.@rel@.m_link_@rel_prop@.vptr()[list_pos.index()].right;";
		} else {
			o + "++ptr;";
		}
		o + "return *this;";
	};
	o + "DCON_RELEASE_INLINE @it_name@& @it_name@::operator--() noexcept" + block{
		if(l.ltype == list_type::list) {
			o + "list_pos = container.@rel@.m_link_@rel_prop@.vptr()[list_pos.index()].left;";
		} else {
			o + "--ptr;";
		}
		o + "return *this;";
	};

	o + line_break{};
	return o;
}

basic_builder& relation_iterator_prop_from_declaration(basic_builder& o, relationship_object_def const& obj, in_relation_information const& ir, related_object const& l) {


	o + line_break{};
	return o;
}

basic_builder& object_iterator_declaration(basic_builder& o, relationship_object_def const& obj) {
	o + substitute{ "obj", obj.name };
	o + "class object_iterator_@obj@" + class_block{
		o + "private:";
		o + "data_container& container;";
		o + "uint32_t index = 0;";

		o + "public:";
		o + "object_iterator_@obj@(data_container& c, uint32_t i) noexcept;";

		o + "DCON_RELEASE_INLINE object_iterator_@obj@& operator++() noexcept;";
		o + "DCON_RELEASE_INLINE object_iterator_@obj@& operator--() noexcept;";
		
		o + "DCON_RELEASE_INLINE bool operator==(object_iterator_@obj@ const& o) const noexcept" + block{
			o + "return &container == &o.container && index == o.index;";
		};
		o + "DCON_RELEASE_INLINE bool operator!=(object_iterator_@obj@ const& o) const noexcept" + block{
			o + "return !(*this == o);";
		};
		o + "DCON_RELEASE_INLINE @obj@_fat_id operator*() const noexcept" + block{
			o + "return @obj@_fat_id(container, @obj@_id(@obj@_id::value_base_t(index)));";
		};

		if(obj.store_type != storage_type::erasable) {
			o + "DCON_RELEASE_INLINE object_iterator_@obj@& operator+=(int32_t n) noexcept" + block{
				o + "index = uint32_t(int32_t(index) + n);";
				o + "return *this;";
			};
			o + "DCON_RELEASE_INLINE object_iterator_@obj@& operator-=(int32_t n) noexcept" + block{
				o + "index = uint32_t(int32_t(index) - n);";
				o + "return *this;";
			};
			o + "DCON_RELEASE_INLINE object_iterator_@obj@ operator+(int32_t n) const noexcept" + block{
				o + "return object_iterator_@obj@(container, uint32_t(int32_t(index) + n));";
			};
			o + "DCON_RELEASE_INLINE object_iterator_@obj@ operator-(int32_t n) const noexcept" + block{
				o + "return object_iterator_@obj@(container, uint32_t(int32_t(index) - n));";
			};
			o + "DCON_RELEASE_INLINE int32_t operator-(object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return int32_t(index) - int32_t(o.index);";
			};
			o + "DCON_RELEASE_INLINE bool operator>(object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return index > o.index;";
			};
			o + "DCON_RELEASE_INLINE bool operator>=(object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return index >= o.index;";
			};
			o + "DCON_RELEASE_INLINE bool operator<(object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return index < o.index;";
			};
			o + "DCON_RELEASE_INLINE bool operator<=(object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return index <= o.index;";
			};
			o + "DCON_RELEASE_INLINE @obj@_fat_id operator[](int32_t n) const noexcept" + block{
				o + "return @obj@_fat_id(container, @obj@_id(@obj@_id::value_base_t(int32_t(index) + n)));";
			};
		}
	};
	o + "class const_object_iterator_@obj@" + class_block{
		o + "private:";
		o + "data_container const& container;";
		o + "uint32_t index = 0;";

		o + "public:";
		o + "const_object_iterator_@obj@(data_container const& c, uint32_t i) noexcept;";

		o + "DCON_RELEASE_INLINE const_object_iterator_@obj@& operator++() noexcept;";
		o + "DCON_RELEASE_INLINE const_object_iterator_@obj@& operator--() noexcept;";

		o + "DCON_RELEASE_INLINE bool operator==(const_object_iterator_@obj@ const& o) const noexcept" + block{
			o + "return &container == &o.container && index == o.index;";
		};
		o + "DCON_RELEASE_INLINE bool operator!=(const_object_iterator_@obj@ const& o) const noexcept" + block{
			o + "return !(*this == o);";
		};
		o + "DCON_RELEASE_INLINE @obj@_const_fat_id operator*() const noexcept" + block{
			o + "return @obj@_const_fat_id(container, @obj@_id(@obj@_id::value_base_t(index)));";
		};

		if(obj.store_type != storage_type::erasable) {
			o + "DCON_RELEASE_INLINE const_object_iterator_@obj@& operator+=(int32_t n) noexcept" + block{
				o + "index = uint32_t(int32_t(index) + n);";
				o + "return *this;";
			};
			o + "DCON_RELEASE_INLINE const_object_iterator_@obj@& operator-=(int32_t n) noexcept" + block{
				o + "index = uint32_t(int32_t(index) - n);";
				o + "return *this;";
			};
			o + "DCON_RELEASE_INLINE const_object_iterator_@obj@ operator+(int32_t n) const noexcept" + block{
				o + "return const_object_iterator_@obj@(container, uint32_t(int32_t(index) + n));";
			};
			o + "DCON_RELEASE_INLINE const_object_iterator_@obj@ operator-(int32_t n) const noexcept" + block{
				o + "return const_object_iterator_@obj@(container, uint32_t(int32_t(index) - n));";
			};
			o + "DCON_RELEASE_INLINE int32_t operator-(const_object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return int32_t(index) - int32_t(o.index);";
			};
			o + "DCON_RELEASE_INLINE bool operator>(const_object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return index > o.index;";
			};
			o + "DCON_RELEASE_INLINE bool operator>=(const_object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return index >= o.index;";
			};
			o + "DCON_RELEASE_INLINE bool operator<(const_object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return index < o.index;";
			};
			o + "DCON_RELEASE_INLINE bool operator<=(const_object_iterator_@obj@ const& o) const noexcept" + block{
				o + "return index <= o.index;";
			};
			o + "DCON_RELEASE_INLINE @obj@_const_fat_id operator[](int32_t n) const noexcept" + block{
				o + "return @obj@_const_fat_id(container, @obj@_id(@obj@_id::value_base_t(int32_t(index) + n)));";
			};
		}
	};

	o + line_break{};
	return o;
}

basic_builder& object_iterator_implementation(basic_builder& o, relationship_object_def const& obj) {
	o + substitute{ "obj", obj.name };

	o + "DCON_RELEASE_INLINE object_iterator_@obj@::object_iterator_@obj@(data_container& c, uint32_t i) noexcept : container(c), index(i)" + block{
		if(obj.store_type == storage_type::erasable) {
			o + "while(container.@obj@.m__index.vptr()[index] != @obj@_id(@obj@_id::value_base_t(index)) && index < container.@obj@.size_used)" + block{
				o + "++index;";
			};
		}
	};
	o + "DCON_RELEASE_INLINE const_object_iterator_@obj@::const_object_iterator_@obj@(data_container const& c, uint32_t i) noexcept : container(c), index(i)" + block{
		if(obj.store_type == storage_type::erasable) {
			o + "while(container.@obj@.m__index.vptr()[index] != @obj@_id(@obj@_id::value_base_t(index)) && index < container.@obj@.size_used)" + block{
				o + "++index;";
			};
		}
	};
	o + "DCON_RELEASE_INLINE object_iterator_@obj@& object_iterator_@obj@::operator++() noexcept" + block{
		if(obj.store_type == storage_type::erasable) {
			o + "++index;";
			o + "while(container.@obj@.m__index.vptr()[index] != @obj@_id(@obj@_id::value_base_t(index)) && index < container.@obj@.size_used)" + block{
				o + "++index;";
			};
		} else {
			o + "++index;";
		}
		o + "return *this;";
	};
	o + "DCON_RELEASE_INLINE const_object_iterator_@obj@& const_object_iterator_@obj@::operator++() noexcept" + block{
		if(obj.store_type == storage_type::erasable) {
			o + "++index;";
			o + "while(container.@obj@.m__index.vptr()[index] != @obj@_id(@obj@_id::value_base_t(index)) && index < container.@obj@.size_used)" + block{
				o + "++index;";
			};
		} else {
			o + "++index;";
		}
		o + "return *this;";
	};
	o + "DCON_RELEASE_INLINE object_iterator_@obj@& object_iterator_@obj@::operator--() noexcept" + block{
		if(obj.store_type == storage_type::erasable) {
			o + "--index;";
			o + "while(container.@obj@.m__index.vptr()[index] != @obj@_id(@obj@_id::value_base_t(index)) && index < container.@obj@.size_used)" + block{
				o + "--index;";
			};
		} else {
			o + "--index;";
		}
		o + "return *this;";
	};
	o + "DCON_RELEASE_INLINE const_object_iterator_@obj@& const_object_iterator_@obj@::operator--() noexcept" + block{
		if(obj.store_type == storage_type::erasable) {
			o + "--index;";
			o + "while(container.@obj@.m__index.vptr()[index] != @obj@_id(@obj@_id::value_base_t(index)) && index < container.@obj@.size_used)" + block{
				o + "--index;";
			};
		} else {
			o + "--index;";
		}
		o + "return *this;";
	};

	o + line_break{};
	return o;
}

basic_builder& make_iterate_over_objects(basic_builder& o, relationship_object_def const& obj) {
	o + substitute{ "obj", obj.name };

	o + "template <typename T>";
	o + "DCON_RELEASE_INLINE void for_each_@obj@(T&& func)" + block{
		o + "for(uint32_t i = 0; i < @obj@.size_used; ++i)" + block{
			o + "@obj@_id tmp = @obj@_id(@obj@_id::value_base_t(i));";
			if(obj.store_type == storage_type::erasable) {
				o + "if(@obj@.m__index.vptr()[tmp.index()] == tmp) func(tmp);";
			} else {
				o + "func(tmp);";
			}
		};
	};

	auto const appstr = std::string(" in_") + obj.name + " ;";

	o + "friend internal::const_object_iterator_@obj@;";
	o + "friend internal::object_iterator_@obj@;";
	
	o + "struct" + block{
		o + "internal::object_iterator_@obj@ begin()" + block{
			o + "data_container* container = reinterpret_cast<data_container*>(reinterpret_cast<std::byte*>(this) - offsetof(data_container, in_@obj@));";
			o + "return internal::object_iterator_@obj@(*container, uint32_t(0));";
		};
		o + "internal::object_iterator_@obj@ end()" + block{
			o + "data_container* container = reinterpret_cast<data_container*>(reinterpret_cast<std::byte*>(this) - offsetof(data_container, in_@obj@));";
			o + "return internal::object_iterator_@obj@(*container, container->@obj@_size());";
		};
		o + "internal::const_object_iterator_@obj@ begin() const" + block{
			o + "data_container const* container = reinterpret_cast<data_container const*>(reinterpret_cast<std::byte const*>(this) - offsetof(data_container, in_@obj@));";
			o + "return internal::const_object_iterator_@obj@(*container, uint32_t(0));";
		};
		o + "internal::const_object_iterator_@obj@ end() const" + block{
			o + "data_container const* container = reinterpret_cast<data_container const*>(reinterpret_cast<std::byte const*>(this) - offsetof(data_container, in_@obj@));";
			o + "return internal::const_object_iterator_@obj@(*container, container->@obj@_size());";
		};
	} + append{ appstr.c_str() };
	o + line_break{};
	return o;
}

std::optional<std::string> to_fat_index_type(file_def const& parsed_file, std::string const& original_name, bool is_const) {
	for(auto& o : parsed_file.relationship_objects) {
		if(o.name + "_id" == original_name) {
			return std::optional<std::string>(o.name + (is_const ? "_const_fat_id" : "_fat_id"));
		}
	}
	return std::optional<std::string>();
}

basic_builder& make_const_fat_id(basic_builder& o, relationship_object_def const& obj, file_def const& parsed_file) {
	o + substitute{ "obj", obj.name };

	o + "class @obj@_const_fat_id" + class_block{
		o + "friend data_container;";
		o + "public:";
		o + "data_container const& container;";
		o + "@obj@_id id;";
		o + "@obj@_const_fat_id(data_container const& c, @obj@_id i) noexcept : container(c), id(i) {}";
		o + "@obj@_const_fat_id(@obj@_const_fat_id const& o) noexcept : container(o.container), id(o.id) {}";
		o + "@obj@_const_fat_id(@obj@_fat_id const& o) noexcept : container(o.container), id(o.id) {}";

		o + "DCON_RELEASE_INLINE operator @obj@_id() const noexcept { return id; }";
		o + "DCON_RELEASE_INLINE @obj@_const_fat_id& operator=(@obj@_const_fat_id const& other) noexcept" + block{
			o + "assert(&container == &other.container);";
			o + "id = other.id;";
			o + "return *this;";
		};
		o + "DCON_RELEASE_INLINE @obj@_const_fat_id& operator=(@obj@_fat_id const& other) noexcept" + block{
			o + "assert(&container == &other.container);";
			o + "id = other.id;";
			o + "return *this;";
		};
		o + "DCON_RELEASE_INLINE @obj@_const_fat_id& operator=(@obj@_id other) noexcept" + block{
			o + "id = other;";
			o + "return *this;";
		};
		o + "DCON_RELEASE_INLINE bool operator==(@obj@_const_fat_id const& other) const noexcept" + block{
			o + "assert(&container == &other.container);";
			o + "return id == other.id;";
		};
		o + "DCON_RELEASE_INLINE bool operator==(@obj@_fat_id const& other) const noexcept" + block{
			o + "assert(&container == &other.container);";
			o + "return id == other.id;";
		};
		o + "DCON_RELEASE_INLINE bool operator==(@obj@_id other) const noexcept" + block{
			o + "return id == other;";
		};
		o + "DCON_RELEASE_INLINE bool operator!=(@obj@_const_fat_id const& other) const noexcept" + block{
			o + "assert(&container == &other.container);";
			o + "return id != other.id;";
		};
		o + "DCON_RELEASE_INLINE bool operator!=(@obj@_fat_id const& other) const noexcept" + block{
			o + "assert(&container == &other.container);";
			o + "return id != other.id;";
		};
		o + "DCON_RELEASE_INLINE bool operator!=(@obj@_id other) const noexcept" + block{
			o + "return id != other;";
		};
		o + "DCON_RELEASE_INLINE explicit operator bool() const noexcept { return bool(id); }";

		make_object_member_declarations(o, parsed_file, obj, false, true /*dec mode*/, "", true /*const mode*/);
		o.declaration_mode = false;
	};

	o + "DCON_RELEASE_INLINE bool operator==(@obj@_fat_id const& l, @obj@_const_fat_id const& other) noexcept" + block{
		o + "assert(&l.container == &other.container);";
		o + "return l.id == other.id;";
	};
	o + "DCON_RELEASE_INLINE bool operator!=(@obj@_fat_id const& l, @obj@_const_fat_id const& other) noexcept" + block{
		o + "assert(&l.container == &other.container);";
		o + "return l.id != other.id;";
	};
	o + "DCON_RELEASE_INLINE @obj@_const_fat_id fatten(data_container const& c, @obj@_id id) noexcept" + block{
		o + "return @obj@_const_fat_id(c, id);";
	};

	o + line_break{};
	return o;
}
basic_builder& make_fat_id(basic_builder& o, relationship_object_def const& obj, file_def const& parsed_file) {
	o + substitute{ "obj", obj.name };

	o + "class @obj@_fat_id" + class_block{
		o + "friend data_container;";
		o + "public:";
		o + "data_container& container;";
		o + "@obj@_id id;";
		o + "@obj@_fat_id(data_container& c, @obj@_id i) noexcept : container(c), id(i) {}";
		o + "@obj@_fat_id(@obj@_fat_id const& o) noexcept : container(o.container), id(o.id) {}";

		o + "DCON_RELEASE_INLINE operator @obj@_id() const noexcept { return id; }";
		o + "DCON_RELEASE_INLINE @obj@_fat_id& operator=(@obj@_fat_id const& other) noexcept" + block{
			o + "assert(&container == &other.container);";
			o + "id = other.id;";
			o + "return *this;";
		};
		o + "DCON_RELEASE_INLINE @obj@_fat_id& operator=(@obj@_id other) noexcept" + block{
			o + "id = other;";
			o + "return *this;";
		};
		o + "DCON_RELEASE_INLINE bool operator==(@obj@_fat_id const& other) const noexcept" + block{
			o + "assert(&container == &other.container);";
			o + "return id == other.id;";
		};
		o + "DCON_RELEASE_INLINE bool operator==(@obj@_id other) const noexcept" + block{
			o + "return id == other;";
		};
		o + "DCON_RELEASE_INLINE bool operator!=(@obj@_fat_id const& other) const noexcept" + block{
			o + "assert(&container == &other.container);";
			o + "return id != other.id;";
		};
		o + "DCON_RELEASE_INLINE bool operator!=(@obj@_id other) const noexcept" + block{
			o + "return id != other;";
		};
		o + "explicit operator bool() const noexcept { return bool(id); }";
		
		make_object_member_declarations(o, parsed_file, obj, false, true /*dec mode*/, "", false /*const mode*/);
		o.declaration_mode = false;
	};

	o + "DCON_RELEASE_INLINE @obj@_fat_id fatten(data_container& c, @obj@_id id) noexcept" + block{
		o + "return @obj@_fat_id(c, id);";
	};

	o + line_break{};
	return o;
}

basic_builder& make_const_fat_id_impl(basic_builder& o, relationship_object_def const& obj, file_def const& parsed_file) {
	make_object_member_declarations(o, parsed_file, obj, false, false /*dec mode*/, obj.name + "_const_fat_id::", true /*const mode*/);
	return o;
}
basic_builder& make_fat_id_impl(basic_builder& o, relationship_object_def const& obj, file_def const& parsed_file) {
	make_object_member_declarations(o, parsed_file, obj, false, false /*dec mode*/, obj.name + "_fat_id::", false /*const mode*/);
	return o;
}

basic_builder& make_composite_key_declarations(basic_builder& o, std::string const& obj_name, composite_index_def const& cc) {
	std::string parameter_string;
	for(auto& k : cc.component_indexes) {
		if(parameter_string.length() != 0)
			parameter_string += ", ";
		if(k.multiplicity == 1) {
			parameter_string += k.object_type + "_id " + k.property_name + "_p";
		} else {
			parameter_string += "std::array<" + k.object_type + "_id, " + std::to_string(k.multiplicity) + "> const& " + k.property_name + "_p";
		}
	}
	o + substitute{ "ccname", cc.name } +substitute{ "obj", obj_name }
	+substitute{ "intvalue", std::to_string(cc.total_bytes) } +substitute{ "params", parameter_string };
	if(cc.total_bytes > 8) {
		o + "ankerl::unordered_dense::map<dcon::key_data_extended<@intvalue@>, "
			"@obj@_id, detail::internal_hash_@intvalue@> hashm_@ccname@;";
		o + "dcon::key_data_extended<@intvalue@> to_@ccname@_keydata(@params@)" + block{
			o + "dcon::key_data_extended<@intvalue@> result;";
			for(auto& k : cc.component_indexes) {
				if(k.multiplicity == 1) {
					o + substitute{ "byte", std::to_string(k.bit_position / 8) };
					o + substitute{ "bit", std::to_string(k.bit_position & 7) };
					o + substitute{ "param", k.property_name + "_p" };
					if(k.number_of_bits > 16) {
						o + "*(reinterpret_cast<uint32_t*>(result.values.data() + @byte@)) = "
							"uint32_t(*(reinterpret_cast<uint32_t*>(result.values.data() + @byte@)) | "
							"(uint32_t(@param@.value) << @bit@));";
					} else if(k.number_of_bits > 8) {
						o + "*(reinterpret_cast<uint16_t*>(result.values.data() + @byte@)) = "
							"uint16_t(*(reinterpret_cast<uint16_t*>(result.values.data() + @byte@)) | "
							"(uint16_t(@param@.value) << @bit@));";
					} else {
						o + "*(reinterpret_cast<uint8_t*>(result.values.data() + @byte@)) = "
							"uint8_t(*(reinterpret_cast<uint8_t*>(result.values.data() + @byte@)) | "
							"(uint8_t(@param@.value) << @bit@));";
					}
				} else {
					for(int32_t i = 0; i < k.multiplicity; ++i) {
						o + substitute{ "byte", std::to_string((k.bit_position + k.number_of_bits * i) / 8) };
						o + substitute{ "bit", std::to_string((k.bit_position + k.number_of_bits * i) & 7) };
						o + substitute{ "i", std::to_string(i) };
						o + substitute{ "param", k.property_name + "_p" };
						if(k.number_of_bits > 16) {
							o + "*(reinterpret_cast<uint32_t*>(result.values.data() + @byte@)) = "
								"uint32_t(*(reinterpret_cast<uint32_t*>(result.values.data() + @byte@)) | "
								"(uint32_t(@param@[@i@].value) << @bit@));";
						} else if(k.number_of_bits > 8) {
							o + "*(reinterpret_cast<uint16_t*>(result.values.data() + @byte@)) = "
								"uint16_t(*(reinterpret_cast<uint16_t*>(result.values.data() + @byte@)) | "
								"(uint16_t(@param@[@i@].value) << @bit@));";
						} else {
							o + "*(reinterpret_cast<uint8_t*>(result.values.data() + @byte@)) = "
								"uint8_t(*(reinterpret_cast<uint8_t*>(result.values.data() + @byte@)) | "
								"(uint8_t(@param@[@i@].value) << @bit@));";
						}
					}
				}
			}
			o + "return result;";
		};
	} else if(cc.total_bytes > 4) {
		o + "ankerl::unordered_dense::map<uint64_t, "
			"@obj@_id, ankerl::unordered_dense::hash<uint64_t>> hashm_@ccname@;";
		o + "uint64_t to_@ccname@_keydata(@params@)" + block{
			o + "uint64_t result = 0;";
			for(auto& k : cc.component_indexes) {
				if(k.multiplicity == 1) {
					o + substitute{ "bit", std::to_string(k.bit_position) };
					o + substitute{ "param", k.property_name + "_p" };
					o + "result |= (uint64_t(@param@.value) << @bit@);";
				} else {
					for(int32_t i = 0; i < k.multiplicity; ++i) {
						o + substitute{ "bit", std::to_string(k.bit_position + k.number_of_bits * i) };
						o + substitute{ "param", k.property_name + "_p" };
						o + substitute{ "i", std::to_string(i) };
						o + "result |= (uint64_t(@param@[@i@].value) << @bit@);";
					}
				}
			}
			o + "return result;";
		};
	} else if(cc.total_bytes > 2) {
		o + "ankerl::unordered_dense::map<uint32_t, "
			"@obj@_id, ankerl::unordered_dense::hash<uint32_t>> hashm_@ccname@;";
		o + "uint32_t to_@ccname@_keydata(@params@)" + block{
			o + "uint32_t result = 0;";
			for(auto& k : cc.component_indexes) {
				if(k.multiplicity == 1) {
					o + substitute{ "bit", std::to_string(k.bit_position) };
					o + substitute{ "param", k.property_name + "_p"};
					o + "result |= (uint32_t(@param@.value) << @bit@);";
				} else {
					for(int32_t i = 0; i < k.multiplicity; ++i) {
						o + substitute{ "bit", std::to_string(k.bit_position + k.number_of_bits * i) };
						o + substitute{ "param", k.property_name + "_p" };
						o + substitute{ "i", std::to_string(i) };
						o + "result |= (uint32_t(@param@[@i@].value) << @bit@);";
					}
				}
			}
			o + "return result;";
		};
	} else { // case uint16_t
		o + "ankerl::unordered_dense::map<uint16_t, "
			"@obj@_id, ankerl::unordered_dense::hash<uint16_t>> hashm_@ccname@;";
		o + "uint16_t to_@ccname@_keydata(@params@)" + block{
			o + "uint16_t result = 0;";
			for(auto& k : cc.component_indexes) {
				if(k.multiplicity == 1) {
					o + substitute{ "bit", std::to_string(k.bit_position) };
					o + substitute{ "param", k.property_name + "_p"};
					o + "result |= (uint16_t(@param@.value) << @bit@);";
				} else {
					for(int32_t i = 0; i < k.multiplicity; ++i) {
						o + substitute{ "bit", std::to_string(k.bit_position + k.number_of_bits * i) };
						o + substitute{ "param", k.property_name + "_p" };
						o + substitute{ "i", std::to_string(i) };
						o + "result |= (uint16_t(@param@[@i@].value) << @bit@);";
					}
				}
			}
			o + "return result;";
		};
	}

	o + line_break{};
	return o;
}


basic_builder& make_composite_key_getter(basic_builder& o, std::string const& obj_name, composite_index_def const& cc) {
	std::string outer_params;
	std::string inner_params;
	for(auto& k : cc.component_indexes) {
		if(outer_params.length() > 0) {
			outer_params += ", ";
			inner_params += ", ";
		}
		
		if(k.multiplicity == 1) {
			outer_params += k.object_type + "_id " + k.property_name + "_p";
			inner_params += k.property_name + "_p";
		} else {
			outer_params += k.object_type + "_id " + k.property_name + "_p0";
			inner_params += "{" + k.property_name + "_p0";
			for(int32_t i = 1; i < k.multiplicity; ++i) {
				outer_params += ", " +  k.object_type + "_id " + k.property_name + "_p" + std::to_string(i);
				inner_params += ", " + k.property_name + "_p" + std::to_string(i);
			}
			inner_params += "}";
		}
	}
	o + substitute{"obj", obj_name } +substitute{ "ckname", cc.name }+substitute{ "oparams", outer_params }
		+ substitute{ "iparams", inner_params };
	o + "@obj@_id get_@obj@_by_@ckname@(@oparams@)" + block{
		for(auto& k : cc.component_indexes) {
			if(k.multiplicity > 1) {
				o + substitute{ "prop", k.property_name };
				o + substitute{ "mult", std::to_string(k.multiplicity) };
				o + substitute{ "id_type", k.object_type + "_id" };

				o + "std::array<@id_type@, @mult@> @prop@_p =" + class_block{
					for(int32_t i = 0; i < k.multiplicity; ++i) {
						o + substitute{ "i", std::to_string(i) };
						o + "@prop@_p@i@,";
					}
				};
				o + "std::sort(@prop@_p.begin(), @prop@_p.end(), [](@id_type@ a, @id_type@ b){ return a.value < b.value; });";
			}
		}
		o + "if(auto it = @obj@.hashm_@ckname@.find(@obj@.to_@ckname@_keydata(@iparams@)); it != @obj@.hashm_@ckname@.end())" + block{
			o + "return it->second;";
		};
		o + "return @obj@_id();";
	};
	return o;
}
