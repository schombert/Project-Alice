#include "code_fragments.hpp"

 
basic_builder& make_serialize_plan_generator(basic_builder& o, file_def const& parsed_file, load_save_def const& rt) {

	o + substitute{ "rt_name", rt.name };

	o + "load_record make_serialize_record_@rt_name@() const noexcept" + block{
		o + "load_record result;";

		for (auto& ob : parsed_file.relationship_objects) {
			o + substitute{ "obj", ob.name };


			bool matched_obj_tag = false;
			for (auto& tg : rt.obj_tags) {
				matched_obj_tag = matched_obj_tag || (std::find(ob.obj_tags.begin(), ob.obj_tags.end(), tg) != ob.obj_tags.end());
			}
			if ((matched_obj_tag && rt.objects_filter == filter_type::include) ||
				(!matched_obj_tag && rt.objects_filter != filter_type::include)) {

				o + "result.@obj@ = true;";
				for (auto& iob : ob.indexed_objects) {
					o + substitute{ "prop", iob.property_name };
					o + "result.@obj@_@prop@ = true;";

				}
				if (ob.store_type == storage_type::erasable) {
					o + "result.@obj@__index = true;";
				}
				for (auto& prop : ob.properties) {
					bool matched_prop_tag = false;
					for (auto& tg : rt.property_tags) {
						matched_prop_tag = matched_prop_tag
							|| (std::find(prop.property_tags.begin(), prop.property_tags.end(), tg) != prop.property_tags.end());
					}

					if ((matched_prop_tag && rt.properties_filter == filter_type::include) ||
						(!matched_prop_tag && rt.properties_filter != filter_type::include)) {
						o + substitute{ "prop", prop.name };
						o + "result.@obj@_@prop@ = true;";
					}
				}
			}
		}
		o + "return result;";
	};

	o + line_break{};
	return o;
}

basic_builder& make_serialize_all_generator(basic_builder& o, file_def const& parsed_file) {

	o + "load_record serialize_entire_container_record() const noexcept" + block{
		o + "load_record result;";

		for (auto& ob : parsed_file.relationship_objects) {
			o + substitute{ "obj", ob.name };
			o + "result.@obj@ = true;";
			for (auto& iob : ob.indexed_objects) {
				o + substitute{ "prop", iob.property_name };
				o + "result.@obj@_@prop@ = true;";
			}
			if (ob.store_type == storage_type::erasable) {
				o + "result.@obj@__index = true;";
			}
			for (auto& prop : ob.properties) {
				o + substitute{ "prop", prop.name };
				o + "result.@obj@_@prop@ = true;";
			}
		}
		o + "return result;";
	};

	o + line_break{};
	return o;
}

basic_builder& make_serialize_size(basic_builder& o, file_def const& parsed_file) {
	o + heading{ "calculate size (in bytes) required to serialize the desired objects, relationships, and properties" };

	o + "uint64_t serialize_size(load_record const& serialize_selection) const" + block{
		o + "uint64_t total_size = 0;";

		for (auto& ob : parsed_file.relationship_objects) {
			o + substitute{ "obj", ob.name };

			o + "if(serialize_selection.@obj@)" + block{
				o + "dcon::record_header header(0, \"uint32_t\", \"@obj@\", \"$size\");";
				o + "total_size += header.serialize_size();";
				o + "total_size += sizeof(uint32_t);";

				for (auto& iob : ob.indexed_objects) {
					o + substitute{ "prop", iob.property_name } + substitute{ "type", iob.type_name };
					o + substitute{ "key_backing", size_to_tag_type(iob.related_to->size) };
					o + substitute{ "mult", std::to_string(iob.multiplicity) };

					if (iob.multiplicity == 1) {
						if (ob.primary_key != iob) {
							o + "if(serialize_selection.@obj@_@prop@)" + block{
								o + "dcon::record_header iheader(0, \"@key_backing@\", \"@obj@\", \"@prop@\");";
								o + "total_size += iheader.serialize_size();";
								o + "total_size += sizeof(@type@_id) * @obj@.size_used;";
							};
						}
					} else {
						o + "if(serialize_selection.@obj@_@prop@)" + block{
							  o + "dcon::record_header iheader(0, \"std::array<@key_backing@,@mult@>\", \"@obj@\", \"@prop@\");";
							  o + "total_size += iheader.serialize_size();";
							  o + "total_size += sizeof(std::array<@type@_id, @mult@>) * @obj@.size_used;";
						};
					}
				}

				if (ob.is_relationship) {
					o + "dcon::record_header headerb(0, \"$\", \"@obj@\", \"$index_end\");";
					o + "total_size += headerb.serialize_size();";
				}
			};

			if (ob.store_type == storage_type::erasable) {
				o + substitute{ "key_backing", size_to_tag_type(ob.size) };
				o + "if(serialize_selection.@obj@__index)" + block{
					o + "dcon::record_header iheader(0, \"@key_backing@\", \"@obj@\", \"_index\");";
					o + "total_size += iheader.serialize_size();";
					o + "total_size += sizeof(@obj@_id) * @obj@.size_used;";
				};
			}

			for (auto& prop : ob.properties) {
				o + substitute{ "prop", prop.name } + substitute{ "type", normalize_type(prop.data_type) };
				if (prop.is_derived) {

				} else if (prop.type == property_type::bitfield) {
					o + "if(serialize_selection.@obj@_@prop@)" + block{
						 o + "dcon::record_header iheader(0, \"bitfield\", \"@obj@\", \"@prop@\");";
						 o + "total_size += iheader.serialize_size();";
						 o + "total_size += (@obj@.size_used + 7) / 8;";
					};
				} else if (prop.type == property_type::special_vector) {
					o + substitute{ "vtype_name_sz", std::to_string(prop.data_type.length() + 1) };
					o + "if(serialize_selection.@obj@_@prop@)" + block{
						o + "std::for_each(@obj@.m_@prop@.vptr(), @obj@.m_@prop@.vptr() + @obj@.size_used, [t = this, &total_size](dcon::stable_mk_2_tag obj)" + block{
							o + "auto rng = dcon::get_range(t->@obj@.@prop@_storage, obj);";
							o + "total_size += sizeof(uint16_t);";
							o + "total_size += sizeof(@type@) * (rng.second - rng.first);";
						} + append{");"};

						o + inline_block{
							o + "total_size += @vtype_name_sz@;";
							o + "dcon::record_header iheader(0, \"stable_mk_2_tag\", \"@obj@\", \"@prop@\");";
							o + "total_size += iheader.serialize_size();";
						};
					};
				} else if (prop.type == property_type::object) {
					o + "if(serialize_selection.@obj@_@prop@)" + block{
						o + "std::for_each(@obj@.m_@prop@.vptr(), @obj@.m_@prop@.vptr() + @obj@.size_used, "
							"[t = this, &total_size](@type@ const& obj){ total_size += t->serialize_size(obj); });";
						o + "dcon::record_header iheader(0, \"@type@\", \"@obj@\", \"@prop@\");";
						o + "total_size += iheader.serialize_size();";
					};
				} else if (prop.type == property_type::array_bitfield) {
					o + substitute{ "vtype_name_sz", std::to_string(strlen("bitfield") + 1) };
					o + "if(serialize_selection.@obj@_@prop@)" + block{
						o + "total_size += @vtype_name_sz@;";
						o + "total_size += sizeof(uint16_t);";
						o + "total_size += @obj@.m_@prop@.size * (@obj@.size_used + 7) / 8;";
						o + "dcon::record_header iheader(0, \"$array\", \"@obj@\", \"@prop@\");";
						o + "total_size += iheader.serialize_size();";
					};
				} else if (prop.type == property_type::array_vectorizable || prop.type == property_type::array_other) {
					o + substitute{ "vtype_name_sz", std::to_string(prop.data_type.length() + 1) };
					o + "if(serialize_selection.@obj@_@prop@)" + block{
						o + "total_size += @vtype_name_sz@;";
						o + "total_size += sizeof(uint16_t);";
						o + "total_size += @obj@.m_@prop@.size * sizeof(@type@) * @obj@.size_used;";
						o + "dcon::record_header iheader(0, \"$array\", \"@obj@\", \"@prop@\");";
						o + "total_size += iheader.serialize_size();";
					};
				} else {
					o + "if(serialize_selection.@obj@_@prop@)" + block{
						o + "dcon::record_header iheader(0, \"@type@\", \"@obj@\", \"@prop@\");";
						o + "total_size += iheader.serialize_size();";
						o + "total_size += sizeof(@type@) * @obj@.size_used;";
					};
				}
			}
		}
	o + "return total_size;";
	};

	o + line_break{};
	return o;
}

