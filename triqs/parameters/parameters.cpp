#include <triqs/utility/first_include.hpp>
#include <boost/serialization/string.hpp>
#include "./parameters.hpp"
#include <triqs/utility/serialization.hpp>
#include <algorithm>
#include <boost/algorithm/string.hpp>

namespace triqs {
namespace params {

 parameters::parameters() {
  using triqs::arrays::array;
  using dcomplex = std::complex<double>;
  if (_field::type_names.size() == 0) {
#define R(...) _field::type_names.insert({typeid(__VA_ARGS__), get_triqs_hdf5_data_scheme(storage_t<__VA_ARGS__>{})});
#define A(N)                                                                                                                     \
 R(array<int, N>);                                                                                                               \
 R(array<long, N>);                                                                                                              \
 R(array<double, N>);                                                                                                            \
 R(array<dcomplex, N>);
   R(long);
   R(double);
   R(int);
   R(dcomplex);
   A(1);
   A(2);
   A(3);
   A(4);
  }
 }
#undef A
#undef R

 parameters::_data_t::iterator parameters::find(std::string const& key) {
  return std::find_if(_data.begin(), _data.end(), [&key](_data_elem const& x) { return x.key == key; });
 }

 parameters::_data_t::const_iterator parameters::find(std::string const& key) const {
  return std::find_if(_data.begin(), _data.end(), [&key](_data_elem const& x) { return x.key == key; });
 }

 bool parameters::has_key(std::string const& k) const { return find(k) != _data.end();}

 void parameters::sort_by_key() {
  std::sort(_data.begin(), _data.end(), [](_data_elem const& x, _data_elem const& y) { return x.key < y.key; });
 }

 void parameters::insert(std::string const& key, _field&& f, std::string const& doc) {
  if (has_key(key)) TRIQS_RUNTIME_ERROR << "Field " << key << " already defined";
  _data.push_back({key, std::move(f),doc});
 }

 _field& parameters::operator[](const char * key) {
  auto it = find(key);
  if (it == _data.end()) TRIQS_RUNTIME_ERROR << "Parameters : the key : " << key << " does not exists";
  return it->f;
 }

 _field const& parameters::operator[](const char * key) const {
  auto it = find(key);
  if (it == _data.end()) TRIQS_RUNTIME_ERROR << "Parameters : the key : " << key << " does not exists";
  return it->f;
 }

 void h5_write(h5::group F, std::string const& subgroup_name, parameters const& p) {
  auto gr = F.create_group(subgroup_name);
  for (auto& pvp : p._data) h5_write(gr, pvp.key, pvp.f);
 }

 void h5_read(h5::group F, std::string const& subgroup_name, parameters& p) {
  auto gr = F.open_group(subgroup_name);
  for (auto& pvp : p._data) h5_read(gr, pvp.key, pvp.f);
 }

 //-----------------------------------------------------------------------

 void parameters::update(parameters const& other) {
  for (auto const& kfd : other._data) {
   auto it = find(kfd.key);
   if (it !=_data.end()) { // already there
    if (it->f.index != kfd.f.index) TRIQS_RUNTIME_ERROR << "Index mismatch in merging parameters";
    if (kfd.doc.size() > 0) it->doc = kfd.doc; // update doc iif there is a doc ...
   } else
   _data.push_back(kfd);
  }
 }

  //-----------------------------------------------------------------------

 std::vector<std::vector<std::string>> parameters::generate_help(bool with_header) const {
  std::vector<std::vector<std::string>> str;
  if (with_header) str.push_back({"parameter:", "type:", "value:", "description:"});
  for (auto const& s : _data)
   if (s.f.is_modification_required()) str.push_back({s.key, s.f.type_name(), "-", s.doc});

  for (auto const& s : _data) {
   if (is_parameter(s.f)) continue; // no subgroup
   if (!s.f.is_modification_required()) {
    std::ostringstream val;
    val << s.f;
    auto sv = val.str();
    boost::algorithm::trim(sv);
    int size_max = 30;
    std::replace( sv.begin(), sv.end(), '\n', ','); // replace all '\n' with ' '
    if (sv.size()>size_max) {
     auto s2 = std::string(size_max+5,'.');
     std::copy(sv.begin(), sv.begin()+size_max/2, s2.begin());
     std::copy(sv.end()-size_max/2, sv.end(), s2.end()-size_max/2);
     sv = s2;
    }
    str.push_back({s.key, s.f.type_name(), sv, s.doc});
   }
  }
  // all sub groups after
  for (auto const& s : _data) {
   if (!is_parameter(s.f)) continue;
   auto* p = dynamic_cast<_field::_data_impl<parameters>*>(s.f.p.get());
   auto str2 = p->x.generate_help(false);
   for (auto & x : str2) x[0] = "  " + x[0];
   str2.insert(str2.begin(), {"Sub-group : ", s.key, " ", ""});
   str.insert(str.end(), str2.begin(), str2.end());
  }
  return str;
 }
 //-----------------------------------------------------------------------

 //
 std::string print_formatted(std::vector<std::vector<std::string>> const &out){
   std::vector<std::size_t> max_len;
   std::ostringstream str;

   for(auto const &s : out){
       max_len.resize(out[0].size(),0);
       for(std::size_t i=0; i<s.size(); ++i)
         if(s[i].length()>max_len[i]) max_len[i]=s[i].length();
   }
   for(auto const &s : out){
     for(std::size_t i=0; i<s.size(); ++i)
       str << std::left << std::setw(max_len[i]+2) << s[i];
     str << std::endl;
   }

   return str.str();
 }

 //-----------------------------------------------------------------------

 /// help as a string
  std::string parameters::help() const {
   return print_formatted(generate_help());
  }

 //-----------------------------------------------------------------------

 std::ostream& operator<<(std::ostream& out, parameters const& p) {
  return out << p.help();
 }

 //-----------------------------------------------------------------------

 _field& _field::operator[](const char* key) {
  auto* pp = dynamic_cast<_data_impl<parameters>*>(p.get());
  if (!pp) TRIQS_RUNTIME_ERROR << "Can only use [] on a subgroup, which " << name() << " is not !";
  return pp->x.operator[](key);
 }

 _field const& _field::operator[](const char* key) const {
  auto* pp = dynamic_cast<_data_impl<parameters>*>(p.get());
  if (!pp) TRIQS_RUNTIME_ERROR << "Can only use [] on a subgroup, which " << name() << " is not !";
  return pp->x.operator[](key);
 }

 bool is_parameter(_field const& f) { return dynamic_cast<_field::_data_impl<parameters>*>(f.p.get()); }

 _field& _field::add_group(std::string const& key, std::string const& doc) {
  auto* pp = dynamic_cast<_data_impl<parameters>*>(p.get());
  if (!pp) TRIQS_RUNTIME_ERROR << "Can only use [] on a subgroup, which " << name() << " is not !";
  pp->x.add_group(key, doc);
  return *this;
 }
}
}

