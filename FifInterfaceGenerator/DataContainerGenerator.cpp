//
// This file provided as part of the DataContainer project
//

#include <string>
#include <vector>
#include <fstream>
#include <filesystem> 
#include <iostream> 
#include <algorithm>
#include <cstring>
#include <set>

#include "parsing.hpp"
#include "code_fragments.hpp"



void error_to_file(std::string const& file_name) {
	std::fstream fileout;
	fileout.open(file_name, std::ios::out);
	if(fileout.is_open()) {
		fileout << "";
		fileout.close();
	}
}

relationship_object_def const* better_primary_key(relationship_object_def const* oldr, relationship_object_def const* newr) {
	if(oldr == nullptr) {
		return newr;
	}

	if(oldr->is_expandable && !newr->is_expandable)
		return newr;
	if(!oldr->is_expandable && newr->is_expandable)
		return oldr;

	switch(oldr->store_type) {
	case storage_type::contiguous:
	{
		switch(newr->store_type) {
		case storage_type::contiguous:
			return oldr->size <= newr->size ? oldr : newr;
		case storage_type::compactable:
		case storage_type::erasable:
			return oldr;
		}
		break;
	}
	case storage_type::erasable:
	{
		switch(newr->store_type) {
		case storage_type::contiguous:
			return newr;
		case storage_type::erasable:
			return oldr->size <= newr->size ? oldr : newr;
		case storage_type::compactable:
			return oldr;
		}
		break;
	}
	case storage_type::compactable:
	{
		switch(newr->store_type) {
		case storage_type::contiguous:
		case storage_type::erasable:
			return newr;
		case storage_type::compactable:
			return oldr->size <= newr->size ? oldr : newr;
		}
		break;
	}
	}

	return oldr;
}