void make_serialize_single_object(basic_builder& o, const relationship_object_def& ob) {
	o + substitute{ "obj", ob.name };
	o + "if(serialize_selection.@obj@)" + block{
		o + "dcon::record_header header(sizeof(uint32_t), \"uint32_t\", \"@obj@\", \"$size\");";
		o + "header.serialize(output_buffer);";
		o + "*(reinterpret_cast<uint32_t*>(output_buffer)) = @obj@.size_used;";
		o + "output_buffer += sizeof(uint32_t);";

		for (auto& iob : ob.indexed_objects) {
			if (ob.primary_key != iob) {
				o + substitute{ "prop", iob.property_name } + substitute{ "type", iob.type_name };
				o + substitute{ "u_type", size_to_tag_type(iob.related_to->size) };
				o + substitute{ "mult", std::to_string(iob.multiplicity) };

				if (iob.multiplicity == 1) {
					o + inline_block{
						o + "dcon::record_header iheader(sizeof(@type@_id) * @obj@.size_used, \"@u_type@\", \"@obj@\", \"@prop@\");";
						o + "iheader.serialize(output_buffer);";
						o + "std::memcpy(reinterpret_cast<@type@_id*>(output_buffer), @obj@.m_@prop@.vptr(), sizeof(@type@_id) * @obj@.size_used);";
						o + "output_buffer += sizeof(@type@_id) *  @obj@.size_used;";
					};
				} else {
					o + inline_block{
						o + "dcon::record_header iheader(sizeof(std::array<@type@_id, @mult@>) * @obj@.size_used, \"std::array<@u_type@,@mult@>\", \"@obj@\", \"@prop@\");";
						o + "iheader.serialize(output_buffer);";
						o + "std::memcpy(reinterpret_cast<std::array<@type@_id, @mult@>*>(output_buffer), @obj@.m_@prop@.vptr(), sizeof(std::array<@type@_id, @mult@>) * @obj@.size_used);";
						o + "output_buffer += sizeof(std::array<@type@_id, @mult@>) *  @obj@.size_used;";
					};
				}
			}
		}

		if (ob.is_relationship) {
			o + "dcon::record_header headerb(0, \"$\", \"@obj@\", \"$index_end\");";
			o + "headerb.serialize(output_buffer);";
		}
	};

	if (ob.store_type == storage_type::erasable) {
		o + substitute{ "u_type", size_to_tag_type(ob.size) };
		o + "if(serialize_selection.@obj@__index)" + block{
			o + "dcon::record_header header(sizeof(@obj@_id) * @obj@.size_used, \"@u_type@\", \"@obj@\", \"_index\");";
			o + "header.serialize(output_buffer);";
			o + "std::memcpy(reinterpret_cast<@obj@_id*>(output_buffer), @obj@.m__index.vptr(), sizeof(@obj@_id) * @obj@.size_used);";
			o + "output_buffer += sizeof(@obj@_id) * @obj@.size_used;";
		};
	}

	for (auto& prop : ob.properties) {
		o + substitute{ "prop", prop.name } + substitute{ "type", normalize_type(prop.data_type) };
		if (prop.is_derived) {

		} else if (prop.type == property_type::bitfield) {
			o + "if(serialize_selection.@obj@_@prop@)" + block{
				o + "dcon::record_header header((@obj@.size_used + 7) / 8, \"bitfield\", \"@obj@\", \"@prop@\");";
				o + "header.serialize(output_buffer);";
				o + "std::memcpy(reinterpret_cast<dcon::bitfield_type*>(output_buffer), @obj@.m_@prop@.vptr(), (@obj@.size_used + 7) / 8);";
				o + "output_buffer += (@obj@.size_used + 7) / 8;";
			};
		} else if (prop.type == property_type::special_vector) {
			o + "if(serialize_selection.@obj@_@prop@)" + block{
				o + "size_t total_size = 0;";
				o + "std::for_each(@obj@.m_@prop@.vptr(), @obj@.m_@prop@.vptr() + @obj@.size_used, [t = this, &total_size](dcon::stable_mk_2_tag obj)" + block{
					o + "auto rng = dcon::get_range(t->@obj@.@prop@_storage, obj);";
					o + "total_size += sizeof(uint16_t) + sizeof(@type@) * (rng.second - rng.first);";
				} + append{ ");" };

				o + substitute{ "vname_sz", std::to_string(prop.data_type.length() + 1) };
				o + "total_size += @vname_sz@;";

				o + "dcon::record_header header(total_size, \"stable_mk_2_tag\", \"@obj@\", \"@prop@\");";
				o + "header.serialize(output_buffer);";

				o + "std::memcpy(reinterpret_cast<char*>(output_buffer), \"@type@\", @vname_sz@);";
				o + "output_buffer += @vname_sz@;";

				o + "std::for_each(@obj@.m_@prop@.vptr(), @obj@.m_@prop@.vptr() + @obj@.size_used, [t = this, &output_buffer](dcon::stable_mk_2_tag obj)" + block{
					o + "auto rng = dcon::get_range(t->@obj@.@prop@_storage, obj);";
					o + "*(reinterpret_cast<uint16_t*>(output_buffer)) = uint16_t(rng.second - rng.first);";
					o + "output_buffer += sizeof(uint16_t);";
					o + "std::memcpy(reinterpret_cast<@type@*>(output_buffer), rng.first, sizeof(@type@) * (rng.second - rng.first));";
					o + "output_buffer += sizeof(@type@) * (rng.second - rng.first);";
				} + append{ ");" };
			};
		} else if (prop.type == property_type::object) {
			o + "if(serialize_selection.@obj@_@prop@)" + block{
				o + "size_t total_size = 0;";
				o + "std::for_each(@obj@.m_@prop@.vptr(), @obj@.m_@prop@.vptr() + @obj@.size_used, [t = this, &total_size](@type@ const& obj)" + block{
					o + "total_size += t->serialize_size(obj);";
				} + append{ ");" };
				o + "dcon::record_header header(total_size, \"@type@\", \"@obj@\", \"@prop@\");";
				o + "header.serialize(output_buffer);";
				o + "std::for_each(@obj@.m_@prop@.vptr(), @obj@.m_@prop@.vptr() + @obj@.size_used, "
					"[t = this, &output_buffer](@type@ const& obj){ t->serialize(output_buffer, obj); });";
			};
		} else if (prop.type == property_type::array_bitfield) {
			o + substitute{ "vname_sz", std::to_string(strlen("bitfield") + 1) };

			o + "if(serialize_selection.@obj@_@prop@)" + block{
				o + "dcon::record_header header(@vname_sz@ + sizeof(uint16_t) + @obj@.m_@prop@.size * (@obj@.size_used + 7) / 8, \"$array\", \"@obj@\", \"@prop@\");";
				o + "header.serialize(output_buffer);";

				o + "std::memcpy(reinterpret_cast<char*>(output_buffer), \"bitfield\", @vname_sz@);";
				o + "output_buffer += @vname_sz@;";

				o + "*(reinterpret_cast<uint16_t*>(output_buffer)) = uint16_t(@obj@.m_@prop@.size);";
				o + "output_buffer += sizeof(uint16_t);";

				o + "for(int32_t s = 0; s < int32_t(@obj@.m_@prop@.size); ++s)" + block{
					o + "std::memcpy(reinterpret_cast<dcon::bitfield_type*>(output_buffer), @obj@.m_@prop@.vptr(s), (@obj@.size_used + 7) / 8);";
					o + "output_buffer += (@obj@.size_used + 7) / 8;";
				};
			};
		} else if (prop.type == property_type::array_vectorizable || prop.type == property_type::array_other) {
			o + substitute{ "vname_sz", std::to_string(prop.data_type.length() + 1) };

			o + "if(serialize_selection.@obj@_@prop@)" + block{
				o + "dcon::record_header header(@vname_sz@ + sizeof(uint16_t) + sizeof(@type@) * @obj@.m_@prop@.size * @obj@.size_used, \"$array\", \"@obj@\", \"@prop@\");";
				o + "header.serialize(output_buffer);";

				o + "std::memcpy(reinterpret_cast<char*>(output_buffer), \"@type@\", @vname_sz@);";
				o + "output_buffer += @vname_sz@;";

				o + "*(reinterpret_cast<uint16_t*>(output_buffer)) = uint16_t(@obj@.m_@prop@.size);";
				o + "output_buffer += sizeof(uint16_t);";

				o + "for(int32_t s = 0; s < int32_t(@obj@.m_@prop@.size); ++s)" + block{
					o + "std::memcpy(reinterpret_cast<@type@*>(output_buffer), @obj@.m_@prop@.vptr(s), sizeof(@type@) * @obj@.size_used);";
					o + "output_buffer +=  sizeof(@type@) * @obj@.size_used;";
				};
			};
		} else {
			o + "if(serialize_selection.@obj@_@prop@)" + block{
				o + "dcon::record_header header(sizeof(@type@) * @obj@.size_used, \"@type@\", \"@obj@\", \"@prop@\");";
				o + "header.serialize(output_buffer);";
				o + "std::memcpy(reinterpret_cast<@type@*>(output_buffer), @obj@.m_@prop@.vptr(), sizeof(@type@) * @obj@.size_used);";
				o + "output_buffer += sizeof(@type@) * @obj@.size_used;";
			};
		}
	}
}

