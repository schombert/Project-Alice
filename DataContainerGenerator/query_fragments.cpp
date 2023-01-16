#include "code_fragments.hpp"

basic_builder& make_query_instance_types(basic_builder& o, prepared_query_definition const& pdef) {
	o + substitute{ "name", pdef.name };

	std::string param_list;
	std::string arg_list;
	int32_t i = 0;

	for(auto& param : pdef.parameters) {
		param_list += ", ";
		arg_list += ", ";
		param_list += param.type + " p" + std::to_string(i);
		arg_list += param.name + "(p" + std::to_string(i) + ")";
		++i;
	}
	
	o + substitute{ "param_list", param_list };
	o + substitute{ "arg_list", arg_list };

	o + "class query_@name@_iterator;";
	o + "class query_@name@_const_iterator;";

	o + "class query_@name@_instance" + class_block{
		o + "public:";
		
		o + "data_container& container;";
		
		for(auto& param : pdef.parameters) {
			o + substitute{ "type", param.type };
			o + substitute{ "pname", param.name };

			o + "@type@ @pname@;";
		}

		o + "query_@name@_instance(data_container& c @param_list@) : container(c) @arg_list@ {}";

		o + "query_@name@_iterator begin();";

		o + "dcon::invalid_iterator_type end() const" + block{
			o + "return dcon::invalid_iterator_type{};";
		};
	};

	o + "class query_@name@_const_instance" + class_block{
		o + "public:";

		o + "data_container const& container;";

		for(auto& param : pdef.parameters) {
			o + substitute{ "type", param.type };
			o + substitute{ "pname", param.name };

			o + "@type@ @pname@;";
		}

		o + "query_@name@_const_instance(data_container const& c @param_list@) : container(c) @arg_list@ {}";

		o + "query_@name@_const_iterator begin();";

		o + "dcon::invalid_iterator_type end() const" + block{
			o + "return dcon::invalid_iterator_type{};";
		};
	};

	o + line_break{};
	return o;
}

basic_builder& make_query_instance_definitions(basic_builder& o, prepared_query_definition const& pdef) {
	o + substitute{ "name", pdef.name };

	o + "query_@name@_iterator query_@name@_instance::begin()" + block{
			o + "return query_@name@_iterator(container, *this);";
	};
	o + "query_@name@_const_iterator query_@name@_const_instance::begin()" + block{
			o + "return query_@name@_const_iterator(container, *this);";
	};
	return o;
}

