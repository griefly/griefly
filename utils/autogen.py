import json
from typing import Union, List, Dict


def import_metadata(filename: str) -> Dict:
    with open(filename, "r") as json_file:
        raw_json = json_file.read()

    metadata = json.JSONDecoder().decode(raw_json)
    return metadata


def get_class_data(metadata: dict, class_name: str) -> Union[Dict, None]:
    for class_data in metadata["classes"]:
        if class_data["class"] == class_name:
            return class_data
    return None


def get_header_list(metadata: dict) -> List:
    header_list = []
    for class_data in metadata["classes"]:
        header = class_data["header"]
        if header not in header_list:
            header_list.append(header)
    return header_list


def write_to_file(content: str, file: str) -> None:
    with open(file, "w") as output_file:
        output_file.write(content)


def generate_autogen_metadata() -> str:
    file_content = "#include \"AutogenMetadata.h\"\n"
    file_content += "#include \"core/Idptr.h\"\n\n"

    for header in header_list:
        file_content += "#include \"{}\"\n".format(header)

    file_content += """
using namespace kv;

std::unordered_map<QString, SettersForType>& GetSettersForTypes()
{
    static std::unordered_map<QString, SettersForType> map;
    return map;
}

int Object::TYPE_INDEX;
const QString& Object::GetTypeStatic()
{
    static const QString type = "main";
    return type;
}
"""

    for class_data in metadata["classes"]:
        file_content += """
int {data_class}::TYPE_INDEX;
const QString& {data_class}::GetTypeStatic()
{{
    static const QString type = "{data_class}";
    return type;
}}\n""".format(data_class=class_data["class"])

    file_content += """
void InitRealTypes()
{
    Object::TYPE_INDEX = 0;
"""

    for class_data in metadata["classes"]:
        index = str(metadata["classes"].index(class_data) + 1)
        file_content += "\t{class_data}::TYPE_INDEX = {index};\n".format(class_data=class_data["class"], index=index)

    file_content += """}

void InitCreators()
{
    (*GetItemsCreators())[Object::GetTypeStatic()] = &Object::_Z_KV_Creator;
    (*GetVoidItemsCreators())[Object::GetTypeStatic()] = &Object::_Z_KV_VoidCreator;"""

    for class_data in metadata["classes"]:
        class_name = class_data["class"]
        file_content += """
    (*GetItemsCreators())[{class_name}::GetTypeStatic()] = &{class_name}::_Z_KV_Creator;
    (*GetVoidItemsCreators())[{class_name}::GetTypeStatic()] = &{class_name}::_Z_KV_VoidCreator;""".\
            format(class_name=class_name)

    file_content += """
}

void InitSettersForTypes()
{
"""
    for class_data in metadata["classes"]:
        class_data_loc = class_data
        while class_data_loc:
            for variable in class_data_loc["variables"]:
                file_content += "\tGetSettersForTypes()[{}::GetTypeStatic()][\"{}\"] = &{}::_Z_KV_SETTERS{};\n" \
                    .format(class_data["class"], variable, class_data_loc["class"], variable)
            class_data_loc = get_class_data(metadata, class_data_loc["base_class"])
    file_content += "}\n"

    return file_content


def generate_autogen_serializer() -> str:
    file_content = ""
    for header in header_list:
        file_content += '#include \"{}\"\n'.format(header)

    file_content += "\nusing namespace kv;\n\n"

    for class_data in metadata["classes"]:
        file_content += """
void {}::Save(FastSerializer& file)
{{
    {}::Save(file);
    """.format(class_data["class"], class_data["base_class"])

        if len(class_data["variables"]):
            file_content += "\n"

        for variable in class_data["variables"]:
            file_content += "    file << {};\n".format(variable)

        file_content += """}}

void {}::Load(FastDeserializer& file)
{{
    {}::Load(file);
    """.format(class_data["class"], class_data["base_class"])

        if len(class_data["variables"]):
            file_content += "\n"

        for variable in class_data["variables"]:
            file_content += "    file >> {};\n".format(variable)

        if len(class_data["on_load_calls"]):
            file_content += "\n"

        for function in class_data["on_load_calls"]:
            file_content += "    {}();\n".format(function)
        file_content += """}}

unsigned int {}::HashMembers() 
{{
    unsigned int retval = 0;
    retval += {}::HashMembers();\n""".format(class_data["class"], class_data["base_class"])

        for variable in class_data["variables"]:
            file_content += "\tretval += Hash({});\n".format(variable)
        file_content += "\treturn retval;\n}\n"
    return file_content


print('Autogen started')

metadata_file = "metadata.json"
autogen_metadata_file = "AutogenMetadata.cpp"
autogen_serialization_file = "AutogenSerialization.cpp"

metadata = import_metadata(metadata_file)
header_list = get_header_list(metadata)

write_to_file(generate_autogen_metadata(), autogen_metadata_file)
print('{} file has been generated'.format(autogen_metadata_file))

write_to_file(generate_autogen_serializer(), autogen_serialization_file)
print('{} file has been generated'.format(autogen_serialization_file))

print('Autogen completed')