basic_builder& make_serialize(basic_builder& o, file_def const& parsed_file) {
	o + heading{ "serialize the desired objects, relationships, and properties" };

	//serialize
	o + "void serialize(std::byte*& output_buffer, load_record const& serialize_selection) const" + block{
		for (auto& ob : parsed_file.relationship_objects) {
			if (!ob.is_relationship)
				make_serialize_single_object(o, ob);
		}
		for (auto& ob : parsed_file.relationship_objects) { // serialize relationships last
			if (ob.is_relationship)
				make_serialize_single_object(o, ob);
		}
	};

	o + line_break{};
	return o;
}

basic_builder& wrong_type_cast(basic_builder& o, std::string const& wrong_type, std::string const& prop_name,
	std::string const& prop_type, bool cast_to_value = true) {
	o + substitute{ "w_type", wrong_type };
	o + substitute{ "w_prop", prop_name };
	o + substitute{ "c_type", prop_type };
	o + "else if(header.is_type(\"@w_type@\"))" + block{
		o + "for(uint32_t i = 0; i < std::min(@obj@.size_used, uint32_t(header.record_size / sizeof(@w_type@))); ++i)" + block{
			if (cast_to_value)
				o + "@obj@.m_@w_prop@.vptr()[i].value = @c_type@(*(reinterpret_cast<@w_type@ const*>(input_buffer) + i));";
			else
				o + "@obj@.m_@w_prop@.vptr()[i] = @c_type@(*(reinterpret_cast<@w_type@ const*>(input_buffer) + i));";
		};
		o + "serialize_selection.@obj@_@w_prop@ = true;";
	};
	return o;
}

