#include <pugixml/pugixml.hpp>
using namespace pugi;

BOOL ExpLua_pugixml(lua_State *L)
{
    try{
        lua_tinker::class_add<xml_parse_result>(L,"xml_parse_result");
        lua_tinker::class_def<xml_parse_result>(L,"isOK",&xml_parse_result::isOK);
        
        lua_tinker::class_add<xml_text>(L,"xml_text");
        lua_tinker::class_con<xml_text>(L,lua_tinker::constructor<xml_text>);
        lua_tinker::class_def<xml_text>(L,"get",&xml_text::get);
        lua_tinker::class_def<xml_text>(L,"as_int",&xml_text::as_int);
        lua_tinker::class_def<xml_text>(L,"as_uint",&xml_text::as_uint);
        lua_tinker::class_def<xml_text>(L,"as_double",&xml_text::as_double);
        lua_tinker::class_def<xml_text>(L,"as_float",&xml_text::as_float);
        lua_tinker::class_def<xml_text>(L,"as_bool",&xml_text::as_bool);
        
        lua_tinker::class_add<xml_attribute>(L,"xml_attribute");
        lua_tinker::class_def<xml_attribute>(L,"name",&xml_attribute::name);
        lua_tinker::class_def<xml_attribute>(L,"value",&xml_attribute::value);
        lua_tinker::class_def<xml_attribute>(L,"empty",&xml_attribute::empty);
        lua_tinker::class_def<xml_attribute>(L,"as_int",&xml_attribute::as_int);
        lua_tinker::class_def<xml_attribute>(L,"as_uint",&xml_attribute::as_uint);
        lua_tinker::class_def<xml_attribute>(L,"as_double",&xml_attribute::as_double);
        lua_tinker::class_def<xml_attribute>(L,"as_float",&xml_attribute::as_float);
        lua_tinker::class_def<xml_attribute>(L,"as_bool",&xml_attribute::as_bool);
        lua_tinker::class_def<xml_attribute>(L,"next_attribute",&xml_attribute::next_attribute);
        lua_tinker::class_def<xml_attribute>(L,"previous_attribute",&xml_attribute::previous_attribute);

        lua_tinker::class_add<xml_node>(L,"xml_node");
        lua_tinker::class_con<xml_node>(L,lua_tinker::constructor<xml_node>);
        lua_tinker::class_def<xml_node>(L,"name",&xml_node::name);
        lua_tinker::class_def<xml_node>(L,"set_name",&xml_node::set_name);
        lua_tinker::class_def<xml_node>(L,"value",&xml_node::value);
        lua_tinker::class_def<xml_node>(L,"set_value",&xml_node::set_value);
        lua_tinker::class_def<xml_node>(L,"first_attribute",&xml_node::first_attribute);
        lua_tinker::class_def<xml_node>(L,"last_attribute",&xml_node::last_attribute);
        lua_tinker::class_def<xml_node>(L,"first_child",&xml_node::first_child);
        lua_tinker::class_def<xml_node>(L,"last_child",&xml_node::last_child);
        lua_tinker::class_def<xml_node>(L,"next_sibling",(xml_node (xml_node::*)()const)&xml_node::next_sibling);
        lua_tinker::class_def<xml_node>(L,"previous_sibling",(xml_node (xml_node::*)()const)&xml_node::previous_sibling);
        lua_tinker::class_def<xml_node>(L,"next_siblingByName",(xml_node (xml_node::*)(const wchar_t *,bool)const)&xml_node::next_sibling);
        lua_tinker::class_def<xml_node>(L,"previous_siblingByName",(xml_node (xml_node::*)(const wchar_t *,bool)const)&xml_node::previous_sibling);
        lua_tinker::class_def<xml_node>(L,"parent",&xml_node::parent);
        lua_tinker::class_def<xml_node>(L,"root",&xml_node::root);
        lua_tinker::class_def<xml_node>(L,"text",&xml_node::text);
        lua_tinker::class_def<xml_node>(L,"child",&xml_node::child);
        lua_tinker::class_def<xml_node>(L,"attribute",&xml_node::attribute);
        
        lua_tinker::class_add<xml_document>(L,"xml_document");
        lua_tinker::class_inh<xml_document,xml_node>(L);
        lua_tinker::class_con<xml_document>(L,lua_tinker::constructor<xml_document>);

        lua_tinker::class_def<xml_document>(L,"load",&xml_document::load);
        lua_tinker::class_def<xml_document>(L,"load_buffer",&xml_document::load_buffer);
        lua_tinker::class_def<xml_document>(L,"load_fileA",(xml_parse_result (xml_document::*)(const char *,unsigned int,xml_encoding))&xml_document::load_file);
        lua_tinker::class_def<xml_document>(L,"load_fileW",(xml_parse_result (xml_document::*)(const wchar_t *,unsigned int,xml_encoding))&xml_document::load_file);
        lua_tinker::class_def<xml_document>(L,"reset",(void (xml_document::*)())&xml_document::reset);
        lua_tinker::class_def<xml_document>(L,"reset2",(void (xml_document::*)(const xml_document&))&xml_document::reset);
        lua_tinker::class_def<xml_document>(L,"save_fileA",(bool (xml_document::*)(const char*, const wchar_t*, unsigned int, xml_encoding)const)&xml_document::save_file);
        lua_tinker::class_def<xml_document>(L,"save_fileW",(bool (xml_document::*)(const wchar_t*, const wchar_t*, unsigned int, xml_encoding)const)&xml_document::save_file);
        
        
        return TRUE;
    }catch(...)
    {
        return FALSE;
    }

}