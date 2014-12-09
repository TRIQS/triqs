// DO NOT EDIT
// Generated automatically using libclang using the command : 
// ${shell_command}
%for c in classes :
<%
  def name_lmax(member_list) : 
   return max(len(m.name) for m in member_list)

  def name_format(name) : 
    f =  '{:<%s}'%(name_lmax(c.members)+2)
    return f.format(name)

  def name_format_q(name) : return name_format('"%s"'%name) 
 
  def type_format(name) : 
    f  = '{:<%s}'%(max(len(m.ctype) for m in c.members))
    return f.format(name)
%>
${''.join('namespace %s {'%n for n in c.ns)}

/// Write into HDF5
void h5_write(h5::group fg, std::string subgroup_name, ${c.name} const &x) {
 auto gr = fg.create_group(subgroup_name);
 gr.write_triqs_hdf5_data_scheme("TRIQS_PARAM:${c.name}");
 %for m in c.members:
 h5_write(gr, ${name_format_q(m.name)}, x.${m.name});
 %endfor
}

/// Read from HDF5
void h5_read(h5::group fg, std::string subgroup_name, ${c.name} &x) {
 auto gr = fg.open_group(subgroup_name);
 // Check the attribute or throw
 auto tag_file = gr.read_triqs_hdf5_data_scheme();
 auto tag_expected = "TRIQS_PARAM:${c.name}";
 if (tag_file != tag_expected)
  TRIQS_RUNTIME_ERROR << "h5_read : mismatch of the tag TRIQS_HDF5_data_scheme tag in the h5 group : found " << tag_file
                      << " while I expected " << tag_expected;
 %for m in c.members:
 h5_read(gr, ${name_format_q(m.name)}, x.${m.name});
 %endfor
}

## close namespace
${'}'*len(c.ns)}

%endfor