basic_builder& wrong_type_cast_with_multiplicity(basic_builder& o, std::string const& wrong_type, std::string const& prop_name,
	std::string const& prop_type, int32_t multiplicity) {
	o + substitute{ "w_type", wrong_type };
	o + substitute{ "w_prop", prop_name };
	o + substitute{ "c_type", prop_type };
	o + substitute{ "mult", std::to_string(multiplicity) };

	o + "else if(header.is_type(\"std::array<@w_type@,@mult@>\"))" + block{
		o + "for(uint32_t i = 0; i < std::min(@obj@.size_used, uint32_t(header.record_size / sizeof(std::array<@w_type@,@mult@>))); ++i)" + block{
			for (int32_t j = 0; j < multiplicity; ++j) {
				o + substitute{ "j", std::to_string(j) };
				o + "@obj@.m_@w_prop@.vptr()[i][@j@].value = @c_type@((*(reinterpret_cast<std::array<@w_type@,@mult@> const*>(input_buffer) + i))[@j@]);";
			}
		};
		o + "serialize_selection.@obj@_@w_prop@ = true;";
	};
	return o;
}

void make_single_deserialize_from_icpy(basic_builder& o, std::string const& destination_expression,
	std::string const& from_type, std::string const& to_type, bool as_object, bool to_bitfield) {

	o + substitute{ "convert_from", from_type };
	o + substitute{ "convert_to", to_type };

	if(as_object) {
		o + "@convert_from@ temp;";
		o + "deserialize(icpy, temp, input_buffer + header.record_size);";
		if(to_bitfield) {
			o + (std::string("dcon::bit_vector_set(") + destination_expression +
				", convert_type(temp, (@convert_to@*)nullptr));");
		} else {
			o + (destination_expression + " = std::move(convert_type(temp, (@convert_to@*)nullptr));");
		}
	} else {
		if(to_bitfield) {
			o + (std::string("dcon::bit_vector_set(") + destination_expression + 
				", convert_type(*(reinterpret_cast<@convert_from@ const*>(input_buffer) + i), (@convert_to@*)nullptr));");
		} else {
			o + destination_expression + " = std::move(convert_type(*(reinterpret_cast<@convert_from@ const*>(input_buffer) + i), (@convert_to@*)nullptr));";
		}
	}
}

basic_builder& conversion_attempt(basic_builder& o, file_def const& parsed_file, std::string const& target_type, bool to_bitfield = false, bool index_with_s = false) {
	o + substitute{ "s_index", index_with_s ? "s" : "" };
	for (auto& con : parsed_file.conversion_list) {
		if (con.to == target_type) {
			bool to_is_object = std::find(parsed_file.object_types.begin(), parsed_file.object_types.end(), con.from) != parsed_file.object_types.end()
				|| std::find(parsed_file.legacy_types.begin(), parsed_file.legacy_types.end(), con.from) != parsed_file.legacy_types.end();

				
			o + substitute{ "f_type", con.from };
			o + "else if(header.is_type(\"@f_type@\"))" + block{
				o + "std::byte const* icpy = input_buffer;";
				o + "for(uint32_t i = 0; icpy < input_buffer + header.record_size && i < @obj@.size_used; ++i)" + block{
					if(to_bitfield)
						make_single_deserialize_from_icpy(o, "@obj@.m_@prop@.vptr(), i", con.from, target_type, to_is_object, to_bitfield);
					else
						make_single_deserialize_from_icpy(o, "@obj@.m_@prop@.vptr()[i]", con.from, target_type, to_is_object, to_bitfield);
				};
				o + "serialize_selection.@obj@_@prop@ = true;";
			};
		}
	}
	return o;
}

void deserialize_size_fragment(basic_builder& o, relationship_object_def const& ob) {
	o + "if(header.is_property(\"$size\") && header.record_size == sizeof(uint32_t))" + block{
		if(ob.is_relationship) {
			o + "if(*(reinterpret_cast<uint32_t const*>(input_buffer)) >= @obj@.size_used)" + block{
				// TODO: a better fix than this
				o + "@obj@_resize(0);";
			};
		}
		o + "@obj@_resize(*(reinterpret_cast<uint32_t const*>(input_buffer)));";
		o + "serialize_selection.@obj@ = true;";
	};
}

void deserialize_erasable_index_fragment(basic_builder& o, relationship_object_def const& ob, bool with_mask) {
	o + substitute{ "u_type" , size_to_tag_type(ob.size) };
	o + substitute{ "mcon", with_mask ? std::string(" && mask.") + ob.name + "__index" : std::string() };
	o + "else if(header.is_property(\"__index\")@mcon@)" + block{
		o + "if(header.is_type(\"@u_type@\"))" + block{
			o + "std::memcpy(@obj@.m__index.vptr(), reinterpret_cast<@u_type@ const*>(input_buffer), "
				"std::min(size_t(@obj@.size_used) * sizeof(@u_type@), header.record_size));";
			o + "serialize_selection.@obj@__index = true;";
		};
		if(size_to_tag_type(ob.size) != "uint8_t") {
			wrong_type_cast(o, "uint8_t", "_index", size_to_tag_type(ob.size), true);
		}
		if(size_to_tag_type(ob.size) != "uint16_t") {
			wrong_type_cast(o, "uint16_t", "_index", size_to_tag_type(ob.size), true);
		}
		if(size_to_tag_type(ob.size) != "uint32_t") {
			wrong_type_cast(o, "uint32_t", "_index", size_to_tag_type(ob.size), true);
		}

		//redo free list
		o + "if(serialize_selection.@obj@__index == true)" + block{
			o + "@obj@.size_used = 0;";
			o + "@obj@.first_free = @obj@_id();";
			o + "for(int32_t j = @obj_sz@ - 1; j > 0; --j)" + block{
				o + "if(@obj@.m__index.vptr()[j] != @obj@_id(@u_type@(j)))" + block{
					o + "@obj@.m__index.vptr()[j] = @obj@.first_free;";
					o + "@obj@.first_free = @obj@_id(@u_type@(j));";
				} + append{"else"} + block{
					o + "@obj@.size_used = std::max(@obj@.size_used, uint32_t(j));";
				};
			};
		};
	};
}

