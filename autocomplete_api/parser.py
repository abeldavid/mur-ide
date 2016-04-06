import xml.etree.ElementTree as ET
tree = ET.parse('cpp.xml')
root = tree.getroot()
cpp_node = root[0]
commands = []
for com_node in cpp_node:
    command_string = com_node.attrib["name"]
    if "func" in com_node.attrib and com_node.attrib["func"] == "yes":
        command_string += "("
        params = []
        for param_node in com_node[0]:
            params.append(param_node.attrib["name"])
        command_string += ", ".join(params)
        command_string += ") -> " + com_node[0].attrib["retVal"]    
    commands.append(command_string)
f = open('c++.api', 'w')
f.write("\n".join(commands))
f.close()