int main(int argc, char* argv[]) {
	if(argc > 1) {
		std::fstream input_file;
		std::string input_file_name(argv[1]);
		input_file.open(argv[1], std::ios::in);

		const std::string output_file_name = [otemp = std::string(argv[1])]() mutable {
			if(otemp.length() >= 4 && otemp[otemp.length() - 4] == '.') {
				otemp[otemp.length() - 3] = 'h';
				otemp[otemp.length() - 2] = 'p';
				otemp[otemp.length() - 1] = 'p';
				auto sep_pos = otemp.find_last_of('\\');
				if(sep_pos == std::string::npos)
					sep_pos = otemp.find_last_of('/');
				if(sep_pos == std::string::npos) {
					return std::string("fif_") + otemp;
				} else {
					return otemp.substr(0, sep_pos + 1) + "fif_" + otemp.substr(sep_pos + 1);
				}
			}
			auto sep_pos = otemp.find_last_of('\\');
			if(sep_pos == std::string::npos)
				sep_pos = otemp.find_last_of('/');
			if(sep_pos == std::string::npos) {
				return std::string("fif_") + otemp + ".hpp";
			} else {
				return otemp.substr(0, sep_pos + 1) + "fif_" + otemp.substr(sep_pos + 1) + +".hpp";
			}
			}();

		error_record err(input_file_name);

		if(!input_file.is_open()) {
			err.add(row_col_pair{ 0,0 }, 1000, "Could not open input file");
			error_to_file(output_file_name);
			std::cout << err.accumulated;
			return -1;
		}

		std::string file_contents((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

		file_def parsed_file = parse_file(file_contents.c_str(), file_contents.c_str() + file_contents.length(), err);

		input_file.close();

		if(err.accumulated.length() > 0) {
			error_to_file(output_file_name);
			std::cout << err.accumulated;
			return -1;
		}

		// patchup relationship pointers & other information

		for(auto& r : parsed_file.relationship_objects) {
			if(r.is_relationship) {
				for(auto& l : r.indexed_objects) {
					if(auto linked_object = find_by_name(parsed_file, l.type_name); linked_object) {
						l.related_to = linked_object;
					} else {
						err.add(row_col_pair{ 0,0 }, 1001, std::string("Could not find object named: ") + l.type_name + " in relationship: " + r.name);
						error_to_file(output_file_name);
						std::cout << err.accumulated;
						return -1;
					}
					if(l.index == index_type::at_most_one && !l.is_optional && l.multiplicity == 1) {
						r.primary_key.points_to = better_primary_key(r.primary_key.points_to, l.related_to);
						if(r.primary_key.points_to == l.related_to)
							r.primary_key.property_name = l.property_name;
					}

					if(l.multiplicity > 1 && l.index == index_type::many && l.ltype == list_type::list) {
						err.add(row_col_pair{ 0,0 }, 1002, std::string("Unsupported combination of list type storage with multiplicity > 1 in link ")
							+ l.property_name + " in relationship: " + r.name);
						error_to_file(output_file_name);
						std::cout << err.accumulated;
						return -1;
					}

					if(l.multiplicity > 1 && l.index == index_type::at_most_one) {
						l.is_distinct = true;
					}
				}



				if(r.indexed_objects.size() == 0) {
					err.add(row_col_pair{ 0,0 }, 1003, std::string("Relationship: ") + r.name + " is between too few objects");
					error_to_file(output_file_name);
					std::cout << err.accumulated;
					return -1;
				}


				if(r.force_pk.length() > 0) {
					bool pk_forced = false;
					for(auto& link : r.indexed_objects) {
						if(link.property_name == r.force_pk && link.index == index_type::at_most_one
							&& !link.is_optional && link.multiplicity == 1) {
							r.primary_key.points_to = link.related_to;
							r.primary_key.property_name = link.property_name;
							pk_forced = true;
						}
					}
					if(!pk_forced) {
						err.add(row_col_pair{ 0,0 }, 1004, std::string("Was unable to use ") + r.force_pk + std::string(" as a primary key for relationship: ") + r.name);
						error_to_file(output_file_name);
						std::cout << err.accumulated;
						return -1;
					}
				}

				for(auto& link : r.indexed_objects) {
					if(link.index != index_type::none)
						link.related_to->relationships_involved_in.push_back(in_relation_information{ r.name, &link, &r });
				}

				if(r.primary_key.points_to) {
					r.size = r.primary_key.points_to->size;
					r.store_type = storage_type::contiguous;
					r.is_expandable = r.primary_key.points_to->is_expandable;

					for(auto& l : r.indexed_objects) {
						if(r.primary_key == l) {
							l.is_primary_key = true;
						}
					}
				} else {
					if(r.store_type != storage_type::erasable && r.store_type != storage_type::compactable) {
						err.add(row_col_pair{ 0,0 }, 1005, std::string("Relationship ") + r.name +
							" has no primary key, and thus must have either a compactable or erasable storage type to provide a delete function.");
						error_to_file(output_file_name);
						std::cout << err.accumulated;
						return -1;
					}
				}

			} // end if is a relationship
		} // patchup relationship pointers loop

		// identify vectorizable types
		for(auto& ob : parsed_file.relationship_objects) {
			for(auto& prop : ob.properties) {
				if(prop.type == property_type::other && is_vectorizable_type(parsed_file, prop.data_type))
					prop.type = property_type::vectorizable;
				if(prop.type == property_type::array_other && is_vectorizable_type(parsed_file, prop.data_type))
					prop.type = property_type::array_vectorizable;
			}
		}


		// patch up composite key info
		bool needs_hash_include = false;
		std::vector<int32_t> byte_sizes_need_hash;

		for(auto& ob : parsed_file.relationship_objects) {
			for(auto& cc : ob.composite_indexes) {
				needs_hash_include = true;

				int32_t bits_so_far = 0;
				for(auto& k : cc.component_indexes) {

					for(auto& ri : ob.indexed_objects) {
						if(ri.property_name == k.property_name) {
							k.object_type = ri.type_name;
							ri.is_covered_by_composite_key = true;
							k.multiplicity = ri.multiplicity;
						}
					}

					if(k.object_type.length() == 0) {
						err.add(row_col_pair{ 0,0 }, 1006, std::string("Indexed link ") + k.property_name + " in composite key " + cc.name +
							" in relationship " + ob.name + " does not refer to a link in the relationship.");
						error_to_file(output_file_name);
						std::cout << err.accumulated;
						return -1;
					}

					k.bit_position = bits_so_far;
					if(k.property_name == ob.primary_key.property_name)
						cc.involves_primary_key = true;

					if(ob.is_expandable) {
						k.number_of_bits = 32;
						bits_so_far += 32;
					} else {
						k.number_of_bits = 0;
						for(auto sz = ob.size; sz != 0; sz = sz >> 1) {
							++k.number_of_bits;
							bits_so_far += k.multiplicity;
						}
					}
				}

				cc.total_bytes = (bits_so_far + 7) / 8;
				if(cc.total_bytes > 8 &&
					std::find(byte_sizes_need_hash.begin(), byte_sizes_need_hash.end(), cc.total_bytes) == byte_sizes_need_hash.end()) {
					byte_sizes_need_hash.push_back(cc.total_bytes);
				}
			}
		}

		if(err.accumulated.length() > 0) {
			error_to_file(output_file_name);
			std::cout << err.accumulated;
			return -1;
		}

		// compose contents of generated file
		std::string output;

		basic_builder o;
		//includes
		output += "#pragma once\n";
		output += "\n";
		output += "//\n";
		output += "// This file was automatically generated from: " + std::string(argv[1]) + "\n";
		output += "// EDIT AT YOUR OWN RISK; all changes will be lost upon regeneration\n";
		output += "// NOT SUITABLE FOR USE IN CRITICAL SOFTWARE WHERE LIVES OR LIVELIHOODS DEPEND ON THE CORRECT OPERATION\n";
		output += "//\n";
		output += "\n";
		output += "#include <cstdint>\n";
		output += "#include <cstddef>\n";
		output += "#include <utility>\n";
		output += "#include <vector>\n";
		output += "#include <algorithm>\n";
		output += "#include <assert.h>\n";
		output += "#include <cstring>\n";
		output += "#include <string>\n";

		if(needs_hash_include) {
			output += "#include \"unordered_dense.h\"\n";
		}

		//open new namespace
		output += "\n";

		output += "\n";
		output += "namespace fif {\n";

		//
		// TODO make content string
		//

		output += "std::string container_interface() {\n";
		output += "return std::string("") + \"\" \n";

		output += "\" ptr(nil) global data-container \"\n";
		output += "\" : set-container data-container ! ; \"\n";
		output += "\" ptr(nil) global vector-storage \"\n";
		output += "\" : set-vector-storage vector-storage ! ; \"\n";
		output += "\" :export set_container ptr(nil) set-container ;  \"\n";
		output += "\" :export set_vector_storage ptr(nil) set-vector-storage ;  \"\n";
		output += "\" :struct bit-proxy i32 bit ptr(i8) byte ; \"\n";
		output += "\" :struct index-view ptr($0) wrapped ; \"\n";
		output += "\" :s @ index-view($0) s: .wrapped @ ; \"\n";
		output += "\" :s make-index-view ptr($0) s: make index-view($0) .wrapped! ; \"\n";
		output += "\" :s ! bool bit-proxy s: .byte@ let byte .bit let bit let arg 1 bit shl not byte @ >i32 and arg >i32 bit shl or >i8 byte ! ; \"\n";
		output += "\" :s @ bit-proxy s: .byte@ let byte .bit let bit byte @ >i32 bit shr 1 and >bool ; \"\n";
		output += "\" :s >index i32 s:  ; \"\n"; // nop
		output += "\" :s >index ui32 s: >i32 ; \"\n";
		output += "\" :s >index i16 s: >i32 ; \"\n";
		output += "\" :s >index ui16 s: >i32 ; \"\n";
		output += "\" :s >index i8 s: >i32 ; \"\n";
		output += "\" :s >index ui8 s: >i32 ; \"\n";
		output += "\" :s >index i64 s: >i32 ; \"\n";
		output += "\" :s >index ui64 s: >i32 ; \"\n";

		//
		//


		std::set<std::string> made_types;

		//id types definitions
		for(auto& ob : parsed_file.relationship_objects) {
			const auto underlying_type = ob.is_expandable ? std::string("uint32_t") : size_to_tag_type(ob.size);
			//id class begin
			output += make_id_definition(ob.name + "_id", underlying_type);
			made_types.insert(ob.name + "_id");
		}
		for(auto& mi : parsed_file.extra_ids) {
			output += make_id_definition(mi.name, mi.base_type);
			made_types.insert(mi.name);
		}


		//
		// class members == dcon::internal:: .... _class
		//


		//write internal classes

		for(auto& ob : parsed_file.relationship_objects) {
			//predeclare helpers


			//begin members declaration
			auto base_index_type = ob.is_expandable ? std::string("uint32_t") : size_to_tag_type(ob.size);
			auto index_type = type_to_fif_type(base_index_type);
			auto self_index_id = std::string("dcon::") + ob.name + "_id";

			if(ob.store_type == storage_type::erasable) {
				output += "\" :s _index " + ob.name + "_id s: >index \" + std::to_string(sizeof(" + self_index_id + ")) + \" * " + offset_of_member_container(ob.name, "_index") + " + data-container @ buf-add ptr-cast ptr(" + ob.name + "_id) ; \"\n";
				output += "\" :s live? " + ob.name + "_id s: dup _index @ = ; \"\n";
			} else {
				output += "\" :s live? " + ob.name + "_id s: true ; \"\n";
			}

			for(auto& p : ob.properties) {

				std::string property_type = p.data_type;
				bool known = known_as_fif_type(p.data_type);
				auto d_type = type_to_fif_type(p.data_type);

				if(made_types.contains(property_type)) {
					property_type = "dcon::" + property_type;
					known = true;
				}

				if(p.is_derived) {
					// no data for a derived property
				} else if(p.type == property_type::bitfield) {
					output += "\" :s " + p.name + " " + ob.name + "_id s: >index  dup 3 shr " + offset_of_member_container(ob.name, p.name) + " + data-container @ buf-add ptr-cast ptr(i8) make bit-proxy .byte! swap >i32 7 and swap .bit! ; \"\n";
				} else if(p.type == property_type::object) {
					output += "\" :s " + p.name + " " + ob.name + "_id s: >index \" + std::to_string(sizeof(" + property_type + ")) + \" * " + offset_of_member_container(ob.name, p.name) + " + data-container @ buf-add ; \"\n";
				} else if(p.type == property_type::special_vector) {
					//fill in with special vector type and pool object

					output += "\" :struct vpool-" + ob.name + "-" + p.name + " ptr(i32) content ;  \"\n";
					output += "\" :s " + p.name + " " + ob.name + "_id s: >index 4 * " + offset_of_member_container(ob.name, p.name) + " + data-container @ buf-add ptr-cast ptr(i32) make vpool-" + ob.name + "-" + p.name + " .content! ; \"\n";
					output += "\" :s size vpool-" + ob.name + "-" + p.name + " s: .content @ dup 1 + >i32 0 = if drop 0 else 8 * 4 + vector-storage @ buf-add ptr-cast ptr(ui16) @ >i32 then ;  \"\n";
					output += "\" :s index vpool-" + ob.name + "-" + p.name + " i32 s: let idx .content @ 8 * 8 + \" + std::to_string(sizeof(" + property_type + ")) + \" idx * + vector-storage @ buf-add ptr-cast ptr(" + (known ? d_type : std::string("nil")) + ") ;  \"\n";

					// TODO: write vpool functions

					//output += "\t\t\tdcon::stable_variable_vector_storage_mk_2<" + p.data_type + ", 16, " + std::to_string(p.special_pool_size) + " > " + p.name + "_storage;\n";
				} else if(p.type == property_type::array_bitfield) {
					auto index_typeb = type_to_fif_type(p.array_index_type);

					output += "\" :s " + p.name + " " + ob.name + "_id " + index_typeb + " s: >index swap >index swap " + offset_of_array_member_container(ob.name, p.name) + " data-container @ buf-add ptr-cast ptr(ptr(nil)) @ swap 1 + " + array_member_row_size(property_type, ob.size, true) + " * " + array_member_leading_padding(property_type, ob.size, true) + " + swap buf-add swap dup let tidx 3 shr swap buf-add ptr-cast ptr(i8) make bit-proxy .byte! tidx >i32 7 and swap .bit! ; \"\n";
				} else if(p.type == property_type::array_other || p.type == property_type::array_vectorizable) {
					auto index_typeb = type_to_fif_type(p.array_index_type);

					output += "\" :s " + p.name + " " + ob.name + "_id " + index_typeb + " s: >index swap >index swap " + offset_of_array_member_container(ob.name, p.name) + " data-container @ buf-add ptr-cast ptr(ptr(nil)) @ swap 1 + " + array_member_row_size(property_type, ob.size, false) + " * " + array_member_leading_padding(property_type, ob.size, false) + " + swap buf-add swap \" + std::to_string(sizeof(" + property_type + ")) + \" * swap buf-add ptr-cast ptr(" + (known ? d_type : std::string("nil")) + ") ; \"\n";
				} else {
					output += "\" :s " + p.name + " " + ob.name + "_id s: >index \" + std::to_string(sizeof(" + property_type + ")) + \" * " + offset_of_member_container(ob.name, p.name) + " + data-container @ buf-add ptr-cast ptr(" + (known ? d_type : std::string("nil")) + ") ; \"\n";
				}
			} //end non relationship members

			// begin relationship members
			for(auto& i : ob.indexed_objects) {
				if(ob.primary_key == i) {
					output += "\" :s " + i.property_name + " " + ob.name + "_id s: >index >" + i.type_name + "_id ; \"\n";
				} else {
					auto d_type = type_to_fif_type(i.type_name);
					output += "\" :s " + i.property_name + " " + ob.name + "_id s: >index \" + std::to_string(sizeof(dcon::" + i.type_name + "_id)) + \" * " + (i.multiplicity > 1 ? std::to_string(i.multiplicity) + " * " : std::string("")) + offset_of_member_container(ob.name, i.property_name) + " + data-container @ buf-add ptr-cast ptr(" + i.type_name + "_id) make-index-view ; \"\n";

					//output += make_member_container(o, i.property_name, i.type_name + "_id",
					//	i.multiplicity == 1 ? expand_size_to_fill_cacheline_calculation(i.type_name + "_id", ob.size) : std::to_string(ob.size),
					//	i.multiplicity == 1 ? struct_padding::fixed : struct_padding::none,
					//	ob.is_expandable, std::optional<std::string>(), i.multiplicity).to_string(3);
				}

				if(i.index == index_type::many) {
					if(i.ltype == list_type::array) {
						//array of relation ids in object
						if(!i.related_to->is_expandable) {
							output += "\" :struct vpool-" + ob.name + "-" + i.property_name + " ptr(i32) content ;  \"\n";
							output += "\" :s " + ob.name + "-" + i.property_name + " " + i.type_name + "_id s: >index 4 * " + offset_of_member_container(ob.name, std::string("array_") + i.property_name) + " + data-container @ buf-add ptr-cast ptr(i32) make vpool-" + ob.name + "-" + i.property_name + " .content! ; \"\n";

							output += "\" :s size vpool-" + ob.name + "-" + i.property_name + " s: .content @ dup 1 + >i32 0 = if drop 0 else 8 * 4 + vector-storage @ buf-add ptr-cast ptr(ui16) @ >i32 then ;  \"\n";
							output += "\" :s index vpool-" + ob.name + "-" + i.property_name + " i32 s: let idx .content @ 8 * 8 + \" + std::to_string(sizeof(dcon::" + ob.name + "_id)) + \" idx * + vector-storage @ buf-add ptr-cast ptr(" + ob.name + "_id) ;  \"\n";

							//output += "\t\t\tdcon::stable_variable_vector_storage_mk_2<" + ob.name + "_id, 4, " + std::to_string(ob.size * 8) + " > "
							//	+ i.property_name + "_storage;\n";
						}

					}
				} else if(i.index == index_type::at_most_one) {
					if(ob.primary_key == i) {
						output += "\" :s " + ob.name + "-" + i.property_name + " " + i.type_name + "_id s: >index >" + ob.name + "_id ; \"\n";
					} else {
						auto d_type = i.type_name + "_id";
						output += "\" :s " + ob.name + "-" + i.property_name + " " + i.type_name + "_id s: >index \" + std::to_string(sizeof(dcon::" + ob.name + "_id)) + \" * " + offset_of_member_container(ob.name, std::string("link_back_") + i.property_name) + " + data-container @ buf-add ptr-cast ptr(" + ob.name + "_id) make-index-view ; \"\n";
					}
				}
			} // end relationship members


			if(!ob.primary_key.points_to) {
				std::string class_name = "dcon::internal::" + ob.name + "_class";
				std::string offset = "offsetof(dcon::data_container, " + ob.name + ") "
					"+ offsetof(" + class_name + ",  size_used)";
				output += "\" : " + ob.name + "-size \" + std::to_string(" + offset + ") + \" data-container @ buf-add ptr-cast ptr(i32) @ ; \"\n";
			}
			/*
			// make composite key functions and hashmaps
			for(auto& cc : ob.composite_indexes) {
				output += make_composite_key_declarations(o, ob.name, cc).to_string(3);
			}
			*/
		}

		//
		// TO ADD:
		// iteration functions
		// create / destroy functions
		// relationship setters


		// creation / deletion routines
		for(auto& cob : parsed_file.relationship_objects) {
			const std::string id_name = cob.name + "_id";

			if(!cob.is_relationship) {
				if(cob.store_type == storage_type::contiguous || cob.store_type == storage_type::compactable) {
					//output += make_pop_back(o, cob).to_string(2);
					//output += make_object_resize(o, cob).to_string(2);
					//output += make_non_erasable_create(o, cob).to_string(2);

					if(cob.store_type == storage_type::compactable) {
						//output += make_compactable_delete(o, cob).to_string(2);
					}

				} else if(cob.store_type == storage_type::erasable) {
					//output += make_erasable_delete(o, cob).to_string(2);
					//output += make_erasable_create(o, cob).to_string(2);
					//output += make_object_resize(o, cob).to_string(2);
				}
			} else if(cob.primary_key.points_to) { // primary key relationship
				//output += make_object_resize(o, cob).to_string(2);
				//output += make_clearing_delete(o, cob).to_string(2);
				//output += make_pop_back(o, cob).to_string(2);

				// += make_relation_try_create(o, cob).to_string(2);
				//output += make_relation_force_create(o, cob).to_string(2);

			} else { // non pk relationship
				if(cob.store_type == storage_type::contiguous || cob.store_type == storage_type::compactable) {
					// output += make_pop_back(o, cob).to_string(2);
					// output += make_object_resize(o, cob).to_string(2);
					if(cob.store_type == storage_type::compactable) {
						//output += make_compactable_delete(o, cob).to_string(2);
					}

					//output += make_relation_try_create(o, cob).to_string(2);
					//output += make_relation_force_create(o, cob).to_string(2);

				} else if(cob.store_type == storage_type::erasable) {
					//output += make_erasable_delete(o, cob).to_string(2);
					//output += make_object_resize(o, cob).to_string(2);

					//output += make_relation_try_create(o, cob).to_string(2);
					//output += make_relation_force_create(o, cob).to_string(2);

				}
			} // end case relationship no primary key
		} // end creation / deletion reoutines creation loop

		//iterate over all routines
		//for(auto& cob : parsed_file.relationship_objects) {
			//output += make_iterate_over_objects(o, cob).to_string(2);
		//}


		for(auto& ob : parsed_file.relationship_objects) {
			//output += object_iterator_implementation(o, ob).to_string(2);

			for(auto& ir : ob.relationships_involved_in) {
				if(ir.linked_as->index == index_type::many) {
					//output += relation_iterator_foreach_as_implementation(o, ob, *ir.rel_ptr, *ir.linked_as).to_string(2);
				}
			}
		}

		output += ";\n"; //end line
		output += "} \n"; // end function

		//close new namespace
		output += "}\n";

		output += "\n";

		//newline at end of file
		output += "\n";

		std::fstream fileout;
		fileout.open(output_file_name, std::ios::out);
		if(fileout.is_open()) {
			fileout << output;
			fileout.close();
		} else {
			std::abort();
		}
	}
}