void deserialize_individual_link_fragment(basic_builder& o, relationship_object_def const& ob,
	related_object const& iob, bool with_mask) {

	if(ob.primary_key != iob) {
		o + substitute{ "prop", iob.property_name } + substitute{ "ob_u_type", size_to_tag_type(ob.size) };
		o + substitute{ "mcon", with_mask ? std::string(" && mask.") + ob.name + "_" + iob.property_name : std::string() };
		o + substitute{ "u_type" , size_to_tag_type(iob.related_to->size) };
		o + substitute{ "mult", std::to_string(iob.multiplicity) };

		//std::array<@u_type@,@mult@>

		if(iob.multiplicity == 1) {
			o + "else if(header.is_property(\"@prop@\")@mcon@)" + block{
				o + "if(header.is_type(\"@u_type@\"))" + block{
					o + "std::memcpy(@obj@.m_@prop@.vptr(), reinterpret_cast<@u_type@ const*>(input_buffer), "
						"std::min(size_t(@obj@.size_used) * sizeof(@u_type@), header.record_size));";
					o + "serialize_selection.@obj@_@prop@ = true;";
				};
				if(size_to_tag_type(iob.related_to->size) != "uint8_t") {
					wrong_type_cast(o, "uint8_t", iob.property_name, size_to_tag_type(iob.related_to->size), true);
				}
				if(size_to_tag_type(iob.related_to->size) != "uint16_t") {
					wrong_type_cast(o, "uint16_t", iob.property_name, size_to_tag_type(iob.related_to->size), true);
				}
				if(size_to_tag_type(iob.related_to->size) != "uint32_t") {
					wrong_type_cast(o, "uint32_t", iob.property_name, size_to_tag_type(iob.related_to->size), true);
				}
			};
		} else {
			o + "else if(header.is_property(\"@prop@\")@mcon@)" + block{
				o + "if(header.is_type(\"std::array<@u_type@,@mult@>\"))" + block{
					o + "std::memcpy(@obj@.m_@prop@.vptr(), reinterpret_cast<std::array<@u_type@,@mult@> const*>(input_buffer), "
					"std::min(size_t(@obj@.size_used) * sizeof(std::array<@u_type@, @mult@>), header.record_size));";
					o + "serialize_selection.@obj@_@prop@ = true;";
				};
				if(size_to_tag_type(iob.related_to->size) != "uint8_t") {
					wrong_type_cast_with_multiplicity(o, "uint8_t", iob.property_name, size_to_tag_type(iob.related_to->size), iob.multiplicity);
				}
				if(size_to_tag_type(iob.related_to->size) != "uint16_t") {
					wrong_type_cast_with_multiplicity(o, "uint16_t", iob.property_name, size_to_tag_type(iob.related_to->size), iob.multiplicity);
				}
				if(size_to_tag_type(iob.related_to->size) != "uint32_t") {
					wrong_type_cast_with_multiplicity(o, "uint32_t", iob.property_name, size_to_tag_type(iob.related_to->size), iob.multiplicity);
				}
			};
		}
	}
}

void deserialize_relationship_end_links_fragment(basic_builder& o, relationship_object_def const& ob,
	bool with_mask) {

	o + substitute{ "mcon", with_mask ? std::string(" && mask.") + ob.name : std::string() };
	o + "else if(header.is_property(\"$index_end\")@mcon@)" + block{
		for(auto& iob : ob.indexed_objects) {
			if(ob.primary_key != iob) {
				o + substitute{ "prop", iob.property_name } + substitute{ "type", iob.type_name };
				o + "if(serialize_selection.@obj@_@prop@ == true)" + block{
					o + "for(uint32_t i = 0; i < @obj@.size_used; ++i)" + block{
						if(iob.multiplicity == 1) {
							o + "auto tmp = @obj@.m_@prop@.vptr()[i];";
							o + "@obj@.m_@prop@.vptr()[i] = @type@_id();";
							o + "internal_@obj@_set_@prop@(@obj@_id(@obj@_id::value_base_t(i)), tmp);";
						} else {
							for(int32_t i = 0; i < iob.multiplicity; ++i) {
								o + substitute{ "i", std::to_string(i) };
								o + inline_block{
									o + "auto tmp = @obj@.m_@prop@.vptr()[i][@i@];";
									o + "@obj@.m_@prop@.vptr()[i][@i@] = @type@_id();";
									o + "internal_@obj@_set_@prop@(@obj@_id(@obj@_id::value_base_t(i)), @i@, tmp);";
								};
							}
						}
					};
				};
			}
		}

		for(auto& cc : ob.composite_indexes) {
			std::string params;
			for(auto& idx : cc.component_indexes) {
				if(params.length() > 0)
					params += ", ";
				if(idx.property_name == ob.primary_key.property_name)
					params += ob.primary_key.points_to->name + "_id(" + ob.primary_key.points_to->name + "_id::value_base_t(idx))";
				else
					params += ob.name + ".m_" + idx.property_name + ".vptr()[idx]";
			}
			o + substitute{ "params", params } + substitute{"ckname", cc.name};

			o + "for(uint32_t idx = 0; idx < @obj@.size_used; ++idx)" + block{
				o + "auto this_key = @obj@_id(@obj@_id::value_base_t(idx));";
				o + "if(@obj@_is_valid(@obj@_id(@obj@_id::value_base_t(idx))))" + block{
					o + "auto key_dat = @obj@.to_@ckname@_keydata(@params@);";
					o + "if(auto it = @obj@.hashm_@ckname@.find(key_dat); it !=  @obj@.hashm_@ckname@.end())" + block{
						o + "delete_@obj@(it->second);";
					};
					o + "@obj@.hashm_@ckname@.insert_or_assign(key_dat, this_key);";
				};
			};
		}
	};
}