basic_builder& make_query_iterator_declarations(basic_builder& o, prepared_query_definition const& pdef) {
	o + substitute{ "name", pdef.name };

	o + "class query_@name@_instance;";
	o + "class query_@name@_const_instance;";

	o + "class query_@name@_iterator" + class_block{
		o + "private:";
		o + "data_container& m_container;";
		o + "query_@name@_instance& m_parameters;";

		for(auto& agg : pdef.exposed_aggregates) {
			if(agg.aggregate_name == "count")
				o + substitute{ "type", "int32_t" };
			else if(agg.from_link)
				o + substitute{ "type", agg.from_link->type_name + "_id" };
			else if(agg.from_property)
				o + substitute{ "type", agg.from_property->data_type };
			o + substitute{ "pname", agg.exposed_name };

			o + "@type@ m_@pname@;";
		}
		for(auto& mm : pdef.exposed_min_max_terms) {
			o + substitute{ "type", mm.derived_from_slot->actual_table->name + "_id" };
			o + substitute{ "pname", mm.exposed_name };

			o + "@type@ m_@pname@;";
		}
		for(auto& ts : pdef.table_slots) {
			if(!ts.is_parameter_type) {
				o + substitute{ "type", ts.actual_table->name + "_id" };
				o + substitute{ "pname", ts.internally_named_as };

				o + "@type@ @pname@;";
				if(ts.joind_by_link && ts.joind_by_link->multiplicity > 1 && !ts.actual_table->is_relationship) {
					o + "int32_t m_index_into_@pname@ = 0;";
				} else if(ts.joind_by_link && ts.joind_by_link->index == index_type::many && ts.joind_by_link->ltype != list_type::list
					&& ts.actual_table->is_relationship) {
					o + "int32_t m_index_into_@pname@ = 0;";
					o + "int32_t m_size_of_@pname@ = 0;";
				}
			}
		}

		o + "public:";

		o + "query_@name@_iterator(data_container& c, query_@name@_instance& p) : m_container(c), m_parameters(p)" + block{
			o + substitute{ "fname", pdef.table_slots[0].internally_named_as };
			o + substitute{ "obj", pdef.table_slots[0].actual_table->name };

			if(pdef.table_slots[0].is_parameter_type) {
				o + "internal_init(@fname@);";
			} else {
				o + "if(m_container.@obj@_size() > 0)" + block{
					o + "internal_init(@obj@_id(@obj@_id::value_base_t(0)));";
				};
			}
		};
		o.declaration_mode = true;
		make_query_iterator_body(o, pdef, "", false);
		o.declaration_mode = false;
	};

	o + "class query_@name@_const_iterator" + class_block{
		o + "private:";
		o + "data_container const& m_container;";
		o + "query_@name@_const_instance& m_parameters;";

		for(auto& agg : pdef.exposed_aggregates) {
			if(agg.aggregate_name == "count")
				o + substitute{ "type", "int32_t" };
			else if(agg.from_link)
				o + substitute{ "type", agg.from_link->type_name + "_id" };
			else if(agg.from_property)
				o + substitute{ "type", agg.from_property->data_type };
			o + substitute{ "pname", agg.exposed_name };

			o + "@type@ m_@pname@;";
		}
		for(auto& mm : pdef.exposed_min_max_terms) {
			o + substitute{ "type", mm.derived_from_slot->actual_table->name + "_id" };
			o + substitute{ "pname", mm.exposed_name };

			o + "@type@ m_@pname@;";
		}
		for(auto& ts : pdef.table_slots) {
			if(!ts.is_parameter_type) {
				o + substitute{ "type", ts.actual_table->name + "_id" };
				o + substitute{ "pname", ts.internally_named_as };

				o + "@type@ @pname@;";
				if(ts.joind_by_link && ts.joind_by_link->multiplicity > 1 && !ts.actual_table->is_relationship) {
					o + "int32_t m_index_into_@pname@ = 0;";
				} else if(ts.joind_by_link && ts.joind_by_link->index == index_type::many && ts.joind_by_link->ltype != list_type::list
					&& ts.actual_table->is_relationship) {
					o + "int32_t m_index_into_@pname@ = 0;";
					o + "int32_t m_size_of_@pname@ = 0;";
				}
			}
		}

		o + "public:";

		o + "query_@name@_const_iterator(data_container const& c, query_@name@_const_instance& p) : m_container(c), m_parameters(p)" + block{
			o + substitute{ "fname", pdef.table_slots[0].internally_named_as };
			o + substitute{ "obj", pdef.table_slots[0].actual_table->name };

			if(pdef.table_slots[0].is_parameter_type) {
				o + "internal_init(@fname@);";
			} else {
				o + "if(m_container.@obj@_size() > 0)" + block{
					o + "internal_init(@obj@_id(@obj@_id::value_base_t(0)));";
				};
			}			
		};
		o.declaration_mode = true;
		make_query_iterator_body(o, pdef, "", true);
		o.declaration_mode = false;

	};

	o + line_break{};
	return o;
}