void deserialize_bitfield_property_fragment(basic_builder& o, file_def const& parsed_file,
	relationship_object_def const& ob, property_def const& prop, bool with_mask) {

	o + "if(header.is_type(\"bitfield\"))" + block{
		o + "std::memcpy(@obj@.m_@prop@.vptr(), reinterpret_cast<dcon::bitfield_type const*>(input_buffer)"
			", std::min(size_t(@obj@.size_used + 7) / 8, header.record_size));";
		o + "serialize_selection.@obj@_@prop@ = true;";
	};
	conversion_attempt(o, parsed_file, "bool", true);
}

void deserialize_special_vector_property_fragment(basic_builder& o, file_def const& parsed_file,
	relationship_object_def const& ob, property_def const& prop, bool with_mask) {

	o + "if(header.is_type(\"stable_mk_2_tag\"))" + block{
		o + "uint32_t ix = 0;";
		//read internal data type
		o + "std::byte const* zero_pos = std::find(input_buffer, input_buffer + header.record_size, std::byte(0));";

		 o + "if(dcon::char_span_equals_str(reinterpret_cast<char const*>(input_buffer), "
			"reinterpret_cast<char const*>(zero_pos), \"@type@\"))" + block{ //correct type

			o + "for(std::byte const* icpy = zero_pos + 1; ix < @obj@.size_used && icpy < input_buffer + header.record_size; )" + block{
				o + "uint16_t sz = 0;";

				o + "if(icpy + sizeof(uint16_t) <= input_buffer + header.record_size)" + block{
					o + "sz = uint16_t(std::min(size_t(*(reinterpret_cast<uint16_t const*>(icpy))), "
						"(input_buffer + header.record_size - (icpy + sizeof(uint16_t))) / sizeof(@type@) ));";
					o + "icpy += sizeof(uint16_t);";
				};
				o + "dcon::load_range(@obj@.@prop@_storage, @obj@.m_@prop@.vptr()[ix], "
					"reinterpret_cast<@type@ const*>(icpy), reinterpret_cast<@type@ const*>(icpy) + sz);";
				o + "icpy += sz * sizeof(@type@);";
				o + "++ix;";
			};
			o + "serialize_selection.@obj@_@prop@ = true;";
		}; // end correct type

		for(auto& con : parsed_file.conversion_list) {
			if(con.to == prop.data_type) {
				o + substitute{ "f_type", con.from };
				bool as_object = std::find(parsed_file.object_types.begin(), parsed_file.object_types.end(), con.from) != parsed_file.object_types.end()
					|| std::find(parsed_file.legacy_types.begin(), parsed_file.legacy_types.end(), con.from) != parsed_file.legacy_types.end();

				// from is an object
				o + "else if(dcon::char_span_equals_str(reinterpret_cast<char const*>(input_buffer), "
					"reinterpret_cast<char const*>(zero_pos), \"@f_type@\"))" + block{

					o + "for(std::byte const* icpy = zero_pos + 1; ix < @obj@.size_used && icpy < input_buffer + header.record_size; )" + block{
						o + "uint16_t sz = 0;";
						o + "if(icpy + sizeof(uint16_t) <= input_buffer + header.record_size)" + block{
							o + "sz = uint16_t(std::min(size_t(*(reinterpret_cast<uint16_t const*>(icpy))), "
								"(input_buffer + header.record_size - (icpy + sizeof(uint16_t))) / sizeof(@f_type@) ));";
							o + "icpy += sizeof(uint16_t);";
						};

						o + "dcon::resize(@obj@.@prop@_storage, @obj@.m_@prop@.vptr()[ix], sz);";
						o + "for(uint32_t ii = 0; ii < sz && icpy < input_buffer + header.record_size; ++ii)" + block{
							make_single_deserialize_from_icpy(o,
								"dcon::get(@obj@.@prop@_storage, @obj@.m_@prop@.vptr()[ix], ii)",
								con.from, prop.data_type, as_object, false);
						};
						o + "++ix;";
					};
					o + "serialize_selection.@obj@_@prop@ = true;";
				};
			}
		} // end for each in conversion list
		if(is_common_type(normalize_type(prop.data_type))) {
			const auto normed_type = normalize_type(prop.data_type);
			for(auto& basic_type : common_types) {
				if(basic_type != normed_type) {
					o + substitute{"b_type", basic_type };
					o + "else if(dcon::char_span_equals_str(reinterpret_cast<char const*>(input_buffer), "
						"reinterpret_cast<char const*>(zero_pos), \"@b_type@\"))" + block{

						o + "for(std::byte const* icpy = zero_pos + 1; ix < @obj@.size_used && icpy < input_buffer + header.record_size; )" + block{
							o + "uint16_t sz = 0;";
							o + "if(icpy + sizeof(uint16_t) <= input_buffer + header.record_size)" + block{
								o + "sz = uint16_t(std::min(size_t(*(reinterpret_cast<uint16_t const*>(icpy))), "
									"(input_buffer + header.record_size - (icpy + sizeof(uint16_t))) / sizeof(@b_type@) ));";
								o + "icpy += sizeof(uint16_t);";
							};

							o + "dcon::resize(@obj@.@prop@_storage, @obj@.m_@prop@.vptr()[ix], sz);";
							o + "for(uint32_t ii = 0; ii < sz && icpy < input_buffer + header.record_size; ++ii)" + block{
								o + "dcon::get(@obj@.@prop@_storage, @obj@.m_@prop@.vptr()[ix], ii) = @type@(*(reinterpret_cast<@b_type@ const*>(icpy)));";
								o + "icpy += sizeof(@b_type@);";
							};
							o + "++ix;";
						};
						o + "serialize_selection.@obj@_@prop@ = true;";
					};
				}
			}
		}
	}; // end if prop is special array
}

void deserialize_object_property_fragment(basic_builder& o, file_def const& parsed_file,
	relationship_object_def const& ob, property_def const& prop, bool with_mask) {

	o + "if(header.is_type(\"@type@\"))" + block{
		o + "std::byte const* icpy = input_buffer;";
		o + "for(uint32_t i = 0; icpy < input_buffer + header.record_size && i < @obj@.size_used; ++i)" + block{
			o + "deserialize(icpy, @obj@.m_@prop@.vptr()[i], input_buffer + header.record_size);";
		};
		o + "serialize_selection.@obj@_@prop@ = true;";
	};
	conversion_attempt(o, parsed_file, prop.data_type, false);
}

void deserialize_a_bitfield_property_fragment(basic_builder& o, file_def const& parsed_file,
	relationship_object_def const& ob, property_def const& prop, bool with_mask) {
	
	o + "if(header.is_type(\"$array\"))" + block{
		o + "std::byte const* zero_pos = std::find(input_buffer, input_buffer + header.record_size, std::byte(0));";
		o + "std::byte const* icpy = zero_pos + 1;";
		o + "if(dcon::char_span_equals_str(reinterpret_cast<char const*>(input_buffer), "
			"reinterpret_cast<char const*>(zero_pos), \"bitfield\"))" + block{ //correct type
			o + "if(icpy + sizeof(uint16_t) <= input_buffer + header.record_size)" + block{
				o + "@obj@.m_@prop@.resize(*(reinterpret_cast<uint16_t const*>(icpy)), @obj@.size_used);";
				o + "icpy += sizeof(uint16_t);";
			} + append{ "else" } + block{
				o + "@obj@.m_@prop@.resize(0, @obj@.size_used);";
			};
			o + "for(int32_t s = 0; s < int32_t(@obj@.m_@prop@.size) && icpy < input_buffer + header.record_size; ++s)" + block{
				o + "std::memcpy(@obj@.m_@prop@.vptr(s), reinterpret_cast<dcon::bitfield_type const*>(icpy)"
				", std::min(size_t(@obj@.size_used + 7) / 8, size_t(input_buffer + header.record_size - icpy)));";
				o + "icpy += (@obj@.size_used + 7) / 8;";
			};
			o + "serialize_selection.@obj@_@prop@ = true;";
		}; // end correct type
		for(auto& con : parsed_file.conversion_list) {
			if(con.to == prop.data_type) {
				o + substitute{ "f_type", con.from };
				bool as_object = std::find(parsed_file.object_types.begin(), parsed_file.object_types.end(), con.from) != parsed_file.object_types.end()
					|| std::find(parsed_file.legacy_types.begin(), parsed_file.legacy_types.end(), con.from) != parsed_file.legacy_types.end();

				o + "else if(dcon::char_span_equals_str(reinterpret_cast<char const*>(input_buffer), "
					"reinterpret_cast<char const*>(zero_pos), \"@f_type@\"))" + block{

					o + "if(icpy + sizeof(uint16_t) <= input_buffer + header.record_size)" + block{
						o + "@obj@.m_@prop@.resize(*(reinterpret_cast<uint16_t const*>(icpy)), @obj@.size_used);";
						o + "icpy += sizeof(uint16_t);";
					} + append{ "else" } + block{
						o + "@obj@.m_@prop@.resize(0, @obj@.size_used);";
					};
					o + "for(int32_t s = 0; s < int32_t(@obj@.m_@prop@.size) && icpy < input_buffer + header.record_size; ++s)" + block{
						o + "for(uint32_t j = 0; j < @obj@.size_used && icpy < input_buffer + header.record_size; ++j)" + block{
							make_single_deserialize_from_icpy(o,
								"@obj@.m_@prop@.vptr(s), j",
								con.from, prop.data_type, as_object, true);
						};
					};
					o + "serialize_selection.@obj@_@prop@ = true;";
				};
			}
		} // end for each in conversion list
	};
}

void deserialize_a_other_property_fragment(basic_builder& o, file_def const& parsed_file,
	relationship_object_def const& ob, property_def const& prop, bool with_mask) {

	o + "if(header.is_type(\"$array\"))" + block{
		o + "std::byte const* zero_pos = std::find(input_buffer, input_buffer + header.record_size, std::byte(0));";
		o + "std::byte const* icpy = zero_pos + 1;";
		o + "if(dcon::char_span_equals_str(reinterpret_cast<char const*>(input_buffer), "
			"reinterpret_cast<char const*>(zero_pos), \"@type@\"))" + block{ //correct type
			o + "if(icpy + sizeof(uint16_t) <= input_buffer + header.record_size)" + block{
				o + "@obj@.m_@prop@.resize(*(reinterpret_cast<uint16_t const*>(icpy)), @obj@.size_used);";
				o + "icpy += sizeof(uint16_t);";
			} + append{ "else" } + block{
				o + "@obj@.m_@prop@.resize(0, @obj@.size_used);";
			};
			o + "for(int32_t s = 0; s < int32_t(@obj@.m_@prop@.size) && icpy < input_buffer + header.record_size; ++s)" + block{
				o + "std::memcpy(@obj@.m_@prop@.vptr(s), reinterpret_cast<@type@ const*>(icpy)"
					", std::min(sizeof(@type@) * @obj@.size_used, size_t(input_buffer + header.record_size - icpy)));";
				o + "icpy += sizeof(@type@) * @obj@.size_used;";
			};
			o + "serialize_selection.@obj@_@prop@ = true;";
		}; // end correct type
		for(auto& con : parsed_file.conversion_list) {
			if(con.to == prop.data_type) {
				o + substitute{ "f_type", con.from };
				bool as_object = std::find(parsed_file.object_types.begin(), parsed_file.object_types.end(), con.from) != parsed_file.object_types.end()
					|| std::find(parsed_file.legacy_types.begin(), parsed_file.legacy_types.end(), con.from) != parsed_file.legacy_types.end();

				o + "else if(dcon::char_span_equals_str(reinterpret_cast<char const*>(input_buffer), "
					"reinterpret_cast<char const*>(zero_pos), \"@f_type@\"))" + block{

					o + "if(icpy + sizeof(uint16_t) <= input_buffer + header.record_size)" + block{
						o + "@obj@.m_@prop@.resize(*(reinterpret_cast<uint16_t const*>(icpy)), @obj@.size_used);";
						o + "icpy += sizeof(uint16_t);";
					} + append{ "else" } + block{
						o + "@obj@.m_@prop@.resize(0, @obj@.size_used);";
					};
					o + "for(int32_t s = 0; s < int32_t(@obj@.m_@prop@.size) && icpy < input_buffer + header.record_size; ++s)" + block{
						o + "for(uint32_t j = 0; j < @obj@.size_used && icpy < input_buffer + header.record_size; ++j)" + block{
							make_single_deserialize_from_icpy(o,
								"@obj@.m_@prop@.vptr(s)[j]",
								con.from, prop.data_type, as_object, false);
						};
					};
					o + "serialize_selection.@obj@_@prop@ = true;";
				};
			}
		} // end for each in conversion list
		if(is_common_type(normalize_type(prop.data_type))) {
			const auto normed_type = normalize_type(prop.data_type);
			for(auto& basic_type : common_types) {
				if(basic_type != normed_type) {
					o + substitute{ "b_type", basic_type };
					o + "else if(dcon::char_span_equals_str(reinterpret_cast<char const*>(input_buffer), "
						"reinterpret_cast<char const*>(zero_pos), \"@b_type@\"))" + block{

						o + "if(icpy + sizeof(uint16_t) <= input_buffer + header.record_size)" + block{
							o + "@obj@.m_@prop@.resize(*(reinterpret_cast<uint16_t const*>(icpy)), @obj@.size_used);";
							o + "icpy += sizeof(uint16_t);";
						} + append{ "else" } + block{
							o + "@obj@.m_@prop@.resize(0, @obj@.size_used);";
						};
						o + "for(int32_t s = 0; s < int32_t(@obj@.m_@prop@.size) && icpy < input_buffer + header.record_size; ++s)" + block{
							o + "for(uint32_t j = 0; j < @obj@.size_used && icpy < input_buffer + header.record_size; ++j)" + block{
								o + "@obj@.m_@prop@.vptr(s)[j] = @type@(*(reinterpret_cast<@b_type@ const*>(icpy)));";
								o + "icpy += sizeof(@b_type@);";
							};
						};
						o + "serialize_selection.@obj@_@prop@ = true;";
					};
				}
			}
		}
	};
}