basic_builder& make_query_iterator_body(basic_builder& o, prepared_query_definition const& pdef, std::string const& ns, bool is_const) {
	o + substitute{ "name", pdef.name };
	o + substitute{ "namesp", ns };
	o + substitute{ "id_type", is_const ? "const_fat_id" : "fat_id" };
	
	
	if(is_const) {
		o + "auto @namesp@operator++() -> query_@name@_const_iterator&" + block{
			o + "internal_increment_to_result(true);";
			o + "return *this;";
		};
	} else {
		o + "auto @namesp@operator++() -> query_@name@_iterator&" + block{
			o + "internal_increment_to_result(true);";
			o + "return *this;";
		};
	}
	o + "bool @namesp@operator==(dcon::invalid_iterator_type)" + block{
		if(pdef.table_slots.size() > 0) {
			o + substitute{"first_id", pdef.table_slots[0].internally_named_as};
			o + "return !bool(@first_id@);";
		} else {
			o + "return true;";
		}
	};
	o + "bool @namesp@operator!=(dcon::invalid_iterator_type)" + block{
		if(pdef.table_slots.size() > 0) {
			o + substitute{"first_id", pdef.table_slots[0].internally_named_as};
			o + "return bool(@first_id@);";
		} else {
			o + "return false;";
		}
	};
	if(is_const) {
		o + "auto @namesp@operator*() -> query_@name@_const_iterator const&" + block{
			o + "return *this;";
		};
	} else {
		o + "auto @namesp@operator*() -> query_@name@_iterator const&" + block{
			o + "return *this;";
		};
	}

	/* internals */

	if(o.declaration_mode)
		o + "private:";

	o + "void @namesp@internal_reset_aggregates()" + block{
		for(auto& agg : pdef.exposed_aggregates) {
			if(agg.aggregate_name == "count")
				o + substitute{ "type", "int32_t" };
			else if(agg.from_link)
				o + substitute{ "type", agg.from_link->type_name + "_id" };
			else if(agg.from_property)
				o + substitute{ "type", agg.from_property->data_type };
			o + substitute{ "pname", agg.exposed_name };

			o + "m_@pname@ = @type@{};";
		}
		for(auto& mm : pdef.exposed_min_max_terms) {
			o + substitute{ "type", mm.derived_from_slot->actual_table->name + "_id" };
			o + substitute{ "pname", mm.exposed_name };

			o + "m_@pname@ = @type@{};";
		}
	};
	o + "void @namesp@internal_set_aggregates()" + block{
		for(auto& agg : pdef.exposed_aggregates) {
			o + substitute{ "pname", agg.exposed_name };

			if(agg.aggregate_name == "count") {
				o + "m_@pname@ = 1;";
			} else if(agg.from_link) {
				o + substitute{ "link_name", agg.from_link->property_name };
				o + substitute{ "int_name", agg.derived_from_slot->internally_named_as };
				o + substitute{ "int_type", agg.derived_from_slot->actual_table->name };
				o + substitute{ "agg", agg.aggregate_name };

				o + "m_@pname@ = m_container.@int_type@_get_@link_name@(@int_name@);";
			} else if(agg.from_property) {
				o + substitute{ "prop_name", agg.from_property->name };
				o + substitute{ "int_name", agg.derived_from_slot->internally_named_as };
				o + substitute{ "int_type", agg.derived_from_slot->actual_table->name };
				o + substitute{ "agg", agg.aggregate_name };

				o + "m_@pname@ = m_container.@int_type@_get_@prop_name@(@int_name@);";
			}
		}
		for(auto& mm : pdef.exposed_min_max_terms) {
			o + substitute{ "int_name", mm.derived_from_slot->internally_named_as };
			o + substitute{ "pname", mm.exposed_name };

			o + "m_@pname@ = @int_name@;";
		}
	};
	o + "void @namesp@internal_update_aggregates()" + block{
		for(auto& agg : pdef.exposed_aggregates) {
			o + substitute{ "pname", agg.exposed_name };

			if(agg.from_link)
				o + substitute{ "prop_name", agg.from_link->property_name };
			else if(agg.from_property)
				o + substitute{ "prop_name", agg.from_property->name };
			if(agg.derived_from_slot) {
				o + substitute{ "int_name", agg.derived_from_slot->internally_named_as };
				o + substitute{ "int_type", agg.derived_from_slot->actual_table->name };
			}

			if(agg.aggregate_name == "count") {
				o + "++m_@pname@;";
			} else if(agg.aggregate_name == "std::min" || agg.aggregate_name == "min") {
				o + "if(bool(@int_name@) && m_container.@int_type@_get_@prop_name@(@int_name@.index) < m_@pname@)" + block{
					for(auto& mm : pdef.exposed_min_max_terms) {
						o + substitute{ "mm_name", mm.exposed_name };
						o + substitute{ "mm_int", mm.derived_from_slot->internally_named_as };
						o + "m_@mm_name@ = @mm_int@;";
					}
					o + "m_@pname@ = m_container.@int_type@_get_@prop_name@(@int_name@);";
				};
			} else if(agg.aggregate_name == "std::max" || agg.aggregate_name == "max") {
				o + "if(bool(@int_name@) && m_@pname@ < m_container.@int_type@_get_@prop_name@(@int_name@) )" + block{
					for(auto& mm : pdef.exposed_min_max_terms) {
						o + substitute{ "mm_name", mm.exposed_name };
						o + substitute{ "mm_int", mm.derived_from_slot->internally_named_as };
						o + "m_@mm_name@ = @mm_int@;";
					}
					o + "m_@pname@ = m_container.@int_type@_get_@prop_name@(@int_name@);";
				};
			} else {
				o + substitute{ "agg", agg.aggregate_name };
				o + "m_@pname@ = @agg@(m_@pname@, m_container.@int_type@_get_@prop_name@(@int_name@));";
			}
		}
	};
	o + substitute{ "first_slot_type",  pdef.table_slots[0].actual_table->name };
	o + "void @namesp@internal_init(@first_slot_type@_id first_value)" + block{
		o + substitute{ "last_index", std::to_string(pdef.table_slots.size() - 1)};
		o + substitute{ "first_id",  pdef.table_slots[0].internally_named_as };
		o + substitute{ "where",  pdef.where_conditional };

		o + "if(internal_set_v0(first_value))" + block{
			if(pdef.where_conditional.length() > 0) {
				o + "if(@where@)" + block{
					if(pdef.has_group) {
						o + "internal_set_aggregates();";
						o + "internal_increment_to_result(false);";
					}
					o + "return;";
				};
			} else {
				if(pdef.has_group) {
					o + "internal_set_aggregates();";
					o + "internal_increment_to_result(false);";
				}
				o + "return;";
			}
		};
		o + "internal_increment_to_result(true);";
	};

	o + (pdef.has_group ? "void @namesp@internal_increment_to_result(bool as_first)" : "void @namesp@internal_increment_to_result(bool)") + block{
		o + substitute{ "last_index", std::to_string(pdef.table_slots.size() - 1)};
		o + substitute{ "first_id",  pdef.table_slots[0].internally_named_as };
		o + substitute{ "where",  pdef.where_conditional };
		if(pdef.table_slots.size() > 0) {
			if(pdef.has_group) {
				o + "if(as_first) internal_reset_aggregates();";
				o + "bool first_time = as_first;";
				o + "while(bool(@first_id@))" + block{
					o + "bool hit_group = false;";
					
					o + "while(bool(@first_id@) && !internal_increment_v@last_index@(first_time, hit_group))" + block{

					};
					
					o + "if(!hit_group)" + block{
						if(pdef.where_conditional.length() > 0) {
							o + "if(bool(@first_id@))" + block{
								o + "if(@where@)" + block{
									o + "if(first_time)" + block{
										o + "first_time = false;";
										o + "internal_set_aggregates();";
									} +append{ "else" } +block{
										o + "internal_update_aggregates();";
									};
								};
							};
						} else {
							o + "if(first_time)" + block{
								o + "first_time = false;";
								o + "internal_set_aggregates();";
							} +append{ "else" } +block{
								o + "internal_update_aggregates();";
							};
						}
					} +append{ "else" } +block{
						o + "return;";
					};
					
				};
			} else {
				o + "bool hit_group = false;";
				o + "while(bool(@first_id@))" + block{
					o + "while(bool(@first_id@) && !internal_increment_v@last_index@(false, hit_group))" + block{

					};
					if(pdef.where_conditional.length() > 0) {
						o + "if(bool(@first_id@))" + block{
							o + "if(@where@)" + block{
								o + "return;";
							};
						};
					} else {
						o + "return;";
					}
				};
			}
		}
	};
	for(int32_t i = 0; i < pdef.table_slots.size(); ++i) {
		auto& table = pdef.table_slots[i];
		o + substitute{ "fname", table.internally_named_as };
		o + substitute{ "obj", table.actual_table->name };
		o + substitute{ "index", std::to_string(i) };
		o + substitute{ "prev_index", std::to_string(i - 1) };

		o + "void @namesp@internal_reset_v@index@()" + block{
			if(table.joind_by_link && table.joind_by_link->multiplicity > 1 && !table.actual_table->is_relationship) {
				o + "m_index_into_@fname@ = 0;";
			} else if(table.joind_by_link && table.joind_by_link->index == index_type::many && table.joind_by_link->ltype != list_type::list
					&& table.actual_table->is_relationship) {
				o + "m_index_into_@fname@ = 0;";
				o + "m_size_of_@fname@ = 0;";
			}
			if(!table.is_parameter_type) {
				o + "@fname@ = @obj@_id();";
			}

			for(int32_t j = i + 1; j < pdef.table_slots.size(); ++j) {
				if(pdef.table_slots[j].joined_to == &table) {
					o + substitute{ "jindex", std::to_string(j) };
					o + "internal_reset_v@jindex@();";
				}
			}
		};

		o + "bool @namesp@internal_set_v@index@(@obj@_id v)" + block{
			if(!table.is_join_plus) {
				o + "if(!bool(v))" + block{
					if(!table.is_parameter_type) {
						o + "@fname@ = v;";
					}
					o + "return false;";
				};
			} else {
				o + "if(!bool(v))" + block{
					o + "internal_reset_v@index@();";
					o + "return true;";
				};
			}
			if(!table.is_parameter_type) {
				o + "@fname@ = v;";
			}
			o + "return ";
			for(int32_t j = i + 1; j < pdef.table_slots.size(); ++j) {
				if(pdef.table_slots[j].joined_to <= &table) {
					auto& ts = pdef.table_slots[j];
					o + substitute{ "par_name", ts.joined_to->internally_named_as };
					o + substitute{ "par_obj",  ts.joined_to->actual_table->name };

					o + substitute{ "lname", ts.joind_by_link->property_name };
					o + substitute{ "cobj", ts.actual_table->name };
					o + substitute{ "next_index", std::to_string(j) };
					o + substitute{ "next_name", pdef.table_slots[j].internally_named_as };

					o + "[&](){";

					if(ts.actual_table->is_relationship) {
						if(ts.joind_by_link->index == index_type::at_most_one) {
							o + "return internal_set_v@next_index@(m_container.@par_obj@_get_@cobj@_as_@lname@(@par_name@));";

						} else if(ts.joind_by_link->index == index_type::many) {
							if(ts.joind_by_link->ltype == list_type::list) {
								o + "auto head = m_container.@cobj@.m_head_back_@lname@.vptr()[@par_name@.index()];";
								o + "if(!bool(head)) return internal_set_v@next_index@(@obj@_id());";
								o + "for( ; bool(head); head = m_container.@obj@.m_link_@lname@.vptr()[@fname@.index()].right)" + block{
									o + "if(internal_set_v@next_index@(head))" + block{
										o + "return true;";
									};
								};
								o + "return false;";
							} else {
								o + "auto range = m_container.@par_obj@_range_of_@cobj@_as_@lname@(@par_name@);";
								o + "m_index_into_@next_name@ = 0;";
								o + "m_size_of_@next_name@ = int32_t(range.second - range.first);";
								o + "if(m_size_of_@next_name@ == 0)" + block{
									o + "return internal_set_v@next_index@( @cobj@_id() );";
								} +append{ "else" } +block{
									o + "for( ; m_index_into_@next_name@ < m_size_of_@next_name@ ; ++ m_index_into_@next_name@)" + block{
										o + "if(internal_set_v@next_index@( *(range.first + m_index_into_@next_name@) ))" + block{
											o + "return true;";
										};
									};
									o + "return false;";
								};
							}
						} else { // type none
							o + "bool found_one = false;";
							o + "for(uint32_t i = 0; i < m_container.@cobj@_size(); ++i)" + block{
								o + "if(m_container.@cobj@_get_@lname@(@cobj@_id(@cobj@_id::value_base_t(i))) == @par_name@)" + block{
									o + "found_one = true;";
									o + "if(internal_set_v@next_index@( @cobj@_id(@cobj@_id::value_base_t(i)) ) )" + block{
										o + "return true;";
									};
								};
							};
							o + "if(!found_one) return internal_set_v@next_index@( @cobj@_id() );";
						}
					} else {
						if(ts.joind_by_link->multiplicity == 1) {
							o + "return internal_set_v@next_index@( m_container.@par_obj@_get_@lname@(@par_name@) );";
						} else {
							o + substitute{ "mul", std::to_string(ts.joind_by_link->multiplicity) };
							o + "m_index_into_@next_name@ = 0;";
							o + "for( ; m_index_into_@next_name@ < @mul@; ++m_index_into_@next_name@)" + block{
								o + "if(internal_set_v@next_index@( m_container.@par_obj@_get_@lname@(@par_name@, m_index_into_@next_name@) ) )" + block{
									o + "return true;";
								};
							};
							o + "return false;";
						}
					}

					o + "}() && ";
				}
			}
			o + "true;";
			
		};

		o + substitute{ "inc_params", i == 0 && !table.is_group_slot ? "bool, bool&" : "bool force, bool& hit_group" };

		o + "bool @namesp@internal_increment_v@index@(@inc_params@)" + block{
			if(i == 0) {
				if(table.is_group_slot) {
					o + "if(!force)" + block{
						o + "hit_group = true;";
						o + "return true;";
					};
				}
				if(table.is_parameter_type) {
					o + "@fname@ = @obj@_id();";
					o + "return false;";
				} else {
					if(table.actual_table->store_type == storage_type::erasable) {
						o + "for(uint32_t i = uint32_t(@fname@.index() + 1); i < m_container.@obj@_size(); ++i)" + block{
							o + "if(m_container.@obj@_is_valid( @obj@_id(@obj@_id::value_base_t(i)) ))" + block{
								o + "if(internal_set_v0( @obj@_id(@obj@_id::value_base_t(i)) )) return true;";
							};
						};
						o + "@fname@ = @obj@_id( );";
						o + "return false;";
					} else {
						o + "for(uint32_t i = uint32_t(@fname@.index() + 1); i < m_container.@obj@_size(); ++i)" + block{
							o + "if(internal_set_v0( @obj@_id(@obj@_id::value_base_t(i)) )) return true;";
						};
						o + "@fname@ = @obj@_id( );";
						o + "return false;";
					}
				}
			} else {
				o + "if(!bool(@fname@))" + block{
					o + "return internal_increment_v@prev_index@(force, hit_group);";
				};

				if(table.is_group_slot) {
					o + "if(!force)" + block{
						o + "hit_group = true;";
						o + "return true;";
					};
				}

				o + substitute{ "lname", table.joind_by_link->property_name };
				if(table.actual_table->is_relationship) {
					o + substitute{ "pname",  table.joined_to->internally_named_as };
					o + substitute{ "pobj",  table.joined_to->actual_table->name };
					if(table.joind_by_link->index == index_type::at_most_one) {
						o + "return internal_increment_v@prev_index@(force, hit_group);";
					} else if(table.joind_by_link->index == index_type::many) {
						if(table.joind_by_link->ltype == list_type::list) {
							o + "auto next = m_container.@obj@.m_link_@lname@.vptr()[@fname@.index()].right;";
							o + "if(bool(next))" + block{
								o + "return internal_set_v@index@(next);";
							} +append{ "else" } +block{
								o + "@fname@ = @obj@_id();";
								o + "return internal_increment_v@prev_index@(force, hit_group);";
							};
						} else {
							o + "if(m_index_into_@fname@ + 1 < m_size_of_@fname@)" + block{
								o + "++m_index_into_@fname@;";
								o + "auto range = m_container.@pobj@_range_of_@obj@_as_@lname@(@pname@);";
								o + "return internal_set_v@index@( *(range.first + m_index_into_@fname@) );";
							} +append{ "else" } +block{
								o + "@fname@ = @obj@_id();";
								o + "return internal_increment_v@prev_index@(force, hit_group);";
							};
						}
					} else { // type none
						o + "for(uint32_t i = uint32_t(@fname@.index()) + 1; i < m_container.@obj@_size(); ++i)" + block{
							o + "if(m_container.@obj@_get_@lname@(@obj@_id(@obj@_id::value_base_t(i))) == @pname@)" + block{
								o + "return internal_set_v@index@( @obj@_id(@obj@_id::value_base_t(i)) );";
							};
						};
						o + "@fname@ = @obj@_id();";
						o + "return internal_increment_v@prev_index@(force, hit_group);";
					}
				} else {
					o + substitute{ "pname",  table.joined_to->internally_named_as };
					o + substitute{ "pobj",  table.joined_to->actual_table->name };

					if(table.joind_by_link->multiplicity == 1) {
						o + "return internal_increment_v@prev_index@(force, hit_group);";
					} else {
						o + substitute{ "mul",  std::to_string(table.joind_by_link->multiplicity) };
						o + "if(m_index_into_@fname@ + 1 < @mul@)" + block{
							o + "++m_index_into_@fname@;";
							o + "return internal_set_v@index@( m_container.@pobj@_get_@lname@(@pname@, m_index_into_@fname@) );";
						} +append{ "else" } +block{
							o + "@fname@ = @obj@_id();";
							o + "return internal_increment_v@prev_index@(force, hit_group);";
						};
					}
				}
			}
		};
	}
	if(o.declaration_mode)
		o + "public:";

	/* getters and setters */

	for(auto& prop : pdef.exposed_values) {
		o + substitute{ "exname", prop.exposed_name };
		o + substitute{ "internal_tag", prop.derived_from_slot->internally_named_as };
		o + substitute{ "obj", prop.derived_from_slot->actual_table->name };

		if(prop.from_link) {
			o + substitute{ "rtype", prop.from_link->type_name };
			o + substitute{ "ptype", prop.from_link->type_name + "_id" };
			o + substitute{ "propname", prop.from_link->property_name };

			o + "@rtype@_@id_type@ @namesp@get_@exname@() const noexcept" + block{
				o + "return @rtype@_@id_type@(m_container, m_container.@obj@_get_@propname@( @internal_tag@ ));";
			};
			if(!is_const) {
				o + "void @namesp@set_@exname@(@ptype@ v) const noexcept" + block{
					o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
				};
			}
		} else if(prop.from_property) {
			o + substitute{ "propname", prop.from_property->name };
			o + substitute{ "ptype", prop.from_property->data_type };
			o + substitute{ "index_type", prop.from_property->array_index_type };
			
			switch(prop.from_property->type) {
				case property_type::array_bitfield:
					o + "bool @namesp@get_@exname@(@index_type@ i) const" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@, i );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@index_type@ i, bool v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, i, v );";
						};
					}
					break;
				case property_type::array_other:
					o + "auto @namesp@get_@exname@(@internal_tag@ i) const -> decltype(m_container.@obj@_get_@propname@(@obj@_id(), @index_type@()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@, i );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@internal_tag@ i, @ptype@ const& v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, i, v );";
						};
					}
					break;
				case property_type::array_vectorizable:
					o + "auto @namesp@get_@exname@(@internal_tag@ i) const -> decltype(m_container.@obj@_get_@propname@(@obj@_id(), @index_type@()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@, i );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@internal_tag@ i, @ptype@ v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, i, v );";
						};
					}
					break;
				case property_type::bitfield:
					o + "bool @namesp@get_@exname@() const" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(bool v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
						};
					}
					break;
				case property_type::object:
					o + "auto @namesp@get_@exname@() const -> decltype(m_container.@obj@_get_@propname@(@obj@_id()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@ptype@ const& v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
						};
					}
					break;
				case property_type::other:
					o + "auto @namesp@get_@exname@() const -> decltype(m_container.@obj@_get_@propname@(@obj@_id()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@ptype@ const& v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
						};
					}
					break;
				case property_type::vectorizable:
					o + "auto @namesp@get_@exname@() const -> decltype(m_container.@obj@_get_@propname@(@obj@_id()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@ptype@ v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
						};
					}
					break;
				case property_type::special_vector:
					o + "auto @namesp@get_@exname@() const -> decltype(m_container.@obj@_get_@propname@(@obj@_id()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					break;
			}
		} else {
			o + substitute{ "rtype", prop.derived_from_slot->actual_table->name };
			o + "@rtype@_@id_type@ @namesp@get_@exname@() const noexcept" + block{
				o + "return @rtype@_@id_type@(m_container, @internal_tag@ );";
			};
		}
	}
	for(auto& prop : pdef.exposed_min_max_terms) {
		o + substitute{ "exname", prop.exposed_name };
		o + substitute{ "internal_tag", "m_" + prop.exposed_name };
		o + substitute{ "obj", prop.derived_from_slot->actual_table->name };

		if(prop.from_link) {
			o + substitute{ "rtype", prop.from_link->type_name };
			o + substitute{ "ptype", prop.from_link->type_name + "_id" };
			o + substitute{ "propname", prop.from_link->property_name };

			o + "@rtype@_@id_type@ @namesp@get_@exname@() const noexcept" + block{
				o + "return @rtype@_@id_type@(m_container, m_container.@obj@_get_@propname@( @internal_tag@ ));";
			};
			if(!is_const) {
				o + "void @namesp@set_@exname@(@ptype@ v) const noexcept" + block{
					o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
				};
			}
		} else if(prop.from_property) {
			o + substitute{ "propname", prop.from_property->name };
			o + substitute{ "ptype", prop.from_property->data_type };
			o + substitute{ "index_type", prop.from_property->array_index_type };

			switch(prop.from_property->type) {
				case property_type::array_bitfield:
					o + "bool @namesp@get_@exname@(@index_type@ i) const" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@, i );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@index_type@ i, bool v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, i, v );";
						};
					}
					break;
				case property_type::array_other:
					o + "auto @namesp@get_@exname@(@internal_tag@ i) const -> decltype(m_container.@obj@_get_@propname@(@obj@_id(), @index_type@()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@, i );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@internal_tag@ i, @ptype@ const& v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, i, v );";
						};
					}
					break;
				case property_type::array_vectorizable:
					o + "auto @namesp@get_@exname@(@internal_tag@ i) const -> decltype(m_container.@obj@_get_@propname@(@obj@_id(), @index_type@()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@, i );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@internal_tag@ i, @ptype@ v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, i, v );";
						};
					}
					break;
				case property_type::bitfield:
					o + "bool @namesp@get_@exname@() const" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(bool v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
						};
					}
					break;
				case property_type::object:
					o + "auto @namesp@get_@exname@() const -> decltype(m_container.@obj@_get_@propname@(@obj@_id()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@ptype@ const& v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
						};
					}
					break;
				case property_type::other:
					o + "auto @namesp@get_@exname@() const -> decltype(m_container.@obj@_get_@propname@(@obj@_id()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@ptype@ const& v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
						};
					}
					break;
				case property_type::vectorizable:
					o + "auto @namesp@get_@exname@() const -> decltype(m_container.@obj@_get_@propname@(@obj@_id()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					if(!is_const) {
						o + "void @namesp@set_@exname@(@ptype@ v) const" + block{
							o + "m_container.@obj@_set_@propname@( @internal_tag@, v );";
						};
					}
					break;
				case property_type::special_vector:
					o + "auto @namesp@get_@exname@() const -> decltype(m_container.@obj@_get_@propname@(@obj@_id()))" + block{
						o + "return m_container.@obj@_get_@propname@( @internal_tag@ );";
					};
					break;
			}
		} else {
			o + substitute{ "rtype", prop.derived_from_slot->actual_table->name };
			o + "@rtype@_@id_type@ @namesp@get_@exname@() const noexcept" + block{
				o + "return @rtype@_@id_type@(m_container, @internal_tag@ );";
			};
		}
	}
	for(auto& prop : pdef.exposed_aggregates) {
		o + substitute{ "exname", prop.exposed_name };
		if(prop.aggregate_name == "count") {
			o + "int32_t @namesp@get_@exname@() const noexcept" + block{
				o + "return m_@exname@;";
			};
		} else if(prop.from_link) {
			o + substitute{ "rtype", prop.from_link->type_name};
			o + "@rtype@_@id_type@ @namesp@get_@exname@() const noexcept" + block{
				o + "return m_@exname@;";
			};
		} else if(prop.from_property) {
			o + substitute{ "obj", prop.derived_from_slot->actual_table->name };
			o + substitute{ "propname", prop.from_property->name };
			o + substitute{ "ptype", prop.from_property->data_type };

			switch(prop.from_property->type) {
				case property_type::bitfield:
					o + "bool @namesp@get_@exname@() const noexcept" + block{
						o + "return m_@exname@;";
					};
					break;
				case property_type::object:
					o + "@ptype@ const& @namesp@get_@exname@() const noexcept" + block{
						o + "return m_@exname@;";
					};
					break;
				case property_type::other:
					o + "@ptype@ const& @namesp@get_@exname@() const noexcept" + block{
						o + "return m_@exname@;";
					};
					break;
				case property_type::vectorizable:
					o + "@ptype@ @namesp@get_@exname@() const noexcept" + block{
						o + "return m_@exname@;";
					};
					break;
				default:
					break;
			}
		} else {
			o + substitute{ "rtype", prop.derived_from_slot->actual_table->name };
			o + "@rtype@_@id_type@ @namesp@get_@exname@() const noexcept" + block{
				o + "return m_@exname@;";
			};
		}
	}
	for(auto& table : pdef.table_slots) {
		if(table.expose_has_name) {
			o + substitute{ "tname", table.reference_name };
			o + substitute{ "int", table.internally_named_as };
			o + "bool @namesp@has_@tname@() const noexcept" + block{
				o + "return bool(@int@);";
			};
		}
	}

	o + line_break{};
	return o;
}