void deserialize_basic_property_fragment(basic_builder& o, file_def const& parsed_file,
	relationship_object_def const& ob, property_def const& prop, bool with_mask) {

	o + "if(header.is_type(\"@type@\"))" + block{
		o + "std::memcpy(@obj@.m_@prop@.vptr(), reinterpret_cast<@type@ const*>(input_buffer)"
			", std::min(size_t(@obj@.size_used) * sizeof(@type@), header.record_size));";
		o + "serialize_selection.@obj@_@prop@ = true;";
	};
	conversion_attempt(o, parsed_file, prop.data_type, false);
	if(is_common_type(normalize_type(prop.data_type))) {
		const auto normed_type = normalize_type(prop.data_type);
		for(auto& basic_type : common_types) {
			if(basic_type != normed_type) {
				wrong_type_cast(o, basic_type, prop.name, prop.data_type, false);
			}
		}
	}
}

void deserialize_individual_property_fragment(basic_builder& o, file_def const& parsed_file,
	relationship_object_def const& ob, property_def const& prop, bool with_mask) {

	o + substitute{ "prop", prop.name } + substitute{ "type", normalize_type(prop.data_type) };
	o + substitute{ "mcon", with_mask ? std::string(" && mask.") + ob.name + "_" + prop.name : std::string() };
	o + "else if(header.is_property(\"@prop@\")@mcon@)" + block{
		if(prop.is_derived) {

		} else if(prop.type == property_type::bitfield) {
			deserialize_bitfield_property_fragment(o, parsed_file, ob, prop, with_mask);
		} else if(prop.type == property_type::special_vector) {
			deserialize_special_vector_property_fragment(o, parsed_file, ob, prop, with_mask);
		} else if(prop.type == property_type::object) {
			deserialize_object_property_fragment(o, parsed_file, ob, prop, with_mask);
		} else if(prop.type == property_type::array_bitfield) {
			deserialize_a_bitfield_property_fragment(o, parsed_file, ob, prop, with_mask);
		} else if(prop.type == property_type::array_vectorizable || prop.type == property_type::array_other) {
			deserialize_a_other_property_fragment(o, parsed_file, ob, prop, with_mask);
		} else {
			deserialize_basic_property_fragment(o, parsed_file, ob, prop, with_mask);
		} // end prop type cases
	}; // end header property == property type" in output 
}

basic_builder& make_deserialize(basic_builder& o, file_def const& parsed_file, bool with_mask) {
	if (with_mask)
		o + heading{ "deserialize the desired objects, relationships, and properties where the mask is set" };
	else
		o + heading{ "deserialize the desired objects, relationships, and properties" };

	o + substitute{ "mask_param", with_mask ? ", load_record const& mask" : "" };

	o + "void deserialize(std::byte const*& input_buffer, std::byte const* end, load_record& serialize_selection@mask_param@)" + block{
		o + "while(input_buffer < end)" + block{
			o + "dcon::record_header header;";
			o + "header.deserialize(input_buffer, end);";

			// wrap: gaurantee enough space to read entire buffer
			o + "if(input_buffer + header.record_size <= end)" + block{
				bool first_header_if = true;
				for (auto& ob : parsed_file.relationship_objects) {
					o + substitute{ "obj", ob.name } + substitute{ "obj_sz", std::to_string(ob.size) }
					+ substitute{ "mcon", with_mask ? std::string(" && mask.") + ob.name : std::string() };
					if (first_header_if)
						first_header_if = false;
					else
						o + append{ "else" };

					o + "if(header.is_object(\"@obj@\")@mcon@)" + block{ //has matched object
						deserialize_size_fragment(o, ob);
						if (ob.store_type == storage_type::erasable) {
							deserialize_erasable_index_fragment(o, ob, with_mask);
						} // end: load index handling for erasable

						for (auto& iob : ob.indexed_objects) {
							deserialize_individual_link_fragment(o, ob, iob, with_mask);
						} // end index properties

						if (ob.is_relationship) {
							deserialize_relationship_end_links_fragment(o, ob, with_mask);
						}

						for (auto& prop : ob.properties) {
							deserialize_individual_property_fragment(o, parsed_file, ob, prop, with_mask);
						} // end loop over object properties 

					}; // end "header object == object type" in output

				} // end loop over object and relation types
			}; // end if ensuring that buffer has enough space to read entire record
			o + "input_buffer += header.record_size;";
		};
	};

	o + line_break{};
	return o;
}
